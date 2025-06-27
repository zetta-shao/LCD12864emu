//    FILE: INA219.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.4.1
//    DATE: 2021-05-18
// PURPOSE: Arduino library for INA219 voltage, current and power sensor
//     URL: https://github.com/RobTillaart/INA219

#include "i2c_ina219.h"

//  REGISTER ADDRESSES
typedef enum ina219_reg {
    INA219_CONFIGURATION = 0,
    INA219_SHUNT_VOLTAGE,
    INA219_BUS_VOLTAGE,
    INA219_POWER,
    INA219_CURRENT,
    INA219_CALIBRATION,
    //INA219_MASK_ENABLE, //no used
    //INA219_ALERT_LIMIT, //no used
    INA219_MANUFACTURER, //0x0020
    INA219_DIE_ID //0x1482
} ina219_reg;

//  CONFIGURATION REGISTER MASKS
#define INA219_CONF_RESET               0x8000
#define INA219_CONF_BUS_RANGE_VOLTAGE   0x2000
#define INA219_CONF_PROG_GAIN           0x1800
#define INA219_CONF_BUS_ADC             0x0780
#define INA219_CONF_SHUNT_ADC           0x0078
#define INA219_CONF_MODE                0x0007

#define ina219_swapwordp(a) ({ uint16_t __t=(*a)>>8; (*a)<<=8; (*a)|=__t; })

int8_t ina219_init(ina219_t *d, swi2c_t *t, uint8_t i2caddr) {
  d->pDev = t;
  d->_address = i2caddr;
#ifdef ina219_float_  
  d->_current_LSB = 0;
  d->_maxCurrent  = 0;
  d->_shunt       = 0;
#endif  
  d->_error       = 0;  
//  if (! isConnected()) return 0;
  return 0;
}

int8_t ina219_init_full(ina219_t *d, swi2c_t *t, uint8_t i2caddr, int32_t maxCurrent_uA, int32_t shunt_mO) {
	d->pDev = t;
	d->_address = i2caddr;
	d->_error = 0;
  if(ina219_isConnected(d)) {
    d->_error = -1; d->_address = 0; d->pDev = NULL; return -1;
  }
	ina219_reset(d);
  ina219_setShuntADC(d, 11); //8-15:12bit,11-8=3=8samples
  ina219_setBusADC(d, 11); //8-15:12bit,11-8=3=8samples
  ina219_setBusVoltageRange(d, 16);
  ina219_setGain(d, 1);
	ina219_setMaxCurrentShunt_uA_mO(d, maxCurrent_uA, shunt_mO);
#ifdef ina219_float_  
  d->_current_LSB = ((float)d->curLSB_uA) * 1e-6f;
  d->_maxCurrent = ((float)d->maxcur_uA) * 1e-6f;
  d->_shunt = ((float)d->shunt_mO) * 1e-3f;
#endif  
  return 0;
}

uint32_t ina219_isConnected(ina219_t *d) {
  uint32_t res;
  if(!d) { return -1; } if(!d->pDev) { return -1; }
  if ((d->_address < 0x40) || (d->_address > 0x4F)) return -1;
  res = ina219_get_manufacturer(d);
  res <<= 16;
  res |= ina219_get_dieid(d);
  if(res != 0x20008214) { return res;  }
  return 0;
}

#ifdef ina219_float_  
float ina219_getShuntVoltage(ina219_t *d) {
  int16_t value = ina219_readRegister(d, INA219_SHUNT_VOLTAGE);
  return value * 1e-5;  //  fixed 10 uV
}

float ina219_getBusVoltage(ina219_t *d) {
  uint16_t value = ina219_readRegister(d, INA219_BUS_VOLTAGE);
  uint8_t flags = value & 0x03;
  //  math overflow handling
  if (flags & 0x01) return -100;
  //  if flags && 0x02 ==> convert flag;  not handled
  float voltage = (value >> 3) * 4e-3;  //  fixed 4 mV
  return voltage;
}

float ina219_getPower(ina219_t *d) {
  uint16_t value = ina219_readRegister(d, INA219_POWER);
  return value * (d->_current_LSB * 20);  //  fixed 20 Watt
}

float ina219_getCurrent(ina219_t *d) {
  int16_t value = ina219_readRegister(d, INA219_CURRENT);
  return value * d->_current_LSB;
}

#endif

//  CORE FUNCTIONS
int32_t ina219_getShuntVoltage_uV(ina219_t *d) {
  int32_t value = (int32_t)ina219_readRegister(d, INA219_SHUNT_VOLTAGE);
  return value * 10;  //  fixed 10 uV
}

int32_t ina219_getBusVoltage_mV(ina219_t *d) {
  int32_t value = (int32_t)ina219_readRegister(d, INA219_BUS_VOLTAGE);
  if(value & 1) return -100000;
  return (value >>= 1);
}

int32_t ina219_getPower_mW(ina219_t *d) {
  int32_t value = (int32_t)ina219_readRegister(d, INA219_POWER);
  return value * (d->curLSB_uA / 50);  //  fixed 20 Watt
}

//  TODO CHECK //  needs _current_LSB factor?
int32_t ina219_getCurrent_mA(ina219_t *d) {
  int32_t value = (int32_t)ina219_readRegister(d, INA219_CURRENT);
  return (value * d->curLSB_uA) / 1000;
}

int8_t ina219_getMathOverflowFlag(ina219_t *d) {
  uint16_t value = ina219_readRegister(d, INA219_BUS_VOLTAGE);
  return ((value & 1) != 0);
}

int8_t ina219_getConversionFlag(ina219_t *d) {
  uint16_t value = ina219_readRegister(d, INA219_BUS_VOLTAGE);
  return ((value & 2) != 0);
}

//  CONFIGURATION
int8_t ina219_reset(ina219_t *d) {
  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  config |= 0x8000;
  uint16_t wrrv = ina219_writeRegister(d, INA219_CONFIGURATION, config);
  //  reset calibration
#ifdef ina219_float_    
  d->_current_LSB = 0;
  d->_maxCurrent  = 0;
  d->_shunt       = 0;
#endif  
  d->curLSB_uA = 0;
  d->maxcur_uA = 0;
  d->shunt_mO = 0;
  return (wrrv == 0);
}

int8_t ina219_setBusVoltageRange(ina219_t *d, uint8_t voltage) {
  if (voltage > 32) return 0;
  if (voltage > 16) voltage = 32;
  else voltage = 16;
  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  config &= ~INA219_CONF_BUS_RANGE_VOLTAGE;
  if (voltage == 32) config |= INA219_CONF_BUS_RANGE_VOLTAGE;

  uint16_t wrrv = ina219_writeRegister(d, INA219_CONFIGURATION, config);
  return (wrrv == 0);
}

uint8_t ina219_getBusVoltageRange(ina219_t *d) {
  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  if (config & INA219_CONF_BUS_RANGE_VOLTAGE) return 32;
  return 16;  //  volts
}

int8_t ina219_setGain(ina219_t *d, uint8_t factor) {
  if (factor != 1 && factor != 2 && factor != 4 && factor != 8) { return 0; }
  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  config &= ~INA219_CONF_PROG_GAIN;
  //  factor == 1 ==> mask = 00
  if      (factor == 2) config |= (1 << 11);
  else if (factor == 4) config |= (2 << 11);
  else if (factor == 8) config |= (3 << 11);

  uint16_t wrrv = ina219_writeRegister(d, INA219_CONFIGURATION, config);
  return (wrrv == 0);
}

uint8_t ina219_getGain(ina219_t *d) {
  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  uint16_t mask = ((config & INA219_CONF_PROG_GAIN) >> 11) ;
  //if (mask == 0x0000) return 1;
  //else if (mask == 0x0800) return 2;
  //else if (mask == 0x1000) return 4;
  //return 8;
  return (1<<mask);
}

int ina219_getMaxShuntVoltage(ina219_t *d) {
  int gain = ina219_getGain(d);  //  1,  2,  4,   8
  return gain * 40;      //  40, 80, 160, 320
}

//  BUS
#if 0
int8_t ina219_setBusResolution(ina219_t *d, uint8_t bits) {
  if ((bits < 9) || (bits > 12)) return 0;
  bits -= 9;

  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  config &= ~INA219_CONF_BUS_ADC;
  config |= ((bits&3) << 7);

  uint16_t wrrv = ina219_writeRegister(d, INA219_CONFIGURATION, config);
  return (wrrv == 0);
}

//  value = 0..7, always 12 bit resolution
int8_t ina219_setBusSamples(ina219_t *d, uint8_t value) {
  value &= 7;// if (value > 7) return 0;
  value |= 8;

  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  config &= ~INA219_CONF_BUS_ADC;
  config |= (value << 7);
  uint16_t wrrv = ina219_writeRegister(d, INA219_CONFIGURATION, config);
  return (wrrv == 0);
}
#endif
int8_t ina219_setBusADC(ina219_t *d, uint8_t mask) {
  mask &=0xf; //if (mask > 0x0F) return 0;

  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  config &= ~INA219_CONF_BUS_ADC;
  config |= (mask << 7);

  uint16_t wrrv = ina219_writeRegister(d, INA219_CONFIGURATION, config);
  return (wrrv == 0);
}

int8_t ina219_setBusResolution(ina219_t *d, uint8_t bits) {
  return ina219_setBusADC(d, ((bits - 9) & 3)); }

int8_t ina219_setBusSamples(ina219_t *d, uint8_t value) {
  return ina219_setBusADC(d, ((value & 7) | 8)); }

uint8_t ina219_getBusADC(ina219_t *d) {
  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  config &= INA219_CONF_BUS_ADC;
  return config >> 7;
}

//  SHUNT
#if 0
int8_t ina219_setShuntResolution(ina219_t *d, uint8_t bits) {
  if ((bits < 9) || (bits > 12)) return 0;
  bits -= 9;

  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  config &= ~INA219_CONF_SHUNT_ADC;
  config |= ((bits & 3) << 3);

  uint16_t wrrv = ina219_writeRegister(d, INA219_CONFIGURATION, config);
  return (wrrv == 0);
}

//  value = 0..7, always 12 bit resolution

int8_t ina219_setShuntSamples(ina219_t *d, uint8_t value) {
  value &= 7; //if (value > 7) return 0;
  value |= 8;

  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  config &= ~INA219_CONF_SHUNT_ADC;
  config |= (value << 3);

  uint16_t wrrv = ina219_writeRegister(d, INA219_CONFIGURATION, config);
  return (wrrv == 0);
}
#endif
int8_t ina219_setShuntADC(ina219_t *d, uint8_t mask) {
  mask &= 0xf; //if (mask > 0x0F) return 0;

  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  config &= ~INA219_CONF_SHUNT_ADC;
  config |= (mask << 3);

  uint16_t wrrv = ina219_writeRegister(d, INA219_CONFIGURATION, config);
  return (wrrv == 0);
}

int8_t ina219_setShuntResolution(ina219_t *d, uint8_t bits) {
  return ina219_setShuntADC(d, ((bits - 9) & 3) ); }

int8_t ina219_setShuntSamples(ina219_t *d, uint8_t value) {
  return ina219_setShuntADC(d, ((value & 7) | 8) ); }

uint8_t ina219_getShuntADC(ina219_t *d) {
  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  config &= INA219_CONF_SHUNT_ADC;
  return config >> 3;
}

//  MODE
int8_t ina219_setMode(ina219_t *d, uint8_t mode) {
  mode &= 7; //if (mode > 7) return 0;
  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  config &= ~INA219_CONF_MODE;
  config |= mode;

  uint16_t wrrv = ina219_writeRegister(d, INA219_CONFIGURATION, config);
  return (wrrv == 0);
}

uint8_t ina219_getMode(ina219_t *d) {
  uint16_t config = ina219_readRegister(d, INA219_CONFIGURATION);
  config &= INA219_CONF_MODE;
  return config;
}

//  CALIBRATION
#ifdef ina219_float_
int8_t ina219_setMaxCurrentShunt(ina219_t *d, float maxCurrent, float shunt) {
  // #define PRINTDEBUG
  if (maxCurrent < 0.001f) return 0;
  if (shunt < 0.001f) return 0;
  //  _current_LSB = maxCurrent / 32768;
  d->_current_LSB = maxCurrent * 3.0517578125e-5f;
  d->_maxCurrent = maxCurrent;
  d->_shunt = shunt;
  uint16_t calib = (uint16_t)(0.04096f / (d->_current_LSB * shunt));
  uint16_t wrrv = ina219_writeRegister(d, INA219_CALIBRATION, calib);
  #ifdef PRINTDEBUG
    Serial.println();
    Serial.print("current_LSB:\t");
    Serial.print(_current_LSB, 8);
    Serial.println(" uA / bit");

    Serial.print("Calibration:\t");
    Serial.println(calib);
    Serial.print("Max current:\t");
    Serial.print(_maxCurrent, 3);
    Serial.println(" A");
    Serial.print("Shunt:\t");
    Serial.print(_shunt, 8);
    Serial.println(" ohm Ω");
  #endif
  return (wrrv == 0);
}
#endif

int8_t ina219_setMaxCurrentShunt_uA_mO(ina219_t *d, int32_t maxCurrent_uA, int32_t shunt_mO) {
  // #define PRINTDEBUG
  if (maxCurrent_uA < 1000) return 0;
  if (shunt_mO < 1) return 0;
  //  _current_LSB = maxCurrent / 32768;
  //d->_current_LSB = maxCurrent * 3.0517578125e-5f;
  d->curLSB_uA = maxCurrent_uA >> 15;
  //d->_maxCurrent = maxCurrent;
  d->maxcur_uA = maxCurrent_uA;
  //d->_shunt = shunt;
  d->shunt_mO = shunt_mO;
  //uint16_t calib = (uint16_t)(0.04096f / (d->_current_LSB * shunt));
  uint16_t calib = (uint16_t)(40960000 / (d->curLSB_uA * d->shunt_mO));
  uint16_t wrrv = ina219_writeRegister(d, INA219_CALIBRATION, calib);
  return (wrrv == 0);
}

uint16_t ina219_get_manufacturer(ina219_t *d) {
  if(!d) return (uint16_t)-1;
  if(!d->pDev) return (uint16_t)-1;
  return ina219_readRegister(d, INA219_MANUFACTURER);
}

uint16_t ina219_get_dieid(ina219_t *d) {
  if(!d) return (uint16_t)-1;
  if(!d->pDev) return (uint16_t)-1;
  return ina219_readRegister(d, INA219_DIE_ID);
}


////////////////////////////////////////////////////////
//
//  ERROR HANDLING
//
int ina219_getLastError(ina219_t *d) { int e = d->_error; d->_error = 0; return e; }

////////////////////////////////////////////////////////
//
//  PRIVATE
//
uint16_t ina219_readRegister(ina219_t *d, uint8_t reg) {
#if 0  
  _wire->beginTransmission(_address);
  _wire->write(reg);
  int n = _wire->endTransmission();
  if (n != 0) { _error = -1; return 0; }
  uint16_t value = 0;
  if (2 == _wire->requestFrom(_address, (uint8_t)2)) {
    value = _wire->read(); value <<= 8; value |= _wire->read();
  } else { _error = -2; return 0; }
  return value;
#endif  
	uint16_t val;
  if(!d) return (uint16_t)-1;
	if(!d->pDev) return (uint16_t)-1;
	swi2c_Read_8addr(d->pDev, d->_address, reg, (uint8_t*)&val, 2);
	ina219_swapwordp(&val);
	return val;
}

uint16_t ina219_writeRegister(ina219_t *d, uint8_t reg, uint16_t value) {
#if 0  
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->write(value >> 8);
  _wire->write(value & 0xFF);
  int n = _wire->endTransmission();
  if (n != 0) { _error = -1; }
  return n;
#endif
  if(!d) return (uint16_t)-1;
	if(!d->pDev) return (uint16_t)-1;
	ina219_swapwordp(&value);
  swi2c_Write_8addr(d->pDev, d->_address, reg, (uint8_t*)&value, 2);
  return 0;
}

#ifdef ina219_float_
float     ina219_getCurrentLSB(ina219_t* d)     { return d->_current_LSB;       };
float     ina219_getShunt(ina219_t* d)          { return d->_shunt;             };
float     ina219_getMaxCurrent(ina219_t* d)     { return d->_maxCurrent;        };
int8_t    ina219_isCalibrated(ina219_t* d)      { return d->_current_LSB != 0.0; };
#else
int8_t    ina219_isCalibrated(ina219_t* d)      { return d->curLSB_uA != 0; };
#endif

uint32_t   ina219_getCurrentLSB_uA(ina219_t *d)  { return d->curLSB_uA;  }
uint32_t   ina219_getShunt_mO(ina219_t* d)       { return d->shunt_mO; }
uint32_t   ina219_getMaxCurrent_uA(ina219_t* d)  { return d->maxcur_uA; }
uint8_t   ina219_getAddress(ina219_t *d)        { return d->_address; }
uint16_t  ina219_getRegister(ina219_t* d, uint8_t reg)  { return ina219_readRegister(d, reg); };
//  -- END OF FILE --


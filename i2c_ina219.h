//#pragma once
//    FILE: INA219.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.4.1
//    DATE: 2021-05-18
// PURPOSE: Arduino library for INA219 voltage, current and power sensor
//     URL: https://github.com/RobTillaart/INA219
//
//  Read the datasheet for the details how to connect!
#ifndef _INA219_H_
#define _INA219_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "swi2c.h"

#define INA219_LIB_VERSION              (F("0.4.1"))

//#define ina219_float_

typedef enum ina219_mode {
    INA219_Shutdown = 0,
    INA219_ShuntTriggrt,  
    INA219_BusTrigger,
    INA219_ShuntBusTrigger,
    INA219_ADCoff,
    INA219_ShuntContinuous,
    INA219_BusContinuous,
    INA219_ShuntBusContinuous,
    INA219_ModeNum,
} ina219_mode;

typedef struct ina219_t {
    swi2c_t   *pDev;
    uint8_t   _address;
    int8_t    _error;
    uint32_t   curLSB_uA;
    uint32_t   shunt_mO;
    uint32_t   maxcur_uA;
#ifdef ina219_float_    
    float     _current_LSB;
    float     _shunt;
    float     _maxCurrent;    
#endif    
} ina219_t;

    //  address between 0x40 and 0x4F
    int8_t ina219_init(ina219_t *d, swi2c_t *t, uint8_t i2caddr);
    int8_t ina219_init_full(ina219_t *d, swi2c_t *t, uint8_t i2caddr, int32_t maxCurrent_uA, int32_t shunt_mO);
    uint32_t ina219_isConnected(ina219_t*);
    uint8_t ina219_getAddress(ina219_t*);


#ifdef ina219_float_    
    //  CORE FUNCTIONS               //  Register
    float ina219_getShuntVoltage(ina219_t*);      //  01
    float ina219_getBusVoltage(ina219_t*);        //  02
    float ina219_getPower(ina219_t*);             //  03
    float ina219_getCurrent(ina219_t*);           //  04
    //  Calibration
    //  These functions return zero if not calibrated!
    float ina219_getCurrentLSB(ina219_t*);
    float ina219_getShunt(ina219_t*);
    float ina219_getMaxCurrent(ina219_t*);
#endif
    //  CORE FUNCTIONS               //  Register
    int32_t ina219_getShuntVoltage_uV(ina219_t *d);
    int32_t ina219_getBusVoltage_mV(ina219_t *d);
    int32_t ina219_getPower_mW(ina219_t *d);
    int32_t ina219_getCurrent_mA(ina219_t *d);
    int8_t ina219_getMathOverflowFlag(ina219_t*);  //  02
    int8_t ina219_getConversionFlag(ina219_t*);    //  02

    uint16_t ina219_get_manufacturer(ina219_t *d);
    uint16_t ina219_get_dieid(ina219_t *d);
    
    //  Configuration
    //  need improvement API wise.
    int8_t ina219_reset(ina219_t*);
    //  voltage = 16, 32  (values below 32 are rounded to 16 or 32)
    //int8_t     ina219_setBusVoltageRange(uint8_t voltage = 16);
    int8_t ina219_setBusVoltageRange(ina219_t* d, uint8_t voltage);
    uint8_t ina219_getBusVoltageRange(ina219_t*);  //  returns 16 or 32.
    //  factor = 1, 2, 4, 8 (8 = sensor default)
    int8_t ina219_setGain(ina219_t*, uint8_t factor);  //  removed default parameter.
    uint8_t ina219_getGain(ina219_t*);
    //  MaxShuntVoltagedepends on GAIN,
    //  See Table 7. Shunt Voltage Register Format
    //  default = 320.
    int ina219_getMaxShuntVoltage(ina219_t*);

    //  configuration BUS
    //  use one of the next three
    int8_t     ina219_setBusResolution(ina219_t*, uint8_t bits);  //  9..12, always 1 sample
    int8_t     ina219_setBusSamples(ina219_t*, uint8_t value);    //  0..7, always 12 bits.
    //int8_t     ina219_setBusADC(uint8_t mask = 0x03);  //  uses a mask, check datasheet
    int8_t     ina219_setBusADC(ina219_t*, uint8_t mask);  //  uses a mask, check datasheet
    uint8_t  ina219_getBusADC(ina219_t*);

    //  configuration SHUNT
    //  use one of the next three
    int8_t ina219_setShuntResolution(ina219_t*, uint8_t bits);  //  9..12, always 1 sample
    int8_t ina219_setShuntSamples(ina219_t*, uint8_t value);    //  0..7, always 12 bits.
    //int8_t     ina219_setShuntADC(uint8_t mask = 0x03);  //  uses a mask, check datasheet
    int8_t ina219_setShuntADC(ina219_t*, uint8_t mask);  //  uses a mask, check datasheet
    uint8_t ina219_getShuntADC(ina219_t*);

    //  Calibration
    //  mandatory to set these! read datasheet.
    //  maxCurrent >= 0.001
    //  shunt      >= 0.001
    //int8_t     setMaxCurrentShunt(float maxCurrent = 3.4, float shunt = 0.002);
    int8_t ina219_setMaxCurrentShunt(ina219_t*, float maxCurrent, float shunt);
    int8_t ina219_setMaxCurrentShunt_uA_mO(ina219_t *d, int32_t maxCurrent_uA, int32_t shunt_mO);
    int8_t ina219_isCalibrated(ina219_t*);

    //  These functions return zero if not calibrated!
    uint32_t ina219_getCurrentLSB_uA(ina219_t*);
    uint32_t ina219_getShunt_mO(ina219_t*);
    uint32_t ina219_getMaxCurrent_uA(ina219_t*);


    //  Operating mode = 0..7
    int8_t ina219_setMode(ina219_t*, uint8_t mode);  //  default ModeShuntBusContinuous
    uint8_t ina219_getMode(ina219_t*);

    //  DEBUG
    uint16_t ina219_getRegister(ina219_t*, uint8_t reg);

    //
    //  ERROR HANDLING
    //
    int      ina219_getLastError();

    uint16_t ina219_readRegister(ina219_t*, uint8_t reg);
    uint16_t ina219_writeRegister(ina219_t*, uint8_t reg, uint16_t value);

#ifdef __cplusplus
}
#endif
#endif
//  -- END OF FILE --


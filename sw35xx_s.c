#include "sw35xx_s.h"
#include "sw3518_reg.h"

#define SW35XX_ADDRESS 0x3c
#define SW35XX_IC_VERSION 0x01
#define SW35XX_FCX_STATUS 0x06
#define SW35XX_PWR_STATUS 0x07
#define SW35XX_I2C_ENABLE 0x12
#define SW35XX_I2C_CTRL 0x13
#define SW35XX_ADC_VIN_H 0x30
#define SW35XX_ADC_VOUT_H 0x31
#define SW35XX_ADC_VIN_VOUT_L 0x32
#define SW35XX_ADC_IOUT_USBC_H 0x33
#define SW35XX_ADC_IOUT_USBA_H 0x34
#define SW35XX_ADC_IOUT_USBC_USBA_L 0x35
#define SW35XX_ADC_TS_H 0x37
#define SW35XX_ADC_TS_L 0x38
#define SW35XX_ADC_DATA_TYPE 0x3a
#define SW35XX_ADC_DATA_BUF_H 0x3b
#define SW35XX_PD_SRC_REQ 0x70
#define SW35XX_ADC_DATA_BUF_L 0x3c
#define SW35XX_PWR_CONF 0xa6
#define SW35XX_QC_CONF0 0xaa
#define SW35XX_VID_CONF0 0xaf
#define SW35XX_PD_CONF1 0xb0
#define SW35XX_PD_CONF2 0xb1
#define SW35XX_PD_CONF3 0xb2
#define SW35XX_PD_CONF4 0xb3
#define SW35XX_PD_CONF5 0xb4
#define SW35XX_PD_CONF6 0xb5
#define SW35XX_PD_CONF7 0xb6
#define SW35XX_PD_CONF8 0xb7
#define SW35XX_PD_CONF9 0xb8
#define SW35XX_QC_CONF1 0xb9
#define SW35XX_QC_CONF2 0xba
#define SW35XX_PD_CONF10 0xbe
#define SW35XX_VID_CONF1 0xbf

#define I2C_RETRIES 10

int __sw35xx_i2cReadReg8(struct t_sw35xx *d, const uint8_t reg) {
/*	int i;
  for(i=0; i<I2C_RETRIES; i++) {
    _i2c.beginTransmission(SW35XX_ADDRESS);
    if (_i2c.write(reg) != 1) { continue; }
    if (_i2c.endTransmission() != 0) { continue; }
    if (_i2c.requestFrom(SW35XX_ADDRESS, 1) != 1) { continue; }
    // Wait until data is available if required 
    for (int k=0; !_i2c.available() && k<I2C_RETRIES; k++) { delay(10); }
    const int value = _i2c.read();
    if (value < 0) { continue; }
    return value;
  }
	return 0;
*/
	int32_t i = I2C_RETRIES;
	uint32_t val=0;
	for(; i > 0; i--) {
        if(swi2c_Read_8addr(d->pDev, SW35XX_ADDRESS, reg, &val, 1) == 0) break;
	}
	return (i == 0) ? 2 : 0;
}

int __sw35xx_i2cWriteReg8(struct t_sw35xx *d, const uint8_t reg, const uint8_t data) {
/*  int error = -1;
  for (int i=0; i<I2C_RETRIES; i++) {
    _i2c.beginTransmission(SW35XX_ADDRESS);
    if (_i2c.write(reg) != 1) { continue; }
    if (_i2c.write(data) != 1) { continue; }
    error = _i2c.endTransmission();
    if (error == 0) { return 0; }
  }
  return error;*/
	int32_t i = I2C_RETRIES;
	for(; i > 0; i--) {
		if(swi2c_Write_8addr(d->pDev, SW35XX_ADDRESS, reg, (uint8_t*)&data, 1) == 0) break;
	}
	return (i == 0) ? 2 : 0;
}

void SW35xx_begin(struct t_sw35xx *d){
  //启用输入电压读取
    __sw35xx_i2cWriteReg8(d, SW35XX_I2C_CTRL, 0x02);
}

uint16_t __sw35xx_readADCDataBuffer(struct t_sw35xx *d, const uint8_t type) {
	uint16_t value;
    __sw35xx_i2cWriteReg8(d, SW35XX_ADC_DATA_TYPE, type);
    value = __sw35xx_i2cReadReg8(d, SW35XX_ADC_DATA_BUF_H) << 4;
    value |= __sw35xx_i2cReadReg8(d, SW35XX_ADC_DATA_BUF_L) | 0x0f;
	return value;
}

void SW35xx_readStatus(struct t_sw35xx *d, uint8_t useADCDataBuffer) {
  uint16_t vin = 0;
  uint16_t vout = 0;
  uint16_t iout_usbc = 0;
  uint16_t iout_usba = 0;

  SW35xx_unlock_i2c_write(d);
  __sw35xx_i2cWriteReg8(d, SW35XX_I2C_CTRL, 0x02);
  if(useADCDataBuffer != 0) {
    //读取输入电压
    vin = __sw35xx_readADCDataBuffer(d, ADC_VIN);
    //读取输出电压
    vout = __sw35xx_readADCDataBuffer(d, ADC_VOUT);
    //读取接口1输出电流
    iout_usbc = __sw35xx_readADCDataBuffer(d, ADC_IOUT_USB_C);
    //读取接口2输出电流
    iout_usba = __sw35xx_readADCDataBuffer(d, ADC_IOUT_USB_A);
  } else {
    uint8_t vin_vout_low = __sw35xx_i2cReadReg8(d, SW35XX_ADC_VIN_VOUT_L);
    vin = __sw35xx_i2cReadReg8(d, SW35XX_ADC_VIN_H) << 4;
    vin |= vin_vout_low >> 4;
    vout = __sw35xx_i2cReadReg8(d, SW35XX_ADC_VOUT_H) << 4;
    vout |= vin_vout_low & 0x0F;

    uint8_t iout_low = __sw35xx_i2cReadReg8(d, SW35XX_ADC_IOUT_USBC_USBA_L);
    iout_usbc = __sw35xx_i2cReadReg8(d, SW35XX_ADC_IOUT_USBC_H) << 4;
    iout_usbc |= iout_low >> 4;
    iout_usba = __sw35xx_i2cReadReg8(d, SW35XX_ADC_IOUT_USBA_H) << 4;
    iout_usba |= iout_low & 0x0F;
  }

  d->vin_mV = vin * 10;
  d->vout_mV = vout * 6;
  //在没有输出的情况下读到的数据是15
  if (iout_usbc > 15) d->iout_usbc_mA = iout_usbc * 5 / 2;
  else d->iout_usbc_mA = 0;

  if (iout_usba > 15) d->iout_usba_mA = iout_usba * 5 / 2;
  else d->iout_usba_mA = 0;
  //读取pd版本和快充协议
  uint8_t status = __sw35xx_i2cReadReg8(d, SW35XX_FCX_STATUS);
  d->PDVersion = ((status & 0x30) >> 4) + 1;
  d->fastChargeType = (status & 0x0f);

    SW35xx_lock_i2c_write(d);
}

float SW35xx_readTemperature(struct t_sw35xx *d, uint8_t useADCDataBuffer) {
  uint16_t temperature = 0;

  if(useADCDataBuffer != 0) {
    temperature = __sw35xx_readADCDataBuffer(d, ADC_TEMPERATURE);
  } else {
    temperature = __sw35xx_i2cReadReg8(d, SW35XX_ADC_TS_H) << 4;
    temperature |= __sw35xx_i2cReadReg8(d, SW35XX_ADC_TS_L) & 0x0F;
  }

  /* return it in mV */
  return temperature * 0.5;
}

void SW35xx_unlock_i2c_write(struct t_sw35xx *d) {
	__sw35xx_i2cWriteReg8(d, SW35XX_I2C_ENABLE, 0x20);
	__sw35xx_i2cWriteReg8(d, SW35XX_I2C_ENABLE, 0x40);
	__sw35xx_i2cWriteReg8(d, SW35XX_I2C_ENABLE, 0x80);
}

void SW35xx_lock_i2c_write(struct t_sw35xx *d) {
	__sw35xx_i2cWriteReg8(d, SW35XX_I2C_ENABLE, 0x00);
}

void SW35xx_sendPDCmd(struct t_sw35xx *d, uint8_t PDcmd){
	__sw35xx_i2cWriteReg8(d, SW35XX_PD_SRC_REQ, (const uint8_t)PDcmd);
	__sw35xx_i2cWriteReg8(d, SW35XX_PD_SRC_REQ, (const uint8_t)(PDcmd | 0x80));
}

void SW35xx_rebroadcastPDO(struct t_sw35xx *d){
	__sw35xx_i2cWriteReg8(d, SW35XX_I2C_CTRL, 0x03);
}

void SW35xx_setMaxCurrent5A(struct t_sw35xx *d) {
	SW35xx_unlock_i2c_write(d);
	__sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF1, 0b01100100);
	__sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF2, 0b01100100);
	__sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF3, 0b01100100);
	__sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF4, 0b01100100);
	__sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF6, 0b01100100);
	__sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF7, 0b01100100);
	SW35xx_lock_i2c_write(d);
}

void SW35xx_setQuickChargeConfiguration(struct t_sw35xx *d, const uint16_t flags,
    uint16_t QuickChargePowerClass) {
  /* mask all available bits to avoid setting reserved bits */
  const uint16_t validFlags = flags & QC_CONF_ALL;
  const uint16_t validPower = QuickChargePowerClass & QC_PWR_20V_2;
  const uint8_t conf1 = validFlags;
  const uint8_t conf2 = (validFlags >> 8) | (validPower << 2);

  SW35xx_unlock_i2c_write(d);
  __sw35xx_i2cWriteReg8(d, SW35XX_QC_CONF1, conf1); //0-7 QuickChargeConfig
  __sw35xx_i2cWriteReg8(d, SW35XX_QC_CONF2, conf2); //bit4-5 QuickChargePowerClass, 6-7 QuickChargeConfig??
  SW35xx_lock_i2c_write(d);
}

void SW35xx_setMaxCurrentsFixed(struct t_sw35xx *d, uint32_t ma_5v, uint32_t ma_9v, uint32_t ma_12v, uint32_t ma_15v, uint32_t ma_20v){
  if(ma_5v > 5000) ma_5v = 5000;
  if(ma_9v > 5000) ma_9v = 5000;
  if(ma_12v > 5000) ma_12v = 5000;
  if(ma_15v > 5000) ma_15v = 5000;
  if(ma_20v > 5000) ma_20v = 5000;

  uint8_t tmp = __sw35xx_i2cReadReg8(d, SW35XX_PD_CONF8);

  if(ma_9v == 0)    tmp &= 0b11111011;
  else              tmp |= 0b00000100;

  if (ma_12v == 0)  tmp &= 0b11110111;
  else              tmp |= 0b00001000;

  if (ma_15v == 0)  tmp &= 0b11101111;
  else              tmp |= 0b00010000;

  if (ma_20v == 0)  tmp &= 0b11011111;
  else              tmp |= 0b00100000;
  
  SW35xx_unlock_i2c_write(d);
  __sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF8, tmp);
  __sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF1, ma_5v/50);
  __sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF2, ma_9v/50);
  __sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF3, ma_12v/50);
  __sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF4, ma_15v/50);
  __sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF5, ma_20v/50);
  SW35xx_lock_i2c_write(d);
}

void SW35xx_setMaxCurrentsPPS(struct t_sw35xx *d, uint32_t ma_pps1, uint32_t ma_pps2) {
  if (ma_pps1 > 5000) ma_pps1 = 5000;
  if (ma_pps2 > 5000) ma_pps2 = 5000;
  uint8_t tmp = __sw35xx_i2cReadReg8(d, SW35XX_PD_CONF8);

  if(ma_pps1 == 0) tmp &= 0b10111111;
  else tmp |= 0b01000000;

  if(ma_pps1 == 0) tmp &= 0b01111111;
  else tmp |= 0b10000000;
    
  SW35xx_unlock_i2c_write(d);
  __sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF8, tmp);
  __sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF6, ma_pps1/50);
  __sw35xx_i2cWriteReg8(d, SW35XX_PD_CONF7, ma_pps2/50);
  SW35xx_lock_i2c_write(d);
}

/*uint8_t sw35xx_unlock_i2c(struct t_sw35xx *d) {
	uint8_t res=0;
	res |= __sw35xx_i2cWriteReg8(d, SW35XX_I2C_ENABLE, 0x20);
	res |= __sw35xx_i2cWriteReg8(d, SW35XX_I2C_ENABLE, 0x40);
	res |= __sw35xx_i2cWriteReg8(d, SW35XX_I2C_ENABLE, 0x80);
	return res;
}

uint8_t sw35xx_lock_i2c(struct t_sw35xx *d) {
	return __sw35xx_i2cWriteReg8(d, SW35XX_I2C_ENABLE, 0);
}*/

uint8_t sw35xx_init(struct t_sw35xx *d, swi2c_t *pDev) {
	int8_t res;
	d->pDev = pDev;
	d->init = 0;
	swi2c_dummy_clock(d->pDev);
    SW35xx_unlock_i2c_write(d);
	//__sw35xx_i2cWriteReg8(d, SW35XX_I2C_CTRL, 0x02);
    res = __sw35xx_i2cWriteReg8(d, SW35XX_I2C_CTRL, 0x02);
	d->init = (res == 0) ? 1 : 0;
    SW35xx_lock_i2c_write(d);
    return 0;
}

uint8_t sw35xx_version(struct t_sw35xx *d) {
    reg_ic_version ver;
    SW35xx_unlock_i2c_write(d);
    *(uint8_t*)&ver = __sw35xx_i2cReadReg8(d, SW35XX_IC_VERSION);
    SW35xx_lock_i2c_write(d);
	return ver.version;
}


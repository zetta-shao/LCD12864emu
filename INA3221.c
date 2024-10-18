/*

    Arduino library for INA3221 current and voltage sensor.

    MIT License

    Copyright (c) 2020 Beast Devices, Andrejs Bondarevs

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

*/

#include "INA3221.h"
#include <stddef.h>

#define swapwordp(a) ({ uint16_t __t=(*a)>>8; (*a)<<=8; (*a)|=__t; })

void ina3221_read(ina3221_t *d, uint8_t reg, uint16_t *val) {
    //pDev->beginTransmission(_i2c_addr);
    //pDev->write(reg);  // Register
    //pDev->endTransmission(false);
    //pDev->requestFrom((uint8_t)_i2c_addr, (uint8_t)2);
    //if (pDev->available()) { *val = ((pDev->read() << 8) | pDev->read()); }
	if(d->pDev == NULL) return;
    swi2c_Read_8addr(d->pDev, d->_i2c_addr, reg, (uint8_t*)val, 2);
    swapwordp(val);
}

uint16_t ina3221_read16(ina3221_t *d, uint8_t reg) {
	uint16_t val;
	if(d->pDev == NULL) return 65535;
	swi2c_Read_8addr(d->pDev, d->_i2c_addr, reg, (uint8_t*)&val, 2);
	swapwordp(&val);
	return val;
}

void ina3221_write(ina3221_t *d, uint8_t reg, uint16_t *val) {
    //_i2c->beginTransmission(_i2c_addr);
    //_i2c->write(reg);                 // Register
    //_i2c->write((*val >> 8) & 0xFF);  // Upper 8-bits
    //_i2c->write(*val & 0xFF);         // Lower 8-bits
    //_i2c->endTransmission();
	if(d->pDev == NULL) return;
	swapwordp(val);
    swi2c_Write_8addr(d->pDev, d->_i2c_addr, reg, (uint8_t*)val, 2);
}

void ina3221_write16(ina3221_t *d, uint8_t reg, uint16_t val) {
	if(d->pDev == NULL) return;
	swapwordp(&val);
    swi2c_Write_8addr(d->pDev, d->_i2c_addr, reg, (uint8_t*)&val, 2);
}

uint8_t ina3221_detect(ina3221_t *d, struct swi2c_t *pvDev) {
    uint8_t i, res;
    uint8_t bAdr[4] = { INA3221_ADDR43_SCL, INA3221_ADDR42_SDA, INA3221_ADDR41_VCC, INA3221_ADDR40_GND };
    uint8_t temp[4];
    uint16_t *pID = (uint16_t*)&temp;
    res = 0;

    d->pDev = pvDev;    
    for(i=0; i<4; i++) {
        swi2c_Read_8addr(d->pDev, bAdr[i], INA3221_REG_DIE_ID, temp, 2);
        if(pID[0] == 0x2032) {
            res = bAdr[i]; 
            d->_i2c_addr = bAdr[i];
            break;
        }
    }
    if(res == 0) return 1;
    return 0;
}

//void HAL_Delay(uint32_t Delay); //stm32 api
uint8_t ina3221_init(ina3221_t *d, struct swi2c_t *pvDev) {
	if(d==NULL || pvDev==NULL) return 255;
	d->pDev = pvDev;
	if(d->_i2c_addr == 0) d->_i2c_addr = default_i2caddr;
	swi2c_dummy_clock(d->pDev);
	d->_shuntRes[0] = 100; //100 mean 100mOhm.
	d->_shuntRes[1] = 100;
	d->_shuntRes[2] = 100;
#ifdef use_float
	d->_filterRes[0] = 0;
	d->_filterRes[1] = 0;
	d->_filterRes[2] = 0;
#endif
    if(ina3221_detect(d, pvDev) != 0) { d->pDev = NULL; return 2; }
    //if(ina3221_getDieID(d) != 0x3220) { d->pDev = NULL; return 2; }

	ina3221_write16(d, INA3221_REG_CONF, 0x8000); //reset
	//HAL_Delay(10);
	//d->pDev->hal_delay_ms(10);
    swi2c_delay_ms(10);
	//STM32_DELAY_MS(10);
	ina3221_write16(d, INA3221_REG_CONF, 0x7127); //to default
	//HAL_Delay(2);
	//if(ina3221_getDieID(d) != 0x3220) { d->pDev = NULL; return 3; }
	//else return 0;
    return 0;
}

/*void ina3221_setShuntRes(ina3221_t *d, uint32_t res_ch1, uint32_t res_ch2,
                          uint32_t res_ch3) {
	d->_shuntRes[0] = res_ch1;
	d->_shuntRes[1] = res_ch2;
	d->_shuntRes[2] = res_ch3;
}

void ina3221_setFilterRes(ina3221_t *d, uint32_t res_ch1, uint32_t res_ch2,
                           uint32_t res_ch3) {
	d->_filterRes[0] = res_ch1;
	d->_filterRes[1] = res_ch2;
	d->_filterRes[2] = res_ch3;
}*/

/*uint16_t ina3221_getReg(ina3221_t *d, uint16_t reg) {
    uint16_t val = 0;
    ina3221_read(d, reg, &val);
    return val;
}*/

/*void ina3221_reset(ina3221_t *d) {
	conf_reg_t CR = { 0 };
	//ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.reset = 1;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}*/

/*void ina3221_setModePowerDown(ina3221_t *d) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.mode_bus_en = 0;
	CR.mode_continious_en = 0;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setModeContinious(ina3221_t *d, uint8_t triggerd) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.mode_continious_en = (triggerd)?0:1;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setShuntMeasAble(ina3221_t *d, uint8_t enable) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.mode_shunt_en = (enable)?1:0;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setBusMeasAble(ina3221_t *d, uint8_t enable) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.mode_bus_en = (enable)?1:0;;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}*/

void ina3221_setAveragingMode(ina3221_t *d, ina3221_avg_mode_t mode) {
	conf_reg_t CR;	
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.avg_mode = mode;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setBusConversionTime(ina3221_t *d, ina3221_conv_time_t convTime) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.bus_conv_time = convTime;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

void ina3221_setShuntConversionTime(ina3221_t *d, ina3221_conv_time_t convTime) {
	conf_reg_t CR;	
	ina3221_read(d, INA3221_REG_CONF, (uint16_t *)&CR);
	CR.shunt_conv_time = convTime;
	ina3221_write(d, INA3221_REG_CONF, (uint16_t *)&CR);
}

/*void ina3221_setPwrValidUpLimit(ina3221_t *d, int16_t voltagemV) {
	ina3221_write(d, INA3221_REG_PWR_VALID_HI_LIM, (uint16_t *)&voltagemV);
}

void ina3221_setPwrValidLowLimit(ina3221_t *d, int16_t voltagemV) {
    ina3221_write(d, INA3221_REG_PWR_VALID_LO_LIM, (uint16_t *)&voltagemV);
}

void ina3221_setShuntSumAlertLimit(ina3221_t *d, int32_t voltageuV) {
    int16_t val = voltageuV / 20;
    ina3221_write(d, INA3221_REG_SHUNTV_SUM_LIM, (uint16_t *)&val);
}

void ina3221_setCurrentSumAlertLimit(ina3221_t *d, int32_t currentmA) {
    int16_t shuntuV = currentmA * (int32_t)d->_shuntRes[INA3221_CH1];
    ina3221_setShuntSumAlertLimit(d, shuntuV);
}*/

/*void ina3221_setWarnAlertLatchAble(ina3221_t *d, uint8_t enable) {
	masken_reg_t MR;	
	ina3221_read(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
	MR.warn_alert_latch_en = 1;
	ina3221_write(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
	d->_masken_reg = MR;
}*/

/*void ina3221_setCritAlertLatchAble(ina3221_t *d, uint8_t enable) {
    masken_reg_t MR;
    ina3221_read(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    MR.crit_alert_latch_en = (enable)?1:0;
    ina3221_write(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    d->_masken_reg = MR;
}*/

/*uint16_t ina3221_readFlags(ina3221_t *d) {
	masken_reg_t MR;
    ina3221_read(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    d->_masken_reg = MR;
    return *(uint16_t*)&MR;
}*/

/*uint8_t ina3221_getTimingCtrlAlertFlag(ina3221_t *d) {
    return (uint8_t)d->_masken_reg.timing_ctrl_alert;
}

uint8_t ina3221_getPwrValidAlertFlag(ina3221_t *d) {
    return (uint8_t)d->_masken_reg.pwr_valid_alert;
}

uint8_t ina3221_getCurrentSumAlertFlag(ina3221_t *d) {
    return (uint8_t)d->_masken_reg.shunt_sum_alert;
}

uint8_t ina3221_getConversionReadyFlag(ina3221_t *d) {
    return (uint8_t)d->_masken_reg.conv_ready;
}*/

uint16_t ina3221_getManufID(ina3221_t *d) {
    return ina3221_read16(d, INA3221_REG_MANUF_ID);
}

uint16_t ina3221_getDieID(ina3221_t *d) {
    return ina3221_read16(d, INA3221_REG_DIE_ID);
}

void ina3221_setChannelAble(ina3221_t *d, uint8_t channel, uint8_t enable) {
	conf_reg_t CR;
	ina3221_read(d, INA3221_REG_CONF, (uint16_t*)&CR);
    enable = (enable)?1:0;
	switch (channel) {
	case INA3221_CH1: CR.ch1_en = enable; break;
	case INA3221_CH2: CR.ch2_en = enable; break;
	case INA3221_CH3: CR.ch3_en = enable; break;
	}
	ina3221_write(d, INA3221_REG_CONF, (uint16_t*)&CR);
}

/*void ina3221_setWarnAlertShuntLimit(ina3221_t *d, uint8_t channel, int32_t voltageuV) {
    int16_t val = voltageuV / 5; uint16_t *pT = (uint16_t*)&val;
    switch (channel) {
        case INA3221_CH1: ina3221_write16(d, INA3221_REG_CH1_WARNING_ALERT_LIM, *pT); break;
        case INA3221_CH2: ina3221_write16(d, INA3221_REG_CH2_WARNING_ALERT_LIM, *pT); break;
        case INA3221_CH3: ina3221_write16(d, INA3221_REG_CH3_WARNING_ALERT_LIM, *pT); break;
    }
}

void ina3221_setCritAlertShuntLimit(ina3221_t *d, uint8_t channel, int32_t voltageuV) {
    int16_t val = voltageuV / 5; uint16_t *pT = (uint16_t*)&val;
    switch (channel) {
        case INA3221_CH1: ina3221_write16(d, INA3221_REG_CH1_CRIT_ALERT_LIM, *pT); break;
        case INA3221_CH2: ina3221_write16(d, INA3221_REG_CH2_CRIT_ALERT_LIM, *pT); break;
        case INA3221_CH3: ina3221_write16(d, INA3221_REG_CH3_CRIT_ALERT_LIM, *pT); break;
    }
}

void ina3221_setWarnAlertCurrentLimit(ina3221_t *d, uint8_t channel,
                                       int32_t currentmA) {
    int32_t shuntuV = currentmA * (int32_t)d->_shuntRes[channel];
    ina3221_setWarnAlertShuntLimit(d, channel, shuntuV);
}

void ina3221_setCritAlertCurrentLimit(ina3221_t *d, uint8_t channel,
                                       int32_t currentmA) {
    int32_t shuntuV = currentmA * (int32_t)d->_shuntRes[channel];
    ina3221_setCritAlertShuntLimit(d, channel, shuntuV);
}*/

void ina3221_setCurrentSumAble(ina3221_t *d, uint8_t channel, uint8_t enable) {
    masken_reg_t MR;
    ina3221_read(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    enable = (enable)?1:0;
    switch (channel) {
        case INA3221_CH1: MR.shunt_sum_en_ch1 = enable; break;
        case INA3221_CH2: MR.shunt_sum_en_ch2 = enable; break;
        case INA3221_CH3: MR.shunt_sum_en_ch3 = enable; break;
    }
    ina3221_write(d, INA3221_REG_MASK_ENABLE, (uint16_t*)&MR);
    d->_masken_reg = MR;
}

int16_t ina3221_getShuntVolRaw(ina3221_t *d, uint8_t channel) { //40uV per bit, -32768 ~ +32760
	int16_t v = -1;
    switch (channel) {	//40uV per bit
        case INA3221_CH1: ina3221_read(d, INA3221_REG_CH1_SHUNTV, (uint16_t*)&v); break;
        case INA3221_CH2: ina3221_read(d, INA3221_REG_CH2_SHUNTV, (uint16_t*)&v); break;
        case INA3221_CH3: ina3221_read(d, INA3221_REG_CH3_SHUNTV, (uint16_t*)&v); break;
    }
    return v;
}

/*int32_t ina3221_getShuntVoltage(ina3221_t *d, uint8_t channel) { //by mV, -163840 ~ +163800
    int32_t res;
    //int16_t val_raw = 0;
    //switch (channel) {
    //    case INA3221_CH1: ina3221_read(d, INA3221_REG_CH1_SHUNTV, (uint16_t*)&val_raw); break;
    //    case INA3221_CH2: ina3221_read(d, INA3221_REG_CH2_SHUNTV, (uint16_t*)&val_raw); break;
    //    case INA3221_CH3: ina3221_read(d, INA3221_REG_CH3_SHUNTV, (uint16_t*)&val_raw); break;
    //}
    //res = (int32_t)(val_raw >> 3) * 40;
    res = (int32_t)ina3221_getShuntVolRaw(d, channel);
    res += res << 2;
    return res;
}*/


/*uint8_t ina3221_getWarnAlertFlag(ina3221_t *d, uint8_t channel) {
    switch (channel) {
        case INA3221_CH1: return (uint8_t)d->_masken_reg.warn_alert_ch1; break;
        case INA3221_CH2: return (uint8_t)d->_masken_reg.warn_alert_ch2; break;
        case INA3221_CH3: return (uint8_t)d->_masken_reg.warn_alert_ch3; break;
        default: return 0;
    }
}

uint8_t ina3221_getCritAlertFlag(ina3221_t *d, uint8_t channel) {
    switch (channel) {
        case INA3221_CH1: return (uint8_t)d->_masken_reg.crit_alert_ch1; break;
        case INA3221_CH2: return (uint8_t)d->_masken_reg.crit_alert_ch2; break;
        case INA3221_CH3: return (uint8_t)d->_masken_reg.crit_alert_ch3; break;
        default: return 0;
    }
}*/

//mV, low-3bit always 0
int16_t ina3221_getVol_Raw(ina3221_t *d, uint8_t channel) { //mV, low-3bit always 0
	int16_t v = 32767;
	if(d->pDev == NULL) return -1;
    switch (channel) { //just get, no shift.
	case INA3221_CH1: ina3221_read(d, INA3221_REG_CH1_BUSV, (uint16_t*)&v); break;
	case INA3221_CH2: ina3221_read(d, INA3221_REG_CH2_BUSV, (uint16_t*)&v); break;
	case INA3221_CH3: ina3221_read(d, INA3221_REG_CH3_BUSV, (uint16_t*)&v); break;
    }
    return v;
}

#ifdef use_float
int32_t ina3221_estimateOffsetVoltage(ina3221_t *d, uint8_t channel, uint32_t busV) {
    float bias_in     = 10.0;   // Input bias current at IN– in uA
    float r_in        = 0.670;  // Input resistance at IN– in MOhm
    uint32_t adc_step = 40;     // smallest shunt ADC step in uV
    float shunt_res   = d->_shuntRes[channel] / 1000.0;  // convert to Ohm
    float filter_res  = d->_filterRes[channel];
    int32_t offset    = 0.0;
    float reminder;

    offset = (shunt_res + filter_res) * (busV / r_in + bias_in) -
             bias_in * filter_res;

    // Round the offset to the closest shunt ADC value
    reminder = offset % adc_step;
    if (reminder < adc_step / 2) {
        offset -= reminder;
    } else {
        offset += adc_step - reminder;
    }

    return offset;
}

float ina3221_getCurrent(ina3221_t *d, uint8_t channel) { //by A
    //int32_t shunt_uV = 0;
    float current_A;

    //shunt_uV  = ina3221_getShuntVoltage(d, channel);
    //current_A = shunt_uV / 1000.0 / (int32_t)d->_shuntRes[channel];
    current_A = (float)ina3221_getShuntVoltage(d, channel);
    current_A /= (1000.0f * (float)d->_shuntRes[channel]);
    return current_A;
}

float ina3221_getVoltage(ina3221_t *d, uint8_t channel) { //by V
    float voltage_V = 0.0;
    //int16_t val_raw = 0;
    //switch (channel) {
    //    case INA3221_CH1: ina3221_read(d, INA3221_REG_CH1_BUSV, (uint16_t*)&val_raw); break;
    //    case INA3221_CH2: ina3221_read(d, INA3221_REG_CH2_BUSV, (uint16_t*)&val_raw); break;
    //    case INA3221_CH3: ina3221_read(d, INA3221_REG_CH3_BUSV, (uint16_t*)&val_raw); break;
    //}
    //voltage_V = (float)(val_raw);
    voltage_V = (float)ina3221_getVol_Raw(d, channel);
    voltage_V /= 1000.0f;
    return voltage_V;
}

float ina3221_getCurrentCompensated(ina3221_t *d, uint8_t channel) {
    int32_t shunt_uV  = 0;
    int32_t bus_V     = 0;
    float current_A   = 0.0;
    int32_t offset_uV = 0;

    shunt_uV  = ina3221_getShuntVoltage(d, channel);
    bus_V     = ina3221_getVoltage(d, channel);
    offset_uV = ina3221_estimateOffsetVoltage(d, channel, bus_V);

    current_A = (shunt_uV - offset_uV) / (int32_t)d->_shuntRes[channel] / 1000.0;

    return current_A;
}
#endif //use_float
int16_t ina3221_getCur_mA(ina3221_t *d, uint8_t channel) {
	int32_t val;
	if(d->pDev == NULL) return -1;
	//switch (channel) {
	//	case INA3221_CH1: ina3221_read(d, INA3221_REG_CH1_SHUNTV, (uint16_t*)&val); break;
	//	case INA3221_CH2: ina3221_read(d, INA3221_REG_CH2_SHUNTV, (uint16_t*)&val); break;
	//	case INA3221_CH3: ina3221_read(d, INA3221_REG_CH3_SHUNTV, (uint16_t*)&val); break;
	//}
	val = ina3221_getShuntVolRaw(d, channel);
	val += val << 2;
	val /= d->_shuntRes[channel];
	return (int16_t)val;
}
#ifdef INA3221_AVGVAL
int32_t ina3221_getAvgVol(ina3221_t *d, uint8_t channel) {
	AvgVal_t *t;
	int16_t wVol;
	if(!d || !d->pDev || channel > 2 || channel < 0) return 0;
	wVol = ina3221_getVol_Raw(d, channel) >> 2;
	t = &(d->avg[channel]);
	t->VolAvg -= (int32_t)t->VolTab[t->vidx];
	t->VolTab[t->vidx] = wVol;
	t->VolAvg += (int32_t)wVol;
	(t->vidx)++;
	if(t->vidx >= VolTabNum) t->vidx = 0;
	return (t->VolAvg >> ScaleShift);
}

int32_t ina3221_getAvgCur(ina3221_t *d, uint8_t channel) { //channel0-2
	AvgVal_t *t;
	int32_t wCur;
	if(!d || !d->pDev || channel > 2 || channel < 0) return 0;
	//when shunt-res=0.04ohm, wCur=ina3221_getShuntVolRaw*5/40=ina3221_getShuntVolRaw/8
#ifdef use_shuntres
	wCur = ina3221_getShuntVolRaw(d, channel);
	wCur += wCur << 2;
	wCur /= d->_shuntRes[channel];
#else
	//wCur = ina3221_getShuntVolRaw(d, channel) >> 3; //2 for 20mOhm, 3 for 40mOhm
	wCur = ina3221_getShuntVolRaw(d, channel) >> 3;
#endif
	t = &(d->avg[channel]);
	t->CurAvg -= (int32_t)t->CurTab[t->cidx];
	t->CurTab[t->cidx] = wCur;
	t->CurAvg += (int32_t)wCur;
	(t->cidx)++;
	if(t->cidx >= CurTabNum) t->cidx = 0;
	return (t->CurAvg >> ScaleShift);
}
#endif
int32_t ina3221_getCurPower(ina3221_t *d, uint8_t channel) { //mW
	if(d->pDev == NULL) return -1;
	d->voltage[channel] = ina3221_getVol_Raw(d, channel);
	d->curcuit[channel] = ina3221_getCur_mA(d, channel);
	d->power[channel] = (d->voltage[channel] * d->curcuit[channel]) / 1000;
	return d->power[channel];
}

#include "IP2365.h"
#include <stddef.h>

#define CUR_BASE 200 //default is 100
#define CUR_MUL 100 //default is 50

void IP2365_writeReg(IP2365_t *d, uint8_t reg, uint8_t val) {
	if(!d || d->pD == NULL) return;
	swi2c_Write_8addr(d->pD, i2caddr, reg, &val, 1);
}

void IP2365_writeRegP(IP2365_t *d, uint8_t reg, uint8_t *val) {
	if(!d || d->pD == NULL) return;
	//uint8_t tmp;
	swi2c_Write_8addr(d->pD, i2caddr, reg, val, 1);
	//swi2c_delay_ms(1);
	//swi2c_Read_8addr(d->pD, i2caddr, reg, &tmp, 1);
	//if(*val != tmp) printf("err:vertify fail.\n");
}

uint8_t IP2365_readReg(IP2365_t *d, uint8_t reg) {
	uint8_t val;
	if(!d || d->pD == NULL) return 255;
	swi2c_Read_8addr(d->pD, i2caddr, reg, &val, 1);
	return val;
}

void IP2365_readRegP(IP2365_t *d, uint8_t reg, uint8_t *val) {
	if(!d || d->pD == NULL) return;
	swi2c_Read_8addr(d->pD, i2caddr, reg, val, 1);
}

int8_t IP2365_detect(IP2365_t *d) {
	uint8_t r1, r2;
	r1 = IP2365_readReg(d, 0x90);
	r2 = IP2365_readReg(d, 0x97);
	if((r1 == 1) && (r2 == 2)) return 0;
	else return -1;
}

int8_t IP2365_init(IP2365_t *d, swi2c_t *pDev) {
	int8_t val;
	d->pD = pDev;
	swi2c_dummy_clock(d->pD);
	//if(swi2c_Read_8addr(d->pD, i2caddr, IP2365_REG_SYSCTL1, &val, 1) != 0) { //getChargeEnable 
	//	d->pD = NULL; val=-1; 
	//} else val = 0;
	val = IP2365_detect(d);
	if(val!=0) d->pD=NULL;
	return val;
}

void ip2365_set_ChargeEnable(IP2365_t *d, uint8_t enable) {
	reg_sysctl1_t r;
	if(d->pD == NULL) return;
	r = (reg_sysctl1_t)IP2365_readReg(d, IP2365_REG_SYSCTL1);
	r.charge_enable = (enable) ? 1 : 0;
	IP2365_writeRegP(d, IP2365_REG_SYSCTL1, (uint8_t*)&r);
}

uint8_t ip2365_get_ChargeEnable(IP2365_t *d) {
	reg_sysctl1_t r;
	if(d->pD == NULL) return 255;
	r = (reg_sysctl1_t)IP2365_readReg(d, IP2365_REG_SYSCTL1);
	return r.charge_enable;
}

uint16_t ip2365_get_VinOVmode(IP2365_t *d) {
	reg_vinovset_t r;
	if(d->pD == NULL) return 65535;
	r = (reg_vinovset_t)IP2365_readReg(d, IP2365_REG_VINOVSET);
	return r.unuse_vinsen;
}

void ip2365_set_VinOVmode(IP2365_t *d, uint8_t enable) {
	reg_vinovset_t r;
	if(d->pD == NULL) return;
	r = (reg_vinovset_t)IP2365_readReg(d, IP2365_REG_VINOVSET);
	r.unuse_vinsen = (enable==0)?0:1;
	IP2365_writeRegP(d, IP2365_REG_VINOVSET, (uint8_t*)&r);
}

uint16_t ip2365_get_VinOV_mV(IP2365_t *d) {
	reg_vinovset_t r;
	if(d->pD == NULL) return 65535;
	r = (reg_vinovset_t)IP2365_readReg(d, IP2365_REG_VINOVSET);
	//uint8_t val = r.winsen;
	if(r.unuse_vinsen) { return 27000; }
	else {
		switch(r.winsen) {
			case 0: return 3000; break;
			case 1: return 2800; break;
			case 2: return 2600; break;
			case 3: return 2200; break;
		}
	}
	return 0;
}

void ip2365_set_VinOV_mV(IP2365_t *d, uint16_t val) {
	reg_vinovset_t r;
	r = (reg_vinovset_t)IP2365_readReg(d, IP2365_REG_VINOVSET);
	if(d->pD == NULL) return;
	r.unuse_vinsen = 0;
	if(val < 2600) r.winsen = 3;
	else if(val >= 2600 && val < 2800) r.winsen = 2;
	else if(val >= 2800 && val < 3000) r.winsen = 1;
	else r.winsen = 0;
	if(val >= 27000) r.unuse_vinsen = 1;
	IP2365_writeRegP(d, IP2365_REG_VINOVSET, (uint8_t*)&r);
}

uint16_t ip2365_get_NTCthres_mV(IP2365_t *d, uint8_t level) {
	reg_ntc_ctl_t r;
	if(d->pD == NULL) return 65535;
	r = (reg_ntc_ctl_t)IP2365_readReg(d, IP2365_REG_NTC_CTL);
	//uint8_t res = IP2365_readReg(d, IP2365_REG_NTC_CTL);
	//if((res & 1)==0) return 0;
	if(r.ntc_enable==0) return 0;
	if(level==0) {
		switch(r.ntc_vol_low) {
			case 0: return 1520; break;
			case 1: return 1490; break;
			case 2: return 1440; break;
			case 3: return 1320; break;
		}
	} else if(level==1) {
		switch(r.ntc_vol_mid) {
			case 0: return 600; break;
			case 1: return 580; break;
			case 2: return 560; break;
			case 3: return 540; break;
		}
	} else if(level==2) {
		switch(r.ntc_vol_high) {
			case 0: return 560; break;
			case 1: return 490; break;
			case 2: return 430; break;
			case 3: return 380; break;
		}
	}
	return 0;
}

void ip2365_set_NTCthres_mV(IP2365_t *d, uint8_t level, uint16_t val) {
	reg_ntc_ctl_t r;
	if(d->pD == NULL) return;
	r = (reg_ntc_ctl_t)IP2365_readReg(d, IP2365_REG_NTC_CTL);
	//uint8_t res = IP2365_readReg(d, IP2365_REG_NTC_CTL);
	//if(val==0) { res &= ~1; IP2365_writeReg(d, IP2365_REG_NTC_CTL, res); }
	if(val==0) { r.ntc_enable=0; level=3; } else { r.ntc_enable = 1; }
	if(level==0) {		
		if(val <= 1320) r.ntc_vol_low = 3;
		else if(val > 1320 && val <= 1440) r.ntc_vol_low = 2;
		else if(val > 1440 && val <= 1490) r.ntc_vol_low = 1;
		else r.ntc_vol_low = 0;
	} else if(level==1) {
		if(val <= 540) r.ntc_vol_mid = 3;
		else if(val > 540 && val <= 560) r.ntc_vol_mid = 2;
		else if(val > 560 && val <= 580) r.ntc_vol_mid = 1;
		else r.ntc_vol_mid = 0;
	} else if(level==2) {
		if(val <= 380) r.ntc_vol_high = 3;
		else if(val > 380 && val <= 430) r.ntc_vol_high = 2;
		else if(val > 430 && val <= 490) r.ntc_vol_high = 1;
		else r.ntc_vol_high = 0;
	}
	//printf("vertify:%02x\n", *(uint8_t*)&r);
	IP2365_writeRegP(d, IP2365_REG_NTC_CTL, (uint8_t*)&r);
}

uint16_t ip2365_get_Isetvin_mA(IP2365_t *d) {
	reg_r_iset_t e;
	reg_vin_iset_t r;
	if(d->pD == NULL) return 255;
	e = (reg_r_iset_t)IP2365_readReg(d, IP2365_REG_R_ISET);
	r = (reg_vin_iset_t)IP2365_readReg(d, IP2365_REG_VIN_ISET);
	if(e.use_r_iadp == 1) return 65535;
	return CUR_BASE + (r.iset_vin * CUR_MUL);
}

uint16_t ip2365_get_Isetbat_mA(IP2365_t *d) {
	reg_r_iset_t e;
	reg_bat_iset_t r;
	if(d->pD == NULL) return 255;
	e = (reg_r_iset_t)IP2365_readReg(d, IP2365_REG_R_ISET);
	r = (reg_bat_iset_t)IP2365_readReg(d, IP2365_REG_BAT_ISET);
	if(e.use_r_ichg == 1) return 65535;
	return CUR_BASE + (r.iset_bat * CUR_MUL);
}

void ip2365_set_Isetvin_mA(IP2365_t *d, uint16_t val) {
	reg_r_iset_t e;
	reg_vin_iset_t r;
	if(d->pD == NULL) return;
	e = (reg_r_iset_t)IP2365_readReg(d, IP2365_REG_R_ISET);
	r = (reg_vin_iset_t)IP2365_readReg(d, IP2365_REG_VIN_ISET);
	e.use_r_iadp = (val < 50) ? 1 : 0;
	if(val < CUR_BASE) val = CUR_BASE;
	if(val > 6350) val = 6350;
	r.iset_vin = (val - CUR_BASE) / CUR_MUL;
	IP2365_writeRegP(d, IP2365_REG_VIN_ISET, (uint8_t*)&r);
	IP2365_writeRegP(d, IP2365_REG_R_ISET, (uint8_t*)&e);
}

void ip2365_set_Isetbat_mA(IP2365_t *d, uint16_t val) {
	reg_r_iset_t e;
	reg_bat_iset_t r;
	if(d->pD == NULL) return;
	e = (reg_r_iset_t)IP2365_readReg(d, IP2365_REG_R_ISET);
	r = (reg_bat_iset_t)IP2365_readReg(d, IP2365_REG_BAT_ISET);
	e.use_r_ichg = (val < 50) ? 1 : 0;
	if(val < CUR_BASE) val = CUR_MUL;
	if(val > 6350) val = 6350;
	r.iset_bat = (val - CUR_BASE) / CUR_MUL;
	IP2365_writeRegP(d, IP2365_REG_BAT_ISET, (uint8_t*)&r);
	IP2365_writeRegP(d, IP2365_REG_R_ISET, (uint8_t*)&e);
}

uint8_t ip2365_get_LED_stat_chgfull(IP2365_t *d) {
	reg_led_t r;
	if(d->pD == NULL) return 255;
	r = (reg_led_t)IP2365_readReg(d, IP2365_REG_LED);
	return r.stat_chgful;
}

uint8_t ip2365_get_LED_stat_chging(IP2365_t *d) {
	reg_led_t r;
	if(d->pD == NULL) return 255;
	r = (reg_led_t)IP2365_readReg(d, IP2365_REG_LED);
	return r.stat_chging;
}

//void ip2365_set_LED_stat_chging(IP2365_t *d, uint8_t stat) {
//	reg_led_t r;
//	if(d->pD == NULL) return;
//	r = (reg_led_t)IP2365_readReg(d, IP2365_REG_LED);
//	r.stat_chging = stat & 3;
//	IP2365_writeRegP(d, IP2365_REG_LED, (uint8_t*)&r);
//}

uint8_t ip2365_get_chg_stat_full(IP2365_t *d) {
	reg_chg_full_t r;
	if(d->pD == NULL) return 255;
	r = (reg_chg_full_t)IP2365_readReg(d, IP2365_REG_CHG_FULL);
	return r.stat_charge_full;
}

uint8_t ip2365_get_chg_stat_cicv(IP2365_t *d) {
	reg_chg_full_t r;
	if(d->pD == NULL) return 255;
	r = (reg_chg_full_t)IP2365_readReg(d, IP2365_REG_CHG_FULL);
	return r.stat_charge;
}

uint8_t ip2365_get_vin_isov(IP2365_t *d) {
	reg_ntc_stat_t r;
	if(d->pD == NULL) return 255;
	r = (reg_ntc_stat_t)IP2365_readReg(d, IP2365_REG_NTC_STAT);
	return r.vin_ov;
}

uint8_t ip2365_get_ntc_stat(IP2365_t *d) {
	reg_ntc_stat_t r;
	if(d->pD == NULL) return 255;
	r = (reg_ntc_stat_t)IP2365_readReg(d, IP2365_REG_NTC_STAT);
	return r.stat_ntc;
}

//only work in my board..
void ip2365_enable(IP2365_t *d, uint8_t enable) {
	if(enable) {
    ip2365_set_VinOV_mV(d, 0);
    ip2365_set_VinOVmode(d, 1);
    ip2365_set_ChargeEnable(d, 1);
	} else {
    ip2365_set_VinOV_mV(d, 3000);
    ip2365_set_VinOVmode(d, 0);
    ip2365_set_ChargeEnable(d, 0);
	}
}
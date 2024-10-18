#ifndef __INCL_IP2365_H__
#define __INCL_IP2365_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "swi2c.h"
#define i2caddr	0x75

#define IP2365_REG_SYSCTL1		0x1
#define IP2365_REG_VINOVSET		0x4
#define IP2365_REG_NTC_CTL		0x5
#define IP2365_REG_R_ISET		0xf
#define IP2365_REG_VIN_ISET		0x19
#define IP2365_REG_LED     		0x1a
#define IP2365_REG_BAT_ISET		0x1f
#define IP2365_REG_NTC_STAT		0x22
#define IP2365_REG_CHG_FULL		0x2b

typedef union { //0x01
	struct {
	uint8_t charge_enable : 1;
	uint8_t unused : 7;
	} __attribute__((packed));
	uint8_t reg_sysctl1;
} reg_sysctl1_t;

typedef union { //0x04
	struct {
	uint8_t unused1 : 1;
	uint8_t unuse_vinsen : 1;
	uint8_t unused2 : 1;
	uint8_t winsen : 2;
	uint8_t unused3 : 3;
	} __attribute__((packed));
	uint8_t reg_vinov_set;
} reg_vinovset_t;

typedef union { //0x05
	struct {
	uint8_t ntc_enable : 1;
	uint8_t ntc_vol_low : 2;
	uint8_t ntc_vol_mid : 2;
	uint8_t ntc_vol_high : 2;
	uint8_t unused1 : 1;
	} __attribute__((packed));
	uint8_t reg_ntc_ctl;
} reg_ntc_ctl_t;

typedef union { //0x0f
	struct {
	uint8_t noused1 : 2;
	uint8_t use_r_iadp : 1;
	uint8_t use_r_ichg : 1;
	uint8_t noused2 : 4;
	} __attribute__((packed));
	uint8_t reg_r_iset;
} reg_r_iset_t;

typedef union { //0x19
	struct {
	uint8_t iset_vin : 6;
	uint8_t unsed1 : 2;
	} __attribute__((packed));
	uint8_t reg_vin_iset;
} reg_vin_iset_t;

typedef union { //0x1a
	struct {
	uint8_t unused1 : 1;
	uint8_t stat_chgful : 1;
	uint8_t unused2 : 2;
	uint8_t stat_chging : 2;
	uint8_t unused3 : 2;
	} __attribute__((packed));
	uint8_t reg_led;
} reg_led_t;

typedef union { //0x1f
	struct {
	uint8_t iset_bat : 6;
	uint8_t unsed1 : 2;
	} __attribute__((packed));
	uint8_t reg_bat_iset;
} reg_bat_iset_t;

typedef union { //0x22
	struct {
	uint8_t vin_ov : 1;
	uint8_t unused1 : 3;
	uint8_t stat_ntc : 3;
	uint8_t unused2 : 1;
	} __attribute__((packed));
	uint8_t reg_ntc_stat;
} reg_ntc_stat_t;

typedef union { //0x2b
	struct {
	uint8_t stat_charge : 3;
	uint8_t unused1 : 2;
	uint8_t stat_charge_full : 1;
	uint8_t unused2 : 2;
	} __attribute__((packed));
	uint8_t reg_chg_full;
} reg_chg_full_t;

typedef struct t_IP2365 {
	swi2c_t	*pD;
} IP2365_t;

int8_t IP2365_init(IP2365_t *d, swi2c_t *pDev);
uint8_t IP2365_readReg(IP2365_t *d, uint8_t reg);
void IP2365_readRegP(IP2365_t *d, uint8_t reg, uint8_t *val);
void IP2365_writeReg(IP2365_t *d, uint8_t reg, uint8_t val);
void IP2365_writeRegP(IP2365_t *d, uint8_t reg, uint8_t *val);

uint8_t ip2365_get_ChargeEnable(IP2365_t *d);
void ip2365_set_ChargeEnable(IP2365_t *d, uint8_t enable);
uint16_t ip2365_get_VinOVmode(IP2365_t *d); //0=using VinSn, 1=not
void ip2365_set_VinOVmode(IP2365_t *d, uint8_t enable); //0=using VinSn, 1=not
uint16_t ip2365_get_VinOV_mV(IP2365_t *d);
void ip2365_set_VinOV_mV(IP2365_t *d, uint16_t val);
uint16_t ip2365_get_Isetbat_mA(IP2365_t *d);
void ip2365_set_Isetbat_mA(IP2365_t *d, uint16_t val); //val=50-6350(mA), disable Iset when val < 50
uint16_t ip2365_get_Isetvin_mA(IP2365_t *d);
void ip2365_set_Isetvin_mA(IP2365_t *d, uint16_t val); //val=50-6350(mA), disable Iset when val < 50
uint16_t ip2365_get_NTCthres_mV(IP2365_t *d, uint8_t level); //level:0=lowTemp, 1=MidTemp, 2=HighTemp,return=0 for disable.
void ip2365_set_NTCthres_mV(IP2365_t *d, uint8_t level, uint16_t val); //level:0=lowTemp, 1=MidTemp, 2=HighTemp,return=0 for disable.
uint8_t ip2365_get_LED_stat_chgfull(IP2365_t *d); //0=off,1=on
uint8_t ip2365_get_LED_stat_chging(IP2365_t *d); //0&1=off,2=blink,3=light
//void ip2365_set_LED_stat_chging(IP2365_t *d, uint8_t stat); 
uint8_t ip2365_get_chg_stat_full(IP2365_t *d); //1=full
uint8_t ip2365_get_chg_stat_cicv(IP2365_t *d); //0=trickle,1=CC,3=CV
uint8_t ip2365_get_vin_isov(IP2365_t *d);//1=Vin over voltage.
uint8_t ip2365_get_ntc_stat(IP2365_t *d);//NTC temp.0=high,4=mid,6=normal,7=low
int8_t IP2365_detect(IP2365_t *d);
void ip2365_enable(IP2365_t *d, uint8_t enable);
#ifdef __cplusplus
}
#endif
#endif

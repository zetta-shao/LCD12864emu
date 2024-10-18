/*
 * sw3518_reg.h
 *
 *  Created on: Mar 8, 2024
 *      Author: zetta
 *      all code base on https://github.com/happyme531/h1_SW35xx
 */

#ifndef INC_SW3518_REG_H_
#define INC_SW3518_REG_H_
#ifdef __cplusplus
extern "C" {
#endif
typedef union { //0x01
	struct {
	uint8_t version : 3;
	uint8_t unused : 5;
	} __attribute__((packed));
	uint8_t reg_ic_version;
} reg_ic_version;

typedef union { //0x06
	struct {
	uint8_t fcx_ind : 4;
	uint8_t pd_src_spec_ver : 2;
	uint8_t unused : 2;
	} __attribute__((packed));
	uint8_t reg_fcx_status;
} reg_fcx_status;

typedef union { //0x07
	struct {
	uint8_t ctrl_1port_on : 1;
	uint8_t ctrl_2port_on : 1;
	uint8_t ctrl_buck_on : 1;
	uint8_t unused : 5;
	} __attribute__((packed));
	uint8_t reg_pwr_status;
} reg_pwr_status;

typedef union { //0x12
	struct {
	uint8_t unused1 : 5;
	uint8_t i2c_pre_enable1 : 1;
	uint8_t i2c_pre_enable2 : 1;
	uint8_t i2c_pre_enable : 1;
	} __attribute__((packed));
	uint8_t reg_i2c_enable;
} reg_i2c_enable;

typedef union { //0x13
	struct {
	uint8_t reg_pd_src_cap_change : 1;
	uint8_t reg_adc_vin_enable : 1;
	uint8_t unused1 : 6;
	} __attribute__((packed));
	uint8_t reg_i2c_ctrl;
} reg_i2c_ctrl;

typedef union { //0x30
	struct {
	uint8_t vin_data : 8; //10mV / LSB
	} __attribute__((packed));
	uint8_t reg_adc_vin_h;
} reg_adc_vin_h;

typedef union { //0x31
	struct {
	uint8_t vin_data : 8; //6mV / LSB
	} __attribute__((packed));
	uint8_t reg_adc_vout_h;
} reg_adc_vout_h;

typedef union { //0x32
	struct {
	uint8_t vout_data : 4;
	uint8_t vin_data : 4; //10mV / LSB
	} __attribute__((packed));
	uint8_t reg_adc_vin_vout_l;
} reg_adc_vin_vout_l;

typedef union { //0x33
	struct {
	uint8_t vin_data : 8; //2.5mA / LSB
	} __attribute__((packed));
	uint8_t reg_adc_iout1_h;
} reg_adc_iout1_h;

typedef union { //0x34
	struct {
	uint8_t vin_data : 8; //2.5mA / LSB
	} __attribute__((packed));
	uint8_t reg_adc_iout2_h;
} reg_adc_iout2_h;

typedef union { //0x35
	struct {
	uint8_t iout2_data : 4;
	uint8_t iout1_data : 4; //10mV / LSB
	} __attribute__((packed));
	uint8_t reg_adc_iout1_iout2_l;
} reg_adc_iout1_iout2_l;

typedef union { //0x37
	struct {
	uint8_t ts_data : 8;
	} __attribute__((packed));
	uint8_t reg_adc_ts_h;
} reg_adc_ts_h;

typedef union { //0x38
	struct {
	uint8_t ts_data : 4;
	uint8_t unused1 : 4;
	} __attribute__((packed));
	uint8_t reg_adc_ts_l;
} reg_adc_ts_l;

typedef union { //0x3a
	struct {
	uint8_t data_type : 3;
	uint8_t unused1 : 5;
	} __attribute__((packed));
	uint8_t reg_adc_data_type_h;
} reg_adc_data_type_h;

typedef union { //0x3b
	struct {
	uint8_t ts_data : 8;
	} __attribute__((packed));
	uint8_t reg_adc_data_buf_h;
} reg_adc_data_buf_h;

typedef union { //0x3c
	struct {
	uint8_t ts_data : 4;
	uint8_t unused1 : 4;
	} __attribute__((packed));
	uint8_t reg_adc_data_buf_l;
} reg_adc_data_buf_l;

typedef union { //0x70
	struct {
	uint8_t pd_req_cmd : 4; // 1:hard-reset, other:reversed.
	uint8_t unused1 : 3;
	uint8_t pd_src_req_vaild : 1; //send PD command by reg@0x70[0:3] wrote.
	} __attribute__((packed));
	uint8_t reg_pd_src_req;
} reg_pd_src_req;

/* 	Vin			Vout	18w					24w					36w					60w
 * 						Imax	spec		Imax	spec		Imax	spec		Imax	spec
 * Vout<=7		  5v	3.2a	5v/3a		3.2a	5v/3a		3.2a	5v/3a		3.2a	5v/3a
 * 7<Vout<=10	  9v	2.2a	9v/2a		3.2a	9v/2.6a		3.2a	9v/3a		3.2a	9v/3a
 * 10<Vout<=16	 12v	1.7a	12v/1.5a	2.2a	12v/2a		3.2a	12v/3a		3.2a	12v/3a
 * Vout>16		 20v	1.4a	20v/0.9a	1.4a	20v/1.2a	2.2a	20v/1.8a	3.2a	20v/3a
 */
typedef union { //0xa6
	struct {
	uint8_t pwr_icc : 2; //0:18w 1:24w 2:36/45w 3:60w
	uint8_t unused1 : 6; //determined by OTP
	} __attribute__((packed));
	uint8_t reg_pwr_conf;
} reg_pwr_conf;

typedef union { //0xaa
	struct {
	uint8_t unused1 : 6; //determined by OTP
	uint8_t	qc3_enable : 1;
	uint8_t	unused2 : 1; //determined by OTP
	} __attribute__((packed));
	uint8_t reg_qc_conf0;
} reg_qc_conf0;

typedef union { //0xaf
	struct {
	uint8_t	vid_h : 8; //PD certification VID, determined by OTP
	} __attribute__((packed));
	uint8_t reg_vid_conf0;
} reg_vid_conf0;

typedef union { //0xb0
	struct {
	uint8_t pd_src_3p0_enable : 1;
	uint8_t pd_src_emk_enable : 1;
	uint8_t pd_src_f09v_enable : 1;
	uint8_t pd_src_f12v_enable : 1;
	uint8_t pd_src_f15v_enable : 1;
	uint8_t pd_src_f20v_enable : 1;
	uint8_t pd_src_pps0_enable : 1;
	uint8_t pd_src_pps1_enable : 1;
	} __attribute__((packed));
	uint8_t reg_pro_conf;
} reg_pro_conf;
#ifdef __cplusplus
}
#endif
#endif /* INC_SW3518_REG_H_ */

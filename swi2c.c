// clone from https://github.com/liyanboy74/soft-i2c
#include <stdlib.h>
#include "swi2c.h"

#ifndef TRUE
	#define TRUE 1
#endif
#ifndef FALSE
	#define FALSE 0
#endif

//swi2c_t *__4delay__ = NULL;
//void __HAL_init_i2c__(swi2c_t *d, swgpio_t *CLK, swgpio_t *DATA);

// sw_i2c 初始化
//void swi2c_initial(swi2c_t *d) { if (!d) return; d->hal_init(d); }

//void swi2c_SWinit(swi2c_t *d, void* SCLport, uint16_t SCLpin, void* SDAport, uint16_t SDApin) {
void swi2c_SWinit(swi2c_t *d, swgpio_t *CLK, swgpio_t *DATA) {
	if(!d) return; 
   	d->SDA.port = DATA->port; d->SDA.pin = DATA->pin;
   	d->SCL.port = CLK->port; d->SCL.pin = CLK->pin;
    d->ptSDA = &d->SDA; d->ptSCL = &d->SCL;
    d->Delay_Time = d->Delay_Time;
    swi2c_port_initial(d);
}

int8_t swi2c_HWinit(swi2c_t *d, void *hWND) {
	//swi2c_SWinit(d, NULL, (swgpio_t *)hWND);
	if(!d) return 0;
    d->SDA.port = hWND; d->SDA.pin = 65535;
    d->SCL.port = NULL; d->SCL.pin = 65535;    
    d->ptSDA = &d->SDA; d->ptSCL = &d->SCL;
    d->Delay_Time = 0; //mean h/w i2c.
    return swi2c_hal_hwinit(d);
}

//static void sda_out(swi2c_t *d, uint8_t out) {
//    swi2c_hal_gpio_out(d->ptSDA, out);
//}

static void i2c_clk_data_out(swi2c_t *d) {
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_HIGH, d);
    swi2c_hal_gpio_out(d->ptSCL, 1);
    swi2c_delay_us(d->Delay_Time);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_LOW, d);
    swi2c_hal_gpio_out(d->ptSCL, 0);
}

void swi2c_port_initial(swi2c_t *d) {
    swi2c_hal_gpio_mode(d->ptSCL, 1);
    swi2c_hal_gpio_mode(d->ptSDA, 1);
    swi2c_hal_gpio_out(d->ptSCL, 1);
    swi2c_hal_gpio_out(d->ptSDA, 1);
}


//static uint8_t swi2c_ReadVal_SDA(swi2c_t *d) {
//    return swi2c_hal_gpio_in(d->ptSDA);
//}

#if 0
static uint8_t swi2c_ReadVal_SCL(swi2c_t *d) {
    return d->hal_io_ctl(IOCTL_SWI2C_GET_SCL_LEVEL, d);
}
#endif

static void i2c_start_condition(swi2c_t *d) {
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_HIGH, d);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_HIGH, d);
    swi2c_hal_gpio_out(d->ptSDA, 1);
    swi2c_hal_gpio_out(d->ptSCL, 1);
    swi2c_delay_us(d->Delay_Time);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_LOW, d);
    swi2c_hal_gpio_out(d->ptSDA, 0);
    swi2c_delay_us(d->Delay_Time);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_LOW, d);
    swi2c_hal_gpio_out(d->ptSCL, 1);
    swi2c_delay_us(d->Delay_Time << 1);
}

static void i2c_stop_condition(swi2c_t *d) {
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_LOW, d);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_HIGH, d);
    swi2c_hal_gpio_out(d->ptSDA, 0);
    swi2c_hal_gpio_out(d->ptSCL, 1);
    swi2c_delay_us(d->Delay_Time);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_HIGH, d);
    swi2c_hal_gpio_out(d->ptSDA, 1);
    swi2c_delay_us(d->Delay_Time);
}

static uint8_t i2c_check_ack(swi2c_t *d) {
    uint8_t ack=0;
    uint8_t i;
    //unsigned int temp;
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_INPUT, d);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_HIGH, d);
    swi2c_hal_gpio_mode(&d->SDA, 0); //input
    swi2c_hal_gpio_mode(&d->SCL, 1); //output
    //ack = 0;
    swi2c_delay_us(d->Delay_Time);
    for (i = 10; i > 0; i--) {
        if(swi2c_hal_gpio_in(&d->SDA) == 0)
        	{ ack = 1; break; }
    }
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_LOW, d);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_OUTPUT, d);
    swi2c_hal_gpio_out(&d->SCL, 0);
    swi2c_hal_gpio_mode(&d->SDA, 1); //output
    swi2c_delay_us(d->Delay_Time);
    return ack;
}

static void i2c_check_not_ack(swi2c_t *d) {
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_INPUT, d);
    swi2c_hal_gpio_mode(d->ptSDA, 0);
    i2c_clk_data_out(d);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_OUTPUT, d);
    swi2c_hal_gpio_mode(d->ptSDA, 1);
    swi2c_delay_us(d->Delay_Time);
}

static void i2c_slave_address(swi2c_t *d, uint8_t IICID, uint8_t readwrite) {
    uint8_t x;
	uint8_t adr8 = IICID << 1;
    if (readwrite) { adr8 |= I2C_READ; }
    else { adr8 &= ~I2C_READ; }
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_LOW, d);
    swi2c_hal_gpio_out(d->ptSCL, 0);
    for(x=0x80; x!=0; x>>=1) {
    	//sda_out(d, adr8 & x);
        swi2c_hal_gpio_out(d->ptSDA, adr8 & x);
        swi2c_delay_us(d->Delay_Time);
        i2c_clk_data_out(d);
    }
}

static void i2c_register_address(swi2c_t *d, uint8_t addr) {
	uint8_t x;
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_LOW, d);
    swi2c_hal_gpio_out(d->ptSCL, 0);
    for(x=0x80; x!=0; x>>=1) {
    	//sda_out(d, addr & x);
        swi2c_hal_gpio_out(d->ptSDA, addr & x);
        swi2c_delay_us(d->Delay_Time);
        i2c_clk_data_out(d);
    }
}

static void i2c_send_ack(swi2c_t *d) {
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_OUTPUT, d);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_LOW, d);
    swi2c_hal_gpio_mode(d->ptSDA, 1);
    swi2c_hal_gpio_out(d->ptSDA, 0);
    swi2c_delay_us(d->Delay_Time);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_HIGH, d);
    swi2c_hal_gpio_out(d->ptSCL, 1);
    swi2c_delay_us(d->Delay_Time << 1);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_LOW, d);
    swi2c_hal_gpio_out(d->ptSDA, 0);
    swi2c_delay_us(d->Delay_Time << 1);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_LOW, d);
    swi2c_hal_gpio_out(d->ptSCL, 1);
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_OUTPUT, d);
    swi2c_delay_us(d->Delay_Time);
}

static void swi2c_Write_Data(swi2c_t *d, uint8_t data) {
	uint8_t idx;
	//d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_LOW, d);
    swi2c_hal_gpio_out(d->ptSCL, 0);
	for(idx=0x80; idx!=0; idx>>=1) {
		//sda_out(d, data & idx);
        swi2c_hal_gpio_out(d->ptSDA, data & idx);
        swi2c_delay_us(d->Delay_Time);
        i2c_clk_data_out(d);
	}
}

static uint8_t swi2c_Read_Data(swi2c_t *d) {
    uint8_t x;
    uint8_t readdata = 0;
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_INPUT, d);
    swi2c_hal_gpio_mode(d->ptSDA, 0);
    for (x=0; x<8; x++) {
        //d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_HIGH, d);
        swi2c_hal_gpio_out(d->ptSCL, 1);
        readdata <<= 1;
        //if(swi2c_ReadVal_SDA(d)) readdata |= 1;
        if(swi2c_hal_gpio_in(d->ptSDA)) readdata |= 1;
        swi2c_delay_us(d->Delay_Time);
        //d->hal_io_ctl(IOCTL_SWI2C_SET_SCL_LOW, d);
        swi2c_hal_gpio_out(d->ptSCL, 0);
        swi2c_delay_us(d->Delay_Time);
    }
    //d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_OUTPUT, d);
    swi2c_hal_gpio_mode(d->ptSDA, 0);
    return readdata;
}

int8_t swi2c_Read_Naddr(swi2c_t *d, uint8_t IICID, uint16_t regaddr, uint8_t addrsize, void *pdata, uint8_t rcnt) {
    int8_t returnack = 0;
    uint8_t index;
    uint8_t *pbData = (uint8_t*)pdata;
    if(d==NULL || !rcnt) return 255;
    //if(!d->SCL.port) {
    if(d->Delay_Time==0) {
    	//hwi2c_t t = { d->SDA.port, IICID, regaddr, addrsize, pdata, rcnt };
    	//returnack = (uint8_t)d->hal_io_ctl(IOCTL_SWI2C_HWI2C_READ, &t);
        returnack = swi2c_hal_receive(d, IICID, regaddr, addrsize, pdata, rcnt);
    } else {
    	uint8_t	*pADR = (uint8_t*)&regaddr;
    swi2c_port_initial(d);

	i2c_start_condition(d);
	//写ID
	i2c_slave_address(d, IICID, WRITE_CMD);
	if (!i2c_check_ack(d)) { i2c_stop_condition(d); return 1; }
	swi2c_delay_us(d->Delay_Time);
	for(index=addrsize; index>0; index--) {
		i2c_register_address(d, (uint8_t)(pADR[index-1]));
		if (!i2c_check_ack(d)) { returnack = 2; break; }
		swi2c_delay_us(d->Delay_Time);
	}

	if (returnack!=0) { i2c_stop_condition(d); return returnack; }

    //重启I2C总线
    i2c_start_condition(d);
    //读ID
    i2c_slave_address(d, IICID, READ_CMD);
    if (!i2c_check_ack(d)) { i2c_stop_condition(d); return 3; }
    //循环读数据
    if(rcnt > 1) {
        for ( index = 0 ; index < (rcnt - 1) ; index++) {
            swi2c_delay_us(d->Delay_Time);
            pbData[index] = swi2c_Read_Data(d);
            i2c_send_ack(d);
        }
    }
    swi2c_delay_us(d->Delay_Time);
    pbData[rcnt-1] = swi2c_Read_Data(d);
    i2c_check_not_ack(d);
    i2c_stop_condition(d);
    }
    return returnack;
}

int8_t swi2c_Read_8addr(swi2c_t *d, uint8_t IICID, uint8_t regaddr, void *pdata, uint8_t rcnt) {
	return swi2c_Read_Naddr(d, IICID, regaddr, 1, pdata, rcnt);
}

int8_t swi2c_Read_16addr(swi2c_t *d, uint8_t IICID, uint16_t regaddr, void *pdata, uint8_t rcnt) {
	return swi2c_Read_Naddr(d, IICID, regaddr, 2, pdata, rcnt);
}

int8_t swi2c_Read_0addr(swi2c_t *d, uint8_t IICID, void *pdata, uint8_t rcnt) {
	return swi2c_Read_Naddr(d, IICID, 0, 0, pdata, rcnt);
}

int8_t swi2c_Write_Naddr(swi2c_t *d, uint8_t IICID, uint16_t regaddr, uint8_t addrsize, void *pdata, uint8_t rcnt) {
    int8_t returnack = 0;
    uint8_t index;
    uint8_t *pbData = (uint8_t*)pdata;
    if(d==NULL || !rcnt) return 255;
    //if(d->SCL.port==NULL) {
    if(d->Delay_Time==0) {
    	//hwi2c_t t = { d->SDA.port, IICID, regaddr, addrsize, pdata, rcnt };
    	//returnack = (uint8_t)d->hal_io_ctl(IOCTL_SWI2C_HWI2C_WRITE, &t);
        returnack = swi2c_hal_transmit(d, IICID, regaddr, addrsize, pdata, rcnt);
    } else {
		uint8_t *pADR = (uint8_t*)&regaddr;
		swi2c_port_initial(d);
		i2c_start_condition(d);
		//写ID
		i2c_slave_address(d, IICID, WRITE_CMD);
		if (!i2c_check_ack(d)) { i2c_stop_condition(d); return 1; }
		swi2c_delay_us(d->Delay_Time);
		for(index=addrsize; index>0; index--) {
			i2c_register_address(d, (uint8_t)(pADR[index-1]));
			if (!i2c_check_ack(d)) { returnack = 2; break; }
			swi2c_delay_us(d->Delay_Time);
		}

		if(returnack!=0) { i2c_stop_condition(d); return returnack; }
		//写数据
		for ( index = 0 ; index < rcnt ; index++) {
			swi2c_Write_Data(d, pbData[index]);
			if (!i2c_check_ack(d)) { returnack = 3; break; }
			swi2c_delay_us(d->Delay_Time);
		}
		i2c_stop_condition(d);
    }
    return returnack;
}

int8_t swi2c_Write_8addr(swi2c_t *d, uint8_t IICID, uint8_t regaddr, void *pdata, uint8_t rcnt) {
	return swi2c_Write_Naddr(d, IICID, regaddr, 1, pdata, rcnt);
}

int8_t swi2c_Write_16addr(swi2c_t *d, uint8_t IICID, uint16_t regaddr, void *pdata, uint8_t rcnt) {
	return swi2c_Write_Naddr(d, IICID, regaddr, 2, pdata, rcnt);
}

int8_t swi2c_Write_0addr(swi2c_t *d, uint8_t IICID, void *pdata, uint8_t rcnt) {
	return swi2c_Write_Naddr(d, IICID, 0, 0, pdata, rcnt);
}
#if 0
int8_t swi2c_Check_SlaveAddr(swi2c_t *d, uint8_t IICID) {
    if(d->Delay_Time == 0) {
        return swi2c_hal_slaveaddr(d, IICID);
    } else {
        uint8_t returnack = TRUE;
        i2c_start_condition(d);
        i2c_slave_address(d, IICID, WRITE_CMD);
        if (!i2c_check_ack(d)) {
            i2c_stop_condition(d);
            returnack = FALSE;
        }
        i2c_stop_condition(d);
        return returnack;
    }
}
#endif
void swi2c_dummy_clock(swi2c_t *d) {
	uint8_t i;
	if(d->Delay_Time == 0) return;	//h/w i2c
	//d->hal_io_ctl(IOCTL_SWI2C_SET_SDA_INPUT, d);
    swi2c_hal_gpio_mode(d->ptSDA, 0);
	for(i=0; i<100; i++) {
		if(swi2c_hal_gpio_in(d->ptSDA) != 0) break;
		i2c_clk_data_out(d);
		swi2c_delay_us(d->Delay_Time);
	}
	swi2c_port_initial(d);
}

//__weak void swi2c_delay_us(uint32_t time) { }
//__weak void swi2c_delay_ms(uint32_t time) { }
void swi2c_i2cfree(swi2c_t *d) { swi2c_hal_i2cfree(d); }
#ifndef _swi2c_H_
#define _swi2c_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "gpiodef.h"
#include <stdint.h>

typedef struct swi2c_t swi2c_t;
//typedef stm32_gpio_t swgpio_t;
#define SWI2C_DELAY_TIME    10 // 10us 100kHz

#define I2C_READ            0x01
#define READ_CMD            1
#define WRITE_CMD           0

typedef enum
{
    IOCTL_SWI2C_SET_SDA_LOW = 0,
    IOCTL_SWI2C_SET_SDA_HIGH,
    IOCTL_SWI2C_SET_SCL_LOW,
    IOCTL_SWI2C_SET_SCL_HIGH,
    IOCTL_SWI2C_SET_SDA_INPUT,
    IOCTL_SWI2C_SET_SDA_OUTPUT,
    IOCTL_SWI2C_SET_SCL_INPUT,
    IOCTL_SWI2C_SET_SCL_OUTPUT,
    IOCTL_SWI2C_GET_SDA_LEVEL,
    IOCTL_SWI2C_GET_SCL_LEVEL,
    IOCTL_SWI2C_SET_GPIO_LOW,
    IOCTL_SWI2C_SET_GPIO_HIGH,
	IOCTL_SWI2C_GET_GPIO_LEVEL,
    IOCTL_SWI2C_SET_GPIO_INPUT,
    IOCTL_SWI2C_SET_GPIO_OUTPUT,
	IOCTL_SWI2C_DELAY_US,
	IOCTL_SWI2C_DELAY_MS,
	IOCTL_SWI2C_HWI2C_READ,
	IOCTL_SWI2C_HWI2C_WRITE,
}hal_io_opt_e;
#if 0
typedef struct hwi2c_stm32 {
	void		*hWND;
	uint8_t		i2cdev_addr;
	uint16_t	i2cmem_addr;
	uint8_t		devaddrsize;
	uint8_t		*data;
	uint16_t	datasize;
	uint8_t		unused1;
} hwi2c_t;
#endif
struct swi2c_t {
    //int (*hal_init)(struct tag_swi2c*);
    int (*hal_io_ctl)(hal_io_opt_e opt, void *arg);
    uint16_t    Delay_Time; //0 when using hwi2c
	swgpio_t	SDA;
	swgpio_t	SCL;
    swgpio_t    *ptSDA;
    swgpio_t    *ptSCL;
};


/* functions */
//void swi2c_initial(swi2c_t *d);
void swi2c_SWinit(swi2c_t *d, swgpio_t *CLK, swgpio_t *DATA);
int8_t swi2c_HWinit(swi2c_t *d, void *hWND);

int8_t swi2c_Read_0addr(swi2c_t *d, uint8_t IICID, void *pdata, uint8_t rcnt);
int8_t swi2c_Read_8addr(swi2c_t *d, uint8_t IICID, uint8_t regaddr, void *pdata, uint8_t rcnt);
int8_t swi2c_Read_16addr(swi2c_t *d, uint8_t IICID, uint16_t regaddr, void *pdata, uint8_t rcnt);
int8_t swi2c_Write_0addr(swi2c_t *d, uint8_t IICID, void *pdata, uint8_t rcnt);
int8_t swi2c_Write_8addr(swi2c_t *d, uint8_t IICID, uint8_t regaddr, void *pdata, uint8_t rcnt);
int8_t swi2c_Write_16addr(swi2c_t *d, uint8_t IICID, uint16_t regaddr, void *pdata, uint8_t rcnt);
//int8_t swi2c_Check_SlaveAddr(swi2c_t *d, uint8_t IICID);
void swi2c_port_initial(swi2c_t *d);
void swi2c_delay_us(uint32_t time);
void swi2c_delay_ms(uint32_t time);
void swi2c_dummy_clock(swi2c_t *d);
void swi2c_setgpo(swi2c_t *d, swgpio_t *gpiogrp, uint8_t val);
uint8_t swi2c_getgpi(swi2c_t *d, swgpio_t *gpiogrp);
void swi2c_i2cfree(swi2c_t *d);

int8_t swi2c_hal_hwinit(swi2c_t *d);
void swi2c_hal_gpio_out(swgpio_t *d, uint8_t val);
uint8_t swi2c_hal_gpio_in(swgpio_t *d);
void swi2c_hal_gpio_mode(swgpio_t *d, uint8_t val); //0=inout
int8_t swi2c_hal_transmit(swi2c_t *d, uint8_t i2caddr, uint32_t memaddr, uint8_t memaddrsize, void *data, uint8_t datasize);
int8_t swi2c_hal_receive(swi2c_t *d, uint8_t i2caddr, uint32_t memaddr, uint8_t memaddrsize, void *data, uint8_t datasize);
//int8_t swi2c_hal_slaveaddr(swi2c_t *d, uint8_t i2caddr); //for check
void swi2c_hal_i2cfree(swi2c_t *d);
#ifdef __cplusplus
}
#endif
#endif  /* __I2C_SW_H */

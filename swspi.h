/**
 * @file    softspi.h
 * @author  Myth
 * @version 0.2
 * @date    2021.10.12
 * @brief   STM32 SoftSPI Library
 */

#ifndef __SOFTSPI_H
#define __SOFTSPI_H

#include <gpiodef.h>

#ifdef __cplusplus
extern "C" {
#endif

//typedef stm32_gpio_t spi_gpio_t;
typedef amd64_gpio_t spi_gpio_t;
#define SWSPI_DELAY 10 //10uS
#define SWSPI_CPHA = 0x1
#define SWSPI_CPOL = 0x2

typedef enum {
    IOCTL_SWSPI_SET_MOSI_LOW = 0,
    IOCTL_SWSPI_SET_MOSI_HIGH,
    IOCTL_SWSPI_SET_CLK_LOW,
    IOCTL_SWSPI_SET_CLK_HIGH,
    IOCTL_SWSPI_GET_MISO_LEVEL,
    IOCTL_SWSPI_SET_GPIO_LOW,
    IOCTL_SWSPI_SET_GPIO_HIGH,
    IOCTL_SWSPI_GET_GPIO_LEVEL,
    IOCTL_SWSPI_SET_GPIO_INPUT,
    IOCTL_SWSPI_SET_GPIO_OUTPUT,
    IOCTL_SWSPI_DELAY_US,
    IOCTL_SWSPI_DELAY_MS,
    IOCTL_SWSPI_HWSPI_READ,
	IOCTL_SWSPI_HWSPI_WRITE,
	IOCTL_SWSPI_HWSPI_RW,
} hal_spiio_opt_e;

typedef struct hwspi_stm32 {
	void		*hWND;
	uint8_t		*bufin;
	uint8_t		*bufout;
	uint16_t	datasize;
} hwspi_t;

#define swspi_cpol_l 0
#define swspi_cpol_h 1
#define swspi_cpha_l 0
#define swspi_cpha_h 1

typedef struct tag_swspi {
    //int (*hal_io_ctl)(hal_spiio_opt_e opt, void *arg);
	uint16_t        Delay_Time;
    uint16_t        bitmask;
    uint8_t         cpol;
    uint8_t         cpha;
    spi_gpio_t      CLK;
    spi_gpio_t      MOSI;
    spi_gpio_t      MISO;
} swspi_t;


/* functions */
void swspi_SWinit(swspi_t *d, spi_gpio_t *clk, spi_gpio_t *mosi, spi_gpio_t *miso);
void swspi_setbits(swspi_t *d, uint8_t val);
void swspi_HWinit(swspi_t *d, void *hWND);
void swspi_read(swspi_t *d, uint8_t *pRead, uint32_t len);
void swspi_write(swspi_t *d, uint8_t *pWrite, uint32_t len);
void swspi_readwrite(swspi_t *d, uint8_t *pWrite, uint8_t *pRead, uint32_t len);
//HAL_StatusTypeDef SoftSPI_Init(SoftSPI_TypeDef *SoftSPIx);
void swspi_setgpo(spi_gpio_t *gpiogrp, uint8_t val);
uint8_t swspi_getgpi(spi_gpio_t *gpiogrp);
#define swspi_delay_us swspi_hal_delay_us
#define swspi_delay_ms swspi_hal_delay_ms

// HAL implements
void swspi_hal_delay_us(uint32_t time);
void swspi_hal_delay_ms(uint32_t time);
void swspi_hal_init(swspi_t *d, spi_gpio_t *clk, spi_gpio_t *mosi, spi_gpio_t *miso);
void swspi_hal_gpio_out(spi_gpio_t *d, uint8_t val);
uint8_t swspi_hal_gpio_in(spi_gpio_t *d);
void swspi_hal_gpio_mode(spi_gpio_t *d, uint8_t val);
int swspi_hal_transmit(void *hWND, uint8_t *pWrite, uint16_t datasize);
int swspi_hal_receive(void *hWND, uint8_t *pRead, uint16_t datasize);
int swspi_hal_transmit_receive(void *hWND, uint8_t *pRead, uint8_t *pWrite, uint16_t datasize);

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file    softspi.c
 * @author  Myth
 * @version 0.2
 * @date    2021.10.12
 * @brief   STM32 SoftSPI Library
 * clone from https://github.com/Mythologyli/SoftSPI_HAL_Lib/tree/master
 */
#include "swspi.h"

// sw_spi 初始化
//void swspi_initial(swspi_t *d) { if (!d) return; d->hal_init(d); }

int swspi_SWinit(swspi_t *d, spi_gpio_t *clk, spi_gpio_t *mosi, spi_gpio_t *miso) {
    d->cpol = 0;
    d->cpha = 0;
    d->Delay_Time = SWSPI_DELAY;
    d->bitmask = 0x80;
    return swspi_hal_init(d, clk, mosi, miso);
}

int swspi_HWinit(swspi_t *d, void *hWND) {
    d->cpol = 0;
    d->cpha = 0;
    d->Delay_Time = 0;
    d->bitmask = 0;
    return swspi_hal_init(d, (spi_gpio_t*)hWND, 0, 0);
}

int swspi_setbits(swspi_t *d, uint8_t val) { 
    if(d->Delay_Time == 0) return swspi_hal_setbits(d, val);
    else {
        if(val>0) d->bitmask = 1 << (val-1); else d->bitmask=0; return 0; }
}

//void swspi_setCLK(swspi_t *d) { d->hal_io_ctl(IOCTL_SWSPI_SET_GPIO_HIGH, &d->CLK); }
//void swspi_clrCLK(swspi_t *d) { d->hal_io_ctl(IOCTL_SWSPI_SET_GPIO_LOW, &d->CLK); }
//void swspi_setMOSI(swspi_t *d) { d->hal_io_ctl(IOCTL_SWSPI_SET_GPIO_HIGH, &d->MOSI); }
//void swspi_clrMOSI(swspi_t *d) { d->hal_io_ctl(IOCTL_SWSPI_SET_GPIO_LOW, &d->MOSI); }
//void delay_def(swspi_t *d) { d->hal_io_ctl(IOCTL_SWSPI_DELAY_US, &d->Delay_Time); }
//uint8_t swspi_getMISO(swspi_t *d) { return d->hal_io_ctl(IOCTL_SWSPI_GET_GPIO_LEVEL, &d->MISO); }

uint16_t swspi_rw2(swspi_t *d, uint16_t word) {
    uint16_t data = 0;
    uint16_t i = d->bitmask;
    for (; i!=0; i>>=1) {
        //swspi_clrCLK(d);
        swspi_hal_gpio_out(&d->CLK, 0);
        //delay_def(d);
        swspi_delay_us(d->Delay_Time);
        //if(word & i) swspi_setMOSI(d);
        //else swspi_clrMOSI(d);
        swspi_hal_gpio_out(&d->MOSI, (word&i)?1:0);
        //delay_def(d);
        swspi_delay_us(d->Delay_Time);
        data <<= 1;
        //swspi_setCLK(d);
        swspi_hal_gpio_out(&d->CLK, 1);
        //delay_def(d);
        swspi_delay_us(d->Delay_Time);
        //data |= swspi_getMISO(d);
        data |= swspi_hal_gpio_in(&d->MISO);
        //delay_def(d);
        swspi_delay_us(d->Delay_Time);
    }
    return data;
}

void __swspi_write(swspi_t *d, uint16_t val) {
    uint16_t i;
    if(!d) return;
    i = d->bitmask;
    for(; i!=0; i>>=1) {
        //swspi_clrCLK(d);
        swspi_hal_gpio_out(&d->CLK, d->cpol ^ 0);
        //delay_def(d);
        swspi_delay_us(d->Delay_Time);
        //if(val & i) swspi_setMOSI(d);
        //else swspi_clrMOSI(d);
        swspi_hal_gpio_out(&d->MOSI, ((val&i)?1:0) ^ d->cpha);
        //delay_def(d);
        swspi_delay_us(d->Delay_Time);
        //swspi_setCLK(d);
        swspi_hal_gpio_out(&d->CLK, 1 ^ d->cpol);
        //delay_def(d);
        swspi_delay_us(d->Delay_Time);
    }
    //swspi_clrMOSI(d);
    swspi_hal_gpio_out(&d->MOSI, 1 ^ d->cpol);
}

uint16_t __swspi_read(swspi_t *d) {
    uint16_t i, res;
    if(!d) return 0;
    i = d->bitmask;
    for(res = 0; i!=0; i>>=1) {
        //swspi_clrCLK(d);
        swspi_hal_gpio_out(&d->CLK, 0);
        //delay_def(d);
        swspi_delay_us(d->Delay_Time);
        res <<= 1;
        //swspi_setCLK(d);
        swspi_hal_gpio_out(&d->CLK, 1);
        //delay_def(d);
        swspi_delay_us(d->Delay_Time);
        //res |= swspi_getMISO(d) ? 1 : 0;
        res |= swspi_hal_gpio_in(&d->MISO);
        //delay_def(d);
        swspi_delay_us(d->Delay_Time);
    }
    return res;
}

void swspi_readwrite(swspi_t *d, uint8_t *pWrite, uint8_t *pRead, uint32_t len) {
    uint8_t j;
    if(d->Delay_Time == 0) {
        //hwspi_t h = { d->CLK.port, pRead, pWrite, len };
        //d->hal_io_ctl(IOCTL_SWSPI_HWSPI_RW, &h);
        swspi_hal_transmit_receive(d->CLK.port, pRead, pWrite, len);
    } else {
        if(d->bitmask > 128) {
            for (j=0; j<len; j++) { pRead[j] = swspi_rw2(d, pWrite[j]); }
        } else {
            uint16_t *t=(uint16_t*)pRead, *s=(uint16_t*)pWrite;
            for (j=0; j<len; j+=2, s++, t++) { *t = swspi_rw2(d, *s); }
        }
    }
}

void swspi_read(swspi_t *d, uint8_t *pRead, uint32_t len) {
    uint8_t j;
    if(d->Delay_Time == 0) { // h/w spi
        //hwspi_t h = { d->CLK.port, pRead, NULL, len };
        //d->hal_io_ctl(IOCTL_SWSPI_HWSPI_READ, &h);
        swspi_hal_receive(d->CLK.port, pRead, len);
    } else if(d->bitmask > 128) {
        uint16_t *t=(uint16_t*)pRead;
        for (j=0; j<len; j+=2, t++) { *t = __swspi_read(d); }
    } else {
        for (j=0; j<len; j++) { pRead[j] = __swspi_read(d); }
    }
}

void swspi_write(swspi_t *d, uint8_t *pWrite, uint32_t len) {
    uint8_t j;
    if(d->Delay_Time == 0) { // h/w spi
        //hwspi_t h;
        //h.hWND=d->CLK.port; h.bufout=pWrite; h.bufin=NULL; h.datasize=len;
        //d->hal_io_ctl(IOCTL_SWSPI_HWSPI_WRITE, &h);
        swspi_hal_transmit(d->CLK.port, pWrite, len);
    } else if(d->bitmask > 128) {
        uint16_t *s=(uint16_t*)pWrite;
        for (j=0; j<len; j++, s++) { __swspi_write(d, *s); }
    } else {
        for (j=0; j<len; j++) { __swspi_write(d, pWrite[j]); }
    }
}

void swspi_setgpo(spi_gpio_t *gpiogrp, uint8_t val) {
    if(!gpiogrp->port || gpiogrp->pin==65535) return;
    swspi_hal_gpio_out(gpiogrp, val);
}

uint8_t swspi_getgpi(spi_gpio_t *gpiogrp) {
    if(!gpiogrp->port || gpiogrp->pin==65535) return 0;
    return swspi_hal_gpio_in(gpiogrp);
}

void swspi_setgpmode(spi_gpio_t *gpiogrp, uint8_t val) { swspi_hal_gpio_mode(gpiogrp, val); }
int swspi_setmode(swspi_t *d, uint8_t val) { return swspi_hal_setmode(d, val); }
int swspi_setspeed(swspi_t *d, uint32_t val) { return swspi_hal_setspeed(d, val); }
void swspi_spifree(swspi_t *d) { swspi_hal_spiclose(d); }

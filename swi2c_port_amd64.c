#include "swi2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>

#define STM32_BLK_DELAY 0x10

void swi2c_delay_us(uint32_t us) { usleep(us); }
void swi2c_delay_ms(uint32_t ms) { usleep(ms*1000); }
#if 0
static int hwi2c_transmit_mem_t(hwi2c_t *d) {
  int res;
	//if(HAL_I2C_IsDeviceReady((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), 2, 2) != HAL_OK) return 8;
#ifdef STM32_USE_IRQ
  if(d->devaddrsize == 0)
    res = HAL_I2C_Master_Transmit_IT((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), d->data, d->datasize);
  else
    res = HAL_I2C_Mem_Write_IT((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), d->i2cmem_addr, d->devaddrsize, d->data, d->datasize);
  swi2c_delay_ms(1);
#else
  if(d->devaddrsize == 0)
    res = HAL_I2C_Master_Transmit((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), d->data, d->datasize, STM32_BLK_DELAY);
  else
    res = HAL_I2C_Mem_Write((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), d->i2cmem_addr, d->devaddrsize, d->data, d->datasize, STM32_BLK_DELAY);
#endif
  return res;
}
#endif

uint32_t swap_u32(uint32_t val) {
  uint32_t t1 = val << 16, t2 = val >> 16;
  t1 >>= 8; t2 <<= 8;
  t1 &= 0x00ff00ff;
  t2 &= 0xff00ff00;
  return t1 | t2;
}

int8_t swi2c_hal_transmit(swi2c_t *d, uint8_t i2caddr, uint32_t memaddr, uint8_t memaddrsize, void *data, uint8_t datasize) {
#ifdef _M_AMD64
  uint8_t i2c_tmp[260];
  int dev, res;
  dev = d->SDA.pin;
  res = ioctl(dev, I2C_SLAVE, i2caddr);
  if(res < 0) return (int8_t)res;
  memcpy(i2c_tmp, &memaddr, memaddrsize);
  memcpy(i2c_tmp+memaddrsize, data, datasize);
  res = write(dev, i2c_tmp, memaddrsize+datasize);

  return (res == datasize) ? 0 : 1;
#else
  void *hWND = d->SDA.port;
#ifdef STM32_USE_IRQ
  if(memaddrsize == 0)
    return HAL_I2C_Master_Transmit_IT(hWND, i2caddr<<1, data, datasize);
  else
    return HAL_I2C_Mem_Write_IT((hWND, i2caddr<<1, memaddr, memaddrsize, data, datasize);
#else
  if(memaddrsize == 0)
    return HAL_I2C_Master_Transmit(hWND, i2caddr<<1, data, datasize, STM32_BLK_DELAY);
  else
    return HAL_I2C_Mem_Write((hWND, i2caddr<<1, memaddr, memaddrsize, data, datasize, STM32_BLK_DELAY);
#endif
#endif
}
#if 0
int8_t swi2c_hal_slaveaddr(swi2c_t *d, uint8_t i2caddr) {
#ifdef _M_AMD64
  int dev, res;
  dev = d->SDA.pin;
  res = ioctl(dev, I2C_SLAVE, i2caddr);
  if(res < 0) return (int8_t)res;
  return 0;
#else
#endif
}
#endif
#if 0
static int hwi2c_receive_mem_t(hwi2c_t *d) {
  int res;
	//if(HAL_I2C_IsDeviceReady((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), 2, 2) != HAL_OK) return 8;
#ifdef STM32_USE_IRQ
  if(d->devaddrsize == 0)
    res = HAL_I2C_Master_Transmit_IT((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), d->data, d->datasize);
  else
    res = HAL_I2C_Mem_Read_IT((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), d->i2cmem_addr, d->devaddrsize, d->data, d->datasize);
  swi2c_delay_ms(1);
#else
  if(d->devaddrsize == 0)
    res= HAL_I2C_Master_Transmit((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), d->data, d->datasize, STM32_BLK_DELAY);
  else
    res = HAL_I2C_Mem_Read((I2C_HandleTypeDef *)d->hWND, (d->i2cdev_addr<<1), d->i2cmem_addr, d->devaddrsize, d->data, d->datasize, STM32_BLK_DELAY);
#endif
  return res;
}
#endif
int8_t swi2c_hal_receive(swi2c_t *d, uint8_t i2caddr, uint32_t memaddr, uint8_t memaddrsize, void *data, uint8_t datasize) {
#ifdef _M_AMD64  
  int dev, res;
  dev = d->SDA.pin;
  res = ioctl(dev, I2C_SLAVE, i2caddr);
  if(res < 0) return (int8_t)res;
  res = write(dev, &memaddr, memaddrsize);
  res = read(dev, data, datasize);
  return (res == datasize) ? 0 : 1;
#else
  void *hWND = d->SDA.port;
#ifdef STM32_USE_IRQ
  if(d->devaddrsize == 0)
    return HAL_I2C_Master_Transmit(hWND, i2caddr<<1, data, datasize);
  else
    return HAL_I2C_Mem_Read(hWND, i2caddr<<1, memaddr, memaddrsize, data, datasize);
#else
  if(memaddrsize == 0)
    return HAL_I2C_Master_Transmit(hWND, i2caddr<<1, data, datasize, STM32_BLK_DELAY);
  else
    return HAL_I2C_Mem_Read(hWND, i2caddr<<1, memaddr, memaddrsize, data, datasize, STM32_BLK_DELAY);
#endif
#endif
}

void swi2c_hal_gpio_out(swgpio_t *d, uint8_t val) { (void)d; (void)val; }
uint8_t swi2c_hal_gpio_in(swgpio_t *d) { (void)d; return 0; }
void swi2c_hal_gpio_mode(swgpio_t *d, uint8_t val) {
  (void)d; (void)val;
}
#if 0
static int swi2c_port_io_ctl(uint8_t opt, void *param) {
    int ret = 0;
    swi2c_t *d=(swi2c_t*)param;
    switch(opt)
    {
    case IOCTL_SWI2C_SET_SDA_HIGH: swi2c_hal_gpio_out(d->ptSDA, 1); break;
    case IOCTL_SWI2C_SET_SDA_LOW: swi2c_hal_gpio_out(d->ptSDA, 0); break;
    case IOCTL_SWI2C_GET_SDA_LEVEL: ret=swi2c_hal_gpio_in(d->ptSDA); break;
    case IOCTL_SWI2C_SET_SDA_INPUT: swi2c_hal_gpio_mode(d->ptSDA, 0); break;
    case IOCTL_SWI2C_SET_SDA_OUTPUT: swi2c_hal_gpio_mode(d->ptSDA, 1); break;

    case IOCTL_SWI2C_SET_SCL_HIGH: swi2c_hal_gpio_out(d->ptSCL, 1); break;
    case IOCTL_SWI2C_SET_SCL_LOW: swi2c_hal_gpio_out(d->ptSCL, 0); break;
    case IOCTL_SWI2C_GET_SCL_LEVEL: ret=swi2c_hal_gpio_in(d->ptSCL); break;
    case IOCTL_SWI2C_SET_SCL_INPUT: swi2c_hal_gpio_mode(d->ptSCL, 0); break;
    case IOCTL_SWI2C_SET_SCL_OUTPUT: swi2c_hal_gpio_mode(d->ptSCL, 1); break;

    case IOCTL_SWI2C_SET_GPIO_LOW: break;
    case IOCTL_SWI2C_SET_GPIO_HIGH: break;
    case IOCTL_SWI2C_GET_GPIO_LEVEL: ret=0; break;
    case IOCTL_SWI2C_SET_GPIO_INPUT: break;
    case IOCTL_SWI2C_SET_GPIO_OUTPUT: break;
    case IOCTL_SWI2C_DELAY_US: swi2c_delay_us(*(uint32_t*)param); break;
    case IOCTL_SWI2C_DELAY_MS: swi2c_delay_ms(*(uint32_t*)param); break;
    case IOCTL_SWI2C_HWI2C_READ: ret = hwi2c_receive_mem_t((hwi2c_t*)param); break;
    case IOCTL_SWI2C_HWI2C_WRITE: ret = hwi2c_transmit_mem_t((hwi2c_t*)param); break;
    default:
        break;
    }
    return ret;
}
#endif
int8_t swi2c_hal_hwinit(swi2c_t *d) {
#ifdef _M_AMD64
  int dev;
  if(d->Delay_Time != 0) return 0; //not h/w i2c
  dev = open(d->SDA.port, O_RDWR);
  if(dev < 0) { d->SDA.pin = 0; d->SDA.port = 0; return 255; }
  d->SDA.pin = (unsigned long)dev;
  return 0;
#else
  return 0;
#endif
}

void swi2c_hal_i2cfree(swi2c_t *d) {
#ifdef _M_AMD64
    close(d->SDA.pin);
    d->SDA.port = NULL; d->SDA.pin = 0;
#endif
}



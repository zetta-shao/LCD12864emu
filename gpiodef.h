/*
 * gpiodef.h
 *
 *  Created on: Feb 18, 2024
 *      Author: zetta
 */

#ifndef INC_GPIODEF_H_
#define INC_GPIODEF_H_

// #include "stm32g0xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define HI2C1L		GPIO_PIN_8	//1
#define HI2C1A		GPIO_PIN_9	//2
#define HI2C1P		GPIOB
#define HI2C2L		GPIO_PIN_11	//16 //PA11/PA9
#define HI2C2A		GPIO_PIN_12	//17 //PA12/PA10
#define HI2C2P		GPIOA
#define SPI1L		GPIO_PIN_5	//12
#define SPI1O		GPIO_PIN_7	//14
#define SPI1P		GPIOA
#define TIM3C1		GPIO_PIN_3	//20 //PB3/PB4/PB5/PB6
#define TIM3P		GPIOB
#define SI2C2A		GPIO_PIN_0	//7
#define SI2C2L		GPIO_PIN_1	//8
#define SI2C2P		GPIOA
#define SI2C1A		GPIO_PIN_2	//9
#define SI2C1L		GPIO_PIN_3	//10
#define SI2C1P		GPIOA
#define BTN1		GPIO_PIN_13	//18
#define BTN2		GPIO_PIN_14	//19 //PA14/PA15
#define BTNP		GPIOA
#define LCD_CS		GPIO_PIN_4	//11
#define LCD_CS_P	GPIOA	//11
#define LCD_CD		GPIO_PIN_6	//13 //noused
#define LCD_CD_P	GPIOA	//13
#define LCD_RST		GPIO_PIN_8	//15 //PB0/PB1/PB2/PA8 noused
#define LCD_RST_P	GPIOA	//15
#define EVB_LED		GPIO_PIN_8	//15
#define EVB_LED2	GPIO_PIN_4	//11
#define EVB_LED_P	GPIOA	//15
#define SPI3CLK		GPIO_PIN_6
#define SPI3CLKP	GPIOA
#define SPI3MOSI	GPIO_PIN_8
#define SPI3MOP		GPIOA
#define SPI3SS		GPIO_PIN_3	//20
#define SPI3SSP		GPIOB
#define LCD_RS		GPIO_PIN_15	//20
#define LCD_RSP		GPIOC

//#define SSD1306_DC_GPIO_Port	LCD_CD_P
//#define SSD1306_DC_Pin			LCD_CD
//#define SSD1306_CS_GPIO_Port	LCD_CS_P
//#define SSD1306_CS_Pin			LCD_CS
//#define SSD1306_RESET_GPIO_Port	LCD_RST_P
//#define SSD1306_RESET_Pin		LCD_RST
#define SCALE_W_ 8
#define SCALE_H_ 8
typedef void* I2C_HandleTypeDef;
typedef void* SPI_HandleTypeDef;
typedef void* TIM_HandleTypeDef;
typedef void* ADC_HandleTypeDef;

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim14;
extern ADC_HandleTypeDef hadc1;

/* Includes ------------------------------------------------------------------*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef*);
void Error_Handler(void);
void GPIOinit(void);
void SI2cInit_stm32(void *pvswI2Ct, void* dwSDAport, void* swSCLport, uint8_t bSDApin, uint8_t bSCLpin);

typedef struct t_amd64_gpio {
	void	*port;
	uint16_t pin;
} amd64_gpio_t;

#ifdef STM32
#define STM32_SYSTICK_LOAD (SystemCoreClock/1000000U)
#define STM32_SYSTICK_DELAY_CALIB (STM32_SYSTICK_LOAD >> 1) 
#define STM32_DELAY_US(us) \
    do { \
         uint32_t start = SysTick->VAL; \
         uint32_t ticks = (us * STM32_SYSTICK_LOAD)-STM32_SYSTICK_DELAY_CALIB;  \
         while((start - SysTick->VAL) < ticks); \
    } while (0)
#define STM32_DELAY_MS(ms) HAL_Delay(ms)
#endif

#ifdef _M_AMD64
void HAL_Delay(uint32_t mS);
#endif

#ifdef __cplusplus
}
#endif

#endif /* INC_GPIODEF_H_ */

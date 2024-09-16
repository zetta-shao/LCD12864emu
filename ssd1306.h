/**
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 *
 * https://github.com/afiskon/stm32-ssd1306
 */

#ifndef __SSD1306_H__
#define __SSD1306_H__

//#include <stddef.h>
#include <stdint.h>
//#include <_ansi.h>
#include "ssd1306_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32F0)
#include "stm32f0xx_hal.h"
#elif defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#elif defined(STM32L0)
#include "stm32l0xx_hal.h"
#elif defined(STM32L1)
#include "stm32l1xx_hal.h"
#elif defined(STM32L4)
#include "stm32l4xx_hal.h"
#elif defined(STM32L5)
#include "stm32l5xx_hal.h"
#elif defined(STM32F3)
#include "stm32f3xx_hal.h"
#elif defined(STM32H7)
#include "stm32h7xx_hal.h"
#elif defined(STM32F7)
#include "stm32f7xx_hal.h"
#elif defined(STM32G0)
#include "stm32g0xx_hal.h"
#elif defined(STM32G4)
#include "stm32g4xx_hal.h"
//#else
//#error "SSD1306 library was tested only on STM32F0, STM32F1, STM32F3, STM32F4, STM32F7, STM32L0, STM32L1, STM32L4, STM32H7, STM32G0, STM32G4 MCU families. Please modify ssd1306.h if you know what you are doing. Also please send a pull request if it turns out the library works on other MCU's as well!"
#endif

#ifdef SSD1306_X_OFFSET
#define SSD1306_X_OFFSET_LOWER (SSD1306_X_OFFSET & 0x0F)
#define SSD1306_X_OFFSET_UPPER ((SSD1306_X_OFFSET >> 4) & 0x07)
#else
#define SSD1306_X_OFFSET_LOWER 0
#define SSD1306_X_OFFSET_UPPER 0
#endif

#define SPI_3PIN 1

//#include "ssd1306_fonts.h"

/* vvv I2C config vvv */

#ifndef SSD1306_I2C_PORT
#define SSD1306_I2C_PORT        hi2c1
#endif

#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR        (0x3C << 1)
#endif

/* ^^^ I2C config ^^^ */
/* vvv SPI config vvv */
/* ^^^ SPI config ^^^ */

#if defined(SSD1306_USE_I2C)
#include "swi2c.h"
extern I2C_HandleTypeDef SSD1306_I2C_PORT;
#elif defined(SSD1306_USE_SPI)
#include "swspi.h"
extern SPI_HandleTypeDef SSD1306_SPI_PORT;
#else
#error "You should define SSD1306_USE_SPI or SSD1306_USE_I2C macro!"
#endif

// SSD1306 OLED height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT          64
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH           128
#endif

#ifndef SSD1306_BUFFER_SIZE
#define SSD1306_BUFFER_SIZE   SSD1306_WIDTH * SSD1306_HEIGHT / 8
#endif

//typedef stm32_gpio_t ssd1306_gpio_t; //change for other MCU
typedef amd64_gpio_t ssd1306_gpio_t; //change for other MCU

#include "lcd_fonts.h"

// Enumeration for screen colors
typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;

typedef enum {
    SSD1306_OK = 0x00,
    SSD1306_ERR = 0x01  // Generic error.
} SSD1306_Error_t;

// Struct to store transformations
typedef struct tSSD1306 {
    //uint16_t 	CurrentX;
    //uint16_t 	CurrentY;
    lcddev_t    d;
	uint8_t 	flag;
	uint8_t		i2c_addr;
	ssd1306_gpio_t	DC;
	ssd1306_gpio_t	CS;
	ssd1306_gpio_t	RST;
	//swspi_t		*pSPI;
#if defined(SSD1306_USE_I2C)
	//struct __I2C_HandleTypeDef *pDev;
	swi2c_t		*pDev;
#elif defined(SSD1306_USE_SPI)
	//struct __SPI_HandleTypeDef *pDev;
	swspi_t		*pDev;
#endif
	//void*		DC_PORT;
	//void*		CS_PORT;
	//void*		RST_PORT;
	//void*		CLK_PORT;
	//void*		DATA_PORT;
	//uint16_t	dc_pin;
	//uint16_t	cs_pin;
	//uint16_t	rst_pin;
	//uint16_t	clk_pin;
	//uint16_t	dta_pin;
	//uint16_t		rsv1;
	uint8_t		SSD1306_Buffer[SSD1306_BUFFER_SIZE];
} SSD1306_t;

typedef struct {
    uint8_t x;
    uint8_t y;
} SSD1306_VERTEX;

#define __INITED		0x01
#define __DISPLAY_ON	0x02
#define __OLED_3WSPI	0x80

// Procedure definitions

void SSD1306_gpioSWSPI(struct tSSD1306 *d, void* SWCLKport, uint16_t SWCLKpin, void* SWDATAport, uint16_t SWDATApin);
void SSD1306_gpioSetCS(struct tSSD1306 *d, void* CSport, uint16_t CSpin);
void SSD1306_gpioSetDC(struct tSSD1306 *d, void* DCport, uint16_t DCpin);
void SSD1306_gpioSetRST(struct tSSD1306 *d, void* RSTport, uint16_t RSTpin);
void SSD1306_gpioinitSW(struct tSSD1306 *d, void* CSport, uint16_t CSpin, void* DCport, uint16_t DCpin, void* RSTport, uint16_t RSTpin);

//void SSD1306_gpioinit(struct tSSD1306 *d, void* CSport, uint16_t CSpin, void* DCport, uint16_t DCpin, void* RSTport, uint16_t RSTpin, void* SWCLKport, uint16_t SWCLKpin, void* SWDATAport, uint16_t SWDATApin);
//void SSD1306_gpioinit5W(struct tSSD1306 *d, void* CSport, uint16_t CSpin, void* DCport, uint16_t DCpin, void* RSTport, uint16_t RSTpin);
void SSD1306_gpioinit5W2(struct tSSD1306 *d, ssd1306_gpio_t *CS, ssd1306_gpio_t *DC, ssd1306_gpio_t *RST);
//void SSD1306_gpioinit4W(struct tSSD1306 *d, void* CSport, uint16_t CSpin, void* DCport, uint16_t DCpin);
void SSD1306_gpioinit4W2(struct tSSD1306 *d, ssd1306_gpio_t *CS, ssd1306_gpio_t *DC);
//void SSD1306_gpioinit3W(struct tSSD1306 *d, void* CSport, uint16_t CSpin);
void SSD1306_gpioinit3W2(struct tSSD1306 *d, ssd1306_gpio_t *CS);
//void SSD1306_Init(struct tSSD1306 *d, void *pvport);
void SSD1306_init(struct tSSD1306 *d, void *pvport, void* pvFontDef);
void SH1106_Init(struct tSSD1306 *d, void *pvport);
void ssd1306_Fill(struct tSSD1306 *d, SSD1306_COLOR color);
void ssd1306_UpdateScreen(struct tSSD1306 *d);
void ssd1306_DrawPixel(struct tSSD1306 *d, uint8_t x, uint8_t y, SSD1306_COLOR color);
char ssd1306_WriteChar(struct tSSD1306 *d, char ch, FontDef Font, SSD1306_COLOR color);
char ssd1306_WriteChar2(struct tSSD1306 *d, uint8_t ch, FontDef Font, SSD1306_COLOR color);
char ssd1306_WriteString(struct tSSD1306 *d, char* str, FontDef Font, SSD1306_COLOR color);
void ssd1306_SetCursor(struct tSSD1306 *d, uint8_t x, uint8_t y);
void ssd1306_Line(struct tSSD1306 *d, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
void ssd1306_DrawArc(struct tSSD1306 *d, uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SSD1306_COLOR color);
void ssd1306_DrawArcWithRadiusLine(struct tSSD1306 *d, uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SSD1306_COLOR color);
void ssd1306_DrawCircle(struct tSSD1306 *d, uint8_t par_x, uint8_t par_y, uint8_t par_r, SSD1306_COLOR color);
void ssd1306_FillCircle(struct tSSD1306 *d, uint8_t par_x,uint8_t par_y,uint8_t par_r,SSD1306_COLOR par_color);
void ssd1306_Polyline(struct tSSD1306 *d, const SSD1306_VERTEX *par_vertex, uint16_t par_size, SSD1306_COLOR color);
void ssd1306_DrawRectangle(struct tSSD1306 *d, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
void ssd1306_FillRectangle(struct tSSD1306 *d, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
void ssd1306_DrawBitmap(struct tSSD1306 *d, uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, SSD1306_COLOR color);

/**
 * @brief Sets the contrast of the display.
 * @param[in] value contrast to set.
 * @note Contrast increases as the value increases.
 * @note RESET = 7Fh.
 */
void ssd1306_SetContrast(struct tSSD1306 *d, const uint8_t value);

/**
 * @brief Set Display ON/OFF.
 * @param[in] on 0 for OFF, any for ON.
 */
void ssd1306_SetDisplayOn(struct tSSD1306 *d, const uint8_t on);

/**
 * @brief Reads DisplayOn state.
 * @return  0: OFF.
 *          1: ON.
 */
uint8_t ssd1306_GetDisplayOn();

// Low-level procedures
void ssd1306_Reset(struct tSSD1306 *d);
void ssd1306_WriteCommand(struct tSSD1306 *d, uint8_t byte);
void ssd1306_WriteData(struct tSSD1306 *d, uint8_t* buffer, uint8_t buff_size);
SSD1306_Error_t ssd1306_FillBuffer(struct tSSD1306 *d, uint8_t* buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // __SSD1306_H__

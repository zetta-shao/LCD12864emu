/**
 * Private configuration file for the SSD1306 library.
 * This example is configured for STM32F0, I2C and including all fonts.
 */

#ifndef __SSD1306_CONF_H__
#define __SSD1306_CONF_H__

#include "gpiodef.h"

// Choose a microcontroller family
//#define STM32F0
//#define STM32F1
//#define STM32F4
//#define STM32L0
//#define STM32L1
//#define STM32L4
//#define STM32F3
//#define STM32H7
//#define STM32F7
//#define STM32G0

#ifdef __cplusplus
extern "C" {
#endif

// Choose a bus
//#define SSD1306_USE_I2C
#define SSD1306_USE_SPI

// I2C Configuration
//#define SSD1306_I2C_PORT        hi2c1
//#define SSD1306_I2C_ADDR        (0x3C << 1)

// SPI Configuration
#define SSD1306_SPI_PORT        hspi1
#define SSD1306_CS_Port         LCD_CS_P
#define SSD1306_CS_Pin          LCD_CS
#define SSD1306_DC_Port         LCD_CD_P
#define SSD1306_DC_Pin          LCD_CD
#define SSD1306_Reset_Port      LCD_RST_P
#define SSD1306_Reset_Pin       LCD_RST

// Mirror the screen if needed
// #define SSD1306_MIRROR_VERT
// #define SSD1306_MIRROR_HORIZ

// Set inverse color if needed
// # define SSD1306_INVERSE_COLOR

// Include only needed fonts
//#define SSD1306_INCLUDE_FONT_5x7
#define SSD1306_INCLUDE_FONT_5x8
//#define SSD1306_INCLUDE_FONT16_6x8
#define SSD1306_INCLUDE_FONT_7x10
//#define SSD1306_INCLUDE_FONT_11x18
//#define SSD1306_INCLUDE_FONT_16x26

//#define SSD1306_INCLUDE_FONT_16x24

// The width of the screen can be set using this
// define. The default value is 128.
#define SSD1306_WIDTH           128

// If your screen horizontal axis does not start
// in column 0 you can use this define to
// adjust the horizontal offset
// #define SSD1306_X_OFFSET

// The height can be changed as well if necessary.
// It can be 32, 64 or 128. The default value is 64.
#define SSD1306_HEIGHT          64

#ifdef __cplusplus
}
#endif

#endif /* __SSD1306_CONF_H__ */

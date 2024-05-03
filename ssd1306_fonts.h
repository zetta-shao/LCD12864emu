#include <stdint.h>

#ifndef __SSD1306_FONTS_H__
#define __SSD1306_FONTS_H__

#include "ssd1306_conf.h"

typedef struct {
	const uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */
} FontDef;

//typedef struct {
//    const uint8_t FontWidth;    /*!< Font width in pixels */
//    uint8_t FontHeight;   /*!< Font height in pixels */
//    const uint8_t *data; /*!< Pointer to data font data array */
//} FontDef8;

#ifdef SSD1306_INCLUDE_FONT_5x7 //5x7 no padding
extern FontDef Font_5x7;
#endif
#ifdef SSD1306_INCLUDE_FONT_5x8 //5x7 with 1pixel padding version = font 6x8
extern FontDef Font_5x8;
#endif
#ifdef SSD1306_INCLUDE_FONT_6x8
extern FontDef Font_6x8;
#endif
#ifdef SSD1306_INCLUDE_FONT_7x10
extern FontDef Font_7x10;
#endif
#ifdef SSD1306_INCLUDE_FONT_11x18
extern FontDef Font_11x18;
#endif
#ifdef SSD1306_INCLUDE_FONT_16x26
extern FontDef Font_16x26;
#endif
#ifdef SSD1306_INCLUDE_FONT_16x24
extern FontDef Font_16x24;
#endif
#endif // __SSD1306_FONTS_H__

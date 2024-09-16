#include "ssd1306.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>  // For memcpy

#if defined(SSD1306_USE_I2C)
//struct __I2C_HandleTypeDef *_SSD1306_I2C_PORT = NULL;
void ssd1306_Reset(void) { /* for I2C - do nothing */ }
// Send a byte to the command register
void ssd1306_WriteCommand(struct tSSD1306 *d, uint8_t byte) {
    //HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
    HAL_I2C_Mem_Write(d->pD, SSD1306_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
}
// Send data
void ssd1306_WriteData(struct tSSD1306 *d, uint8_t* buffer, size_t buff_size) {
    //HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(d->pD, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
}
#elif defined(SSD1306_USE_SPI)
//struct __SPI_HandleTypeDef *_SSD1306_SPI_PORT = NULL;
void ssd1306_Reset(struct tSSD1306 *d) { //Reset the OLED //CS = High (not selected)
	//if(d->RST_PORT == NULL) return;
	if(d->RST.pin == 65535) return;
    //HAL_GPIO_WritePin(d->CS_PORT, d->cs_pin, GPIO_PIN_RESET);
    swspi_setgpo(&d->CS, 0);
    //HAL_GPIO_WritePin(d->RST_PORT, d->rst_pin, GPIO_PIN_RESET);
    swspi_setgpo(&d->RST, 0);
    //HAL_Delay(10);
    swspi_delay_ms(10);
    //HAL_GPIO_WritePin(d->RST_PORT, d->rst_pin, GPIO_PIN_SET);
    swspi_setgpo(&d->RST, 0);
    //HAL_Delay(10);
    swspi_delay_ms(10);
    //HAL_GPIO_WritePin(d->CS_PORT, d->cs_pin, GPIO_PIN_SET);
    swspi_setgpo(&d->CS, 1);
}

// Send a byte to the command register
void ssd1306_WriteCommand(struct tSSD1306 *d, uint8_t byte) {
    uint16_t tmp=0; uint8_t *pT=(uint8_t*)&tmp;
#if 0
	//uint8_t pT[2];
    HAL_GPIO_WritePin(d->CS_PORT, d->cs_pin, GPIO_PIN_RESET); // select OLED
	if((d->flag & __OLED_3WSPI) != 0) { //must set SPI 9bit data mode first
		pT[0] = byte; pT[1] = 0;
	    HAL_SPI_Transmit(d->pDev, pT, 1, HAL_MAX_DELAY);
	} else {
	    HAL_GPIO_WritePin(d->DC_PORT, d->dc_pin, GPIO_PIN_RESET); // command
    	HAL_SPI_Transmit(d->pDev, (uint8_t *) &byte, 1, HAL_MAX_DELAY);
	}
    HAL_GPIO_WritePin(d->CS_PORT, d->cs_pin, GPIO_PIN_SET); // un-select OLED
#else
    swspi_setgpo(&d->CS, 0); // select OLED
	if((d->flag & __OLED_3WSPI) != 0) { //must set SPI 9bit data mode first
		pT[0] = byte;
	    //HAL_SPI_Transmit(d->pDev, pT, 1, HAL_MAX_DELAY);
	    swspi_write(d->pDev, pT, 1);
	} else {
	    //HAL_GPIO_WritePin(d->DC_PORT, d->dc_pin, GPIO_PIN_RESET); // command
	    swspi_setgpo(&d->DC, 0); // data
    	//HAL_SPI_Transmit(d->pDev, (uint8_t *) &byte, 1, HAL_MAX_DELAY);
    	swspi_write(d->pDev, &byte, 1);
	}
	swspi_setgpo(&d->CS, 1); // un-select OLED

#endif
}

// Send data
void ssd1306_WriteData(struct tSSD1306 *d, uint8_t* buffer, uint8_t buff_size) {
    uint16_t tmp=256; uint8_t *pT=(uint8_t*)&tmp;
#if 0
	//uint8_t pT[2];
    HAL_GPIO_WritePin(d->CS_PORT, d->cs_pin, GPIO_PIN_RESET); // select OLED
	if((d->flag & __OLED_3WSPI) != 0) { //must set SPI 9bit data mode first
	    for(pT[1]=1; buff_size>0; buff_size--, buffer++) {
	    	pT[0] = *buffer;
	    	HAL_SPI_Transmit(d->pDev, pT, 1, HAL_MAX_DELAY);
	    }
	} else {
	    HAL_GPIO_WritePin(d->DC_PORT, d->dc_pin, GPIO_PIN_SET); // data
	    HAL_SPI_Transmit(d->pDev, buffer, buff_size, HAL_MAX_DELAY);

	    HAL_GPIO_WritePin(d->DC_PORT, d->dc_pin, GPIO_PIN_RESET); // command
	}
    HAL_GPIO_WritePin(d->CS_PORT, d->cs_pin, GPIO_PIN_SET); // un-select OLED
#else
    swspi_setgpo(&d->CS, 0); // select OLED
    //HAL_GPIO_WritePin(d->CS_PORT, d->cs_pin, GPIO_PIN_RESET); // select OLED
	if((d->flag & __OLED_3WSPI) != 0) { //must set SPI 9bit data mode first
	    for(; buff_size>0; buff_size--, buffer++) {
	    	pT[0] = *buffer;
	    	//HAL_SPI_Transmit(d->pDev, pT, 1, HAL_MAX_DELAY);
	    	swspi_write(d->pDev, pT, 1);
	    }
	} else {
		swspi_setgpo(&d->DC, 1); // data
	    //HAL_SPI_Transmit(d->pDev, buffer, buff_size, HAL_MAX_DELAY);
		swspi_write(d->pDev, buffer, buff_size);
	    swspi_setgpo(&d->DC, 0); // command
	}
	swspi_setgpo(&d->CS, 1); // un-select OLED
	//HAL_GPIO_WritePin(d->CS_PORT, d->cs_pin, GPIO_PIN_SET); // select OLED
#endif
}

#else
#error "You should define SSD1306_USE_SPI or SSD1306_USE_I2C macro"
#endif

// Screenbuffer
//static uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];
// Screen object
//static SSD1306_t SSD1306;
/* Fills the Screenbuffer with values from a given buffer of a fixed length */
SSD1306_Error_t ssd1306_FillBuffer(struct tSSD1306 *d, uint8_t* buf, uint32_t len) {    
	if(len > SSD1306_BUFFER_SIZE) return SSD1306_ERR;
	memcpy(d->SSD1306_Buffer, buf, len);
    return SSD1306_OK;
}

//void SSD1306_gpioSetCS(struct tSSD1306 *d, void* CSport, uint16_t CSpin) {
//	if(CSport) { d->CS_PORT=CSport; d->cs_pin=CSpin; }
//	else { d->CS_PORT=NULL; d->cs_pin=65535; }
//}

//void SSD1306_gpioSetDC(struct tSSD1306 *d, void* DCport, uint16_t DCpin) {
//	if(DCport) { d->DC_PORT=DCport; d->dc_pin=DCpin; }
//	else { d->DC_PORT=NULL; d->dc_pin=65535; }
//}

//void SSD1306_gpioSetRST(struct tSSD1306 *d, void* RSTport, uint16_t RSTpin ) {
//	if(RSTport) { d->RST_PORT=RSTport; d->rst_pin=RSTpin; }
//	else { d->RST_PORT=NULL; d->rst_pin=65535; }
//}

//void SSD1306_gpioinitSW(struct tSSD1306 *d, void* CSport, uint16_t CSpin, void* DCport, uint16_t DCpin, void* RSTport, uint16_t RSTpin) {
//	SSD1306_gpioSetCS(d, CSport, CSpin);
//	SSD1306_gpioSetDC(d, DCport, DCpin);
//	SSD1306_gpioSetRST(d, RSTport, RSTpin);
//}
void SSD1306_gpioinit5W2(struct tSSD1306 *d, ssd1306_gpio_t *CS, ssd1306_gpio_t *DC, ssd1306_gpio_t *RST) {
	d->pDev=NULL;
	d->CS.port=NULL; d->CS.pin=65535;
	d->DC.port=NULL; d->DC.pin=65535;
	d->RST.port=NULL; d->RST.pin=65535;
	d->flag = 0;
	d->i2c_addr = 0x3c;
    d->d.curX = 0;
    d->d.curY = 0;
	if(CS) { d->CS.port=CS->port; d->CS.pin=CS->pin; };
	if(DC) { d->DC.port=DC->port; d->DC.pin=DC->pin; };
	if(RST) { d->RST.port=RST->port; d->RST.pin=RST->pin; };
}
//void SSD1306_gpioinit5W(struct tSSD1306 *d, void* CSport, uint16_t CSpin, void* DCport, uint16_t DCpin, void* RSTport, uint16_t RSTpin) {
//	SSD1306_gpioinitSW(d, CSport, CSpin, DCport, DCpin, RSTport, RSTpin);
//}

//void SSD1306_gpioinit4W(struct tSSD1306 *d, void* CSport, uint16_t CSpin, void* DCport, uint16_t DCpin) {
//	SSD1306_gpioinitSW(d, CSport, CSpin, DCport, DCpin, NULL, 0);
//}
void SSD1306_gpioinit4W2(struct tSSD1306 *d, ssd1306_gpio_t *CS, ssd1306_gpio_t *DC) { SSD1306_gpioinit5W2(d, CS, DC, NULL); }

//void SSD1306_gpioinit3W(struct tSSD1306 *d, void* CSport, uint16_t CSpin) {
//	SSD1306_gpioinitSW(d, CSport, CSpin, NULL, 0, NULL, 0);
//	d->flag |= __OLED_3WSPI;
//}

void SSD1306_gpioinit3W2(struct tSSD1306 *d, ssd1306_gpio_t *CS) {
	SSD1306_gpioinit5W2(d, CS, NULL, NULL);
	d->flag |= __OLED_3WSPI;
}

/* Initialize the oled screen */
//void SSD1306_Init(struct tSSD1306 *d, void *pvport) {
void SSD1306_init(struct tSSD1306 *d, void *pvport, void* pvFontDef) {
#if defined(SSD1306_USE_I2C)
	d->pDev = (swi2c_t*)pvport;
#elif defined(SSD1306_USE_SPI)
	d->pDev	= (swspi_t*)pvport;
#endif

    d->d.flags = (FONTDRAW_HEIGHTMUL | FONTDRAW_HEIGHTPREDIV | FONTDRAW_VERTICALDRAW);
    d->d.frameWidth = 128;
    d->d.frameHeight = 64;
    //d->d.heightprediv = 3;
    d->d.heightScale = 4; //2^4 = 128(weight)/8
    d->d.widthScale = 0;
    d->d.posmask = 7;
    d->d.invposmask = 0;
    d->d.oneLineOffsetSize = 16;
    d->d.pFrameBuf = d->SSD1306_Buffer;
    if(pvFontDef) d->d.pFont = (struct FontDef*)pvFontDef;
    d->d.color = 1;
    // Reset OLED
    ssd1306_Reset(d);
    // Wait for the screen to boot
    HAL_Delay(100);
    // Init OLED
    ssd1306_SetDisplayOn(d, 0); //display off
    ssd1306_WriteCommand(d, 0x20); //Set Memory Addressing Mode
    ssd1306_WriteCommand(d, 0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                // 10b,Page Addressing Mode (RESET); 11b,Invalid
    ssd1306_WriteCommand(d, 0xB0); //Set Page Start Address for Page Addressing Mode,0-7

#ifdef SSD1306_MIRROR_VERT
    ssd1306_WriteCommand(d, 0xC0); // Mirror vertically
#else
    ssd1306_WriteCommand(d, 0xC8); //Set COM Output Scan Direction
#endif

    ssd1306_WriteCommand(d, 0x00); //---set low column address
    ssd1306_WriteCommand(d, 0x10); //---set high column address

    ssd1306_WriteCommand(d, 0x40); //--set start line address - CHECK

    ssd1306_SetContrast(d, 0xFF);

#ifdef SSD1306_MIRROR_HORIZ
    ssd1306_WriteCommand(d, 0xA0); // Mirror horizontally
#else
    ssd1306_WriteCommand(d, 0xA1); //--set segment re-map 0 to 127 - CHECK
#endif

#ifdef SSD1306_INVERSE_COLOR
    ssd1306_WriteCommand(d, 0xA7); //--set inverse color
#else
    ssd1306_WriteCommand(d, 0xA6); //--set normal color
#endif

// Set multiplex ratio.
#if (SSD1306_HEIGHT == 128)
    // Found in the Luma Python lib for SH1106.
    ssd1306_WriteCommand(d, 0xFF);
#else
    ssd1306_WriteCommand(d, 0xA8); //--set multiplex ratio(1 to 64) - CHECK
#endif

#if (SSD1306_HEIGHT == 32)
    ssd1306_WriteCommand(d, 0x1F); //
#elif (SSD1306_HEIGHT >= 64)
    ssd1306_WriteCommand(d, 0x3F); // Seems to work for 128px high displays too.
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    ssd1306_WriteCommand(d, 0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    ssd1306_WriteCommand(d, 0xD3); //-set display offset - CHECK
    ssd1306_WriteCommand(d, 0x00); //-not offset

    ssd1306_WriteCommand(d, 0xD5); //--set display clock divide ratio/oscillator frequency
    ssd1306_WriteCommand(d, 0xF0); //--set divide ratio

    ssd1306_WriteCommand(d, 0xD9); //--set pre-charge period
    ssd1306_WriteCommand(d, 0x22); //

    ssd1306_WriteCommand(d, 0xDA); //--set com pins hardware configuration - CHECK
#if (SSD1306_HEIGHT == 32)
    ssd1306_WriteCommand(d, 0x02);
#elif (SSD1306_HEIGHT >= 64)
    ssd1306_WriteCommand(d, 0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    ssd1306_WriteCommand(d, 0xDB); //--set vcomh
    ssd1306_WriteCommand(d, 0x20); //0x20,0.77xVcc

    ssd1306_WriteCommand(d, 0x8D); //--set DC-DC enable
    ssd1306_WriteCommand(d, 0x14); //
    ssd1306_SetDisplayOn(d, 1); //--turn on SSD1306 panel

    // Clear screen
    ssd1306_Fill(d, Black);
    
    // Flush buffer to screen
    ssd1306_UpdateScreen(d);
    
    // Set default values for screen object
    d->d.curX = 0; //d->CurrentX = 0;
    d->d.curY = 0; //d->CurrentY = 0;
    d->flag |= __INITED;
}

void SH1106_Init(struct tSSD1306 *d, void *pvport) {
#if defined(SSD1306_USE_I2C)
	d->pDev = (swi2c_t*)pvport;
#elif defined(SSD1306_USE_SPI)
	d->pDev	= (swspi_t*)pvport;
#endif
    //ssd1306_Reset(d); // Reset OLED
    //HAL_Delay(100); // Wait for the screen to boot
    ssd1306_SetDisplayOn(d, 0); // Init OLED //display off
    ssd1306_WriteCommand(d, 0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	ssd1306_SetContrast(d, 255);	//Set Contrast
#ifdef SSD1306_MIRROR_HORIZ
    ssd1306_WriteCommand(d, 0xA0); // Mirror horizontally
#else
    ssd1306_WriteCommand(d, 0xA1); //--set segment re-map 0 to 127 - CHECK
#endif
#ifdef SSD1306_INVERSE_COLOR
    ssd1306_WriteCommand(d, 0xA7); //--set inverse color
#else
    ssd1306_WriteCommand(d, 0xA6); //--set normal color
#endif
    // Set multiplex ratio.
#if (SSD1306_HEIGHT == 128)
    // Found in the Luma Python lib for SH1106.
    ssd1306_WriteCommand(d, 0xFF);
#else
    ssd1306_WriteCommand(d, 0xA8); //--set multiplex ratio(1 to 64) - CHECK
#endif
#if (SSD1306_HEIGHT == 32)
    ssd1306_WriteCommand(d, 0x1F); //
#elif (SSD1306_HEIGHT >= 64)
    ssd1306_WriteCommand(d, 0x3F); // Seems to work for 128px high displays too.
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif
    ssd1306_WriteCommand(d, 0xAD); //set pump mode
    ssd1306_WriteCommand(d, 0x8B);	//set pump on
    ssd1306_WriteCommand(d, 0x32); //set pump voltage
#ifdef SSD1306_MIRROR_VERT
    ssd1306_WriteCommand(d, 0xC0); // Mirror vertically
#else
    ssd1306_WriteCommand(d, 0xC8); //Set COM Output Scan Direction
#endif
    ssd1306_WriteCommand(d, 0xD3); //-set display offset - CHECK
    ssd1306_WriteCommand(d, 0x00); //-not offset
    ssd1306_WriteCommand(d, 0xD5); //--set display clock divide ratio/oscillator frequency
    ssd1306_WriteCommand(d, 0x80); //--set divide ratio
    ssd1306_WriteCommand(d, 0xD9); //--set pre-charge period
    ssd1306_WriteCommand(d, 0x1F); //
    ssd1306_WriteCommand(d, 0xDA); //--set com pins hardware configuration - CHECK
#if (SSD1306_HEIGHT == 32)
    ssd1306_WriteCommand(d, 0x02);
#elif (SSD1306_HEIGHT >= 64)
    ssd1306_WriteCommand(d, 0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif
    ssd1306_WriteCommand(d, 0xDB); //--set vcomh
    ssd1306_WriteCommand(d, 0x40); //0x20,0.77xVcc
    ssd1306_SetDisplayOn(d, 1); //--turn on SSD1306 panel
    // Clear screen
    ssd1306_Fill(d, Black);
    // Flush buffer to screen
    ssd1306_UpdateScreen(d);
    // Set default values for screen object
    d->d.curX = 0; //d->CurrentX = 0;
    d->d.curY = 0; //d->CurrentY = 0;
    d->flag |= __INITED;
}

/* Fill the whole screen with the given color */
void ssd1306_Fill(struct tSSD1306 *d, SSD1306_COLOR color) {
    //uint32_t i;
    //uint8_t c = (color == Black) ? 0x00 : 0xFF;
    //for(i = 0; i < SSD1306_BUFFER_SIZE; i++) { d->SSD1306_Buffer[i] = c; }
    memset(d->SSD1306_Buffer, ((color == Black) ? 0x00 : 0xFF), SSD1306_BUFFER_SIZE);
}

/* Write the screenbuffer with changed to the screen */
void ssd1306_UpdateScreen(struct tSSD1306 *d) {
    // Write data to each page of RAM. Number of pages
    // depends on the screen height:
    //
    //  * 32px   ==  4 pages
    //  * 64px   ==  8 pages
    //  * 128px  ==  16 pages
	uint8_t *pT = d->SSD1306_Buffer;
    for(uint8_t i = 0; i < SSD1306_HEIGHT/8; i++) {
        ssd1306_WriteCommand(d, 0xB0 + i); // Set the current RAM page address.
        ssd1306_WriteCommand(d, 0x00 + SSD1306_X_OFFSET_LOWER);
        ssd1306_WriteCommand(d, 0x10 + SSD1306_X_OFFSET_UPPER);
        //ssd1306_WriteData(d, d->SSD1306_Buffer + (SSD1306_WIDTH*i), SSD1306_WIDTH);
        ssd1306_WriteData(d, pT, SSD1306_WIDTH);
        pT += SSD1306_WIDTH;
    }
}

/*
 * Draw one pixel in the screenbuffer
 * X => X Coordinate
 * Y => Y Coordinate
 * color => Pixel color
 */
void ssd1306_DrawPixel(struct tSSD1306 *d, uint8_t x, uint8_t y, SSD1306_COLOR color) {
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        // Don't write outside the buffer
        return;
    }
    // Draw in the right color
    if(color == White) {
        d->SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } else { 
        d->SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }

}

/*
 * Draw 1 char to the screen buffer
 * ch       => char om weg te schrijven
 * Font     => Font waarmee we gaan schrijven
 * color    => Black or White
 */
char ssd1306_WriteChar(struct tSSD1306 *d, char ch, FontDef Font, SSD1306_COLOR color) {
    uint32_t i, b, j;

    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;

    // Check remaining space on current line
    if (SSD1306_WIDTH < (d->d.curX + Font.FontWidth) ||
        SSD1306_HEIGHT < (d->d.curY + Font.FontHeight))
    {
        // Not enough space on current line
        return 0;
    }

    // Use the font to write
    for(i = 0; i < Font.FontHeight; i++) {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for(j = 0; j < Font.FontWidth; j++) {
            if((b << j) & 0x8000)  {
                ssd1306_DrawPixel(d, d->d.curX + j, (d->d.curY + i), (SSD1306_COLOR) color);
            } else {
                ssd1306_DrawPixel(d, d->d.curX + j, (d->d.curY + i), (SSD1306_COLOR)!color);
            }
        }
    }

    // The current space is now taken
    d->d.curX += Font.FontWidth;

    // Return written char for validation
    return ch;
}

// 1-31 => 95-125
// 32-126 => 0-94
// 127-255 => 126-254
char ssd1306_WriteChar2(struct tSSD1306 *d, uint8_t ch, FontDef Font, SSD1306_COLOR color) {
    uint32_t i, j;
    if(ch == 0) return 0;
    if (SSD1306_WIDTH < (d->d.curX + Font.FontWidth) ||
        SSD1306_HEIGHT < (d->d.curY + Font.FontHeight)) { return 0; }
    if(Font.FontHeight <= 8) {
        uint8_t *px, m;
        px = ((uint8_t*)Font.data);
        if(Font.flags & FONT_FLAG_BTAB) //if(Font.bBigTable != 0)
            px += (ch - 1) * Font.FontWidth;
        else
            px += (ch - 32) * Font.FontWidth;

        for(i = 0; i < Font.FontWidth; i++) {
            for(m=0x80,j=0; j<Font.FontHeight; j++, m>>=1) {
                ssd1306_DrawPixel(d, d->d.curX + i, (d->d.curY + j), (px[i] & m) ? color : !color);
            }
        }

    } else {
        uint16_t *px, m;
        if (ch < 32 || ch > 126) return 0;
        px = (uint16_t*)Font.data + ((ch-32)*Font.FontHeight);
        for(i = 0; i < Font.FontHeight; i++) {
            for(m=0x8000,j=0; j<Font.FontWidth; j++, m>>=1) {
                ssd1306_DrawPixel(d, d->d.curX + j, (d->d.curY + i), (px[i] & m) ? color : !color);
            }
        }
    }

    d->d.curX += Font.FontWidth;
    return ch;
}

/* Write full string to screenbuffer */
char ssd1306_WriteString(struct tSSD1306 *d, char* str, FontDef Font, SSD1306_COLOR color) {
    while (*str) {
        if (ssd1306_WriteChar2(d, *(uint8_t*)str, Font, color) != *str) {
            // Char could not be written
            return *str;
        }
        str++;
    }
    // Everything ok
    return *str;
}

/* Position the cursor */
void ssd1306_SetCursor(struct tSSD1306 *d, uint8_t x, uint8_t y) {
    d->d.curX = x;
    d->d.curY = y;
}

/* Draw line by Bresenhem's algorithm */
void ssd1306_Line(struct tSSD1306 *d, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = ((x1 < x2) ? 1 : -1);
    int32_t signY = ((y1 < y2) ? 1 : -1);
    int32_t error = deltaX - deltaY;
    int32_t error2;
    
    ssd1306_DrawPixel(d, x2, y2, color);

    while((x1 != x2) || (y1 != y2)) {
        ssd1306_DrawPixel(d, x1, y1, color);
        error2 = error * 2;
        if(error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;
        }
        
        if(error2 < deltaX) {
            error += deltaX;
            y1 += signY;
        }
    }
    return;
}

/* Draw polyline */
void ssd1306_Polyline(struct tSSD1306 *d, const SSD1306_VERTEX *par_vertex, uint16_t par_size, SSD1306_COLOR color) {
    uint16_t i;
    if(par_vertex == NULL) {
        return;
    }

    for(i = 1; i < par_size; i++) {
        ssd1306_Line(d, par_vertex[i - 1].x, par_vertex[i - 1].y, par_vertex[i].x, par_vertex[i].y, color);
    }

    return;
}

/* Convert Degrees to Radians */
static float ssd1306_DegToRad(float par_deg) {
    return par_deg * 3.14 / 180.0;
}

/* Normalize degree to [0;360] */
static uint16_t ssd1306_NormalizeTo0_360(uint16_t par_deg) {
    uint16_t loc_angle;
    if(par_deg <= 360) {
        loc_angle = par_deg;
    } else {
        loc_angle = par_deg % 360;
        loc_angle = ((par_deg != 0)?par_deg:360);
    }
    return loc_angle;
}

/*
 * DrawArc. Draw angle is beginning from 4 quart of trigonometric circle (3pi/2)
 * start_angle in degree
 * sweep in degree
 */
void ssd1306_DrawArc(struct tSSD1306 *d, uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SSD1306_COLOR color) {
    static const uint8_t CIRCLE_APPROXIMATION_SEGMENTS = 36;
    float approx_degree;
    uint32_t approx_segments;
    uint8_t xp1,xp2;
    uint8_t yp1,yp2;
    uint32_t count = 0;
    uint32_t loc_sweep = 0;
    float rad;
    
    loc_sweep = ssd1306_NormalizeTo0_360(sweep);
    
    count = (ssd1306_NormalizeTo0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_degree = loc_sweep / (float)approx_segments;
    while(count < approx_segments)
    {
        rad = ssd1306_DegToRad(count*approx_degree);
        xp1 = x + (int8_t)(sin(rad)*radius);
        yp1 = y + (int8_t)(cos(rad)*radius);    
        count++;
        if(count != approx_segments) {
            rad = ssd1306_DegToRad(count*approx_degree);
        } else {
            rad = ssd1306_DegToRad(loc_sweep);
        }
        xp2 = x + (int8_t)(sin(rad)*radius);
        yp2 = y + (int8_t)(cos(rad)*radius);    
        ssd1306_Line(d, xp1,yp1,xp2,yp2,color);
    }
    
    return;
}

/*
 * Draw arc with radius line
 * Angle is beginning from 4 quart of trigonometric circle (3pi/2)
 * start_angle: start angle in degree
 * sweep: finish angle in degree
 */
void ssd1306_DrawArcWithRadiusLine(struct tSSD1306 *d, uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SSD1306_COLOR color) {
    static const uint8_t CIRCLE_APPROXIMATION_SEGMENTS = 36;
    float approx_degree;
    uint32_t approx_segments;
    uint8_t xp1 = 0;
    uint8_t xp2 = 0;
    uint8_t yp1 = 0;
    uint8_t yp2 = 0;
    uint32_t count = 0;
    uint32_t loc_sweep = 0;
    float rad;
    
    loc_sweep = ssd1306_NormalizeTo0_360(sweep);
    
    count = (ssd1306_NormalizeTo0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_degree = loc_sweep / (float)approx_segments;

    rad = ssd1306_DegToRad(count*approx_degree);
    uint8_t first_point_x = x + (int8_t)(sin(rad)*radius);
    uint8_t first_point_y = y + (int8_t)(cos(rad)*radius);   
    while (count < approx_segments) {
        rad = ssd1306_DegToRad(count*approx_degree);
        xp1 = x + (int8_t)(sin(rad)*radius);
        yp1 = y + (int8_t)(cos(rad)*radius);    
        count++;
        if (count != approx_segments) {
            rad = ssd1306_DegToRad(count*approx_degree);
        } else {
            rad = ssd1306_DegToRad(loc_sweep);
        }
        xp2 = x + (int8_t)(sin(rad)*radius);
        yp2 = y + (int8_t)(cos(rad)*radius);    
        ssd1306_Line(d, xp1,yp1,xp2,yp2,color);
    }
    
    // Radius line
    ssd1306_Line(d, x,y,first_point_x,first_point_y,color);
    ssd1306_Line(d, x,y,xp2,yp2,color);
    return;
}

/* Draw circle by Bresenhem's algorithm */
void ssd1306_DrawCircle(struct tSSD1306 *d, uint8_t par_x,uint8_t par_y,uint8_t par_r,SSD1306_COLOR par_color) {
    int32_t x = -par_r;
    int32_t y = 0;
    int32_t err = 2 - 2 * par_r;
    int32_t e2;

    if (par_x >= SSD1306_WIDTH || par_y >= SSD1306_HEIGHT) {
        return;
    }

    do {
        ssd1306_DrawPixel(d, par_x - x, par_y + y, par_color);
        ssd1306_DrawPixel(d, par_x + x, par_y + y, par_color);
        ssd1306_DrawPixel(d, par_x + x, par_y - y, par_color);
        ssd1306_DrawPixel(d, par_x - x, par_y - y, par_color);
        e2 = err;

        if (e2 <= y) {
            y++;
            err = err + (y * 2 + 1);
            if(-x == y && e2 <= x) {
                e2 = 0;
            }
        }

        if (e2 > x) {
            x++;
            err = err + (x * 2 + 1);
        }
    } while (x <= 0);

    return;
}

/* Draw filled circle. Pixel positions calculated using Bresenham's algorithm */
void ssd1306_FillCircle(struct tSSD1306 *d, uint8_t par_x,uint8_t par_y,uint8_t par_r,SSD1306_COLOR par_color) {
    int32_t x = -par_r;
    int32_t y = 0;
    int32_t err = 2 - 2 * par_r;
    int32_t e2;

    if (par_x >= SSD1306_WIDTH || par_y >= SSD1306_HEIGHT) {
        return;
    }

    do {
        for (uint8_t _y = (par_y + y); _y >= (par_y - y); _y--) {
            for (uint8_t _x = (par_x - x); _x >= (par_x + x); _x--) {
                ssd1306_DrawPixel(d, _x, _y, par_color);
            }
        }

        e2 = err;
        if (e2 <= y) {
            y++;
            err = err + (y * 2 + 1);
            if (-x == y && e2 <= x) {
                e2 = 0;
            }
        }

        if (e2 > x) {
            x++;
            err = err + (x * 2 + 1);
        }
    } while (x <= 0);

    return;
}

/* Draw a rectangle */
void ssd1306_DrawRectangle(struct tSSD1306 *d, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    ssd1306_Line(d, x1,y1,x2,y1,color);
    ssd1306_Line(d, x2,y1,x2,y2,color);
    ssd1306_Line(d, x2,y2,x1,y2,color);
    ssd1306_Line(d, x1,y2,x1,y1,color);

    return;
}

/* Draw a filled rectangle */
void ssd1306_FillRectangle(struct tSSD1306 *d, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    uint8_t x_start = ((x1<=x2) ? x1 : x2);
    uint8_t x_end   = ((x1<=x2) ? x2 : x1);
    uint8_t y_start = ((y1<=y2) ? y1 : y2);
    uint8_t y_end   = ((y1<=y2) ? y2 : y1);

    for (uint8_t y= y_start; (y<= y_end)&&(y<SSD1306_HEIGHT); y++) {
        for (uint8_t x= x_start; (x<= x_end)&&(x<SSD1306_WIDTH); x++) {
            ssd1306_DrawPixel(d, x, y, color);
        }
    }
    return;
}

/* Draw a bitmap */
void ssd1306_DrawBitmap(struct tSSD1306 *d, uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, SSD1306_COLOR color) {
    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        return;
    }

    for (uint8_t j = 0; j < h; j++, y++) {
        for (uint8_t i = 0; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }

            if (byte & 0x80) {
                ssd1306_DrawPixel(d, x + i, y, color);
            }
        }
    }
    return;
}

void ssd1306_SetContrast(struct tSSD1306 *d, const uint8_t value) {
    const uint8_t kSetContrastControlRegister = 0x81;
    ssd1306_WriteCommand(d, kSetContrastControlRegister);
    ssd1306_WriteCommand(d, value);
}

void ssd1306_SetDisplayOn(struct tSSD1306 *d, const uint8_t on) {
    uint8_t value;
    if (on) {
        value = 0xAF;   // Display on
        //d->DisplayOn = 1;
        d->flag |= __DISPLAY_ON;
    } else {
        value = 0xAE;   // Display off
        //d->DisplayOn = 0;
        d->flag &= ~__DISPLAY_ON;
    }
    ssd1306_WriteCommand(d, value);
}

//uint8_t ssd1306_GetDisplayOn(struct tSSD1306 *d) { return d->DisplayOn; }
uint8_t ssd1306_GetDisplayOn(struct tSSD1306 *d) { return (d->flag & __DISPLAY_ON)?1:0; }

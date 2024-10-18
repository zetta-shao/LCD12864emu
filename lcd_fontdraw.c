#include "lcd_fontdraw.h"
#include "lcd_fonts.h"
#include <stdio.h>

/*void init_ssd1306_128x64(fontdraw_t *d, void *pvBUF, void *pvFontDef) {
    d->flags = (FONTDRAW_HEIGHTMUL | FONTDRAW_VERTICALDRAW);
    d->frameWidth = 128;
    d->frameHeight = 64;
    d->heightScale = 4;
    d->widthScale = 0;
    d->hormask = ~0;
    d->invhormask = 0;
    d->vermask = 7;
    d->invvermask = 0;
    d->oneLineOffsetSize = 8;
    d->pFrameBuf = pvBUF;
    if(pvFontDef) d->pFont = (struct FontDef*)pvFontDef;
}*/

/*void init_st7920_128x64(fontdraw_t *d, void *pvBUF, void *pvFontDef) {
    d->flags = (FONTDRAW_HEIGHTMUL | FONTDRAW_WIDTHDIV );
    d->frameWidth = 128;
    d->frameHeight = 64;
    d->heightScale = 4;
    d->widthScale = 3;
    d->posmask = 7;
    d->invposmask = 7; //position from 7-0 to 0-7
    d->oneLineOffsetSize = 16;
    d->pFrameBuf = pvBUF;
    if(pvFontDef) d->pFont = (struct FontDef*)pvFontDef;
}*/

void fontdraw_drawpixelBW(fontdraw_t *d, uint32_t x, uint32_t y, int color) {
    int ox = (x & d->posmask) ^ d->invposmask;
    int oy = (y & d->posmask) ^ d->invposmask;
    //if(d->flags & FONTDRAW_WIDTHPREDIV) x >>= d->widthprediv;
    if(d->flags & FONTDRAW_WIDTHPREDIV) x &= ~(d->posmask);
    //if(d->flags & FONTDRAW_HEIGHTPREDIV) y >>= d->heightprediv;
    if(d->flags & FONTDRAW_HEIGHTPREDIV) y &= ~(d->posmask);
    if(d->flags & FONTDRAW_HEIGHTDIV) y >>= d->heightScale;
    else if(d->flags & FONTDRAW_HEIGHTMUL) { y <<= d->heightScale; }
    if(d->flags & FONTDRAW_WIDTHDIV) x >>= d->widthScale;
    else if(d->flags & FONTDRAW_WIDTHMUL) x <<= d->widthScale;
    if(d->flags & FONTDRAW_VERTICALDRAW) { 
        if(color) d->pFrameBuf[x+y] |= 1 << oy;
        else d->pFrameBuf[x+y] &= ~(1 << oy); 
    } else {
        if(color) d->pFrameBuf[x+y] |= 1 << ox;
        else d->pFrameBuf[x+y] &= ~(1 << ox); 
    }
}

//char fontdraw_char(fontdraw_t *d, void *pFontDef, int posx, int posy, uint8_t ch, int color) {
char fontdraw_charFont(fontdraw_t *d, uint8_t ch, int color, void *pvFontDef) {    
    uint32_t i, j;
    int posx=d->curX, posy=d->curY;
    FontDef *font = (FontDef*)pvFontDef;
    if(ch == 0) return 0;
    if( d->frameWidth < (posx + d->pFont->FontWidth) ||
        d->frameHeight < (posy + d->pFont->FontHeight)) { return 0; }

    if(d->pFont->flags & FONT_FLAG_WPTR) {
        uint16_t *px, m;
        if(font->flags & FONT_FLAG_VERT) {
            px = (uint16_t*)font->data + ((ch-32)*font->FontWidth);
            for(i = 0; i < font->FontWidth; i++) {
                for(m=0x8000,j=0; j<font->FontHeight; j++, m>>=1) {
                    fontdraw_drawpixelBW(d, posx + j, (posy + i), (px[i] & m) ? color : !color);
                }
            }
        } else {
            px = (uint16_t*)font->data + ((ch-32)*font->FontHeight);
            for(i = 0; i < font->FontHeight; i++) {
                for(m=0x8000,j=0; j<font->FontWidth; j++, m>>=1) {
                    fontdraw_drawpixelBW(d, posx + j, (posy + i), (px[i] & m) ? color : !color);
                }
            }
        }
    } else {
        uint8_t *px, m;
        if(font->flags & FONT_FLAG_VERT) {
            px = ((uint8_t*)font->data);
            if(font->flags & FONT_FLAG_BTAB) px += (ch - 1) * font->FontWidth;
            else px += (ch - 32) * font->FontWidth;
            for(i = 0; i < font->FontWidth; i++) {
                for(m=0x80,j=0; j<font->FontHeight; j++, m>>=1) {
                    fontdraw_drawpixelBW(d, posx + i, (posy + j), (px[i] & m) ? color : !color);
                }
            }
        } else {
            px = ((uint8_t*)font->data);
            if(font->flags & FONT_FLAG_BTAB) px += (ch - 1) * font->FontHeight;
            else px += (ch - 32) * font->FontHeight;
            for(i = 0; i < font->FontHeight; i++) {
                for(m=0x80,j=0; j<font->FontWidth; j++, m>>=1) {
                    fontdraw_drawpixelBW(d, posx + j, (posy + i), (px[i] & m) ? color : !color);
                }
            }
        }
    }
    d->curX += font->FontWidth;
    return ch;
}

char fontdraw_char(fontdraw_t *d, uint8_t ch) { return fontdraw_charFont(d, ch, d->color, d->pFont); }
char fontdraw_charC(fontdraw_t *d, uint8_t ch, int color) { return fontdraw_charFont(d, ch, color, d->pFont); }

void fontdraw_string(fontdraw_t *d, char *s) {
    while(*s) { fontdraw_charFont(d, *s, d->color, d->pFont); s++; }
}

void fontdraw_stringC(fontdraw_t *d, char *s, uint8_t color) {
    while(*s) { fontdraw_charFont(d, *s, color, d->pFont); s++; }
}

void fontdraw_stringFont(fontdraw_t *d, char *s, uint8_t color, void *pvFontDef) {
    while(*s) { fontdraw_charFont(d, *s, color, pvFontDef); s++; }
}

void fontdraw_setpos(fontdraw_t *d, uint32_t x, uint32_t y) { d->curX = x; d->curY = y; }

void fontdraw_setColor(fontdraw_t *d, uint8_t color) { d->color=color; }

#include <string.h>
void fontdraw_fill(fontdraw_t *d, int color) {
    if(d->oneLineOffsetSize>0 && d->frameHeight>0) {
        memset(d->pFrameBuf, color, (d->frameWidth*d->frameHeight)/8);
    }
}

int8_t str_3digit(int16_t val, char *outstr) {
    int8_t res = (val < 0) ? 0 : 1;
#if defined (ICO_NUMBER_DOT) && defined(ICO_DOT_NUMBER)
    if(val < 1000) {
        sprintf(outstr, " %02d", val%100);
        outstr[0] = ((val/100)%10) + ICO_DOT_NUMBER;
    } else if(val < 10000) {
        sprintf(outstr, " %02d", (val%1000)/10);
        outstr[0] = ((val/1000)%10) + ICO_NUMBER_DOT;
    } else if(val < 32767) {
        sprintf(outstr, "%01d %01d", (val/10000)%10, (val%1000)/100);
        outstr[1] = ((val/1000)%10) + ICO_NUMBER_DOT;
    }
    outstr[3] = 0;
#else
    (void)val; (void)outstr;
#endif
    return res;
}

int8_t str_3digitL(int16_t val, char *outstr) { //3+1
    int8_t res = (val < 0) ? 0 : 1;
    val = (val > 0) ? val : -val;
    if(val < 1000) {
        sprintf(outstr, ".%03d", val);
    } else if(val < 10000) {
        sprintf(outstr, "%01d.%02d", (val/1000)%10, (val%1000)/10);
    } else if(val < 32767) {
        sprintf(outstr, "%02d.%01d", (val/1000)%100, (val%1000)/100);
    }
    outstr[4] = 0;
    return res;
}

int8_t str_5digit_m(int16_t val, char *outstr, char unit) {
    int8_t res = (val < 0) ? 0 : 1;
    val = (val > 0) ? val : -val;
    if(val < 1000) {
        sprintf(outstr, "%3dm%c", val, unit);
    } else if(val < 10000) {
        sprintf(outstr, "%1d.%2d%c", (val/1000)%10, (val%1000)/10, unit);
    } else if(val < 32767) {
        sprintf(outstr, "%2d.%1d%c", (val/1000)%100, (val%1000)/100, unit);
    }
    outstr[6] = 0;
    return res;
}

int8_t str_4digit(int16_t val, char *outstr) {
    int8_t res = (val < 0) ? 0 : 1;
#ifdef ICO_NUMBER_DOT
    if(val < 10000) {
        sprintf(outstr, " %03d", val%1000);
        outstr[0] = ((val/1000)%10) + ICO_NUMBER_DOT;
    } else if(val < 32767) {
        sprintf(outstr, "%01d %02d", (val/10000)%10, (val%1000)/10);
        outstr[1] = ((val/1000)%10) + ICO_NUMBER_DOT;
    }
    outstr[4] = 0;
#else
    (void)val; (void)outstr;
#endif
    return res;
}

int8_t str_4digitL(int16_t val, char *outstr) { //4+1
    int8_t res = (val < 0) ? 0 : 1;
    if(val < 10000) {
        sprintf(outstr, "%01d.%03d", (val/1000)%10, val%1000);
    } else if(val < 32767) {
        sprintf(outstr, "%02d.%02d", (val/1000)%100, (val%1000)/10);
    }
    outstr[5] = 0;
    return res;
}


int8_t str_5digit(int16_t val, char *outstr) {
    int8_t res = (val < 0) ? 0 : 1;
#ifdef ICO_NUMBER_DOT
    if(val < 10000)
        sprintf(outstr, "  %03d", val%1000);
    else
        sprintf(outstr, "%01d %03d", (val/10000)%10, val%1000);

    outstr[1] = ((val/1000)%10) + ICO_NUMBER_DOT;
    outstr[5] = 0;
#else
    (void)val; (void)outstr;
#endif
    return res;
}

typedef int8_t (*__strfmt)(int16_t, char*);


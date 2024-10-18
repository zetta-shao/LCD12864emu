#ifndef __INCL_FONTDRAW__
#define __INCL_FONTDRAW__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

#define FONTDRAW_WIDTHMUL       0x01
#define FONTDRAW_WIDTHDIV       0x02
#define FONTDRAW_HEIGHTMUL      0x04
#define FONTDRAW_HEIGHTDIV      0x08
#define FONTDRAW_WIDTHPREDIV    0x10
#define FONTDRAW_HEIGHTPREDIV   0x20
#define FONTDRAW_VERTICALDRAW   0x80

typedef struct fontdraw_t fontdraw_t;
typedef struct fontdraw_t lcddev_t;
typedef struct pos_t pos_t;

int8_t str_3digit(int16_t val, char *outstr);
int8_t str_3digitL(int16_t val, char *outstr);
int8_t str_4digit(int16_t val, char *outstr);
int8_t str_4digitL(int16_t val, char *outstr);
int8_t str_5digit(int16_t val, char *outstr);
int8_t str_5digit_m(int16_t val, char *outstr, char unit);

typedef int8_t (*__strfmt)(int16_t, char*);
typedef void (*__update_screen)(lcddev_t*);
typedef void (*__fill_color)(lcddev_t*, uint8_t);

struct fontdraw_t {
    uint8_t     *pFrameBuf;
    struct FontDef *pFont;
    uint8_t     curX;
    uint8_t     curY;
    uint8_t     color;
    uint8_t     widthScale;
    uint8_t     heightScale;
    uint8_t     posmask;
    uint8_t     invposmask;
    uint8_t     posmask_inv;
    uint16_t    flags;
    uint16_t    frameWidth;
    uint16_t    frameHeight;
    uint16_t    oneLineOffsetSize;
    void        *parent;
    __update_screen update;
    __fill_color    fill_color;
};

struct pos_t {
    uint8_t x;
    uint8_t y;
};

void init_ssd1306_128x64(fontdraw_t *d, void *pvBUF, void *pvFontDef);
//void init_st7920_128x64(fontdraw_t *d, void *pvBUF, void *pvFontDef);
char fontdraw_char(fontdraw_t *d, uint8_t ch);
char fontdraw_charC(fontdraw_t *d, uint8_t ch, int color);
char fontdraw_charFont(fontdraw_t *d, uint8_t ch, int color, void *pvFontDef);
void fontdraw_drawpixelBW(fontdraw_t *d, uint32_t x, uint32_t y, int color);
void fontdraw_string(fontdraw_t *d, char *s);
void fontdraw_stringC(fontdraw_t *d, char *s,  uint8_t color);
void fontdraw_stringFont(fontdraw_t *d, char *s, uint8_t color, void *pvFontDef);
void fontdraw_setpos(fontdraw_t *d, uint32_t x, uint32_t y);
void fontdraw_setColor(fontdraw_t *d, uint8_t color);
void fontdraw_fill(fontdraw_t *d, int color);
#ifdef USE_GEOMETRIC
void grapdraw_Line(lcddev_t *d, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int color);
void grapdraw_Polyline(lcddev_t *d, const pos_t *par_vertex, uint16_t par_size, int color);
//static uint16_t grapdraw_NormalizeTo0_360(uint16_t par_deg);
void grapdraw_DrawArc(lcddev_t *d, uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, int color);
void grapdraw_DrawArcWithRadiusLine(lcddev_t *d, uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, int color);
void grapdraw_DrawCircle(lcddev_t *d, uint8_t par_x,uint8_t par_y,uint8_t par_r,int par_color);
void grapdraw_FillCircle(lcddev_t *d, uint8_t par_x,uint8_t par_y,uint8_t par_r,int par_color);
void grapdraw_DrawRectangle(lcddev_t *d, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int color);
void grapdraw_DrawBitmap(lcddev_t *d, uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, int color);
#endif
#ifdef __cplusplus
}
#endif
#endif

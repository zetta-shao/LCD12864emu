#ifndef __INCL_FONTDRAW__
#define __INCL_FONTDRAW__
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

int8_t str_3digit(int16_t val, char *outstr);
int8_t str_3digitL(int16_t val, char *outstr);
int8_t str_4digit(int16_t val, char *outstr);
int8_t str_4digitL(int16_t val, char *outstr);
int8_t str_5digit(int16_t val, char *outstr);

typedef int8_t (*__strfmt)(int16_t, char*);

struct fontdraw_t {
    uint8_t     *pFrameBuf;
    struct FontDef *pFont;
    int         curX;
    int         curY;
    int         color;
    int         frameWidth;
    int         frameHeight;
    int         oneLineOffsetSize;
    int         flags;
    int         widthScale;
    int         heightScale;
    //int         widthprediv;
    //int         heightprediv;
    int         posmask;
    int         invposmask;
    int         posmask_inv;
};

void init_ssd1306_128x64(fontdraw_t *d, void *pvBUF, void *pvFontDef);
//void init_st7920_128x64(fontdraw_t *d, void *pvBUF, void *pvFontDef);
void fontdraw_1bit(fontdraw_t *d, uint32_t x, uint32_t y, int color);
char fontdraw_char(fontdraw_t *d, uint8_t ch);
char fontdraw_charC(fontdraw_t *d, uint8_t ch, int color);
char fontdraw_charFont(fontdraw_t *d, uint8_t ch, int color, void *pvFontDef);
void fontdraw_1bit(fontdraw_t *d, uint32_t x, uint32_t y, int color);
void fontdraw_string(fontdraw_t *d, char *s);
void fontdraw_stringC(fontdraw_t *d, char *s,  uint8_t color);
void fontdraw_stringFont(fontdraw_t *d, char *s, uint8_t color, void *pvFontDef);
void fontdraw_setpos(fontdraw_t *d, uint32_t x, uint32_t y);
void fontdraw_setColor(fontdraw_t *d, uint8_t color);
void fontdraw_fill(fontdraw_t *d, int color);
#endif

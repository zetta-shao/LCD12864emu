// rewrite from https://github.com/afiskon/stm32-ssd1306
#include "lcd_fontdraw.h"
#ifdef USE_GEOMETRIC
#include <math.h>
#include <stdlib.h>
#include <string.h>  // For memcpy

/* Draw line by Bresenhem's algorithm */
void grapdraw_Line(lcddev_t *d, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int color) {
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = ((x1 < x2) ? 1 : -1);
    int32_t signY = ((y1 < y2) ? 1 : -1);
    int32_t error = deltaX - deltaY;
    int32_t error2;
    
    fontdraw_drawpixelBW(d, x2, y2, color);

    while((x1 != x2) || (y1 != y2)) {
        fontdraw_drawpixelBW(d, x1, y1, color);
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
void grapdraw_Polyline(lcddev_t *d, const pos_t *par_vertex, uint16_t par_size, int color) {
    uint16_t i;
    if(par_vertex == NULL) {
        return;
    }

    for(i = 1; i < par_size; i++) {
        grapdraw_Line(d, par_vertex[i - 1].x, par_vertex[i - 1].y, par_vertex[i].x, par_vertex[i].y, color);
    }

    return;
}

/* Convert Degrees to Radians */
static float grapdraw_DegToRad(float par_deg) {
    return par_deg * 3.14 / 180.0;
}

/* Normalize degree to [0;360] */
static uint16_t grapdraw_NormalizeTo0_360(uint16_t par_deg) {
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
void grapdraw_DrawArc(lcddev_t *d, uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, int color) {
    static const uint8_t CIRCLE_APPROXIMATION_SEGMENTS = 36;
    float approx_degree;
    uint32_t approx_segments;
    uint8_t xp1,xp2;
    uint8_t yp1,yp2;
    uint32_t count = 0;
    uint32_t loc_sweep = 0;
    float rad;
    
    loc_sweep = grapdraw_NormalizeTo0_360(sweep);
    
    count = (grapdraw_NormalizeTo0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_degree = loc_sweep / (float)approx_segments;
    while(count < approx_segments)
    {
        rad = grapdraw_DegToRad(count*approx_degree);
        xp1 = x + (int8_t)(sin(rad)*radius);
        yp1 = y + (int8_t)(cos(rad)*radius);    
        count++;
        if(count != approx_segments) {
            rad = grapdraw_DegToRad(count*approx_degree);
        } else {
            rad = grapdraw_DegToRad(loc_sweep);
        }
        xp2 = x + (int8_t)(sin(rad)*radius);
        yp2 = y + (int8_t)(cos(rad)*radius);    
        grapdraw_Line(d, xp1,yp1,xp2,yp2,color);
    }
    
    return;
}

/*
 * Draw arc with radius line
 * Angle is beginning from 4 quart of trigonometric circle (3pi/2)
 * start_angle: start angle in degree
 * sweep: finish angle in degree
 */
void grapdraw_DrawArcWithRadiusLine(lcddev_t *d, uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, int color) {
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
    
    loc_sweep = grapdraw_NormalizeTo0_360(sweep);
    
    count = (grapdraw_NormalizeTo0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_degree = loc_sweep / (float)approx_segments;

    rad = grapdraw_DegToRad(count*approx_degree);
    uint8_t first_point_x = x + (int8_t)(sin(rad)*radius);
    uint8_t first_point_y = y + (int8_t)(cos(rad)*radius);   
    while (count < approx_segments) {
        rad = grapdraw_DegToRad(count*approx_degree);
        xp1 = x + (int8_t)(sin(rad)*radius);
        yp1 = y + (int8_t)(cos(rad)*radius);    
        count++;
        if (count != approx_segments) {
            rad = grapdraw_DegToRad(count*approx_degree);
        } else {
            rad = grapdraw_DegToRad(loc_sweep);
        }
        xp2 = x + (int8_t)(sin(rad)*radius);
        yp2 = y + (int8_t)(cos(rad)*radius);    
        grapdraw_Line(d, xp1,yp1,xp2,yp2,color);
    }
    
    // Radius line
    grapdraw_Line(d, x,y,first_point_x,first_point_y,color);
    grapdraw_Line(d, x,y,xp2,yp2,color);
    return;
}

/* Draw circle by Bresenhem's algorithm */
void grapdraw_DrawCircle(lcddev_t *d, uint8_t par_x,uint8_t par_y,uint8_t par_r,int par_color) {
    int32_t x = -par_r;
    int32_t y = 0;
    int32_t err = 2 - 2 * par_r;
    int32_t e2;

    if (par_x >= d->frameWidth || par_y >= d->frameHeight) {
        return;
    }

    do {
        fontdraw_drawpixelBW(d, par_x - x, par_y + y, par_color);
        fontdraw_drawpixelBW(d, par_x + x, par_y + y, par_color);
        fontdraw_drawpixelBW(d, par_x + x, par_y - y, par_color);
        fontdraw_drawpixelBW(d, par_x - x, par_y - y, par_color);
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
void grapdraw_FillCircle(lcddev_t *d, uint8_t par_x,uint8_t par_y,uint8_t par_r,int par_color) {
    int32_t x = -par_r;
    int32_t y = 0;
    int32_t err = 2 - 2 * par_r;
    int32_t e2;

    if (par_x >= d->frameWidth || par_y >= d->frameHeight) {
        return;
    }

    do {
        for (uint8_t _y = (par_y + y); _y >= (par_y - y); _y--) {
            for (uint8_t _x = (par_x - x); _x >= (par_x + x); _x--) {
                fontdraw_drawpixelBW(d, _x, _y, par_color);
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
void grapdraw_DrawRectangle(lcddev_t *d, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int color) {
    grapdraw_Line(d, x1,y1,x2,y1,color);
    grapdraw_Line(d, x2,y1,x2,y2,color);
    grapdraw_Line(d, x2,y2,x1,y2,color);
    grapdraw_Line(d, x1,y2,x1,y1,color);

    return;
}

/* Draw a filled rectangle */
void grapdraw_FillRectangle(lcddev_t *d, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int color) {
    uint8_t x_start = ((x1<=x2) ? x1 : x2);
    uint8_t x_end   = ((x1<=x2) ? x2 : x1);
    uint8_t y_start = ((y1<=y2) ? y1 : y2);
    uint8_t y_end   = ((y1<=y2) ? y2 : y1);

    for (uint8_t y= y_start; (y<= y_end)&&(y<d->frameHeight); y++) {
        for (uint8_t x= x_start; (x<= x_end)&&(x<d->frameWidth); x++) {
            fontdraw_drawpixelBW(d, x, y, color);
        }
    }
    return;
}

/* Draw a bitmap */
void grapdraw_DrawBitmap(lcddev_t *d, uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, int color) {
    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    if (x >= d->frameWidth || y >= d->frameHeight) {
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
                fontdraw_drawpixelBW(d, x + i, y, color);
            }
        }
    }
    return;
}

#endif

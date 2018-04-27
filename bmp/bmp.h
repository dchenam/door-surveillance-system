#ifndef __BMP_H
#define __BMP_H

#include "stm32f10x.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;

typedef struct tagBITMAPFILEHEADER
{
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, tagBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, tagBITMAPINFOHEADER;

typedef struct tagRGBQUAD
{
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
} RGBQUAD, tagRGBQUAD;

typedef struct RGB_PIXEL
{
    unsigned char rgbBlue;
    unsigned char rgbGreen;
    unsigned char rgbRed;
} RGB_PIXEL;

#define GETR_FROM_RGB16(RGB565) ((unsigned char)((((unsigned short int)RGB565) >> 11) << 3))
#define GETG_FROM_RGB16(RGB565) ((unsigned char)((((unsigned short int)(RGB565 & 0x7ff)) >> 5) << 2))
#define GETB_FROM_RGB16(RGB565) ((unsigned char)((((unsigned short int)(RGB565 & 0x1f)) << 3)))
#pragma diag_suppress 870

void LCD_Show_BMP(uint16_t x, uint16_t y, char *pic_name);
int Screen_Shot(uint16_t x, uint16_t y, uint16_t Width, uint16_t Height, char *filename);

#endif /* __BMP_H */

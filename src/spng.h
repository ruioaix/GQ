#ifndef SPNG_H
#define SPNG_H 

#include <stdint.h>
#include <stdlib.h>
#include <png.h>

/* A coloured pixel. */

typedef struct {
	png_byte red;
	png_byte green;
	png_byte blue;
} PIXEL;

/* A picture. */

typedef struct  {
	PIXEL *pixels;
	size_t width;
	size_t height;
} BITMAP;

void setPIXEL(BITMAP *bitmap, int x, int y, uint8_t red, uint8_t green, uint8_t blue);
void getPIXEL(BITMAP *bitmap, int x, int y, uint8_t *red, uint8_t *green, uint8_t *blue);
void savePNG(BITMAP *bitmap, const char *path);

#endif

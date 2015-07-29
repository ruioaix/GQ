#include <stdio.h>
#include "spng.h"
#include "log.h"
#include "utils.h"

void setPIXEL(BITMAP * bitmap, int x, int y, uint8_t red, uint8_t green, uint8_t blue) {
	PIXEL *pi = bitmap->pixels + bitmap->width * y + x;
	pi->red = red;
	pi->green = green;
	pi->blue = blue;
}

void getPIXEL(BITMAP *bitmap, int x, int y, uint8_t *red, uint8_t *green, uint8_t *blue) {
	PIXEL *pi = bitmap->pixels + bitmap->width * y + x;
	*red = pi->red;
	*green = pi->green;
	*blue = pi->blue;
}

/* Write "bitmap" to a PNG file specified by "path"; returns 0 on
 *    success, non-zero on error. */
void savePNG(BITMAP *bitmap, const char *path) {
	FILE * fp = sfopen (path, "wb");

	png_structp png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) LOG(LOG_FATAL, "png_create_write_struct failed");

	png_infop info_ptr = png_create_info_struct (png_ptr);
	if (!info_ptr) LOG(LOG_FATAL, "png_create_info_struct failed");

	png_set_IHDR (png_ptr, info_ptr, bitmap->width, bitmap->height, 
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_byte **row_pointers = png_malloc(png_ptr, bitmap->height * sizeof (png_byte *));

	size_t i, j;
	for (i = 0; i < bitmap->height; ++i) {
		png_byte *row = png_malloc(png_ptr, sizeof(png_byte) * bitmap->width * 3);
		row_pointers[i] = row;
		for (j = 0; j < bitmap->width; ++j) {
			getPIXEL(bitmap, j, i, row, row + 1, row + 2);
			row += 3;
		}
	}

	/* Write the image data to "fp". */
	png_init_io(png_ptr, fp);
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	for (i = 0; i < bitmap->height; i++) {
		png_free(png_ptr, row_pointers[i]);
	}
	png_free(png_ptr, row_pointers);
	png_destroy_write_struct(&png_ptr,  &info_ptr);
	fclose(fp);
}


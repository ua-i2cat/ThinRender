/*
 *  thin render - Mobile render engine based on OpenGL ES 2.0
 *  Copyright (C) 2013  Fundació i2CAT, Internet i Innovació digital a Catalunya
 *
 *  This file is part of thin render.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Author:         Marc Fernandez Vanaclocha <marc.fernandez@i2cat.net>
 */

#ifndef IMAGE_H
#define IMAGE_H
#include <stdlib.h>
#include "../log/Log.h"
/*
@png.h:
#define PNG_COLOR_MASK_PALETTE    1
#define PNG_COLOR_MASK_COLOR      2
#define PNG_COLOR_MASK_ALPHA      4
#define PNG_COLOR_TYPE_GRAY 0
#define PNG_COLOR_TYPE_PALETTE  (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_PALETTE)
#define PNG_COLOR_TYPE_RGB        (PNG_COLOR_MASK_COLOR)
#define PNG_COLOR_TYPE_RGB_ALPHA  (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA)
#define PNG_COLOR_TYPE_GRAY_ALPHA (PNG_COLOR_MASK_ALPHA)
#define PNG_COLOR_TYPE_RGBA  PNG_COLOR_TYPE_RGB_ALPHA
#define PNG_COLOR_TYPE_GA  PNG_COLOR_TYPE_GRAY_ALPHA
*/
class Image {
public:
	const static int RGB = 2;
	const static int RGBA = 6;
	unsigned int 	 width;///width of the image
	unsigned int 	 height;///height of the image
	unsigned int 	 bytes_per_pixel; ///bytes per pixel 2:RGB, 6:RGBA
	unsigned char*	 pixel_data;/// pixel data width * height * bytes per pixel + 1
	Image(int w, int h, int bytes, int sizeBuffer){
		width = w;
		height = h;
		bytes_per_pixel = bytes;
		pixel_data = 0;
		pixel_data = (unsigned char*)malloc(sizeBuffer+1);
	}

	~Image(){
		if(pixel_data != 0){
			delete[] pixel_data;
        }
	}
};
#endif

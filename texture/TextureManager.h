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

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "Image.h"
#include "Texture.h"

#include <string>
#include <map>
#include "../libs/libpng/png.h"

using namespace std;

///internal use to decode png
typedef struct
{
    const unsigned char *buffer;
    png_uint_32 bufsize;
    png_uint_32 current_pos;

} MEMORY_READER_STATE;

/**
 * TextureManager class controlls all Texture instances and decoding, uses Singleton pattern
 */
class TextureManager {
public:
	static TextureManager* getInstance();
	static void freeInstance();
	///read_data_memory used internally to decode png, maybe private?
    static void read_data_memory(png_structp png_ptr, png_bytep data, png_uint_32 length);
    /**
     * getTexture
     * @param std::string image file name from assets folder
     * @return Texture*
     */
	Texture* getTexture(string name);
private:
	map<string, Texture*> textureMap;
	TextureManager();
	~TextureManager();

	Image* loadImageMemoryBMP(const void *buffer, int bufsize);
	Image* loadImageMemoryJPEG(const void *buffer, int bufsize);
	Image* loadImageMemoryPNG(const void *buffer, int bufsize);

	static TextureManager* instance;
};
#endif

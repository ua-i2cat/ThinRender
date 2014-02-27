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

#include "TextureManager.h"
#include "../fileSystem/FileSystem.h"
#include "../Platform.h"
#include "../log/Log.h"

#ifdef ANDROID_PLATFORM
extern "C" {
	#include "../utils/JPEG.h"
}
#else
#include "../utils/JPEG.h"
#endif

TextureManager* TextureManager::instance = 0;

TextureManager* TextureManager::getInstance(){
	if(instance == 0){
		instance = new TextureManager();
	}
	return instance;
}

void TextureManager::freeInstance(){
	if(TextureManager::instance != 0){
		delete instance;
		TextureManager::instance = 0;
	}
}

TextureManager::TextureManager(){

}
TextureManager::~TextureManager(){
    for(map<string, Texture*>::iterator it = textureMap.begin(); it != textureMap.end(); ++it){
        delete (it->second);
        (it->second) = 0;
    }
	textureMap.clear();
}
void TextureManager::deleteAllNotMarkedTextures(){
	for(map<string, Texture*>::iterator it = textureMap.begin(); it != textureMap.end(); ++it){
		if((it->second)->markedNotToErase()) continue;
		delete (it->second);
		(it->second) = 0;
		textureMap.erase (it);
	}
}

Texture* TextureManager::getTexture(std::string name){
	std::map<std::string, Texture*>::iterator it;
	it = textureMap.find(name);
	if(it != textureMap.end()){//textura previament carregada
		return (it->second);
	}
	if(FileSystem::getInstance()->openFile(name) == -1){
		logErr("Error TextureManager::getTexture opening %s: ", name.c_str());
	}

	bool foundPNG = (name.find(".png") != std::string::npos);
	bool foundBMP = (name.find(".bmp") != std::string::npos);
	bool foundJPEG = (name.find(".jpg") != std::string::npos) || (name.find(".jpeg") != std::string::npos);

	Image* image = 0;
	if (foundPNG) {
		image = loadImageMemoryPNG((const void *)FileSystem::getInstance()->getFileData(name), FileSystem::getInstance()->getFileSize(name));
	} else if (foundBMP) {
		image = loadImageMemoryBMP((const void *)FileSystem::getInstance()->getFileData(name), FileSystem::getInstance()->getFileSize(name));
	} else if (foundJPEG) {
		image = loadImageMemoryJPEG((const void *)FileSystem::getInstance()->getFileData(name), FileSystem::getInstance()->getFileSize(name));
	}

	// TODO: Error handling?

	textureMap[name] = new Texture(image);
	delete image;
	FileSystem::getInstance()->destroyFileData(name);
	return textureMap[name];
}

void TextureManager::read_data_memory(png_structp png_ptr, png_bytep data, png_uint_32 length)
{
   MEMORY_READER_STATE *f = (MEMORY_READER_STATE *)png_get_io_ptr(png_ptr);
   if (length > (f->bufsize - f->current_pos)){
	   logErr("Texture manager read error in read_data_memory (loadpng)");
   }
   memcpy(data, f->buffer + f->current_pos, length);
   f->current_pos += length;
}

Image* TextureManager::loadImageMemoryBMP(const void *buffer, int bufsize)
{
	logInf("TextureManager::loadImageMemoryBMP: start processing");
	unsigned char *inputBuffer = (unsigned char *) buffer;

	unsigned int pixelInputOffset = (unsigned int) (*(inputBuffer + 0x0A));

	unsigned int widthInPixels = (unsigned int) (*(inputBuffer + 0x12));
	unsigned int heightInPixels = (unsigned int) (*(inputBuffer + 0x16));

	unsigned int size = widthInPixels * heightInPixels * 3;

	logInf("BMP data: pixel_data offset: %d - width: %d - height: %d - size %d", pixelInputOffset, widthInPixels, heightInPixels, size);

	logInf("Creating a new Image");
	Image* image = new Image(widthInPixels, heightInPixels, 3, size);

	// TODO: check bufsize

	logInf("About to memcpy!");

	for (int i = 0; i < size; i += 3) {
		unsigned char b = (inputBuffer + pixelInputOffset + i)[0];
		unsigned char r = (inputBuffer + pixelInputOffset + i)[2];
		(inputBuffer + pixelInputOffset + i)[0] = r;
		(inputBuffer + pixelInputOffset + i)[2] = b;
	}

	memcpy(image->pixel_data, inputBuffer + pixelInputOffset, size);

	return image;
}

Image* TextureManager::loadImageMemoryJPEG(const void *buffer, int bufsize)
{
    int size, w, h;
    parseJPEG((const char *)buffer, bufsize, &size, &w, &h);
    Image* image = new Image(w, h, 2, size);

    int ret = decompressJPEG((char const *)buffer, bufsize, (char *)image->pixel_data);
    logInf("TextureManager::loadImageMemoryJPEG: processed size %d, expected size %d", ret, size);

    if (ret == 0) {
    	logErr("TextureManager::loadImageMemoryJPEG: processed size is 0");
    	return 0;
    }

    return image;
}

Image* TextureManager::loadImageMemoryPNG(const void *buffer, int bufsize)
{
	png_structp png_ptr;
	png_infop info_ptr;
	MEMORY_READER_STATE memory_reader_state;

	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;

	if (!buffer || (bufsize <= 0)) return NULL;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		return NULL;
	}

	png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, (png_error_ptr) NULL);

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		return NULL;
	}

	memory_reader_state.buffer = (unsigned char *)buffer;
	memory_reader_state.bufsize = bufsize;
	memory_reader_state.current_pos = 0;

	png_set_read_fn(png_ptr, &memory_reader_state, (png_rw_ptr)TextureManager::read_data_memory);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

	png_set_sig_bytes(png_ptr, sig_read);
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);

	if (color_type == PNG_COLOR_TYPE_PALETTE) {
	    png_set_palette_to_rgb(png_ptr);
	}
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
	    png_set_expand_gray_1_2_4_to_8(png_ptr);
	}
	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
	    png_set_gray_to_rgb(png_ptr);
	}
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
	    png_set_tRNS_to_alpha(png_ptr);
	}
	if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_MASK_COLOR) {
	    //png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
        png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
	}

	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);

	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

	png_read_update_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

	Image* image = new Image(width, height, color_type, png_get_rowbytes(png_ptr, info_ptr)*height);
	png_bytep* row_p = new png_bytep [image->height];

	int i;
	for (i = 0; i < height; i++)
	{
		row_p[height - 1 - i] = &(image->pixel_data[png_get_rowbytes(png_ptr, info_ptr)*i]);
	}
	png_read_image(png_ptr, row_p);
	delete [] row_p;

	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return image;
}

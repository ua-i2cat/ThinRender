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

#include "Texture.h"

#include "../log/Log.h"

Texture::Texture(Image* image){
	xRatio = 1.0f;
	yRatio = 1.0f;
	nonPowerOfTwo = false;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//);GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	GLint internalFormat = GL_RGB;
	format = GL_RGB;
	logInf("image w:%i h:%i format:%i",image->width,image->height,image->bytes_per_pixel);

	xSquare = getNextPowerOf2(image->width);
	ySquare = getNextPowerOf2(image->height);

	widthOriginal = image->width;
	heightOriginal = image->height;

	if(image->bytes_per_pixel == Image::RGBA){
		internalFormat = GL_RGBA;
		format = GL_RGBA;
	}

	if(xSquare != image->width || ySquare != image->height){
		nonPowerOfTwo = true;
		logWar("the current image isn't a power of two w:%i h:%i the image will be of w:%i h:%i and cropped", image->width, image->height, xSquare, ySquare);
		xRatio = (float)image->width / xSquare;
		yRatio = (float)image->height / ySquare;
		glTexImage2D(GL_TEXTURE_2D, 		// target
				0,  						// level, 0 = base, no minimap,
				internalFormat,			 	// internalformat
				xSquare,  					// width
				ySquare, 					// height
				0,  						// border, always 0 in OpenGL ES
				format,						// format
				GL_UNSIGNED_BYTE, 			// type
				0);
		glTexSubImage2D(GL_TEXTURE_2D, 		// target
				0,  						// level, 0 = base, no minimap,
				0,							//xoffset
				0,							//yoffset
				image->width,  				// width
				image->height, 				// height
				format,						// format
				GL_UNSIGNED_BYTE, 			// type
				image->pixel_data);
		return;
	}


	glTexImage2D(GL_TEXTURE_2D, 		// target
			0,  						// level, 0 = base, no minimap,
			internalFormat,			 	// internalformat
			image->width,  				// width
			image->height, 				// height
			0,  						// border, always 0 in OpenGL ES
			format,						// format
			GL_UNSIGNED_BYTE, 			// type
			image->pixel_data);
}

Texture::~Texture(){
	glDeleteTextures(1, &textureId);
}
int Texture::getNextPowerOf2(int value)
{
	int result = 1;
	for (int i = 1; i < 12; ++i)
	{
		result = result << 1;
		if (result >= value)
			return result;
	}
	logErr("getNextPowerOf2 value is too large, value %i max %i", value, result);
	return result;
}

//this method updates the texture, we expect the same resolution
void Texture::updateTexture(unsigned char* imageData){
	glBindTexture(GL_TEXTURE_2D, textureId);
	//logInf("updateTexture widthOriginal %i heightOriginal %i format %i GL_RGBA %i imageData %i"
	//		, widthOriginal, heightOriginal, format, GL_RGBA, imageData);
	glTexSubImage2D(GL_TEXTURE_2D, 				// target
					0,  						// level, 0 = base, no minimap,
					0,							//xoffset
					0,							//yoffset
					widthOriginal,  			// width
					heightOriginal, 			// height
					format,						// format
					GL_UNSIGNED_BYTE, 			// type
					imageData);
}

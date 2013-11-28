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

#ifndef TEXTURE_H
#define TEXTURE_H

#include "Image.h"

#include "../OpenGLPlatform.h"
/**
 * Texture class that manages the creation of opengl texture2D in pot resolution
 */
class Texture {
public:
	Texture(Image* image);
	~Texture();
    
    /**
     * setTextureId deletes the previous texture2D
     * @param GLuint newTextureId
     */
	void setTextureId(GLuint newTextureId){
        glDeleteTextures(1, &textureId);
		textureId = newTextureId;
	}
    /**
     * getTextureId returns the texture2D
     * @return GLuint
     */
	GLuint inline getTextureId(){
		return textureId;
	}
    /**
     * getTextureNPOT
     * @return bool
     */
	bool getTextureNPOT(){
		return nonPowerOfTwo;
	}
    /**
     * getXratio
     * @return float
     */
	float getXratio(){
		return xRatio;
	}
    /**
     * getYratio
     * @return float
     */
	float getYratio(){
		return yRatio;
	}
	void updateTexture(unsigned char* imageData);

	/**
	 * getYOriginal
	 * @return int
	 */
	int getXOriginal(){
		return widthOriginal;
	}
	/**
	 * getYOriginal
	 * @return int
	 */
	int getYOriginal(){
		return heightOriginal;
	}

	int getXSquare(){
		return xSquare;
	}

	int getYSquare(){
		return ySquare;
	}
private:
	GLuint textureId;
	GLint uniformTexture;
	bool nonPowerOfTwo;
	float xRatio, yRatio;
	int getNextPowerOf2(int value);
	int widthOriginal, heightOriginal;
	int xSquare, ySquare;
	GLenum format;
};
#endif

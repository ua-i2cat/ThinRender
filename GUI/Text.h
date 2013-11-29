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

#ifndef TEXT_H
#define TEXT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>

#include "../OpenGLPlatform.h"
#include "../log/Log.h"
#include "../utils/Maths.h"

// Maximum texture width
#define MAXWIDTH 1024
/**
 * Class that manages the creation of glyph, atlas and vertex with uv using free type
 */
class Text{
public:
	/** maximum height of an char */
	int maxHeight;
    /** texture objext */
	GLuint tex;
    
    /** width of texture in pixels */
	int w;
    
    /** height of texture in pixels */
    int h;
	struct point {
		GLfloat x;
		GLfloat y;
		GLfloat s;
		GLfloat t;
	};
	struct glyphData{
		float ax;	/// advance.x
		float ay;	/// advance.y
		float bw;	/// bitmap.width;
		float bh;	/// bitmap.height;
		float bl;	/// bitmap_left;
		float bt;	/// bitmap_top;
		float tx;	/// x offset of glyph in texture coordinates
		float ty;	/// y offset of glyph in texture coordinates
	};
    
    glyphData * c;
    
    ///shader, uniforms and vertex buffer object
	GLuint uniform_tex, attribute_coord, uniform_color, uniform_pvmMatrix;
	GLuint programText;
	GLuint vbo;

	/**
     * copy constructor
     * @param Text* text to clone
     */
    Text(Text* text);
	
    /**
     * creates shader, uniform, vbo and glyph data
     * @param FT_Face with 
     */
    Text(FT_Face face, int height);
    
    /**
     * destructor frees all opengl data and glyphData, the destruction of freetype libraries are managed by TextManager
     */
    ~Text();
    /**
     * setText set using with first point x and y
     * @param const char* text the message ended with \0
     * @param float x start
     * @param float y start
     * @param float sx multiplier x
     * @param float sy multiplier y
     */
	void setText(std::string text, float x, float y);
	/** IN PROGRESS */
	void setBlockText(std::string text, float width, int align = 0);
	uint8_t* calculatePointerEndLine(const char *text, float width);
	float calculateLength(const char *start, const char *end);
	int calculateNumWhiteSpaces(const char *start, const char *end);
	/**
     * render function enables the program, set the uniform, the vbo and do a draw call
     */
    void render();
	/**
     * render function enables the program, set the uniform, the vbo and do a draw call, also sets the projection view model matrix
     * @param glm::mat4 pvmMatrix projection view model matrix
     */
    void render(glm::mat4 pvmMatrix);
	/**
     * getWidthOfMessage calculates the width in pixels of the message
     * @param const char* message
     * @param float sx
     */
	float getWidthOfMessage(const char* message);
	/**
     * getHeightOfMessage calculates the height in pixels of the message
     * @param const char* message
     * @param float sy
     */
	float getHeightOfMessage(const char* message);
	int cIndex;
	glm::vec4 color;
private:
	int atlasWidth, atlasHeight;
    std::string parseAndReplaceAccents(std::string text);
};

#endif

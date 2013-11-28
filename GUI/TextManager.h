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

#ifndef TEXT_MANAGER_H
#define TEXT_MANAGER_H

#include "Text.h"
#include <string>
#include <vector>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

/**
 * Class that manages the creation and destruction of Text, also manages the FT_Library and FT_Face, uses Singleton pattern
 */
class TextManager{
public:
	static TextManager* getInstance();
	static void freeInstance();
    /**
     * getText looks first if there is a text with the same font and font size, if is the case, it creates one from the other, otherwise, it creates a new FT_Library and FT_Face
     * @param std::string fontFile path to file with ttf format from assets folder
     * @param int fontSize
     * @return Text* with an empty message
     */
	Text* getText(std::string fontFile, int fontSize);
private:
	//font, size, atlas texture
	std::map<std::string, std::map<int, Text*> > textMap;
	std::vector<Text*> textInstances;
    std::vector<FT_Library> FTLibraryUsed;
    std::vector<FT_Face> FTFaceUsed;
	TextManager();
	~TextManager();
	static TextManager* instance;
};
#endif

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

#include "TextManager.h"
#include "../fileSystem/FileSystem.h"
TextManager* TextManager::instance = 0;

TextManager* TextManager::getInstance(){
	if(instance == 0){
		instance = new TextManager();
	}
	return instance;
}

void TextManager::freeInstance(){
	if(TextManager::instance != 0){
		delete instance;
		TextManager::instance = 0;
	}
}

TextManager::TextManager(){

}
TextManager::~TextManager(){
	std::map<std::string, std::map<int, Text*> >::iterator firstIterator;
	std::map<int, Text*>::iterator secondIterator;
    
    for(int n = 0; n < FTFaceUsed.size(); n++){
        FT_Done_Face(FTFaceUsed.at(n));
    }
    for(int j = 0; j < FTLibraryUsed.size(); j++){
        FT_Done_FreeType(FTLibraryUsed.at(j));
    }
    FTLibraryUsed.clear();
    FTFaceUsed.clear();
	for(int i = 0; i < textInstances.size(); i++){
		delete textInstances[i];//deleting all the texture atlas generated!!
		textInstances[i] = 0;
	}
	textInstances.clear();
	for(firstIterator = textMap.begin(); firstIterator != textMap.end(); ++firstIterator){
		for(secondIterator = firstIterator->second.begin(); secondIterator != firstIterator->second.end(); ++secondIterator){
            (secondIterator->second) = 0;
		}
		firstIterator->second.clear();
	}
	textMap.clear();

}

Text* TextManager::getText(std::string fontFile, int fontSize){
	Text* result = 0;
	std::map<std::string, std::map<int, Text*> >::iterator firstIterator = textMap.find(fontFile);
	if(firstIterator != textMap.end()){//textura previament carregada
		std::map<int, Text*>::iterator secondIterator = (firstIterator->second).find(fontSize);
		if(secondIterator != (firstIterator->second).end()){
			result = new Text(secondIterator->second);
			textInstances.push_back(result);
			return result;
		}
	}
	FT_Library ft;

    
	if(FT_Init_FreeType(&ft)) {
		logErr("Could not init freetype library");
		return 0;
	}

	if(FileSystem::getInstance()->openFile(fontFile) == -1){
		logErr("Error opening %s", fontFile.c_str());
	}

	FT_Face face;

	if(FT_New_Memory_Face(	ft,
							(unsigned char const*)FileSystem::getInstance()->getFileData(fontFile), /* first byte in memory */
							FileSystem::getInstance()->getFileSize(fontFile),    					/* size in bytes        */
							0,        												 				/* face_index           */
							&face )){
		logErr("Could not open font");
		return 0;
	}
	result = new Text(face, fontSize);
	(textMap[fontFile])[fontSize] = result;
	FileSystem::getInstance()->destroyFileData(fontFile);
	textInstances.push_back(result);
    FTLibraryUsed.push_back(ft);
    FTFaceUsed.push_back(face);
	return result;
}

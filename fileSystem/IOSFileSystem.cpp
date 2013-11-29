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

#include "../Platform.h"
#ifdef IOS_PLATFORM

#include "../globalData/GlobalData.h"
#include "IOSFileSystem.h"
#include <string>
#include <stdio.h>

IOSFileSystem::IOSFileSystem(){
	init();
}

IOSFileSystem::~IOSFileSystem(){
	release();
}

int IOSFileSystem::openFile(string filePath){
    if(getFileSize(filePath) != -1){
		return 1;//the file exist and is loaded
	}
    
    std::string filename = GlobalData::getInstance()->iOSPath+"/assets/"+filePath;
    FILE* fp;
    fp = fopen(filename.c_str(), "r");
    if(fp == 0){
        logErr("unable to open asset %s", filename.c_str());
        fclose(fp);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    unsigned long fileSize = ftell(fp);
    rewind(fp);
    
    char* buffer = (char*) malloc(fileSize+1);
    unsigned long readResult = fread(buffer, sizeof(char), fileSize, fp);
    
    if(readResult != fileSize){
        logErr("something bad happens, read result != file size");
        return -1;
    }
    buffer[fileSize] = '\0';
    
    FileStruct* fileStruct = new FileStruct((int)fileSize+1);
	
    memcpy(fileStruct->data, buffer, fileSize+1);
    fileMap.emplace(filePath, fileStruct);
    
    free(buffer);
    fclose(fp);

    return 0;
}

int IOSFileSystem::writeFile(string filePath, const char* content){
    logInf("IOSFileSystem::writeFile not implemented yet");
	return 0;
}

void IOSFileSystem::init(){
}

#endif

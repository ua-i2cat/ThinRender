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

#include "FileSystem.h"
#include "AndroidFileSystem.h"
#include "IOSFileSystem.h"

FileSystem* FileSystem::fileSystemInstance = 0;

FileSystem* FileSystem::getInstance(){
	if(fileSystemInstance == 0){
#ifdef ANDROID_PLATFORM
		fileSystemInstance = new AndroidFileSystem();
#else
		fileSystemInstance = new IOSFileSystem();
#endif
	}
	return fileSystemInstance;
}

void FileSystem::freeInstance(){
	if(FileSystem::fileSystemInstance != 0){
#ifdef ANDROID_PLATFORM
		delete (AndroidFileSystem*)fileSystemInstance;
#else
		delete fileSystemInstance;
#endif
		FileSystem::fileSystemInstance = 0;
	}
}

int FileSystem::openFile(string filePath){
    logInf("FileSystem::openFile not implemented");
	return -1;
}
int FileSystem::writeFile(string filePath, const char* content){
    logInf("FileSystem::openFile not implemented");
	return -1;
}

int FileSystem::getFileSize(string filePath){
	std::map<string, FileStruct * >::iterator it;
	it = fileMap.find(filePath);
	if(it == fileMap.end()){//the file isn't found
		return -1;
	}
	return (it->second)->size;
}
char* FileSystem::getFileData(string filePath){
	std::map<string, FileStruct* >::iterator it;
	it = fileMap.find(filePath);
	if(it == fileMap.end()){//the file isn't found
		logErr("the file %s isn't at fileMap", filePath.c_str());
		return 0;
	}
	return (it->second)->data;
}

void FileSystem::destroyFileData(string filePath){
	std::map<string, FileStruct* >::iterator it;
    it = fileMap.find(filePath);
    if(it == fileMap.end()) return;
    delete[] (it->second)->data;
    (it->second)->data = 0;
    fileMap.erase(it);
}

FileSystem::FileSystem(){
	init();
}

FileSystem::~FileSystem(){
	release();
}

void FileSystem::init(){
}

void FileSystem::release(){
	fileMap.clear();
}

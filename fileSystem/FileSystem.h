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

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <map>
#include "../platform.h"
#include "../log/log.h"

using namespace std;

/**
 * Data structure, a simple char* and size
 */
class FileStruct{
public:
	char* data;
	int size;
	FileStruct(int size){
		data = (char*)malloc(size);
		this->size = size;
	}
	~FileStruct(){
		if(data != 0){
			delete [] data;
			data = 0;
		}
	}
};

/**
 * Base class to file system access, works with Singleton pattern
 */
class FileSystem {
public:
	static FileSystem* getInstance();
	static void freeInstance();

	/**
     * virtual funcion to open file, is reimplemented depending on the SO
     * @param std::string filePath with the path from the assets folder
	 * @return -1 if there is any error
	 * @return 0 if successfully added this one to the hash
	 * @return 1 if it exist
	 */
	virtual int openFile(string filePath);

	/**
	 * Write file function is reimplemented depending on the SO
     * @param std::string filePath with the path
     * @param const char* with data and ending with \0 char
     * @return -1 if there is any error
	 * @return 0 if successfully added the file in the sd
	 */
	virtual int writeFile(string filePath, const char* content);

	/**
     * Get file size function
     * @param std::string filePath with the path
	 * @return -1 if isn't charged
	 * @return real in case that it is charged
	 */
	virtual int getFileSize(string filePath);

	/**
     * Get file data function
     * @param std::string filePath with the path
	 * return 0 if isn't charged
	 * return a valid pointer in other case
	 */
	virtual char* getFileData(string filePath);

	/**
     * Destroy file data function deletes all data from filepath stored in memory (not hard drive)
     * @param std::string filePath with the path
	 */
	virtual void destroyFileData(string filePath);

protected:
	map<string, FileStruct* > fileMap;
	FileSystem();
	~FileSystem();

	virtual void init();
	virtual void release();
	static FileSystem* fileSystemInstance;
};
#endif

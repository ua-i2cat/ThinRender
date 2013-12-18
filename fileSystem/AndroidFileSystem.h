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
#ifndef ANDROID_FILE_SYSTEM_H
#define ANDROID_FILE_SYSTEM_H

#include "FileSystem.h"
#include <android/asset_manager.h>
#include <android_native_app_glue.h>
#include <string>
/**
 * Class to manage the file access on Android OS
 */
class AndroidFileSystem : public FileSystem {
public:
	/**
     * Open file
     * @param std::string with the file path from assets folder
	 * @return -1 if there is any error
	 * @return 0 if successfully added this one to the hash
	 * @return 1 if the file previously exists
	 */
	int openFile(string filePath);
	/**
     * Write file
     * @param std::string with the file path
     * @param const char* content ended with \0
	 * @return -1 if there is any error
	 * @return 0 if successfully added the file in the sd
	 */
	int writeFile(string filePath, const char* content);

	AndroidFileSystem();
	~AndroidFileSystem();

    /**
     * setAssetManager is the one difference between so to access to the assets folder, 
     * it needs to be setted externally from main
     * @param ANativeActivity* can be extracted from main function
     */
	void setAssetManager(ANativeActivity* nativeActivity);

    int getFileDescriptor(string filePath, long* start, long* end);
protected:
	std::string dataPath;
	AAssetManager* assetManager;
	virtual void init();
};
#endif

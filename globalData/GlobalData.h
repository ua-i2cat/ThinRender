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

#ifndef GLOBAL_DATA_H
#define GLOBAL_DATA_H

#include "../Platform.h"
class BasicSceneManager;
#include "../scene/BasicSceneManager.h"
#include <string>
#include <stack>
#ifdef ANDROID_PLATFORM
#include <android/native_window_jni.h>
#endif

/**
 * Class to manage global parameters, used to share OS data to the project, uses Singleton pattern
 */
class GlobalData {
public:
	static GlobalData* getInstance(){
		if(instance == 0){
			instance = new GlobalData();
		}
		return instance;
	}
#ifdef ANDROID_PLATFORM
    struct java_env {
        JavaVM* javaVM;
		jclass activityClass;
		JNIEnv *env;
		jobject activityObj;
		ANativeWindow* _theNativeWindow;
	} ;
	
    
	struct android_app* app;
	struct java_env jenv;
#endif
    int screenWidth, screenHeight, screenDPI;
    bool isMobileDevice;
    BasicSceneManager* scene;
    std::string iOSPath;
    int screenMode;
    const static int VERTICAL_SCREEN = 0;
    const static int HORIZONTAL_SCREEN = 1;

    std::stack<int> appSceneStack;

private:
	GlobalData(){
		screenMode = HORIZONTAL_SCREEN;
		isMobileDevice = true;
	}
	~GlobalData(){
	}
	static GlobalData* instance;
};
#endif

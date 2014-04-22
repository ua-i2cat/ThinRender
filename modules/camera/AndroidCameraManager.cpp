/*
 *  thin render - Mobile render engine based on OpenGL ES 2.0
 *  Copyright (C) 2013  FundaciÛ i2CAT, Internet i InnovaciÛ digital a Catalunya
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
 *  Author:         Daniel Cabrera Benitez <dcabrera@quarkfly.com>
 *  Author:         Antonio Quesada Frias <aquesada@quarkfly.com>
 */

#include "AndroidCameraManager.h"

AndroidCameraManager::AndroidCameraManager(){
    logInf("AndroidCameraManager: created");
    methodUpdateCamera = 0;
    stopCamera = 0;
}

void AndroidCameraManager::closeCamera(){
    GlobalData *ginstance = GlobalData::getInstance();
    if(ginstance->jenv.javaVM == 0)return;
	logInf("AndroidCameraManager: closeCamera");
	(ginstance->jenv.javaVM)->AttachCurrentThread(&(ginstance->jenv.env), 0);
	(ginstance->jenv.env)->CallVoidMethod(ginstance->jenv.activityObj, stopCamera);
}

void AndroidCameraManager::updateTextureCamera(){
    GlobalData *ginstance = GlobalData::getInstance();
    (ginstance->jenv.env)->CallVoidMethod(ginstance->jenv.activityObj, methodUpdateCamera);
}

bool AndroidCameraManager::setCameraTexturePreview(int texture){
    GlobalData *ginstance = GlobalData::getInstance();
    logInf("setCameraTexturePreview");
	if (ginstance->jenv.javaVM == 0) {
		logErr("TextureSurface::setCameraTexturePreview Error: javaVM is NULL");
		return 0;
	}
	(ginstance->jenv.javaVM)->AttachCurrentThread(&(ginstance->jenv.env), 0);
	jmethodID method = (ginstance->jenv.env)->GetMethodID(ginstance->jenv.activityClass, "initSurfaceTextureCamera", "(I)V");
	if (method == 0) {
		logErr("TextureSurface::setTextureSurface Error: could not find createSurfaceFromTexture method");
		return false;
	}
	methodUpdateCamera = (ginstance->jenv.env)->GetMethodID(ginstance->jenv.activityClass, "updateTextureCamera", "()V");
	if (methodUpdateCamera == 0) {
		logErr("TextureSurface::setTextureSurface Error: could not find createSurfaceFromTexture method");
		return false;
	}
	stopCamera = (ginstance->jenv.env)->GetMethodID(ginstance->jenv.activityClass, "stopCamera", "()V");
	if (stopCamera == 0) {
		logErr("TextureSurface::setTextureSurface Error: could not find createSurfaceFromTexture method");
		return false;
	}
	(ginstance->jenv.env)->CallVoidMethod(ginstance->jenv.activityObj, method, texture);
	return true;
}

AndroidCameraManager::~AndroidCameraManager(){
    logInf("AndroidCameraManager: destructor");
}

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
#include <stdint.h>
#include "TextureWindow.h"


#ifdef __cplusplus
extern "C"
{
#endif

JavaVM* javaVM = 0;
jclass activityClass;
JNIEnv *env;
jobject activityObj;
ANativeWindow* _theNativeWindow;
jmethodID methodUpdateVideo = 0;
jmethodID methodUpdateCamera = 0;
jmethodID stopCamera = 0;
jmethodID stopVideo = 0;

void shutdownTextureWindow(){
	if(javaVM == 0)return;
	javaVM->AttachCurrentThread(&env, 0);
	if(stopCamera != 0){
		env->CallVoidMethod(activityObj, stopCamera);
	}
	if(stopVideo != 0){
		env->CallVoidMethod(activityObj, stopVideo);
	}
	javaVM->DetachCurrentThread();
	javaVM = 0;
}

void closeVideo(){
	if(javaVM == 0)return;
	logInf("closeVideo");
	javaVM->AttachCurrentThread(&env, 0);
	env->CallVoidMethod(activityObj, stopVideo);
}

void closeCamera(){
	if(javaVM == 0)return;
	logInf("closeCamera");
	javaVM->AttachCurrentThread(&env, 0);
	env->CallVoidMethod(activityObj, stopCamera);
}

void updateTextureVideo(){
	env->CallVoidMethod(activityObj, methodUpdateVideo);
}

void updateTextureCamera(){
	env->CallVoidMethod(activityObj, methodUpdateCamera);
}

ANativeWindow* getANativeWindow(int texture) {
	logInf("getANativeWindow START!");
	if (javaVM == 0) {
		logErr("TextureSurface::setTextureSurface Error: javaVM is NULL");
		return 0;
	}

	javaVM->AttachCurrentThread(&env, 0);
	jmethodID method = env->GetMethodID(activityClass, "initSurfaceTextureVideo", "(I)V");
	if (method == 0) {
		logErr("TextureSurface::setTextureSurface Error: could not find createSurfaceFromTexture method");
		return 0;
	}
	methodUpdateVideo = env->GetMethodID(activityClass, "updateTextureVideo", "()V");
	if (methodUpdateVideo == 0) {
		logErr("TextureSurface::setTextureSurface Error: could not find createSurfaceFromTexture method");
		return 0;
	}
	stopVideo = env->GetMethodID(activityClass, "stopVideo", "()V");
	if (stopVideo == 0) {
		logErr("TextureSurface::setTextureSurface Error: could not find createSurfaceFromTexture method");
		return 0;
	}
	logInf("getANativeWindow A!");
	env->CallVoidMethod(activityObj, method, texture);

	return _theNativeWindow;
}

bool setCameraTexturePreview(int texture) {
	logInf("setCameraTexturePreview");
	if (javaVM == 0) {
		logErr("TextureSurface::setCameraTexturePreview Error: javaVM is NULL");
		return 0;
	}

	javaVM->AttachCurrentThread(&env, 0);
	jmethodID method = env->GetMethodID(activityClass, "initSurfaceTextureCamera", "(I)V");
	if (method == 0) {
		logErr("TextureSurface::setTextureSurface Error: could not find createSurfaceFromTexture method");
		return false;
	}
	methodUpdateCamera = env->GetMethodID(activityClass, "updateTextureCamera", "()V");
	if (methodUpdateCamera == 0) {
		logErr("TextureSurface::setTextureSurface Error: could not find createSurfaceFromTexture method");
		return false;
	}
	stopCamera = env->GetMethodID(activityClass, "stopCamera", "()V");
	if (stopCamera == 0) {
		logErr("TextureSurface::setTextureSurface Error: could not find createSurfaceFromTexture method");
		return false;
	}
	env->CallVoidMethod(activityObj, method, texture);
	return true;
}

JNIEXPORT void JNICALL
Java_net_i2cat_modernismemnactec_TextureSurfaceActivity_cacheJNIVars(JNIEnv *envParam, jobject jobj)
{
	envParam->GetJavaVM(&javaVM);
	jclass cls = envParam->GetObjectClass(jobj);
	activityClass = (jclass) envParam->NewGlobalRef(cls);
	activityObj = envParam->NewGlobalRef(jobj);
	javaVM->AttachCurrentThread(&envParam, 0);
	env = envParam;
	logInf("==========================CACHE JNI VARS!");
}

JNIEXPORT void JNICALL
Java_net_i2cat_modernismemnactec_TextureSurfaceActivity_setSurface(JNIEnv *env, jclass clazz, jobject surface)
{
	// obtain a native window from a Java surface
	_theNativeWindow = ANativeWindow_fromSurface(env, surface);
}

#ifdef __cplusplus
}
#endif

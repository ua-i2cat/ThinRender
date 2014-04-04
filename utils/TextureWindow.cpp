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

//TODO: Quarkfly Refactor
#include "../modules/sensors/LocationManager.h"




#ifdef ANDROID_PLATFORM



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

//gps methods...
jmethodID shutdownGPSMethod = 0;
jmethodID getLatitudeMethod = 0;
jmethodID getLongitudeMethod = 0;
jmethodID initGPSMethod = 0;

extern double getLatitude(){
	if(javaVM == 0)return 0.0;
	if(getLatitudeMethod == 0){
		javaVM->AttachCurrentThread(&env, 0);
		getLatitudeMethod = env->GetMethodID(activityClass, "getLatitude", "()D");
	}
	return env->CallDoubleMethod(activityObj, getLatitudeMethod);
}
double getLongitude(){
	if(javaVM == 0)return 0.0;
	if(getLongitudeMethod == 0){
		javaVM->AttachCurrentThread(&env, 0);
		getLongitudeMethod = env->GetMethodID(activityClass, "getLongitude", "()D");
	}
	return env->CallDoubleMethod(activityObj, getLongitudeMethod);
}

void initGPS(){
	//return;
	logInf("initGPS");
	if(javaVM == 0)return;
	javaVM->AttachCurrentThread(&env, 0);
	if(initGPSMethod != 0){ return; }
	initGPSMethod = env->GetMethodID(activityClass, "initGPS", "()V");//Ljava/lang/Object;
	env->CallVoidMethod(activityObj, initGPSMethod, activityObj);
	return;/*
	logInf("initGPS1");
	if(initGPSMethod != 0){ return; }

	logInf("initGPS2");
	javaVM->AttachCurrentThread(&env, 0);
	logInf("initGPS3");
	initGPSMethod = env->GetMethodID(activityClass, "initGPS", "(Ljava/lang/Object;)V");
	if(initGPSMethod == 0) logErr("AL LORO EH!");

	logInf("initGPS4");
	//jobject jListenerObj;
	jmethodID midGetSystemService = env->GetMethodID(activityClass,"getSystemService","(Ljava/lang/String;)Ljava/lang/Object;");
	logInf("initGPS5");
	jstring StringArg = env->NewStringUTF("location");//Context.LOCATION_SERVICE = "location"
	jobject jSystemServiceObj = env->CallObjectMethod(activityObj,midGetSystemService,StringArg);//new GPSListener((LocationManager)this.getSystemService(Context.LOCATION_SERVICE));
	logInf("initGPS6");
	//jclass listenerClass = env->FindClass("net/i2cat/modernismemnactec/GPSListener");
	//if(listenerClass == 0) logErr("FIND CLASS NULL!!!!!!!!");
	//logInf("initGPS6.5");
	//jmethodID midConstListener = env->GetMethodID(listenerClass, "<init>", "(Ljava/lang/Object;)V");
	//logInf("initGPS7");
	//jListenerObj = env->NewObject(listenerClass, midConstListener);
	logInf("initGPS8");
	if(jSystemServiceObj == 0) logErr("FIND CLASS NULL!!!!!!!!");
	env->CallVoidMethod(activityObj, initGPSMethod, jSystemServiceObj);
	logInf("initGPS9");*/
}
void shutDownGPS(){
	if(javaVM == 0)return;
		if(shutdownGPSMethod == 0){
			javaVM->AttachCurrentThread(&env, 0);
			shutdownGPSMethod = env->GetMethodID(activityClass, "shutDownGPS", "()V");
		}
		env->CallVoidMethod(activityObj, shutdownGPSMethod);
		shutdownGPSMethod = 0;
		getLatitudeMethod = 0;
		getLongitudeMethod = 0;
		initGPSMethod = 0;
}
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

#else
void initGPS(){
    LocationManager::getInstance()->initGPS();
};
void shutdownTextureWindow(){};
void closeVideo(){};
void closeCamera(){};
void updateTextureVideo(){};
void updateTextureCamera(){};
bool setCameraTexturePreview(int texture){return true;};

double getLatitude(){
    double lat = LocationManager::getInstance()->getLatitude();
    logInf("LAT: %f",lat);
    return lat;
};
double getLongitude(){return LocationManager::getInstance()->getLongitude();};
void shutDownGPS(){ LocationManager::getInstance()->shutDownGPS(); };
#endif

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

#ifndef TEXTURE_WINDOW_H
#define TEXTURE_WINDOW_H

#include <android/native_window_jni.h>
#include "../log/Log.h"


#ifdef __cplusplus
extern "C"
{
#endif

extern void initGPS();
extern void shutdownTextureWindow();
extern void closeVideo();
extern void closeCamera();
extern void updateTextureVideo();
extern void updateTextureCamera();
extern ANativeWindow* getANativeWindow(int texture);
extern bool setCameraTexturePreview(int texture);

extern double getLatitude();
extern double getLongitude();
extern void shutDownGPS();

extern JNIEXPORT void JNICALL
Java_net_i2cat_modernismemnactec_TextureSurfaceActivity_cacheJNIVars(JNIEnv *envParam, jobject jobj);

extern JNIEXPORT void JNICALL
Java_net_i2cat_modernismemnactec_TextureSurfaceActivity_setSurface(JNIEnv *envParam, jclass clazz, jobject surface);

#ifdef __cplusplus
}
#endif

#endif

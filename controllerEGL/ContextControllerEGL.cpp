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

#include "ContextControllerEGL.h"


ContextControllerEGL* ContextControllerEGL::contextControllerInstance = 0;


ContextControllerEGL* ContextControllerEGL::getInstance(){
	if(contextControllerInstance == 0){
		contextControllerInstance = new ContextControllerEGL();
	}
	return contextControllerInstance;
}

ContextControllerEGL::ContextControllerEGL(){
    display = 0;
    surface = 0;
    context = 0;
    width = 0;
    height = 0;
}
int ContextControllerEGL::startDisplay(){
	logInf("creating context");
	/*
	 * Here specify the attributes of the desired configuration.
	 * Below, we select an EGLConfig with at least 8 bits per color
	 * component compatible with on-screen windows
	 */
	/*EGLint const attribs[] = {
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
			EGL_OPENGL_ES2_BIT,
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
	};*/
	EGLint const attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 16,
		EGL_STENCIL_SIZE, 8,
		EGL_NONE
	};
	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	EGLint major = 0;
	EGLint minor = 0;

	int errorEGL;

	int resultEglInitialize = eglInitialize(display, &major, &minor);
	errorEGL = eglGetError();
	if(errorEGL != EGL_SUCCESS) logErr("error a eglInitialize %i",errorEGL);
	logInf("result eglInitialize: %i egl version: %i.%i", resultEglInitialize, major, minor);
	/* Here, the application chooses the configuration it desires. In this
	 * sample, we have a very simplified selection process, where we pick
	 * the first EGLConfig that matches our criteria */
	EGLBoolean resultEglChooseConfig = eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	errorEGL = eglGetError();
	if(errorEGL != EGL_SUCCESS) logErr("error a eglChooseConfig %i",errorEGL);
	logInf("eglChooseConfig %s num of frame buffers returned %i",(resultEglChooseConfig == EGL_TRUE)?"true":"false", numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	bool resultEglGetConfigAttrib = eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	errorEGL = eglGetError();
	if(errorEGL!= EGL_SUCCESS) logErr("error a eglGetConfigAttrib %i",errorEGL);
	logInf("eglGetConfigAttrib %s",(resultEglGetConfigAttrib)?"true":"false");

	ANativeWindow_setBuffersGeometry(GlobalData::getInstance()->app->window, 0, 0, format);

	logInf("ANativeWindow_setBuffersGeometry");

	surface = eglCreateWindowSurface(display, config, GlobalData::getInstance()->app->window, NULL);

	errorEGL = eglGetError();
	if(errorEGL != EGL_SUCCESS) logErr("error a eglCreateWindowSurface %i",errorEGL);
	logInf("GlobalData::getInstance()->app->window : %i", GlobalData::getInstance()->app->window);
	EGLint AttribList[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	context = eglCreateContext(display, config, NULL, AttribList);
	if(EGL_NO_CONTEXT == context) logErr("context failed, eglCreateContext return EGL_NO_CONTEXT");
	errorEGL = eglGetError();
	if(errorEGL != EGL_SUCCESS) logErr("error a eglCreateContext %i",errorEGL);
	logInf("context: %i display: %i surface: %i",context, display,surface);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		logWar("Unable to eglMakeCurrent");
		errorEGL = eglGetError();
		if(errorEGL != EGL_SUCCESS) logErr("error a eglMakeCurrent %i",errorEGL);
		return -1;
	}

	if (eglSwapInterval(display, EGL_MIN_SWAP_INTERVAL) == EGL_FALSE) {
		logWar("Unable to eglSwapInterval EGL_MIN_SWAP_INTERVAL");
		errorEGL = eglGetError();
		if(errorEGL != EGL_SUCCESS) logErr("error a eglSwapInterval %i",errorEGL);
		return -1;
	}
	logInf("successfully setted swap interval to min value:%i (EGL_MIN_SWAP_INTERVAL) (EGL_MAX_SWAP_INTERVAL %i)", EGL_MIN_SWAP_INTERVAL, EGL_MAX_SWAP_INTERVAL);

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	this->display = display;
	this->context = context;
	this->surface = surface;

	int aux = w;
	if(h > w){
		w = h;
		h = aux;
	}
	if(GlobalData::getInstance()->screenMode == GlobalData::VERTICAL_SCREEN){
		aux = h;
		h = w;
		w = aux;
	//	glViewport(0,0,w,h);
	}
	this->width = w;
	this->height = h;

	GlobalData::getInstance()->screenWidth = this->width;
    GlobalData::getInstance()->screenHeight = this->height;

    logInf("w:%i h:%i",w,h);
	logInf("engine w:%i h:%i",this->width,this->height);

	// Initialize GL state.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_STENCIL_TEST);
	int error = glGetError();
	if(error != GL_NO_ERROR)logErr("ERROR %i at glEnable(GL_STENCIL_TEST)", error);

	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	error = glGetError();
	if(error != GL_NO_ERROR)logErr("ERROR %i at glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE)", error);

	GLint iUnits;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &iUnits);
	logInf("MAX TEXTURE UNITS %i", iUnits);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iUnits);
	logInf("GL MAX TEXTURE SIZE %i", iUnits);

	return 0;
}

ContextControllerEGL::~ContextControllerEGL(){
	endDisplay();
}
void ContextControllerEGL::endDisplay(){
    if (this->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(this->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (this->context != EGL_NO_CONTEXT) {
            eglDestroyContext(this->display, this->context);
        }
        if (this->surface != EGL_NO_SURFACE) {
            eglDestroySurface(this->display, this->surface);
        }
        eglTerminate(this->display);
    }
    logInf("engine_term_display");
    this->display = EGL_NO_DISPLAY;
    this->context = EGL_NO_CONTEXT;
    this->surface = EGL_NO_SURFACE;
}

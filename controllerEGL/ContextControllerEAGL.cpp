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

#include "ContextControllerEAGL.h"
#include "../globalData/GlobalData.h"

ContextControllerEAGL* ContextControllerEAGL::contextControllerInstance = 0;


ContextControllerEAGL* ContextControllerEAGL::getInstance(){
	if(contextControllerInstance == 0){
		contextControllerInstance = new ContextControllerEAGL();
	}
	return contextControllerInstance;
}

ContextControllerEAGL::ContextControllerEAGL(){
    context = 0;
    width = 0;
    height = 0;
}
int ContextControllerEAGL::startDisplay(GLKView *view){
	logInf("creating context");
	CGRect screenRect = [[UIScreen mainScreen] bounds];
    width = screenRect.size.width;
    height = screenRect.size.height;
    GlobalData::getInstance()->screenWidth = width;
    GlobalData::getInstance()->screenHeight = height;

    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if(!context || ![EAGLContext setCurrentContext:context]){
        logErr("ContextControllerEAGL::startDisplay context creation error, if %li == 0 context alloc failed else set current context failed", (unsigned long)context);
        return -1;
    }

	logInf("screen w:%i h:%i", width, height);
    view.context = context;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.drawableStencilFormat = GLKViewDrawableStencilFormatNone;

	// Initialize GL state.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//glEnable(GL_STENCIL_TEST);
	//int error = glGetError();
	//if(error != GL_NO_ERROR)logErr("ERROR %i at glEnable(GL_STENCIL_TEST)", error);

	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//int error = glGetError();
	//if(error != GL_NO_ERROR)logErr("ERROR %i at glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE)", error);

	GLint iUnits;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &iUnits);
	logInf("MAX TEXTURE UNITS %i", iUnits);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &iUnits);
	logInf("GL MAX TEXTURE SIZE %i", iUnits);

	return 0;
}

ContextControllerEAGL::~ContextControllerEAGL(){
	endDisplay();
}
void ContextControllerEAGL::endDisplay(){
    logInf("engine_term_display");
    context = 0;
    [EAGLContext setCurrentContext:nil];
}

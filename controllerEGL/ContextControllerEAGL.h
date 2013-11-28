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

#ifndef CONTEXT_CONTROLLER_EGL_H
#define CONTEXT_CONTROLLER_EGL_H

#include "../Platform.h"
#include "../log/Log.h"

#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include <OpenGLES/EAGL.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

/**
 * Class to generate an OpenGL ES 2.0 context using EAGL, works with Singleton pattern
 */
class ContextControllerEAGL {
public:
    /**
     * static function, the first execution or the first after calling freeInstance() calls the private constructor
     */
	static ContextControllerEAGL* getInstance();
    
    EAGLContext* context;/** context pointer*/
    
    int32_t width;/** screen width */
    int32_t height;/** screen height */

    /**
     * Tear down the EAGL context currently associated with the display.
     */
    void endDisplay();

    /**
     * Initialize an EAGL context for the current display.
     * @return 0 if there isn't any problem -1 if can't allocate or assign the context
     */
    int startDisplay(GLKView *view);
private:
    static ContextControllerEAGL* contextControllerInstance;
    ContextControllerEAGL();
	~ContextControllerEAGL();
};
#endif

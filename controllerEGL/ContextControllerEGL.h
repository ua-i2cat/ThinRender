/*
 *  thin render - Mobile render engine based on OpenGL ES 2.0
 *  Copyright (C) 2013  Fundaci— i2CAT, Internet i Innovaci— digital a Catalunya
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
#ifndef CONTEXT_CONTROLLER_EGL_H
#define CONTEXT_CONTROLLER_EGL_H

#include "../log/Log.h"
#include "../globalData/GlobalData.h"

#include <android_native_app_glue.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>

/**
 * Class to generate an OpenGL ES 2.0 context using EGL, works with Singleton pattern
 */
class ContextControllerEGL {
public:
    /**
     * static function, the first execution or the first after calling freeInstance() calls the private constructor
     */
	static ContextControllerEGL* getInstance();
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;

    /**
     * Tear down the EGL context currently associated with the display.
     */
    void endDisplay();

    /**
     * Initialize an EGL context for the current display.
     * @return 0 if there isn't any problem -1 if can't allocate or assign the context
     */
    int startDisplay();
private:
    static ContextControllerEGL* contextControllerInstance;
    ContextControllerEGL();
	~ContextControllerEGL();
};
#endif


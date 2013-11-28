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

#ifndef LOG_H
#define LOG_H

#include "../Platform.h"

/**
 * Log macros
 * in iOS and Android platforms there are 3 kind of logs, all with printf style
 * logInf("Info int %i", i);
 * logWar("Warning!");
 * logErr("somthing wrong happends, error message: %s", stdStringError.c_str());
 */
#ifdef ANDROID_PLATFORM

#include <android/log.h>

#define logTag "Render"
#define logInf(...) ((void)__android_log_print(ANDROID_LOG_INFO, logTag, __VA_ARGS__))
#define logWar(...) ((void)__android_log_print(ANDROID_LOG_WARN, logTag, __VA_ARGS__))
#define logErr(...) ((void)__android_log_print(ANDROID_LOG_ERROR, logTag, __VA_ARGS__))

#else
#include <Foundation/Foundation.h>

#define logInf(fmt, ...) NSLog(@fmt, ##__VA_ARGS__)
#define logWar(fmt, ...) NSLog(@fmt, ##__VA_ARGS__)
#define logErr(fmt, ...) NSLog(@fmt, ##__VA_ARGS__)

#endif

#endif

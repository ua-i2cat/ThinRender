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

#ifndef VIDEO_WRAPPER_IOS_H
#define VIDEO_WRAPPER_IOS_H

#include "../../log/Log.h"
#include "../../Platform.h"
#include "../../controllerEGL/ContextControllerEAGL.h"

#ifndef __cplusplus
extern "C"{
#endif
    
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
    
    extern void createAssetReader(AVURLAsset * inputAsset);
    
    extern void getVideoTextures(GLuint &luminance, GLuint &chrominance);
  
    extern void releaseVideoCache();
    
    extern void readNextVideoFrameFromOutput(AVAssetReaderOutput * readerVideoTrackOutput);
    
    extern void processAsset(AVURLAsset * inputAsset);
    
    extern void startVideo();
    
    extern bool playAction();
    extern bool pauseAction();
    extern void stopAction();
#ifndef __cplusplus
}
#endif

#endif
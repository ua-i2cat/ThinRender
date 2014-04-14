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

#include "VideoDecoder.h"

#include "../../utils/TextureWindow.h"

#ifdef ANDROID_PLATFORM
#include "AndroidVideoDecoder.h"
#else
#include "IOSVideoDecoder.h"
#endif

VideoDecoder* VideoDecoder::instanceVideo = NULL;

VideoDecoder* VideoDecoder::getInstance(RectGUI* rect, std::string path) {
    
    if (!instanceVideo){
#ifdef ANDROID_PLATFORM
        instanceVideo = new AndroidVideoDecoder(rect, path);
#else
        instanceVideo = new IOSVideoDecoder(rect, path);
#endif
    }
    
    return instanceVideo;
}

void VideoDecoder::setEnded(){
	logInf("VideoDecoder::setEnded()!!!!");
	ended = true;
}

bool VideoDecoder::isEnded(){
	if(ended) logInf("ended = true");
	return ended;
}



void VideoDecoder::maximize(){
	textureRect->rotate(-Maths::PI*0.5f);
	float w = GlobalData::getInstance()->screenWidth;
	float h = GlobalData::getInstance()->screenHeight;
	textureRect->setWidthAndHeight(w, h);
	textureRect->setPosition(0.0f, 0.0f);
}

void VideoDecoder::restore(){
	textureRect->rotate(Maths::PI*0.5f);
	textureRect->setWidth(originalWidth);
	textureRect->setHeight(originalHeight);
	textureRect->setPosition(originalLeft, originalTop);
}
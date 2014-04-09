//
//  VideoDecoder.cpp
//  ModernismeMNACTEC
//
//  Created by Daniel Cabrera on 09/04/14.
//  Copyright (c) 2014 i2CAT. All rights reserved.
//

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

void VideoDecoder::updateTexture(){
	updateTextureVideo();
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
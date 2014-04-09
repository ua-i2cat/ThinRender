//
//  IOSVideoDecoder.cpp
//  ModernismeMNACTEC
//
//  Created by Daniel Cabrera on 09/04/14.
//  Copyright (c) 2014 i2CAT. All rights reserved.
//

#include "IOSVideoDecoder.h"

IOSVideoDecoder::IOSVideoDecoder(RectGUI* rect, std::string path){};
IOSVideoDecoder::~IOSVideoDecoder(){instanceVideo = NULL;};

void IOSVideoDecoder::setSource(std::string fileName){}
void IOSVideoDecoder::releaseVideo(){}
void IOSVideoDecoder::play(){}
void IOSVideoDecoder::pause(){}
void IOSVideoDecoder::stop(){}
void IOSVideoDecoder::setMute(bool enable){}
bool IOSVideoDecoder::getMute(){return true;}
bool IOSVideoDecoder::isStopped(){return true;}
bool IOSVideoDecoder::isPaused(){return true;}
bool IOSVideoDecoder::isPlaying(){return true;}

void IOSVideoDecoder::setSplash(std::string texturePath){}
void IOSVideoDecoder::setSplash(){}
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
 *  Author:         Marc Fernandez Vanaclocha <marc.fernandez@i2cat.net>
 */

#ifndef IOS_VIDEO_DECODER_H
#define IOS_VIDEO_DECODER_H

#include "VideoDecoder.h"
#import <AVFoundation/AVFoundation.h>


@class IOSVideoDecoderDelegate;

class IOSVideoDecoder:public VideoDecoder {
public:
	IOSVideoDecoder(RectGUI* rect, std::string path);
	~IOSVideoDecoder();
    
	void setSource(std::string fileName);
	void releaseVideo();
	void play();
	void pause();
	void stop();
	void setMute(bool enable);
	bool getMute();
	bool isStopped();
	bool isPaused();
	bool isPlaying();
    void updateTexture();

    
	void setSplash(std::string texturePath);
	void setSplash();
    void setEnded(bool end);
private:
    IOSVideoDecoderDelegate *videoDecoderObject;

};

@interface IOSVideoDecoderDelegate : NSObject

- (id) init:(IOSVideoDecoder *) vm withURL: (NSURL *) url withTextureId:(GLuint)texture;
- (void) startVideo;
- (void) stopVideo;
- (void) readNextMovieFrame;
- (void) readAudio;
- (void) startPlaying;

@end
#endif
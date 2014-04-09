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

#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H


#include <string>
#include "../../globalData/GlobalData.h"
#include "../../log/Log.h"

class VideoDecoder {
public:
	static VideoDecoder* getInstance(RectGUI* rect, std::string path);
	~VideoDecoder(){};
    
	virtual void setSource(std::string fileName) = 0;
	virtual void releaseVideo() = 0;
	virtual void play() = 0;
	virtual void pause() = 0;
	virtual void stop() = 0;
	virtual void setMute(bool enable) = 0;
	virtual bool getMute() = 0;
	virtual bool isStopped() = 0;
	virtual bool isPaused() = 0;
	virtual bool isPlaying() = 0;
    

    
	virtual void setSplash(std::string texturePath) = 0;
	virtual void setSplash() = 0;
    
    
    void maximize();
	void restore();
	void updateTexture();
    
    void setEnded();
    bool isEnded();


protected:
    static VideoDecoder* videoInstance;
    
    RectGUI* textureRect;
    GLuint textureID;
    
    float originalWidth;
	float originalHeight;
	float originalTop;
	float originalLeft;
    
    std::string sourcePath;
    
    bool ended;

    

};

#endif

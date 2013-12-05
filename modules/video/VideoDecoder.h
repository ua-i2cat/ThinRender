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

#include <OMXAL/OpenMAXAL.h>
#include <OMXAL/OpenMAXAL_Android.h>
#include <pthread.h>

/**
 * VideoDecoder gives the integration of different platforms to decode via hardware
 * This class gives also the option to set source, play, stop, pause and release the video
 * the destructor will shut down the openmax engine
 */
class VideDecoder {
public:
	VideDecoder(RectGUI* rect);
	~VideDecoder();

	void setSource(std::string fileName);
	void releaseVideo();
	void play();
	void pause();
	void stop();

	void updateTexture();
private:
	static XAObjectItf engineObject = NULL;
	static XAEngineItf engineEngine = NULL;
};

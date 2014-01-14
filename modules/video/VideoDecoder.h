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

#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include <OMXAL/OpenMAXAL.h>
#include <OMXAL/OpenMAXAL_Android.h>
#include <pthread.h>

#include <string>

// we're streaming MPEG-2 transport stream data, operate on transport stream block size
#define MPEG2_TS_PACKET_SIZE 188
// number of MPEG-2 transport stream blocks per buffer, an arbitrary number
#define PACKETS_PER_BUFFER 10
// number of required interfaces for the MediaPlayer creation
#define NB_MAXAL_INTERFACES 3 // XAAndroidBufferQueueItf, XAStreamInformationItf and XAPlayItf
// number of buffers in our buffer queue, an arbitrary number
#define NB_BUFFERS 8
// determines how much memory we're dedicating to memory caching
#define BUFFER_SIZE (PACKETS_PER_BUFFER*MPEG2_TS_PACKET_SIZE)

/**
 * VideoDecoder gives the integration of different platforms to decode via hardware
 * This class gives also the option to set source, play, stop, pause and release the video
 * the destructor will shut down the openmax engine
 */
class VideoDecoder {
public:
	VideoDecoder(RectGUI* rect, std::string path);
	~VideoDecoder();

	void setSource(std::string fileName);
	void releaseVideo();
	void play();
	void pause();
	void stop();

	void updateTexture();
private:
	static std::string sourcePath;
	static XAObjectItf engineObject;
	static XAEngineItf engineEngine;

	static XAObjectItf outputMixObject;

	static XAObjectItf playerObj;
	static XAPlayItf playerPlayItf;
	static XAAndroidBufferQueueItf playerBQItf;
	static XAStreamInformationItf playerStreamInfoItf;
	static XAVolumeItf playerVolItf;

	static ANativeWindow* theNativeWindow;

	static char dataCache[BUFFER_SIZE * NB_BUFFERS];

	// where we cache in memory the data to play
	// note this memory is re-used by the buffer queue callback
	static FILE *file;
	// constant to identify a buffer context which is the end of the stream to decode
	static const int kEosBufferCntxt = 1980; // a magic value we can compare against

	static pthread_mutex_t mutex;
	static pthread_cond_t cond;

	static bool discontinuity;
	static bool reachedEof;

	static XAresult AndroidBufferQueueCallback(
			XAAndroidBufferQueueItf caller, void *pCallbackContext, void *pBufferContext, void *pBufferData,
			XAuint32 dataSize, XAuint32 dataUsed, const XAAndroidBufferItem *pItems, XAuint32 itemsLength
		);
	static void StreamChangeCallback(
			XAStreamInformationItf caller, XAuint32 eventId, XAuint32 streamIndex,
			void * pEventData, void * pContext
		);
	static bool enqueueInitialBuffers(bool discontinuity);
	static bool createStreamingMediaPlayer();
	void setPlayingStreamingMediaPlayer();
};

#endif

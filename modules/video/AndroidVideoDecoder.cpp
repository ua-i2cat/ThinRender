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
 *  Authors:         Marc Fernandez Vanaclocha <marc.fernandez@i2cat.net>
 *					 Ignacio Contreras Pinilla <ignacio.contreras@i2cat.net>
 */

#define EGL_EGLEXT_PROTOTYPES

#include <jni.h>
#include <errno.h>
#include <android/input.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2ext.h>

#include "render/Platform.h"
#include "render/OpenGLPlatform.h"
#include "render/controllerEGL/ContextControllerEGL.h"
#include "render/log/Log.h"
#include "render/fileSystem/FileSystem.h"
#include "render/fileSystem/AndroidFileSystem.h"
#include "render/shader/Shader.h"
#include "render/shader/gui/VideoPlaneShader.h"
#include "render/utils/Maths.h"
#include "render/utils/Timer.h"
#include "render/GUI/Rect.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <string>

#include "render/utils/TextureWindow.h"
#include "AndroidVideoDecoder.h"

AndroidVideoDecoder* instanceDecoder;

extern "C" {
	void endCallbackDecoder(XAPlayItf caller, void* context, XAuint32 playevent){
		instanceDecoder->setEnded();
		instanceDecoder->stop();
	}
}

char AndroidVideoDecoder::dataCache[BUFFER_SIZE * NB_BUFFERS];
const int AndroidVideoDecoder::kEosBufferCntxt;

// engine interfaces
XAObjectItf AndroidVideoDecoder::engineObject = NULL;
XAEngineItf AndroidVideoDecoder::engineEngine = NULL;

// output mix interfaces
XAObjectItf AndroidVideoDecoder::outputMixObject = NULL;

// streaming media player interfaces
XAObjectItf AndroidVideoDecoder::playerObj = NULL;
XAPlayItf AndroidVideoDecoder::playerPlayItf = NULL;
XAAndroidBufferQueueItf AndroidVideoDecoder::playerBQItf = NULL;
XAStreamInformationItf AndroidVideoDecoder::playerStreamInfoItf = NULL;
XAVolumeItf AndroidVideoDecoder::playerVolItf = NULL;

// video sink for the player
ANativeWindow* AndroidVideoDecoder::theNativeWindow = NULL;

// handle of the file to play
FILE* AndroidVideoDecoder::file = NULL;

// For mutual exclusion between callback thread and application thread(s).
// The mutex protects reachedEof, discontinuity,
// The condition is signalled when a discontinuity is acknowledged.

pthread_mutex_t AndroidVideoDecoder::mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t AndroidVideoDecoder::cond = PTHREAD_COND_INITIALIZER;

bool AndroidVideoDecoder::discontinuity = false;
bool AndroidVideoDecoder::reachedEof = false;

// AndroidBufferQueueItf callback to supply MPEG-2 TS packets to the media player
XAresult AndroidVideoDecoder::AndroidBufferQueueCallback(
                                                  XAAndroidBufferQueueItf caller,
                                                  void *pCallbackContext,        /* input */
                                                  void *pBufferContext,          /* input */
                                                  void *pBufferData,             /* input */
                                                  XAuint32 dataSize,             /* input */
                                                  XAuint32 dataUsed,             /* input */
                                                  const XAAndroidBufferItem *pItems,/* input */
                                                  XAuint32 itemsLength           /* input */)
{
    XAresult res;
    int ok;
    
    // pCallbackContext was specified as NULL at RegisterCallback and is unused here
    assert(NULL == pCallbackContext);
    
    // note there is never any contention on this mutex unless a discontinuity request is active
    ok = pthread_mutex_lock(&mutex);
    assert(0 == ok);
    
    // was a discontinuity requested?
    if (discontinuity) {
        // Note: can't rewind after EOS, which we send when reaching EOF
        // (don't send EOS if you plan to play more content through the same player)
        if (!reachedEof) {
            // clear the buffer queue
            res = (*playerBQItf)->Clear(playerBQItf);
            assert(XA_RESULT_SUCCESS == res);
            // rewind the data source so we are guaranteed to be at an appropriate point
            rewind(file);
            // Enqueue the initial buffers, with a discontinuity indicator on first buffer
            (void) enqueueInitialBuffers(true);
        }
        // acknowledge the discontinuity request
        discontinuity = false;
        ok = pthread_cond_signal(&cond);
        assert(0 == ok);
        goto exit;
    }
    
    if ((pBufferData == NULL) && (pBufferContext != NULL)) {
        const int processedCommand = *(int *)pBufferContext;
        if (kEosBufferCntxt == processedCommand) {
            logInf("EOS was processed\n");
            // our buffer with the EOS message has been consumed
            assert(0 == dataSize);
            goto exit;
        }
    }
    
    // pBufferData is a pointer to a buffer that we previously Enqueued
    assert((dataSize > 0) && ((dataSize % MPEG2_TS_PACKET_SIZE) == 0));
    assert(dataCache <= (char *) pBufferData && (char *) pBufferData <
           &dataCache[BUFFER_SIZE * NB_BUFFERS]);
    assert(0 == (((char *) pBufferData - dataCache) % BUFFER_SIZE));
    
    // don't bother trying to read more data once we've hit EOF
    if (reachedEof) {
        goto exit;
    }
    
    size_t nbRead;
    // note we do call fread from multiple threads, but never concurrently
    size_t bytesRead;
    bytesRead = fread(pBufferData, 1, BUFFER_SIZE, file);
    if (bytesRead > 0) {
        if ((bytesRead % MPEG2_TS_PACKET_SIZE) != 0) {
            logInf("Dropping last packet because it is not whole");
        }
        size_t packetsRead = bytesRead / MPEG2_TS_PACKET_SIZE;
        size_t bufferSize = packetsRead * MPEG2_TS_PACKET_SIZE;
        res = (*caller)->Enqueue(caller, NULL /*pBufferContext*/,
                                 pBufferData /*pData*/,
                                 bufferSize /*dataLength*/,
                                 NULL /*pMsg*/,
                                 0 /*msgLength*/);
        assert(XA_RESULT_SUCCESS == res);
    } else {
        // EOF or I/O error, signal EOS
        XAAndroidBufferItem msgEos[1];
        msgEos[0].itemKey = XA_ANDROID_ITEMKEY_EOS;
        msgEos[0].itemSize = 0;
        // EOS message has no parameters, so the total size of the message is the size of the key
        //   plus the size if itemSize, both XAuint32
        res = (*caller)->Enqueue(caller, (void *)&kEosBufferCntxt /*pBufferContext*/,
                                 NULL /*pData*/, 0 /*dataLength*/,
                                 msgEos /*pMsg*/,
                                 sizeof(XAuint32)*2 /*msgLength*/);
        assert(XA_RESULT_SUCCESS == res);
        reachedEof = true;
    }
    
    
exit:
    ok = pthread_mutex_unlock(&mutex);
    assert(0 == ok);
    return XA_RESULT_SUCCESS;
}

// callback invoked whenever there is new or changed stream information
void AndroidVideoDecoder::StreamChangeCallback(XAStreamInformationItf caller,
                                        XAuint32 eventId,
                                        XAuint32 streamIndex,
                                        void * pEventData,
                                        void * pContext )
{
    logInf("StreamChangeCallback called for stream %u", streamIndex);
    // pContext was specified as NULL at RegisterStreamChangeCallback and is unused here
    assert(NULL == pContext);
    switch (eventId) {
        case XA_STREAMCBEVENT_PROPERTYCHANGE: {
            /** From spec 1.0.1:
             "This event indicates that stream property change has occurred.
             The streamIndex parameter identifies the stream with the property change.
             The pEventData parameter for this event is not used and shall be ignored."
             */
            
            XAresult res;
            XAuint32 domain;
            res = (*caller)->QueryStreamType(caller, streamIndex, &domain);
            assert(XA_RESULT_SUCCESS == res);
            switch (domain) {
                case XA_DOMAINTYPE_VIDEO: {
                    XAVideoStreamInformation videoInfo;
                    res = (*caller)->QueryStreamInformation(caller, streamIndex, &videoInfo);
                    assert(XA_RESULT_SUCCESS == res);
                    logInf("Found video size %u x %u, codec ID=%u, frameRate=%u, bitRate=%u, duration=%u ms",
                           videoInfo.width, videoInfo.height, videoInfo.codecId, videoInfo.frameRate,
                           videoInfo.bitRate, videoInfo.duration);
                } break;
                default:
                    fprintf(stderr, "Unexpected domain %u\n", domain);
                    break;
            }
        } break;
        default:
            fprintf(stderr, "Unexpected stream event ID %u\n", eventId);
            break;
    }
}

// Enqueue the initial buffers, and optionally signal a discontinuity in the first buffer
bool AndroidVideoDecoder::enqueueInitialBuffers(bool discontinuity)
{
    
    /* Fill our cache.
     * We want to read whole packets (integral multiples of MPEG2_TS_PACKET_SIZE).
     * fread returns units of "elements" not bytes, so we ask for 1-byte elements
     * and then check that the number of elements is a multiple of the packet size.
     */
    size_t bytesRead;
    bytesRead = fread(dataCache, 1, BUFFER_SIZE * NB_BUFFERS, file);
    if (bytesRead <= 0) {
        // could be premature EOF or I/O error
        return false;
    }
    if ((bytesRead % MPEG2_TS_PACKET_SIZE) != 0) {
        logInf("Dropping last packet because it is not whole");
    }
    size_t packetsRead = bytesRead / MPEG2_TS_PACKET_SIZE;
    logInf("Initially queueing %u packets", packetsRead);
    
    /* Enqueue the content of our cache before starting to play,
     we don't want to starve the player */
    size_t i;
    for (i = 0; i < NB_BUFFERS && packetsRead > 0; i++) {
        // compute size of this buffer
        size_t packetsThisBuffer = packetsRead;
        if (packetsThisBuffer > PACKETS_PER_BUFFER) {
            packetsThisBuffer = PACKETS_PER_BUFFER;
        }
        size_t bufferSize = packetsThisBuffer * MPEG2_TS_PACKET_SIZE;
        XAresult res;
        if (discontinuity) {
            // signal discontinuity
            XAAndroidBufferItem items[1];
            items[0].itemKey = XA_ANDROID_ITEMKEY_DISCONTINUITY;
            items[0].itemSize = 0;
            // DISCONTINUITY message has no parameters,
            //   so the total size of the message is the size of the key
            //   plus the size if itemSize, both XAuint32
            res = (*playerBQItf)->Enqueue(playerBQItf, NULL /*pBufferContext*/,
                                          dataCache + i*BUFFER_SIZE, bufferSize, items /*pMsg*/,
                                          sizeof(XAuint32)*2 /*msgLength*/);
            discontinuity = false;
        } else {
            res = (*playerBQItf)->Enqueue(playerBQItf, NULL /*pBufferContext*/,
                                          dataCache + i*BUFFER_SIZE, bufferSize, NULL, 0);
        }
        assert(XA_RESULT_SUCCESS == res);
        packetsRead -= packetsThisBuffer;
    }
    
    return true;
}


#include <sys/stat.h>
#include <errno.h>

// create streaming media player
bool AndroidVideoDecoder::createStreamingMediaPlayer()
{
    XAresult res;
    // convert Java string to UTF-8
    std::string path = sourcePath;
    unsigned pos = path.find("/");
    path = path.substr(pos+1);
	std::string diskPath = "/sdcard/renderScenes/" + path;
	std::string assetPath = sourcePath;
    
    // open the file to play
    file = fopen(diskPath.c_str(), "rb");
	if(file == NULL){
		FileSystem::getInstance()->openFile(assetPath);
		//another approach, this works!
		FILE * pFile;
		pFile = fopen (diskPath.c_str(), "wb");
		if(pFile == 0){
			//mkdir fails, after looking the implementation we supose that is a problem of the sandbox and the duplication of kernel static variables
			//and the strategy of not release all the memory from ram to speed up
			logErr("you find a bug, have a nice crash :) (and stop fucking the sd!)");
		}
		fwrite (FileSystem::getInstance()->getFileData(assetPath) , sizeof(char), FileSystem::getInstance()->getFileSize(assetPath) - 1, pFile);
		fclose (pFile);
		FileSystem::getInstance()->destroyFileData(assetPath);
	}
    
    file = fopen(diskPath.c_str(), "rb");
    if (file == NULL) {
    	logErr("AndroidVideoDecoder::createStreamingMediaPlayer HARD FAIL: fail open! %s", diskPath.c_str());
    	return false;
    }
    logInf("file opened!");
    // configure data source
    XADataLocator_AndroidBufferQueue loc_abq = { XA_DATALOCATOR_ANDROIDBUFFERQUEUE, NB_BUFFERS };
    XADataFormat_MIME format_mime = {
        XA_DATAFORMAT_MIME, XA_ANDROID_MIME_MP2TS, XA_CONTAINERTYPE_MPEG_TS };
    XADataSource dataSrc = {&loc_abq, &format_mime};
    
    // configure audio sink
    XADataLocator_OutputMix loc_outmix = { XA_DATALOCATOR_OUTPUTMIX, outputMixObject };
    XADataSink audioSnk = { &loc_outmix, NULL };
    
    // configure image video sink
    XADataLocator_NativeDisplay loc_nd = {
        XA_DATALOCATOR_NATIVEDISPLAY,        // locatorType
        // the video sink must be an ANativeWindow created from a Surface or SurfaceTexture
        (void*)theNativeWindow,              // hWindow
        // must be NULL
        NULL                                 // hDisplay
    };
    XADataSink imageVideoSink = {&loc_nd, NULL};
    
    // declare interfaces to use
    XAboolean     required[NB_MAXAL_INTERFACES]
    = {XA_BOOLEAN_TRUE, XA_BOOLEAN_TRUE,           XA_BOOLEAN_TRUE};
    XAInterfaceID iidArray[NB_MAXAL_INTERFACES]
    = {XA_IID_PLAY,     XA_IID_ANDROIDBUFFERQUEUESOURCE,
        XA_IID_STREAMINFORMATION};
    
    // create media player
    res = (*engineEngine)->CreateMediaPlayer(engineEngine, &playerObj, &dataSrc,
                                             NULL, &audioSnk, &imageVideoSink, NULL, NULL,
                                             NB_MAXAL_INTERFACES /*XAuint32 numInterfaces*/,
                                             iidArray /*const XAInterfaceID *pInterfaceIds*/,
                                             required /*const XAboolean *pInterfaceRequired*/);
    assert(XA_RESULT_SUCCESS == res);
    
    // realize the player
    res = (*playerObj)->Realize(playerObj, XA_BOOLEAN_FALSE);
    assert(XA_RESULT_SUCCESS == res);
    // get the play interface
    res = (*playerObj)->GetInterface(playerObj, XA_IID_PLAY, &playerPlayItf);
    assert(XA_RESULT_SUCCESS == res);
    
    // get the stream information interface (for video size)
    res = (*playerObj)->GetInterface(playerObj, XA_IID_STREAMINFORMATION, &playerStreamInfoItf);
    assert(XA_RESULT_SUCCESS == res);
    
    // get the volume interface
    res = (*playerObj)->GetInterface(playerObj, XA_IID_VOLUME, &playerVolItf);
    assert(XA_RESULT_SUCCESS == res);
    
    // get the Android buffer queue interface
    res = (*playerObj)->GetInterface(playerObj, XA_IID_ANDROIDBUFFERQUEUESOURCE, &playerBQItf);
    assert(XA_RESULT_SUCCESS == res);
    
    // specify which events we want to be notified of
    res = (*playerBQItf)->SetCallbackEventsMask(playerBQItf, XA_ANDROIDBUFFERQUEUEEVENT_PROCESSED);
    assert(XA_RESULT_SUCCESS == res);
    
    // register the callback from which OpenMAX AL can retrieve the data to play
    res = (*playerBQItf)->RegisterCallback(playerBQItf, AndroidBufferQueueCallback, NULL);
    assert(XA_RESULT_SUCCESS == res);
    
    // we want to be notified of the video size once it's found, so we register a callback for that
    res = (*playerStreamInfoItf)->RegisterStreamChangeCallback(playerStreamInfoItf,
                                                               StreamChangeCallback, NULL);
    assert(XA_RESULT_SUCCESS == res);
    
    // enqueue the initial buffers
    if (!enqueueInitialBuffers(false)) {
        return false;
    }
    
    // prepare the player
    res = (*playerPlayItf)->SetPlayState(playerPlayItf, XA_PLAYSTATE_PAUSED);
    assert(XA_RESULT_SUCCESS == res);
    
    // set the volume
    res = (*playerVolItf)->SetVolumeLevel(playerVolItf, 0);
    assert(XA_RESULT_SUCCESS == res);
    
    // start the playback
    res = (*playerPlayItf)->SetPlayState(playerPlayItf, XA_PLAYSTATE_PLAYING);
    assert(XA_RESULT_SUCCESS == res);
    
    return true;
}

void AndroidVideoDecoder::setPlayingStreamingMediaPlayer(){
    XAresult res;
    // make sure the streaming media player was created
    if (NULL != playerPlayItf) {
        // set the player's state
        res = (*playerPlayItf)->SetPlayState(playerPlayItf, XA_PLAYSTATE_PLAYING);
        assert(XA_RESULT_SUCCESS == res);
    }
}

AndroidVideoDecoder::AndroidVideoDecoder(RectGUI* rect, std::string path){
    
	textureRect = NULL;
	textureID = (GLuint)NULL;
	originalWidth = 0.0;
	originalHeight = 0.0;
	originalTop = 0.0;
	originalLeft = 0.0;
	engineObject = NULL;
	engineEngine = NULL;
	outputMixObject = NULL;
	playerObj = NULL;
	playerPlayItf = NULL;
	playerBQItf = NULL;
	playerStreamInfoItf = NULL;
	playerVolItf = NULL;
	theNativeWindow = NULL;
	file = NULL;
	discontinuity = false;
	reachedEof = false;
    
	ended = false;
	sourcePath = path;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureID);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
	theNativeWindow = getANativeWindow(textureID);//texture->getTextureId());
    
	textureRect = rect;
	originalWidth = rect->getWidth();
	originalHeight = rect->getHeight();
	originalLeft = rect->getLeft();
	originalTop = rect->getTop();
    
	Shader* shader = new VideoPlaneShader();
	rect->setShader(shader);
	rect->setTexture(TextureManager::getInstance()->getTexture("blueSquare.png"));
	rect->setTexture(textureID);
    
    XAresult res;
    
    // create engine
    res = xaCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    assert(XA_RESULT_SUCCESS == res);
    
    // realize the engine
    res = (*engineObject)->Realize(engineObject, XA_BOOLEAN_FALSE);
    assert(XA_RESULT_SUCCESS == res);
    
    // get the engine interface, which is needed in order to create other objects
    res = (*engineObject)->GetInterface(engineObject, XA_IID_ENGINE, &engineEngine);
    assert(XA_RESULT_SUCCESS == res);
    
    // create output mix
    res = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, NULL, NULL);
    assert(XA_RESULT_SUCCESS == res);
    
    // realize the output mix
    res = (*outputMixObject)->Realize(outputMixObject, XA_BOOLEAN_FALSE);
    assert(XA_RESULT_SUCCESS == res);
    
    createStreamingMediaPlayer();
    setPlayingStreamingMediaPlayer();
    logInf("setted all stuff from openmax");
	this->stop();  // We force the head to stay at the start of the video (pause will not suffice)
    
	textureRect->setTexture(textureID);
    
	instanceDecoder = this;
    
	res = (*playerPlayItf)->RegisterCallback(playerPlayItf, endCallbackDecoder, NULL);
	assert(XA_RESULT_SUCCESS == res);
    
	res = (*playerPlayItf)->SetCallbackEventsMask(playerPlayItf, XA_PLAYEVENT_HEADATEND);
	assert(XA_RESULT_SUCCESS == res);
}


AndroidVideoDecoder::~AndroidVideoDecoder(){
	glDeleteTextures(1, &textureID);
    
	// OpenMAX AL destruction
	(*playerObj)->Destroy(playerObj);
	(*outputMixObject)->Destroy(outputMixObject);
	(*engineObject)->Destroy(engineObject);
    
	closeVideo();
    instanceVideo = NULL;
}


void AndroidVideoDecoder::pause(){
	XAresult res = (*playerPlayItf)->SetPlayState(playerPlayItf, XA_PLAYSTATE_PAUSED);
    assert(XA_RESULT_SUCCESS == res);
}

void AndroidVideoDecoder::play(){
	this->stop();
    
	XAresult res = (*playerPlayItf)->SetPlayState(playerPlayItf, XA_PLAYSTATE_PLAYING);
    assert(XA_RESULT_SUCCESS == res);
}

void AndroidVideoDecoder::setMute(bool enable){
	if(enable){
		XAresult res = (*playerVolItf)->SetMute(playerVolItf, XA_BOOLEAN_TRUE);
	}else{
		XAresult res = (*playerVolItf)->SetMute(playerVolItf, XA_BOOLEAN_FALSE);
	}
    assert(XA_RESULT_SUCCESS == res);
}

bool AndroidVideoDecoder::getMute(){
	XAboolean mute;
	XAresult res = (*playerVolItf)->GetMute(playerVolItf, &mute);
	assert(XA_RESULT_SUCCESS == res);
	return (mute == XA_BOOLEAN_TRUE);
}

void AndroidVideoDecoder::stop(){
	XAresult res = (*playerPlayItf)->SetPlayState(playerPlayItf, XA_PLAYSTATE_STOPPED);
    assert(XA_RESULT_SUCCESS == res);
}

bool AndroidVideoDecoder::isStopped(){
	XAuint32 state;
	XAresult res = (*playerPlayItf)->GetPlayState(playerPlayItf, &state);
	assert(XA_RESULT_SUCCESS == res);
	return (state == XA_PLAYSTATE_STOPPED);
}

bool AndroidVideoDecoder::isPaused(){
	XAuint32 state;
	XAresult res = (*playerPlayItf)->GetPlayState(playerPlayItf, &state);
	assert(XA_RESULT_SUCCESS == res);
	return (state == XA_PLAYSTATE_PAUSED);
}

bool AndroidVideoDecoder::isPlaying(){
	XAuint32 state;
	XAresult res = (*playerPlayItf)->GetPlayState(playerPlayItf, &state);
	assert(XA_RESULT_SUCCESS == res);
	return (state == XA_PLAYSTATE_PLAYING);
}

void AndroidVideoDecoder::updateTexture(){
	updateTextureVideo();
}



void AndroidVideoDecoder::setSource(std::string fileName){}
void AndroidVideoDecoder::releaseVideo(){}

void AndroidVideoDecoder::setSplash(std::string texturePath){}
void AndroidVideoDecoder::setSplash(){}


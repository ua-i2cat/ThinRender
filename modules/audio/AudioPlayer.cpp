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
 *  Author:         Ignacio Contreras Pinilla <ignacio.contreras@i2cat.net>
 */

#include "AudioPlayer.h"
#include "../../globalData/GlobalData.h"
#include <android/asset_manager.h>
#include <android_native_app_glue.h>

int AudioPlayer::playerCount = 0;
SLObjectItf AudioPlayer::engineObject = NULL;
SLEngineItf AudioPlayer::engineEngine = NULL;
SLObjectItf AudioPlayer::outputMixObject = NULL;

AudioPlayer::AudioPlayer(std::string filePath) {
    SLObjectItf fdPlayerObject = NULL;
    SLPlayItf fdPlayerPlay = NULL;
    SLSeekItf fdPlayerSeek = NULL;
    SLMuteSoloItf fdPlayerMuteSolo = NULL;
    SLVolumeItf fdPlayerVolume = NULL;

    if (engineObject == NULL) {
        createEngine();
    }

    long start;
    long length;
    int fileDescriptor = FileSystem::getInstance()->getFileDescriptor(filePath, &start, &length);
    createAssetAudioPlayer(fileDescriptor, start, length);
    playerCount++;
}

AudioPlayer::~AudioPlayer() {
    logInf("AudioPlayer::~AudioPlayer: stopping");
    stop();
    logInf("AudioPlayer::~AudioPlayer: destroying the player");
    destroyPlayer();
    playerCount--;
    if (playerCount == 0) {
        logInf("AudioPlayer::~AudioPlayer: destroying the engine");
        destroyEngine();
    }
}

bool AudioPlayer::play() {
    setPlayingAssetAudioPlayer(true);
}

bool AudioPlayer::pause() {
    setPlayingAssetAudioPlayer(false);

}

bool AudioPlayer::stop() {
    setPlayingAssetAudioPlayer(false);
}


// Private methods

bool AudioPlayer::createEngine()
{
    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);

    // create output mix
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
}

// create asset audio player
bool AudioPlayer::createAssetAudioPlayer(int fileDescriptor, long start, long length)
{
    SLresult result;

    // configure audio source
    SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD, fileDescriptor, start, length};
    SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
    SLDataSource audioSrc = {&loc_fd, &format_mime};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // create audio player
    const SLInterfaceID ids[3] = {SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &fdPlayerObject, &audioSrc, &audioSnk,
            3, ids, req);
    assert(SL_RESULT_SUCCESS == result);

    // realize the player
    result = (*fdPlayerObject)->Realize(fdPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);

    // get the play interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_PLAY, &fdPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);

    // get the seek interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_SEEK, &fdPlayerSeek);
    assert(SL_RESULT_SUCCESS == result);

    // get the mute/solo interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_MUTESOLO, &fdPlayerMuteSolo);
    assert(SL_RESULT_SUCCESS == result);

    // get the volume interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_VOLUME, &fdPlayerVolume);
    assert(SL_RESULT_SUCCESS == result);

    // enable whole file looping
    result = (*fdPlayerSeek)->SetLoop(fdPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
    assert(SL_RESULT_SUCCESS == result);

    return true;
}

// set the playing state for the asset audio player
bool AudioPlayer::setPlayingAssetAudioPlayer(bool isPlaying)
{
    SLresult result;
    result = (*fdPlayerPlay)->SetPlayState(fdPlayerPlay, isPlaying ?
            SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED);
    return (result == SL_RESULT_SUCCESS);
}

void AudioPlayer::destroyEngine()
{
	if(outputMixObject != NULL){
		logInf("AudioPlayer::destroyEngine: destroying the outputMixObject");
		(*outputMixObject)->Destroy(outputMixObject);
		outputMixObject = NULL;
	}

	if(engineObject != NULL){
		logInf("AudioPlayer::destroyEngine: destroying the engineObject");
		(*engineObject)->Destroy(engineObject);
		engineObject = NULL;
		engineEngine = NULL;
	}
}

void AudioPlayer::destroyPlayer()
{
	if(fdPlayerObject != NULL){
		(*fdPlayerObject)->Destroy(fdPlayerObject);
		fdPlayerObject = NULL;
		fdPlayerPlay = NULL;
		fdPlayerSeek = NULL;
		fdPlayerMuteSolo = NULL;
		fdPlayerVolume = NULL;
	}
}

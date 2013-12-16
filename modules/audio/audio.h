/*
 * Copyright (C) 2010 The Android Open Source Project
 * Copyright (C) 2013 i2CAT Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */

#include <assert.h>
#include <android/log.h>

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>

// engine interfaces
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;

// output mix interfaces
static SLObjectItf outputMixObject = NULL;
static SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;

// buffer queue player interfaces
static SLObjectItf bqPlayerObject = NULL;
static SLPlayItf bqPlayerPlay;
static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
static SLEffectSendItf bqPlayerEffectSend;
static SLMuteSoloItf bqPlayerMuteSolo;
static SLVolumeItf bqPlayerVolume;

// aux effect on the output mix, used by the buffer queue player
static const SLEnvironmentalReverbSettings reverbSettings =
    SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

// file descriptor player interfaces
static SLObjectItf fdPlayerObject = NULL;
static SLPlayItf fdPlayerPlay;
static SLSeekItf fdPlayerSeek;
static SLMuteSoloItf fdPlayerMuteSolo;
static SLVolumeItf fdPlayerVolume;

// synthesized sawtooth clip
#define SAWTOOTH_FRAMES 8000
static short sawtoothBuffer[SAWTOOTH_FRAMES];

// 5 seconds of recorded audio at 16 kHz mono, 16-bit signed little endian
#define RECORDER_FRAMES (16000 * 5)
static short recorderBuffer[RECORDER_FRAMES];
static unsigned recorderSize = 0;
static SLmilliHertz recorderSR;

// pointer and size of the next player buffer to enqueue, and number of remaining buffers
static short *nextBuffer;
static unsigned nextSize;
static int nextCount;


// synthesize a mono sawtooth wave and place it into a buffer (called automatically on load)
__attribute__((constructor)) static void onDlOpen(void)
{
    unsigned i;
    for (i = 0; i < SAWTOOTH_FRAMES; ++i) {
        sawtoothBuffer[i] = 32768 - ((i % 100) * 660);
    }
}

// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    assert(bq == bqPlayerBufferQueue);
    assert(NULL == context);
    // for streaming playback, replace this test by logic to find and fill the next buffer
    if (--nextCount > 0 && NULL != nextBuffer && 0 != nextSize) {
        SLresult result;
        // enqueue another buffer
        result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, nextBuffer, nextSize);
        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
        // which for this code example would indicate a programming error
        assert(SL_RESULT_SUCCESS == result);
        (void)result;
    }
}

// create the engine and output mix objects
void audioCreateEngine()
{
    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
            &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void)result;
    }
    // ignore unsuccessful result codes for environmental reverb, as it is optional for this example

}

// create buffer queue audio player
void audioCreateBufferQueueAudioPlayer()
{
    SLresult result;

    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_8,
        SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
        SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // create audio player
    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/ SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,
            3, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
            &bqPlayerBufferQueue);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the effect send interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
            &bqPlayerEffectSend);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the volume interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
}

// expose the mute/solo APIs to Java for one of the 3 players
static SLMuteSoloItf getMuteSolo()
{
    if (fdPlayerMuteSolo != NULL)
        return fdPlayerMuteSolo;
    else
        return bqPlayerMuteSolo;
}

// expose the volume APIs to Java for one of the 3 players
static SLVolumeItf getVolume()
{
    if (fdPlayerVolume != NULL)
        return fdPlayerVolume;
    else
        return bqPlayerVolume;
}

// enable reverb on the buffer queue player
bool audioEnableReverb(bool enabled)
{
    SLresult result;

    // we might not have been able to add environmental reverb to the output mix
    if (NULL == outputMixEnvironmentalReverb) {
        return false;
    }

    result = (*bqPlayerEffectSend)->EnableEffectSend(bqPlayerEffectSend,
            outputMixEnvironmentalReverb, (SLboolean) enabled, (SLmillibel) 0);
    // and even if environmental reverb was present, it might no longer be available
    if (SL_RESULT_SUCCESS != result) {
        return false;
    }

    return true;
}

// create asset audio player
bool audioCreateAssetAudioPlayer(AAssetManager* assetManager, const char* filename)
{
    SLresult result;

    // TODO: use render fileSystem
    AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_UNKNOWN);

    // the asset might not be found
    if (NULL == asset) {
        return false;
    }

    // open asset as file descriptor
    off_t start, length;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    assert(0 <= fd);
    AAsset_close(asset);

    // configure audio source
    SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};
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
    (void)result;

    // realize the player
    result = (*fdPlayerObject)->Realize(fdPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the play interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_PLAY, &fdPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the seek interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_SEEK, &fdPlayerSeek);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the mute/solo interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_MUTESOLO, &fdPlayerMuteSolo);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the volume interface
    result = (*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_VOLUME, &fdPlayerVolume);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // enable whole file looping
    result = (*fdPlayerSeek)->SetLoop(fdPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    return true;
}

// set the playing state for the asset audio player
void audioSetPlayingAssetAudioPlayer(bool isPlaying)
{
    SLresult result;

    // make sure the asset audio player was created
    if (NULL != fdPlayerPlay) {

        // set the player's state
        result = (*fdPlayerPlay)->SetPlayState(fdPlayerPlay, isPlaying ?
            SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;
    }
}

// shut down the native audio system
void audioShutdown()
{

    __android_log_write(ANDROID_LOG_INFO, "audio-api", "Destroying bqPlayer");
    // destroy buffer queue audio player object, and invalidate all associated interfaces
    if (bqPlayerObject != NULL) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerObject = NULL;
        bqPlayerPlay = NULL;
        bqPlayerBufferQueue = NULL;
        bqPlayerEffectSend = NULL;
        bqPlayerMuteSolo = NULL;
        bqPlayerVolume = NULL;
    }

    __android_log_write(ANDROID_LOG_INFO, "audio-api", "Destroying fdPlayerObject");
    // destroy file descriptor audio player object, and invalidate all associated interfaces
    if (fdPlayerObject != NULL) {
        (*fdPlayerObject)->Destroy(fdPlayerObject);
        fdPlayerObject = NULL;
        fdPlayerPlay = NULL;
        fdPlayerSeek = NULL;
        fdPlayerMuteSolo = NULL;
        fdPlayerVolume = NULL;
    }

    __android_log_write(ANDROID_LOG_INFO, "audio-api", "Destroying outputMixObject");
    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    __android_log_write(ANDROID_LOG_INFO, "audio-api", "Destroying engineObject");
    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }
    __android_log_write(ANDROID_LOG_INFO, "audio-api", "Destroying engineObject done!");
}

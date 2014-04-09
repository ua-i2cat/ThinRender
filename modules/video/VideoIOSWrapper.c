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

#include "VideoIOSWrapper.h"

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#include <pthread.h>


#include <AudioToolbox/AudioToolbox.h>

pthread_mutex_t mutex;

AudioQueueRef _playQueue = NULL;

AVAssetReader* reader = NULL;
CMTime previousFrameTime;
CFAbsoluteTime previousActualFrameTime;

CVOpenGLESTextureRef luminanceTextureRef = NULL;
CVOpenGLESTextureRef chrominanceTextureRef = NULL;

CVOpenGLESTextureCacheRef videoTextureCache;

CVImageBufferRef movieFrame;

AVAssetReaderOutput *readerAudioTrackOutput = nil;
int maxBuffers = 3;
AudioQueueBufferRef audioQueueBuffer[3];
int audioBuffer = 0;
AudioQueueTimelineRef timeLine;

bool loop = true;

bool initReproduction = false;
bool pauseReproduction = false;
bool endReproduction = false;
CMSampleBufferRef lastSampleBufferRef = NULL;
    void createAssetReader(AVURLAsset * inputAsset){
        NSError *error = nil;
        reader = [AVAssetReader assetReaderWithAsset:inputAsset error:&error];
        
        NSDictionary *outputSettings = @{(id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_420YpCbCr8BiPlanarFullRange)};
        // Maybe set alwaysCopiesSampleData to NO on iOS 5.0 for faster video decoding
        AVAssetReaderTrackOutput *readerVideoTrackOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:[[inputAsset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0] outputSettings:outputSettings];
        readerVideoTrackOutput.alwaysCopiesSampleData = NO;
        [reader addOutput:readerVideoTrackOutput];
        
        NSArray *audioTracks = [inputAsset tracksWithMediaType:AVMediaTypeAudio];
        AVAssetReaderTrackOutput *readerAudioTrackOutput = nil;
        AVAssetTrack* audioTrack = [audioTracks objectAtIndex:0];
        
        readerAudioTrackOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:audioTrack outputSettings:nil];
        readerAudioTrackOutput.alwaysCopiesSampleData = NO;
        [reader addOutput:readerAudioTrackOutput];
    }
    
    void getVideoTextures(GLuint &luminance, GLuint &chrominance){
        pthread_mutex_lock(&mutex);
        int bufferHeight = (int) CVPixelBufferGetHeight(movieFrame);
        int bufferWidth = (int) CVPixelBufferGetWidth(movieFrame);
        if (CVPixelBufferGetPlaneCount(movieFrame) == 0){ // Check for YUV planar inputs to do RGB conversion
            logErr("getVideoTextures error, pixel format different to planar YUV");
        }
        
        CVReturn err;
            
        err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, videoTextureCache, movieFrame, NULL, GL_TEXTURE_2D, GL_LUMINANCE, bufferWidth, bufferHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0, &luminanceTextureRef);
        if (err){
            logErr("Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", err);
        }
        luminance = CVOpenGLESTextureGetName(luminanceTextureRef);
        
        err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, videoTextureCache, movieFrame, NULL, GL_TEXTURE_2D, GL_LUMINANCE_ALPHA, bufferWidth/2, bufferHeight/2, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 1, &chrominanceTextureRef);
        if (err){
            logErr("Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", err);
        }
        chrominance = CVOpenGLESTextureGetName(chrominanceTextureRef);
        
    }
    
    
    void releaseVideoCache(){
        CVPixelBufferUnlockBaseAddress(movieFrame, 0);
        CVOpenGLESTextureCacheFlush(videoTextureCache, 0);
        if(luminanceTextureRef != NULL){
            CFRelease(luminanceTextureRef);
        }
        if(chrominanceTextureRef != NULL){
            CFRelease(chrominanceTextureRef);
        }
        pthread_mutex_unlock(&mutex);
    }
void audioCallback(void *                  inUserData,
                   AudioQueueRef           inAQ,
                   AudioQueueBufferRef     inCompleteAQBuffer) {
    CMSampleBufferRef audioSampleBufferRef = [readerAudioTrackOutput copyNextSampleBuffer];
    if (!audioSampleBufferRef){
        logErr("read next audio frame output fail");
        return;
    }
    
    CMBlockBufferRef blockBufferRef;// = CMSampleBufferGetDataBuffer(audioSampleBufferRef);
    AudioBufferList audioBufferList;
    CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(audioSampleBufferRef, NULL, &audioBufferList, sizeof(audioBufferList), NULL, NULL, 0, &blockBufferRef);
    if(audioBufferList.mNumberBuffers == 0) logErr("====================== mNumberBuffers = 0");
    for( int i=0; i< audioBufferList.mNumberBuffers; i++ ){
        if (audioBufferList.mBuffers[i].mData && inCompleteAQBuffer->mAudioData) {
            memcpy(inCompleteAQBuffer->mAudioData, audioBufferList.mBuffers[i].mData, audioBufferList.mBuffers[i].mDataByteSize);
            inCompleteAQBuffer->mAudioDataByteSize = audioBufferList.mBuffers[i].mDataByteSize;
            AudioQueueEnqueueBuffer(_playQueue, inCompleteAQBuffer, 0, NULL);
        }else{
            CMSampleBufferInvalidate(audioSampleBufferRef);
            CFRelease(audioSampleBufferRef);
            return;
        }
    }
    CFRelease(blockBufferRef);
    CMSampleBufferInvalidate(audioSampleBufferRef);
    CFRelease(audioSampleBufferRef);
    return;
}

int counterAudioSamples = 0;
    void readNextAudioSampleFromOutput(AVAssetReaderOutput * readerAudioTrackOutput){
        if(counterAudioSamples >= maxBuffers) return;
        
        CMSampleBufferRef audioSampleBufferRef = [readerAudioTrackOutput copyNextSampleBuffer];
        if (!audioSampleBufferRef){
            logErr("read next audio frame output fail");
            return;
        }
        
        
        AudioStreamBasicDescription inAudioStreamBasicDescription = *CMAudioFormatDescriptionGetStreamBasicDescription((CMAudioFormatDescriptionRef)CMSampleBufferGetFormatDescription(audioSampleBufferRef));
        
        OSStatus status;
        if(_playQueue == NULL){
            status = AudioQueueNewOutput(&inAudioStreamBasicDescription, audioCallback, NULL, NULL, NULL, 0, &_playQueue);
            if(status != 0) logInf("--------------------AudioQueueNewOutput OSStatus error %i", (int) status);
            
            status = AudioQueuePrime(_playQueue, 0, NULL);
            if(status != 0) logInf("--------------------AudioQueuePrime OSStatus error %i", (int) status);
            
            status = AudioQueueCreateTimeline(_playQueue, &timeLine);
            if(status != 0) logInf("--------------------AudioQueuePrime OSStatus error %i", (int) status);
        }
        CMBlockBufferRef blockBufferRef;// = CMSampleBufferGetDataBuffer(audioSampleBufferRef);
        AudioBufferList audioBufferList;
        CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(audioSampleBufferRef, NULL, &audioBufferList, sizeof(audioBufferList), NULL, NULL, 0, &blockBufferRef);
        
        for( int i=0; i< audioBufferList.mNumberBuffers; i++ ){
            
            AudioBuffer audioBuffer = audioBufferList.mBuffers[i];
            //AudioQueueBufferRef audioQueueBuffer;
            status = AudioQueueAllocateBuffer(_playQueue, audioBuffer.mDataByteSize, &audioQueueBuffer[counterAudioSamples]);
            
            if (status == 0 && audioQueueBuffer[counterAudioSamples]->mAudioData) {
                memcpy(audioQueueBuffer[counterAudioSamples]->mAudioData, audioBuffer.mData, audioBuffer.mDataByteSize);
                audioQueueBuffer[counterAudioSamples]->mAudioDataByteSize = audioBuffer.mDataByteSize;
            }else{
                logInf("--------------------OSStatus error %i", (int) status);
            }
        }
        counterAudioSamples++;
        CFRelease(blockBufferRef);
        CMSampleBufferInvalidate(audioSampleBufferRef);
        CFRelease(audioSampleBufferRef);
        if(counterAudioSamples == maxBuffers){
            status = AudioQueueStart(_playQueue, NULL);
            if(status != 0) logInf("--------------------AudioQueueStart OSStatus error %i", (int) status);
            for(int n = 0; n < maxBuffers; n++){
                AudioTimeStamp bufferStartTime;
                AudioQueueGetCurrentTime(_playQueue, NULL, &bufferStartTime, NULL);
            
                status = AudioQueueEnqueueBuffer(_playQueue, audioQueueBuffer[n], 0, NULL);
                if(status != 0) logInf("--------------------AudioQueueEnqueueBuffer OSStatus error %i", (int) status);
            }

        }
        return;
            
    }
    void readNextVideoFrameFromOutput(AVAssetReaderOutput * readerVideoTrackOutput){
        CMSampleBufferRef sampleBufferRef = [readerVideoTrackOutput copyNextSampleBuffer];
        if (!sampleBufferRef){
            logErr("read next video frame output fail");
            [reader cancelReading];
            return;
        }
        
        // Do this outside of the video processing queue to not slow that down while waiting
        CMTime currentSampleTime = CMSampleBufferGetOutputPresentationTimeStamp(sampleBufferRef);
        CMTime differenceFromLastFrame = CMTimeSubtract(currentSampleTime, previousFrameTime);
        CFAbsoluteTime currentActualTime = CFAbsoluteTimeGetCurrent();
                
        CGFloat frameTimeDifference = CMTimeGetSeconds(differenceFromLastFrame);
        CGFloat actualTimeDifference = currentActualTime - previousActualFrameTime;
        
        if (frameTimeDifference > actualTimeDifference){
            usleep(1000000.0 * (frameTimeDifference - actualTimeDifference));
        }
        while(pauseReproduction){
            usleep(100000.0);
        }
        previousFrameTime = currentSampleTime;
        previousActualFrameTime = CFAbsoluteTimeGetCurrent();
        pthread_mutex_lock(&mutex);
        if(lastSampleBufferRef != NULL){
            CMSampleBufferInvalidate(lastSampleBufferRef);
            CFRelease(lastSampleBufferRef);
        }
        movieFrame = CMSampleBufferGetImageBuffer(sampleBufferRef);
        lastSampleBufferRef = sampleBufferRef;
        pthread_mutex_unlock(&mutex);
       
    }
    void processAsset(AVURLAsset * inputAsset){
        createAssetReader(inputAsset);
        
        AVAssetReaderOutput *readerVideoTrackOutput = nil;
        
        for( AVAssetReaderOutput *output in reader.outputs){
            if( [output.mediaType isEqualToString:AVMediaTypeAudio] ) {
                readerAudioTrackOutput = output;
            }
            if( [output.mediaType isEqualToString:AVMediaTypeVideo]){
                readerVideoTrackOutput = output;
            }
        }
        
        if ([reader startReading] == NO){
            logErr("VideoIOSWrapper process asset Error reading from file");
            return;
        }
        
        while (reader.status == AVAssetReaderStatusReading){
            readNextAudioSampleFromOutput(readerAudioTrackOutput);
            readNextVideoFrameFromOutput(readerVideoTrackOutput);
        }
        
        if (reader.status == AVAssetWriterStatusCompleted){
            [reader cancelReading];
        }
    }
    
    void startVideo(){
        
        initReproduction = true;
        pauseReproduction = false;
        endReproduction = false;
        pthread_mutex_init(&mutex, NULL);
        //pthread_mutex_destroy(&mutex);
        
        CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, ContextControllerEAGL::getInstance()->context, NULL, &videoTextureCache);
        if(err){
            logErr("ERROR CREATING CVOpenGLESTextureCache!!! %d", err);
        }
        previousFrameTime = kCMTimeZero;
        previousActualFrameTime = CFAbsoluteTimeGetCurrent();
        NSURL *url = [[NSBundle mainBundle] URLForResource:@"sintel" withExtension:@"mov"];
        NSDictionary *inputOptions = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:AVURLAssetPreferPreciseDurationAndTimingKey];
        AVURLAsset *inputAsset = [[AVURLAsset alloc] initWithURL:url options:inputOptions];
        
        [inputAsset loadValuesAsynchronouslyForKeys:[NSArray arrayWithObject:@"tracks"] completionHandler: ^{
                NSError *error = nil;
                AVKeyValueStatus tracksStatus = [inputAsset statusOfValueForKey:@"tracks" error:&error];
                if (!(tracksStatus == AVKeyValueStatusLoaded)){
                    logErr("status start video not loaded!!! ");
                    return;
                }
                bool loopCondition = true;
                while(loopCondition){
                    processAsset(inputAsset);
                    loopCondition = loop;
                    AudioQueueReset (_playQueue);
                    AudioQueueStop (_playQueue, YES);
                    AudioQueueDispose (_playQueue, YES);
                    _playQueue = NULL;
                    counterAudioSamples = 0;
                }
                endReproduction = true;
                initReproduction = false;
        }];
        
    }

AudioTimeStamp* audioTimeStamp;
bool playAction(){
    logInf("playAction");
    pauseReproduction = false;
    if(_playQueue != NULL){
        AudioQueueStart(_playQueue, audioTimeStamp);
    }
    if(!initReproduction){
        startVideo();
    }
    return true;
}
bool pauseAction(){
    logInf("pause action %i", endReproduction);
    pauseReproduction = true;
    AudioQueueGetCurrentTime(_playQueue, timeLine, audioTimeStamp, NULL);
    AudioQueuePause(_playQueue);
    return !endReproduction;
}
void stopAction(){
    logInf("stopAction");
    if (reader){
        [reader cancelReading];
    }
    //this will end the reproduction!
}
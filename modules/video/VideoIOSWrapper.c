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
pthread_mutex_t mutex;


AVAssetReader* reader = NULL;
CMTime previousFrameTime;
CFAbsoluteTime previousActualFrameTime;

CVOpenGLESTextureRef luminanceTextureRef = NULL;
CVOpenGLESTextureRef chrominanceTextureRef = NULL;

CVOpenGLESTextureCacheRef videoTextureCache;

CVImageBufferRef movieFrame;

bool loop = false;

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
    void readNextVideoFrameFromOutput(AVAssetReaderOutput * readerVideoTrackOutput){
        CMSampleBufferRef sampleBufferRef = [readerVideoTrackOutput copyNextSampleBuffer];
        if (!sampleBufferRef){
            logErr("read next video frame output fail");
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
            if( [output.mediaType isEqualToString:AVMediaTypeVideo]){
                readerVideoTrackOutput = output;
            }
        }
        
        if ([reader startReading] == NO)
        {
            logErr("VideoIOSWrapper process asset Error reading from file");
            return;
        }
        
        while (reader.status == AVAssetReaderStatusReading){
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
        NSURL *url = [[NSBundle mainBundle] URLForResource:@"sintel-1080-baseline" withExtension:@"mp4"];
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
                }
                endReproduction = true;
                initReproduction = false;
        }];
        
    }

bool playAction(){
    logInf("playAction");
    pauseReproduction = false;
    
    if(!initReproduction){
        startVideo();
    }
    return true;
}
bool pauseAction(){
    logInf("pause action %i", endReproduction);
    pauseReproduction = true;
    return !endReproduction;
}
void stopAction(){
    logInf("stopAction");
    if (reader){
        [reader cancelReading];
    }
    //this will end the reproduction!
}
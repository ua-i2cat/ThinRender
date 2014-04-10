//
//  IOSVideoDecoder.cpp
//  ModernismeMNACTEC
//
//  Created by Daniel Cabrera on 09/04/14.
//  Copyright (c) 2014 i2CAT. All rights reserved.
//

#include "IOSVideoDecoder.h"
#include "../../shader/gui/VideoPlaneShader.h"
#import <AudioToolbox/AudioToolbox.h>

enum{PAUSED=0, STOPPED, PLAYING};

int videoState;

IOSVideoDecoder::IOSVideoDecoder(RectGUI* rect, std::string path){
    NSLog(@"IOSAudioPlayer constructor");
    ended = false;
	sourcePath = path;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    
	textureRect = rect;
	originalWidth = rect->getWidth();
	originalHeight = rect->getHeight();
	originalLeft = rect->getLeft();
	originalTop = rect->getTop();
    
	Shader* shader = new VideoPlaneShader();
	rect->setShader(shader);
	rect->setTexture(TextureManager::getInstance()->getTexture("blueSquare.png"));
	rect->setTexture(textureID);
    
    path = "multimedia/9.mp4";
    
    string filename = GlobalData::getInstance()->iOSPath+"/assets/"+path;
    NSString *videopath = [NSString stringWithCString:filename.c_str()
                                        encoding:[NSString defaultCStringEncoding]];
    NSURL *url = [NSURL fileURLWithPath:videopath];
    
    
   // BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:videopath];
    
    videoDecoderObject = [[IOSVideoDecoderDelegate alloc] init:this withURL:url withTextureId: textureID];
}

void IOSVideoDecoder::updateTexture(){
    if (isPlaying())
    {
        [videoDecoderObject readNextMovieFrame];
    }
}

IOSVideoDecoder::~IOSVideoDecoder()
{
    glDeleteTextures(1, &textureID);
    instanceVideo = NULL;
    videoDecoderObject = NULL;
}

void IOSVideoDecoder::setSource(std::string fileName){}
void IOSVideoDecoder::releaseVideo(){}
void IOSVideoDecoder::play(){
    videoState = PLAYING;
}
void IOSVideoDecoder::pause()
{
   videoState = PAUSED;
}

void IOSVideoDecoder::stop()
{
    [videoDecoderObject stopVideo];
    videoState = STOPPED;

}
void IOSVideoDecoder::setMute(bool enable){}
bool IOSVideoDecoder::getMute(){return false;}

bool IOSVideoDecoder::isStopped()
{
    return videoState == STOPPED;
}

bool IOSVideoDecoder::isPaused()
{
    return videoState == PAUSED;
}

bool IOSVideoDecoder::isPlaying()
{
    return videoState == PLAYING;
}

void IOSVideoDecoder::setSplash(std::string texturePath){}
void IOSVideoDecoder::setSplash(){}
void IOSVideoDecoder::setEnded(bool end){
    ended = end;
}


@implementation IOSVideoDecoderDelegate

AVAssetReader * _movieReader;
AVAssetReader * _audioReader;

GLuint textureID;
IOSVideoDecoder *videoManager;
int maxBuffers = 3;
AudioQueueBufferRef audioQueueBuffer[3];
int audioBuffer = 0;
AudioQueueTimelineRef timeLine;
AudioQueueRef _playQueue = NULL;


- (id) init:(IOSVideoDecoder *) vm withURL: (NSURL *) url withTextureId:(GLuint)texture{
    if ((self = [super init])) {
        textureID = texture;
        AVURLAsset * asset = [AVURLAsset URLAssetWithURL:url options:nil];
        
        [asset loadValuesAsynchronouslyForKeys:[NSArray arrayWithObject:@"tracks"] completionHandler:
         ^{
         dispatch_async(dispatch_get_main_queue(),
                        ^{
                            AVAssetTrack * videoTrack = nil;
                            BOOL p = [asset isPlayable];
                            NSError *error;
                            [asset statusOfValueForKey:@"tracks" error:&error];
                            
                            //Check for video tracks
                            NSArray * tracks = [asset tracksWithMediaType:AVMediaTypeVideo];
                            if ([tracks count] == 1)
                            {
                                videoTrack = [tracks objectAtIndex:0];
                                
                                NSError * error = nil;
                                
                                _movieReader = [[AVAssetReader alloc] initWithAsset:asset error:&error];
                                
                                NSString* key = (NSString*)kCVPixelBufferPixelFormatTypeKey;
                                NSNumber* value = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA];
                                NSDictionary* videoSettings = 
                                [NSDictionary dictionaryWithObject:value forKey:key]; 
                                
                                [_movieReader addOutput:[AVAssetReaderTrackOutput 
                                                         assetReaderTrackOutputWithTrack:videoTrack 
                                                         outputSettings:videoSettings]];
                                [_movieReader startReading];

                            }
                            // Check for audiotracks
                            NSArray *audioTracks =[asset tracksWithMediaType:AVMediaTypeAudio];
                            if ([audioTracks count] == 1){
                                
                            }
                            vm->play();
                        });
         }];
        videoManager = vm;
    }
    
    return self;
}

- (void) startVideo{
    [_movieReader startReading];
}

- (void) stopVideo{
    [_movieReader cancelReading];
    videoManager->setEnded(true);
}

- (void) readNextMovieFrame
{
   if (_movieReader.status == AVAssetReaderStatusReading)
    {
        AVAssetReaderTrackOutput * output = [_movieReader.outputs objectAtIndex:0];
        CMSampleBufferRef sampleBuffer = [output copyNextSampleBuffer];
        if (sampleBuffer)
        {
            CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
            
            // Lock the image buffer
            CVPixelBufferLockBaseAddress(imageBuffer,0);
            
            // Get information of the image
            size_t width = CVPixelBufferGetWidth(imageBuffer);
            size_t height = CVPixelBufferGetHeight(imageBuffer);
            
            
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // This is necessary for non-power-of-two textures
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            // Using BGRA extension to pull in video frame data directly
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, CVPixelBufferGetBaseAddress(imageBuffer));
            
            CMSampleBufferInvalidate(sampleBuffer);

            sampleBuffer = nil;
            // Unlock the image buffer
            CVPixelBufferUnlockBaseAddress(imageBuffer,0);
            CVPixelBufferRelease(imageBuffer);
        }
    }
    
    else if (_movieReader.status == AVAssetReaderStatusCompleted) {
        [_movieReader cancelReading];
        videoManager->setEnded(true);
    }
}
@end
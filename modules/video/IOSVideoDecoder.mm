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
#include <pthread.h>

enum{PAUSED=0, STOPPED, PLAYING};

int videoState;
bool muted;

IOSVideoDecoder::IOSVideoDecoder(RectGUI* rect, std::string path){
    NSLog(@"IOSAudioPlayer constructor");
    ended = false;
    muted = false;
    
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
        
        [videoDecoderObject readAudio];
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
    [videoDecoderObject startPlaying];
    videoState = PLAYING;
}
void IOSVideoDecoder::pause()
{
  [videoDecoderObject pauseVideo];
   videoState = PAUSED;
}

void IOSVideoDecoder::stop()
{
    [videoDecoderObject stopVideo];
    videoState = STOPPED;

}

void IOSVideoDecoder::setMute(bool enable)
{
    [videoDecoderObject setMute:enable];

    muted = enable;
}

bool IOSVideoDecoder::getMute()
{
    return muted;
}

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
    if(end)
    {
        videoState = STOPPED;
    }
    ended = end;
}


@implementation IOSVideoDecoderDelegate

AVAssetReader * _movieReader;
//AVAssetReader * _audioReader;

GLuint textureID;
IOSVideoDecoder *videoManager;
int maxBuffers = 3;
AudioQueueBufferRef audioQueueBuffer[3];
int audioBuffer = 0;
AudioTimeStamp* audioTimeStamp;
AudioQueueTimelineRef timeLine;
AudioQueueRef _playQueue = NULL;

static int counterAudioSamples = 0;
AudioStreamBasicDescription inAudioStreamBasicDescription;
static BOOL fullBuffer = false;
float volume;



- (id) init:(IOSVideoDecoder *) vm withURL: (NSURL *) url withTextureId:(GLuint)texture{
    if ((self = [super init])) {

        textureID = texture;
        AVURLAsset * asset = [AVURLAsset URLAssetWithURL:url options:nil];
        
        [asset loadValuesAsynchronouslyForKeys:[NSArray arrayWithObject:@"tracks"] completionHandler:
         ^{
         dispatch_async(dispatch_get_main_queue(),
                        ^{
                            AVAssetTrack * videoTrack = nil;
                            AVAssetTrack * audioTrack = nil;
                            
                            BOOL playable = [asset isPlayable];
                            
                            if (playable){
                                
                                NSError * error = nil;
                                
                                _movieReader = [[AVAssetReader alloc] initWithAsset:asset error:&error];
                                
                                [asset statusOfValueForKey:@"tracks" error:&error];
                                
                                if (error != nil){
                                    _movieReader = nil;
                                    logInf("IOSVideoDecoder: Error loading tracks inside assetReader");
                                }
                                //Check for video tracks
                                NSArray * tracks = [asset tracksWithMediaType:AVMediaTypeVideo];
                                if ([tracks count] == 1)
                                {
                                    videoTrack = [tracks objectAtIndex:0];
                             
                                    
                                    NSString* key = (NSString*)kCVPixelBufferPixelFormatTypeKey;
                                    NSNumber* value = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA];
                                    NSDictionary* videoSettings =
                                    [NSDictionary dictionaryWithObject:value forKey:key];
                                    
                                    [_movieReader addOutput:[AVAssetReaderTrackOutput
                                                             assetReaderTrackOutputWithTrack:videoTrack
                                                             outputSettings:videoSettings]];
                                    
                                }
                                // Check for audiotracks
                                NSArray *audioTracks =[asset tracksWithMediaType:AVMediaTypeAudio];
                                if ([audioTracks count] == 1)
                                {
                                    audioTrack = [audioTracks objectAtIndex:0];
                                    
                                    NSMutableDictionary* audioReadSettings = [NSMutableDictionary dictionary];
                                    [audioReadSettings setValue:[NSNumber numberWithInt:kAudioFormatLinearPCM]
                                                         forKey:AVFormatIDKey];
                                    
                                    [_movieReader addOutput:[AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:audioTrack outputSettings:audioReadSettings]];
                                    
                                }
                                
                                
                                [_movieReader startReading];
                            }
                            
                            vm->play();
                        });
         }];
        videoManager = vm;
        _playQueue = NULL;
        counterAudioSamples = 0;
        fullBuffer = false;

    }
    
    return self;
}

- (void) startVideo{
    [_movieReader startReading];
}

- (void) stopVideo{
    [_movieReader cancelReading];
    AudioQueueStop(_playQueue, YES);
    //TODO: Perform buffer free
    AudioQueueDispose(_playQueue, YES);
    _playQueue = NULL;
    counterAudioSamples = 0;
    videoManager->setEnded(true);
}

- (void) startPlaying{
    if(_playQueue != NULL){
        AudioQueueStart(_playQueue, audioTimeStamp);
    }
}


- (void) pauseVideo{
    AudioQueueGetCurrentTime(_playQueue, timeLine, audioTimeStamp, NULL);
    AudioQueuePause(_playQueue);
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
            //Free video frame
            CVPixelBufferRelease(imageBuffer);
        }
  
        
    }
    
    else if (_movieReader.status == AVAssetReaderStatusCompleted) {
        [self stopVideo];
    }
}


void audioCallback(void *                  inUserData,
                   AudioQueueRef           inAQ,
                   AudioQueueBufferRef     inCompleteAQBuffer)
{
    AVAssetReaderTrackOutput * audioOutput = [_movieReader.outputs objectAtIndex:1];
    CMSampleBufferRef audioSampleBufferRef = [audioOutput copyNextSampleBuffer];
    if (!audioSampleBufferRef){

        logErr("read next audio frame output fail");

        return;
    }
    
    CMBlockBufferRef blockBufferRef= CMSampleBufferGetDataBuffer(audioSampleBufferRef);
    
    AudioBufferList audioBufferList;
    
    CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(audioSampleBufferRef,
                                                            NULL,
                                                            &audioBufferList,
                                                            sizeof(audioBufferList),
                                                            NULL,
                                                            NULL,
                                                            0,
                                                            &blockBufferRef);
    
    if(audioBufferList.mNumberBuffers == 0) logErr("====================== mNumberBuffers = 0");
    OSStatus status;
    for( int i=0; i< audioBufferList.mNumberBuffers; i++ ){
        if (audioBufferList.mBuffers[i].mData && inCompleteAQBuffer->mAudioData) {
            
            //Buffer size are variable so this free is to be safe
            AudioQueueFreeBuffer(_playQueue, inCompleteAQBuffer);
            AudioBuffer audioBuffer = audioBufferList.mBuffers[i];

            status = AudioQueueAllocateBuffer(_playQueue, audioBuffer.mDataByteSize, &audioQueueBuffer[counterAudioSamples]);
            if (status == 0 && audioQueueBuffer[counterAudioSamples]->mAudioData) {
                memcpy(audioQueueBuffer[counterAudioSamples]->mAudioData, audioBuffer.mData, audioBuffer.mDataByteSize);
                audioQueueBuffer[counterAudioSamples]->mAudioDataByteSize = audioBuffer.mDataByteSize;
                AudioQueueEnqueueBuffer(_playQueue, audioQueueBuffer[counterAudioSamples], 0, NULL);

                counterAudioSamples = (counterAudioSamples +1)%maxBuffers;
            }

        }
        else{
            break;
        }
    }
    CMSampleBufferInvalidate(audioSampleBufferRef);

    return;
}




-(void) readAudio
{
    if(!fullBuffer){
        AVAssetReaderTrackOutput * audioOutput = [_movieReader.outputs objectAtIndex:1];
        CMSampleBufferRef sampleBuffer = [audioOutput copyNextSampleBuffer];
        if (sampleBuffer!= NULL )
        {
            CMBlockBufferRef blockBuffer;
            AudioBufferList audioBufferList;
            
            //Read the audio from the asset
            blockBuffer = CMSampleBufferGetDataBuffer( sampleBuffer );
            
            CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer( sampleBuffer,
                                                                    NULL,
                                                                    &audioBufferList,
                                                                    sizeof(audioBufferList),
                                                                    NULL,
                                                                    NULL,
                                                                    0,
                                                                    &blockBuffer);
            
            
            
            OSStatus status;
            if(_playQueue == NULL){
                
                AudioStreamBasicDescription inAudioStreamBasicDescription = *CMAudioFormatDescriptionGetStreamBasicDescription((CMAudioFormatDescriptionRef)CMSampleBufferGetFormatDescription(sampleBuffer));
                status = AudioQueueNewOutput(&inAudioStreamBasicDescription, audioCallback, NULL, NULL, NULL, 0, &_playQueue);
                if(status != 0) logInf("--------------------AudioQueueNewOutput OSStatus error %i", (int) status);
                
                status = AudioQueuePrime(_playQueue, 0, NULL);
                if(status != 0) logInf("--------------------AudioQueuePrime OSStatus error %i", (int) status);
                
                status = AudioQueueCreateTimeline(_playQueue, &timeLine);
                if(status != 0) logInf("--------------------AudioQueuePrime OSStatus error %i", (int) status);
            }
            
            
            [self fillAudioBufferList: &audioBufferList queue:audioQueueBuffer];
            
            
            //Buffers full now we are going to start the sound and continue reading from the asset in the callback
            if(counterAudioSamples == maxBuffers){
                status = AudioQueueStart(_playQueue, NULL);
                if(status != 0) logInf("--------------------AudioQueueStart OSStatus error %i", (int) status);
                for(int n = 0; n < maxBuffers; n++){
                    AudioTimeStamp bufferStartTime;
                    AudioQueueGetCurrentTime(_playQueue, NULL, &bufferStartTime, NULL);
                    
                    status = AudioQueueEnqueueBuffer(_playQueue, audioQueueBuffer[n], 0, NULL);
                    if(status != 0) logInf("--------------------AudioQueueEnqueueBuffer OSStatus error %i", (int) status);
                }
                fullBuffer = true;
                counterAudioSamples = 0;
            }
            
            CMSampleBufferInvalidate(sampleBuffer);

        }
    }
}

- (void) fillAudioBufferList: (AudioBufferList *) audioBufferList queue:(AudioQueueBufferRef *) audioQueueBuffer

{
    OSStatus status;

    for( int y=0; y< audioBufferList->mNumberBuffers; y++ ){
        
        AudioBuffer audioBuffer = audioBufferList->mBuffers[y];
        status = AudioQueueAllocateBuffer(_playQueue, audioBuffer.mDataByteSize, &audioQueueBuffer[counterAudioSamples]);
        
        if (status == 0 && audioQueueBuffer[counterAudioSamples]->mAudioData) {
            memcpy(audioQueueBuffer[counterAudioSamples]->mAudioData, audioBuffer.mData, audioBuffer.mDataByteSize);
            audioQueueBuffer[counterAudioSamples]->mAudioDataByteSize = audioBuffer.mDataByteSize;
            
            counterAudioSamples++;
            
        }else{
            logInf("--------------------OSStatus error %i", (int) status);
        }
        
    }
}


- (void) setMute:(BOOL) mute {
    OSStatus status;
    if (mute == true){
        status = AudioQueueGetParameter(_playQueue, kAudioQueueParam_Volume, &volume);
        status = AudioQueueSetParameter(_playQueue, kAudioQueueParam_Volume, 0);
    }
    
    else{
        status = AudioQueueSetParameter(_playQueue, kAudioQueueParam_Volume, volume);

    }
    if (status) {
        logInf("AudioQueueSetParameter returned %d when setting the volume.\n", (int)status);
    }
}

@end
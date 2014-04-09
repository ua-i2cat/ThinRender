//
//  IOSVideoDecoder.cpp
//  ModernismeMNACTEC
//
//  Created by Daniel Cabrera on 09/04/14.
//  Copyright (c) 2014 i2CAT. All rights reserved.
//

#include "IOSVideoDecoder.h"
#include "../../shader/gui/VideoPlaneShader.h"

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
    
    
    string filename = GlobalData::getInstance()->iOSPath+"/assets/"+path;
    NSString *videopath = [NSString stringWithCString:filename.c_str()
                                        encoding:[NSString defaultCStringEncoding]];
    NSURL *url = [NSURL fileURLWithPath:videopath];
    
    
   // BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:videopath];
    
    videoDecoderObject = [[IOSVideoDecoderDelegate alloc] init:this withURL:url withTextureId: textureID];
    
}

void IOSVideoDecoder::updateTexture(){
    [videoDecoderObject readNextMovieFrame];
}

IOSVideoDecoder::~IOSVideoDecoder(){instanceVideo = NULL;videoDecoderObject = NULL;};

void IOSVideoDecoder::setSource(std::string fileName){}
void IOSVideoDecoder::releaseVideo(){}
void IOSVideoDecoder::play(){
    [videoDecoderObject startVideo];
}
void IOSVideoDecoder::pause(){}
void IOSVideoDecoder::stop(){
    [videoDecoderObject stopVideo];
}
void IOSVideoDecoder::setMute(bool enable){}
bool IOSVideoDecoder::getMute(){return false;}
bool IOSVideoDecoder::isStopped(){return false;}
bool IOSVideoDecoder::isPaused(){return false;}
bool IOSVideoDecoder::isPlaying(){return true;}

void IOSVideoDecoder::setSplash(std::string texturePath){}
void IOSVideoDecoder::setSplash(){}

@implementation IOSVideoDecoderDelegate

AVAssetReader* _movieReader;
GLuint textureID;

- (id) init:(IOSVideoDecoder *) vm withURL: (NSURL *) url withTextureId:(GLuint)texture{
    if ((self = [super init])) {
        textureID = texture;
        AVURLAsset * asset = [AVURLAsset URLAssetWithURL:url options:nil];
        [asset loadValuesAsynchronouslyForKeys:[NSArray arrayWithObject:@"tracks"] completionHandler:
         ^{
         dispatch_async(dispatch_get_main_queue(),
                        ^{
                            AVAssetTrack * videoTrack = nil;
                            NSArray * tracks = [asset tracksWithMediaType:AVMediaTypeVideo];
                            if ([tracks count] == 1)
                            {
                                videoTrack = [tracks objectAtIndex:0];
                                
                                NSError * error = nil;
                                
                                // _movieReader is a member variable
                                _movieReader = [[AVAssetReader alloc] initWithAsset:asset error:&error];
                                /*if (error)
                                    NSLog(error.localizedDescription);*/
                                
                                NSString* key = (NSString*)kCVPixelBufferPixelFormatTypeKey;
                                NSNumber* value = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA];
                                NSDictionary* videoSettings = 
                                [NSDictionary dictionaryWithObject:value forKey:key]; 
                                
                                [_movieReader addOutput:[AVAssetReaderTrackOutput 
                                                         assetReaderTrackOutputWithTrack:videoTrack 
                                                         outputSettings:videoSettings]];
                            }
                        });
         }];
    }
    
    return self;
}

- (void) startVideo{
    [_movieReader startReading];
}

- (void) stopVideo{
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
            //uint8_t *baseAddress = (uint8_t *)CVPixelBufferGetBaseAddress(imageBuffer);
            //size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
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
            

            // Unlock the image buffer
            CVPixelBufferUnlockBaseAddress(imageBuffer,0);
            CFRelease(sampleBuffer);
        }
    }
}

@end
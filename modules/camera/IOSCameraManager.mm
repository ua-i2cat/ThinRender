/*
 *  thin render - Mobile render engine based on OpenGL ES 2.0
 *  Copyright (C) 2013  FundaciÛ i2CAT, Internet i InnovaciÛ digital a Catalunya
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
 *  Author:         Daniel Cabrera Benitez <dcabrera@quarkfly.com>
 *  Author:         Antonio Quesada Frias <aquesada@quarkfly.com>
 */

#include "IOSCameraManager.h"
#import <CoreVideo/CVPixelBuffer.h>
#include "../../controllerEGL/ContextControllerEAGL.h"
//#import <CoreVideo/CVOpenGLESTextureCache.h>

GLuint textureId;


IOSCameraManager::IOSCameraManager(){
    cameraObject = [[IOSCameraManagerDelegate alloc] init:this];
}

void IOSCameraManager::closeCamera(){
    [cameraObject stopCameraCapturing];

}

void IOSCameraManager::updateTextureCamera(){
    
    [cameraObject setNextFrame:YES];
}

bool IOSCameraManager::setCameraTexturePreview(int texture){
    
    textureId = texture;
    [cameraObject setTextureId:textureId];
    [cameraObject startCameraCapturing];
    return true;
}

void IOSCameraManager::setTextureId(GLuint tid){
    textureId = tid;
}

GLuint IOSCameraManager::getTextureId(){
    return textureId;
}


IOSCameraManager::~IOSCameraManager(){
    this->closeCamera();
    instanceCamera = NULL;
    cameraObject = NULL;
}



@implementation IOSCameraManagerDelegate
IOSCameraManager *cameraManager;
CVImageBufferRef pixelBuffer;
CVOpenGLESTextureCacheRef videoTextureCache2;
GLuint frameBufferHandle;


- (id) init:(IOSCameraManager *) cm{
    if ((self = [super init])) {
        AVCaptureDevice *backCamera = nil;
        NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
        for (AVCaptureDevice *device in devices)
        {
            if ([device position] == AVCaptureDevicePositionBack)
            {
                backCamera = device;
            }
        }
        
          // Create the capture session
        captureSession = [[AVCaptureSession alloc] init];
        
        // Add the video input
        NSError *error = nil;
        videoInput = [[AVCaptureDeviceInput alloc] initWithDevice:backCamera error:&error];
        if ([captureSession canAddInput:videoInput])
        {
            [captureSession addInput:videoInput];
        }
        // Add the video frame output
        videoOutput = [[AVCaptureVideoDataOutput alloc] init];
        [videoOutput setAlwaysDiscardsLateVideoFrames:YES];
        
        
        CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, ContextControllerEAGL::getInstance()->context, NULL, &videoTextureCache2);
        if(err){
            logErr("ERROR CREATING CVOpenGLESTextureCache!!! %d", err);
        }

        
        
        // Use RGB frames instead of YUV to ease color processing
        [videoOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
        
        [videoOutput setSampleBufferDelegate:self queue:dispatch_get_main_queue()];
        
        if ([captureSession canAddOutput:videoOutput])
        {
            [captureSession addOutput:videoOutput];
        }
        else
        {
            NSLog(@"Couldn't add video output");
        }
        
        cameraManager = cm;
    }
    return self;
}

- (void) startCameraCapturing{

    // Start capturing
    [captureSession setSessionPreset:AVCaptureSessionPresetHigh];
    //[captureSession setSessionPreset:AVCaptureSessionPreset640x480];
    if (![captureSession isRunning])
    {
        [captureSession startRunning];
    }
    
    NSLog(@"Camera is Running! = %d", [captureSession isRunning]);
    

}

-(void) setNextFrame:(BOOL) next
{
    nextFrame = next;
}

-(void) setTextureId:(int) texture
{
    textureId = texture;
}


- (void) stopCameraCapturing{
    
    if (![captureSession isRunning])
    {
        [captureSession stopRunning];
    }
}

#pragma mark -
#pragma mark AVCaptureVideoDataOutputSampleBufferDelegate
#define GL_TEXTURE_EXTERNAL_OES GL_TEXTURE_2D

BOOL nextFrame;

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    
    CVReturn err;
    if(!nextFrame)return;
    nextFrame = NO;
	CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);

    size_t width = CVPixelBufferGetWidth(pixelBuffer);
    size_t height = CVPixelBufferGetHeight(pixelBuffer);
    
	glBindTexture(GL_TEXTURE_2D, textureId);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// This is necessary for non-power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
	
	// Using BGRA extension to pull in video frame data directly
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, CVPixelBufferGetBaseAddress(pixelBuffer));
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);


}


@end

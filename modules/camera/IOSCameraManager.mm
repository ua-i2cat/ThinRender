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

IOSCameraManager::IOSCameraManager(){
    cameraObject = [[IOSCameraManagerDelegate alloc] init:this];
    [cameraObject startCameraCapturing];
    //pixelBuffer =
    //CVPixelBufferCreate(NULL, 480, 640, kCVPixelFormatType_32BGRA, nil, &pixelBuffer);
}


void IOSCameraManager::shutdownTextureWindow(){
  //  pixelBuffer = 0;
}

void IOSCameraManager::closeVideo(){
    //pixelBuffer = 0;
}

void IOSCameraManager::closeCamera(){
    //pixelBuffer = 0;
}

void IOSCameraManager::updateTextureVideo(){
    
   // pixelBuffer = 0;
}

void IOSCameraManager::updateTextureCamera(){
   //[cameraObject drawPixelBuffer];
   // [cameraObject updatePiuxelBuffer:textureId];
}

bool IOSCameraManager::setCameraTexturePreview(int texture){
    
  /*  //while(pixelBuffer==0) sleep(10);
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
	int bufferHeight = CVPixelBufferGetHeight(pixelBuffer);
	int bufferWidth = CVPixelBufferGetWidth(pixelBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bufferWidth, bufferHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, CVPixelBufferGetBaseAddress(pixelBuffer));

 */
    [cameraObject updatePixelBuffer:texture];
    [cameraObject drawPixelBuffer];
    return true;
}

IOSCameraManager::~IOSCameraManager(){
    [cameraObject stopCameraCapturing];
    instanceCamera = NULL;
    cameraObject = NULL;
}



@implementation IOSCameraManagerDelegate
IOSCameraManager *cameraManager;
CVImageBufferRef pixelBuffer;
int textureId;
CVOpenGLESTextureCacheRef videoTextureCache2;
EAGLContext* oglContext;
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
        oglContext = [EAGLContext currentContext];
        
        glDisable(GL_DEPTH_TEST);
        
        glGenFramebuffers(1, &frameBufferHandle);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);

        // Create the capture session
        captureSession = [[AVCaptureSession alloc] init];
        
        // Add the video input
        NSError *error = nil;
        videoInput = [[AVCaptureDeviceInput alloc] initWithDevice:backCamera error:&error];
        if ([captureSession canAddInput:videoInput])
        {
            [captureSession addInput:videoInput];
        }
        //CVPixelBufferCreate(NULL, GlobalData::getInstance()->screenWidth, GlobalData::getInstance()->screenHeight, kCVPixelFormatType_32BGRA, nil, &pixelBuffer);
        // Add the video frame output
        videoOutput = [[AVCaptureVideoDataOutput alloc] init];
        [videoOutput setAlwaysDiscardsLateVideoFrames:YES];
        
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

//#import "../../inputSystem/Input.h"

- (void) startCameraCapturing{
  
  //  glm::quat imuOrientation = (glm::quat(1, 1,0,0));
    
   // Input::getInstance()->deviceOrientation = imuOrientation;

    // Start capturing
    //[captureSession setSessionPreset:AVCaptureSessionPresetHigh];
    [captureSession setSessionPreset:AVCaptureSessionPreset640x480];
    if (![captureSession isRunning])
    {
        [captureSession startRunning];
    }
    
    NSLog(@"Camera is Running! = %d", [captureSession isRunning]);
    

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



-(void) updatePixelBuffer:(int) texId
{
    textureId = texId;
}


- (void) drawPixelBuffer
{
   // CVPixelBufferLockBaseAddress(pixelBuffer, 0);
   // dispatch_async(dispatch_get_main_queue(), ^{
        CVPixelBufferLockBaseAddress(pixelBuffer, 0);

    CVImageBufferRef pixelBuffer2 = pixelBuffer;
        
    glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    int bufferHeight = CVPixelBufferGetHeight(pixelBuffer2);
	int bufferWidth = CVPixelBufferGetWidth(pixelBuffer2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bufferWidth, bufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, CVPixelBufferGetBaseAddress(pixelBuffer2));
	glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_TEXTURE_2D);
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
   // });
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    CVPixelBufferLockBaseAddress(pixelBuffer, 0);
    pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    

}


@end

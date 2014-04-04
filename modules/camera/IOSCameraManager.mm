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

IOSCameraManager::IOSCameraManager(){
    cameraObject = [[IOSCameraManagerDelegate alloc] init:this];
}


void IOSCameraManager::shutdownTextureWindow(){
    
}

void IOSCameraManager::closeVideo(){
    
}

void IOSCameraManager::closeCamera(){
    
}

void IOSCameraManager::updateTextureVideo(){
    
}

void IOSCameraManager::updateTextureCamera(){
    
}

bool IOSCameraManager::setCameraTexturePreview(int texture){
    return true;
}

IOSCameraManager::~IOSCameraManager(){
    [cameraObject stopCameraCapturing];
    instanceCamera = NULL;
    cameraObject = NULL;
}



@implementation IOSCameraManagerDelegate

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
    

}

- (void) stopCameraCapturing{
    
    if (![captureSession isRunning])
    {
        [captureSession stopRunning];
    }
}

@end

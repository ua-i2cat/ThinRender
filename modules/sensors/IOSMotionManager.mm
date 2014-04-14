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

#include "IOSMotionManager.h"
#include "../../inputSystem/Input.h"
#include "../../inputSystem/IMU.h"

IOSMotionManager::IOSMotionManager(){
    
    motionObject = [[IOSMotionManagerDelegate alloc] init:this];
    
}

IOSMotionManager::~IOSMotionManager(){
    shutDownMotion();
    instanceMotion = NULL;
    motionObject = NULL;
}

void IOSMotionManager::initMotion(){
    [motionObject startUpdateMotion];
}

void IOSMotionManager::shutDownMotion(){
    [motionObject stopUpdateMotion];
}



@implementation IOSMotionManagerDelegate

CMMotionManager *motionManager;
IOSMotionManager *motionObject;

NSOperationQueue *opqueue;
CMDeviceMotionHandler dmHandler;

- (id) init:(IOSMotionManager *) mm
{
    if ((self = [super init])) {
        motionObject = mm;

        motionManager = [[CMMotionManager alloc] init];
        motionManager.deviceMotionUpdateInterval = 1.0/REFRESH_FREQUENCY; 
        
        dmHandler = ^(CMDeviceMotion * aMotion, NSError * error) {
            CMDeviceMotion *deviceMotion = motionManager.deviceMotion;
            CMAttitude * anAttitude = deviceMotion.attitude;
            CMQuaternion quaternion = anAttitude.quaternion;
            
            glm::quat imuOrientation = (glm::quat(quaternion.w, quaternion.x,quaternion.y, quaternion.z));
			
			Input::getInstance()->deviceOrientation = imuOrientation;
  
            // Checking if some errors occured
            if (error) {
                // Restart the motion manager in case of error
                [motionManager stopDeviceMotionUpdates];
                motionManager = nil;
                opqueue = nil;
                if (!motionManager) motionManager = [[CMMotionManager alloc] init];
                if (!opqueue) opqueue = [[NSOperationQueue alloc] init];
                [self startUpdateMotion];
                
                return;
            }
        };

    }
    return self;
}


- (void) startUpdateMotion
{
    logInf(">Start Update Motion");
    
    if (motionManager.deviceMotionAvailable) { //Checks gyroscope AND accelerometer avalalability
        
        opqueue = [NSOperationQueue currentQueue];
        [motionManager startDeviceMotionUpdatesUsingReferenceFrame:CMAttitudeReferenceFrameXTrueNorthZVertical toQueue:opqueue withHandler:dmHandler];
    }
    
}

- (void) stopUpdateMotion
{
    logInf(">Stop Update Position");
    [motionManager stopDeviceMotionUpdates];

}


//Automatic reference counting mode. Release not needed

@end

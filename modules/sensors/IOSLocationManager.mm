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

#include "IOSLocationManager.h"

IOSLocationManager::IOSLocationManager(){
    
    locationObject = [[IOSLocationManagerDelegate alloc] init:this];
    
}

IOSLocationManager::~IOSLocationManager(){
    shutDownGPS();
    instanceLocation = NULL;
    locationObject = NULL;
}

void IOSLocationManager::initGPS(){
    [locationObject startUpdatePosition];
}

void IOSLocationManager::shutDownGPS(){
    [locationObject stopUpdatePosition];
}


void IOSLocationManager::setPosition(double lat, double lon){
    latitude = lat;
    longitude = lon;
}


double IOSLocationManager::getLatitude(){
    return latitude;
}
double IOSLocationManager::getLongitude(){
    return longitude;
}



@implementation IOSLocationManagerDelegate

CLLocationManager *locationManager;
IOSLocationManager *locationObject;

- (id) init:(IOSLocationManager *) lm
{
    if ((self = [super init])) {
        locationManager = [[CLLocationManager alloc] init];
        locationManager.delegate = self;
        locationObject = lm;
        
    }
    return self;
}


- (void) startUpdatePosition
{
    NSLog(@">Start Update Position");
    [locationManager startUpdatingLocation];
}

- (void) stopUpdatePosition
{
    NSLog(@">Stop Update Position");
    [locationManager stopUpdatingLocation];
}

- (void)locationManager:(CLLocationManager *)manager didUpdateToLocation:(CLLocation *)newLocation fromLocation:(CLLocation *)oldLocation {
    NSLog(@">Did Update Position");
    locationObject->setPosition(newLocation.coordinate.latitude, newLocation.coordinate.longitude);
    
}

//FIXME: What to do with these errors
- (void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error {
    if(error.code == kCLErrorDenied) {
        NSLog(@"Error locating position. Stoping locating position");
        [locationManager stopUpdatingLocation];
    } else if(error.code == kCLErrorLocationUnknown) {
        // retry
    } else {
        NSLog(@"Error locating position");
    }
}



//Automatic reference counting mode. Release not needed

@end

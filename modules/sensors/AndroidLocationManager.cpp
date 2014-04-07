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
#include "AndroidLocationManager.h"


AndroidLocationManager::AndroidLocationManager(){
        
    //gps methods...
    shutdownGPSMethod = 0;
    getLatitudeMethod = 0;
    getLongitudeMethod = 0;
    initGPSMethod = 0;
    
    
}

AndroidLocationManager::~AndroidLocationManager(){
    shutDownGPS();
}


double AndroidLocationManager::getLatitude(){
    GlobalData *ginstance = GlobalData::getInstance();

    if(ginstance->jenv.javaVM == 0)return 0.0;
    if(getLatitudeMethod == 0){
        ginstance->jenv.javaVM->AttachCurrentThread(&(ginstance->jenv.env), 0);
        getLatitudeMethod =  (ginstance->jenv.env)->GetMethodID(ginstance->jenv.activityClass, "getLatitude", "()D");
    }
    return  (ginstance->jenv.env)->CallDoubleMethod(ginstance->jenv.activityObj, getLatitudeMethod);
}
double AndroidLocationManager::getLongitude(){
    GlobalData *ginstance = GlobalData::getInstance();

    if(ginstance->jenv.javaVM == 0)return 0.0;
    if(getLongitudeMethod == 0){
        ginstance->jenv.javaVM->AttachCurrentThread(&(ginstance->jenv.env), 0);
        getLongitudeMethod =  (ginstance->jenv.env)->GetMethodID(ginstance->jenv.activityClass, "getLongitude", "()D");
    }
    return  (ginstance->jenv.env)->CallDoubleMethod(ginstance->jenv.activityObj, getLongitudeMethod);
}

void AndroidLocationManager::initGPS(){
    GlobalData *ginstance = GlobalData::getInstance();

    logInf("initGPS Android ----");
    if(ginstance->jenv.javaVM == 0)return;
    ginstance->jenv.javaVM->AttachCurrentThread(&(ginstance->jenv.env), 0);
    if(initGPSMethod != 0){ return; }
    initGPSMethod =  (ginstance->jenv.env)->GetMethodID(ginstance->jenv.activityClass, "initGPS", "()V");//Ljava/lang/Object;
    (ginstance->jenv.env)->CallVoidMethod(ginstance->jenv.activityObj, initGPSMethod, ginstance->jenv.activityObj);
    return;
}

void AndroidLocationManager::shutDownGPS(){
    GlobalData *ginstance = GlobalData::getInstance();

    if(ginstance->jenv.javaVM == 0)return;
    if(shutdownGPSMethod == 0){
        ginstance->jenv.javaVM->AttachCurrentThread(&(ginstance->jenv.env), 0);
        shutdownGPSMethod =  (ginstance->jenv.env)->GetMethodID(ginstance->jenv.activityClass, "shutDownGPS", "()V");
    }
    (ginstance->jenv.env)->CallVoidMethod(ginstance->jenv.activityObj, shutdownGPSMethod);
    shutdownGPSMethod = 0;
    getLatitudeMethod = 0;
    getLongitudeMethod = 0;
    initGPSMethod = 0;
}

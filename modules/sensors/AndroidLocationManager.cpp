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
    javaVM = 0;
        
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
    if(javaVM == 0)return 0.0;
    if(getLatitudeMethod == 0){
        javaVM->AttachCurrentThread(&env, 0);
        getLatitudeMethod = env->GetMethodID(activityClass, "getLatitude", "()D");
    }
    return env->CallDoubleMethod(activityObj, getLatitudeMethod);
}
double AndroidLocationManager::getLongitude(){
    if(javaVM == 0)return 0.0;
    if(getLongitudeMethod == 0){
        javaVM->AttachCurrentThread(&env, 0);
        getLongitudeMethod = env->GetMethodID(activityClass, "getLongitude", "()D");
    }
    return env->CallDoubleMethod(activityObj, getLongitudeMethod);
}

void AndroidLocationManager::initGPS(){
    logInf("initGPS");
    if(javaVM == 0)return;
    javaVM->AttachCurrentThread(&env, 0);
    if(initGPSMethod != 0){ return; }
    initGPSMethod = env->GetMethodID(activityClass, "initGPS", "()V");//Ljava/lang/Object;
    env->CallVoidMethod(activityObj, initGPSMethod, activityObj);
    return;
}

void AndroidLocationManager::shutDownGPS(){
    if(javaVM == 0)return;
    if(shutdownGPSMethod == 0){
        javaVM->AttachCurrentThread(&env, 0);
        shutdownGPSMethod = env->GetMethodID(activityClass, "shutDownGPS", "()V");
    }
    env->CallVoidMethod(activityObj, shutdownGPSMethod);
    shutdownGPSMethod = 0;
    getLatitudeMethod = 0;
    getLongitudeMethod = 0;
    initGPSMethod = 0;
}

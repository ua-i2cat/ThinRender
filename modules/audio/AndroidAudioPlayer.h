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
 *  Author:         Ignacio Contreras Pinilla <ignacio.contreras@i2cat.net>
 */

#ifndef ANDROID_AUDIO_PLAYER_H
#define ANDROID_AUDIO_PLAYER_H

#include "AudioPlayer.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
//TODO: Quarkfly
/**
 * Basic audio player class. Currently it only accepts one audio file per instance.
 */
class AndroidAudioPlayer: public AudioPlayer {
    
public:
    AndroidAudioPlayer(std::string filePath);
    ~AndroidAudioPlayer();
    
private:
    
    static SLObjectItf engineObject;
    static SLEngineItf engineEngine;
    static SLObjectItf outputMixObject;
    
    SLObjectItf fdPlayerObject;
    SLPlayItf fdPlayerPlay;
    SLSeekItf fdPlayerSeek;
    SLMuteSoloItf fdPlayerMuteSolo;
    SLVolumeItf fdPlayerVolume;
    
    bool createAssetAudioPlayer(int fileDescriptor, long start, long length);
    void destroyPlayer();
    
	static bool createEngine();
    static void destroyEngine();
    
    static int playerCount;

    bool setPlayingAssetAudioPlayer(bool isPlaying);

    
};

#endif

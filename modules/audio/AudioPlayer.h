/*
 *  thin render - Mobile render engine based on OpenGL ES 2.0
 *  Copyright (C) 2013  Fundació i2CAT, Internet i Innovació digital a Catalunya
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

#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <sys/types.h>
#include <string>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

class AudioPlayer {

public:
    AudioPlayer(std::string filePath);
    ~AudioPlayer();

    bool changeSource(std::string filePath);
	bool play();
	bool pause();
	bool stop();

private:
    static SLObjectItf engineObject;
    static SLEngineItf engineEngine;
    static SLObjectItf outputMixObject;

    SLObjectItf fdPlayerObject;
    SLPlayItf fdPlayerPlay;
    SLSeekItf fdPlayerSeek;
    SLMuteSoloItf fdPlayerMuteSolo;
    SLVolumeItf fdPlayerVolume;

    bool createEngine();
    bool createAssetAudioPlayer(int fileDescriptor, long start, long length);
    bool setPlayingAssetAudioPlayer(bool isPlaying); 
    bool shutdown();
};

#endif

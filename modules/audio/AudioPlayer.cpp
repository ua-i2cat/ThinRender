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

#include "AudioPlayer.h"

#ifdef ANDROID_PLATFORM
  #include "AndroidAudioPlayer.h"
#else
  #include "IOSAudioPlayer.h"
#endif

AudioPlayer* AudioPlayer::instancePlayer = NULL;

AudioPlayer* AudioPlayer::getInstance(std::string filePath) {
    
    if (!instancePlayer){
        #ifdef ANDROID_PLATFORM
        instancePlayer = new AndroidAudioPlayer(filePath);
        #else
        instancePlayer = new IOSAudioPlayer(filePath);
        #endif
    }
    
    return instancePlayer;
}

bool AudioPlayer::play() {
    setPlayingAssetAudioPlayer(true);
	return playing = true;
}

bool AudioPlayer::pause() {
    setPlayingAssetAudioPlayer(false);
	return playing = false;
}

bool AudioPlayer::stop() {
    setPlayingAssetAudioPlayer(false);
	return playing = false;
}

bool AudioPlayer::isPlaying() {
	return playing;
}

void AudioPlayer::setEnded() {
	playing = false;
}

// Private methods
/*
bool AudioPlayer::setPlayingAssetAudioPlayer(bool isPlaying)
{
    return false;
}*/

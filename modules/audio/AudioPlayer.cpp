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
#include "../../globalData/GlobalData.h"
#include <android/asset_manager.h>
#include <android_native_app_glue.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "audio.h"

AudioPlayer::AudioPlayer(std::string filePath) {
    audioCreateEngine();
    audioCreateBufferQueueAudioPlayer();
    
    AAssetManager *assetManager = GlobalData::getInstance()->app->activity->assetManager;
    long start;
    long length;
    int fileDescriptor = FileSystem::getInstance()->getFileDescriptor(filePath, &start, &length);
    audioCreateAssetAudioPlayer(fileDescriptor, start, length);
}

AudioPlayer::~AudioPlayer() {
    stop();
    audioShutdown();
}

bool AudioPlayer::changeSource(std::string filePath) {
    long start;
    long length;
    int fileDescriptor = FileSystem::getInstance()->getFileDescriptor(filePath, &start, &length);
    audioChangeAssetAudioPlayer(fileDescriptor, start, length);
}

bool AudioPlayer::play() {
    audioSetPlayingAssetAudioPlayer(true);
}

bool AudioPlayer::pause() {
    audioSetPlayingAssetAudioPlayer(false);

}

bool AudioPlayer::stop() {
    audioSetPlayingAssetAudioPlayer(false);
}

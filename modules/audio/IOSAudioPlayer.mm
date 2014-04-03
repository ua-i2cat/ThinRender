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

#include "IOSAudioPlayer.h"

IOSAudioPlayer::IOSAudioPlayer(std::string filePath) {
    //TODO: Quarkfly
    NSLog(@"IOSAudioPlayer constructor");
    string filename = GlobalData::getInstance()->iOSPath+"/assets/"+filePath;
    NSString *path = [NSString stringWithCString:filename.c_str()
                       encoding:[NSString defaultCStringEncoding]];
    
    engineObject = [[IOSAudioPlayerDelegate alloc] initWithFile:path];
    this->setEnded();
}


bool IOSAudioPlayer::setPlayingAssetAudioPlayer(bool isPlaying)
{
    return [engineObject setPlaying:isPlaying];
}


IOSAudioPlayer::~IOSAudioPlayer(){
    NSLog(@"IOSAudioPlayer destructor");
    instancePlayer = NULL;
    engineObject = NULL;
}

@implementation IOSAudioPlayerDelegate
@synthesize audioPlayer;

- (id) initWithFile:(NSString *) path
{
    if ((self = [super init])) {
        NSError *error = nil;
        audioPlayer = [(AVAudioPlayer*)[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL fileURLWithPath:path] error:&error];
        if( error == nil) {
            [audioPlayer prepareToPlay];
            audioPlayer.delegate = self;
        }
        assert(error == nil);
    }
    return self;
}

- (BOOL) setPlaying:(BOOL) play
{
    if(play)
        [audioPlayer play];
    else
        [audioPlayer pause];
    return play;
}

//Automatic reference counting mode. Release not needed

@end
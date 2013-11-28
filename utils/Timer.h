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
 *  Author:         Marc Fernandez Vanaclocha <marc.fernandez@i2cat.net>
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#include "../Platform.h"

#ifdef WINDOWS_PLATFORM
#include <time.h>
#include <Windows.h>
//winmm.lib al input del linker, sino no va el timeGetTime
static int gettimeofday(struct timeval* tp, void* tzp) {
    unsigned int t;
    t = timeGetTime();
    tp->tv_sec = t / 1000;
    tp->tv_usec = (t % 1000) * 1000;
    /* 0 indicates that the call succeeded. */
    return 0;
}
#endif
#ifndef WINDOWS_PLATFORM
#include <sys/time.h>
#endif
/**
 * Timer class controls the delta time and some other functions, uses Singleton pattern
 */
class Timer{
private:
	static Timer* instance;
	void initTimer();
	struct timeval start, current;
	int lastTime;
	int currentTime;
	int deltaTime;
	int offset;
public:
	static Timer* getInstance();

	Timer();

    /**
     * reset sets to init value all the timers
     */
	void reset();
    
    /**
     * getCurrentTime
     * @return int the time in ms
     */
	int getCurrentTime();
    
    /**
     * getCurrentTimeInSeconds
     * @return float the time in s
     */
	float getCurrentTimeInSeconds();
    
    /**
     * calculeCurrentTime updates the timer, this needs to be called 1 time per frame
     */
	void calculeCurrentTime();
    
    /**
     * getDeltaTime
     * @return int delta time in ms from the last update
     */
	int getDeltaTime();
    
	/**
     * setDeltaTime (method used by online game)
     * @param int delta time in ms
     */
	void setDeltaTime(int deltaTime);

    /**
     * method called after button pause, reset delta time
	 */
    void release();

    /**
     * getRawTime
     * @return int delta time in ms from the start
     */
	int getRawTime();
};
#endif

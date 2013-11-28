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

#include "Timer.h"

Timer* Timer::instance = 0;
Timer* Timer::getInstance(){
	if(Timer::instance == 0)
		instance = new Timer();
	return instance;
}

Timer::Timer(){
	initTimer();
}

void Timer::reset(){
	initTimer();
}

void Timer::initTimer(){
	gettimeofday(&start, 0);
	lastTime = 0;
	currentTime = 0;
	deltaTime = 0;
	offset = 0;
}
int Timer::getCurrentTime(){
	return currentTime - offset;
}

float Timer::getCurrentTimeInSeconds(){
	float result = currentTime - offset;
	return (result/1000.0f);
}

void Timer::calculeCurrentTime(){
	gettimeofday(&current, 0);
	lastTime = currentTime;
	currentTime =(int)((current.tv_sec*1000 -  start.tv_sec*1000)
						+ (current.tv_usec - start.tv_usec)/1000 + 0.5f);
	deltaTime = currentTime - lastTime;
}
int Timer::getRawTime(){
	struct timeval current;
	gettimeofday(&current, 0);
	int result = (int)((current.tv_sec*1000 -  start.tv_sec*1000)
			+ (current.tv_usec - start.tv_usec)/1000 + 0.5f);
	return result;
}

void Timer::release(){
	calculeCurrentTime();
	offset += deltaTime;
	deltaTime = 0;
}

int Timer::getDeltaTime(){
	return deltaTime;
}
void Timer::setDeltaTime(int deltaTime){
	this->deltaTime = deltaTime;
}

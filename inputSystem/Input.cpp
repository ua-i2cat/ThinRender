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

#include "Input.h"

Input* Input::instance = 0;

Input* Input::getInstance(){
	if(Input::instance == 0){
		instance = new Input();
	}
	return instance;
}
void Input::freeInstance(){
	if(Input::instance != 0){
		delete instance;
	}
    instance = 0;
}

Input::Input(){
    lastID = 0;
}

Input::~Input(){
	eventVector.clear();
}

// TODO: better doxygen docs

/**
 * Delete an event from the vector
 * @param int id id of the element to delete
 */
void Input::deleteEvent(int id){
    for(std::vector<event>::iterator it = eventVector.begin(); it != eventVector.end(); it++){
        if((*it).idEvent == id){
            eventVector.erase(it);
            return;
        }
    }
}

/*
 * Sets the event's active flag to false (e.g. finger outside the screen)
 */
bool Input::updateEventToEnd(float lastX, float lastY){
    for(std::vector<event>::iterator it = eventVector.begin(); it < eventVector.end(); it++){
        if((floor((*it).x) == floor(lastX) || floor((*it).x) == (floor(lastX)-1) || floor((*it).x) == (floor(lastX)+1)) && (floor((*it).y) == floor(lastY) || floor((*it).y) == (floor(lastY)+1) || floor((*it).y) == (floor(lastY)-1))){
            (*it).active = false;
            (*it).visited = false;
            (*it).state = END_INPUT;
            return true;
        }
    }
    return false;
}

/*
 * Sets the event's active flag to false (e.g. finger outside the screen)
 */
bool Input::updateEventToEnd(int id){
    for(std::vector<event>::iterator it = eventVector.begin(); it < eventVector.end(); it++){
        if((*it).idEvent == id){
            (*it).active = false;
            (*it).visited = false;
            (*it).state = END_INPUT;
            return true;
        }
    }
    return false;
}

/**
 * On screen touch
 */
void Input::newEvent(float x,float y){
    event e = {x,y,x,y,lastID,BEGIN_INPUT,true,false, false};
    eventVector.push_back(e);
    lastID++;
}
void Input::newEvent(int id, float x,float y){
    event e = {x,y,x,y,id,BEGIN_INPUT,true,false, false};
    eventVector.push_back(e);
}

/**
 * Always mark the event as not visited in case of movement
 */
bool Input::updateEvent(float lastX, float lastY, float newX, float newY){
    for(std::vector<event>::iterator it = eventVector.begin(); it < eventVector.end(); it++){
        if((*it).x == lastX && (*it).y == lastY){
            (*it).x = newX;
            (*it).y = newY;
            (*it).xLast = lastX;
            (*it).yLast = lastY;
            (*it).state = UPDATE_INPUT;
            (*it).visited = false;
            return true;
        }
    }
    return false;
}

/**
 * Always mark the event as not visited in case of movement
 */
bool Input::updateEvent(int id, float newX, float newY){
    for(std::vector<event>::iterator it = eventVector.begin(); it < eventVector.end(); it++){
        if((*it).idEvent == id){
            (*it).xLast = (*it).x;
            (*it).yLast = (*it).y;
            (*it).x = newX;
            (*it).y = newY;
            (*it).state = UPDATE_INPUT;
            (*it).visited = false;
            return true;
        }
    }
    return false;
}

void Input::clearEvents(){
	eventVector.clear();
}

bool Input::noActiveEvents(){
    for(std::vector<event>::iterator it = eventVector.begin(); it < eventVector.end(); it++){
        if((*it).active){
            return false;
        }
    }
    return true;
}

/**
 * Returns the unvisited events and marks them as visited
 */
std::vector<event> Input::getEventsNotLooked(){
    std::vector<event> eV;
    for(std::vector<event>::iterator it = eventVector.begin(); it < eventVector.end(); it++){
        if((*it).visited == false && !(*it).flushed){
            eV.push_back((*it));
            // Android only gives the difference between inputs and we're expected to
            // mantain the static events (i.e. two-finger zoom: we should preserve the
            // both fingers events even if one of them is not moving)
            if ((*it).state == Input::BEGIN_INPUT) {
                (*it).visited = true;
            }
        }
        if(!(*it).active){
        	eventVector.erase(it);
        }
    }
    return eV;
}

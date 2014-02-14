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

#ifndef INPUT_H
#define INPUT_H

#include <vector>
#include "../utils/Maths.h"

/**
 * Struct with the basic data to evaluate multiple touches
 */
struct event{
    float x,y;
    float xLast, yLast;
    int idEvent;
    int state;
    bool active;
    bool visited;
    bool flushed;
};

/**
 * Class to access all data related with inputs, works with Singleton pattern
 */
class Input {
public:
	glm::quat deviceOrientation;
	const static int BEGIN_INPUT = 0;
	const static int END_INPUT = 1;
	const static int UPDATE_INPUT = -1;

    /**
     * static function, the first execution or the first after calling freeInstance() calls the private constructor
     */
	static Input* getInstance();

    /**
     * static function, if the instance is different to 0 calls the destructor and sets it to 0
     */
    static void freeInstance();
	/*
	 -deleteEvent borra el event del vector
	 -updateEventToEnd posa l'actiu a false (s'ha alliberat el event, p.e: dit fora de la pantalla)
	 -newEvent quan es polsa
	 -updateEvent quan es mou, posarem visited a false sempre
	 -getEventsNotLooked retorna tots els events q tenen visited a false, i es seteja a true
     -clearEvents() neteja tots els events marcant-los com a flushed(va be entre escenes)
	 */
    /**
     * Deletes the event with id form parameter from the internal vector, normally used when a finger releases touching
     */
	void deleteEvent(int id);
    /**
     * Creates an internal event, normally used when there is a new finger on screen
     */
	void newEvent(float x,float y);
	/**
	 * Creates an internal event, normally used when there is a new finger on screen using a id
	 */
	void newEvent(int id, float x,float y);
    /**
     * Search and updates an event using the last and current position, it works on iOS, in Android you need to call update event passing the id
     * @return true if successfully updated, false otherwise
     */
	bool updateEvent(float lastX, float lastY, float newX, float newY);
	/**
	 * Search and updates an event using the id
	 * @return true if successfully updated, false otherwise
	 */
	bool updateEvent(int id, float newX, float newY);
	/**
	 * Search and updates an event using the id, used to avoid stange effects in Android
	 * @return true if successfully updated, false otherwise
	 */
	bool updateEvent(int id);
    /**
     * updates an event to the end using the last position, this happens when a finger release the screen
     * @return true if successfully updated, false otherwise
     */
	bool updateEventToEnd(float lastX, float lastY);
    /**
     * updates an event to the end using the id, this happens when a finger release the screen
     * @return true if successfully updated, false otherwise
     */
	bool updateEventToEnd(int id);
    /**
     * flushes all events, is used normally when changes the screen and wants to avoid previous events
     */
    void clearEvents();
    /**
     * gets if there is no one active event
     * @return true if there aren't active events, false otherwise
     */
    bool noActiveEvents();
    /**
     * @return a vector with the new events from the previous call
     */
	std::vector<event> getEventsNotLooked();
private:
	Input();
	~Input();
	static Input* instance;
	int lastID;
	std::vector<event> eventVector;
};
#endif

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

#include "SliderBlockGUI.h"

void SliderBlockGUI::update(float xDiff, float yDiff){

	if(xDiff == -1.0f){

		assert(rects.size > 0);
		RectGUI* nearestRect = rects[0];
		float minLeft = abs(rects[0]->getLeft());

		for(int i = 0; i < rects.size(); i++){
			if(abs(rects[i]->getLeft()) < minLeft) {
				nearestRect = rects[i];
			}
		}
		xDiff = nearestRect->getLeft();
		logInf("xDiff = %f", xDiff);

		/*
		glm::vec3 position = internalNode->getPosition();

		assert(rects.size > 0);
		glm::vec3 rectPosition = rects[0]->getPosition;
		glm::vec3 diff = rectPosition - position;

		float distance = dot(diff, diff);
		float minDistance = distance;
		RectGUI nearestRect = rects[0];

		for(int i = 0; i < rects.size(); i++){
			rectPosition = rects[i]->getPosition();
			diff = rectPosition - position;
			distance = dot(diff, diff);
			if(distance < minDistance){
				nearestRect = rects[i];
			}
		}
		return;
		*/
	}

	glm::vec3 position = internalNode->getPosition();
	if(type == HORIZONTAL_SLIDER){
		position.x = abs(position.x);
		position.x -= xDiff;
		if(position.x > maxTranslation) position.x = maxTranslation;
		if(position.x < minTranslation) position.x = minTranslation;
		//logInf("position.x: %f",position.x);
		position.x = -position.x;
	}else{
		position.y = -abs(position.y);
		position.y += yDiff;
		if(position.y < maxTranslation) position.y = maxTranslation;
		if(position.y > minTranslation) position.y = minTranslation;
		position.y = -position.y;
	}
	internalNode->setPosition(position);
}

SliderBlockGUI::~SliderBlockGUI(){
	for(int i = 0; i < rects.size(); i++){
		delete rects[i];
		rects[i] = 0;
	}
	rects.clear();
}

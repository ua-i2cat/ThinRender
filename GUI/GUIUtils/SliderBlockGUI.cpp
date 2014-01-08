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

void SliderBlockGUI::update(float xDiff, float yDiff, bool input){

	if(!input){

		assert(rects.size() > 0);

		glm::vec3 position = glm::vec3(this->width*0.5f, 0.0f, 0.0f);
		glm::vec3 internalNodePosition = internalNode->getPosition();
		RectGUI* currentRect = rects[0];
		glm::vec3 rectPosition = glm::vec3(currentRect->getLeft() + 0.5f*currentRect->getWidth() + internalNodePosition.x, 0.0f, 0.0f);
		glm::vec3 diff = position - rectPosition;
		float distance = glm::dot(diff, diff);

		logInf("SliderBlockGUI::update with no input - slider center at (%f, %f, %f)", position[0], position[1], position[2]);
		logInf("SliderBlockGUI::update with no input - slider internal node at (%f, %f, %f)", internalNode->getPosition()[0], internalNode->getPosition()[1], internalNode->getPosition()[2]);

		float minDistance = distance;
		RectGUI* nearestRect = currentRect;
		float nodeOffset = 0.0f;
		float nodeOffsetAux = currentRect->getWidth() + 10.0f;
		for(int i = 1; i < rects.size(); i++){
			currentRect = rects[i];
			rectPosition = glm::vec3(currentRect->getLeft() + 0.5f*currentRect->getWidth() + internalNodePosition.x, 0.0f, 0.0f);
			diff = position - rectPosition;
			distance = glm::dot(diff, diff);
			if(distance < minDistance){
				minDistance = distance;
				nearestRect = rects[i];
				nodeOffset = nodeOffsetAux;
				logInf("minDistance for index %d", i);
			}
			nodeOffsetAux += currentRect->getWidth() + 10.0f;
		}

		internalNode->setPosition(glm::vec3(0.5f*this->width - 0.5f*nearestRect->getWidth() - nodeOffset, internalNodePosition.y, internalNodePosition.z));
		logInf("SliderBlockGUI::update with no input - final node position (%f, %f, %f)", internalNode->getPosition()[0], internalNode->getPosition()[1], internalNode->getPosition()[2]);
		logInf("SliderBlockGUI::update with no input - this->width = %f - getWidth() = %f - nodeOffset = %f", this->width, nearestRect->getWidth(), nodeOffset);
		return;
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

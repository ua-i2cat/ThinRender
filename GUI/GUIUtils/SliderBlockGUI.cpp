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
 *                  Marc Fernandez Vanaclocha <marc.fernandez@i2cat.net>
 */

#include "SliderBlockGUI.h"
#include "../../utils/Timer.h"

void SliderBlockGUI::update(float xDiff, float yDiff, bool input){
	if(!input){

		assert(rects.size() > 0);

		glm::vec3 position = glm::vec3(this->width*0.5f, 0.0f, 0.0f);
		glm::vec3 internalNodePosition = internalNode->getPosition();
		RectGUI* currentRect;
		this->index = 0;
		glm::vec3 rectPosition;
		glm::vec3 diff;
		float distance;

		float minDistance = 10000000.0f;//max float value better
		RectGUI* nearestRect = 0;
		float nodeOffset = minTranslation;
		float nodeOffsetAux = nodeOffset;
		for(int i = 0; i < rects.size(); i++){
			currentRect = rects[i];
			rectPosition = glm::vec3(currentRect->getLeft() + 0.5f*currentRect->getWidth() + internalNodePosition.x - this->left, 0.0f, 0.0f);
			diff = position - rectPosition;
			distance = glm::dot(diff, diff);
			if(distance < minDistance){
				minDistance = distance;
				nearestRect = rects[i];
				this->index = i;
				nodeOffset = nodeOffsetAux;
				if(i>0){
					nodeOffset += currentRect->getWidth()*0.5f;
				}
			}
			if(i == 0){
				nodeOffsetAux += currentRect->getWidth()*0.5f + 10.0f;
			}else{
				nodeOffsetAux += currentRect->getWidth() + 10.0f;
			}
		}
        float maxSpeed = GlobalData::getInstance()->screenWidth * 0.9f;
        float dist = maxSpeed * (float)Timer::getInstance()->getDeltaTime()/1000.0f;
        glm::vec3 finalPosition = glm::vec3(minTranslation -  nodeOffset, internalNodePosition.y, internalNodePosition.z);
        if(abs(finalPosition.x - internalNodePosition.x) > dist){
			finalPosition.x = dist * Maths::signf(finalPosition.x - internalNodePosition.x) + internalNodePosition.x;
		}
        internalNode->setPosition(finalPosition);
		return;
	}

	glm::vec3 position = internalNode->getPosition();
	if(type == HORIZONTAL_SLIDER){
		position.x = - position.x;
		position.x -= xDiff;
		if(position.x > maxTranslation) position.x = maxTranslation;
		if(position.x < (minTranslation-0.5f*rects[0]->getWidth())) position.x = minTranslation-0.5f*rects[0]->getWidth();
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

void SliderBlockGUI::includeRect(RectGUI* rect, float offset){
	if(type == HORIZONTAL_SLIDER){
		float left = this->width*0.5f;
		if(rects.size() == 0){
			left += -0.5f * rect->getWidth();
			minTranslation = left;
		}else{
			left += -0.5f * rects[0]->getWidth();
		}
		for(int i = 0; i < rects.size(); i++){
			left += rects[i]->getWidth() + offset;
		}
		rect->setPosition(left + this->left,rect->getTop());
		maxTranslation = left;
	}else{
		float top = 0.0f;
		for(int i = 0; i < rects.size(); i++){
			top -= rects[i]->getHeight() + offset;
		}
		rect->setPosition(rect->getLeft(),this->top + top);
		maxTranslation = height + top - rect->getHeight();
	}
	rects.push_back(rect);
}
int SliderBlockGUI::getIndex(){
	return this->index;
}

RectGUI* SliderBlockGUI::getCurrentRect(){
	return rects[this->getIndex()];
}

RectGUI* SliderBlockGUI::getRect(int i){
	return rects[i];
}

SliderBlockGUI::~SliderBlockGUI(){
	for(int i = 0; i < rects.size(); i++){
		delete rects[i];
		rects[i] = 0;
	}
	rects.clear();
}

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


#include "SliderGUI.h"
#include "../../globalData/GlobalData.h"
#include "../../log/Log.h"

SliderGUI::SliderGUI(float left, float top, float width, float height, int type){
	this->left = left;
	this->top = top;
	this->width = width;
	this->height = height;
	minTranslation = 0.0f;
	maxTranslation = 0.0f;
	internalNode = GlobalData::getInstance()->scene->getRootNode()->createChild();
	this->type = type;
	projMatrix = glm::ortho(left, left+width, top-height, top, 0.0f, 10.0f);
}

SliderGUI::~SliderGUI(){
	for(int i = 0; i < rects.size(); i++){
		delete rects[i];
		rects[i] = 0;
	}
	rects.clear();
}

void SliderGUI::includeRect(RectGUI* rect){
	if(type == HORIZONTAL_SLIDER){
		float left = 0.0f;
		for(int i = 0; i < rects.size(); i++){
			left += rects[i]->getWidth() + 10.0f;
		}
		rect->setPosition(left,rect->getTop());
		maxTranslation = left - this->left;
	}else {
		float top = 0.0f;
		for(int i = 0; i < rects.size(); i++){
			top -= rects[i]->getHeight() + 10.0f;
		}
		rect->setPosition(rect->getLeft(),this->top + top);
		maxTranslation = height + top - rect->getHeight();
	}
	rects.push_back(rect);
}

void SliderGUI::update(float xDiff, float yDiff){
	glm::vec3 position = internalNode->getPosition();
	if(type == HORIZONTAL_SLIDER){
		position.x = abs(position.x);
		position.x -= xDiff;
		if(position.x > maxTranslation) position.x = maxTranslation;
		if(position.x < minTranslation) position.x = minTranslation;
		//logInf("position.x: %f",position.x);
		position.x = -position.x;
	}else {
		position.y = -abs(position.y);
		position.y += yDiff;
		if(position.y < maxTranslation) position.y = maxTranslation;
		if(position.y > minTranslation) position.y = minTranslation;
		position.y = -position.y;
	}
	internalNode->setPosition(position);
}
bool SliderGUI::isInside(float x, float y){
	if(x < left || x > (left + width) || y > top || y < (top - height)){
		return false;
	}
	return true;
}

RectGUI* SliderGUI::click(float x, float y){
	glm::vec3 position = internalNode->getPosition();
	for(int i = 0; i < rects.size(); i++){
		if(rects[i]->isInside(x-position.x, y-position.y)){
			return rects[i];
		}
	}
	return 0;
}
void SliderGUI::draw(){
	glViewport((int)left,(int)top-(int)height,(int)width,(int)height);
	glm::mat4 resultMatrix = projMatrix*internalNode->getFullTransform();
	for(int i = 0; i < rects.size(); i++){
		rects[i]->draw(resultMatrix);
	}
	glViewport(0,0,GlobalData::getInstance()->screenWidth,GlobalData::getInstance()->screenHeight);
}
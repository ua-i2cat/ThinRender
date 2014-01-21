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

#include "Rect.h"
#include "../log/Log.h"
#include "TextManager.h"

RectGUI::RectGUI(float top, float left, float width, float height, Node* nodeRect){
	plane = MeshManager::getInstance()->getMeshPlane2D(0.0f, float(GlobalData::getInstance()->screenHeight), width, height);
	plane->shader->color = glm::vec4(0.0f,0.0f,0.0f,1.0f);
	plane->options = Mesh::GUI_MASK;
	node = nodeRect;
	this->width = width;
	this->height = height;
	setProjectionMatrix();
	setPosition(left, top);
	text = 0;
	clickable = false;
	textMessage = "";
	enabled = true;
}

RectGUI::RectGUI(Node* nodeRect, float left, float top, float w, float h){
	plane = MeshManager::getInstance()->getMeshPlane2D(0.0f, float(GlobalData::getInstance()->screenHeight), w, h);
	plane->shader->color = glm::vec4(0.0f,0.0f,0.0f,1.0f);
	plane->options = Mesh::GUI_MASK;
	node = nodeRect;

	width = w;
	height = h;

	setProjectionMatrix();
	setPosition(left, top);

	text = 0;
	clickable = false;
	textMessage = "";
	enabled = true;
}

RectGUI::~RectGUI(){
	if(plane != 0){
		delete plane;
	}
}

void RectGUI::setTexture(Texture* texture){
    if(texture == 0){
        plane->shader->texture = 0;
        return;
    }
    if(plane->shader->texture != 0 && plane->shader->texture->getXratio() == texture->getXratio() && plane->shader->texture->getYratio() == texture->getYratio()){
    	plane->shader->texture = texture;
    	return;
    }
	plane->shader->texture = texture;

	plane->subMeshes.at(0)->textureCoord.clear();
	float marginXTexture = 0.5f/(float)texture->getXSquare();
	float marginYTexture = 0.5f/(float)texture->getYSquare();

	plane->subMeshes.at(0)->textureCoord.push_back(glm::vec2(1.0f * texture->getXratio()-marginXTexture, 	1.0f * texture->getYratio()-marginYTexture));
	plane->subMeshes.at(0)->textureCoord.push_back(glm::vec2(0.0f+marginXTexture,					  		1.0f * texture->getYratio()-marginYTexture));
	plane->subMeshes.at(0)->textureCoord.push_back(glm::vec2(0.0f+marginXTexture, 					  		0.0f+marginYTexture));
	plane->subMeshes.at(0)->textureCoord.push_back(glm::vec2(1.0f * texture->getXratio()-marginXTexture, 	1.0f * texture->getYratio()-marginYTexture));
	plane->subMeshes.at(0)->textureCoord.push_back(glm::vec2(0.0f+marginXTexture,					  		0.0f+marginYTexture));
	plane->subMeshes.at(0)->textureCoord.push_back(glm::vec2(1.0f * texture->getXratio()-marginXTexture, 	0.0f+marginYTexture));

	plane->generateVBO();
}

void RectGUI::setTexture(unsigned int glTextId, float xRatio, float yRatio){
	if(plane->shader->texture == 0) return;
	plane->shader->texture->setTextureId(glTextId);
}

void RectGUI::setTexture(unsigned char* imageData){
	if(plane->shader->texture == 0) return;
	plane->shader->texture->updateTexture(imageData);
}

void RectGUI::setPosition(float newLeft, float newTop){
	node->setPosition(glm::vec3(newLeft, newTop - GlobalData::getInstance()->screenHeight, 0.0f));
	top = newTop;
	left = newLeft;
}

void RectGUI::setWidthAndHeight(float newWidth, float newHeight){
	width = newWidth;
	height = newHeight;
	glm::vec3 scaleFactor = node->getScale();
	scaleFactor.x = width;
	scaleFactor.y = height;
	node->setScale(scaleFactor);
	setPosition(left, top);
}
void RectGUI::setWidth(float newWidth){
	width = newWidth;
	glm::vec3 scaleFactor = node->getScale();
	scaleFactor.x = width;
	node->setScale(scaleFactor);
	setPosition(left, top);
}

void RectGUI::setHeight(float newHeight){
	height = newHeight;
	glm::vec3 scaleFactor = node->getScale();
	scaleFactor.y = height;
	node->setScale(scaleFactor);
	setPosition(left, top);
}

void RectGUI::setShader(Shader* shader){
	if(plane->shader != 0){
		delete plane->shader;
	}
	plane->shader = shader;
}
void RectGUI::draw(){
	if(!enabled)return;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm::mat4 pvmMatrix = projMatrix * node->getFullTransform();
	if(plane->shader->texture != 0){
		plane->render(pvmMatrix, projMatrix, glm::mat4(), 0, plane->shader->texture->getTextureId(), glm::vec4(0.0f), glm::vec4(0.0f), glm::vec3(0.0f), glm::mat4(0.0f), glm::mat4(0.0f));
	}
	if(text != 0){
		text->render(pvmMatrix);
	}
	glDisable(GL_BLEND);
}
void RectGUI::draw(glm::mat4 intermediateMatrix){
	if(!enabled)return;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm::mat4 pvmMatrix = intermediateMatrix * node->getFullTransform();
	if(plane->shader->texture != 0){
		plane->render(
				pvmMatrix, projMatrix, glm::mat4(), 0, plane->shader->texture->getTextureId(), glm::vec4(0.0f),
				glm::vec4(0.0f), glm::vec3(0.0f), glm::mat4(0.0f), glm::mat4(0.0f)
			);
	}
	if(text != 0){
		text->render(pvmMatrix);
	}
	glDisable(GL_BLEND);
}

float RectGUI::getTop(){
	return top;
}

float RectGUI::getLeft(){
	return left;
}

float RectGUI::getWidth(){
	return width;
}

float RectGUI::getHeight(){
	return height;
}
void RectGUI::setTextColor(glm::vec4 color){
	if(text == 0) return;
	text->color = color;
}
void RectGUI::setColor(glm::vec4 color){
	plane->shader->color = color;
}

void RectGUI::setText(std::string message, std::string typographyFile, int fontSize, int align, float offsetX, float offsetY){
	textMessage = message;
	text = TextManager::getInstance()->getText(typographyFile, fontSize);
	float widthMessage = text->getWidthOfMessage(textMessage.c_str());
	float heightMessage = text->getHeightOfMessage(textMessage.c_str());
	if(align == 0){
		text->setText(textMessage, width/2.0f - widthMessage/2.0f + offsetX, GlobalData::getInstance()->screenHeight-height/2.0f-heightMessage/2.0f + offsetY);
	}else{//LEFT ALIGN!!! :D
		text->setText(textMessage, offsetX, GlobalData::getInstance()->screenHeight-height/2.0f-heightMessage/2.0f + offsetY);
	}
}

void RectGUI::setBlockText(std::string message, std::string typographyFile, int fontSize, int align){
	textMessage = message;
	text = TextManager::getInstance()->getText(typographyFile, fontSize);
	text->setBlockText(textMessage, width, align);
}

void RectGUI::setProjectionMatrix(){
	float leftOrtho = 0.0f;
	float rightOrtho = GlobalData::getInstance()->screenWidth;
	float bottomOrtho = 0.0f;
	float topOrtho = GlobalData::getInstance()->screenHeight;
	float zNearOrtho = 0.0f;
	float zFarOrtho = 10.0f;
	projMatrix = glm::ortho(leftOrtho, rightOrtho, bottomOrtho, topOrtho, zNearOrtho, zFarOrtho);
}

void RectGUI::setClickable(bool click){
	clickable = click;
}

bool RectGUI::isInside(float x, float y){
	if(!enabled)return false;
	if(!clickable) return false;
	if(x < left || x > (left + width) || y > top || y < (top - height)){
		return false;
	}
	return true;
}

std::string RectGUI::getMessage(){
	return textMessage;
}

void RectGUI::setEnabled(bool enabled){
	this->enabled = enabled;
}

bool RectGUI::getEnabled(){
	return enabled;
}

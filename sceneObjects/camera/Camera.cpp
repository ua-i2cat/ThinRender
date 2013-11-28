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

#include "Camera.h"

Camera::Camera(){
	nearClip = 2.0f;
	farClip = 500.0f;
	aspectRatio = 16.0f/9.0f;
	apertureAngle = 45.0f;
	cameraType = Camera::PERSPECTIVE;
	enabled = true;
	cacheProjection = false;
	projection = glm::mat4(0);

	leftOrtho = -1.0f;
	rightOrtho = 1.0f;
	bottomOrtho = -1.0f;
	topOrtho = 1.0f;
	zNearOrtho = 2.0f;
	zFarOrtho = 500.0f;
}

Camera::Camera(float nearDistance, float farDistance, float ratio, float aperture){
	nearClip = nearDistance;
	farClip = farDistance;
	aspectRatio = ratio;
	apertureAngle = aperture;
	cameraType = Camera::PERSPECTIVE;
	enabled = true;
	cacheProjection = false;
	projection = glm::mat4(0);

	leftOrtho = -1.0f;
	rightOrtho = 1.0f;
	bottomOrtho = -1.0f;
	topOrtho = 1.0f;
	zNearOrtho = -1.0f;
	zFarOrtho = 1.0f;
}

Camera::Camera(float left, float right, float bottom, float top, float zNear, float zFar){
	leftOrtho = left;
	rightOrtho = right;
	bottomOrtho = bottom;
	topOrtho = top;
	zNearOrtho = zNear;
	zFarOrtho = zFar;
	cameraType = Camera::ORTHOGONAL;
	enabled = true;
	cacheProjection = false;
	projection = glm::mat4(0);

	nearClip = 0.1f;
	farClip = 100.0f;
	aspectRatio = 16.0f/9.0f;
	apertureAngle = 45.0f;
}

Camera::~Camera(){

}

void Camera::setNearClipDistance(float nearDistance){
	nearClip = nearDistance;
	cacheProjection = false;
}
void Camera::setFarClipDistance(float farDistance){
	farClip = farDistance;
	cacheProjection = false;
}
void Camera::setAspectRatio(float ratio){
	aspectRatio = ratio;
	cacheProjection = false;
}
void Camera::setAperture(float aperture){
	apertureAngle = aperture;
	cacheProjection = false;
}
void Camera::setCameraType(int type){
	cameraType = type;
	cacheProjection = false;
}

float Camera::getNearClipDistance(){
	return nearClip;
}
float Camera::getFarClipDistance(){
	return farClip;
}
float Camera::getAspectRatio(){
	return aspectRatio;
}
float Camera::getAperture(){
	return apertureAngle;
}
int Camera::getCameraType(){
	return cameraType;
}

void Camera::setEnabled(bool enable){
	enabled = enable;
}
bool Camera::getEnabled(){
	return enabled;
}

void Camera::setLeftOrtho(float left){
	leftOrtho = left;
	cacheProjection = false;
}
void Camera::setRightOrtho(float right){
	rightOrtho = right;
	cacheProjection = false;
}
void Camera::setBottomOrtho(float bottom){
	bottomOrtho = bottom;
	cacheProjection = false;
}
void Camera::setTopOrtho(float top){
	topOrtho = top;
	cacheProjection = false;
}
void Camera::setZNearOrtho(float zNear){
	zNearOrtho = zNear;
	cacheProjection = false;
}
void Camera::setZFarOrtho(float zFar){
	zFarOrtho = zFar;
	cacheProjection = false;
}

float Camera::getLeftOrtho(){
	return leftOrtho;
}
float Camera::getRightOrtho(){
	return rightOrtho;
}
float Camera::getBottomOrtho(){
	return bottomOrtho;
}
float Camera::getTopOrtho(){
	return topOrtho;
}
float Camera::getZNearOrtho(){
	return zNearOrtho;
}
float Camera::getZFarOrtho(){
	return zFarOrtho;
}
void Camera::setProjectionMatrix(glm::mat4 projectionMatrix){
	cacheProjection = true;
	projection = projectionMatrix;
}
//#include "../../log/Log.h"
glm::mat4 Camera::getViewMatrix(){
	if(attachedNode == 0) return glm::mat4(0);
	viewMatrix = attachedNode->parent->getFullTransform();
	viewMatrix = viewMatrix * glm::translate(glm::mat4(1.0f), attachedNode->getPosition()) * glm::mat4_cast(attachedNode->getOrientation());
	viewMatrix = glm::inverse(viewMatrix);
/*	viewMatrix = glm::mat4_cast(glm::inverse(attachedNode->getHierarchyOrientation()));
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), attachedNode->getHierarchyPosition());
	viewMatrix = translation * viewMatrix;
	viewMatrix = glm::inverse(viewMatrix);*/
/*	logInf("getViewMatrix result matrix:");
	int j;
	for (j=0; j<4; j++){
		logInf("|%f %f %f %f|",viewMatrix[0][j], viewMatrix[1][j], viewMatrix[2][j], viewMatrix[3][j]);
	}*/
	return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix(){
	if(!cacheProjection){
		switch(cameraType){
		case PERSPECTIVE:
			projection = glm::perspective(apertureAngle, aspectRatio, nearClip, farClip);
			cacheProjection = true;
			break;
		case ORTHOGONAL:
			projection = glm::ortho(leftOrtho, rightOrtho, bottomOrtho, topOrtho, zNearOrtho, zFarOrtho);
			cacheProjection = true;
			break;
		}

	}
	return projection;
}

glm::mat4 Camera::getOrthogonalProjectionMatrix(){
	leftOrtho = -2.0f;
	rightOrtho = 2.0f;
	bottomOrtho = -2.0f;
	topOrtho = 2.0f;
	zNearOrtho = 2.0f;
	zFarOrtho = 500.0f;
	return glm::ortho(leftOrtho, rightOrtho, bottomOrtho, topOrtho, zNearOrtho, zFarOrtho);
}

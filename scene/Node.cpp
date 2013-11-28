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

#include "Node.h"
#include "../sceneObjects/SceneObject.h"
#include "../log/Log.h"

Node::Node(){
	localPosition = glm::vec3(0);
	localOrientation = glm::quat();
	localScale = glm::vec3(1.0f);

	//these only are actualized when scene need it
	derivedPosition = glm::vec3(0);
	derivedOrientation = glm::quat();
	derivedScale = glm::vec3(1.0f);

//	localModel = glm::mat4();
	derivedModel= glm::mat4();

	parent = 0;
	setModified();
}

Node::~Node(){
    for(int i = 0; i < childs.size(); i++){
        delete childs.at(i);
        childs.at(i) = 0;
    }
	childs.clear();
}

void Node::attachSceneObject(SceneObject* so){
	std::vector<SceneObject*>::iterator it = std::find(sceneObjects.begin(), sceneObjects.end(), so);
	if(it != sceneObjects.end()) return;
	sceneObjects.push_back(so);
	so->attachTo(this);
}
void Node::unatachSceneObject(SceneObject* so){
	std::vector<SceneObject*>::iterator it = std::find(sceneObjects.begin(), sceneObjects.end(), so);
	if(it == sceneObjects.end()) return;
	(*it)->unatachNodeWithoutConsistence();
	(*it) = 0;
	sceneObjects.erase(it);
}
void Node::unatachSceneObjectWithoutConsistence(SceneObject* so){
	std::vector<SceneObject*>::iterator it = std::find(sceneObjects.begin(), sceneObjects.end(), so);
	if(it == sceneObjects.end()) return;
	(*it) = 0;
	sceneObjects.erase(it);
}


//tree system
void Node::setParent(Node* parent){
	if((this->parent != 0) && (parent != 0) && (this->parent != parent)){
		this->parent->unatachChild(this);
	}
	this->parent = parent;
}

Node* Node::createChild(){
	Node* child = new Node();
	child->setParent(this);
	childs.push_back(child);
	return child;
}
void Node::atachChild(Node* child){
	child->setParent(this);
	childs.push_back(child);
}
void Node::unatachChild(Node* child){
	std::vector<Node*>::iterator it = childs.begin();
	while((it != childs.end()) && (*it != child)) it++;
	//(*it)->setParent(0);
	//(*it) = 0;
	childs.erase(it);
}

void Node::deleteChild(Node* child){
	std::vector<Node*>::iterator it = childs.begin();
	while((it != childs.end()) && (*it != child)) it++;
    delete (*it);
	childs.erase(it);
}
//coordinates
void Node::setPosition(glm::vec3 position){
	localPosition = position;
	setModified();
}

glm::vec3 Node::getPosition(){
	return localPosition;
}

void Node::setOrientation(glm::quat orientation){
	localOrientation = orientation;
	setModified();
}

glm::quat Node::getOrientation(){
	return localOrientation;
}
void Node::resetOrientation(){
	localOrientation = glm::quat();
	setModified();
}

void Node::setScale(glm::vec3 scale){
	localScale = scale;
	setModified();
}

glm::vec3 Node::getScale(){
	return localScale;
}


void Node::rotate(glm::vec3 axis, float angleOffset, TransformSpace relativeTo){
	// assert:  axis[] is unit length
	//
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

	glm::quat q;
	float fHalfAngle = (0.5f * angleOffset);
	float fSin = Maths::sinf(fHalfAngle);
	q.w = Maths::cosf(fHalfAngle);
	q.x = fSin*axis.x;
	q.y = fSin*axis.y;
	q.z = fSin*axis.z;
//	logInf("rotate from axis {%f,%f,%f} degrees:%f result quaternion {%f,%f,%f,%f}", axis.x, axis.y, axis.z, angleOffset, q.x, q.y, q.z, q.w);
	rotate(q, relativeTo);
}

void Node::rotate(glm::quat orientationOffset, TransformSpace relativeTo){
	glm::quat qnorm = orientationOffset;
	qnorm = glm::normalize(orientationOffset);// o qnorm.normalize();

	switch(relativeTo)
	{
	case TS_PARENT:
		// Rotations are normally relative to local axes, transform up
		localOrientation = qnorm * localOrientation;
		break;
	case TS_WORLD:
		// Rotations are normally relative to local axes, transform up
		localOrientation = localOrientation * glm::inverse(getHierarchyOrientation())
			* qnorm * getHierarchyOrientation();
		break;
	case TS_LOCAL:
		// Note the order of the mult, i.e. q comes after
//		logInf("rotate localOrientation{%f,%f,%f,%f} qnorm{%f,%f,%f,%f}", localOrientation.x, localOrientation.y, localOrientation.z, localOrientation.w,
//				qnorm.x, qnorm.y, qnorm.z, qnorm.w);
		localOrientation = localOrientation * qnorm;
		break;
	}
	setModified();
}

void Node::translate(float x, float y, float z, TransformSpace relativeTo){
	translate(glm::vec3(x,y,z), relativeTo);
}

void Node::translate(glm::vec3 positionOffset, TransformSpace relativeTo)
{
	switch(relativeTo)
	{
	case TS_LOCAL:
		// position is relative to parent so transform downwards
		localPosition += localOrientation * positionOffset;
		break;
	case TS_WORLD:
		// position is relative to parent so transform upwards
		if (parent) {
			localPosition += (glm::inverse(parent->getHierarchyOrientation()) * positionOffset)
				/ parent->getHierarchyPosition();
		}else {
			localPosition += positionOffset;
		}
		break;
	case TS_PARENT:
		localPosition += positionOffset;
		break;
	}
	setModified();
}

void Node::scale(glm::vec3 scaleOffset)
{
	localScale = localScale * scaleOffset;
	setModified();
}

void Node::scale(float x, float y, float z)
{
	localScale.x *= x;
	localScale.y *= y;
	localScale.z *= z;
	setModified();
}
void Node::lookAt(glm::vec3 targetPoint)
{

//	logInf("localPosition {%f, %f, %f}", localPosition.x, localPosition.y, localPosition.z);
	//glm::vec3 worldPos = getHierarchyPosition();
	glm::vec3 worldPos = localPosition;
	glm::mat4 parentInversTransform = glm::inverse(parent->getFullTransform());
	targetPoint = glm::vec3(parentInversTransform * glm::vec4(targetPoint, 1.0f));
	//logInf("worldPos {%f, %f, %f}", worldPos.x, worldPos.y, worldPos.z);
	//logInf("targetPoint {%f, %f, %f}", targetPoint.x, targetPoint.y, targetPoint.z);


	glm::vec3 upVector = glm::vec3(0.0f,1.0f,0.0f);
	glm::vec3 destVector = targetPoint - worldPos;
	destVector = glm::normalize(destVector);
	float cosTheta = glm::dot(upVector, destVector);

	if (abs(cosTheta) > 0.9f){//are near parallel!
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		/*upVector = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), upVector);
		if (glm::length2(upVector) < 0.01f ) // bad luck, they were parallel, try again!
			upVector = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f,1.0f,0.0f));

		upVector = glm::normalize(upVector);*/
		upVector = glm::vec3(0.0f,0.0f,1.0f);
	}
//	logInf("worldPos %f %f %f targetPoint %f %f %f", worldPos.x,worldPos.y,worldPos.z , targetPoint.x,targetPoint.y,targetPoint.z);
//	logInf("destVector {%f, %f, %f} dot: %f upVector {%f, %f, %f}", destVector.x, destVector.y, destVector.z, cosTheta, upVector.x, upVector.y, upVector.z);
	glm::mat4 resultMatrix = glm::lookAt(
			worldPos,			// the position of your camera, in world space
			targetPoint,   		// where you want to look at, in world space
			upVector	// up vector glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
		);
//	glm::vec4 result = resultMatrix * glm::vec4(targetPoint, 1.0f);
/*	logInf("lookAt result {%f, %f, %f}  target point {%f, %f, %f}", result.x, result.y, result.z, targetPoint.x, targetPoint.y, targetPoint.z);//OK
*/
	/*logInf("result matrix look at:");
	int j;
	for (j=0; j<4; j++){
		logInf("|%f %f %f %f|",resultMatrix[0][j], resultMatrix[1][j], resultMatrix[2][j], resultMatrix[3][j]);
	}

	resultMatrix = glm::inverse(parent->getFullTransform()) * resultMatrix;
	logInf("============= result matrix look at:");
	for (j=0; j<4; j++){
		logInf("|%f %f %f %f|",resultMatrix[0][j], resultMatrix[1][j], resultMatrix[2][j], resultMatrix[3][j]);
	}*/
	glm::quat resultOrientation = glm::toQuat(resultMatrix);
//	logInf("!!!!!!!!!!!!!!!!!resultOrientation {%f, %f, %f, %f}", resultOrientation.w, resultOrientation.x, resultOrientation.y, resultOrientation.z);


//	resultOrientation = convertWorldToLocalOrientation(resultOrientation);
//	logInf("AAAAAAAAAAAAAAAAAresultOrientation {%f, %f, %f, %f}", resultOrientation.w, resultOrientation.x, resultOrientation.y, resultOrientation.z);

//		return glm::inverse(getHierarchyOrientation()) * worldOrientation;
	setOrientation(glm::inverse(resultOrientation));

//	logInf("orientation {%f, %f, %f, %f}", localOrientation.w, localOrientation.x, localOrientation.y, localOrientation.z);
//	logInf("derivedOrientation {%f, %f, %f, %f}", getHierarchyOrientation().w, getHierarchyOrientation().x, getHierarchyOrientation().y, getHierarchyOrientation().z);

}

glm::quat Node::getHierarchyOrientation(){
	if(changed){
		updateFromParent();
	}
	return derivedOrientation;
}

glm::vec3 Node::getHierarchyPosition(){
	if(changed){
		updateFromParent();
	}
	return derivedPosition;
}

glm::vec3 Node::getHierarchyScale(){
	if(changed){
		updateFromParent();
	}
	return derivedScale;
}

glm::quat Node::getHierarchyParentOrientation(){
	if(parent){
		return parent->getHierarchyOrientation();
	}
	return glm::quat();
}
glm::vec3 Node::getHierarchyParentPosition(){
	if(parent){
		return parent->getHierarchyPosition();
	}
	return glm::vec3(0);
}
glm::vec3 Node::getHierarchyParentScale(){
	if(parent){
		return parent->getHierarchyScale();
	}
	return glm::vec3(1);
}

glm::mat4 Node::getFullTransform(){
	if(transformNeedToUpdate){
		derivedModel = makeTransform(getHierarchyPosition(), getHierarchyScale(), getHierarchyOrientation());
		transformNeedToUpdate = false;
	}
	return derivedModel;
}

glm::quat Node::convertWorldToLocalOrientation(glm::quat worldOrientation)
{
	return glm::inverse(parent->getHierarchyOrientation()) * worldOrientation;
}

glm::quat Node::convertLocalToWorldOrientation(glm::quat localOrientation){
	return parent->getHierarchyOrientation() * localOrientation;
}

glm::vec3 Node::convertWorldToLocalPosition(glm::vec3 worldPos){
	return glm::inverse(getHierarchyOrientation()) * (worldPos - getHierarchyPosition()) / getHierarchyScale();
}

glm::vec3 Node::convertLocalToWorldPosition(glm::vec3 localPos){
	return (getHierarchyOrientation() * localPosition * getHierarchyScale()) + getHierarchyPosition();
}

void Node::setChange(){
	setModified();
}


glm::mat4 Node::makeTransform(glm::vec3 position, glm::vec3 scale, glm::quat orientation)
{
	// Ordering:
	//    1. Scale
	//    2. Rotate
	//    3. Translate
/*	logInf("makeTransform");
	logInf("makeTransform position: {%f, %f, %f}" , position.x, position.y, position.z);
	logInf("makeTransform scale: {%f, %f, %f}" , scale.x, scale.y, scale.z);
	logInf("makeTransform orientation: {%f, %f, %f, %f}" , orientation.x, orientation.y, orientation.z, orientation.w);
*/
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 rotationMatrix = glm::toMat4(orientation);
	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), position);
	return translateMatrix*rotationMatrix*scaleMatrix;
}


void Node::updateFromParent(){
//	logInf("updateFromParent parent: %i", parent);
	if (parent){
		glm::quat parentOrientation = parent->getHierarchyOrientation();
		derivedOrientation = parentOrientation * localOrientation;

		glm::vec3 parentScale = parent->getHierarchyScale();
		derivedScale = parentScale * localScale;

		derivedPosition = parentOrientation * (parentScale * localPosition);
		derivedPosition += parent->getHierarchyPosition();
//		logInf("derivedPosition: {%f, %f, %f} ", derivedPosition.x, derivedPosition.y, derivedPosition.z);
//		logInf("localPosition: {%f, %f, %f} ", localPosition.x, localPosition.y, localPosition.z);
	}else {
		// Root node, no parent
		derivedOrientation = localOrientation;
		derivedPosition = localPosition;
		derivedScale = localScale;
	}
	changed = false;
}

glm::quat Node::lookRotation(glm::vec3 lookAt, glm::vec3 upDirection) {
	glm::vec3 forward = glm::normalize(lookAt);
	glm::vec3 up = upDirection;
	forward = glm::orthonormalize(forward, up);
	glm::vec3 right = glm::cross(up, forward);
	glm::quat ret;
	ret.w = Maths::sqrtf(1.0f + right.x + up.y + forward.z) * 0.5f;
	float w4_recip = 1.0f / (4.0f * ret.w);
	ret.x = (up.z - forward.y) * w4_recip;
	ret.y = (forward.x - right.z) * w4_recip;
	ret.z = (right.y - up.x) * w4_recip;
	return ret;
}
/*
glm::quat Node::quatFromTo(glm::vec3 from, glm::vec3 to){
	glm::quat q;
	glm::vec3 crossV = glm::cross(from, to);
	q.x = crossV.x;
	q.y = crossV.y;
	q.z = crossV.z;
	q.w = sqrt((from.Length ^ 2) * (to.Length ^ 2)) + glm::dot(from, to));
	return glm::normalize(q);
}
*/
glm::quat Node::getRotationFromTo(glm::vec3 origin, glm::vec3 dest)//, glm::vec3 fallbackAxis = glm::vec3(0))
{
	// Based on Stan Melax's article in Game Programming Gems
	glm::quat q;
	// Copy, since cannot modify local
	glm::vec3 v0 = origin;
	glm::vec3 v1 = dest;
	v0 = glm::normalize(v0);
	v1 = glm::normalize(v1);

	float d = glm::dot(v0, v1);
	// If dot == 1, vectors are the same
	if (d >= 1.0f){
		return glm::quat();//IDENTITY
	}
	if (d < (1e-6f - 1.0f))
	{
		/*if (fallbackAxis != Vector3::ZERO)
		{
			// rotate 180 degrees about the fallback axis
			q.FromAngleAxis(Radian(Math::PI), fallbackAxis);
		}
		else
		{*/
			// Generate an axis
			glm::vec3 axis = glm::cross(glm::vec3(1.0f,0.0f,0.0f), v1);//Vector3::UNIT_X.crossProduct(*this);
			//if (axis.isZeroLength()) // pick another if colinear
				//axis = glm::cross(glm::vec3(0.0f,1.0f,0.0f), v1);
			axis = glm::normalize(axis);
			q = glm::angleAxis(Maths::PI, axis);
//			q.FromAngleAxis(Radian(Math::PI), axis);
		//}
	}
	else
	{
		float s = Maths::sqrtf( (1+d)*2 );
		float invs = 1 / s;
		glm::vec3 c = glm::cross(v0, v1);

		q.x = c.x * invs;
		q.y = c.y * invs;
		q.z = c.z * invs;
		q.w = s * 0.5f;
		q = glm::normalize(q);
	}
	return q;
}

glm::vec3 Node::getForwardVector()
{
	glm::vec3 result = localOrientation * glm::vec3(0.0f,0.0f,-1.0f);
	return result;
}

glm::vec3 Node::getWorldForwardVector()
{
	glm::vec3 result = getHierarchyOrientation() * glm::vec3(0.0f,0.0f,-1.0f);
	return result;
}
glm::vec3 Node::getUpVector()
{
	glm::vec3 result = localOrientation * glm::vec3(0.0f,1.0f,0.0f);
	return result;
}

glm::vec3 Node::getRightVector()
{
	glm::vec3 result = localOrientation * glm::vec3(1.0f,0.0f,0.0f);
	return result;
}

void Node::setModified(){
	changed = true;
	transformNeedToUpdate = true;
	for (int i=0; i < childs.size(); i++){
		childs.at(i)->setChange();
	}
}

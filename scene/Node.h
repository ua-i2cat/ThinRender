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

#ifndef NODE_H
#define NODE_H
//based in Ogre3D node
#include "../utils/Maths.h"
#include <vector>
#include <algorithm>
#include <string>

class SceneObject;

///TransformSpace enum
enum TransformSpace
{
	/// Transform is relative to the local space
	TS_LOCAL,
	/// Transform is relative to the space of the parent node
	TS_PARENT,
	/// Transform is relative to world space
	TS_WORLD
};
/**
 * Node class is the the ones that manage the different spaces and the matrix propagation
 * It provides the option to anidate different nodes
 */
class Node {
public:
    
	Node();
	~Node();
    ///childs attached to the node
	std::vector<Node*> childs;
    ///scene objects attached to the node
    std::vector<SceneObject*> sceneObjects;

    /**
     * attachSceneObject
     * @param SceneObject* so
     */
	void attachSceneObject(SceneObject* so);
    /**
     * unattachSceneObject
     * @param SceneObject* so
     */
	void unatachSceneObject(SceneObject* so);
	/**
     * unatachSceneObjectWithoutConsistence (internal use, private?)
     * @param SceneObject* so
     */
    void unatachSceneObjectWithoutConsistence(SceneObject* so);

    /**
     * setParent
     * @param Node* parent
     */
	void setParent(Node* parent);

    /**
     * createChild creates a child and set it to the childs vector
     * @return Node* child
     */
	Node* createChild();

    /**
     * atachChild adds the node to the child vector, avoid graphs, is a tree!
     * @param Node* child
     */
    void atachChild(Node* child);
	
    /**
     * deleteChild deletes the child pointer and all their sons
     * @param Node* child
     */
    void deleteChild(Node* child);
    
    /**
     * unatachChild gets the child out without deleting the child and sons, be careful with memory leaks
     * @param Node* child
     */
	void unatachChild(Node* child);

	/**
     * setPosition from local space
     * @param glm::vec3 position (x,y,z)
     */
	void setPosition(glm::vec3 position);
    
    /**
     * getPosition from local space
     * @return glm::vec3 (x,y,z) format
     */
	glm::vec3 getPosition();

    /**
     * setOrientation from local space
     * @param glm::quat orientation (w,x,y,z)
     */
	void setOrientation(glm::quat orientation);
    
    /**
     * getOrientation from local space
     * @return glm::quat (w,x,y,z)
     */
	glm::quat getOrientation();
    
    /**
     * resetOrientation set quaternion identity as orientation
     */
    void resetOrientation();

	/**
     * setScale from local space
     * @param glm::vec3 scale (x,y,z)
     */
    void setScale(glm::vec3 scale);
	/**
     * getScale from local space
     * @return glm::vec3 (x,y,z)
     */
	glm::vec3 getScale();

	/**
     * rotate
     * @param glm::vec3 axis to rotate (x,y,z)
     * @param float angleOffset in radiants
     * @param relative space, by default is local
     */
	void rotate(glm::vec3 axis, float angleOffset, TransformSpace relativeTo = TS_LOCAL);
	
    /**
     * rotate
     * @param glm::quat offset (w,x,y,z)
     * @param relative space, by default is local
     */
	void rotate(glm::quat orientationOffset, TransformSpace relativeTo = TS_LOCAL);

	/**
     * translate adds to current position
     * @param float x
     * @param float y
     * @param float z
     * @param relative space, by default is local
     */
    void translate(float x, float y, float z, TransformSpace relativeTo = TS_PARENT);
	
    /**
     * translate adds to current position
     * @param glm::vec3 positionOffset (x,y,z)
     * @param relative space, by default is local
     */
	void translate(glm::vec3 positionOffset, TransformSpace relativeTo = TS_PARENT);

	/**
     * scale multiplies current scale with params
     * @param float x
     * @param float y
     * @param float z
     */
    void scale(float x, float y, float z);
	/**
     * scale multiplies current scale with params
     * @param glm::vec3 scaleOffset (x,y,z)
     */
	void scale(glm::vec3 scaleOffset);

    /**
     * lookAt modifies the local orientation to look at the target in the world space
     * @param glm::vec3 targetPoint (x,y,z) in world space
     */
	void lookAt(glm::vec3 targetPoint);

    /**
     * getHierarchyOrientation
     * @return glm::quat orientation in world space
     */
	glm::quat getHierarchyOrientation();
    
    /**
     * getHierarchyPosition
     * @return glm::vec3 position in world space
     */
	glm::vec3 getHierarchyPosition();
    
    /**
     * getHierarchyScale
     * @return glm::vec3 scale in world space
     */
	glm::vec3 getHierarchyScale();

    /**
     * getHierarchyParentOrientation
     * @return glm::quat orientation in world space of the parent
     */
	glm::quat getHierarchyParentOrientation();
    /**
     * getHierarchyParentPosition
     * @return glm::vec3 position in world space of the parent
     */
	glm::vec3 getHierarchyParentPosition();
    /**
     * getHierarchyParentScale
     * @return glm::vec3 scale in world space of the parent
     */
	glm::vec3 getHierarchyParentScale();

    /**
     * getFullTransform
     * @return glm::mat4 model matrix, (translation * rotation * scale)
     */
	glm::mat4 getFullTransform();

    /**
     * convertWorldToLocalOrientation
     * @param glm::quat worldOrientation
     * @return glm::quat in local space (inverse(parentOrientation) * worldOrientation)
     */
    //test
	glm::quat convertWorldToLocalOrientation(glm::quat worldOrientation);
    
    /**
     * convertLocalToWorldOrientation
     * @param glm::quat localOrientation
     * @return glm::quat in local space (parentOrientation * localOrientation)
     */
	glm::quat convertLocalToWorldOrientation(glm::quat localOrientation);
    
    /**
     * convertWorldToLocalPosition
     * @param glm::vec3 worldPos
     * @return glm::vec3 in local space (inverse(parentCompleteHierarchi) * worldPos)
     */
    glm::vec3 convertWorldToLocalPosition(glm::vec3 worldPos);
    
    /**
     * convertLocalToWorldPosition
     * @param glm::vec3 localPos
     * @return glm::vec3 in local space (parentCompleteHierarchi * localPos)
     */
    glm::vec3 convertLocalToWorldPosition(glm::vec3 localPos);

	///used to change childs (internal use)
	void setChange();

    /**
     * getForwardVector from local space
     * @return glm::vec3
     */
	glm::vec3 getForwardVector();
    
    /**
     * getWorldForwardVector from world space
     * @return glm::vec3
     */
	glm::vec3 getWorldForwardVector();
	
    /**
     * getUpVector from local space
     * @return glm::vec3
     */
    glm::vec3 getUpVector();
	
    /**
     * getRightVector from local space
     * @return glm::vec3
     */
    glm::vec3 getRightVector();
//http://stackoverflow.com/questions/1766284/opengl-position-and-orientation-from-modelview-matrix
    
    ///parent node
	Node* parent;

	void setId(int id){
		idNode = id;
	}

	int getId(){
		return idNode;
	}

	void setName(std::string name){
		nameNode = name;
	}

	std::string getName(){
		return nameNode;
	}
    
private:
	int idNode;
	std::string nameNode;
	glm::quat rotationBetweenVectors(glm::vec3 start, glm::vec3 dest);

	glm::vec3 localPosition;
	glm::quat localOrientation;
	glm::vec3 localScale;

	//these only are actualized when scene need it
	glm::vec3 derivedPosition;
	glm::quat derivedOrientation;
	glm::vec3 derivedScale;

	glm::mat4 derivedModel;

	//bool inherateOrientation, inherateScale;

	glm::mat4 makeTransform(glm::vec3 position, glm::vec3 scale, glm::quat orientation);

	void updateFromParent();

	//to avoid recalculate matrix, if this doesn't change, the childs will recalculate the matrix (if they have this boolean to false)
	bool changed;
	bool transformNeedToUpdate;

	glm::quat lookRotation(glm::vec3 lookAt, glm::vec3 upDirection);
	glm::quat getRotationFromTo(glm::vec3 origin, glm::vec3 dest);

	void setModified();

};
#endif

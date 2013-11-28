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

#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include "../scene/Node.h"
#include <string>

/**
 * Base class for all scene objects, interacts with Node
 */
class SceneObject {
public:
	static const int NO_TYPE = -1;
	static const int CAMERA_TYPE = 0;
	static const int MESH_TYPE = 1;
	static const int LIGHT_TYPE = 2;
    /**
     * SceneObject constructor
     */
	SceneObject(){
		type = NO_TYPE;
		attachedNode = 0;
		nameSceneObject = "";
		idSceneObject = 0;
	}
    /**
     * SceneObject destructor
     */
	~SceneObject(){
		unatachNode();
	}
    
    /**
     * getType overloaded this one returns NO_TYPE
     * @return int
     */
	virtual int getType(){
		return type;
	}

    /**
     * attachTo sets the link between node and sceneObject
     * @param Node* node
     */
	void attachTo(Node* node){
		//to avoid duplicates
		if(attachedNode != 0 && attachedNode != node){
			unatachNode();
		}
		attachedNode = node;
	}
    
    /**
     * unatachNode removes the link between node and sceneObject
     */
	void unatachNode(){
		if(attachedNode){
			attachedNode->unatachSceneObjectWithoutConsistence(this);
		}
		attachedNode = 0;
	}
    
    /**
     * unatachNodeWithoutConsistence removes the link between node and sceneObject without calling to be removet at the other side
     */
	void unatachNodeWithoutConsistence(){
		attachedNode = 0;
	}
    
    /**
     * getAttachedNode
     * @return Node*
     */
	Node* getAttachedNode(){
		return attachedNode;
	}
    
    /**
     * getAttachedNode
     * @param int id
     */
	void setId(int id){
		idSceneObject = id;
	}
    
    /**
     * getId
     * @return int
     */
	int getId(){
		return idSceneObject;
	}
    
    /**
     * setName
     * @param std::string name
     */
	void setName(std::string name){
		nameSceneObject = name;
	}
    
    /**
     * getName
     * @return std::string
     */
	std::string getName(){
		return nameSceneObject;
	}
protected:
	int type;
	Node* attachedNode;
	std::string nameSceneObject;
	int idSceneObject;
};
#endif

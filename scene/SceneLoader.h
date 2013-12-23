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

#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H
#include "../libs/rapidXML/rapidxml.hpp"
#include "../log/Log.h"
#include "BasicSceneManager.h"
#include "Node.h"
#include "../sceneObjects/mesh/Mesh.h"
#include "../sceneObjects/mesh/MeshManager.h"
#include "../sceneObjects/camera/Camera.h"
#include "../sceneObjects/light/Light.h"
#include "../fileSystem/FileSystem.h"
#include "../utils/fast_atof.h"
#include "../utils/Maths.h"
#include "../globalData/GlobalData.h"

#include <string>

using namespace std;
using namespace rapidxml;
/**
 * SceneLoader are a set of static functions under a namespace to load from an xml file, this can be made better
 */
namespace SceneLoader {
	BasicSceneManager* currentSecene;
    /**
     * static extractQuat returns a quaternion from a string quaternion divided by " "
     * @value std::string value
     * @return glm::quat
     */
	static glm::quat extractQuat(string value){
		char* tempBuffer = new char [value.length()+1];
		std::strcpy(tempBuffer, value.c_str());
		float result[4];

		char* tempPointer = std::strtok(tempBuffer," ");//split string into tokens
		int i = 0;
		while(tempPointer!=0)
		{
			if(i > 3) logErr("SceneLoader::extractQuat this will end in a seg fault... because %s", value.c_str());
			result[i] = (float)fast_atof(tempPointer);
			i++;
			tempPointer = strtok(NULL," ");
		}

		delete[] tempBuffer;
		glm::quat orientationResult = glm::quat(result[0], result[1], result[2], result[3]);
		return orientationResult;
	}
    /**
     * static extractVec3 returns a vector 3 from a string vector 3 divided by " "
     * @value std::string value
     * @return glm::vec3
     */
	static glm::vec3 extractVec3(string value){
		char* tempBuffer = new char [value.length()+1];
		std::strcpy(tempBuffer, value.c_str());
		float result[3];

		char* tempPointer = std::strtok(tempBuffer," ");
		int i = 0;
		while(tempPointer!=0){
			if(i > 2) logErr("SceneLoader::extractVec3 this will end in a seg fault... because %s", value.c_str());
			result[i] = (float)fast_atof(tempPointer);
			i++;
			tempPointer = strtok(NULL," ");
		}

		delete[] tempBuffer;
		glm::vec3 vec3Result = glm::vec3(result[0], result[1], result[2]);
		return vec3Result;
	}

    /**
     * static extractVec3 returns a int with all option masks from a string
     * @value std::string value
     * @return int options
     */
	static int getMeshOptions(string value){
		char* tempBuffer = new char [value.length()+1];
		std::strcpy(tempBuffer, value.c_str());
		int result = 0;
		char* tempPointer = std::strtok(tempBuffer,"|");
		int i = 0;
		while(tempPointer!=0){
			if(strncmp(tempPointer,"BACK_CULLING_MASK",17) == 0){
				result = result | Mesh::BACK_CULLING_MASK;
			}
			else if(strncmp(tempPointer,"FRONT_CULLING_MASK",18) == 0){
				result = result | Mesh::FRONT_CULLING_MASK;
			}
			else if(strncmp(tempPointer,"NO_CULLING_MASK",15) == 0){
				result = result | Mesh::NO_CULLING_MASK;
			}
			else if(strncmp(tempPointer,"ALPHA_BLENDING_MASK",19) == 0){
				result = result | Mesh::ALPHA_BLENDING_MASK;
			}
			else if(strncmp(tempPointer,"CAST_SHADOWS_MASK",17) == 0){
				result = result | Mesh::CAST_SHADOWS_MASK;
			}
			else if(strncmp(tempPointer,"FAKE_GEOMETRY_CAST_SHADOWS_MASK",31) == 0){
				result = result | Mesh::FAKE_GEOMETRY_CAST_SHADOWS_MASK;
			}
			else if(strncmp(tempPointer,"RECEIVE_SHADOWS_MASK",20) == 0){
				result = result | Mesh::RECEIVE_SHADOWS_MASK;
			}
			else if(strncmp(tempPointer,"SELECTIONABLE_MASK",18) == 0){
				result = result | Mesh::SELECTIONABLE_MASK;
			}
			else if(strncmp(tempPointer,"GUI_MASK",8) == 0){
				result = result | Mesh::GUI_MASK;
			}
			else{
				logErr("SceneLoader::getMeshOptions mesh error, unknown render option %s", tempPointer);
			}
			tempPointer = strtok(NULL,"|");
		}

		delete[] tempBuffer;
		return result;
	}

    /**
     * static parseNode sets all node data to destSceneNode pointer
     * @value xml_node<>* nodeXML
     * @value Node* destSceneNode
     */
	static void parseNode(xml_node<> *nodeXML, Node* destSceneNode){
		string atributeName;
		string value;
		for (xml_attribute<> *attribute = nodeXML->first_attribute(); attribute; attribute = attribute->next_attribute()) {
			atributeName = attribute->name();
			value = attribute->value();
			if(atributeName.compare("name")==0){
				logInf("parse node name %s",value.c_str());
				destSceneNode->setName(value);
			}
			else if(atributeName.compare("id")==0){
				destSceneNode->setId(atoi(value.c_str()));
			}
			else if(atributeName.compare("position")==0){
				glm::vec3 position = extractVec3(value);
				destSceneNode->setPosition(position);
			}
			else if(atributeName.compare("scale")==0){
				glm::vec3 scale = extractVec3(value);
				logInf("scale value %s, result %f %f %f", value.c_str(), scale.x, scale.y, scale.z);
				destSceneNode->setScale(scale);
			}
			else if(atributeName.compare("orientation")==0){
				glm::quat orientation = extractQuat(value);
				destSceneNode->setOrientation(orientation);
			}
			else if(atributeName.compare("rotateX")==0){
				float radiants = (float)fast_atof(value.c_str()) * Maths::PI / 180.0f;
				logInf("rotateX value %s result in radiants %f node pointer value %i", value.c_str(), radiants, (int)destSceneNode);
				destSceneNode->rotate(glm::vec3(1.0f,0.0f,0.0f), radiants);
			}
			else if(atributeName.compare("rotateY")==0){
				float radiants = (float)fast_atof(value.c_str()) * Maths::PI / 180.0f;
				destSceneNode->rotate(glm::vec3(0.0f,1.0f,0.0f), radiants);
			}
			else if(atributeName.compare("rotateZ")==0){
				float radiants = (float)fast_atof(value.c_str()) * Maths::PI / 180.0f;
				destSceneNode->rotate(glm::vec3(0.0f,0.0f,1.0f), radiants);
			}
			else if(atributeName.compare("lookAt")==0){
				glm::vec3 destinationLookAt = extractVec3(value);
				logInf("scene node name %s, look at %f, %f, %f", destSceneNode->getName().c_str(), destinationLookAt.x, destinationLookAt.y, destinationLookAt.z);
				destSceneNode->lookAt(destinationLookAt);
				glm::vec3 forward = destSceneNode->getForwardVector();
				logErr("and forward vector %f %f %f", forward.x, forward.y,forward.z);
			}
			else if(atributeName.compare("showAxis")==0){
				 if(value.compare("true")==0){
					 currentSecene->setNodeToShowAxis(destSceneNode);
				 }
			}
			else {
				logErr("SceneLoader::parseNode node error, unknown type %s", atributeName.c_str());
			}
		}
	}

    /**
     * static parseMesh sets all node data to a Mesh and set it to sceneNode pointer
     * @value xml_node<>* nodeXML
     * @value Node* sceneNode
     */
    static void parseMesh(xml_node<> *nodeXML, Node* sceneNode){
		Mesh* aux = 0;
		string atributeName;
		string value;
		for (xml_attribute<> *attribute = nodeXML->first_attribute(); attribute; attribute = attribute->next_attribute()) {
			atributeName = attribute->name();
			value = attribute->value();
			logInf("mesh attribute %s value %s", atributeName.c_str(), value.c_str());
			if(atributeName.compare("name")==0){
				logInf("parse mesh name %s sceneNode pointer value %i",value.c_str(), (int)sceneNode);
				aux = currentSecene->createMesh(value);
				aux->name = value;
				sceneNode->attachSceneObject(aux);
			}
			else if(atributeName.compare("shader")==0){
				if(aux == 0) continue;
				logErr("parseMesh atribute name shader, until there is a shader factory this will not work");
				//aux->shader = ShaderFactory::getInstance()->getShader(value);
			}
			else if(atributeName.compare("enabled")==0){
				if(aux == 0) continue;
				aux->renderEnabled = (value.compare("true")==0);
			}
			else if(atributeName.compare("options")==0){
				if(aux == 0) continue;
				int options = getMeshOptions(value);
				aux->options = options;
			}
			else {
				logErr("SceneLoader::parseMesh mesh error, unknown type %s", atributeName.c_str());
			}
		}
	}

    /**
     * static parseCamera sets all node data to a Camera and set it to sceneNode pointer
     * @value xml_node<>* nodeXML
     * @value Node* sceneNode
     */
	static void parseCamera(xml_node<> *nodeXML, Node* sceneNode){
		Camera* aux = currentSecene->createCamera();
		aux->setAspectRatio(1.0f*GlobalData::getInstance()->screenWidth/GlobalData::getInstance()->screenHeight);
		sceneNode->attachSceneObject(aux);
		logInf("new camera, scene node name %s", sceneNode->getName().c_str());

		string atributeName;
		string value;
		for (xml_attribute<> *attribute = nodeXML->first_attribute(); attribute; attribute = attribute->next_attribute()) {
			atributeName = attribute->name();
			value = attribute->value();
			if(atributeName.compare("name")==0){
				aux->setName(value);
			}
			else if(atributeName.compare("id")==0){
				aux->setId(atoi(value.c_str()));
			}
			else if(atributeName.compare("enabled")==0){
				aux->setEnabled(value.compare("true")==0);
			}
			else if(atributeName.compare("type")==0){
				if(value.compare("PERSPECTIVE")==0){
					aux->setCameraType(Camera::PERSPECTIVE);
				}else{
					aux->setCameraType(Camera::ORTHOGONAL);
				}
			}
			else if(atributeName.compare("aspectRatio")==0){
				aux->setAspectRatio((float)fast_atof(value.c_str()));
			}
			else if(atributeName.compare("aperture")==0){
				aux->setAperture((float)fast_atof(value.c_str()));
			}
			else if(atributeName.compare("near")==0){
				aux->setNearClipDistance((float)fast_atof(value.c_str()));
			}
			else if(atributeName.compare("far")==0){
				aux->setFarClipDistance((float)fast_atof(value.c_str()));
			}
			else if(atributeName.compare("leftOrtho")==0){
				aux->setLeftOrtho((float)fast_atof(value.c_str()));
			}
			else if(atributeName.compare("rightOrtho")==0){
				aux->setRightOrtho((float)fast_atof(value.c_str()));
			}
			else if(atributeName.compare("bottomOrtho")==0){
				aux->setBottomOrtho((float)fast_atof(value.c_str()));
			}
			else if(atributeName.compare("topOrtho")==0){
				aux->setTopOrtho((float)fast_atof(value.c_str()));
			}
			else if(atributeName.compare("nearOrtho")==0){
				aux->setZNearOrtho((float)fast_atof(value.c_str()));
			}
			else if(atributeName.compare("farOrtho")==0){
				aux->setZFarOrtho((float)fast_atof(value.c_str()));
			}
			else {
				logErr("SceneLoader::parseCamera camera error, unknown type %s", atributeName.c_str());
			}
		}
	}

    /**
     * static recursive sets all node data to parent pointer
     * @value xml_node<>* nodeXML
     * @value Node* parent
     */
	static void recursive(xml_node<> *nodeXML, Node* parent){
		string nodeType = nodeXML->name();
		Node* newChild = 0;

		if(nodeType.compare("Node")==0){
			newChild = parent->createChild();
			parseNode(nodeXML, newChild);
			if (nodeXML->first_node() != 0) {
				for (xml_node<> *child = nodeXML->first_node(); child; child = child->next_sibling()) {
					recursive(child, newChild);
				}
			}
		}
		else if(nodeType.compare("Mesh")==0){
			parseMesh(nodeXML, parent);
		}
		else if(nodeType.compare("Camera")==0){
			parseCamera(nodeXML, parent);
		}
		else if(nodeType.compare("Light")==0){
			logWar("light type TODO");
		}
		else if(nodeType.compare("Scene")==0){
			if (nodeXML->first_node() != 0) {
				for (xml_node<> *child = nodeXML->first_node(); child; child = child->next_sibling()) {
					recursive(child, parent);
				}
			}
		}
		else {
			logErr("SceneLoader::recursive parsing scene file error, unknown type %s", nodeType.c_str());
		}
	}

    /**
     * static loadScene sets all node data to parent pointer
     * @value BasicSceneManager* sceneReceiver
     * @value std::string file path to
     * @return bool false if can't open the file, true if is successfully loaded
     */
	static bool loadScene(BasicSceneManager* sceneReceiver, string file){
		if(FileSystem::getInstance()->openFile(file) == -1){
			logErr("SceneLoader::loadScene FileSystem can't open the file %s", file.c_str());
            return false;
        }
		currentSecene = sceneReceiver;
		xml_document<> doc;
		doc.parse<0>(FileSystem::getInstance()->getFileData(file));
		recursive(doc.first_node(), sceneReceiver->getRootNode());
		FileSystem::getInstance()->destroyFileData(file);
        return true;
	}
};

#endif

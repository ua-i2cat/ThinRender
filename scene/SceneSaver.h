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

#ifndef SCENE_SAVER_H
#define SCENE_SAVER_H
#include "../libs/rapidXML/rapidxml.hpp"
#include "../libs/rapidXML/rapidxml_print.hpp"
#include "../log/Log.h"
#include "BasicSceneManager.h"
#include "Node.h"
#include "../sceneObjects/SceneObject.h"
#include "../sceneObjects/mesh/Mesh.h"
#include "../sceneObjects/mesh/MeshManager.h"
#include "../sceneObjects/camera/Camera.h"
#include "../sceneObjects/light/Light.h"
#include "../fileSystem/FileSystem.h"
#include "../utils/fast_atof.h"
#include "../utils/Maths.h"
#include "../fileSystem/FileSystem.h"

#include <string>
#include <sstream>

using namespace std;
using namespace rapidxml;
/**
 * SceneSaver are a set of static functions under a namespace to save to an xml file, this can be made better
 */
namespace SceneSaver {
	BasicSceneManager* currentSecene;
	static xml_document<> doc;

    /**
     * static serializeCamera
     * @value Camera* camera
     * @value  xml_node<>* parentXMLNode
     */
	static void serializeCamera(Camera* camera, xml_node<>* parentXMLNode){
		logInf("serialize camera");
		xml_node<>* child;
		std::ostringstream oss;

		char* attributeValue;

		child = doc.allocate_node(node_element, "Camera");

		attributeValue = doc.allocate_string(camera->getName().c_str());
		child->append_attribute(doc.allocate_attribute("name", attributeValue));

		oss << camera->getId();
		attributeValue = doc.allocate_string(oss.str().c_str());
		child->append_attribute(doc.allocate_attribute("id", attributeValue));
		oss.str("");
		oss.clear();

		child->append_attribute(doc.allocate_attribute("type", (camera->getCameraType()==Camera::PERSPECTIVE)?"PERSPECTIVE":"ORTHOGONAL"));

		child->append_attribute(doc.allocate_attribute("enabled", (camera->getEnabled())?"true":"false"));
		logInf("end serialize camera");
		parentXMLNode->append_node(child);
	}

    /**
     * static serializeMesh
     * @value Mesh* mesh
     * @value  xml_node<>* parentXMLNode
     */
	static void serializeMesh(Mesh* mesh, xml_node<>* parentXMLNode){
		xml_node<>* child;
		std::ostringstream oss;

		char* attributeValue;

		attributeValue = doc.allocate_string(mesh->name.c_str());
		child = doc.allocate_node(node_element, "Mesh");
		child->append_attribute(doc.allocate_attribute("name", attributeValue));

		logErr("SceneSaver::serializeMesh setting shader TODO");
		logErr("SceneSaver::serializeMesh setting textures TODO");
		child->append_attribute(doc.allocate_attribute("enabled", mesh->renderEnabled?"true":"false"));


		oss << ((mesh->options&Mesh::BACK_CULLING_MASK)?"BACK_CULLING_MASK|":"")
			<< ((mesh->options&Mesh::FRONT_CULLING_MASK)?"FRONT_CULLING_MASK|":"")
			<< ((mesh->options&Mesh::NO_CULLING_MASK)?"NO_CULLING_MASK|":"")
			<< ((mesh->options&Mesh::ALPHA_BLENDING_MASK)?"ALPHA_BLENDING_MASK|":"")
			<< ((mesh->options&Mesh::CAST_SHADOWS_MASK)?"CAST_SHADOWS_MASK|":"")
			<< ((mesh->options&Mesh::FAKE_GEOMETRY_CAST_SHADOWS_MASK)?"FAKE_GEOMETRY_CAST_SHADOWS_MASK|":"")
			<< ((mesh->options&Mesh::RECEIVE_SHADOWS_MASK)?"RECEIVE_SHADOWS_MASK|":"")
			<< ((mesh->options&Mesh::SELECTIONABLE_MASK)?"SELECTIONABLE_MASK|":"")
			<< ((mesh->options&Mesh::GUI_MASK)?"GUI_MASK":"");
		attributeValue = doc.allocate_string(oss.str().c_str());
		child->append_attribute(doc.allocate_attribute("options", attributeValue));
		oss.str("");
		oss.clear();

		parentXMLNode->append_node(child);
	}
    /**
     * static serializeNode
     * @value Node* parent
     * @value  xml_node<>* parentXMLNode
     */
	static void serializeNode(Node* parent, xml_node<>* parentXMLNode){
		SceneObject* auxiliarSceneObject;
		for(int i = 0; i < parent->sceneObjects.size(); i++){
			auxiliarSceneObject = parent->sceneObjects.at(i);
			switch(auxiliarSceneObject->getType()){
			case SceneObject::CAMERA_TYPE:
				logInf("serialize camera");
				serializeCamera((Camera*)auxiliarSceneObject, parentXMLNode);
				break;
			case SceneObject::MESH_TYPE:
				serializeMesh((Mesh*)auxiliarSceneObject, parentXMLNode);
				break;
			case SceneObject::LIGHT_TYPE:
				logErr("SceneSaver::serializeNode serialize light TODO");
				break;
			default:
				logErr("SceneSaver::serializeNode at node %s unknown scene object type %i", parent->getName().c_str(), auxiliarSceneObject->getType());
				break;
			}
		}
		Node* auxiliarChildNode;
		xml_node<>* child;
		std::ostringstream oss;
		glm::vec3 auxV3;
		glm::quat auxQuat;

		char* attributeValue;
		for(int i = 0; i < parent->childs.size(); i++){
			auxiliarChildNode = parent->childs.at(i);
			//if(auxiliarChildNode->childs.size() == 0 && auxiliarChildNode->sceneObjects.size() == 0) continue;
			attributeValue = doc.allocate_string(auxiliarChildNode->getName().c_str());
			child = doc.allocate_node(node_element, "Node");
			child->append_attribute(doc.allocate_attribute("name", attributeValue));

			oss << auxiliarChildNode->getId();
			attributeValue = doc.allocate_string(oss.str().c_str());
			child->append_attribute(doc.allocate_attribute("id", attributeValue));
			oss.str("");
			oss.clear();

			auxV3 = auxiliarChildNode->getPosition();
			oss << auxV3.x << " " << auxV3.y << " " << auxV3.z;
			attributeValue = doc.allocate_string(oss.str().c_str());
			child->append_attribute(doc.allocate_attribute("position", attributeValue));
			oss.str("");
			oss.clear();

			auxV3 = auxiliarChildNode->getScale();
			oss << auxV3.x << " " << auxV3.y << " " << auxV3.z;
			attributeValue = doc.allocate_string(oss.str().c_str());
			child->append_attribute(doc.allocate_attribute("scale", attributeValue));
			oss.str("");
			oss.clear();

			auxQuat = auxiliarChildNode->getOrientation();
			oss << auxQuat.w << " " << auxQuat.x << " " << auxQuat.y << " " << auxQuat.z;
			attributeValue = doc.allocate_string(oss.str().c_str());
			child->append_attribute(doc.allocate_attribute("orientation", attributeValue));
			oss.str("");
			oss.clear();

			parentXMLNode->append_node(child);
			serializeNode(auxiliarChildNode, child);
		}
	}
#include <time.h>
    /**
     * static serializeScene
     * @value BasicSceneManager* sceneReceiver
     * @return always true...
     */
	static bool serializeScene(BasicSceneManager* sceneReceiver){
		currentSecene = sceneReceiver;

		// root node
		xml_node<>* root = doc.allocate_node(node_element, "Scene");
		root->append_attribute(doc.allocate_attribute("ambientLight", "0.2 0.2 0.2"));
		doc.append_node(root);

		serializeNode(sceneReceiver->getRootNode(), root);

		std::string xml_as_string;
		// watch for name collisions here, print() is a very common function name!
		print(std::back_inserter(xml_as_string), doc);
		// xml_as_string now contains the XML in string form, indented
		// (in all its angle bracket glory)

		time_t     now = time(0);
		struct tm  tstruct;
		char       buf[80];
		tstruct = *localtime(&now);
		// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime for more information about date/time format
		strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
		std::string date = buf;
		std::string name = "scene"+date+".xml";
		logInf("saving scene in file %s", name.c_str());

		FileSystem::getInstance()->writeFile(name, xml_as_string.c_str());
		//std::string xml_no_indent;
		// print_no_indenting is the only flag that print() knows about
		//print(std::back_inserter(xml_no_indent), doc, print_no_indenting);
		// xml_no_indent now contains non-indented XML
		doc.clear();
        return true;
	}
	static bool serializeScene(BasicSceneManager* sceneReceiver, std::string fileName){
		currentSecene = sceneReceiver;

		// root node
		xml_node<>* root = doc.allocate_node(node_element, "Scene");
		root->append_attribute(doc.allocate_attribute("ambientLight", "0.2 0.2 0.2"));
		doc.append_node(root);

		logInf("sceneReceiver->getRootNode() childs size %i", sceneReceiver->getRootNode()->childs.size());
		serializeNode(sceneReceiver->getRootNode(), root);

		std::string xml_as_string = "";
		// watch for name collisions here, print() is a very common function name!
		print(std::back_inserter(xml_as_string), doc);

		std::string name = "scene"+fileName+".xml";
		logInf("saving scene in file %s", name.c_str());
		logInf("xml_as_string length %i", xml_as_string.length());
		FileSystem::getInstance()->writeFile(name, xml_as_string.c_str());
		doc.clear();

		return true;
	}
};

#endif

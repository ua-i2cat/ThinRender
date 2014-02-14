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

#include "BasicSceneManager.h"
#include "SceneLoader.h"
#include "SceneSaver.h"
#include "../globalData/GlobalData.h"
#include "../GUI/GUIUtils/SliderBlockGUI.h"

BasicSceneManager::BasicSceneManager(){
	root = new Node();
	ambientLight = glm::vec3(0.2f, 0.2f, 0.2f);
	loadScene();
	initDebug();
	guiBackground = 0;
	shaderAxis = 0;
	//initAxisVectorMesh();
	depthRTT = new DepthRTT();
	depthRTT->initFrameBuffer(1024,1024);//ContextControllerEGL::getInstance()->width, ContextControllerEGL::getInstance()->height);
	pickRTT = new PickRTT();
	pickRTT->initFrameBuffer(8,8);
	colorBufferClean = glm::vec3(1.0f,1.0f,1.0f);
}

BasicSceneManager::~BasicSceneManager(){
    delete root;
    delete depthRTT;
    delete pickRTT;
	closeScene();
}

void BasicSceneManager::loadScene(){

}

void BasicSceneManager::loadScene(std::string fileName){
	SceneLoader::loadScene(this, fileName);
}
void BasicSceneManager::saveScene(std::string fileName){
	if(fileName.compare("") == 0)
		SceneSaver::serializeScene(this);
	else
		SceneSaver::serializeScene(this, fileName);
}

void BasicSceneManager::initAxisVectorMesh(){
	shaderAxis = new AxisShader();
	rightVectorMesh = MeshManager::getInstance()->getMesh("right.obj");
	upVectorMesh = MeshManager::getInstance()->getMesh("up.obj");
	forwardVectorMesh = MeshManager::getInstance()->getMesh("forward.obj");
	nodeToPrintAxis = 0;
}

void BasicSceneManager::initDebug(){
	float w = GlobalData::getInstance()->screenWidth;
	float h = GlobalData::getInstance()->screenHeight;
	float ratio = (float)w/(float)h;
	debugPlane = MeshManager::getInstance()->getMeshPlane2D(0.65f, 0.5f, 0.3f, 0.3f*ratio);
	debugPlane->name = "debug plane";
	logErr("initDebug done!");
}

void BasicSceneManager::closeScene(){
	int i;
	for(i = 0; i < root->childs.size(); i++){
		delete root->childs[i];
		root->childs[i] = 0;
	}
	root->childs.clear();
	for(i = 0; i < meshVector.size(); i++){
		meshVector[i] = 0;
	}
	for(i = 0; i < guiRect.size(); i++){
		delete guiRect[i];
		guiRect[i] = 0;
	}

	for(i = 0; i < guiSlider.size(); i++){
		delete guiSlider[i];
		guiSlider[i] = 0;
	}
	//Removes all elements from the vector (which are destroyed), leaving the container with a size of 0.
	meshVector.clear();
	cameraVector.clear();
	lightVector.clear();
	guiRect.clear();
	guiSlider.clear();
    if(guiBackground != 0)
        delete guiBackground;
    guiBackground = 0;
}
/*
void BasicSceneManager::cleanGUI(){
    for(int i = 0; i < guiRect.size(); i++){
		delete guiRect[i];
		guiRect[i] = 0;
	}
    guiRect.clear();
}
void BasicSceneManager::cleanBackground(){
    if(guiBackground != 0)
        delete guiBackground;
    guiBackground = 0;
}
*/
Camera* BasicSceneManager::createCamera(){
	Camera* result = new Camera();
	cameraVector.push_back(result);
	return result;
}

Mesh* BasicSceneManager::createMesh(string name){
	Mesh* result = MeshManager::getInstance()->getMesh(name);
	meshVector.push_back(result);
	return result;
}

Mesh* BasicSceneManager::createPlaneMesh(){
	Mesh* result = MeshManager::getInstance()->getMeshPlane2D();
	result->name = "generatedPlaneMesh";
	meshVector.push_back(result);
	return result;
}

RectGUI* BasicSceneManager::createRectangleBackground(){
	if(guiBackground == 0){
		Node* node = root->createChild();
		guiBackground = new RectGUI(node, 0.0f,GlobalData::getInstance()->screenHeight,GlobalData::getInstance()->screenWidth, GlobalData::getInstance()->screenHeight);
	}
	return guiBackground;
}
RectGUI* BasicSceneManager::createRectangleGUI(){
	Node* node = root->createChild();
	RectGUI* rectResult = new RectGUI(node, 0.0f,1.0f,1.0f, 1.0f);
	guiRect.push_back(rectResult);
	return rectResult;
}

SliderGUI* BasicSceneManager::createSliderGUI(float left, float top, float w, float h, int type){
	SliderGUI* sliderResult = new SliderGUI(left, top, w, h, type);
	guiSlider.push_back(sliderResult);
	return sliderResult;
}

SliderGUI* BasicSceneManager::createSliderBlockGUI(float left, float top, float w, float h, int type){
	SliderGUI* sliderResult = new SliderBlockGUI(left, top, w, h, type);
	guiSlider.push_back(sliderResult);
	return sliderResult;
}

RectGUI* BasicSceneManager::createRectangleGUI(float left, float top, float w, float h){
	Node* node = root->createChild();
	RectGUI* rectResult = new RectGUI(node, left, top, w, h);
	guiRect.push_back(rectResult);
	return rectResult;
}

Node* BasicSceneManager::getRootNode(){
	return root;
}

Light* BasicSceneManager::createLight(){
	Light* result = new Light();
	lightVector.push_back(result);
	return result;
}

void BasicSceneManager::setAmbientLight(glm::vec3 ambient){
	ambientLight = ambient;
}

void BasicSceneManager::sortMeshes(){
	Node* auxNode;
	//first we sort all the renderable items
	for(int i = 0; i < meshVector.size(); i++){
		auxNode = meshVector[i]->getAttachedNode();
		if(!auxNode) continue;
		//PORCADA....
		meshVector[i]->calculateViewPosition(view);
		if(meshVector[i]->options & Mesh::FAKE_GEOMETRY_CAST_SHADOWS_MASK) castShadow.push(meshVector[i]);
		if(!meshVector[i]->renderEnabled) continue;

		if(meshVector[i]->options & Mesh::ALPHA_BLENDING_MASK){
			withAlpha.push(meshVector[i]);
		}else{
			withoutAlpha.push(meshVector[i]);
		}

		if(!meshVector[i]->options & Mesh::CAST_SHADOWS_MASK)continue;
		castShadow.push(meshVector[i]);
	}
}

void BasicSceneManager::createShadowMap(){
	//if there are shadows we need to render it properly, with simple shaders
	shadows = false;

    Camera* cameraAux = getFirstCameraNotEnabled();// getFirstCameraEnabled();

	if(castShadow.size() != 0 && cameraAux != 0/* && currentLight != 0*/){
		projectionLight = cameraAux->getProjectionMatrix();//getOrthogonalProjectionMatrix();

		//Node* cameraNodeAux = cameraAux->getAttachedNode();
		//trick of the trade
        //lightWorldPosition = cameraNodeAux->getHierarchyPosition();
		//lightWorldDirection = cameraNodeAux->getWorldForwardVector();

		viewLight = cameraAux->getViewMatrix();

		//glm::vec4 result = projectionLight * viewLight * glm::vec4(0.0f,0.0f,0.0f,1.0f);
		//logInf("result vector from projection*view* 0 0 0 = {%f,%f,%f}", result.x, result.y, result.z);
		shadows = true;
		depthRTT->renderTextureFrom(projectionLight, viewLight, meshVector);
	}
}

int BasicSceneManager::getIdObjectSelected(int xClick, int yClick){
	pickRTT->xSelectedScreen = xClick;
	pickRTT->ySelectedScreen = yClick;
	Camera* cameraAux = getFirstCameraEnabled();
	pickRTT->renderTextureFrom(cameraAux->getProjectionMatrix(), cameraAux->getViewMatrix(), meshVector);
	//logInf("BasicSceneManager::getIdObjectSelected pickRTT result %i", pickRTT->previousPickId);
	return pickRTT->previousPickId;
}

void BasicSceneManager::setColorClean(glm::vec3 color){
	colorBufferClean = color;
}
void BasicSceneManager::cleanBuffers(){
	glClearColor(colorBufferClean.x, colorBufferClean.y, colorBufferClean.z, 1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}

void BasicSceneManager::updateVariables(){
	currentCamera = getFirstCameraEnabled();
	if(currentCamera == 0) return;
	projection = currentCamera->getProjectionMatrix();

	Node* auxNode = currentCamera->getAttachedNode();
	if(auxNode == 0) return;

	view = currentCamera->getViewMatrix();

	projView = projection * view;

	light_diffuse = glm::vec4(0.0f);
	light_specular = glm::vec4(0.0f);
	light_position = glm::vec4(0.0f);
	light_direction = glm::vec4(0.0f);
	currentLight = getFirstLightEnabled();
	if(currentLight != 0){
		auxNode = currentLight->getAttachedNode();
		if(auxNode == 0) return;
        lightWorldPosition = auxNode->getHierarchyPosition();
		lightWorldDirection = auxNode->getWorldForwardVector();
		light_diffuse = currentLight->getDiffuse();
		light_specular = currentLight->getSpecular();
		glm::mat4 lightMatrix = view * auxNode->getFullTransform();
		light_position = lightMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		light_direction = lightMatrix * glm::vec4(auxNode->getWorldForwardVector(),1.0f);
		light_direction = glm::normalize(light_direction);
		//logInf("light direcion: {%f, %f, %f} ", light_direction.x, light_direction.y, light_direction.z);//, position.x, position.y, position.z);
	}

	sortMeshes();
}
void BasicSceneManager::render(){
	if(currentCamera == 0) return;
	int error = glGetError();
	if(error != GL_NO_ERROR)logErr("BasicSceneManager::render ERRORRRR a %i", error);
	GLuint depthTexture = depthRTT->getTeture();
	if(!shadows) depthTexture = 0;
	Mesh* aux;
	while (!withoutAlpha.empty()){
		aux = withoutAlpha.top();
		withoutAlpha.pop();
		//logInf("lightWorldPosition: {%f, %f, %f} lightWorldDirection: {%f, %f, %f}", lightWorldPosition.x, lightWorldPosition.y, lightWorldPosition.z, lightWorldDirection.x, lightWorldDirection.y, lightWorldDirection.z);
		aux->render(projView, projection, view, currentLight, depthTexture, glm::vec4(lightWorldPosition,0.0f), glm::vec4(lightWorldDirection,0.0f), ambientLight, projectionLight, viewLight);
	}
	if(withAlpha.empty()) return;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (!withAlpha.empty()){
		aux = withAlpha.top();
		withAlpha.pop();
		aux->render(projView, projection, view, currentLight, depthTexture, glm::vec4(lightWorldPosition,0.0f), glm::vec4(lightWorldDirection,0.0f), ambientLight, projectionLight, viewLight);
	}
	glDisable(GL_BLEND);
/*	for(int i = 0; i < meshVector.size(); i++){
		if(!meshVector[i]->renderEnabled) continue;
		if(!meshVector[i]->getAttachedNode()) continue;
		//aqui posaria els models per ordenar segons la z, fer el culling i tal pero per ara em molaria que funcioni aixo
		meshVector[i]->render(projView, view, light_diffuse, light_specular, light_position, light_direction, ambientLight);
	}*/
/*	if(depthTexture){
		debugPlane->render(projView, projection, view, currentLight, depthTexture, glm::vec4(lightWorldPosition,0.0f), glm::vec4(lightWorldDirection,0.0f), ambientLight, projViewLight, viewLight);
	}*/
	/*if(nodeToPrintAxis){
		nodeToPrintAxis->attachSceneObject(rightVectorMesh);
		nodeToPrintAxis->attachSceneObject(upVectorMesh);
		nodeToPrintAxis->attachSceneObject(forwardVectorMesh);
		glUseProgram(shaderAxis->program);

		glUniform3f(shaderAxis->uniformColor, 1.0f, 0.0f, 0.0f);
		rightVectorMesh->renderWithoutShader(projection, view, shaderAxis);
		glUniform3f(shaderAxis->uniformColor, 0.0f, 1.0f, 0.0f);
		upVectorMesh->renderWithoutShader(projection, view, shaderAxis);
		glUniform3f(shaderAxis->uniformColor, 0.0f, 0.0f, 1.0f);
		forwardVectorMesh->renderWithoutShader(projection, view, shaderAxis);

		nodeToPrintAxis->unatachSceneObject(rightVectorMesh);
		nodeToPrintAxis->unatachSceneObject(upVectorMesh);
		nodeToPrintAxis->unatachSceneObject(forwardVectorMesh);
	}*/
}

void BasicSceneManager::setNodeToShowAxis(Node* nodeWithAxis){
	nodeToPrintAxis = nodeWithAxis;
}

void BasicSceneManager::renderGUI(){
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < guiRect.size(); i++){
		guiRect.at(i)->draw();
	}
	for (int i = 0; i < guiSlider.size(); i++){
		guiSlider.at(i)->draw();
	}
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}
void BasicSceneManager::renderBackground(){
	if(guiBackground == 0) return;
	guiBackground->draw();
}
Camera* BasicSceneManager::getFirstCameraEnabled(){
	int i;
	for(i = 0; i < cameraVector.size(); i++){
		if(cameraVector[i]->getEnabled()) return cameraVector[i];
	}
	return 0;
}
Camera* BasicSceneManager::getFirstCameraNotEnabled(){
	int i;
	for(i = 0; i < cameraVector.size(); i++){
		if(!cameraVector[i]->getEnabled()) return cameraVector[i];
	}
	return 0;
}
Light* BasicSceneManager::getFirstLightEnabled(){
	int i;
	for(i = 0; i < lightVector.size(); i++){
		return lightVector[i];
	}
	return 0;
}

RectGUI* BasicSceneManager::getRectTouch(float x, float y){
	RectGUI* result = 0;
	for(int i = 0; i < guiRect.size(); i++){
		result = guiRect.at(i);
		if(result->isInside(x, y))
			return result;
	}
	return 0;
}

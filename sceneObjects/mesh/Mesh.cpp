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

#include "Mesh.h"
#include "../../utils/Timer.h"
#include "../../controllerEGL/ContextControllerEGL.h"
#include "../../shader/lightning/DirectionalLightNormalMapShader.h"
#include "../../shader/lightning/DirectionalLightNormalMapShadowMapShader.h"
#include "../../shader/lightning/EdgelShader.h"

Mesh::Mesh(string name, unsigned int id){
	this->name = name;
	std::size_t foundMD5 = name.find(".md5");
	if (foundMD5!=std::string::npos){
		shader = new DirectionalLightNormalMapShadowMapShader();//EdgelShader();//DirectionalLightNormalMapShader();//
		//shader->texture = TextureManager::getInstance()->getTexture("hellknight2.png");
		//((DirectionalLightNormalMapShader*)shader)->normalMapTexture = TextureManager::getInstance()->getTexture("hellknight_local.png");
	}else{
		shader = new DirectionalLightShadowMapShader();//
	}
	renderEnabled = true;
	this->id = id;
	options = BACK_CULLING_MASK | CAST_SHADOWS_MASK | RECEIVE_SHADOWS_MASK | SELECTIONABLE_MASK;
}

Mesh::Mesh(Mesh* createMeshFromOther, unsigned int id){
	name = createMeshFromOther->name;
	std::size_t foundMD5 = name.find(".md5");
	if (foundMD5!=std::string::npos){
		shader = new EdgelShader();//DirectionalLightNormalMapShadowMapShader();//DirectionalLightNormalMapShader();
		//shader->texture = TextureManager::getInstance()->getTexture("hellknight2.png");
		//((DirectionalLightNormalMapShader*)shader)->normalMapTexture = TextureManager::getInstance()->getTexture("hellknight_local.png");
	}else{
		shader = new DirectionalLightShadowMapShader();//
	}
	subMeshes = createMeshFromOther->subMeshes;
	renderEnabled = true;
	this->id = id;
	options = createMeshFromOther->options;
}

Mesh::Mesh(Shader* shader, unsigned int id){
	this->name = name;
	renderEnabled = true;
	this->id = id;
	options = BACK_CULLING_MASK | CAST_SHADOWS_MASK | RECEIVE_SHADOWS_MASK | SELECTIONABLE_MASK;

	this->shader = shader;
	this->id = id;
}

void Mesh::setShader(Shader* newShader){
    if(newShader == 0) return;
    delete shader;
    shader = newShader;
}

Mesh::~Mesh(){
    delete shader;
    for(int i = 0; i < subMeshes.size(); i++){
        delete subMeshes.at(i);
    }
	subMeshes.clear();
}

void Mesh::generateVBO(){
	for(int i = 0; i < subMeshes.size(); i++){
		subMeshes.at(i)->generateVBO();
	}
	calculateBoundingBox();
}

void Mesh::renderWithoutShader(glm::mat4 proj, glm::mat4 view, Shader* shader){
	glm::mat4 model = attachedNode->getFullTransform();
	//glm::mat4 mvp = projView * model;

	//glUniformMatrix4fv(shader->uniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));

	shader->setVars(glm::mat4(), model, glm::mat4(), glm::mat4(), 0, glm::vec4(), glm::vec4(), glm::vec3(), proj, view, 0, false);

	//if(this->shader->texture){
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, this->shader->texture->getTextureId());
	//	glUniform1i(shader->attributeTexture, /*GL_TEXTURE*/0);
	//}

	for(int i = 0; i < subMeshes.size(); i++){
		subMeshes.at(i)->render(shader);
	}
}

void Mesh::render(glm::mat4 projView, glm::mat4 projection, glm::mat4 view, Light* renderLight, GLuint depthTexture, glm::vec4 light_position, glm::vec4 light_direction, glm::vec3 light_ambient, glm::mat4 projViewLight, glm::mat4 viewLight){
	int error = glGetError();
	if(error != GL_NO_ERROR)logErr("Mesh::render ERRORRRR a %i name %s", error, name.c_str());
/*	if(options & SELECTIONABLE_MASK){

		glStencilFunc(GL_ALWAYS, id, 0xff);
	}else{
		glDisable(GL_STENCIL_TEST);
	}
*/
	glm::mat4 model;
	//glm::mat4 mvp;
	if(attachedNode){
		model = attachedNode->getFullTransform();
	//	mvp = projView * model;
	}

	shader->useProgram();
	shader->setVars(projView, model, view, projection, renderLight, light_position, light_direction, light_ambient, projViewLight, viewLight, depthTexture, 0!=(options & RECEIVE_SHADOWS_MASK));

	for(int i = 0; i < subMeshes.size(); i++){
		if(subMeshes.at(i)->textureSubMesh != 0){
			shader->loadTexture(subMeshes.at(i)->textureSubMesh);
		}
		subMeshes.at(i)->render(shader);
		error = glGetError();
		if(error != GL_NO_ERROR)logErr("MESH RENDER ERRORRRR END %i name %s", error, name.c_str());
	}
}

void Mesh::calculateBoundingBox(){
	AABBmin = glm::vec3(0.0f);
	AABBmax = glm::vec3(0.0f);
	for(int j = 0; j < subMeshes.size(); j++){
		for (int i = 0; i < subMeshes.at(j)->vertices.size(); i++) {
			if (subMeshes.at(j)->vertices[i].x < AABBmin.x) AABBmin.x = subMeshes.at(j)->vertices[i].x;
			if (subMeshes.at(j)->vertices[i].x > AABBmax.x) AABBmax.x = subMeshes.at(j)->vertices[i].x;
			if (subMeshes.at(j)->vertices[i].y < AABBmin.y) AABBmin.y = subMeshes.at(j)->vertices[i].y;
			if (subMeshes.at(j)->vertices[i].y > AABBmax.y) AABBmax.y = subMeshes.at(j)->vertices[i].y;
			if (subMeshes.at(j)->vertices[i].z < AABBmin.z) AABBmin.z = subMeshes.at(j)->vertices[i].z;
			if (subMeshes.at(j)->vertices[i].z > AABBmax.z) AABBmax.z = subMeshes.at(j)->vertices[i].z;
		}
	}
}


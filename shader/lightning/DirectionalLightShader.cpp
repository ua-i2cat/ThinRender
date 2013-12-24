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

#include "DirectionalLightShader.h"

DirectionalLightShader::DirectionalLightShader(){
	texture = 0;
	hasNormals = true;
	hasUV = true;
	hasTangents = false;
	hasBitangents = false;
	loadShader("DirectionalLightShader");
}

void DirectionalLightShader::loadVars(){
	const char* attributeName = "Vertex";
	attributeCoord = glGetAttribLocation(program, attributeName);
	if (attributeCoord == -1) {
		logErr("Could not bind attribute %s\n", attributeName);
		return;
	}
	attributeName = "Normal";
	attributeNormal = glGetAttribLocation(program, attributeName);
	if (attributeNormal == -1) {
		logErr("Could not bind attribute %s\n", attributeName);
		return;
	}
	attributeName = "Texcoord";
	attributeTexture = glGetAttribLocation(program, attributeName);
	if (attributeTexture == -1) {
		logErr("Could not bind attribute %s\n", attributeName);
		return;
	}

	const char* uniformName;
	uniformName = "ProjectionMatrix";
	uniformProjectionMatrix = glGetUniformLocation(program, uniformName);
	if (uniformProjectionMatrix == -1) {
		logErr("Could not bind uniform %s\n", uniformName);
		return;
	}
	uniformName = "ViewMatrix";
	uniformViewMatrix = glGetUniformLocation(program, uniformName);
	if (uniformViewMatrix == -1) {
		logErr("Could not bind uniform %s\n", uniformName);
	}

	uniformName = "ModelMatrix";
	uniformModelMatrix = glGetUniformLocation(program, uniformName);
	if (uniformModelMatrix == -1) {
		logErr("Could not bind uniform %s\n", uniformName);
	}

	uniformName = "LightPosition";
	uniformLightPosition = glGetUniformLocation(program, uniformName);
	if (uniformLightPosition == -1) {
		logErr("Could not bind uniform %s\n", uniformName);
	}
	uniformName = "LightColor";
	uniformLightAmbient = glGetUniformLocation(program, uniformName);
	if (uniformLightAmbient == -1){
		logErr("Could not bind uniform %s\n", uniformName);
	}
    
    uniformName = "texture";
    uniformTexture = glGetUniformLocation(program, uniformName);
	if (uniformTexture == -1){
		logErr("Could not bind uniform %s\n", uniformName);
	}

}


void DirectionalLightShader::setVars(glm::mat4 projView, glm::mat4 model, glm::mat4 view, glm::mat4 projection, Light* renderLight, glm::vec4 lightWorldPosition, glm::vec4 light_direction, glm::vec3 light_ambient, glm::mat4 projectionLight, glm::mat4 viewLight, GLuint depthTexture, bool receiveShadows){
	glUniformMatrix4fv(uniformModelMatrix, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniformViewMatrix, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniformProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3f(uniformLightPosition, lightWorldPosition.x, lightWorldPosition.y, lightWorldPosition.z);
	glUniform3f(uniformLightAmbient, light_ambient.x, light_ambient.y, light_ambient.z);

	if(texture){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->getTextureId());
		glUniform1i(uniformTexture, 0);
	}
}

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

#include "DepthMapShader.h"

DepthMapShader::DepthMapShader(){
	texture = 0;

	hasNormals = false;
	hasTangents = false;
	hasBitangents = false;
	hasUV = false;

	loadShader("DepthMapShader");
}


void DepthMapShader::loadVars(){
	const char* attributeName = "coord3d";
	attributeCoord = glGetAttribLocation(program, attributeName);
	if (attributeCoord == -1) {
		logErr("Could not bind attribute %s\n", attributeName);
		return;
	}
	const char* uniformName;
	uniformName = "mvp";
	uniformMVP = glGetUniformLocation(program, uniformName);
	if (uniformMVP == -1) {
		logErr("Could not bind uniform %s\n", uniformName);
		return;
	}
}

void DepthMapShader::setVars(glm::mat4 projView, glm::mat4 model, glm::mat4 view, glm::mat4 projection, Light* renderLight, glm::vec4 lightWorldPosition, glm::vec4 light_direction, glm::vec3 light_ambient, glm::mat4 projectionLight, glm::mat4 viewLight, GLuint depthTexture, bool receiveShadows){
	glUniformMatrix4fv(uniformMVP, 1, GL_FALSE, glm::value_ptr(projectionLight*viewLight*model));
}

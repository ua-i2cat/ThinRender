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

#include "VideoPlaneShader.h"
#include "../../log/Log.h"
VideoPlaneShader::VideoPlaneShader(){
	logInf("VideoPlaneShader::VideoPlaneShader");
	texture = 0;
	hasNormals = false;
	hasTangents = false;
	hasBitangents = false;
	hasUV = true;
    #ifdef ANDROID_PLATFORM
	loadShader("VideoPlaneShader");
#else
    loadShader("VideoIOSShader");
#endif
    color = glm::vec4(1.0f,1.0f,1.0f,1.0f);
}
void VideoPlaneShader::loadVars(){
	logInf("VideoPlaneShader::loadVars");
	const char* attributeName = "coord3d";
	attributeCoord = glGetAttribLocation(program, attributeName);
	if (attributeCoord == -1) {
		logErr("Could not bind attribute %s\n", attributeName);
		return;
	}
	attributeName = "texcoord";
	attributeTexture = glGetAttribLocation(program, attributeName);
	if (attributeTexture == -1) {
		logErr("Could not bind attribute %s\n", attributeName);
		attributeTexture = 0;
	}
    const char* uniformName;
#ifdef IOS_PLATFORM
    uniformName = "SamplerY";
	uniformLuminanceTexture = glGetUniformLocation(program, uniformName);
	if (uniformLuminanceTexture == -1) {
		logErr("Could not bind uniform %s\n", uniformName);
		return;
	}
    glUniform1i(uniformLuminanceTexture, 0);
    
#else
	uniformName = "texture";
	uniformTexture = glGetUniformLocation(program, uniformName);
	if (uniformTexture == -1) {
		logErr("Could not bind uniform %s\n", uniformName);
		return;
	}
    
#endif
	uniformName = "mvp";
	uniformMVP = glGetUniformLocation(program, uniformName);
	if (uniformMVP == -1) {
		logErr("Could not bind uniform %s\n", uniformName);
		return;
	}
}
void VideoPlaneShader::setVars(glm::mat4 projView, glm::mat4 model, glm::mat4 view, glm::mat4 projection, Light* renderLight, glm::vec4 lightWorldPosition, glm::vec4 light_direction, glm::vec3 light_ambient, glm::mat4 projectionLight, glm::mat4 viewLight, GLuint depthTexture, bool receiveShadows){

	if(texture == 0) return;

	int error = glGetError();
	if(error != GL_NO_ERROR)logErr("1");
	glUniformMatrix4fv(uniformMVP, 1, GL_FALSE, glm::value_ptr(projView));
	
	error = glGetError();
	if(error != GL_NO_ERROR)logErr("2");

	
#ifdef ANDROID_PLATFORM
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture->getTextureId());
    glUniform1i(uniformTexture, 0);
#else

    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->getTextureId());
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glUniform1i(uniformLuminanceTexture, 0);
    
    
#endif
    

	error = glGetError();
	if(error != GL_NO_ERROR)logErr("VideoPlaneShader::setVars error 3 error num %i", error);
}

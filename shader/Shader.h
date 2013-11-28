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

#ifndef SHADER_H
#define SHADER_H

#include "../OpenGLPlatform.h"

#include <string>
using namespace std;

#include "../Platform.h"
#include "../log/Log.h"
#include "../fileSystem/FileSystem.h"
#include "../texture/Texture.h"
#include "../texture/TextureManager.h"
#include "../utils/Maths.h"
#include "../sceneObjects/light/Light.h"

/**
 * Shader class is the base for the other more specific implementations
 */
class Shader {
public:
    /**
     * Shader constructor
     */
	Shader();
    /**
     * Shader destructor
     */
	virtual ~Shader();
    /**
     * freeResources deletes the program
     */
	void freeResources();
    /**
     * loadShader
     * @param std::string fileName of the shader, id there is diffuse.v.glsl and diffuse.f.glsl the name will be diffuse
     * @return 0 if successfully compiled shader, -1 if there is any error
     */
	int loadShader(string fileName);
    /**
     * printShaderLog prints all compiller errors from this shader
     * @param GLuint object
     */
	void printShaderLog(GLuint object);
    /**
     * haveNormals
     * @return bool
     */
	bool haveNormals();
    /**
     * haveTangents
     * @return bool
     */
	bool haveTangents();
    /**
     * haveBitangents
     * @return bool
     */
	bool haveBitangents();
    /**
     * haveUV
     * @return bool
     */
	bool haveUV();
    /**
     * useProgram activates the current program
     */
	void useProgram();
    /**
     * loadVars is a pure virtual function to load all the attributes and uniforms
     */
	virtual void loadVars() = 0;

    /**
     * setVars is a pure virtual function to set all variables to the attributes and uniforms
     * @param glm::mat4 projView
     * @param glm::mat4 projection
     * @param glm::mat4 view
     * @param Light* renderLight
     * @param GLuint depthTexture
     * @param glm::vec4 light_position
     * @param glm::vec4 light_direction
     * @param glm::vec3 light_ambient
     * @param glm::mat4 projViewLight
     * @param glm::mat4 viewLight
     * @param GLuint depthTexture
     * @param bool receiveShadows
     */
	virtual void setVars(glm::mat4 projView, glm::mat4 model, glm::mat4 view, glm::mat4 projection, Light* renderLight, glm::vec4 lightWorldPosition, glm::vec4 light_direction, glm::vec3 light_ambient, glm::mat4 projectionLight, glm::mat4 viewLight, GLuint depthTexture, bool receiveShadows) = 0;
	GLuint program;
//	GLint attributeCoord, attributeTexture, attributeNormal, uniformMVP, uniformM/*, uniformV, uniformP*/, uniformInvTransp/*, uniformVinv, uniformAnim*/;
//	GLint uniformLightAmbient, uniformLightPosition, uniformLightDirection, uniformLightDifuse, uniformLightSpecular;
	GLint attributeCoord, attributeTexture, attributeNormal, attributeTangent, attributeBitangent, uniformMVP;
    
    /**
     * disableTextures set the bind texture to 0
     */
	void disableTextures();
    /**
     * loadTexture is a pure virtual function to bind the texture2D
     * @param Texture* texture
     */
	virtual void loadTexture(Texture* texture);//used in submeshes with more than 1 texture...
	Texture* texture;
	//used in other shaders if it implements
	glm::vec4 color;
protected:
	bool hasNormals, hasTangents, hasBitangents, hasUV;
	GLuint createShader(const char* filename, GLenum type);
};
#endif

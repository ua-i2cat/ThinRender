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

#ifndef VIDEO_PLANE_SHADER_H
#define VIDEO_PLANE_SHADER_H

#include "../../OpenGLPlatform.h"

#include <string>
using namespace std;

#include "../../Platform.h"
#include "../../log/Log.h"
#include "../../fileSystem/FileSystem.h"
#include "../../texture/Texture.h"
#include "../../texture/TextureManager.h"
#include "../Shader.h"
#include "../../utils/Maths.h"

#ifdef IOS_PLATFORM
#include "../../modules/video/VideoIOSWrapper.h"
#endif
class VideoPlaneShader : public Shader {
public:
	VideoPlaneShader();
	/**
     * loadVars is a function to load all the attributes and uniforms
     */
	void loadVars();
    /**
     * setVars is a function to set all variables to the attributes and uniforms
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
	void setVars(glm::mat4 projView, glm::mat4 model, glm::mat4 view, glm::mat4 projection, Light* renderLight, glm::vec4 lightWorldPosition, glm::vec4 light_direction, glm::vec3 light_ambient, glm::mat4 projectionLight, glm::mat4 viewLight, GLuint depthTexture, bool receiveShadows);
	GLint uniformTexture;
#ifdef IOS_PLATFORM
    GLint uniformLuminanceTexture;
    GLint uniformChrominanceTexture;
#endif
};
#endif

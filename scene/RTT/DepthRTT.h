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

#ifndef DEPTH_RENDER_TO_TEXTURE_H
#define DEPTH_RENDER_TO_TEXTURE_H

#include "BasicRTT.h"
#include "../../shader/shadow/DepthMapShader.h"
#include "../../shader/Shader.h"

class DepthRTT : public BasicRTT {
public:
	DepthRTT();
	~DepthRTT();
    /**
     * renderTextureFrom renders the meshes using the view projection matrix
     * @param glm::mat4 projection matrix
     * @param glm::mat4 view matrix
     * @param std::vector<Mesh*> meshes
     */
	virtual void renderTextureFrom(glm::mat4 projection, glm::mat4 view, std::vector<Mesh*> meshes);
    
    ///shader used to render all the objects without changing them
	Shader* shadowShader;
};

#endif

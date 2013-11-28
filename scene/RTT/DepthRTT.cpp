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

#include "DepthRTT.h"

DepthRTT::DepthRTT(){
	shadowShader = new DepthMapShader();
}
DepthRTT::~DepthRTT(){
	delete shadowShader;
}

void DepthRTT::renderTextureFrom(glm::mat4 projection, glm::mat4 view, std::vector<Mesh*> meshes){
//	glCullFace(GL_FRONT);
	setViewPortRTT();
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_RTT_frame_buffer);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUseProgram(shadowShader->program);

	for(int i = 0; i < meshes.size(); i++){
		if(!meshes[i]->options & (Mesh::CAST_SHADOWS_MASK | Mesh::FAKE_GEOMETRY_CAST_SHADOWS_MASK)) continue;
		meshes[i]->renderWithoutShader(projection, view, shadowShader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	setViewPortContext();
//	glCullFace(GL_BACK);
}

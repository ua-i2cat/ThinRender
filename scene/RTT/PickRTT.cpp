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

#include "PickRTT.h"
#include "../../globalData/GlobalData.h"

PickRTT::PickRTT(){
	colorShader = new ColorShader();
	xSelectedScreen = 0;
	ySelectedScreen = 0;
	previousPickId = 0;
}
PickRTT::~PickRTT(){
	delete colorShader;
}

void PickRTT::initFrameBuffer(int w, int h){
	this->w = w;
	this->h = h;

	/* Depth buffer */
	glGenRenderbuffers(1, &rbo_RTT_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_RTT_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	/* Framebuffer to link everything together */
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_RTT_frame_buffer);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_RTT_depth);

	GLenum status;
	if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		logErr("PickRTT glCheckFramebufferStatus: error %p", status);
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void PickRTT::renderTextureFrom(glm::mat4 projection, glm::mat4 view, std::vector<Mesh*> meshes){
	setViewPortRTT();
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_RTT_frame_buffer);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(colorShader->program);
	unsigned int id;
	for(int i = 0; i < meshes.size(); i++){
		if(!meshes[i]->options & Mesh::SELECTIONABLE_MASK) continue;
		id = meshes[i]->id;
		glUniform4f((colorShader->uniformColor), ((float)((id)&0xff))/255.0f, ((float)((id>>8)&0xff))/255.0f, ((float)((id>>16)&0xff))/255.0f, ((float)((id>>24)&0xff))/255.0f);
		meshes[i]->renderWithoutShader(projection, view, colorShader);
	}
	glReadPixels(w*((float)xSelectedScreen/(float)GlobalData::getInstance()->screenWidth), h*((float)GlobalData::getInstance()->screenHeight - ySelectedScreen - 1)/(float)GlobalData::getInstance()->screenHeight, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &previousPickId);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	setViewPortContext();
}

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

#include "BasicRTT.h"
#include "../../globalData/GlobalData.h"
BasicRTT::BasicRTT(){
	fbo_RTT_frame_buffer = 0;
	fbo_RTT_texture = 0;
	rbo_RTT_depth = 0;
}

BasicRTT::~BasicRTT(){
   glDeleteTextures(1, &fbo_RTT_texture);
   glDeleteRenderbuffers(1, &rbo_RTT_depth);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glDeleteFramebuffers(1, &fbo_RTT_frame_buffer);
}
void BasicRTT::initFrameBuffer(int w, int h){
	this->w = w;
	this->h = h;
	/* Texture */
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &fbo_RTT_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_RTT_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	/* Depth buffer */
	glGenRenderbuffers(1, &rbo_RTT_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_RTT_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	/* Framebuffer to link everything together */
	glGenFramebuffers(1, &fbo_RTT_frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_RTT_frame_buffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_RTT_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_RTT_depth);

	GLenum status;
	if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
		logErr("BasicSceneManager glCheckFramebufferStatus: error %p", (void*)status);
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint BasicRTT::getTeture(){
	return fbo_RTT_texture;
}
void BasicRTT::setViewPortRTT(){
	glViewport(0,0,w,h);
}
void BasicRTT::setViewPortContext(){
	glViewport(0,0,GlobalData::getInstance()->screenWidth,GlobalData::getInstance()->screenHeight);
}



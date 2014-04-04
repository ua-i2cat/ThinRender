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
 *  Authors:         Marc Fernandez Vanaclocha <marc.fernandez@i2cat.net>
 */

#include "CaptureCamera.h"
#include "../../utils/TextureWindow.h"
#include "../../shader/Shader.h"
#include "../../shader/gui/CameraPreviewPlaneShader.h"

#ifdef IOS_PLATFORM
#define GL_TEXTURE_EXTERNAL_OES GL_TEXTURE_2D
#endif

CaptureCamera::CaptureCamera(RectGUI* rect){
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureID);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	setCameraTexturePreview(textureID);

	Shader* shader = new CameraPreviewPlaneShader();
	rect->setShader(shader);
	rect->setTexture(TextureManager::getInstance()->getTexture("blueSquare.png"));
	rect->setTexture(textureID);
}

CaptureCamera::~CaptureCamera(){
	logInf("Camera capture destructor, deleting textureId");
	glDeleteTextures(1, &textureID);
	logInf("Camera capture destructor, deleted textureId");
	closeCamera();
	logInf("Camera capture destructor ended");
}

void CaptureCamera::cameraUpdatePreview(){
	updateTextureCamera();
}

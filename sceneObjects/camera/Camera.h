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

#ifndef CAMERA_H
#define CAMERA_H

#include "../SceneObject.h"
#include "../../utils/Maths.h"

/**
 * Camera class
 */
class Camera : public SceneObject {
public:
	const static int ORTHOGONAL = 0;
	const static int PERSPECTIVE = 1;
	int getType() { return CAMERA_TYPE; }
    /**
     * Camera constructor, it creates a perspective one
     */
	Camera();
    /**
     * Camera constructor, it creates a perspective one with
     * @param float nearDistance
     * @param float farDistance
     * @param float ratio
     * @param float aperture
     */
	Camera(float nearDistance, float farDistance, float ratio, float aperture);
    /**
     * Camera constructor, it creates a orthogonal one with
     * @param float left
     * @param float right
     * @param float bottom
     * @param float top
     * @param float zNear
     * @param float zFar
     */
	Camera(float left, float right, float bottom, float top, float zNear, float zFar);
    
    /**
     * Camera destructor
     */
	~Camera();

    /**
     * setNearClipDistance
     * @param float nearDistance
     */
	void setNearClipDistance(float nearDistance);
    
    /**
     * setFarClipDistance
     * @param float farDistance
     */
	void setFarClipDistance(float farDistance);
    
    /**
     * setAspectRatio
     * @param float ratio
     */
	void setAspectRatio(float ratio);
    
    /**
     * setAperture
     * @param float aperture
     */
	void setAperture(float aperture);
    
    /**
     * setCameraType (ORTHOGONAL = 0, PERSPECTIVE = 1)
     * @param int type
     */
	void setCameraType(int type);
    
    /**
     * getNearClipDistance
     * @return float
     */
	float getNearClipDistance();
    
    /**
     * getFarClipDistance
     * @return float
     */
	float getFarClipDistance();
    
    /**
     * getAspectRatio
     * @return float
     */
	float getAspectRatio();
    
    /**
     * getAperture
     * @return float
     */
	float getAperture();
    
    /**
     * getCameraType (ORTHOGONAL = 0, PERSPECTIVE = 1)
     * @return int
     */
	int getCameraType();
   
    /**
     * setEnabled
     * @param bool enable
     */
	void setEnabled(bool enable);
    
    /**
     * getEnabled
     * @return bool
     */
	bool getEnabled();
    
    /**
     * setLeftOrtho
     * @param float left
     */
	void setLeftOrtho(float left);
    
    /**
     * setRightOrtho
     * @param float right
     */
	void setRightOrtho(float right);
    
    /**
     * setBottomOrtho
     * @param float bottom
     */
	void setBottomOrtho(float bottom);
    
    /**
     * setTopOrtho
     * @param float top
     */
	void setTopOrtho(float top);
    
    /**
     * setZNearOrtho
     * @param float zNear
     */
	void setZNearOrtho(float zNear);
    
    /**
     * setZFarOrtho
     * @param float zFar
     */
	void setZFarOrtho(float zFar);
    
    /**
     * getLeftOrtho
     * @return float
     */
	float getLeftOrtho();
    
    /**
     * getRightOrtho
     * @return float
     */
	float getRightOrtho();
    
    /**
     * getBottomOrtho
     * @return float
     */
	float getBottomOrtho();
    
    /**
     * getTopOrtho
     * @return float
     */
	float getTopOrtho();
    
    /**
     * getZNearOrtho
     * @return float
     */
	float getZNearOrtho();
    
    /**
     * getZFarOrtho
     * @return float
     */
	float getZFarOrtho();
    
    /**
     * setProjectionMatrix
     * @param glm::mat4 projectionMatrix
     */
    void setProjectionMatrix(glm::mat4 projectionMatrix);
    
    /**
     * getViewMatrix
     * @return glm::mat4
     */
	glm::mat4 getViewMatrix();
    
    /**
     * getProjectionMatrix
     * @return glm::mat4
     */
	glm::mat4 getProjectionMatrix();
    
    /**
     * getOrthogonalProjectionMatrix
     * @return glm::mat4
     */
	glm::mat4 getOrthogonalProjectionMatrix();
private:
	float nearClip;
	float farClip;
	float aspectRatio;
	float apertureAngle;
	int cameraType;
	bool enabled;

	//for orthogonal camera
	float leftOrtho;
	float rightOrtho;
	float bottomOrtho;
	float topOrtho;
	float zNearOrtho;
	float zFarOrtho;

//	bool cacheView;
	glm::mat4 viewMatrix;

	bool cacheProjection;
	glm::mat4 projection;
};

#endif

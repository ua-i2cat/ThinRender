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

#ifndef LIGHT_H
#define LIGHT_H

#include "../SceneObject.h"
#include "../../utils/Maths.h"
/**
 * This class needs to be completed not here but in the whole structure, currently is useless
 */
class Light : public SceneObject{
public:
	const static int DIRECTIONAL_LIGHT = 0;
	const static int SPOT_LIGHT = 1;
	const static int POINT_LIGHT = 2;

	int getType() { return LIGHT_TYPE; }

	Light();
	Light(int type);
	~Light();

	void setDiffuse(glm::vec4 diffuseColor);
	void setSpecular(glm::vec4 specularColor);
	glm::vec4 getDiffuse();
	glm::vec4 getSpecular();

	void setAttenuation(float constant, float linear, float quadratic);

	float getConstantAttenuation();
	float getLinearAttenuation();
	float getQuadraticAttenuation();

	void setSpotCutoff(float cutoff);
	void setSpotExponent(float exponent);
	float getSpotCutoff();
	float getSpotExponent();
	glm::mat4 getOrthogonalProjectionMatrix();
private:
	glm::vec4 diffuse;
	glm::vec4 specular;
	float constantAttenuation, linearAttenuation, quadraticAttenuation;
	float spotCutoff, spotExponent;
};
#endif

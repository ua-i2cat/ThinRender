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

#include "Light.h"

Light::Light(){
	type = DIRECTIONAL_LIGHT;
	diffuse = glm::vec4(1.0f,  1.0f,  1.0f, 1.0f);
	specular = glm::vec4(1.0f,  1.0f,  1.0f, 1.0f);
}

Light::Light(int type){//TODO
	type = DIRECTIONAL_LIGHT;
}

Light::~Light(){

}

void Light::setDiffuse(glm::vec4 diffuseColor){
	diffuse = diffuseColor;
}
void Light::setSpecular(glm::vec4 specularColor){
	specular = specularColor;
}

glm::vec4 Light::getDiffuse(){
	return diffuse;
}
glm::vec4 Light::getSpecular(){
	return specular;
}

void Light::setAttenuation(float constant, float linear, float quadratic){
	constantAttenuation = constant;
	linearAttenuation = linear;
	quadraticAttenuation = quadratic;
}
float Light::getConstantAttenuation(){
	return constantAttenuation;
}
float Light::getLinearAttenuation(){
	return linearAttenuation;
}
float Light::getQuadraticAttenuation(){
	return quadraticAttenuation;
}

void Light::setSpotCutoff(float cutoff){
	spotCutoff = cutoff;
}
void Light::setSpotExponent(float exponent){
	spotExponent = exponent;
}
float Light::getSpotCutoff(){
	return spotCutoff;
}
float Light::getSpotExponent(){
	return spotExponent;

}

glm::mat4 Light::getOrthogonalProjectionMatrix(){//ONLY USED FOR SHADOWMAPS
	float leftOrtho = -1.0f;
	float rightOrtho = 1.0f;
	float bottomOrtho = -1.0f;
	float topOrtho = 1.0f;
	float zNearOrtho = -10.0f;
	float zFarOrtho = 30.0f;
	return glm::ortho(leftOrtho, rightOrtho, bottomOrtho, topOrtho, zNearOrtho, zFarOrtho);
}
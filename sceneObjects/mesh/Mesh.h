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

#ifndef MESH_H
#define MESH_H
#include "../../utils/Maths.h"
#include <vector>
#include <string>

#include "../../OpenGLPlatform.h"

#include "../../shader/Shader.h"
#include "../../shader/lightning/DirectionalLightShader.h"
#include "../../shader/lightning/DirectionalLightShadowMapShader.h"
#include "../SceneObject.h"
#include "../light/Light.h"
#include "SubMesh.h"

using namespace std;

/**
 * Mesh class, this class manages all the submeshes and the shaders
 */
class Mesh : public SceneObject {
public:
    //TODO
	struct Joint
	{
		std::string     jointName;
		int             jointParentID;
		glm::vec3       jointPos;
		glm::quat       jointOrient;
	};
	int getType() { return MESH_TYPE; }
	Mesh(string name, unsigned int id);
	Mesh(Mesh* createMeshFromOther, unsigned int id);
	Mesh(Shader* shader, unsigned int id);
	~Mesh();
    /**
     * generateVBO generates all the vertex buffer objects from the vectors
     */
	void generateVBO();

	///currently without use
    vector<Joint> joints;
    
    /// subMeshes vector containing at las 1
	vector<SubMesh*> subMeshes;

    Shader* shader;
    
    /**
     * setShader sets the shader deletting the previos one
     * @param Shader* newShader
     */
    void setShader(Shader* newShader);
    
	string name;
	bool renderEnabled;
	unsigned int id;

    /**
     * render makes n drawcalls where n are the submesh vector size
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
     */
	void render(glm::mat4 projView, glm::mat4 projection, glm::mat4 view, Light* renderLight, GLuint depthTexture, glm::vec4 light_position, glm::vec4 light_direction, glm::vec3 light_ambient, glm::mat4 projViewLight, glm::mat4 viewLight);
	/**
     * renderWithoutShader makes n drawcalls where n are the submesh vector size without changing of shader
     * @param glm::mat4 projView
     * @param glm::mat4 view
     * @param Shader* shader
     */
    void renderWithoutShader(glm::mat4 projView, glm::mat4 view, Shader* shader);

	const static int BACK_CULLING_MASK = 0x00000001;
	const static int FRONT_CULLING_MASK = 0x00000002;
	const static int NO_CULLING_MASK = 0x00000004;
	const static int ALPHA_BLENDING_MASK = 0x00000008;
	const static int CAST_SHADOWS_MASK = 0x00000010;
	const static int FAKE_GEOMETRY_CAST_SHADOWS_MASK = 0x00000020;//for geometry that isn't rendered at the end but generates shadows
	const static int RECEIVE_SHADOWS_MASK = 0x00000040;
	const static int SELECTIONABLE_MASK = 0x00000080;
	const static int GUI_MASK = 0x00000100;

	int options;//defined in previous masks

    /**
     * setCulling BACK_CULLING_MASK FRONT_CULLING_MASK NO_CULLING_MASK
     * @param int culling
     */
	void setCulling(int culling);//BACK_CULLING_MASK FRONT_CULLING_MASK NO_CULLING_MASK
	/**
     * setAlpha
     * @param bool alpha
     */
    void setAlpha(bool alpha);//in other time, there is the option of using discard and avoid using alpha blending (like color.a < 0.5 => discard, but all this in the shader)
	/**
     * setCastShadows
     * @param bool cast
     */
	void setCastShadows(bool cast);
	/**
     * setReceiveShadows
     * @param bool receive
     */
	void setReceiveShadows(bool receive);


	//sort meshes via view projection
	glm::vec4 viewPosition;
	bool operator<(const Mesh& right) const
	{
		if(options & ALPHA_BLENDING_MASK){
			return viewPosition.z > right.viewPosition.z;
		}else{
			return viewPosition.z < right.viewPosition.z;
		}
	}
    
	/**
     * calculateViewPosition to sort z
     * @param glm::mat4 viewMatrix
     */
	void calculateViewPosition(glm::mat4 viewMatrix){
		viewPosition = viewMatrix * getAttachedNode()->getFullTransform() * glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}

protected:
	void calculateBoundingBox();
	glm::vec3 AABBmin;
	glm::vec3 AABBmax;
};

#endif

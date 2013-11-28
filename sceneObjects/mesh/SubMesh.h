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

#ifndef SUBMESH_H
#define SUBMESH_H

#include <vector>
#include <string>

#include "../../OpenGLPlatform.h"

#include "../../shader/Shader.h"

using namespace std;

/**
 * SubMesh class makes the draw call and manages the different vertex buffer objects or different textures
 */
class SubMesh{
public:
	struct Weight
	{
		int             weightJointID;
		float           weightBias;
		glm::vec3       weightPos;
	};

	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec3> tangents;
	vector<glm::vec3> bitangents;
	vector<glm::vec2> textureCoord;
	vector<glm::vec3> colors;
	vector<glm::i32vec3> elements;
	vector<glm::i32vec2> weightsIndex;//first weight is star and the other one the count
	vector<Weight> weights;
	std::string subMeshTextureName;

    /**
     * setSubMeshTextureName
     * @param const char* textureName
     */
	void setSubMeshTextureName(const char* textureName);
	Texture* textureSubMesh;

	bool renderEnabled;
	unsigned int id;
    
    /**
     * SubMesh constructor
     * @param unsigned int id)
     */
	SubMesh(unsigned int id);
    /**
     * SubMesh destructor, frees all vertex buffer objects
     */
	~SubMesh();
    
    /**
     * render does the draw call
     * @param Shader* shader
     */
	void render(Shader* shader);

    /**
     * generateVBO generates the vertex buffer objects from the different vectors
     */
	void generateVBO();
protected:
	GLuint vbo_vertices, vbo_normals, vbo_tangents, vbo_bitangents, vbo_texcoords, vbo_colors;
	GLuint ibo_elements;
};

#endif

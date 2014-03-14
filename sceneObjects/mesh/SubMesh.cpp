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

#include "SubMesh.h"

SubMesh::SubMesh(unsigned int id){
	vbo_vertices = 0;
	vbo_normals = 0;
	vbo_tangents = 0;
	vbo_bitangents = 0;
	vbo_texcoords = 0;
	vbo_colors = 0;
	ibo_elements = 0;
	this->id = id;
	subMeshTextureName = "";
	textureSubMesh = 0;
}

SubMesh::~SubMesh(){
	if(vbo_vertices != 0)
		glDeleteBuffers(1, &vbo_vertices);
	if(vbo_normals != 0)
		glDeleteBuffers(1, &vbo_normals);
	if(vbo_tangents != 0)
		glDeleteBuffers(1, &vbo_tangents);
	if(vbo_bitangents != 0)
		glDeleteBuffers(1, &vbo_bitangents);
	if(vbo_texcoords != 0)
		glDeleteBuffers(1, &vbo_texcoords);
	if(vbo_colors != 0)
		glDeleteBuffers(1, &vbo_colors);
	if(ibo_elements != 0)
		glDeleteBuffers(1, &ibo_elements);

	normals.clear();
	tangents.clear();
	bitangents.clear();
	textureCoord.clear();
	vertices.clear();
	colors.clear();
	elements.clear();
}
void SubMesh::setSubMeshTextureName(const char* textureName){
	subMeshTextureName = textureName;
	textureSubMesh = TextureManager::getInstance()->getTexture(textureName);
}

void SubMesh::render(Shader* shader){
	glEnableVertexAttribArray(shader->attributeCoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glVertexAttribPointer(
		shader->attributeCoord,   // attribute
		3,                   // number of elements per vertex, here (x,y,z)
		GL_FLOAT,            // the type of each element
		GL_FALSE,            // take our values as-is
		0,                	 // no extra data between each position
		0                    // offset of the first element
		);
	if(shader->haveNormals() && vbo_normals){
		glEnableVertexAttribArray(shader->attributeNormal);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
		glVertexAttribPointer(
			shader->attributeNormal,      // attribute
			3,                      // number of elements per vertex, here (r,g,b)
			GL_FLOAT,               // the type of each element
			GL_FALSE,               // take our values as-is
			0,               		// no extra data between each position
			0						// offset of first element
			);
	}

	if(shader->haveTangents() && vbo_tangents){
			glEnableVertexAttribArray(shader->attributeTangent);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
			glVertexAttribPointer(
				shader->attributeTangent,      // attribute
				3,                      // number of elements per vertex, here (r,g,b)
				GL_FLOAT,               // the type of each element
				GL_FALSE,               // take our values as-is
				0,               		// no extra data between each position
				0						// offset of first element
				);
	}

	if(shader->haveBitangents() && vbo_bitangents){
			glEnableVertexAttribArray(shader->attributeBitangent);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_bitangents);
			glVertexAttribPointer(
				shader->attributeBitangent,      // attribute
				3,                      // number of elements per vertex, here (r,g,b)
				GL_FLOAT,               // the type of each element
				GL_FALSE,               // take our values as-is
				0,               		// no extra data between each position
				0						// offset of first element
				);
	}

	if(shader->haveUV() && vbo_texcoords){
		glEnableVertexAttribArray(shader->attributeTexture);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glVertexAttribPointer(
			shader->attributeTexture,      // attribute
			2,                      // number of elements per vertex, here (r,g,b)
			GL_FLOAT,               // the type of each element
			GL_FALSE,               // take our values as-is
			0,               		// no extra data between each position
			0						// offset of first element
			);
	}
	if(ibo_elements){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLuint), GL_UNSIGNED_INT, 0);
	}else{
		/* Push each element in buffer_vertices to the vertex shader */
		glDrawArrays(GL_TRIANGLES, 0,vertices.size());
	}

	glDisableVertexAttribArray(shader->attributeCoord);
	if(shader->haveNormals() && vbo_normals) 	glDisableVertexAttribArray(shader->attributeNormal);
	if(shader->haveTangents() && vbo_tangents) 	glDisableVertexAttribArray(shader->attributeTangent);
	if(shader->haveBitangents() && vbo_bitangents) 	glDisableVertexAttribArray(shader->attributeBitangent);
	if(shader->haveUV() && vbo_texcoords) 		glDisableVertexAttribArray(shader->attributeTexture);
	shader->disableTextures();
}

void SubMesh::generateVBO(){
	if(vbo_vertices != 0)
		glDeleteBuffers(1, &vbo_vertices);
	if(vbo_normals != 0)
		glDeleteBuffers(1, &vbo_normals);
	if(vbo_tangents != 0)
		glDeleteBuffers(1, &vbo_tangents);
	if(vbo_bitangents != 0)
		glDeleteBuffers(1, &vbo_bitangents);
	if(vbo_colors != 0)
		glDeleteBuffers(1, &vbo_colors);
	if(vbo_texcoords != 0)
		glDeleteBuffers(1, &vbo_texcoords);
	if(ibo_elements != 0)
		glDeleteBuffers(1, &ibo_elements);
	if(vertices.size() != 0){
		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	}
	if(normals.size() != 0){
		glGenBuffers(1, &vbo_normals);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	}
	if(tangents.size() != 0){
		glGenBuffers(1, &vbo_tangents);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
		glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
	}
	if(bitangents.size() != 0){
		glGenBuffers(1, &vbo_bitangents);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_bitangents);
		glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
	}
	if(textureCoord.size() != 0){
		glGenBuffers(1, &vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, textureCoord.size() * sizeof(glm::vec2), &textureCoord[0], GL_STATIC_DRAW);
	}
	if(colors.size() != 0){
		glGenBuffers(1, &vbo_colors);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
	}
	if(elements.size() != 0){
		glGenBuffers(1, &ibo_elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(glm::i32vec3), &elements[0], GL_STATIC_DRAW);
	}

}

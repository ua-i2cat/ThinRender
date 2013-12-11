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

#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H

#include "Mesh.h"
#include "SubMesh.h"
#include "../../fileSystem/FileSystem.h"

/**
 * MeshManager class controlls all the created meshes, also parse the different formats to create the mesh, uses Singleton pattern
 */
class MeshManager{
public:
	static const size_t BUFFERSIZE = 4096;
	char tempBuffer[BUFFERSIZE];//temporal buffer!

	static MeshManager* getInstance();
	static void freeInstance();
    /**
     * getMesh
     * @param std::string name with path from assets folder
     * @return Mesh* with the parsed mesh
     */
	Mesh* getMesh(string name);
    /**
     * getMeshPlane2D
     * @param float left
     * @param float top
     * @param float width
     * @param float height
     * @return Mesh*
     */
	Mesh* getMeshPlane2D(float left, float top, float width, float height);
    /**
     * getMeshPlane2D returns a plane with full screen size
     * @return Mesh*
     */
	Mesh* getMeshPlane2D();

private:
	unsigned int getNewId();
	unsigned int idGenerator;
	Mesh* resultMesh;
	map<string, Mesh*> meshMap;
	MeshManager();
	~MeshManager();

	Mesh* parseMD5(string filename, char* buffer, int lenght);
	void prepareSubMeshVertex(Mesh* mesh, SubMesh* subMesh);
	void prepareSubMeshNormals(Mesh* mesh, SubMesh* subMesh);

	Mesh* parseOBJ(string filename, char* buffer, int lenght);

	static MeshManager* instance;

	void getNextWord(char* buffer, int* i);
	void skipLine(char* buffer, int* i);
	void getVector3(std::vector<glm::vec3>* vertices, char* buffer, int* i);
	void getVector2(std::vector<glm::vec2>* vertices, char* buffer, int* i);
	bool isSeparator(char current);
	void copyNextWord(char* tempBuffer, size_t length, char* buffer, int* i);
	void getFace(std::vector<glm::i32vec3>* vertices, char* buffer, int* i);

	void removeQuotes(string* str);
	void computeQuaternionW(glm::quat* quat);

	void printMeshData(Mesh* mesh);
	int elementOffsetOBJ;
};

#endif

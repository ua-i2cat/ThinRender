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

#include "../../utils/Maths.h"
#include "../../utils/fast_atof.h"
#include "MeshManager.h"
#include "../../log/Log.h"
#include "../../shader/gui/SimplePlaneShader.h"
#include "../../shader/gui/SimplePlaneDepthShader.h"
#include "../../globalData/GlobalData.h"

MeshManager* MeshManager::instance = 0;

MeshManager* MeshManager::getInstance(){
	if(instance == 0){
		instance = new MeshManager();
	}
	return instance;
}

void MeshManager::freeInstance(){
	if(MeshManager::instance != 0){
		delete instance;
		MeshManager::instance = 0;
	}
}

MeshManager::MeshManager(){
	idGenerator = 1;
}
MeshManager::~MeshManager(){
	meshMap.clear();
}
void MeshManager::printMeshData(Mesh* mesh){
	logInf("printMeshData: %s", mesh->name.c_str());
	int i, j;
	for(j = 0; j < mesh->subMeshes.size(); j++){
		logInf("vertex data submesh[%i]:",j);
		for(i = 0; i < mesh->subMeshes.at(j)->vertices.size(); i++){
			logInf("[%i]: {%f, %f, %f}", i, mesh->subMeshes.at(j)->vertices.at(i).x , mesh->subMeshes.at(j)->vertices.at(i).y, mesh->subMeshes.at(j)->vertices.at(i).z);
		}
		logInf("normal data:");
		for(i = 0; i < mesh->subMeshes.at(j)->normals.size(); i++){
			logInf("[%i]: {%f, %f, %f}", i, mesh->subMeshes.at(j)->normals.at(i).x , mesh->subMeshes.at(j)->normals.at(i).y, mesh->subMeshes.at(j)->normals.at(i).z);
		}
		logInf("texture data:");
		for(i = 0; i < mesh->subMeshes.at(j)->textureCoord.size(); i++){
			logInf("[%i]: {%f, %f}", i, mesh->subMeshes.at(j)->textureCoord.at(i).x , mesh->subMeshes.at(j)->textureCoord.at(i).y);
		}
		logInf("color data:");
		for(i = 0; i < mesh->subMeshes.at(j)->colors.size(); i++){
			logInf("[%i]: {%f, %f, %f}", i, mesh->subMeshes.at(j)->colors.at(i).x , mesh->subMeshes.at(j)->colors.at(i).y, mesh->subMeshes.at(j)->colors.at(i).z);
		}

		logInf("elements data:");
		for(i = 0; i < mesh->subMeshes.at(j)->elements.size(); i++){
			logInf("[%i]: {%i, %i, %i}", i, mesh->subMeshes.at(j)->elements.at(i).x , mesh->subMeshes.at(j)->elements.at(i).y,  mesh->subMeshes.at(j)->elements.at(i).z);
		}
	}
}

Mesh* MeshManager::getMesh(string name){
	std::map<string, Mesh*>::iterator it;
	it = meshMap.find(name);
	if(it != meshMap.end()){//mesh previament carregat LEAK!
		Mesh* result = new Mesh(it->second, getNewId());
		result->generateVBO();
		return result;
	}

	if(FileSystem::getInstance()->openFile(name) == -1){
		logErr("Error TextureManager::getTexture opening %s: ", name.c_str());
		return 0;
	}
	std::size_t foundMD5 = name.find(".md5");
	std::size_t foundOBJ = name.find(".obj");
	if (foundOBJ!=std::string::npos){
		meshMap[name] = parseOBJ(name, FileSystem::getInstance()->getFileData(name), FileSystem::getInstance()->getFileSize(name));
	}
	else if (foundMD5!=std::string::npos){
		meshMap[name] = parseMD5(name, FileSystem::getInstance()->getFileData(name), FileSystem::getInstance()->getFileSize(name));
	}
	else{
		FileSystem::getInstance()->destroyFileData(name);
		return 0;
	}
	FileSystem::getInstance()->destroyFileData(name);

//	printMeshData(meshMap[name]);
	meshMap[name]->generateVBO();
	return meshMap[name];
}

//normalized between 0.0f to 1.0f
Mesh* MeshManager::getMeshPlane2D(float left, float top, float width, float height, Shader* shader){
	if(shader == 0){
		shader = new SimplePlaneShader();
	}
	Mesh* resultPlane = new Mesh(shader, getNewId());
	SubMesh* subMesh = new SubMesh(0);
	subMesh->vertices.push_back(glm::vec3(left + width, 	top, 0.0f));
	subMesh->vertices.push_back(glm::vec3(left, 			top, 0.0f));
	subMesh->vertices.push_back(glm::vec3(left, 			top - height, 0.0f));

	subMesh->vertices.push_back(glm::vec3(left + width, 	top, 0.0f));
	subMesh->vertices.push_back(glm::vec3(left, 			top - height, 0.0f));
	subMesh->vertices.push_back(glm::vec3(left + width, 	top - height, 0.0f));

	subMesh->textureCoord.push_back(glm::vec2(1.0f, 1.0f));
	subMesh->textureCoord.push_back(glm::vec2(0.0f, 1.0f));
	subMesh->textureCoord.push_back(glm::vec2(0.0f, 0.0f));
	subMesh->textureCoord.push_back(glm::vec2(1.0f, 1.0f));
	subMesh->textureCoord.push_back(glm::vec2(0.0f, 0.0f));
	subMesh->textureCoord.push_back(glm::vec2(1.0f, 0.0f));

	resultPlane->subMeshes.push_back(subMesh);
	resultPlane->generateVBO();
	return resultPlane;
}

Mesh* MeshManager::getMeshPlane2D(Shader* shader){
	if(shader == 0){
		shader = new SimplePlaneShader();
	}
	Mesh* resultPlane = new Mesh(shader, getNewId());
	SubMesh* subMesh = new SubMesh(0);

	subMesh->vertices.push_back(glm::vec3(0.5f, 0.5f, 	0.0f));
	subMesh->vertices.push_back(glm::vec3(-0.5f, 0.5f, 	0.0f));
	subMesh->vertices.push_back(glm::vec3(-0.5f, -0.5f,	0.0f));

	subMesh->vertices.push_back(glm::vec3(0.5f,	0.5f,	 0.0f));
	subMesh->vertices.push_back(glm::vec3(-0.5f, -0.5f, 	0.0f));
	subMesh->vertices.push_back(glm::vec3(0.5f,	-0.5f, 	0.0f));

	subMesh->textureCoord.push_back(glm::vec2(1.0f, 1.0f));
	subMesh->textureCoord.push_back(glm::vec2(0.0f, 1.0f));
	subMesh->textureCoord.push_back(glm::vec2(0.0f, 0.0f));
	subMesh->textureCoord.push_back(glm::vec2(1.0f, 1.0f));
	subMesh->textureCoord.push_back(glm::vec2(0.0f, 0.0f));
	subMesh->textureCoord.push_back(glm::vec2(1.0f, 0.0f));

	resultPlane->subMeshes.push_back(subMesh);
	resultPlane->generateVBO();
	return resultPlane;
}

unsigned int MeshManager::getNewId(){
	idGenerator++;
	return idGenerator;
}

Mesh* MeshManager::parseMD5(string filename, char* buffer, int lenght){
    Mesh* resultMesh = new Mesh(filename, getNewId());
	int numJoints = 0;
	int numMeshes = 0;
	int numVerts = 0;
	int numTris = 0;
	int numWeights = 0;
	unsigned int idSubMesh = 0;
	int n = 0;
	copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);//autoincrease i
	while (buffer[n] != '\0'){
		if(strncmp(tempBuffer, "MD5Version", 10) == 0){
			copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
			//resultMesh->versionMD5 = (int) atoi(tempBuffer);
			//logInf("md5 version %s", tempBuffer);
		}
		else if(strncmp(tempBuffer, "commandline", 11) == 0){
			skipLine(buffer, &n);
		}
		else if(strncmp(tempBuffer, "numJoints", 9) == 0){
			copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
			numJoints = (int) atoi(tempBuffer);
			resultMesh->joints.reserve(numJoints);
		}
		else if(strncmp(tempBuffer, "numMeshes", 9) == 0){
			copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
			numMeshes = (int) atoi(tempBuffer);
			resultMesh->subMeshes.reserve(numMeshes);
		}
		else if(strncmp(tempBuffer, "joints", 6) == 0){
			copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n); //read the '{' character
			Mesh::Joint auxJoint;
			for(int i = 0; i < numJoints; i++){
				copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
				auxJoint.jointName = tempBuffer;
				copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
				auxJoint.jointParentID = (int) atoi(tempBuffer);
				copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);// '(' char
				copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
				auxJoint.jointPos.x = (float) fast_atof(tempBuffer);
				copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
				auxJoint.jointPos.y = (float) fast_atof(tempBuffer);
				copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
				auxJoint.jointPos.z = (float) fast_atof(tempBuffer);
				copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);// ')' char
				copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);// '(' char
				copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
				auxJoint.jointOrient.x = (float) fast_atof(tempBuffer);
				copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
				auxJoint.jointOrient.y = (float) fast_atof(tempBuffer);
				copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
				auxJoint.jointOrient.z = (float) fast_atof(tempBuffer);
				skipLine(buffer, &n);

				removeQuotes(&(auxJoint.jointName));
				computeQuaternionW(&(auxJoint.jointOrient));

				resultMesh->joints.push_back(auxJoint);
			}
		}
		else if(strncmp(tempBuffer, "mesh", 4) == 0){
			SubMesh* auxSubMesh = new SubMesh(idSubMesh);
			idSubMesh++;
			copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);// char {
			copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
			while(strncmp(tempBuffer, "}", 1) != 0){
				if(strncmp(tempBuffer, "shader", 6) == 0){
					///////////////TODO
					//shader factory
					//used to set the texture!! be careful
					copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
					auxSubMesh->setSubMeshTextureName(tempBuffer);
					skipLine(buffer, &n);
				}
				else if(strncmp(tempBuffer, "numverts", 8) == 0){
					copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
					numVerts = (int) atoi(tempBuffer);
					glm::vec2 textCoord;
					glm::i32vec2 weightStartAndCount;
					for(int i = 0; i < numVerts; ++i){
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);//"vert"
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);//id (sorted)
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);//'('

						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
						textCoord.x = (float) fast_atof(tempBuffer);
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
						textCoord.y = (float) fast_atof(tempBuffer);

						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);//')'

						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
						weightStartAndCount.x = (int) atoi(tempBuffer);
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
						weightStartAndCount.y = (int) atoi(tempBuffer);

						auxSubMesh->textureCoord.push_back(textCoord);
						auxSubMesh->weightsIndex.push_back(weightStartAndCount);
					}
				}
				else if(strncmp(tempBuffer, "numtris", 7) == 0){
					copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
					numTris = (int) atoi(tempBuffer);
					glm::i32vec3 tri;
					auxSubMesh->elements.reserve(numTris);
					for (int i = 0; i < numTris; ++i){
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
						tri.x = (int) atoi(tempBuffer);
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
						tri.z = (int) atoi(tempBuffer);
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
						tri.y = (int) atoi(tempBuffer);
						auxSubMesh->elements.push_back(tri);
					}
				}
				else if(strncmp(tempBuffer, "numweights", 10) == 0){
					copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
					numWeights = (int) atoi(tempBuffer);
					SubMesh::Weight auxWeight;
					auxSubMesh->weights.reserve(numWeights);
					for (int i = 0; i < numWeights; ++i){
						//logInf("Weight[%i]",i);
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);//weight
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);//id (sorted)
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);//id joint
						auxWeight.weightJointID = (int) atoi(tempBuffer);
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);//bias
						auxWeight.weightBias = (float) fast_atof(tempBuffer);
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);//'('
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
						auxWeight.weightPos.x = (float) fast_atof(tempBuffer);
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
						auxWeight.weightPos.y = (float) fast_atof(tempBuffer);
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);
						auxWeight.weightPos.z = (float) fast_atof(tempBuffer);
						copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);//')'

						auxSubMesh->weights.push_back(auxWeight);
						//logInf("Weight[%i]",i);
					}
				}
				else if(strncmp(tempBuffer, "//", 2) == 0){
					skipLine(buffer, &n);
				}
				copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);

			}
			prepareSubMeshVertex(resultMesh, auxSubMesh);
			prepareSubMeshNormals(resultMesh, auxSubMesh);
			resultMesh->subMeshes.push_back(auxSubMesh);
		}

		copyNextWord(tempBuffer, BUFFERSIZE, buffer, &n);//autoincrease i
        if(n>=lenght)break;

	}
	return resultMesh;
}

void MeshManager::prepareSubMeshVertex(Mesh* mesh, SubMesh* subMesh){
	subMesh->vertices.clear();
	subMesh->vertices.reserve(subMesh->weightsIndex.size());
	glm::vec3 position, normal;
    // Compute vertex positions
    for (int i = 0; i < subMesh->weightsIndex.size(); ++i){
        position = glm::vec3(0);
        normal = glm::vec3(0);

        // Sum the position of the weights
        for (int j = 0; j < subMesh->weightsIndex.at(i).y; ++j)//subMesh.weightsIndex.at(i).y weight count
        {
        	SubMesh::Weight& weight = subMesh->weights.at(subMesh->weightsIndex.at(i).x + j);//subMesh.weights.at(i).x weight start
        	Mesh::Joint& joint = mesh->joints.at(weight.weightJointID);

            // Convert the weight position from Joint local space to object space
            glm::vec3 rotPos = joint.jointOrient * weight.weightPos;

            position += (joint.jointPos + rotPos) * weight.weightBias;
        }

        subMesh->vertices.push_back(position);
    }
}
void MeshManager::prepareSubMeshNormals(Mesh* mesh, SubMesh* subMesh){
	subMesh->normals.clear();
	subMesh->normals.resize(subMesh->vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	subMesh->tangents.resize(subMesh->vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	subMesh->bitangents.resize(subMesh->vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	// Loop through all triangles and calculate the normal of each triangle
	for(unsigned int i = 0; i < subMesh->elements.size(); ++i ){
		glm::vec3 v0 = subMesh->vertices.at(subMesh->elements.at(i).x);
		glm::vec3 v1 = subMesh->vertices.at(subMesh->elements.at(i).y);
		glm::vec3 v2 = subMesh->vertices.at(subMesh->elements.at(i).z);

		glm::vec2 uv0 = subMesh->textureCoord.at(subMesh->elements.at(i).x);
		glm::vec2 uv1 = subMesh->textureCoord.at(subMesh->elements.at(i).y);
		glm::vec2 uv2 = subMesh->textureCoord.at(subMesh->elements.at(i).z);

		// Edges of the triangle : postion delta
	    glm::vec3 deltaPos1 = v1-v0;
	    glm::vec3 deltaPos2 = v2-v0;

	    // UV delta
	    glm::vec2 deltaUV1 = uv1-uv0;
	    glm::vec2 deltaUV2 = uv2-uv0;

		glm::vec3 normal = glm::cross(deltaPos1, deltaPos2);

		subMesh->normals.at(subMesh->elements.at(i).x) += normal;
		subMesh->normals.at(subMesh->elements.at(i).y) += normal;
		subMesh->normals.at(subMesh->elements.at(i).z) += normal;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;

		subMesh->tangents.at(subMesh->elements.at(i).x) += tangent;
		subMesh->tangents.at(subMesh->elements.at(i).y) += tangent;
		subMesh->tangents.at(subMesh->elements.at(i).z) += tangent;

		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;

		subMesh->bitangents.at(subMesh->elements.at(i).x) += bitangent;
		subMesh->bitangents.at(subMesh->elements.at(i).y) += bitangent;
		subMesh->bitangents.at(subMesh->elements.at(i).z) += bitangent;
	}
	for(unsigned int i = 0; i < subMesh->normals.size(); i++){
		subMesh->normals.at(i) = glm::normalize(subMesh->normals.at(i));
		subMesh->tangents.at(i) = glm::normalize(subMesh->tangents.at(i));
	}
	// Now normalize all the normals
/*	for (unsigned int i = 0; i < mesh.m_Verts.size(); ++i ){
		Vertex& vert = mesh.m_Verts[i];

		glm::vec3 normal = glm::normalize( vert.m_Normal );
		mesh.m_NormalBuffer.push_back( normal );

		// Reset the normal to calculate the bind-pose normal in joint space
		vert.m_Normal = glm::vec3(0);

		// Put the bind-pose normal into joint-local space
		// so the animated normal can be computed faster later
		for ( int j = 0; j < vert.m_WeightCount; ++j )
		{
			const Weight& weight = mesh.m_Weights[vert.m_StartWeight + j];
			const Joint& joint = m_Joints[weight.m_JointID];
			vert.m_Normal += ( normal * joint.m_Orient ) * weight.m_Bias;
		}
	}*/
}

Mesh* MeshManager::parseOBJ(string filename, char* buffer, int lenght){
//	logInf("parse OBJ file with name %s lenght %i", filename.c_str(),lenght);
    //logInf("buffer:\n%s",buffer);
	elementOffsetOBJ = 0;
    resultMesh = new Mesh(filename, getNewId());
    int idSubMesh = 0;
	SubMesh* auxSubMesh = new SubMesh(idSubMesh);
	int i = 0;
	while (buffer[i] != '\0')
	{
	switch (buffer[i]){
	case 'v': // Parse a vertex texture coordinate
		++i;
		if (buffer[i] == ' '){
			// Read in vertex definition
			getVector3(&(auxSubMesh->vertices), buffer, &i);
		}
		else if (buffer[i] == 't'){
			// Read in texture coordinate (2D)
			i++;
			getVector2(&(auxSubMesh->textureCoord), buffer, &i);
		}
		else if (buffer[i] == 'n'){
			// Read in normal vector definition
			i++;
			getVector3(&(auxSubMesh->normals), buffer, &i);
		}
		break;
	case 'f': // Parse a face
		i++;
		getFace(&(auxSubMesh->elements), buffer, &i);
		//skipLine(buffer, &i);
		break;
	case '#': // Parse a comment
		//getComment();
		skipLine(buffer, &i);
		break;
	case 'u': // Parse a material desc. setter
		//getMaterialDesc();
		skipLine(buffer, &i);
		break;
	case 'm': // Parse a material library
		//getMaterialLib();
		skipLine(buffer, &i);
		break;
	case 'g': // Parse group name
		//getGroupName();
		skipLine(buffer, &i);
		break;
	case 's': // Parse group number
		//getGroupNumber();
		skipLine(buffer, &i);
		break;
	case 'o': // Parse object name
		//getObjectName();
		if (idSubMesh != 0) {
			elementOffsetOBJ += auxSubMesh->vertices.size();
			resultMesh->subMeshes.push_back(auxSubMesh);
		}
		idSubMesh++;
	    auxSubMesh = new SubMesh(idSubMesh);
		skipLine(buffer, &i);
		break;
	default:
		skipLine(buffer, &i);
		break;
	}
	}

	resultMesh->subMeshes.push_back(auxSubMesh);

    for (int t = 0; t < resultMesh->subMeshes.size(); t++) {
        auxSubMesh = resultMesh->subMeshes.at(t);

        //logInf("calculating normals auxSubMesh->vertices.size() %lu", auxSubMesh->vertices.size());
		//logInf("calculating normals auxSubMesh->elements.size() %lu", auxSubMesh->elements.size());

        auxSubMesh->normals.resize(auxSubMesh->vertices.size(), glm::vec3(0.0, 0.0, 0.0));
        vector<int> nb_seen;
        nb_seen.clear();
        nb_seen.resize(auxSubMesh->vertices.size(), 0);
        for (int i = 0; i < auxSubMesh->elements.size(); i++) {
        	int ia, ib, ic;
        	glm::vec3 normal;
        	int current;
        	int v[3];
            ia = auxSubMesh->elements.at(i).x;
            ib = auxSubMesh->elements.at(i).y;
            ic = auxSubMesh->elements.at(i).z;
            //logInf("a %i, b %i, c %i",ia,ib,ic);
            normal = glm::normalize(glm::cross(
                        auxSubMesh->vertices.at(ib) - auxSubMesh->vertices.at(ia),
                        auxSubMesh->vertices.at(ic) - auxSubMesh->vertices.at(ia)));
            v[0] = ia;
            v[1] = ib;
            v[2] = ic;
            for (int j = 0; j < 3; j++) {
                current = v[j];
                //logInf("current %i", current);
                nb_seen[current]++;
                if(nb_seen[current] == 1){
                    auxSubMesh->normals[current] = normal;
                }else{
                    auxSubMesh->normals[current].x = auxSubMesh->normals[current].x * (1.0f - 1.0f/(float)nb_seen[current]) + normal.x * 1.0f/(float)nb_seen[current];
                    auxSubMesh->normals[current].y = auxSubMesh->normals[current].y * (1.0f - 1.0f/(float)nb_seen[current]) + normal.y * 1.0f/(float)nb_seen[current];
                    auxSubMesh->normals[current].z = auxSubMesh->normals[current].z * (1.0f - 1.0f/(float)nb_seen[current]) + normal.z * 1.0f/(float)nb_seen[current];
                    auxSubMesh->normals[current] = glm::normalize(auxSubMesh->normals[current]);
                }
            }
        }
    }

	return resultMesh;
}
void MeshManager::getNextWord(char* buffer, int* i){
    //logInf("i value %i",*i);
	while(isSeparator(buffer[*i]) && buffer[*i] != '\0'){//changed!
		(*i)++;
      //  logInf("getNextWord value %c at position %i", buffer[*i], (*i));
	}
/*	while(buffer[*i] == ' '){
		(*i)++;
	}*/
}

void MeshManager::skipLine(char* buffer, int* i){
	while(buffer[*i] != '\n' && buffer[*i] != '\0'){
		(*i)++;
	}
	if(buffer[*i] == '\n'){
		(*i)++;
	}
}
void MeshManager::getFace(std::vector<glm::i32vec3>* vertices, char* buffer, int* i){
	getNextWord(buffer, i);
	int x, y, z;
	copyNextWord(tempBuffer, BUFFERSIZE, buffer, i);
	x = (int) atoi(tempBuffer) - 1 - elementOffsetOBJ;

	copyNextWord(tempBuffer, BUFFERSIZE, buffer, i);
	y = (int) atoi(tempBuffer) - 1 - elementOffsetOBJ;

	copyNextWord(tempBuffer, BUFFERSIZE, buffer, i);
	z = (int) atoi(tempBuffer) - 1 - elementOffsetOBJ;

	vertices->push_back(glm::i32vec3(x, y, z));

	skipLine(buffer, i);
}

void MeshManager::getVector3(std::vector<glm::vec3>* vertices, char* buffer, int* i){
	getNextWord(buffer, i);
	float x, y, z;

	copyNextWord(tempBuffer, BUFFERSIZE, buffer, i);
	x = (float) fast_atof(tempBuffer);

	copyNextWord(tempBuffer, BUFFERSIZE, buffer, i);
	y = (float) fast_atof(tempBuffer);

	copyNextWord(tempBuffer, BUFFERSIZE, buffer, i);
	z = (float) fast_atof(tempBuffer);

	vertices->push_back(glm::vec3(x, y, z));

	skipLine(buffer, i);
}

void MeshManager::getVector2(std::vector<glm::vec2>* vertices, char* buffer, int* i){
	getNextWord(buffer, i);
	float x, y;

	copyNextWord(tempBuffer, BUFFERSIZE, buffer, i);
	x = (float) fast_atof(tempBuffer);

	copyNextWord(tempBuffer, BUFFERSIZE, buffer, i);
	y = (float) fast_atof(tempBuffer);

	vertices->push_back(glm::vec2(x, y));

	skipLine(buffer, i);
}


bool MeshManager::isSeparator(char current){
	if(current == ' ' || current == '\n' || current == '\r' || current == '\t' || current == '\0')
		return true;
	return false;
}
// -------------------------------------------------------------------
//	Copy the next word in a temporary buffer
void MeshManager::copyNextWord(char* tempBuffer, size_t length, char* buffer, int* i){
	size_t index = 0;
	getNextWord(buffer, i);
	while (!isSeparator(buffer[*i]))
	{
		tempBuffer[index] = buffer[*i];
		index++;
		if (index == length-1)
			break;
		(*i)++;
	}
	tempBuffer[index] = '\0';
}
// Remove the quotes from a string
void MeshManager::removeQuotes(string* str){
    size_t n;
    while ((n = str->find('\"')) != std::string::npos){
    	str->erase(n,1);
    }
}

// Computes the W component of the quaternion based on the X, Y, and Z components.
// This method assumes the quaternion is of unit length.
void MeshManager::computeQuaternionW(glm::quat* quat){
	float t = 1.0f - (quat->x * quat->x) - (quat->y * quat->y) - (quat->z * quat->z);
	if ( t < 0.0f ){
		quat->w = 0.0f;
	}
	else{
		quat->w = -sqrtf(t);
	}
}


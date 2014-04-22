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

#include "Shader.h"

Shader::Shader(){
	texture = 0;
	hasNormals = false;
	hasTangents = false;
	hasBitangents = false;
	hasUV = false;
}

Shader::~Shader(){
	freeResources();
}

void Shader::freeResources(){
    glUseProgram(0);
	glDeleteProgram(program);
}
/**
 * Display compilation errors from the OpenGL shader compiler
 */
void Shader::printShaderLog(GLuint object){
	GLint log_length = 0;
	if (glIsShader(object))
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else if (glIsProgram(object))
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	else {
		logErr("printlog: Not a shader or a program\n");
		return;
	}

	char* log = (char*)malloc(log_length);

	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, log);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, log);

	logErr("%s", log);
	free(log);
}

/**
 * Compile the shader from file 'filename', with error handling
 */
int Shader::loadShader(string fileName){
	GLint link_ok = GL_FALSE;
	GLuint vs, fs;

	string vsName = fileName+".v.glsl";
	string fsName = fileName+".f.glsl";

	if ((vs = createShader(vsName.c_str(), GL_VERTEX_SHADER))   == 0) return 0;
	if ((fs = createShader(fsName.c_str(), GL_FRAGMENT_SHADER)) == 0) return 0;
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		logWar("glLinkProgram:");
		printShaderLog(program);
        glDeleteShader(vs);
        glDeleteShader(fs);
		return -1;
	}
    glDeleteShader(vs);
    glDeleteShader(fs);
	loadVars();
    return 0;
}
/**
 * Compile the shader from file 'filename', with error handling
 */
GLuint Shader::createShader(const char* filename, GLenum type)
{
	if(FileSystem::getInstance()->openFile(filename) == -1){
		logErr("Error opening %s: ", filename);
		return 0;
	}
	const char* source = FileSystem::getInstance()->getFileData(filename);
	if (source == NULL) {
		logErr("Error data %s: ", filename);
		return 0;
	}

	//logInf("filename shader: %s",filename);
	GLuint res = glCreateShader(type);
	glShaderSource(res, 1, &source, NULL);

    FileSystem::getInstance()->destroyFileData(filename);
	glCompileShader(res);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
		logErr("%s:", filename);
		printShaderLog(res);
		glDeleteShader(res);
		return 0;
	}

    
	return res;
}

bool Shader::haveNormals(){
	return hasNormals;
}
bool Shader::haveTangents(){
	return hasTangents;
}
bool Shader::haveBitangents(){
	return hasBitangents;
}
bool Shader::haveUV(){
	return hasUV;
}

void Shader::useProgram(){
	glUseProgram(program);
}

void Shader::disableTextures(){
	//glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Shader::loadTexture(Texture* texture){
//virtual
}

#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
public:
	//Reference ID of the shader Program
	GLuint ID;

	//Constructor that build the shader program from 2 different shaders
	Shader(const char* vertexfile, const char* fragmentfile);

	//Activates the shader program
	void Activate();

	//Delates the shader Program
	void Delete();

private:
	void compileErrors(unsigned int shader, const char* tyhpe);
};

#endif
#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>

class VBO
{
public:
	//Reference ID of the vertex Buffer Object
	GLuint ID;

	//Constructor that generates a vertex buffer object and links it to vertices
	VBO(GLfloat* vertices, GLsizeiptr size);

	//binds the VBO
	void Bind();

	//Unbinds the VBO
	void Unbind();

	//delates the VBO
	void Delete();
};

#endif
#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include<glad/glad.h>

class EBO
{
public:
	//ID references of elements buffer objects
	GLuint ID;

	//Constructor that generates a element buffer object and links it to vertices
	EBO(GLuint* vertices, GLsizeiptr size);

	//binds the EBO
	void Bind();

	//Unbinds the EBO
	void Unbind();

	//Delete the EBO
	void Delete();
};

#endif
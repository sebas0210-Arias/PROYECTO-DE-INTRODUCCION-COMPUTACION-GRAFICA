#ifndef VAO_CLASS_H
#define VAOO_CLASS_H

#include<glad/glad.h>
#include"VBO.h"

class VAO
{
public:
	//Reference ID of the vertex array Object
	GLuint ID;

	//Constructor that generates a VAO ID
	VAO();

	// Links a VBO to the VAO USING A CERTAIN LAYOUT
	void LinkAttrib(VBO VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);

	//Binds the VAO
	void Bind();

	//Unbinds the VAO
	void Unbind();

	//Delates the VAO
	void Delete();
};

#endif

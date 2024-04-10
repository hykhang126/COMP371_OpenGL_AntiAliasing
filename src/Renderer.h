#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer {

public:
	// Constructors
	Renderer();
	~Renderer();

	// Renderer methods
	void drawWithIBO(VArray& va, IndexBuffer& ibo, Shader& shader, bool isSphere) const; // Not modifying
	void draw(VArray& va, GLuint& ibo, Shader& shader, bool isSphere) const;
	void clear();

};
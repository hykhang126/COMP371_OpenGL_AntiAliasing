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
	void draw(VArray& va, IndexBuffer& ibo, Shader& shader, bool isSphere) const; // Not modifying
	void clear();

};
#pragma once

#include <GL/glew.h>

#include "VArray.h"
#include "IBuffer.h"
#include "Shader.h"

class Renderer {

public:
	// Constructors
	Renderer();
	~Renderer();

	// Renderer methods
	void draw(VArray& va, IBuffer& ibo, Shader& shader, bool isSphere) const; // Not modifying
	void clear();

};
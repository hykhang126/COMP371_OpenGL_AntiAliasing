#pragma once

#include "VBuffer.h"

class VBufferLayout;

class VArray {

public:
	VArray();
	~VArray();

	void addBuffer(VBuffer& vbo, VBufferLayout& layout);

	void bind();
	void unbind();
private:
	unsigned int renderer_id;

};
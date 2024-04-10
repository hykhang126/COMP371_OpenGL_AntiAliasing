#pragma once

#include <vector>
#include "Renderer.h"

struct VBufferElem
{
	unsigned int type; // OpenGL type GLuint
	unsigned int count; // OpenGL type GLuint
	unsigned char normalized;
};

class VBufferLayout {

public:
	VBufferLayout();

	// Layout templates
	template<typename T>
	void add(unsigned int count);

	// Getters
	unsigned int getStride();
	std::vector<VBufferElem> getElems();
	int getTypeSize(GLenum type);

private:
	std::vector<VBufferElem> m_Elems;
	unsigned int m_Stride;

};

// Float layout
template<>
void VBufferLayout::add<float>(unsigned int count);

// Unsigned int layout
template<>
void VBufferLayout::add<unsigned int>(unsigned int count);

// Unsigned char layout
template<>
void VBufferLayout::add<unsigned char>(unsigned int count);
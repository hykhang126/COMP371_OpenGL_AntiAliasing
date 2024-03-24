#include "VBufferLayout.h"

// Constructors
VBufferLayout::VBufferLayout() : m_Stride(0)
{

}

// Layout templates
template<typename T>
void VBufferLayout::add(unsigned int count)
{

}

// Float layout
template<>
void VBufferLayout::add<float>(unsigned int count)
{
	m_Elems.push_back({ GL_FLOAT, count, GL_FALSE });
	m_Stride += getTypeSize(GL_FLOAT) * count;
}

// Unsigned int layout
template<>
void VBufferLayout::add<unsigned int>(unsigned int count)
{
	m_Elems.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
	m_Stride += getTypeSize(GL_UNSIGNED_INT) * count;
}

// Unsigned char layout
template<>
void VBufferLayout::add<unsigned char>(unsigned int count)
{
	m_Elems.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
	m_Stride += getTypeSize(GL_UNSIGNED_BYTE) * count;
}

// Getters
unsigned int VBufferLayout::getStride()
{
	return m_Stride;
}

std::vector<VBufferElem> VBufferLayout::getElems()
{
	return m_Elems;
}

int VBufferLayout::getTypeSize(GLenum type)
{
    switch (type) {
    case GL_BYTE:           return sizeof(GLbyte);    // 1 byte
    case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);   // 1 byte
    case GL_SHORT:          return sizeof(GLshort);   // 2 bytes
    case GL_UNSIGNED_SHORT: return sizeof(GLushort);  // 2 bytes
    case GL_INT:            return sizeof(GLint);     // 4 bytes
    case GL_UNSIGNED_INT:   return sizeof(GLuint);    // 4 bytes
    case GL_FLOAT:          return sizeof(GLfloat);   // 4 bytes
    case GL_DOUBLE:         return sizeof(GLdouble);  // 8 bytes
    default:
        // Handle unknown type
        return 0;
    }
}
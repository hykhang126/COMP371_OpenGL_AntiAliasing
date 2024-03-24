#include "IBuffer.h"
#include <GL/glew.h>

IBuffer::IBuffer(const unsigned int* data, unsigned int count) : count(count) {
    glGenBuffers(1, &renderer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_DYNAMIC_DRAW);
}

IBuffer::~IBuffer() {
    glDeleteBuffers(1, &renderer_id);
}

void IBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id);
}

void IBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
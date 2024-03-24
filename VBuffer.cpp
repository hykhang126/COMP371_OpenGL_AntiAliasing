#include "VBuffer.h"
#include "Renderer.h"

VBuffer::VBuffer(const void* data, unsigned int size) {
    glGenBuffers(1, &renderer_id);
    glBindBuffer(GL_ARRAY_BUFFER, renderer_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

VBuffer::~VBuffer() {
    glDeleteBuffers(1, &renderer_id);
}

void VBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, renderer_id);
}

void VBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBuffer::update(const void* data, unsigned int size)
{
    // Binds
    this->bind();

    // Updates
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);

    // Unbinds
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
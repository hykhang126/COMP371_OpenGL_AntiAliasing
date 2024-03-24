#include "VArray.h"
#include "VBufferLayout.h"
#include "Renderer.h"

// Constructors
VArray::VArray()
{
    glGenVertexArrays(1, &renderer_id);
}

// Destructors
VArray::~VArray()
{
    glDeleteVertexArrays(1, &renderer_id);
}

// Methods
void VArray::bind()
{
    glBindVertexArray(renderer_id);
}

void VArray::unbind()
{
    glBindVertexArray(0);
}

void VArray::addBuffer(VBuffer& vbo, VBufferLayout& layout)
{
    // Binds vertex array
    this->bind();

    // Binds vertex buffer
	vbo.bind();

    // Gets layout
    const auto& elems = layout.getElems();
    unsigned int offset = 0;

    // Specifies layout 
    for (unsigned int i = 0; i < elems.size(); i++)
    {
        const auto& elem = elems[i];

        // Sets attribues of VBO at index i
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(
            i,
            elem.count, // Parameters of vertex
            elem.type, // Type of vertex
            elem.normalized, // No normalize
            layout.getStride(), // Stride --> Amount to second vertex
            (const void*) offset // Offset of 0
        );

        offset += elem.count * layout.getTypeSize(elem.type);
    }
}
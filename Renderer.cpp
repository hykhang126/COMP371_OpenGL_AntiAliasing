#include "Renderer.h"

// Constructors
Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

void Renderer::clear()
{
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Renderer Methods
void Renderer::draw(VArray& va, IBuffer& ibo, Shader& shader, bool isSphere) const // Not modifying
{
    // Specifies shader
    shader.bind();

    // Binds vertex array and index buffer object
    va.bind();
    ibo.bind();

    // Draw primitives
    if (!isSphere)
        glDrawElements(GL_TRIANGLES, ibo.getCount(), GL_UNSIGNED_INT, nullptr);
    else
        glDrawElements(GL_TRIANGLE_STRIP, ibo.getCount(), GL_UNSIGNED_INT, nullptr);
}
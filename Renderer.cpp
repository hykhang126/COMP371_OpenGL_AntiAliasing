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
void Renderer::drawWithIBO(VArray& va, IndexBuffer& ibo, Shader& shader, bool isSphere) const // Not modifying
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

void Renderer::draw(VArray &va, GLuint &ibo, Shader &shader, bool isSphere) const
{
    // Specifies shader
    shader.bind();

    // Binds vertex array and index buffer object
    va.bind();

    // Draw primitives
    // draw the triangle
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

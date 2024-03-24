#include "Texture.h"

// Vendor include
#include "vendors/stb_image.h"

Texture::Texture(const std::string& fp)
    : fp(fp), renderer_id(0), textureBuffer(nullptr), width(0), height(0), bitsPerPixel(0) 
{
    // Flips texture 
    stbi_set_flip_vertically_on_load(1);

    // Loads texture data
    textureBuffer = stbi_load(fp.c_str(), &width, &height, &bitsPerPixel, 4);

    // returns n textures names in textures
    // Creates framebuffer with texture-based color attachment
    // and texture-based depth attachment
    glGenTextures(1, &renderer_id);

    // binds 2D texture buffer
    glBindBuffer(GL_TEXTURE_2D, renderer_id);

    // assign the values in parameters to the texture parameter specified
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Allows image array to be read by shaders
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer);

    // Unbinds texture array 
    glBindTexture(GL_TEXTURE_2D, 0);

    // Frees buffer if exists
    if (textureBuffer)
        stbi_image_free(textureBuffer);
}

Texture::~Texture() 
{
    // Deletes textures
    glDeleteTextures(1, &renderer_id);
}

void Texture::bind(unsigned int slot)
{
    // Sets slot "slot" to active texture
    glActiveTexture(GL_TEXTURE0 + slot);
    
    // Binds texture to slot 0
    glBindBuffer(GL_TEXTURE_2D, renderer_id);
}

void Texture::unbind() 
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture::getWidth() 
{
    return width;
}

int Texture::getHeight()
{
    return height;
}
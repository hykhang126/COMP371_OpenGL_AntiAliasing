#pragma once

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class AntiAliasing {
public:
    int WINDOW_WIDTH = 600, WINDOW_HEIGHT = 600;

    GLuint renderedTexture, textureColorBufferMultiSampled, FXAATexture;

    AntiAliasing() = default;

    AntiAliasing(int WINDOW_WIDTH, int WINDOW_HEIGHT, int FBO_WIDTH, int FBO_HEIGHT) {
        this->WINDOW_WIDTH = WINDOW_WIDTH;
        this->WINDOW_HEIGHT = WINDOW_HEIGHT;
        setupNone(FBO_WIDTH, FBO_HEIGHT);
    };

    // No anti-aliasing implementation
    GLuint setupNone(int width, int height);

    // MSAA setup
    GLuint setupMSAA(int width, int height);

    // FXAA implementation
    GLuint setupFXAA(int width, int height);

    // SMAA implementation
    GLuint setupSMAA(int width, int height);

    // Apply framebuffer
    static void applyFramebuffer(GLuint& framebuffer, int width, int height, 
                                float clearColor[4], bool isDepth = true);
};

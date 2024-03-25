#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class AntiAliasing {
public:
    AntiAliasing() = default;

    AntiAliasing(int WINDOW_WIDTH, int WINDOW_HEIGHT) {
        /* ! UNDER INVESTIGATION ! */
        // setupMSAA(WINDOW_WIDTH, WINDOW_HEIGHT);
    };

    // MSAA setup
    void setupMSAA(int WINDOW_WIDTH, int WINDOW_HEIGHT);

    // No anti-aliasing implementation
    void applyNone();

    // MSAA implementation
    void applyMSAA();

    // FXAA implementation
    void applyFXAA();

    // MSAA implementation
    void applySMAA();
};

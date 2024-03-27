#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <sstream>
#include <Eigen/Core>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <filesystem>
#include "Cloth.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "AntiAliasing.h"

#define ANTI_ALIASING

static std::string getCurrentPath();

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    int WINDOW_WIDTH = 1920;
    int WINDOW_HEIGHT = 1080;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Anti-Aliasing", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    glewInit();

    // Blending enabled
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Z-buffer
    glEnable(GL_DEPTH_TEST);

    // Creates VAO
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Simulation Setup
    Cloth cloth;

    // Array of particles
    std::vector<Particle> particles = cloth.initializeClothVertexArray(10, 10);

    // Creates vertex array
    VArray va;
    
    // Creates vertex buffer
    VBuffer currentBuffer(particles.data(), particles.size() * sizeof(Particle));

    // Creates vertex buffer layout
    VBufferLayout lo;

    // Specifies new layout and adds vertex coordinates
    lo.add<float>(3);

    // Texture coordinates
    lo.add<float>(2);

    // Normal Coordinates
    lo.add<float>(3);

    // Attaches vertex buffer and vertex buffer layout
    // to the vertex array
    va.addBuffer(currentBuffer, lo);

    // Cloth indices
    std::vector<unsigned int> particleIndices = cloth.generateIndices(10, 10);

    // Creates index buffer layout
    IndexBuffer ibo(particleIndices.data(), particleIndices.size());

    std::string src = getCurrentPath();

    // Creates shader off of input file
    Shader shader(src + "/shaders/Basic.shader");

    // Binds shader to program
    shader.bind();

    // Sets known shader uniform
    shader.setUniform4f("u_Color", 0.2f, 0.5f, 0.4f, 1.0f);

    // Aspect ratio modifier
    float ar = (float)WINDOW_WIDTH / WINDOW_HEIGHT;

    // Creates texture buffer and binds to slot 0
    Texture texture(src + "/textures/basketball.png");
    texture.bind(0);

    // Sets uniform location to texture at slot 0
    shader.setUniform1i("u_Texture", 0);

    shader.unbind();
    va.unbind();
    currentBuffer.unbind();
    ibo.unbind();

    Renderer renderer;

    Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, -0.5f, 4.0f));

#ifdef ANTI_ALIASING
    AntiAliasing aa (WINDOW_WIDTH, WINDOW_HEIGHT);
#endif

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Clear renderer
        renderer.clear();

        glm::mat4 viewMat = camera.mat(45.0, 0.1f, 100.0f);

        // Camera polling
        camera.inputs(window);
        camera.inputs_AA(window);

        // Specifies shader
        shader.bind();

        // Sets known shader uniform
        shader.setUniform4f("u_Color", 0.3f, 0.5f, 0.4f, 1.0f);

        // Sets projection matrix uniform
        shader.setUniformMat4f("u_Camera", viewMat);

        // Bidirectional Lighting ON (Cloth)
        shader.setUniform1i("is_Bidirectional", 1);

        // Draws triangles
        renderer.draw(va, ibo, shader, false);

        // // Bidirectional Lighting OFF (Sphere)
        // shader.setUniform1i("is_Bidirectional", 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

static std::string getCurrentPath() {
    // Get the path of the current source file being compiled
    std::filesystem::path currentFilePath(__FILE__);

    // Extract the directory from the file path
    return currentFilePath.parent_path().string();
}

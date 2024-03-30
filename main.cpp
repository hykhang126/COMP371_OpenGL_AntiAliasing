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
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Settings
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

// camera
Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, -0.5f, 4.0f));
float lastX = (float)WINDOW_WIDTH / 2.0;
float lastY = (float)WINDOW_HEIGHT / 2.0;
bool firstMouse = true;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_WIDTH, "Anti-Aliasing", NULL, NULL);
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

    // // Simulation Setup
    // Cloth cloth;

    // // Array of particles
    // std::vector<Particle> particles = cloth.initializeClothVertexArray(10, 10);

    // // Creates vertex array
    // VArray va;
    
    // // Creates vertex buffer
    // VBuffer currentBuffer(particles.data(), particles.size() * sizeof(Particle));

    // // Creates vertex buffer layout
    // VBufferLayout lo;

    // // Specifies new layout and adds vertex coordinates
    // lo.add<float>(3);

    // // Texture coordinates
    // lo.add<float>(2);

    // // Normal Coordinates
    // lo.add<float>(3);

    // // Attaches vertex buffer and vertex buffer layout
    // // to the vertex array
    // va.addBuffer(currentBuffer, lo);

    // // Cloth indices
    // std::vector<unsigned int> particleIndices = cloth.generateIndices(10, 10);

    // // Creates index buffer layout
    // IndexBuffer ibo(particleIndices.data(), particleIndices.size());

    std::string src = getCurrentPath();

    // Creates shader off of input file
    Shader shader(src + "/shaders/Basic.shader");

    // Binds shader to program
    shader.bind();

    // Sets known shader uniform
    shader.setUniform4f("u_Color", 0.2f, 0.5f, 0.4f, 1.0f);

    // Creates texture buffer and binds to slot 0
    Texture texture(src + "/textures/basketball.png");
    texture.bind(0);

    // Sets uniform location to texture at slot 0
    shader.setUniform1i("u_Texture", 0);

    float cubeVertices[] = {
        // positions       
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
    };

    // setup cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // shader.unbind();
    // va.unbind();
    // currentBuffer.unbind();
    // ibo.unbind();

    Renderer renderer;

    // timing
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

#ifdef ANTI_ALIASING
    AntiAliasing aa (WINDOW_WIDTH, WINDOW_HEIGHT);
#endif

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);   

        glm::mat4 viewMat = camera.mat(45.0, 0.1f, 100.0f);

        // Specifies shader
        shader.bind();

        // Sets known shader uniform
        shader.setUniform4f("u_Color", 0.3f, 0.5f, 0.4f, 1.0f);

        // Sets projection matrix uniform
        shader.setUniformMat4f("u_Camera", viewMat);

        // Bidirectional Lighting ON (Cloth)
        shader.setUniform1i("is_Bidirectional", 1);

        // // Draws triangles
        // renderer.draw(va, ibo, shader, false);

        // // Bidirectional Lighting OFF (Sphere)
        // shader.setUniform1i("is_Bidirectional", 0);

        // Camera polling
        camera.inputs(window);
        camera.inputs_AA(window);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
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

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

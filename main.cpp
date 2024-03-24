#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// #include <imgui.h>
// #include <imgui_impl_glfw.h>
// #include <imgui_impl_opengl3.h>
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
#include "VBuffer.h"
#include "VBufferLayout.h"
#include "IBuffer.h"
#include "VArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

#define WITH_SPHERE
#define FIXED_CORNERS
#define ADD_SHEAR_SPRINGS
#define ADD_BEND_SPRINGS

static std::string getCurrentPath();

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    int WINDOW_WIDTH, WINDOW_HEIGHT;
    WINDOW_WIDTH = 1920;
    WINDOW_HEIGHT = 1080;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cloth Simulation", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    glewInit();

    {
        // Blending enabled
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Z-buffer
        glEnable(GL_DEPTH_TEST);

        // Creates VAO
        unsigned int vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Simulation Setup
        Cloth cloth;

        int CLOTH_WIDTH = 10;
        int CLOTH_HEIGHT = 10;

        //*** Function Placeholder ***//
        std::vector<Particle> sphereVertices;

#ifdef WITH_SPHERE
        // Programmatic Vertex and Index positions referenced from
        // https://learnopengl.com/code_viewer_gh.php?code=src/6.pbr/1.2.lighting_textured/lighting_textured.cpp

        // *** Multiple Object Test *** //
        std::vector<unsigned int> sphereIndices;

        const unsigned int X_SEGMENTS = 32;
        const unsigned int Y_SEGMENTS = 32;
        const float PI = 3.14159265359f;

        // Vertices
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                // Sets position and texture coordinates
                Particle sphereParticle;
                sphereParticle.position = glm::vec3((xPos / 2.0f) + 0.5f, (yPos / 2.0f) - 1.0f, (zPos / 2.0f) - 1.0f);
                sphereParticle.textureCoordinates = glm::vec2(xSegment, ySegment);

                // Calculate normal (direction from the center of the sphere to the vertex)
                glm::vec3 normal = glm::normalize(sphereParticle.position - glm::vec3(0.5f, -0.5f, -1.0f)); // Center of the sphere
                sphereParticle.normal = normal;

                sphereVertices.push_back(sphereParticle);
            }
        }

        // Indices
        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    sphereIndices.push_back(y * (X_SEGMENTS + 1) + x);
                    sphereIndices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    sphereIndices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    sphereIndices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        // ***  *** //
#endif

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

#ifdef WITH_SPHERE
        //*** TESTING ***//
        VArray va2;

        VBuffer sphere(sphereVertices.data(), sphereVertices.size() * sizeof(Particle));

        va2.addBuffer(sphere, lo);

        IBuffer ibo2(sphereIndices.data(), sphereIndices.size());
        // *** *** //
#endif

        // Attaches vertex buffer and vertex buffer layout
        // to the vertex array
        va.addBuffer(currentBuffer, lo);

        // Cloth indices
        std::vector<unsigned int> particleIndices = cloth.generateIndices(10, 10);

        // Creates index buffer layout
        IBuffer ibo(particleIndices.data(), particleIndices.size());

        //******* Spring instantiations *******// 

        // Spring Constants
        float ks_STRETCH = 0.75f;
        float kd_STRETCH = -0.25f;
        float ks_SHEAR = 0.75f;
        float kd_SHEAR = -0.25f;
        float ks_BEND = 0.95f;
        float kd_BEND = -0.25f;

        // Springs initialization
        std::vector<Spring> springs;

        // Stretch Springs //

        // --> Horizontal: Adds 90 horizontal stretch springs
        for (int i = 0; i < particles.size() - 1; i += 10)
        {
            for (int j = i; j < i + 9; j++)
                springs.push_back(cloth.initializeSpring(particles[j], particles[j + 1], ks_STRETCH, kd_STRETCH));
        }
        // --> Vertical: Adds 90 vertical stretch springs
        for (int i = 0; i < particles.size() - 10; i += 10)
        {
            for (int j = i; j < i + 10; j++)
                springs.push_back(cloth.initializeSpring(particles[j], particles[j + 10], ks_STRETCH, kd_STRETCH));
        }

#ifdef ADD_SHEAR_SPRINGS
        // Shear Springs //
        // --> Horizontal: Adds 81 (bottom left to top right) shear springs
        for (int i = 0; i < particles.size() - 10; i += 10)
        {
            for (int j = i; j < i + 9; j++)
                springs.push_back(cloth.initializeSpring(particles[j], particles[j + 11], ks_SHEAR, kd_SHEAR));
        }
        // --> Vertical: Adds 81 (bottom right to top left) left shear springs
        for (int i = 0; i < particles.size() - 10; i += 10)
        {
            for (int j = i + 1; j < i + 10; j++)
                springs.push_back(cloth.initializeSpring(particles[j], particles[j + 9], ks_SHEAR, kd_SHEAR));
        }
#endif

#ifdef ADD_BEND_SPRINGS
        // Bend Springs //
// --> Horizontal: Adds 40 horizontal bend springs
        for (int i = 0; i < particles.size() - 1; i += 10)
        {
            for (int j = i; j < i + 8; j += 2)
                springs.push_back(cloth.initializeSpring(particles[j], particles[j + 2], ks_BEND, kd_BEND));
        }
        // --> Vertical: Adds 40 vertical bend springs
        for (int i = 0; i < particles.size() - 20; i += 20)
        {
            for (int j = i; j < i + 10; j++)
                springs.push_back(cloth.initializeSpring(particles[j], particles[j + 20], ks_BEND, kd_BEND));
        }
#endif

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
        Texture texture(src + "/textures/cloth.png");
        texture.bind(0);

        // Sets uniform location to texture at slot 0
        shader.setUniform1i("u_Texture", 0);

        shader.unbind();
        va.unbind();
        currentBuffer.unbind();
        ibo.unbind();

#ifdef WITH_SPHERE
        va2.unbind();
        sphere.unbind();
        ibo2.unbind();
#endif

        Renderer renderer;

#ifdef WITH_SPHERE
        Renderer renderer2;
#endif

        // IMGUI_CHECKVERSION();
        // ImGui::CreateContext();
        // ImGui::StyleColorsDark();
        // ImGui_ImplGlfw_InitForOpenGL(window, true);
        // ImGui_ImplOpenGL3_Init("#version 330");

        Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, -0.5f, 4.0f));

        // Initialize velocities and forces
        std::map<Particle*, glm::vec3> velocities;
        std::map<Particle*, glm::vec3> forces;

        // Map for velocities and forces at particle addresses
        // Constant time access?
        for (int i = 0; i < particles.size(); i++)
        {
            glm::vec3 zeroes(0);
            velocities[&particles[i]] = zeroes;
            forces[&particles[i]] = zeroes;
        }

        // Wireframe
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Changeables
        float wind = 0.0f;
        float mass = 0.5f;
        bool isFixed = true;

        // Bidirectional Lighting
        shader.setUniform1i("is_Bidirectional", 1);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            // Clear renderer
            renderer.clear();

#ifdef WITH_SPHERE
            renderer2.clear();
#endif

            // ImGui_ImplOpenGL3_NewFrame();
            // ImGui_ImplGlfw_NewFrame();
            // ImGui::NewFrame();

            glm::mat4 viewMat = camera.mat(45.0, 0.1f, 100.0f);

            // Simulation Step
            cloth.calculateForces(particles, velocities, forces, springs, wind, sphereVertices); // --> sums all forces external and internal on particles

            if (isFixed)
            {
#ifdef FIXED_CORNERS
                //*** Remove forces from top 2 particles ***//
                glm::vec3 zeroes(0);
                velocities[&particles[99]] = zeroes;
                velocities[&particles[90]] = zeroes;
                forces[&particles[99]] = zeroes;
                forces[&particles[90]] = zeroes;
                //***  ***//
#endif
            }

            cloth.semiImplicit(particles, velocities, forces, mass); // --> semi implicit euler integration
            cloth.inverseSpringForce(particles, velocities, forces, springs); // --> checks springs rest length, applies opposite forces to particles proportionally

            currentBuffer.update(particles.data(), particles.size() * sizeof(Particle));

#ifdef WITH_SPHERE
            sphere.update(sphereVertices.data(), sphereVertices.size() * sizeof(Particle));
#endif

            // Camera shit
            camera.inputs(window);

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

            // Bidirectional Lighting OFF (Sphere)
            shader.setUniform1i("is_Bidirectional", 0);

#ifdef WITH_SPHERE
            renderer2.draw(va2, ibo2, shader, true);
#endif

            {
                // ImGui::Begin("Simulation Attributes", nullptr, ImGuiWindowFlags_NoMove);
                // ImGui::SliderFloat("Wind Intensity", &wind, 0.0f, 0.5f);
                // ImGui::SliderFloat("Change Particle Mass", &mass, 0.1f, 0.5f);
                // ImGui::Checkbox("Unfix Corners", &isFixed);
                // ImGui::End();
            }

            // ImGui::Render();
            // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        // ImGui_ImplOpenGL3_Shutdown();
        // ImGui_ImplGlfw_Shutdown();
        // ImGui::DestroyContext();

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
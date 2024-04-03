﻿#define GLM_ENABLE_EXPERIMENTAL

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
#define RENDER_TO_TEXTURE
#define MY_SHADER



static std::string getCurrentPath();
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

struct Particle {
    glm::vec3 position;
    glm::vec2 textureCoordinates;
};

float triVertices[] = {
    // positions      // texture Coords    
    -0.5f, -0.0f,       0.5f, 0.0f, // middle left
    0.5f, -0.5f,        1.0f, 0.0f, // bottom right
    0.5f, 0.5f,         1.0f, 1.0f, // top right

};

// // Indices for vertices order
// unsigned int triIndices[] =
// {
//     0, 1, 2, // Lower triangle
//     2, 3, 0 // Upper triangle
// };

// Settings
int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 600;

int FBO_WIDTH = 15;
int FBO_HEIGHT = 15;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// camera
Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 4.0f));

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_WIDTH, "Anti-Aliasing", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSwapInterval(1);

    glewInit();

    // Blending enabled
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Z-buffer
    glEnable(GL_DEPTH_TEST);

    // Wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    std::string src = getCurrentPath();

#ifdef MY_SHADER
    // // Creates shader off of input file
    // Shader shader(src + "/shaders/Basic.shader");
    // build and compile our shader program
    // ------------------------------------
    Shader shader( src + "/shaders/Simple.vertex", src + "/shaders/Simple.frag" ); 
#endif

    // // Binds shader to program
    // shader.bind();

    // glm::mat4 viewMat = camera.mat(45.0f, 0.1f, 100.0f);

    // // Sets projection matrix uniform
    // shader.setUniformMat4f("u_Camera", viewMat);

    // // Creates texture buffer and binds to slot 0
    // Texture texture(src + "/textures/basketball.png");
    // texture.bind(0);

    // // Creates vertex array
    // VArray va;

    // // Creates vertex buffer
    // VBuffer vb(triVertices, sizeof(triVertices) * sizeof(float));

    // // Creates vertex buffer layout
    // VBufferLayout layout;

    // // Specifies new layout and adds vertex coordinates
    // layout.add<float>(2);

    // // Attaches vertex buffer and vertex buffer layout
    // // to the vertex array
    // va.addBuffer(vb, layout);

    // // Creates index buffer layout
    // IndexBuffer ibo( triIndices, sizeof(triIndices));

    // // Create renderer
    // Renderer renderer;


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // The fullscreen quad's FBO
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    // setup tri VAO
    GLuint triVAO, triVBO;
    glGenVertexArrays(1, &triVAO);
    glGenBuffers(1, &triVBO);
    glBindVertexArray(triVAO);
    glBindBuffer(GL_ARRAY_BUFFER, triVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), &triVertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);  

    // screen quad VAO
    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // texture coord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // framebuffer configuration
    // -------------------------
#ifdef RENDER_TO_TEXTURE
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint FramebufferName;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // The texture we're going to render to
    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, FBO_WIDTH, FBO_HEIGHT, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

    // // Set the list of draw buffers.
    // GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    // glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;

    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    #ifndef MY_SHADER
        // Create and compile our GLSL program from the shaders
        GLuint quad_programID = Shader::LoadShaders( src + "/shaders/Simple.vertex", src + "/shaders/FXAA.frag" );
        GLuint texID = glGetUniformLocation(quad_programID, "renderedTexture");
        GLuint timeID = glGetUniformLocation(quad_programID, "time");
    #else
        // std::cout << "Using my shader with a rendered texture" << std::endl;
        // shader.setUniform1i("screenTexture", 0);
    #endif
#endif




#ifdef ANTI_ALIASING
    AntiAliasing aa (WINDOW_WIDTH, WINDOW_HEIGHT);
    // Do something
#endif




    // ---------------------
    // render loop
    // ---------------------
    while (!glfwWindowShouldClose(window))
    {
        // renderer.clear();
        // // Draws triangles
        // renderer.draw(va, ibo, shader, false);

        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

#ifdef MY_SHADER
        // Generates view matrix
        glm::mat4 viewMat = camera.mat(camera.Zoom, 0.1f, 100.0f);

        // Specifies shader
        shader.bind();

        // Sets projection matrix uniform
        shader.setUniformMat4f("u_Camera", viewMat);
#endif

        // render
        // ------

        // bind to framebuffer and draw scene as we normally would to color texture 
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

        // make sure we clear the framebuffer's content
        glClearColor(0.1f, 0.1f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render the triangle
        glBindVertexArray(triVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);


        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        // glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
        //                     GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        // Camera polling
        camera.inputs(window);
        camera.inputs_AA(window);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}





/*--------------------------------------------------------------*/


// Returns the current path of the file being compiled
static std::string getCurrentPath() {
    // Get the path of the current source file being compiled
    std::filesystem::path currentFilePath(__FILE__);

    // Extract the directory from the file path
    return currentFilePath.parent_path().string();
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
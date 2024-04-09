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

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "AntiAliasing.h"

// Vendor include
#include "vendors/stb_image.h"

#define ANTI_ALIASING
#define MY_SHADER
#define MSAA_FXAA



static std::string getCurrentPath();
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
unsigned int loadTexture(std::string& path);

struct Particle {
    glm::vec3 position;
    glm::vec2 textureCoordinates;
};

// The fullscreen quad's FBO
float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -3.0f,  1.0f,  0.0f, 1.0f,
    -3.0f, -1.0f,  0.0f, 0.0f,
    -1.0f, -1.0f,  1.0f, 0.0f,

    -3.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f, 1.0f
};

// The low res quad's FBO
float lr_quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
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
int WINDOW_WIDTH = 900;
int WINDOW_HEIGHT = 900;

int FBO_WIDTH = 15;
int FBO_HEIGHT = 15;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// camera
Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 4.0f));

// Path to the source directory
std::string src = getCurrentPath();

// Clears the screen and sets the background color
float clearColorBlack[4] = {0.1f, 0.1f, 0.1f, 1.0f};
float clearColorWhite[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float clearColorRed[4] = {1.0f, 0.0f, 0.0f, 1.0f};
float clearColorGreen[4] = {0.0f, 1.0f, 0.0f, 1.0f};
float clearColorBlue[4] = {0.0f, 0.0f, 1.0f, 1.0f};

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window;

#ifdef ANTI_ALIASING
    glfwWindowHint(GLFW_SAMPLES, 4);
#endif

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

    // // Blending enabled
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Z-buffer
    glEnable(GL_DEPTH_TEST);
#ifdef ANTI_ALIASING
    glEnable(GL_MULTISAMPLE); 
#endif

    // // Wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

#ifdef MY_SHADER
    // // Creates shader off of input file
    // Shader shader(src + "/shaders/Basic.shader");

    // build and compile our shader program
    // ------------------------------------
    // Shader for triangles
    Shader shader( src + "/shaders/Simple.vs", src + "/shaders/Simple.fs" );
    shader.bind();
    shader.setUniform1i("screenTexture", 0);

    // Shader for MSAA
    Shader MSAA_shader( src + "/shaders/MSAA.vs", src + "/shaders/MSAA.fs" );
    MSAA_shader.bind();

    // Shader for FXAA
    Shader FXAA_Shader( src + "/shaders/Simple.vs", src + "/shaders/FXAA_2.fs" );
    FXAA_Shader.bind();
    FXAA_Shader.setUniform1i("u_colorTexture", 0);
    FXAA_Shader.setUniform2f("u_texelStep", 1.0f / FBO_WIDTH, 1.0f / FBO_HEIGHT);
    /*
    uniform float u_lumaThreshold;
    uniform float u_mulReduce;
    uniform float u_minReduce;
    uniform float u_maxSpan;
    */
    FXAA_Shader.setUniform1f("u_lumaThreshold", 0.125f);
    FXAA_Shader.setUniform1f("u_mulReduce", (1.0f/8.0f));
    FXAA_Shader.setUniform1f("u_minReduce", (1.0f/128.0f));
    FXAA_Shader.setUniform1f("u_maxSpan", 1.0f);

    FXAA_Shader.setUniform1i("u_fxaaOn", true);

    // Shader for screen
    Shader screenShader( src + "/shaders/Screen.vs", src + "/shaders/Screen.fs" ); 
    screenShader.bind();
    screenShader.setUniform1i("screenTexture", 0);
#endif

    // load textures
    // -------------
    std::filesystem::path filePath = src + "/textures/basketball.png";
    std::string path = filePath.string();
    unsigned int triTexture = loadTexture(path);
    filePath = src + "/textures/quad.png";
    path = filePath.string();
    unsigned int quadTexture = loadTexture(path);


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // setup tri VAO
    /*
    THE ORDER OF VAO: NO ANTI-ALIASING 0, MSAA 1, FXAA 2
    */
    const int size = 3;
    float offset = triVertices[4] - triVertices[0] + 0.5f;

    GLuint triVAO[size], triVBO[size];
    glGenVertexArrays(size, triVAO);
    glGenBuffers(size, triVBO);

    for (int i = 0; i < size; i++)
    {
        float newVertices[sizeof(triVertices)];
        std::copy(triVertices, triVertices + sizeof(triVertices) / sizeof(float), newVertices);
        for (int j = 0; j < sizeof(triVertices) / sizeof(float); j++)
        {
            if (j % 4 == 0)
            {
                newVertices[j] = triVertices[j] + i * offset;
            }
        }   

        glBindVertexArray(triVAO[i]);

        glBindBuffer(GL_ARRAY_BUFFER, triVBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(newVertices), &newVertices, GL_STATIC_DRAW);
        // position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);  
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);  
    }  

    // Set up the low res quad VAO
    GLuint lr_quadVAO, lr_quadVBO;
    glGenVertexArrays(1, &lr_quadVAO);
    glGenBuffers(1, &lr_quadVBO);
    glBindVertexArray(lr_quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lr_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lr_quadVertices), &lr_quadVertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);  


    // setup screen quad VAO
    offset = quadVertices[8] - quadVertices[0] + 0.1f;
    float xAdjust = 0.1f;

    GLuint quadVAO[size], quadVBO[size];
    glGenVertexArrays(size, quadVAO);
    glGenBuffers(size, quadVBO);

    for (int i = 0; i < size; i++)
    {
        float newQuadVertices[sizeof(quadVertices)];
        std::copy(quadVertices, quadVertices + sizeof(quadVertices) / sizeof(float), newQuadVertices);
        for (int j = 0; j < sizeof(quadVertices) / sizeof(float); j++)
        {
            if (j % 4 == 0)
            {
                newQuadVertices[j] = (quadVertices[j] + i * offset - xAdjust) / (size + xAdjust);
            }
            else if ((j-1) % 4 == 0)
            {
                newQuadVertices[j] = quadVertices[j] / (size + xAdjust);
            }
        }   

        glBindVertexArray(quadVAO[i]);

        glBindBuffer(GL_ARRAY_BUFFER, quadVBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(newQuadVertices), &newQuadVertices, GL_STATIC_DRAW);
        // position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);  
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);  
    } 


    // Anti aliasing configuration by returning framebuffer ref.
    // -------------------------
#ifdef ANTI_ALIASING
    AntiAliasing aa (WINDOW_WIDTH, WINDOW_HEIGHT);

    // None setup
    GLuint NoAAframebuffer = aa.setupNone(FBO_WIDTH, FBO_HEIGHT);
    // MSAA setup
    GLuint MSAAframebuffer = aa.setupMSAA(FBO_WIDTH, FBO_HEIGHT);
    // FXAA setup
    GLuint FXAAframebuffer = aa.setupFXAA(FBO_WIDTH, FBO_HEIGHT);
#endif

#ifdef MSAA_FXAA
    // configure second post-processing framebuffer
    unsigned int intermediateFBO;
    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
    // create a color attachment texture in lower res context
    unsigned int MSAATexture;
    glGenTextures(1, &MSAATexture);
    glBindTexture(GL_TEXTURE_2D, MSAATexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FBO_WIDTH, FBO_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, MSAATexture, 0);	// we only need a color buffer

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif





    // ---------------------
    // render loop
    // ---------------------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;



        // render
        // -----------------------------------------
        // Generates view matrix
        glm::mat4 viewMat = camera.mat(camera.Zoom, 0.1f, 100.0f);

        for (int i = 0; i < size; i++)
        {
            // 0. None 1. MSAA 2. FXAA
            if (i == 0)
            {
                // Specifies shader
                shader.bind();
                // Sets projection matrix uniform
                shader.setUniformMat4f("u_Camera", viewMat);
                // Bind to the corresponding framebuffer
                aa.applyFramebuffer(NoAAframebuffer, FBO_WIDTH, FBO_HEIGHT, clearColorWhite, true);
                // render the triangle
                glBindVertexArray(triVAO[0]);
                // binds texture to slot 0
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, triTexture);

                // draw the triangle
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
            if (i == 1)
            {
                MSAA_shader.bind();
                MSAA_shader.setUniformMat4f("u_Camera", viewMat);
                aa.applyFramebuffer(MSAAframebuffer, FBO_WIDTH, FBO_HEIGHT, clearColorWhite, true);

                glBindVertexArray(triVAO[0]);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, triTexture);

                // 2. now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
                glBindFramebuffer(GL_READ_FRAMEBUFFER, MSAAframebuffer);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
                glBlitFramebuffer(0, 0, FBO_WIDTH, FBO_HEIGHT, 
                                0, 0, FBO_WIDTH, FBO_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
            if (i == 2)
            {
                FXAA_Shader.bind();
                FXAA_Shader.setUniformMat4f("u_Camera", viewMat);
                aa.applyFramebuffer(FXAAframebuffer, FBO_WIDTH, FBO_HEIGHT, clearColorWhite, true);

                glBindVertexArray(triVAO[0]);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, triTexture);

                glDrawArrays(GL_TRIANGLES, 0, 3);
            }

            if (i != 1)
            {
                // DRAW QUAD
                glBindVertexArray(lr_quadVAO);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, quadTexture);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }

            // RESET
            glBindVertexArray(0);
        }

        // Specifies screen shader
        screenShader.bind();

        // 3. now render quad with scene's visuals as its texture image
        GLuint DefaultFramebuffer = 0;
        aa.applyFramebuffer(DefaultFramebuffer, WINDOW_WIDTH, WINDOW_HEIGHT, clearColorBlack, false);

        for (int i = 0; i < size; i++)
        {
            glBindVertexArray(quadVAO[i]);
            glActiveTexture(GL_TEXTURE0);
            if (i == 0)
            {
                glBindTexture(GL_TEXTURE_2D, aa.renderedTexture);	// use the color attachment texture as the texture of the quad plane
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            else if (i == 1)
            {
                glBindTexture(GL_TEXTURE_2D, MSAATexture);	// use the color attachment texture as the texture of the quad plane
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            else if (i == 2)
            {
                glBindTexture(GL_TEXTURE_2D, aa.FXAATexture);	// use the color attachment texture as the texture of the quad plane
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }

        }


        
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

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(std::string& path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
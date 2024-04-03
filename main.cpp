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
Camera camera(WINDOW_WIDTH *2, WINDOW_HEIGHT *2, glm::vec3(0.0f, 0.0f, 4.0f));
float lastX = (float)WINDOW_WIDTH / 2.0;
float lastY = (float)WINDOW_HEIGHT / 2.0;
bool firstMouse = true;

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

    // // Wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
    // VBuffer vb(cubeVertices, sizeof(cubeVertices) * sizeof(float));

    // // Creates vertex buffer layout
    // VBufferLayout layout;

    // // Specifies new layout and adds vertex coordinates
    // layout.add<float>(2);

    // // Attaches vertex buffer and vertex buffer layout
    // // to the vertex array
    // va.addBuffer(vb, layout);

    // // Creates index buffer layout
    // IndexBuffer ibo( cubeIndices, sizeof(cubeIndices));

    // // Create renderer
    // Renderer renderer;

#ifdef RENDER_TO_TEXTURE
    // RENDER TO TEXTURE
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint FramebufferName = 0;
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
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;

    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    // glViewport(0,0,FBO_WIDTH,FBO_HEIGHT); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    // The fullscreen quad's FBO
    GLuint quad_VertexArrayID;
    glGenVertexArrays(1, &quad_VertexArrayID);
    glBindVertexArray(quad_VertexArrayID);

    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };

    GLuint quad_vertexbuffer;
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

    #ifndef MY_SHADER
        // Create and compile our GLSL program from the shaders
        GLuint quad_programID = Shader::LoadShaders( src + "/shaders/Simple.vertex", src + "/shaders/FXAA.frag" );
        GLuint texID = glGetUniformLocation(quad_programID, "renderedTexture");
        GLuint timeID = glGetUniformLocation(quad_programID, "time");
    #else
        std::cout << "Using my shader with a rendered texture" << std::endl;
        shader.setUniform1i("screenTexture", 0);
    #endif
    
    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

#endif

    // setup cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), &triVertices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);  





#ifdef ANTI_ALIASING
    AntiAliasing aa (WINDOW_WIDTH, WINDOW_HEIGHT);
    // Do something
#endif




    // GLuint quad_programID = Shader::LoadShaders( src + "/shaders/Passthrough.vertex", src + "/shaders/firstpass.frag" );
    // Shader shader;
    // shader.setShaderProgram(quad_programID);

    // Texture renderToText;



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

        // // Sets known shader uniform
        // shader.setUniform4f("u_Color", 0.3f, 0.5f, 0.4f, 1.0f);

        // Sets projection matrix uniform
        shader.setUniformMat4f("u_Camera", viewMat);

        // // Bidirectional Lighting ON (Cloth)
        // shader.setUniform1i("is_Bidirectional", 1);
#endif

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

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
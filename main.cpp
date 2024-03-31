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


static std::string getCurrentPath();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

struct Particle {
    glm::vec3 position;
    glm::vec2 textureCoordinates;
};

float cubeVertices[] = {
    // positions       
    -0.5f, -0.0f,
    0.5f, -0.5f,
    0.5f, 0.5f,

};

// Indices for vertices order
unsigned int cubeIndices[] =
{
    0, 1, 2, // Lower triangle
    2, 3, 0 // Upper triangle
};

// Settings
int WINDOW_WIDTH = 1080;
int WINDOW_HEIGHT = 720;

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

    std::string src = getCurrentPath();

    // // Creates shader off of input file
    // Shader shader(src + "/shaders/Basic.shader");

    // // Binds shader to program
    // shader.bind();

    // // Sets known shader uniform
    // shader.setUniform4f("u_Color", 0.2f, 0.5f, 0.4f, 1.0f);

    // // Sets uniform location to texture at slot 0
    // shader.setUniform1i("u_Texture", 0);

    // Creates texture buffer and binds to slot 0
    Texture texture(src + "/textures/basketball.png");
    texture.bind(0);

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

    // setup cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);



    // // RENDER TO TEXTURE
    // // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    // GLuint FramebufferName = 0;
    // glGenFramebuffers(1, &FramebufferName);
    // glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // // The texture we're going to render to
    // GLuint renderedTexture;
    // glGenTextures(1, &renderedTexture);

    // // "Bind" the newly created texture : all future texture functions will modify this texture
    // glBindTexture(GL_TEXTURE_2D, renderedTexture);

    // // Give an empty image to OpenGL ( the last "0" )
    // glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 1024, 768, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

    // // Poor filtering. Needed !
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // // Set "renderedTexture" as our colour attachement #0
    // glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    // // Set the list of draw buffers.
    // GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    // glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // // Always check that our framebuffer is ok
    // if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    // return false;

    // // Render to our framebuffer
    // glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    // // glViewport(0,0,1024,768); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    // // The fullscreen quad's FBO
    // GLuint quad_VertexArrayID;
    // glGenVertexArrays(1, &quad_VertexArrayID);
    // glBindVertexArray(quad_VertexArrayID);

    // static const GLfloat g_quad_vertex_buffer_data[] = {
    //     -1.0f, -1.0f, 0.0f,
    //     1.0f, -1.0f, 0.0f,
    //     -1.0f,  1.0f, 0.0f,
    //     -1.0f,  1.0f, 0.0f,
    //     1.0f, -1.0f, 0.0f,
    //     1.0f,  1.0f, 0.0f,
    // };

    // GLuint quad_vertexbuffer;
    // glGenBuffers(1, &quad_vertexbuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

    // // Create and compile our GLSL program from the shaders
    // GLuint quad_programID = Shader::LoadShaders( src + "/shaders/Passthrough.vertex", src + "/shaders/firstpass.frag" );
    // GLuint texID = glGetUniformLocation(quad_programID, "renderedTexture");
    // GLuint timeID = glGetUniformLocation(quad_programID, "time");

    // // Render to the screen
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);



#ifdef ANTI_ALIASING
    AntiAliasing aa (WINDOW_WIDTH, WINDOW_HEIGHT);
#endif

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // renderer.clear();

        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 viewMat = camera.mat(45.0f, 0.1f, 100.0f);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);     
        
        // // Draws triangles
        // renderer.draw(va, ibo, shader, false);

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
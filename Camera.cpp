#include "Camera.h"
// #include <imgui.h>
// #include <imgui_impl_glfw.h>
// #include <imgui_impl_opengl3.h>

Camera::Camera(int width, int height, glm::vec3 origin)
{
    this->width = width;
    this->height = height;
    this->origin = origin;
}

glm::mat4 Camera::mat(float fov, float near, float far)
{
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    view = glm::lookAt(origin, origin + lookat, up);

    view = glm::rotate(view, glm::radians(45.0f), up);

    projection = glm::perspective(glm::radians(fov), (float)width / height, near, far);

    return (projection * view);
}

void Camera::inputs(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    // Forward movement (W)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        origin += cameraSpeed * lookat;
    }
    // Backward movement (S)
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        origin -= cameraSpeed * lookat;
    }
    // Left movement (A)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        origin -= glm::normalize(glm::cross(lookat, up)) * cameraSpeed;
    }
    // Right movement (D)
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        origin += glm::normalize(glm::cross(lookat, up)) * cameraSpeed;
    }

    // // Globals
    // float yaw = 0.0f;
    // float pitch = 0.0f;
    // float sensitivity = 0.1f; // Adjust this value for rotation speed
    // bool firstClick = true;
    // double lastX, lastY;

    // Additional logic for moving up and down
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            origin += up * cameraSpeed; // Move Up
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            origin -= up * cameraSpeed; // Move Down
        }
    }

}

void Camera::inputs_AA(GLFWwindow *window)
{
    /*
    This input listener function wait for number key 1 to 4 to be pressed.
    In order to change the type of Anti-Aliasing applied to the scene.
    */
    // Number 1 No anti-aliasing
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        std::cout << "No anti-aliasing" << std::endl;
    }
    // Number 2 MSAA
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        std::cout << "MSAA" << std::endl;
    }
    // Number 3 FXAA
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        std::cout << "FXAA" << std::endl;
    }
    // Number 4 SMAA
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        std::cout << "SMAA" << std::endl;
    }
}
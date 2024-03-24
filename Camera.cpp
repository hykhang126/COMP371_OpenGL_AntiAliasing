#include "Camera.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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

    // Globals
    float yaw = 0.0f;
    float pitch = 0.0f;
    float sensitivity = 0.1f; // Adjust this value for rotation speed
    bool firstClick = true;
    double lastX, lastY;

    // Settings
    const unsigned int SCR_WIDTH = 1920;
    const unsigned int SCR_HEIGHT = 1080;
    const float ROTATION_SPEED = 0.001f;

    // If not adjusting simulation attributes and shift is not pressed then pseudo-trackball is activated
    if (!ImGui::IsWindowFocused(ImGuiHoveredFlags_AnyWindow) && !(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            // Get cursor position
            double xpos, ypos;

            glfwGetCursorPos(window, &xpos, &ypos);

            // Center the trackball from top left to middle
            xpos -= SCR_WIDTH / 2;
            ypos -= SCR_HEIGHT / 2;

            // Calculate rotation angle
            float angleX = -xpos * ROTATION_SPEED;
            float angleY = -ypos * ROTATION_SPEED;
            
            // Apply rotation to camera
            glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(angleX), up);
            glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::normalize(glm::cross(lookat, up)));

            // Update lookat
            lookat = glm::mat3(rotationX) * lookat;

            // Rotate about objects
            glm::mat4 translateX = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            origin = glm::mat3(rotationX) * origin;

        }
    }

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
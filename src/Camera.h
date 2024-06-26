#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "Shader.h"

class Camera {

public:
	glm::vec3 origin;
	glm::vec3 lookat = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	
	int width;
	int height;
	float cameraSpeed = 0.005f;
	float cameraSensitivity = 0.5f;

	Camera(int width, int height, glm::vec3 origin);

	glm::mat4 mat(float fov, float near, float far);
	void inputs(GLFWwindow* window);
	void inputs_AA(GLFWwindow* window);



	float Zoom = 45.0f;
	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }
};
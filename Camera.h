#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
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
	float cameraSpeed = 0.1f;
	float cameraSensitivity = 0.5f;

	Camera(int width, int height, glm::vec3 origin);

	glm::mat4 mat(float fov, float near, float far);
	void inputs(GLFWwindow* window);
};
#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <GL/glew.h>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <Eigen/Core>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

// #include <Eigen/Core>
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader {

public:
	std::string fp;
	std::map<std::string, unsigned int> cache;
	Shader() = default;
	Shader(const std::string& fp);
	~Shader();

	void bind();
	void unbind();

	// Set shader program id
	void setShaderProgram(unsigned int id) { renderer_id = id; };

	// Sets uniforms
	void setUniform4f(const std::string& name, float v0, float v1, float f2, float f3);
	void setUniform1i(const std::string& name, int value);
	void setUniform1f(const std::string& name, float value);
	void setUniformMat4f(const std::string& name, glm::mat4& projection);

	// Getters
	int getUniformLocation(const std::string& name);

	// Load from file and return GLuint
	static GLuint LoadShaders(const std::string vertex_file_path,const std::string fragment_file_path);

private:
	unsigned int renderer_id;

	// Methods
	ShaderProgramSource parseShader(const std::string& filePath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
};
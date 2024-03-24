#include "Shader.h"
#include <stdlib.h>

Shader::Shader(const std::string& fp) : fp(fp), renderer_id(0)
{
    // Read file
    ShaderProgramSource source = parseShader(fp);
    
    // Output shaders
    std::cout << source.VertexSource << std::endl;
    std::cout << source.FragmentSource << std::endl;

    // Compile and create shader from parsed file
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

    renderer_id = shader;
}

Shader::~Shader()
{
    glDeleteProgram(renderer_id);
}

ShaderProgramSource Shader::parseShader(const std::string& filePath)
{
    std::ifstream stream(filePath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream s[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            s[(int)type] << line << "\n";
        }
    }

    return { s[0].str(), s[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    /* Error Handling */

    // Gets result of shader compilation
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    // If compiled unsuccessfully
    if (result == GL_FALSE)
    {
        // Get number of characters in shader log
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        // Allocates on stack dynamically
        // OOOO DANGEROUS OOOOOOOOO
        char* message = (char*) alloca(length * sizeof(char));

        // Gets log info passing in size of log message
        glGetShaderInfoLog(id, length, &length, message);

        // Shader failed output
        std::cout << "Shader failed to compile!" << std::endl;

        // Outputs type of shader that failed compilation
        int type;
        glGetShaderiv(id, GL_SHADER_TYPE, &type);
        if (type == GL_VERTEX_SHADER)
            std::cout << "Type: Vertex Shader" << std::endl;
        if (type == GL_FRAGMENT_SHADER)
            std::cout << "Type: Fragment Shader" << std::endl;
        std::cout << message << std::endl;

        // Deletes and returns 
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // Attaches shader objects to program
    glAttachShader(program, vs);
    glAttachShader(program, fs);

    // Creates shader executables
    glLinkProgram(program);

    // Validates whether executables can be run
    glValidateProgram(program);

    // Deletes shader objects
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


void Shader::bind()
{
    glUseProgram(renderer_id);
}

void Shader::unbind()
{
    glUseProgram(0);
}

// Setters
void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void Shader::setUniform1i(const std::string& name, int value)
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform1f(const std::string& name, float value)
{
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniformMat4f(const std::string& name, glm::mat4& projection)
{
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_TRUE, &projection[0][0]);
}

// Getters
int Shader::getUniformLocation(const std::string& name)
{
    if (cache.find(name) != cache.end())
        return cache[name];

    int location = glGetUniformLocation(renderer_id, name.c_str());
    if (location == -1)
        std::cout << "Not valid location! " << name.c_str() << std::endl;
    
    cache[name] = location;
    return location;
}

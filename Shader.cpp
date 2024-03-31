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

GLuint Shader::LoadShaders(const std::string vertex_file_path,const std::string fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
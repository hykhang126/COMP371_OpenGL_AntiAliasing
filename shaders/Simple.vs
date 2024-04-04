#version 330 core
     
// layout (location = 0) in vec4 position;
// layout (location = 1) in vec2 textureCoordinate;

// // Output vertex shader data (texture coordinate) to fragment shader
// out vec2 TexCoord;
// out vec4 FragPos;

// uniform mat4 u_Camera;
        
// void main()
// {
//     FragPos = u_Camera * position;
//     gl_Position = position * u_Camera;
//     TexCoord = textureCoordinate;
// };

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 u_Camera;

void main()
{
    TexCoords = aTexCoords;    
    gl_Position = vec4(aPos, 1.0) * u_Camera;
}
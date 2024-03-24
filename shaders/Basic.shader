#shader vertex
#version 330 core
        
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 textureCoordinate;
layout (location = 2) in vec3 aNormal;

// Output vertex shader data (texture coordinate) to fragment shader
out vec2 v_TextureCoordinate;
out vec3 Normal;
out vec4 FragPos;

uniform mat4 u_Camera;
        
void main()
{
    Normal = aNormal;
    FragPos = u_Camera * position;
    gl_Position = position * u_Camera;
    v_TextureCoordinate = textureCoordinate;
};

#shader fragment
#version 330 core
        
layout (location = 0) out vec4 color;

// Gets texture coordinate as input
in vec2 v_TextureCoordinate;

// Gets normal coordinates
in vec3 Normal;

// Gets fragment position from vertices
in vec4 FragPos;
        
uniform vec4 u_Color;
uniform sampler2D u_Texture;

uniform int is_Bidirectional;

void main()
{
    // Get texture color
    vec4 textureColor = texture(u_Texture, v_TextureCoordinate);
    vec3 textureColor3 = vec3(textureColor.x, textureColor.y, textureColor.z);

    // Conversion
    vec3 FragPos3 = vec3(FragPos.x, FragPos.y, FragPos.z);

    // Lighting
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 lightPos = vec3(0.5, -2.0, 10.0);
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos3); 
    float temp = dot(norm, lightDir);

    if (is_Bidirectional == 1)
        temp = abs(temp);

    float diff = max(temp, 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 result = diffuse * textureColor3;
    vec4 FragColor = vec4(result, 1.0);

    // Final Output
    color = FragColor;
};
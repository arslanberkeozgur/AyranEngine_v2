#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormalCoords;
layout (location = 2) in vec2 inTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform vec3 lightPos;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec3 LightPosition;

void main()
{
    gl_Position = projection * view * model * vec4(inPos, 1.0f);
    TexCoords = vec2(inTexCoords.x, inTexCoords.y);
    FragPos = vec3(view * model * vec4(inPos, 1.0));
    Normal = normalMatrix * inNormalCoords;
    LightPosition = vec3(view * vec4(lightPos, 1.0f));
}
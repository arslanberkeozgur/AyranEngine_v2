#version 330 core 
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(inPos.x, inPos.z, 0.0f, 1.0f);
    TexCoords = inTexCoords;
}
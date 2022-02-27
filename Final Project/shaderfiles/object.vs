#version 440 core 

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoordinate;
layout (location = 2) in vec3 normal;

out vec3 vertexNormal;
out vec3 vertexFragmentPos;
out vec2 vertexTextureCoordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);

    vertexFragmentPos = vec3(model * vec4(position, 1.0f));

    vertexNormal = mat3(transpose(inverse(model))) * normal;

    vertexTextureCoordinate = textureCoordinate;
}
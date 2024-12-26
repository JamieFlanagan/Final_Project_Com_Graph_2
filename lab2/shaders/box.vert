#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec3 vertexNormal;

out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;
out vec2 UV;

uniform mat4 MVP;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() {
    gl_Position = MVP * vec4(vertexPosition, 1.0);
    UV = vertexUV;
    FragPos = vec3(model * vec4(vertexPosition, 1.0));
    Normal = mat3(transpose(inverse(model))) * vertexNormal;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}

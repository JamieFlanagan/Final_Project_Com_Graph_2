#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoords; // Texture coordinates

out vec2 TexCoords;

uniform mat4 MVP;

void main() {
    TexCoords = vertexTexCoords; // Pass UV coordinates
    gl_Position = MVP * vec4(vertexPosition, 1.0);
}

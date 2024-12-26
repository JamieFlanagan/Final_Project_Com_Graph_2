#version 330 core

layout(location = 0) in vec3 vertexPosition; // Vertex positions
layout(location = 2) in vec2 vertexUV;       // UV coordinates

out vec2 uv;                                // Output UV for fragment shader

uniform mat4 MVP; // Model-View-Projection matrix

void main() {
    gl_Position = MVP * vec4(vertexPosition, 1.0);
    uv = vertexUV; // Pass UV coordinates to fragment shader
}

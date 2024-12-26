#version 330 core

layout(location = 0) in vec3 aPos; // Vertex position

uniform mat4 lightSpaceMatrix;     // Light projection-view matrix
uniform mat4 model;                // Model matrix

void main() {
    // Compute vertex position in light space
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}

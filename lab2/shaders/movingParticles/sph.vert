#version 330 core
layout (location = 0) in vec3 vertexPosition;

uniform mat4 MVP;           // Model-View-Projection matrix
uniform vec3 cameraPosition; // Camera world position
uniform float baseSize;      // Base size for particles in screen space

void main() {
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    // Compute the distance from the particle to the camera
    float distance = length(cameraPosition - vertexPosition);

    // Adjust point size to maintain a constant size on the screen
    gl_PointSize = baseSize / distance;
}

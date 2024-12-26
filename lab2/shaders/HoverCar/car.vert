#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 UV;

uniform mat4 MVP;

out vec2 fragUV;

void main() {
    gl_Position = MVP * vec4(vertexPosition, 1.0);
    fragUV = UV; // Pass UV to fragment shader
}

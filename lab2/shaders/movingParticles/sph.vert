#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aSize;

uniform mat4 VP;

out vec3 Color;
out float Alpha;

void main() {
    gl_Position = VP * vec4(aPos, 1.0);
    gl_PointSize = aSize * (300.0 / gl_Position.w);  // Size attenuation
    Color = aColor;
    Alpha = min(1.0, gl_Position.w / 300.0);  // Fade out with distance
}
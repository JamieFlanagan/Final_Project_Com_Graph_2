#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 MVP;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord; // Pass texture coordinates to fragment shader

void main() {
    FragPos = aPos;
    Normal = aNormal;
    TexCoord = aTexCoord; // Pass UVs
    gl_Position = MVP * vec4(aPos, 1.0);
}

#version 330 core

in vec2 uv; // UV coordinates from vertex shader
out vec4 FragColor;

uniform sampler2D textureSampler; // Texture sampler

void main() {
    FragColor = texture(textureSampler, uv);
}

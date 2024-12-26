#version 330 core
in vec2 fragUV;

out vec4 fragColor;

uniform sampler2D textureSampler;

void main() {
    fragColor = texture(textureSampler, fragUV); // Use only the texture color
}

#version 330 core
in vec2 TexCoord; // Receive texture coordinates

uniform sampler2D textureSampler; // Texture sampler

out vec4 FragColor;

void main() {
    // Fetch and output the texture color
    vec3 texColor = texture(textureSampler, TexCoord).rgb;
    FragColor = vec4(texColor, 1.0);
}

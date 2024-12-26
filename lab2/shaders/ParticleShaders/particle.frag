#version 330 core
out vec4 FragColor;

uniform float alpha; // Transparency

void main() {
    // Simple green particle
    FragColor = vec4(0.0, 1.0, 0.0, alpha);
}

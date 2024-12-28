#version 330 core
in vec3 Color;
in float Alpha;
out vec4 FragColor;

void main() {
    // Create a circular particle
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    float circle = dot(circCoord, circCoord);
    if (circle > 1.0) {
        discard;
    }

    // Fade from center outward
    float fadeAlpha = 1.0 - smoothstep(0.0, 1.0, circle);
    FragColor = vec4(Color, fadeAlpha * Alpha);
}
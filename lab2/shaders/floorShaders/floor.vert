#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec3 vertexNormal;

out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;
out vec2 UV;

uniform mat4 MVP;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform float tileScale;       // Scale for texture tiling
uniform vec3 cameraPosition;

void main() {
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    vec3 worldPos = vec3(model * vec4(vertexPosition, 1.0));

    // Adjust UV coordinates for infinite tiling
    UV = worldPos.xz * tileScale;

    // Pass data to the fragment shader
    FragPos = vec3(model * vec4(vertexPosition, 1.0));
    Normal = mat3(transpose(inverse(model))) * vertexNormal;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}

#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;
in vec2 UV;

uniform sampler2D shadowMap;
uniform sampler2D textureSampler; // Add the texture sampler
uniform bool useTexture;           // Toggle for texture usage
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

out vec4 fragColor;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 ||
            projCoords.y < 0.0 || projCoords.y > 1.0) {
            return 0.0; // Outside shadow map bounds
        }

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(Normal, normalize(lightPos - FragPos))), 0.005);
    return currentDepth > closestDepth + bias ? 0.3 : 1.0;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 ambient = 0.2 * lightColor;
    vec3 diffuse = diff * lightColor;
    vec3 specular = spec * lightColor;

    float shadow = ShadowCalculation(FragPosLightSpace);

    // Sample the texture
    vec3 texColor = texture(textureSampler, UV).rgb;
    vec3 lighting = (ambient + shadow * (diffuse + specular)) * texColor;

    fragColor = vec4(lighting, 1.0);
}

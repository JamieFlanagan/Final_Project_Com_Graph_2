#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 texCoord;

out vec3 finalColor;
out vec4 FragColor;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;

// Texture uniforms
uniform sampler2D diffuseMap;
uniform sampler2D emissiveMap;
uniform sampler2D glossinessMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

void main()
{
    // Sample textures
    vec3 diffuseColor = texture(diffuseMap, texCoord).rgb;
    vec3 emissiveColor = texture(emissiveMap, texCoord).rgb;
    float glossiness = texture(glossinessMap, texCoord).r;
    vec3 specularColor = texture(specularMap, texCoord).rgb;

    // Lighting calculations
    vec3 lightDir = lightPosition - worldPosition;
    float lightDist = dot(lightDir, lightDir);
    lightDir = normalize(lightDir);

    // Diffuse lighting
    float diffuseFactor = clamp(dot(lightDir, worldNormal), 0.0, 1.0);
    vec3 diffuseLight = lightIntensity * diffuseFactor / lightDist;

    // Specular lighting
    vec3 viewDir = normalize(-worldPosition);
    vec3 halfDir = normalize(lightDir + viewDir);
    float specularFactor = pow(max(dot(worldNormal, halfDir), 0.0), 32.0 * glossiness);
    vec3 specularLight = lightIntensity * specularFactor / lightDist;

    // Combine lighting with textures
    vec3 finalLighting = (diffuseLight * diffuseColor) +
                        (specularLight * specularColor) +
                        emissiveColor;

    // Tone mapping
    finalLighting = finalLighting / (1.0 + finalLighting);

    // Gamma correction
    FragColor = vec4(pow(finalLighting, vec3(1.0 / 2.2)), 1.0);
}
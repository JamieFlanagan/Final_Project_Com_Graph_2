#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

//Was not used kept it in
uniform sampler2D branchTexture;
uniform sampler2D leafTexture;
uniform sampler2D trunkTexture;

void main() {
    // Example logic: combine textures based on UV mapping
    vec4 branchColor = texture(branchTexture, TexCoords);
    vec4 leafColor = texture(leafTexture, TexCoords);
    vec4 trunkColor = texture(trunkTexture, TexCoords);

    // Blend them for simplicity (customize based on the model)
    FragColor = mix(branchColor, mix(leafColor, trunkColor, 0.5), 0.5);
}

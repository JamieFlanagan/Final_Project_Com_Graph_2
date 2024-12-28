#version 330 core

// Attributes
layout(location = 0) in vec3 inPosition;   // Vertex position
layout(location = 1) in vec3 inNormal;     // Vertex normal
layout(location = 2) in vec2 inTexCoord;   // Texture coordinates
layout(location = 3) in uvec4 inJoints;    // Joint indices
layout(location = 4) in vec4 inWeights;    // Joint weights

// Uniforms
uniform mat4 MVP;                  // Model-View-Projection matrix
uniform mat4 jointMatrices[99];    // Array of joint matrices
uniform float scaleFactor;         // For making the model smaller

// Outputs to the fragment shader
out vec3 worldPosition;
out vec3 worldNormal;
out vec2 texCoord;

void main() {
    // Skinning transformation
    vec4 skinnedPosition = vec4(0.0); // Initialize skinned position
    vec3 skinnedNormal = vec3(0.0);   // Initialize skinned normal

    // Loop over the four possible joint influences
    for (int i = 0; i < 4; i++) {
        float weight = inWeights[i];
        if (weight > 0.0) {
            // Get the joint index from the vertex attribute
            uint jointIndex = inJoints[i];

            // Retrieve the joint matrix
            mat4 jointMatrix = jointMatrices[jointIndex];

            // Apply skinning to the position
            skinnedPosition += weight * (jointMatrix * vec4(inPosition, 1.0));

            // Apply skinning to the normal
            mat3 jointMatrix3 = mat3(jointMatrix);
            skinnedNormal += weight * (jointMatrix3 * inNormal);
        }
    }

    //Apply scale
    skinnedPosition.xyz *= scaleFactor;

    // Pass world data to the fragment shader
    worldPosition = vec3(skinnedPosition);
    worldNormal = normalize(skinnedNormal);
    texCoord = inTexCoord;  // Pass texture coordinates to fragment shader

    // Transform
    gl_Position = MVP * skinnedPosition;
}
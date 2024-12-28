
#include "WelcomeSign.h"
#include <render/shader.h>
#include <glm/gtx/transform.hpp>
#include <iostream>

void WelcomeSign::initialize(glm::vec3 pos, GLuint texture) {
    position = pos;
    this->textureID = texture;

    // Create pole (skinny cube)
    float poleWidth = 2.0f;
    float poleHeight = 20.0f;
    float poleDepth = 2.0f;

    // Create sign (larger cube on top)
    float signWidth = 30.0f;
    float signHeight = 15.0f;
    float signDepth = 3.0f;

    // Add pole vertices
    addCube(-poleWidth / 2, 0, -poleDepth / 2, poleWidth, poleHeight, poleDepth);

    // Add sign vertices
    addCube(-signWidth / 2, poleHeight, -signDepth / 2, signWidth, signHeight, signDepth);

    // Set grey
    for (int i = 0; i < vertices.size(); i += 3) {
        colors.push_back(0.7f); // R
        colors.push_back(0.7f); // G
        colors.push_back(0.7f); // B
    }

    for (int i = 0; i < vertices.size(); i += 9) {
        // Calculate normal for each triangle
        glm::vec3 v1(vertices[i], vertices[i + 1], vertices[i + 2]);
        glm::vec3 v2(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
        glm::vec3 v3(vertices[i + 6], vertices[i + 7], vertices[i + 8]);

        glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));

        // Add normal for each vertex of the triangle
        for (int j = 0; j < 3; j++) {
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }
    }

    // Generate and bind VAO
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Vertex buffer
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Color buffer
    std::vector<float> colors(vertices.size(), 0.7f);
    glGenBuffers(1, &colorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);

    // UV buffer
    glGenBuffers(1, &uvBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);

    // Normal buffer
    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

    // Load shaders and get uniform locations
    programID = LoadShadersFromFile("../lab2/shaders/BillboardShaders/billboard.vert", "../lab2/shaders/BillboardShaders/billboard.frag");
    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    modelMatrixID = glGetUniformLocation(programID, "model");
    lightSpaceMatrixID = glGetUniformLocation(programID, "lightSpaceMatrix");
}

void WelcomeSign::addCube(float x, float y, float z, float width, float height, float depth) {
    // Front face
    addQuad(
        glm::vec3(x, y, z+depth),
        glm::vec3(x+width, y, z+depth),
        glm::vec3(x+width, y+height, z+depth),
        glm::vec3(x, y+height, z+depth),
        glm::vec3(0, 0, 1), width, height
    );

    // Back face
    addQuad(
        glm::vec3(x+width, y, z),
        glm::vec3(x, y, z),
        glm::vec3(x, y+height, z),
        glm::vec3(x+width, y+height, z),
        glm::vec3(0, 0, -1), width, height
    );

    // Top face
    addQuad(
        glm::vec3(x, y+height, z),
        glm::vec3(x+width, y+height, z),
        glm::vec3(x+width, y+height, z+depth),
        glm::vec3(x, y+height, z+depth),
        glm::vec3(0, 1, 0),width, height
    );

    // Bottom face
    addQuad(
        glm::vec3(x, y, z+depth),
        glm::vec3(x+width, y, z+depth),
        glm::vec3(x+width, y, z),
        glm::vec3(x, y, z),
        glm::vec3(0, -1, 0), width, height
    );

    // Right face
    addQuad(
        glm::vec3(x+width, y, z+depth),
        glm::vec3(x+width, y, z),
        glm::vec3(x+width, y+height, z),
        glm::vec3(x+width, y+height, z+depth),
        glm::vec3(1, 0, 0), width, height
    );

    // Left face
    addQuad(
        glm::vec3(x, y, z),
        glm::vec3(x, y, z+depth),
        glm::vec3(x, y+height, z+depth),
        glm::vec3(x, y+height, z),
        glm::vec3(-1, 0, 0), width, height
    );
}

void WelcomeSign::addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 normal, float faceWidth, float faceHeight) {
    // Calculate UV coordinates based on which face we're on
    float uvWidth = 1.0f;  // Full texture width
    float uvHeight = 1.0f; // Full texture height

    // For top/bottom faces, adjust UV based on sign's aspect ratio
    if (normal.y != 0) {  // Top or bottom face
        uvWidth = 1.0f;   // Full width (30 units)
        uvHeight = 0.1f;  // Scaled for depth (3 units)
    }

    // First triangle
    vertices.push_back(v1.x); vertices.push_back(v1.y); vertices.push_back(v1.z);
    vertices.push_back(v2.x); vertices.push_back(v2.y); vertices.push_back(v2.z);
    vertices.push_back(v3.x); vertices.push_back(v3.y); vertices.push_back(v3.z);

    uvs.push_back(0.0f);    uvs.push_back(1.0f);   // Top-left
    uvs.push_back(uvWidth); uvs.push_back(1.0f);     // Top-right
    uvs.push_back(uvWidth); uvs.push_back(0.0f);     // Bottom-right

    // Second triangle
    vertices.push_back(v1.x); vertices.push_back(v1.y); vertices.push_back(v1.z);
    vertices.push_back(v3.x); vertices.push_back(v3.y); vertices.push_back(v3.z);
    vertices.push_back(v4.x); vertices.push_back(v4.y); vertices.push_back(v4.z);

    uvs.push_back(0.0f);    uvs.push_back(1.0f);     // Top-left
    uvs.push_back(uvWidth); uvs.push_back(0.0f);     // Bottom-right
    uvs.push_back(0.0f);    uvs.push_back(0.0f); /// Bottom-left

    // Add normals for lighting
    for (int i = 0; i < 6; i++) {
        normals.push_back(normal.x);
        normals.push_back(normal.y);
        normals.push_back(normal.z);
    }
}

void WelcomeSign::render(glm::mat4 cameraMatrix) {
    glUseProgram(programID);

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 mvp = cameraMatrix * modelMatrix;

    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniform1i(glGetUniformLocation(programID, "useTexture"), GL_TRUE);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glUniform1i(glGetUniformLocation(programID, "textureSampler"), 0);


    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(2); // UV attribute location
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(3); // Normal attribute
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}
//Putting this line in here so I can check for github
//Just another change to check

void WelcomeSign:: renderDepth(GLuint shaderProgramID, glm::mat4 lightSpaceMatrix) {
    glUseProgram(shaderProgramID);
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, &modelMatrix[0][0]);

    glBindVertexArray(vertexArrayID);

    // Enable vertex position attribute
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Draw the geometry
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

    // Disable vertex attributes
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

}

void WelcomeSign::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(programID);
}

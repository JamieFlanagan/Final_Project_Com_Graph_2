//
// Created by JAMIE on 02/12/2024.
//

#include "components/drone.h"
#include <render/shader.h>
#include <stb/stb_image.h>
#include <iostream>
#include <vector>

static GLuint LoadTextureTileBox(const char *texture_file_path) {
    int w, h, channels;
    stbi_set_flip_vertically_on_load(false);
    uint8_t* img = stbi_load(texture_file_path, &w, &h, &channels, 3);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // To tile textures on a box, we set wrapping to repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (img) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture " << texture_file_path << std::endl;
    }
    stbi_image_free(img);

    return texture;
}

void Drone::initialize(glm::vec3 startPosition, GLuint guinessTexture) {
    position = startPosition;
    textureID = guinessTexture;
    height = startPosition.y;

    generateDroneBody();
    generateRotors();

    programID = LoadShadersFromFile("../lab2/shaders/box.vert", "../lab2/shaders/box.frag");
    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    textureSamplerID = glGetUniformLocation(programID, "textureSampler");
    useTextureID = glGetUniformLocation(programID, "useTexture");
}

void Drone::generateDroneBody() {
    std::vector<float> bodyVertices;
    std::vector<float> uvCoords;
    std::vector<float> normals;
    std::vector<float> colors;

    float height = 10.0f;
    float radius = 25.0f;
    int segments = 32;

    for(int i = 0; i <= segments; i++) {
        float angle = (2.0f * M_PI * i) / segments;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        float nx = cos(angle);
        float nz = sin(angle);

        // Add vertices for top and bottom
        bodyVertices.push_back(x);
        bodyVertices.push_back(height/2);
        bodyVertices.push_back(z);

        bodyVertices.push_back(x);
        bodyVertices.push_back(-height/2);
        bodyVertices.push_back(z);

        // UV coordinates - adjusted for proper texture wrapping
        float u = i / (float)segments;
        uvCoords.push_back(u);
        uvCoords.push_back(0.0f);
        uvCoords.push_back(u);
        uvCoords.push_back(1.0f);

        // Normal vectors - one per vertex
        normals.push_back(nx);
        normals.push_back(0.0f);
        normals.push_back(nz);

        normals.push_back(nx);
        normals.push_back(0.0f);
        normals.push_back(nz);

        // Colors - white for both vertices
        colors.push_back(1.0f);
        colors.push_back(1.0f);
        colors.push_back(1.0f);

        colors.push_back(1.0f);
        colors.push_back(1.0f);
        colors.push_back(1.0f);
    }

    glGenVertexArrays(1, &bodyVAO);
    glBindVertexArray(bodyVAO);

    // Position attribute
    glGenBuffers(1, &bodyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, bodyVBO);
    glBufferData(GL_ARRAY_BUFFER, bodyVertices.size() * sizeof(float), bodyVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Color attribute
    GLuint colorVBO;
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // UV attribute
    glGenBuffers(1, &uvVBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
    glBufferData(GL_ARRAY_BUFFER, uvCoords.size() * sizeof(float), uvCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Normal attribute
    GLuint normalVBO;
    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);
}

void Drone::generateRotors() {
    float bladeLength = 10.0f;
    float rotorVertices[] = {
        -bladeLength, 0.0f, 0.0f,
        bladeLength, 0.0f, 0.0f,
        0.0f, 0.0f, -bladeLength,
        0.0f, 0.0f, bladeLength
    };

    glGenVertexArrays(1, &rotorVAO);
    glBindVertexArray(rotorVAO);

    glGenBuffers(1, &rotorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rotorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rotorVertices), rotorVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void Drone::updatePosition(float deltaTime) {
    float baseX = radius * cos(time * speed);
    float baseZ = radius * sin(time * speed);

    float figure8X = 30.0f * sin(2 * time * speed);
    float figure8Z = 30.0f * sin(time * speed);

    position.x = baseX + figure8X;
    position.z = baseZ + figure8Z;
    position.y = height + 20.0f * sin(time * 1.5f);

    rotorAngle += 15.0f * deltaTime;
    time += deltaTime;
}

void Drone::render(glm::mat4 cameraMatrix, glm::vec3 lightPos, glm::vec3 lightColor,
                  glm::vec3 viewPos, GLuint shadowMap, const glm::mat4& lightSpaceMatrix) {
    glUseProgram(programID);

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 mvp = cameraMatrix * modelMatrix;

    // Pass uniforms
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

    glUniform3fv(glGetUniformLocation(programID, "lightPos"), 1, &lightPos[0]);
    glUniform3fv(glGetUniformLocation(programID, "lightColor"), 1, &lightColor[0]);
    glUniform3fv(glGetUniformLocation(programID, "viewPos"), 1, &viewPos[0]);

    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureSamplerID, 0);
    glUniform1i(useTextureID, GL_TRUE);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glUniform1i(glGetUniformLocation(programID, "shadowMap"), 1);

    // Draw body
    glBindVertexArray(bodyVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (32 + 1) * 2);

    // Draw rotors
    glBindVertexArray(rotorVAO);
    glUniform1i(useTextureID, GL_FALSE);
    for(int i = 0; i < 4; i++) {
        glm::mat4 rotorModel = glm::translate(modelMatrix, rotorOffsets[i]);
        rotorModel = glm::rotate(rotorModel, glm::radians(rotorAngle), glm::vec3(0,1,0));
        mvp = cameraMatrix * rotorModel;
        glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &rotorModel[0][0]);
        glDrawArrays(GL_LINES, 0, 4);
    }
}

void Drone::cleanup() {
    glDeleteVertexArrays(1, &bodyVAO);
    glDeleteVertexArrays(1, &rotorVAO);
    glDeleteBuffers(1, &bodyVBO);
    glDeleteBuffers(1, &rotorVBO);
    glDeleteBuffers(1, &uvVBO);
    glDeleteProgram(programID);
}

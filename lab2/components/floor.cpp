//
// Created by JAMIE on 13/12/2024.
//

#include "floor.h"
#include <render/shader.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
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

GLfloat vertex_buffer_data[12] = {
    -800.0f, 0.0f, -800.0f,
    800.0f, 0.0f, -800.0f,
    800.0f, 0.0f, 800.0f,
    -800.0f, 0.0f, 800.0f
};

GLfloat uv_buffer_data[8] = {
    0.0f, 0.0f,
    20.0f, 0.0f,
    20.0f, 20.0f,
    0.0f, 20.0f
};

GLuint index_buffer_data[6] = {
    0, 2, 1,
    0, 3, 2
};

GLfloat normal_buffer_data[12] = {
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
};

void Floor::initialize(GLuint floorTexture) {
    textureID = floorTexture;

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buffer_data), normal_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &uvBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

    programID = LoadShadersFromFile("../lab2/shaders/floorShaders/floor.vert", "../lab2/shaders/floorShaders/floor.frag");
    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    textureSamplerID = glGetUniformLocation(programID, "textureSampler");
    useTextureID = glGetUniformLocation(programID, "useTexture");
    lightSpaceMatrixID = glGetUniformLocation(programID, "lightSpaceMatrix");
    shadowMapID = glGetUniformLocation(programID, "shadowMap");
}

void Floor::render(glm::mat4 cameraMatrix, glm::vec3 cameraPosition, glm::vec3 lightPos, glm::vec3 lightColor, glm::vec3 viewPos, glm::mat4 lightSpaceMatrix, GLuint depthMap) {
    glUseProgram(programID);

    glm::mat4 modelMatrix;
    glm::mat4 mvp;
    float tileSize = 800.0f;

    // Determine the range of tiles to draw based on the camera position
    int range = 5;

    for (int x = -range; x <= range; ++x) {
        for (int z = -range; z <= range; ++z) {
            // Calculate the position of the current tile
            float tileX = floor(cameraPosition.x / tileSize) * tileSize + x * tileSize;
            float tileZ = floor(cameraPosition.z / tileSize) * tileSize + z * tileSize;

            modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(tileX, 0.0f, tileZ));
            mvp = cameraMatrix * modelMatrix;

            // Pass uniforms to the shader
            glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &modelMatrix[0][0]);
            glUniform3fv(glGetUniformLocation(programID, "cameraPosition"), 1, &cameraPosition[0]);
            glUniform1f(glGetUniformLocation(programID, "tileScale"), 0.1f);

            glUniform3fv(glGetUniformLocation(programID, "lightPos"), 1, &lightPos[0]);
            glUniform3fv(glGetUniformLocation(programID, "lightColor"), 1, &lightColor[0]);
            glUniform3fv(glGetUniformLocation(programID, "viewPos"), 1, &viewPos[0]);
            glUniformMatrix4fv(lightSpaceMatrixID, 1, GL_FALSE, &lightSpaceMatrix[0][0]);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glUniform1i(shadowMapID, 1);

            glUniform1i(useTextureID, GL_TRUE);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glEnableVertexAttribArray(3);
            glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glUniform1i(textureSamplerID, 0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(2);
            glDisableVertexAttribArray(3);
        }
    }
}


void Floor:: renderDepth(GLuint depthShaderProg, glm::mat4 lightSpaceMatrix) {
    glUseProgram(depthShaderProg);

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(depthShaderProg, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(depthShaderProg, "model"), 1, GL_FALSE, &modelMatrix[0][0]);

    glBindVertexArray(vertexArrayID);

    // Enable and bind vertex attributes
    glEnableVertexAttribArray(0); // Position attribute
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    // Draw the floor
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Disable vertex attributes
    glDisableVertexAttribArray(0);

}

void Floor::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &uvBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(programID);
}
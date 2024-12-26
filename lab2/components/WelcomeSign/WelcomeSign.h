//
// Created by JAMIE on 19/12/2024.
//

#ifndef WELCOMESIGN_H
#define WELCOMESIGN_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


class WelcomeSign {

public:
    // For shaders
    GLuint vertexArrayID, vertexBufferID, colorBufferID, normalBufferID, uvBufferID;
    GLuint programID, mvpMatrixID, modelMatrixID, lightSpaceMatrixID, useTexture, textureID;

    // Struct data
    std::vector<float> vertices;
    std::vector<float> colors;
    std::vector<float> normals;
    std::vector<float> uvs;
    glm::vec3 position;

    void initialize(glm::vec3 pos, GLuint texture);
    void addCube(float x, float y, float z, float width, float height, float depth);
    void addQuad(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, glm::vec3 normal, float faceWidth, float faceHeight);
    void render(glm::mat4 cameraMatrix);
    void renderDepth(GLuint shaderProgramID, glm::mat4 lightSpaceMatrix);
    void cleanup();

};



#endif //WELCOMESIGN_H

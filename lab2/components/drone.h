//
// Created by JAMIE on 02/12/2024.
//

#ifndef DRONE_H
#define DRONE_H
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



struct Drone {
    // Movement variables
    glm::vec3 position;
    float time = 0.0f;
    float radius = 100.0f;
    float height = 300.0f;
    float speed = 0.5f;
    float rotorAngle = 0.0f;

    // OpenGL buffers
    GLuint bodyVAO, rotorVAO;
    GLuint bodyVBO, rotorVBO, uvVBO, normalVBO;
    GLuint textureID;
    GLuint programID;
    GLuint mvpMatrixID;
    GLuint textureSamplerID;
    GLuint useTextureID;
    GLuint modelMatrixID;
    GLuint lightSpaceMatrixID;
    GLuint shadowMapID;

    void initialize(glm::vec3 startPosition, GLuint guinessTexture);
    void updatePosition(float deltaTime);
    void render(glm::mat4 cameraMatrix, glm::vec3 lightPos, glm::vec3 lightColor,
               glm::vec3 viewPos, GLuint shadowMap, const glm::mat4& lightSpaceMatrix);
    void renderDepth(const glm::mat4& lightSpaceMatrix);
    void cleanup();

private:
    void generateDroneBody();
    void generateRotors();

    glm::vec3 rotorOffsets[4] = {
        glm::vec3(-15.0f, 0.0f, -15.0f),
        glm::vec3(15.0f, 0.0f, -15.0f),
        glm::vec3(-15.0f, 0.0f, 15.0f),
        glm::vec3(15.0f, 0.0f, 15.0f)
    };
};

#endif //DRONE_H

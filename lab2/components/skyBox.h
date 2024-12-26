#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

struct SkyBox {
    glm::vec3 position; // Position of the box
    glm::vec3 scale;    // Size of the box in each axis

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint colorBufferID;
    GLuint uvBufferID;
    GLuint textureID;

    // Shader variable IDs
    GLuint mvpMatrixID;
    GLuint textureSamplerID;
    GLuint programID;

    // Functions
    void initialize(glm::vec3 position, glm::vec3 scale, const char* texturePath);
    void render(const glm::mat4& cameraMatrix, const glm::mat4& projectionMatrix);
    void cleanup();
};

#endif // SKYBOX_H

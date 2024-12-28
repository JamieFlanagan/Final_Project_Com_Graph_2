#ifndef FLAG_H
#define FLAG_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <render/shader.h>

class Flag {
public:
    void initialize(glm::vec3 initPosition, glm::vec3 initScale, GLuint texture);
    void render(glm::mat4 vpMatrix);
    void cleanup();

private:
    glm::vec3 position;
    glm::vec3 scale;
    GLuint textureID;
    GLuint vertexArrayID, vertexBufferID, uvBufferID, programID;

    GLfloat vertices[12] = {
        -0.5f,  0.5f, 0.0f, // Top-left
         0.5f,  0.5f, 0.0f, // Top-right
        -0.5f, -0.5f, 0.0f, // Bottom-left
         0.5f, -0.5f, 0.0f  // Bottom-right
    };

    GLfloat uvCoords[12] = {
        1.0f, 0.0f, // Top-left
        0.0f, 0.0f, // Top-right
        1.0f, 1.0f, // Bottom-left
        0.0f, 1.0f  // Bottom-right
    };
};



#endif //FLAG_H

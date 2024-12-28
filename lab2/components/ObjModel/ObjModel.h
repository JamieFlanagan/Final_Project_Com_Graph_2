#ifndef OBJMODEL_H
#define OBJMODEL_H
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <iostream>


class ObjModel {
public:
    glm::vec3 position; // Model position
    glm::vec3 scale;    // Model scale

    //Hover variables
    float hoverTime = 0.0f;
    float hoverAmplitude = 0.5f;
    float hoverFrequency = 2.0f;

    struct Mesh {
        GLuint vao, vbo, ebo; // OpenGL buffers for each mesh
        unsigned int indexCount; // Number of indices
        GLuint textureID;
    };

    std::vector<Mesh> meshes; // Store all meshes
    GLuint programID; // Shader program
    GLuint mvpMatrixID; // Uniform location
    GLuint colorID; // Uniform location for color

    void initialize(const std::string &path, glm::vec3 initPosition, glm::vec3 initScale);
    void render(glm::mat4 vpMatrix, float deltaTime);
    void cleanup();
};



#endif //OBJMODEL_H

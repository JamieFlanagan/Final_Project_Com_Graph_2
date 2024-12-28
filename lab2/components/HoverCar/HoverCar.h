#ifndef HOVERCAR_H
#define HOVERCAR_H
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class HoverCar {
public:
    glm::vec3 position; // Position of the car
    glm::vec3 scale;    // Scale of the car
    glm::vec3 color;    // Color of the car

    void initialize(glm::vec3 initialPosition, glm::vec3 initialScale, glm::vec3 carColor, GLuint TextureId);
    void render(glm::mat4 vpMatrix);
    void cleanup();


    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint normalBufferID;
    GLuint indexBufferID;
    GLuint uvBufferID;
    GLuint textureID;
    GLuint textureSamplerID;
    GLuint programID;
    GLuint mvpMatrixID;
    GLuint colorID;

};



#endif //HOVERCAR_H

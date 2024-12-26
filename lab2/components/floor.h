//
// Created by JAMIE on 13/12/2024.
//

#ifndef FLOOR_H
#define FLOOR_H


#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "glad/gl.h"

class Floor {
public:
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint colorBufferID;
    GLuint uvBufferID;
    GLuint indexBufferID;
    GLuint textureID;
    GLuint programID;
    GLuint mvpMatrixID;
    GLuint textureSamplerID;
    GLuint useTextureID;
    GLuint normalBufferID;
    GLuint shadowMapID;
    GLuint lightSpaceMatrixID;

    void initialize(GLuint floorTexture);
    void render(glm::mat4 cameraMatrix,  glm::vec3 cameraPosition, glm::vec3 lightPos, glm::vec3 lightColor, glm::vec3 viewPos, glm::mat4 lightSpaceMatrix, GLuint depthMap);
    void renderDepth(GLuint depthShaderProg, glm::mat4 lightSpaceMatrix);
    void cleanup();

};



#endif //FLOOR_H

//
// Created by JAMIE on 13/12/2024.
//


#ifndef BUILDING_H
#define BUILDING_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Building {
public:
    glm::vec3 position;		// Position of the box
    glm::vec3 scale;		// Size of the box in each axis

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint colorBufferID;
    GLuint uvBufferID;
    GLuint textureID;
    GLuint lightSpaceMatrixID;
    GLuint modelMatrixID;
    GLuint normalBufferID;
    GLuint mvpMatrixID;
    GLuint textureSamplerID;
    GLuint programID, useTextureID;
    GLuint shadowMapID;

    void initialize(glm::vec3 position, glm::vec3 scale, GLuint TextureID);
    void render(glm::mat4 cameraMatrix, glm::vec3 lightPos, glm::vec3 lightColor, glm::vec3 viewPos, glm::mat4 lightSpaceMatrix, GLuint depthMap);
    void renderDepth(GLuint shaderProgramID, glm::mat4 lightSpaceMatrix);
    void cleanup();
    bool checkCollision(const glm::vec3& position) const {
        // Calculate building bounds
        float halfWidth = scale.x;
        float halfDepth = scale.z;

        // Check if position is within building bounds
        return (position.x >= position.x - halfWidth &&
                position.x <= position.x + halfWidth &&
                position.z >= position.z - halfDepth &&
                position.z <= position.z + halfDepth);
    }

};



#endif //BUILDING_H


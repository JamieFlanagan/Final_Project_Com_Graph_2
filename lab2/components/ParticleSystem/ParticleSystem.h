//
// Created by JAMIE on 17/12/2024.
//

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <render/shader.h>
#include <random>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime;  // Time to live
    float alpha;     // Transparency
};

class ParticleSystem {
public:
    ParticleSystem();
    void initialize(int maxParticles , int rows, int cols, float spacing);
    void update(float deltaTime);
    void render(glm::mat4 lightSpaceMatrix);
    void cleanup();

private:
    GLuint vao, vertexBuffer, programID, textureID;
    GLint mvpMatrixID, alphaID, textureSamplerID;

    std::vector<Particle> particles;
    std::vector<float> vertices; // Quad vertices
    int maxParticles;
    int rows, cols;
    float spacing;
    float minX, maxX, minZ, maxZ;


    std::default_random_engine generator;
    std::uniform_real_distribution<float> positionDist;
    std::uniform_real_distribution<float> velocityDist;
    std::uniform_real_distribution<float> lifetimeDist;

    void respawnParticle(Particle& particle);
    void initializeQuad();

};



#endif //PARTICLESYSTEM_H

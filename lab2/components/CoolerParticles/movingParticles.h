#ifndef MOVINGPARTICLES_H
#define MOVINGPARTICLES_H

#include <glm/glm.hpp>
#include <vector>
#include <glad/gl.h>
#include "render/shader.h"
#include <random>

// Struct representing an orange particle
struct OrangeParticle {
    glm::vec3 position; // Current position
    float angle;        // Angle for circular motion
    float radius;       // Radius for circular motion
    float lifetime;     // Time to live
    float alpha;        // Transparency
};

class OrangeParticleSystem {
public:
    OrangeParticleSystem();
    ~OrangeParticleSystem();

    void initialize(int maxParticles, glm::vec3 center, float radius, float speed, glm::vec3 velocity);
    void update(float deltaTime);
    void render(glm::mat4 vpMatrix);
    void cleanup();

private:
    GLuint vao, vertexBuffer, programID;
    GLint mvpMatrixID, colorID, alphaID;

    std::vector<OrangeParticle> particles;

    glm::vec3 center; // Center of the circular motion
    float speed;      // Speed of circular motion
    int maxParticles;
    glm::vec3 velocity; // for movement

    std::default_random_engine generator;
    std::uniform_real_distribution<float> angleDist;
    std::uniform_real_distribution<float> lifetimeDist;

    void respawnParticle(OrangeParticle& particle);
};

#endif // MOVINGPARTICLES_H

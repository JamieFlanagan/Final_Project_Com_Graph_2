#ifndef IRISH_PARTICLE_H
#define IRISH_PARTICLE_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <random>

class IrishParticleSystem {
private:
    struct Particle {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 color;
        float life;
        float size;
        float angle;        // Current angle in the swirl
        float radius;       // Distance from center of swirl
        float heightOffset;
    };

    std::vector<Particle> particles;
    GLuint VAO, VBO;
    GLuint shaderProgram;
    unsigned int maxParticles;

    float swirlSpeed;      // Speed of rotation
    float coneHeight;      // Height of the cone
    float coneBaseRadius;  // Radius at the base of the cone
    float particleSpeed;   // Speed of particles moving up

    // Area bounds
    float width;
    float height;
    float depth;
    glm::vec3 systemCenter;

    // Random number generation
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> randomFloat;

    // Irish flag colors
    const glm::vec3 GREEN = glm::vec3(0.0f, 0.451f, 0.0f);
    const glm::vec3 WHITE = glm::vec3(1.0f, 1.0f, 1.0f);
    const glm::vec3 ORANGE = glm::vec3(1.0f, 0.498f, 0.0f);

    void resetParticle(Particle& particle, bool initial = false);
    glm::vec3 getRandomIrishColor();
    float getRadiusAtHeight(float height);

public:
    IrishParticleSystem();
    ~IrishParticleSystem();

    void initialize(unsigned int count, float areaWidth, float areaHeight, float areaDepth, glm::vec3 center);
    void update(float deltaTime);
    void render(const glm::mat4& vpMatrix);
    void cleanup();

    // Getters/Setters for runtime modifications
    void setSystemCenter(const glm::vec3& newCenter) { systemCenter = newCenter; }
    glm::vec3 getSystemCenter() const { return systemCenter; }
    void setSwirlSpeed(float speed) { swirlSpeed = speed; }
    void setConeHeight(float height) { coneHeight = height; }
    void setConeBaseRadius(float radius) { coneBaseRadius = radius; }
};

#endif // IRISH_PARTICLE_H
#include "movingParticles.h"
#include <render/shader.h>
#include <iostream>

IrishParticleSystem::IrishParticleSystem()
    : gen(rd()),
      randomFloat(-1.0f, 1.0f),
      swirlSpeed(2.0f),
      coneHeight(50.0f),
      coneBaseRadius(15.0f),
      particleSpeed(20.0f) {}

IrishParticleSystem::~IrishParticleSystem() {
    cleanup();
}


float IrishParticleSystem::getRadiusAtHeight(float height) {
    // Calculate radius at given height for cone shape
    float heightRatio = 1.0f - (height / coneHeight);
    return coneBaseRadius * heightRatio;
}

void IrishParticleSystem::initialize(unsigned int count, float areaWidth, float areaHeight, float areaDepth, glm::vec3 center) {
    maxParticles = count;
    width = areaWidth;
    height = areaHeight;
    depth = areaDepth;
    systemCenter = center;

    // Initialize particles
    particles.resize(maxParticles);
    for (auto& particle : particles) {
        resetParticle(particle, true);
    }

    // Create and bind VAO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Allocate buffer for all particle data
    glBufferData(GL_ARRAY_BUFFER, maxParticles * 7 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Size attribute
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Load shaders
    shaderProgram = LoadShadersFromFile("../lab2/shaders/movingParticles/sph.vert",
                                      "../lab2/shaders/movingParticles/sph.frag");
}

glm::vec3 IrishParticleSystem::getRandomIrishColor() {
    float random = randomFloat(gen);
    if (random < -0.33f) return GREEN;
    else if (random < 0.33f) return WHITE;
    else return ORANGE;
}

void IrishParticleSystem::resetParticle(Particle& particle, bool initial) {
    // Start at the bottom of the cone
    particle.heightOffset = initial ? randomFloat(gen) * coneHeight : 0.0f;

    // Random angle in the circle
    particle.angle = randomFloat(gen) * glm::two_pi<float>();

    // Random radius based on height (constrained by cone shape)
    float maxRadius = getRadiusAtHeight(particle.heightOffset);
    particle.radius = randomFloat(gen) * maxRadius;

    // Calculate position based on angle and radius
    float x = particle.radius * cos(particle.angle);
    float z = particle.radius * sin(particle.angle);
    particle.position = systemCenter + glm::vec3(x, particle.heightOffset, z);

    // Initialize other properties
    particle.color = getRandomIrishColor();
    particle.life = 1.0f + randomFloat(gen) * 0.5f;
    particle.size = 0.5f + randomFloat(gen) * 0.5f;
}

void IrishParticleSystem::update(float deltaTime) {
    std::vector<float> particleData;
    particleData.reserve(maxParticles * 7);

    for (auto& particle : particles) {
        // Update life
        particle.life -= deltaTime;

        if (particle.life <= 0.0f) {
            resetParticle(particle);
            continue;
        }

        // Update particle position in the swirl
        particle.angle += swirlSpeed * deltaTime;
        particle.heightOffset += particleSpeed * deltaTime;

        // Get the current radius based on height in the cone
        float currentRadius = getRadiusAtHeight(particle.heightOffset);

        // If particle reaches top of cone, reset it
        if (particle.heightOffset >= coneHeight) {
            resetParticle(particle);
            continue;
        }

        // Calculate new position
        float x = currentRadius * cos(particle.angle);
        float z = currentRadius * sin(particle.angle);
        particle.position = systemCenter + glm::vec3(x, particle.heightOffset, z);

        // Add particle data to buffer
        particleData.push_back(particle.position.x);
        particleData.push_back(particle.position.y);
        particleData.push_back(particle.position.z);
        particleData.push_back(particle.color.r);
        particleData.push_back(particle.color.g);
        particleData.push_back(particle.color.b);
        particleData.push_back(particle.size * (1.0f - particle.heightOffset/coneHeight)); // Particles get smaller as they rise
    }

    // Update VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particleData.size() * sizeof(float), particleData.data());
}

void IrishParticleSystem::render(const glm::mat4& vpMatrix) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "VP"), 1, GL_FALSE, &vpMatrix[0][0]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, maxParticles);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void IrishParticleSystem::cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}
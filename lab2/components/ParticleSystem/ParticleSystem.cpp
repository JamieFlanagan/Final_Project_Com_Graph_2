

#include "ParticleSystem.h"

#include <iostream>
#include <glm/glm.hpp>
#include <vector>

ParticleSystem::ParticleSystem()
    : maxParticles(0), vao(0), vertexBuffer(0), programID(0), textureID(0) {}


//Create the particiles within the bounds of my new city layout
void ParticleSystem::initialize( int maxParticles, int rows, int cols, float spacing) {
    this->maxParticles = maxParticles;
    this->rows = rows;
    this->cols = cols;
    this->spacing = spacing;

    float offsetX = -((cols - 1) * spacing) / 2.0f;
    float offsetZ = -((rows - 1) * spacing) / 2.0f;

    minX = offsetX;
    maxX = offsetX + (cols - 1) * spacing;
    minZ = offsetZ;
    maxZ = offsetZ + (rows - 1) * spacing;

    // Shader setup
    programID = LoadShadersFromFile("../lab2/shaders/ParticleShaders/particle.vert", "../lab2/shaders/ParticleShaders/particle.frag");
    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    alphaID = glGetUniformLocation(programID, "alpha");
    textureSamplerID = glGetUniformLocation(programID, "textureSampler");

    // Particle properties randomization
    positionDist = std::uniform_real_distribution<float>(-200.0f, 200.0f);
    velocityDist = std::uniform_real_distribution<float>(-5.0f, 5.0f);
    lifetimeDist = std::uniform_real_distribution<float>(1.0f, 5.0f);

    // Initialize particles
    particles.resize(maxParticles);
    for (auto& particle : particles) {
        respawnParticle(particle);
    }

    // Set up a quad for particles
    initializeQuad();
}

void ParticleSystem::initializeQuad() {
    float quadVertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
}

void ParticleSystem::respawnParticle(Particle& particle) {
    float minHeight = 0.0f;  // Ground level
    float maxHeight = 25.0f; // Max height above buildings

    // Randomly position particles within the city bounds and height range
    particle.position = glm::vec3(
        std::uniform_real_distribution<float>(minX, maxX)(generator), // Random X within grid
        std::uniform_real_distribution<float>(minHeight, maxHeight)(generator), // Random height
        std::uniform_real_distribution<float>(minZ, maxZ)(generator)  // Random Z within grid
    );

    // Add a small velocity for movement (optional)
    particle.velocity = glm::vec3(
        velocityDist(generator),
        0.0f,  // Keep particles at constant height
        velocityDist(generator)
    );

    // Randomize lifetime and set initial alpha
    particle.lifetime = lifetimeDist(generator);
    particle.alpha = 1.0f; // Fully opaque initially
}

void ParticleSystem::update(float deltaTime) {
    for (auto& particle : particles) {
        particle.lifetime -= deltaTime;
        if (particle.lifetime > 0.0f) {
            particle.position += particle.velocity * deltaTime;
            particle.alpha = particle.lifetime / 5.0f; // Fade effect
        } else {
            respawnParticle(particle);
        }
    }
}


void ParticleSystem::render(glm::mat4 vpMatrix) {
    glUseProgram(programID);
    glBindVertexArray(vao);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth writes but keep depth testing enabled
    glDepthMask(GL_FALSE);

    for (const auto& particle : particles) {
        if (particle.lifetime > 0.0f) {
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), particle.position);
            glm::mat4 mvp = vpMatrix * modelMatrix;
            glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
            glUniform1f(alphaID, particle.alpha);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }

    // Re-enable depth writes
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    glBindVertexArray(0);
    glUseProgram(0);
}


void ParticleSystem::cleanup() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);
}
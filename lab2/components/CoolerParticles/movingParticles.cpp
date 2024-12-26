#include "movingParticles.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

OrangeParticleSystem::OrangeParticleSystem() : vao(0), vertexBuffer(0), programID(0) {}

OrangeParticleSystem::~OrangeParticleSystem() {
    cleanup();
}

void OrangeParticleSystem::initialize(int maxParticles, glm::vec3 center, float radius, float speed, glm::vec3 velocity) {
    this->maxParticles = maxParticles;
    this->center = center;
    this->speed = speed;
    this->velocity = velocity;

    // Random generators for particle properties
    angleDist = std::uniform_real_distribution<float>(0.0f, 2.0f * glm::pi<float>());
    lifetimeDist = std::uniform_real_distribution<float>(2.0f, 5.0f);

    // Initialize particles
    particles.resize(maxParticles);
    for (auto& particle : particles) {
        particle.angle = angleDist(generator);
        particle.radius = radius;
        particle.lifetime = lifetimeDist(generator);
        particle.alpha = 1.0f;
        particle.position = center + glm::vec3(glm::cos(particle.angle) * radius, 0.0f, glm::sin(particle.angle) * radius);
    }

    // OpenGL setup
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);

    // Load shaders
    programID = LoadShadersFromFile("../lab2/shaders/movingParticles/sph.vert", "../lab2/shaders/movingParticles/sph.frag");
    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    colorID = glGetUniformLocation(programID, "particleColor");
    alphaID = glGetUniformLocation(programID, "alpha");
}


void OrangeParticleSystem::respawnParticle(OrangeParticle& particle) {
    particle.angle = angleDist(generator); // Randomize initial angle
    particle.lifetime = lifetimeDist(generator); // Reset lifetime
    particle.alpha = 1.0f; // Reset transparency
    particle.radius = particle.radius; // Keep the same radius

    // Spawn the particle relative to the current center
    particle.position = center + glm::vec3(
        glm::cos(particle.angle) * particle.radius,
        glm::sin(particle.angle) * particle.radius,
        center.z + std::uniform_real_distribution<float>(-10.0f, 10.0f)(generator) // Random z-offset
    );
}


//swirl pattern
void OrangeParticleSystem::update(float deltaTime) {
    //Move the particles around
    center += velocity * deltaTime;

    for (auto& particle : particles) {
        particle.lifetime -= deltaTime;

        if (particle.lifetime > 0.0f) {
            // Increase the angle to make the particle rotate
            particle.angle += speed * deltaTime;
            // Ensure angle stays within bounds [0, 2π]
            if (particle.angle > 2.0f * glm::pi<float>()) {
                particle.angle -= 2.0f * glm::pi<float>();
            }

            // Move the particle along the z-axis (twirl progression)
            particle.position.z += speed * deltaTime;

            // Swirling effect: X and Y positions change dynamically
            float dynamicRadius = particle.radius * (1.0f + 0.3f * glm::sin(particle.angle * 2.0f)); // Slight pulsation in the radius
            particle.position.x = center.x + glm::cos(particle.angle) * dynamicRadius;
            particle.position.y = center.y + glm::sin(particle.angle) * dynamicRadius;

            // Update alpha to create a fading effect
            particle.alpha = glm::max(particle.lifetime / 5.0f, 0.0f); // Gradual fading
        } else {
            // Respawn the particle when its lifetime ends
            respawnParticle(particle);
        }
    }
}




void OrangeParticleSystem::render(glm::mat4 vpMatrix) {
    glUseProgram(programID);
    glBindVertexArray(vao);

    std::vector<glm::vec3> positions;
    for (const auto& particle : particles) {
        positions.push_back(particle.position);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec3), positions.data());

    glEnableVertexAttribArray(0); // Enable position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
    glUniform3f(colorID, 1.0f, 0.5f, 0.0f); // Orange color

    for (const auto& particle : particles) {
        glUniform3f(colorID, 1.0f, 0.5f, 0.0f); // Orange color
        glUniform1f(alphaID, particle.alpha);   // Pass alpha for fading
    }

    glDrawArrays(GL_POINTS, 0, particles.size());

    glBindVertexArray(0);
    glUseProgram(0);
}

void OrangeParticleSystem::cleanup() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);
}

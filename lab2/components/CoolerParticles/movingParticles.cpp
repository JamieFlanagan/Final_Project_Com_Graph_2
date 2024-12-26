#include "movingParticles.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
/*

GreenParticleSystem::GreenParticleSystem() : maxParticles(0), vao(0), vertexBuffer(0), programID(0) {}

void GreenParticleSystem::initialize(int maxParticles) {
    this->maxParticles = maxParticles;

    // Random distributions for particle initialization
    positionDist = std::uniform_real_distribution<float>(-0.5f, 0.5f); // Small range around the origin
    phaseDist = std::uniform_real_distribution<float>(0.0f, glm::pi<float>() * 2.0f); // Random phase (0 to 2π)

    // Initialize particles
    particles.resize(maxParticles);
    initializeParticles();

    // Shader setup
    programID = LoadShadersFromFile("../lab2/shaders/ParticleShaders/particle.vert", "../lab2/shaders/ParticleShaders/particle.frag");
    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    alphaID = glGetUniformLocation(programID, "alpha");

    // Set up particle quad geometry
    initializeQuad();
}

void GreenParticleSystem::initializeParticles() {
    for (auto& particle : particles) {
        particle.position = glm::vec3(positionDist(generator), 0.0f, positionDist(generator));
        particle.phase = phaseDist(generator); // Random initial wave phase
        particle.alpha = 1.0f; // Fully opaque initially
    }
}

void GreenParticleSystem::initializeQuad() {
    float quadVertices[] = {
        -0.05f, -0.05f, 0.0f,
         0.05f, -0.05f, 0.0f,
        -0.05f,  0.05f, 0.0f,
         0.05f,  0.05f, 0.0f
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

void GreenParticleSystem::update(float deltaTime) {
    static float time = 0.0f;
    time += deltaTime;

    for (auto& particle : particles) {
        // Simulate wave motion on Y-axis, static X and Z positions
        particle.position.y = sin(time * 2.0f + particle.phase) * 0.1f; // Small wave amplitude (0.1f)
        particle.alpha = 1.0f; // Keep particles fully visible for now
    }
}

void GreenParticleSystem::render(glm::mat4 vpMatrix) {
    glUseProgram(programID);
    glBindVertexArray(vao);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth writes but keep depth testing enabled
    glDepthMask(GL_FALSE);

    for (const auto& particle : particles) {
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), particle.position);
        glm::mat4 mvp = vpMatrix * modelMatrix;

        glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
        glUniform1f(alphaID, particle.alpha);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // Re-enable depth writes
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    glBindVertexArray(0);
    glUseProgram(0);
}

void GreenParticleSystem::cleanup() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);
}
*/
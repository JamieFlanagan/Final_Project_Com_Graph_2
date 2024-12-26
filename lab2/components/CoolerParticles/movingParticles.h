#ifndef GREENPARTICLESYSTEM_H
#define GREENPARTICLESYSTEM_H

#include <vector>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <render/shader.h>
#include <random>

/*
struct GreenParticle {
    glm::vec3 position;
    glm::vec3 velocity;
    float phase;  // Wave phase for motion
    float alpha;  // Transparency
};

class GreenParticleSystem {
public:
    GreenParticleSystem();
    void initialize(int maxParticles);
    void update(float deltaTime);
    void render(glm::mat4 vpMatrix);
    void cleanup();

private:
    GLuint vao, vertexBuffer, programID;
    GLint mvpMatrixID, alphaID;

    std::vector<GreenParticle> particles;
    int maxParticles;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> positionDist;
    std::uniform_real_distribution<float> phaseDist;

    void initializeParticles();
    void initializeQuad();
};
*/
#endif // GREENPARTICLESYSTEM_H

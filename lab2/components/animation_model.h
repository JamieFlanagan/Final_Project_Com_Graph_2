//
// Created by JAMIE on 03/12/2024.
//

#ifndef ANIMATION_MODEL_H
#define ANIMATION_MODEL_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <tiny_gltf.h>
#include "components/Building.h"


#include <vector>
#include <map>
#include <string>
#include "render/shader.h"

struct MaterialTextures {
    GLuint diffuseTexture;
    GLuint emissiveTexture;
    GLuint glossinessTexture;
    GLuint normalTexture;
    GLuint specularTexture;
};



class animationModel {
public:

    animationModel();
    ~animationModel();

    GLuint mvpMatrixID;
    GLuint jointMatricesID;
    GLuint lightPositionID;
    GLuint lightIntensityID;
    GLuint textureID;
    GLuint programID;
    glm::vec3 lightPosition;  // Position of the light source
    glm::vec3 lightIntensity;
    glm::vec3 spawnPosition;
    glm::mat4 rotationMatrix;

    //For movement
    glm::vec3 targetPosition;
    size_t currentWaypointIndex;
    float movementSpeed;
    std::vector<glm::vec3> wayPoints;


    tinygltf::Model model;

    struct PrimitiveObject {
        GLuint vao;
        std::map<int, GLuint> vbos;
    };

    struct SkinObject {
        std::vector<glm::mat4> inverseBindMatrices;
        std::vector<glm::mat4> globalJointTransforms;
        std::vector<glm::mat4> jointMatrices;
    };

    struct SamplerObject {
        std::vector<float> input;
        std::vector<glm::vec4> output;
        int interpolation;
    };

    struct AnimationObject {
        std::vector<SamplerObject> samplers;
    };
    std::vector<AnimationObject> animationObjects;

    glm::mat4 getNodeTransform(const tinygltf::Node& node);
    void computeLocalNodeTransform(const tinygltf::Model& model, int nodeIndex, std::vector<glm::mat4>& localTransforms);
    void computeGlobalNodeTransform(const tinygltf::Model& model, const std::vector<glm::mat4>& localTransforms, int nodeIndex, const glm::mat4& parentTransform, std::vector<glm::mat4>& globalTransforms);
    std::vector<SkinObject> prepareSkinning(const tinygltf::Model& model);
    int findKeyframeIndex(const std::vector<float>& times, float animationTime);
    std::vector<AnimationObject> prepareAnimation(const tinygltf::Model& model);
    void updateAnimation(const tinygltf::Model& model, const tinygltf::Animation& anim, const AnimationObject& animationObject, float time, std::vector<glm::mat4>& nodeTransforms);
    void updateSkinning(const tinygltf::Skin& skin, const std::vector<glm::mat4>& nodeTransforms);
    void update(float time);
    bool loadModel(tinygltf::Model& model, const char* filename);
    void initialize(glm::vec3 position);
    void bindMesh(std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model& model, tinygltf::Mesh& mesh);
    void bindModelNodes(std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model& model, tinygltf::Node& node);
    std::vector<PrimitiveObject> bindModel(tinygltf::Model& model);
    void drawMesh(const std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model& model, tinygltf::Mesh& mesh);
    void drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model& model, tinygltf::Node& node);
    void drawModel(const std::vector<PrimitiveObject>& primitiveObjects, tinygltf::Model& model);
    void render(glm::mat4 cameraMatrix);
    void loadMaterialTextures();
    void moveToTarget(float deltaTime);

    void cleanup();

private:

    std::vector<PrimitiveObject> primitiveObjects;
    std::vector<SkinObject> skinObjects;
    GLuint diffuseMapID;
    GLuint emissiveMapID;
    GLuint glossinessMapID;
    GLuint normalMapID;
    GLuint specularMapID;
    std::vector<MaterialTextures> materials;


};

#endif //ANIMATION_MODEL_H

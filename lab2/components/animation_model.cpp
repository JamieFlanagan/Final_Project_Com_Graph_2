//
// Created by JAMIE on 03/12/2024.
//

#include "animation_model.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <render/shader.h>
#define BUFFER_OFFSET(offset) ((char*)nullptr + (offset))
#include <tiny_gltf.h>
#include <stb/stb_image.h>
#include "components/Building.h"


/*
static GLuint LoadTextureTileBox(const char *texture_file_path) {
    int w, h, channels;
    stbi_set_flip_vertically_on_load(false);
    uint8_t* img = stbi_load(texture_file_path, &w, &h, &channels, 3);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // To tile textures on a box, we set wrapping to repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (img) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture " << texture_file_path << std::endl;
    }
    stbi_image_free(img);

    return texture;
}

*/

GLuint loadTextureFromFile(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data) {
        std::cout << "Failed to load texture: " << filename << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return textureID;
}

animationModel::animationModel() : programID(0) {}

animationModel::~animationModel() {
    cleanup();
}

glm::mat4 animationModel::getNodeTransform(const tinygltf::Node& node) {
    glm::mat4 transform(1.0f);

    if (node.matrix.size() == 16) {
        transform = glm::make_mat4(node.matrix.data());
    } else {
        if (node.translation.size() == 3) {
            transform = glm::translate(transform, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
        }
        if (node.rotation.size() == 4) {
            glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
            transform *= glm::mat4_cast(q);
        }
        if (node.scale.size() == 3) {
            transform = glm::scale(transform, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
        }
    }
    return transform;
}

void animationModel::computeLocalNodeTransform(const tinygltf::Model& model,
        int nodeIndex,
        std::vector<glm::mat4> &localTransforms)
{
    // ---------------------------------------
    // TODO: your code here
    const tinygltf::Node& node = model.nodes[nodeIndex];

    glm::mat4 localMatrix = getNodeTransform(node);
    localTransforms[nodeIndex] = localMatrix;

    for (const int childIndex : node.children) {
        computeLocalNodeTransform(model, childIndex, localTransforms);
    }
    // ---------------------------------------
}

void animationModel::computeGlobalNodeTransform(const tinygltf::Model& model,
        const std::vector<glm::mat4> &localTransforms,
        int nodeIndex, const glm::mat4& parentTransform,
        std::vector<glm::mat4> &globalTransforms)
{
    // ----------------------------------------
    // TODO: your code here
    globalTransforms[nodeIndex] = parentTransform * localTransforms[nodeIndex];

    const tinygltf::Node& node = model.nodes[nodeIndex];

    // Recursively compute the global transforms for child nodes.
    for (int childIndex : node.children) {
        computeGlobalNodeTransform(model, localTransforms, childIndex, globalTransforms[nodeIndex], globalTransforms);
    }
    // ----------------------------------------
}

std::vector<animationModel::SkinObject> animationModel::prepareSkinning(const tinygltf::Model &model) {
		std::vector<SkinObject> skinObjects;

		// In our Blender exporter, the default number of joints that may influence a vertex is set to 4, just for convenient implementation in shaders.
		for (size_t i = 0; i < model.skins.size(); i++) {
			SkinObject skinObject;

			const tinygltf::Skin &skin = model.skins[i];

			// Read inverseBindMatrices
			const tinygltf::Accessor &accessor = model.accessors[skin.inverseBindMatrices];
			assert(accessor.type == TINYGLTF_TYPE_MAT4);
			const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
			const float *ptr = reinterpret_cast<const float *>(
            	buffer.data.data() + accessor.byteOffset + bufferView.byteOffset);

			skinObject.inverseBindMatrices.resize(accessor.count);
			for (size_t j = 0; j < accessor.count; j++) {
				float m[16];
				memcpy(m, ptr + j * 16, 16 * sizeof(float));
				skinObject.inverseBindMatrices[j] = glm::make_mat4(m);
			}

			assert(skin.joints.size() == accessor.count);

			skinObject.globalJointTransforms.resize(skin.joints.size());
			skinObject.jointMatrices.resize(skin.joints.size());

			// ----------------------------------------------
			// TODO: your code here to compute joint matrices
			// ----------------------------------------------

			std::vector<glm::mat4> localTransforms(model.nodes.size(), glm::mat4(1.0f));
			std::vector<glm::mat4> globalTransforms(model.nodes.size(), glm::mat4(1.0f));

			int root = skin.joints[0];

			computeLocalNodeTransform(model, root, localTransforms);
			computeGlobalNodeTransform(model, localTransforms, root, glm::mat4(1.0f), globalTransforms);

			for (size_t j = 0; j < skin.joints.size(); j++) {
				int jointIndex = skin.joints[j];
				skinObject.jointMatrices[j] = globalTransforms[jointIndex] * skinObject.inverseBindMatrices[j];
			}
			// ----------------------------------------------

			skinObjects.push_back(skinObject);
		}
		return skinObjects;
	}

int animationModel::findKeyframeIndex(const std::vector<float>& times, float animationTime) {
    // Find the first element that is greater than animationTime
    auto it = std::lower_bound(times.begin(), times.end(), animationTime);

    // Handle edge cases
    if (it == times.end()) {
        return times.size() - 2; // Wrap around to the last valid keyframe
    }
    if (it == times.begin()) {
        return 0; // If animationTime is before the first keyframe
    }

    // Return the index of the previous element
    return std::distance(times.begin(), it) - 1;
}

std::vector<animationModel::AnimationObject> animationModel::prepareAnimation(const tinygltf::Model &model)
	{
		std::vector<AnimationObject> animationObjects;
		for (const auto &anim : model.animations) {
			AnimationObject animationObject;

			for (const auto &sampler : anim.samplers) {
				SamplerObject samplerObject;

				const tinygltf::Accessor &inputAccessor = model.accessors[sampler.input];
				const tinygltf::BufferView &inputBufferView = model.bufferViews[inputAccessor.bufferView];
				const tinygltf::Buffer &inputBuffer = model.buffers[inputBufferView.buffer];

				assert(inputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				assert(inputAccessor.type == TINYGLTF_TYPE_SCALAR);

				// Input (time) values
				samplerObject.input.resize(inputAccessor.count);

				const unsigned char *inputPtr = &inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset];
				const float *inputBuf = reinterpret_cast<const float*>(inputPtr);

				// Read input (time) values
				int stride = inputAccessor.ByteStride(inputBufferView);
				for (size_t i = 0; i < inputAccessor.count; ++i) {
					samplerObject.input[i] = *reinterpret_cast<const float*>(inputPtr + i * stride);
				}

				const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
				const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
				const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

				assert(outputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
				const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

				int outputStride = outputAccessor.ByteStride(outputBufferView);

				// Output values
				samplerObject.output.resize(outputAccessor.count);

				for (size_t i = 0; i < outputAccessor.count; ++i) {

					if (outputAccessor.type == TINYGLTF_TYPE_VEC3) {
						memcpy(&samplerObject.output[i], outputPtr + i * 3 * sizeof(float), 3 * sizeof(float));
					} else if (outputAccessor.type == TINYGLTF_TYPE_VEC4) {
						memcpy(&samplerObject.output[i], outputPtr + i * 4 * sizeof(float), 4 * sizeof(float));
					} else {
						std::cout << "Unsupport accessor type ..." << std::endl;
					}

				}

				animationObject.samplers.push_back(samplerObject);
			}

			animationObjects.push_back(animationObject);
		}
		return animationObjects;
	}

void animationModel::updateAnimation(
    const tinygltf::Model &model,
    const tinygltf::Animation &anim,
    const AnimationObject &animationObject,
    float time,
    std::vector<glm::mat4> &nodeTransforms)
{
    for (const auto &channel : anim.channels) {
        int targetNodeIndex = channel.target_node;

        // Get sampler for the channel
        const auto &sampler = anim.samplers[channel.sampler];
        const std::vector<float> &times = animationObject.samplers[channel.sampler].input;
        float animationTime = fmod(time, times.back());

        // TODO Find keyframes
        int keyframeIndex = findKeyframeIndex(times, animationTime);
        int nextKeyframeIndex = (keyframeIndex + 1) % times.size();

        // Calculate interpolation factor
        float t0 = times[keyframeIndex];
        float t1 = times[nextKeyframeIndex];
        float factor = (animationTime - t0) / (t1 - t0);

        // Get output buffer data
        const auto &samplerOutput = animationObject.samplers[channel.sampler].output;

        if (channel.target_path == "translation") {
            glm::vec3 translation0, translation1;
            memcpy(&translation0, &samplerOutput[keyframeIndex], sizeof(glm::vec3));
            memcpy(&translation1, &samplerOutput[nextKeyframeIndex], sizeof(glm::vec3));
            glm::vec3 translation = glm::mix(translation0, translation1, factor);

            nodeTransforms[targetNodeIndex] = glm::translate(nodeTransforms[targetNodeIndex], translation);
        }
        else if (channel.target_path == "rotation") {
            glm::quat rotation0, rotation1;
            memcpy(&rotation0, &samplerOutput[keyframeIndex], sizeof(glm::quat));
            memcpy(&rotation1, &samplerOutput[nextKeyframeIndex], sizeof(glm::quat));
            glm::quat rotation = glm::slerp(rotation0, rotation1, factor);
            nodeTransforms[targetNodeIndex] *= glm::mat4_cast(rotation);
        }
        else if (channel.target_path == "scale") {
            glm::vec3 scale0, scale1;
            memcpy(&scale0, &samplerOutput[keyframeIndex], sizeof(glm::vec3));
            memcpy(&scale1, &samplerOutput[nextKeyframeIndex], sizeof(glm::vec3));
            glm::vec3 scale = glm::mix(scale0, scale1, factor);
            nodeTransforms[targetNodeIndex] = glm::scale(nodeTransforms[targetNodeIndex], scale);
        }
    }
}

void animationModel::updateSkinning(const tinygltf::Skin &skin,const std::vector<glm::mat4> &globalTransforms) {

    // -------------------------------------------------
    // TODO: Recompute joint matrices
    for (size_t i = 0; i < skin.joints.size(); ++i) {
        int jointIndex = skin.joints[i];
        skinObjects[0].jointMatrices[i] = globalTransforms[jointIndex] * skinObjects[0].inverseBindMatrices[i];
    }
    // -------------------------------------------------
}

void animationModel::update(float time) {

    if (model.animations.empty() || model.skins.empty()) {
        return;
    }

    // Animation and skin objects
    const tinygltf::Animation &animation = model.animations[0];
    const AnimationObject &animationObject = animationObjects[0];
    const tinygltf::Skin &skin = model.skins[0];

    // Initialize node transforms (identity for all nodes)
    std::vector<glm::mat4> nodeTransforms(model.nodes.size(), glm::mat4(1.0f));

    // Update node transforms with animation data
    updateAnimation(model, animation, animationObject, time, nodeTransforms);

    // Global transforms for all nodes
    std::vector<glm::mat4> globalTransforms(model.nodes.size(), glm::mat4(1.0f));

    // Compute global transforms starting from the root joint
    int rootNodeIndex = skin.joints[0];
    glm::mat4 parentTransform = glm::mat4(1.0f); // Identity matrix for the root's parent
    computeGlobalNodeTransform(model, nodeTransforms, rootNodeIndex, parentTransform, globalTransforms);

    // Apply the spawn position to all global transforms
    glm::mat4 globalTransformWithSpawn = glm::translate(glm::mat4(1.0f), spawnPosition);
    for (size_t i = 0; i < globalTransforms.size(); ++i) {
        globalTransforms[i] = globalTransformWithSpawn * globalTransforms[i];
    }

    // Update skinning joint matrices
    for (SkinObject &skinObject : skinObjects) {
        for (size_t i = 0; i < skin.joints.size(); ++i) {
            int jointIndex = skin.joints[i];
            skinObject.jointMatrices[i] = globalTransforms[jointIndex] * skinObject.inverseBindMatrices[i];
        }
    }
}

bool animationModel::loadModel(tinygltf::Model &model, const char *filename) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cout << "ERR: " << err << std::endl;
    }

    if (!res)
        std::cout << "Failed to load glTF: " << filename << std::endl;
    else
        std::cout << "Loaded glTF: " << filename << std::endl;

    return res;
}

void animationModel::initialize(glm::vec3 position) {
    this->spawnPosition=position;
    this->currentWaypointIndex=0;
    this->movementSpeed=1.0f;
    targetPosition=glm::vec3(0.0f);
    rotationMatrix=glm::mat4(1.0f); // No rotation/ identity matrix
    std::vector<glm::vec3> wayPoints;


    if (!loadModel(model, "../lab2/models/stationaryAlien/alienStationary.gltf")) {
        return;
    }

    // Prepare buffers for rendering
    primitiveObjects = bindModel(model);

    // Prepare joint matrices
    skinObjects = prepareSkinning(model);

    // Prepare animation data
    animationObjects = prepareAnimation(model);

    // Create and compile our GLSL program from the shaders
    programID = LoadShadersFromFile("../lab2/shaders/bot.vert", "../lab2/shaders/bot.frag");
    if (programID == 0)
    {
        std::cerr << "Failed to load shaders." << std::endl;
    }

    // Get a handle for GLSL variables
    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    jointMatricesID = glGetUniformLocation(programID, "jointMatrices");
    lightPositionID = glGetUniformLocation(programID, "lightPosition");
    lightIntensityID = glGetUniformLocation(programID, "lightIntensity");

    //The textures from blender
    diffuseMapID = glGetUniformLocation(programID, "diffuseMap");
    emissiveMapID = glGetUniformLocation(programID, "emissiveMap");
    glossinessMapID = glGetUniformLocation(programID, "glossinessMap");
    normalMapID = glGetUniformLocation(programID, "normalMap");
    specularMapID = glGetUniformLocation(programID, "specularMap");
    loadMaterialTextures();
}

void animationModel:: loadMaterialTextures() {
    //Have two materials
    materials.resize(2);

    // Load textures for first material (1001)
    materials[0].diffuseTexture = loadTextureFromFile("../lab2/models/alienModel/CH44_1001_Diffuse.png");
    if (materials[0].diffuseTexture == 0) {
        std::cerr << "Failed to load diffuse texture!" << std::endl;
    }
    materials[0].emissiveTexture = loadTextureFromFile("../lab2/models/alienModel/CH44_1001_Emissive.png");
    materials[0].glossinessTexture = loadTextureFromFile("../lab2/models/alienModel/CH44_1001_Glossiness.png");
    materials[0].normalTexture = loadTextureFromFile("../lab2/models/alienModel/CH44_1001_Normal.png");
    materials[0].specularTexture = loadTextureFromFile("../lab2/models/alienModel/CH44_1001_Specular.png");

    // Load textures for second material (1002)
    materials[1].diffuseTexture = loadTextureFromFile("../lab2/models/alienModel/CH44_1002_Diffuse.png");
    materials[1].emissiveTexture = loadTextureFromFile("../lab2/models/alienModel/CH44_1002_Emissive.png");
    materials[1].glossinessTexture = loadTextureFromFile("../lab2/models/alienModel/CH44_1002_Glossiness.png");
    materials[1].normalTexture = loadTextureFromFile("../lab2/models/alienModel/CH44_1002_Normal.png");
    materials[1].specularTexture = loadTextureFromFile("../lab2/models/alienModel/CH44_1002_Specular.png");
}




void animationModel::bindMesh(std::vector<PrimitiveObject> &primitiveObjects,
              tinygltf::Model &model, tinygltf::Mesh &mesh) {

    std::map<int, GLuint> vbos;
    for (size_t i = 0; i < model.bufferViews.size(); ++i) {
        const tinygltf::BufferView &bufferView = model.bufferViews[i];

        int target = bufferView.target;

        if (bufferView.target == 0) {
            continue;
        }

        const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(target, vbo);
        glBufferData(target, bufferView.byteLength,
                     &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);

        vbos[i] = vbo;
    }

    for (size_t i = 0; i < mesh.primitives.size(); ++i) {

        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        for (auto &attrib : primitive.attributes) {
            tinygltf::Accessor accessor = model.accessors[attrib.second];
            int byteStride =
                accessor.ByteStride(model.bufferViews[accessor.bufferView]);
            glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

            int size = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                size = accessor.type;
            }

            if (attrib.first.compare("POSITION") == 0) {
                int vaa = 0;
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                                      accessor.normalized ? GL_TRUE : GL_FALSE,
                                      byteStride, BUFFER_OFFSET(accessor.byteOffset));
            } else if (attrib.first.compare("NORMAL") == 0) {
                int vaa = 1;
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                                      accessor.normalized ? GL_TRUE : GL_FALSE,
                                      byteStride, BUFFER_OFFSET(accessor.byteOffset));
            } else if (attrib.first.compare("TEXCOORD_0") == 0) {
                int vaa = 2;
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                                      accessor.normalized ? GL_TRUE : GL_FALSE,
                                      byteStride, BUFFER_OFFSET(accessor.byteOffset));
            } else if (attrib.first.compare("JOINTS_0") == 0) {
                int vaa = 3; // Attribute location for JOINTS_0
                glEnableVertexAttribArray(vaa);
                glVertexAttribIPointer(vaa, size, accessor.componentType,
                                       byteStride, BUFFER_OFFSET(accessor.byteOffset));
            } else if (attrib.first.compare("WEIGHTS_0") == 0) {
                int vaa = 4; // Attribute location for WEIGHTS_0
                glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                                      accessor.normalized ? GL_TRUE : GL_FALSE,
                                      byteStride, BUFFER_OFFSET(accessor.byteOffset));
            } else {
                std::cout << "Unrecognized attribute: " << attrib.first << std::endl;
            }
        }

        // Record VAO for later use
        PrimitiveObject primitiveObject;
        primitiveObject.vao = vao;
        primitiveObject.vbos = vbos;
        primitiveObjects.push_back(primitiveObject);

        glBindVertexArray(0);
    }
}

void animationModel::bindModelNodes(std::vector<PrimitiveObject> &primitiveObjects,
                        tinygltf::Model &model,
                        tinygltf::Node &node) {
    // Bind buffers for the current mesh at the node
    if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
        bindMesh(primitiveObjects, model, model.meshes[node.mesh]);
    }

    // Recursive into children nodes
    for (size_t i = 0; i < node.children.size(); i++) {
        assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
        bindModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
    }
}
std::vector<animationModel::PrimitiveObject> animationModel::bindModel(tinygltf::Model &model) {
    std::vector<PrimitiveObject> primitiveObjects;

    const tinygltf::Scene &scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
        bindModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
    }

    return primitiveObjects;
}

void animationModel::drawMesh(const std::vector<PrimitiveObject> &primitiveObjects,
                tinygltf::Model &model, tinygltf::Mesh &mesh) {

    for (size_t i = 0; i < mesh.primitives.size(); ++i)
    {
        GLuint vao = primitiveObjects[i].vao;
        std::map<int, GLuint> vbos = primitiveObjects[i].vbos;

        glBindVertexArray(vao);

        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));

        glDrawElements(primitive.mode, indexAccessor.count,
                    indexAccessor.componentType,
                    BUFFER_OFFSET(indexAccessor.byteOffset));

        glBindVertexArray(0);
    }
}

void animationModel::drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects,
                        tinygltf::Model &model, tinygltf::Node &node) {
    // Draw the mesh at the node, and recursively do so for children nodes
    if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
        drawMesh(primitiveObjects, model, model.meshes[node.mesh]);
    }
    for (size_t i = 0; i < node.children.size(); i++) {
        drawModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
    }
}

void animationModel::drawModel(const std::vector<PrimitiveObject>& primitiveObjects,
                tinygltf::Model &model) {
    // Draw all nodes
    const tinygltf::Scene &scene = model.scenes[model.defaultScene];
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        drawModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
    }
}

void animationModel::render(glm::mat4 cameraMatrix) {
    glUseProgram(programID);
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), spawnPosition) * rotationMatrix; // translation with spawn and rotate

    // Set camera
    glm::mat4 mvp = cameraMatrix*modelMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    float scaleFactor = 0.1f;
    glUniform1f(glGetUniformLocation(programID, "scaleFactor"), scaleFactor);

    if (!skinObjects.empty()) {
        // Ensure we don't exceed the maximum number of joints in the shader
        size_t numJoints = std::min(skinObjects[0].jointMatrices.size(), size_t(99));
        glUniformMatrix4fv(jointMatricesID, static_cast<GLsizei>(numJoints), GL_FALSE,
                           glm::value_ptr(skinObjects[0].jointMatrices[0]));
    }

    // Bind textures
    // Diffuse texture in texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, materials[0].diffuseTexture);
    glUniform1i(diffuseMapID, 0);

    // Emissive texture in texture unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, materials[0].emissiveTexture);
    glUniform1i(emissiveMapID, 1);

    // Glossiness texture in texture unit 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, materials[0].glossinessTexture);
    glUniform1i(glossinessMapID, 2);

    // Normal texture in texture unit 3
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, materials[0].normalTexture);
    glUniform1i(normalMapID, 3);

    // Specular texture in texture unit 4
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, materials[0].specularTexture);
    glUniform1i(specularMapID, 4);
    // Set light data
    glUniform3fv(lightPositionID, 1, &lightPosition[0]);
    glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);

    // Draw the GLTF model
    drawModel(primitiveObjects, model);
}


//function to make my models move to the way points
void animationModel::moveToTarget(float deltaTime) {
    if (wayPoints.empty()) return;

    glm::vec3 currentTarget = wayPoints[currentWaypointIndex];
    glm::vec3 direction = currentTarget - spawnPosition;
    float distanceToTarget = glm::length(direction);

    if (distanceToTarget < 0.1f) { // Check if close enough to the waypoint
        spawnPosition = currentTarget; // Snap to the waypoint
        currentWaypointIndex = (currentWaypointIndex + 1) % wayPoints.size();
        return;
    }
    direction = glm::normalize(direction);
    glm::vec3 newPosition = spawnPosition + direction * movementSpeed * deltaTime;
    if (glm::dot(newPosition - spawnPosition, currentTarget - spawnPosition) >= glm::dot(currentTarget - spawnPosition, currentTarget - spawnPosition)) {
        spawnPosition = currentTarget; // Clamp to waypoint
        currentWaypointIndex = (currentWaypointIndex + 1) % wayPoints.size();
    } else {
        spawnPosition = newPosition; // Update position
    }
    // Update rotation to face movement direction
    float targetAngle = atan2(direction.x, direction.z);
    rotationMatrix = glm::rotate(glm::mat4(1.0f), targetAngle, glm::vec3(0.0f, 1.0f, 0.0f));
}



void animationModel::cleanup() {
    glDeleteProgram(programID);
}



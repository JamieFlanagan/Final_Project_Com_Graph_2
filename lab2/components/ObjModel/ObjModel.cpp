#include "ObjModel.h"
#include <render/shader.h>
#include <cmath>
#include <stb/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static GLuint LoadTextureTileBoxObjModel(const char *texture_file_path) {
    int w, h, channels;
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

void ObjModel::initialize(const std::string &path, glm::vec3 initPosition, glm::vec3 initScale) {
        position = initPosition;
        scale = initScale;

        // Load shaders
        programID = LoadShadersFromFile("../lab2/shaders/UFO/ufoModel.vert", "../lab2/shaders/UFO/ufoModel.frag");
        glUseProgram(programID);
        mvpMatrixID = glGetUniformLocation(programID, "MVP");
        colorID = glGetUniformLocation(programID, "modelColor");

        // Load .obj file using Assimp
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        // Process all meshes
        for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
            aiMesh *mesh = scene->mMeshes[meshIndex];
            std::vector<float> vertices;
            std::vector<unsigned int> indices;

            // Process vertices
            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                vertices.push_back(mesh->mVertices[i].x);
                vertices.push_back(mesh->mVertices[i].y);
                vertices.push_back(mesh->mVertices[i].z);

                if (mesh->mNormals) {
                    vertices.push_back(mesh->mNormals[i].x);
                    vertices.push_back(mesh->mNormals[i].y);
                    vertices.push_back(mesh->mNormals[i].z);
                } else {
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                    vertices.push_back(0.0f);
                }
            }

            // Process indices
            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j]);
                }
            }

            Mesh meshData;
            meshData.indexCount = indices.size();

            // OpenGL buffer setup
            glGenVertexArrays(1, &meshData.vao);
            glGenBuffers(1, &meshData.vbo);
            glGenBuffers(1, &meshData.ebo);

            glBindVertexArray(meshData.vao);

            glBindBuffer(GL_ARRAY_BUFFER, meshData.vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0); // Positions
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float))); // Normals
            glEnableVertexAttribArray(1);

            glBindVertexArray(0);

        	//Texture
        	if (mesh->mMaterialIndex >= 0) {
        		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        		aiString texPath;

        		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
        			std::string fullPath = texPath.C_Str();
        			meshData.textureID = LoadTextureTileBoxObjModel(fullPath.c_str());
        		} else {
        			meshData.textureID = 0; // No texture
        		}
        	}

            meshes.push_back(meshData);
        }
    }

void ObjModel:: render(glm::mat4 vpMatrix, float deltaTime) {
    glUseProgram(programID);

    //Add slight hover to the UFO
    hoverTime+=deltaTime;
    float hoverOffSet = hoverAmplitude*std::sin(hoverFrequency*hoverTime);
    glm::vec3 hoverPostition = position;
    hoverPostition.y+=hoverOffSet;

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), hoverPostition);
    modelMatrix = glm::scale(modelMatrix, scale);
    glm::mat4 mvp = vpMatrix * modelMatrix;

    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);


    for (const auto &mesh : meshes) {
        if (mesh.textureID != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh.textureID);
        }
        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void ObjModel::cleanup() {
    for (const auto &mesh : meshes) {
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);
        glDeleteVertexArrays(1, &mesh.vao);
    }
    glDeleteProgram(programID);
}


#include "HoverCar.h"
#include <render/shader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <stb/stb_image.h>

static GLuint LoadTextureTileBoxHoverCar(const char *texture_file_path) {
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

//My hoverCar data

	GLfloat hoverCar_vertex_buffer_data[72] = {
        // Front face (tapered)
        -0.8f, -0.3f,  1.0f,  // Narrower front
         0.8f, -0.3f,  1.0f,
         0.6f,  0.2f,  0.8f,  // Sloped windshield
        -0.6f,  0.2f,  0.8f,

        // Back face (wider)
        -1.0f, -0.3f, -1.0f,
         1.0f, -0.3f, -1.0f,
         0.8f,  0.1f, -1.0f,
        -0.8f,  0.1f, -1.0f,

        // Left face
        -1.0f, -0.3f, -1.0f,
        -0.8f, -0.3f,  1.0f,
        -0.6f,  0.2f,  0.8f,
        -0.8f,  0.1f, -1.0f,

        // Right face
         0.8f, -0.3f,  1.0f,
         1.0f, -0.3f, -1.0f,
         0.8f,  0.1f, -1.0f,
         0.6f,  0.2f,  0.8f,

        // Top face (cockpit)
        -0.6f,  0.2f,  0.8f,
         0.6f,  0.2f,  0.8f,
         0.8f,  0.1f, -1.0f,
        -0.8f,  0.1f, -1.0f,

        // Bottom face (slightly curved up at edges)
        -1.0f, -0.3f, -1.0f,
         1.0f, -0.3f, -1.0f,
         0.8f, -0.3f,  1.0f,
        -0.8f, -0.3f,  1.0f
    };

    // Updated normals for the modified shape
    GLfloat hoverCar_normal_buffer_data[72] = {
        // Front face
         0.0f,  0.2f,  1.0f,
         0.0f,  0.2f,  1.0f,
         0.0f,  0.2f,  1.0f,
         0.0f,  0.2f,  1.0f,

        // Back face
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,

        // Left face
        -1.0f,  0.1f,  0.0f,
        -1.0f,  0.1f,  0.0f,
        -1.0f,  0.1f,  0.0f,
        -1.0f,  0.1f,  0.0f,

        // Right face
         1.0f,  0.1f,  0.0f,
         1.0f,  0.1f,  0.0f,
         1.0f,  0.1f,  0.0f,
         1.0f,  0.1f,  0.0f,

        // Top face
         0.0f,  1.0f,  0.1f,
         0.0f,  1.0f,  0.1f,
         0.0f,  1.0f,  0.1f,
         0.0f,  1.0f,  0.1f,

        // Bottom face
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f
    };

    // Index buffer remains the same
    GLuint hoverCar_index_buffer_data[36] = {
        0, 1, 2, 0, 2, 3,     // Front face
        4, 5, 6, 4, 6, 7,     // Back face
        8, 9, 10, 8, 10, 11,  // Left face
        12, 13, 14, 12, 14, 15, // Right face
        16, 17, 18, 16, 18, 19, // Top face
        20, 21, 22, 20, 22, 23  // Bottom face
    };


	//UV
	GLfloat hoverCar_uvBufferData[48] = {
		// Front face
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		// Back face
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		// Left face
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		// Right face
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		// Top face
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		// Bottom face
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f
	};

void HoverCar::initialize(glm::vec3 initialPosition, glm::vec3 initialScale, glm::vec3 carColor, GLuint TextureId) {
	position = initialPosition;
	scale = initialScale;
	color = carColor;
	this->textureID = TextureId;

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hoverCar_vertex_buffer_data), hoverCar_vertex_buffer_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); // Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &normalBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hoverCar_normal_buffer_data), hoverCar_normal_buffer_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); // Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &uvBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hoverCar_uvBufferData), hoverCar_uvBufferData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hoverCar_index_buffer_data), hoverCar_index_buffer_data, GL_STATIC_DRAW);

	// Load shaders
	programID = LoadShadersFromFile("../lab2/shaders/HoverCar/car.vert", "../lab2/shaders/HoverCar/car.frag");
	mvpMatrixID = glGetUniformLocation(programID, "MVP");
	colorID = glGetUniformLocation(programID, "carColor");
	textureSamplerID = glGetUniformLocation(programID, "textureSampler");
}

void HoverCar::render(glm::mat4 vpMatrix) {
	glUseProgram(programID);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
	modelMatrix = glm::scale(modelMatrix, scale);
	glm::mat4 mvp = vpMatrix * modelMatrix;

	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniform3fv(colorID, 1, &color[0]);

	//Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(textureSamplerID, 0);

	glBindVertexArray(vertexArrayID);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void HoverCar::cleanup() {
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &indexBufferID);
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &normalBufferID);
	glDeleteTextures(1, &textureID);
	glDeleteBuffers(1, &uvBufferID);
	glDeleteProgram(programID);
}




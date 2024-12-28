#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <render/shader.h>
#include <stb/stb_image.h>
#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <algorithm>
#include <iomanip>
#include <math.h>
#include <bits/random.h>
#include <random>
#include <iostream>
#include <stb/stb_image_write.h>
#include <tiny_gltf.h>
//Components
#include "components/skyBox.h"
#include "components/drone.h"
#include "components/Floor//floor.h"
#include "components/Buildings/Building.h"
#include "components/AnimationRobot/animation_model.h"
#include "components/WelcomeSign/WelcomeSign.h"
#include "components/Flag/Flag.h"
#include "components/CoolerParticles/movingParticles.h"
#include "components/ParticleSystem/ParticleSystem.h"
#include "components/HoverCar/HoverCar.h"
#include "components/ObjModel/ObjModel.h"

static GLFWwindow *window;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// OpenGL camera view parameters
static glm::vec3 eye_center(-0.73395, 10.0f, -341.383f);
//Debug eye center for spawning very high in the scene
//static glm::vec3 eye_center(29.2804, 952.36, -344.483);
static glm::vec3 lookat(0, 0, 0);
static glm::vec3 up(0, 1, 0);
static glm:: vec3 forward;
static glm::vec3 right;

// View control
static float viewAzimuth = -90.0f; //was 0
static float viewPolar = 0.f;
static float viewDistance = 600.0f;
static float movementSpeed = 2.5f;
static float rotationSpeed = 3.0f;

//Shadows
const unsigned int shadow_width = 4096, shadow_height = 4096;
GLuint depthMapFBO;
GLuint depthMap;
bool saveDepthMap =false;


//Lighting
glm::vec3 lightPosition(100.0f, 800.0f, 0.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);       // White light
glm::vec3 viewPosition;
static glm::vec3 lightLookAt(0.0f, 0.0f, 0.0f);

std::vector<glm::vec3> wayPoints ={
	glm::vec3(98.076f, 0.0f, -160.728f),
	glm::vec3(96.36f, 0.0f, 99.2665f),
	glm::vec3(-96.1358f, 0.0f, 97.9782f),
	glm::vec3(-96.8901f, 0.0f, -162.033f)
};
std::vector<glm::vec3> wayPointsBot2 ={
	glm::vec3(33.0999, 0, -161.157),
	glm::vec3(33.0999, 0, -33.657),
	glm::vec3(-94.4001, 0, -33.657),
	glm::vec3(-94.4001, 0.0, -161.157)
};

std::vector<glm::vec3> wayPointsBot3 ={
	glm::vec3(281.15f, 0.0f, -247.148f),  // Start point
	glm::vec3(-256.338f, 0.0f, -247.148f),  // Second corner
	glm::vec3(-268.901f, 0.0f, 251.071f),  // Third corner
	glm::vec3(278.052f, 0.0f, 251.071f)
};


static GLuint LoadTextureTileBox(const char *texture_file_path) {
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

static void saveDepthTexture(GLuint fbo, std::string filename) {
	int width = shadow_width;
	int height = shadow_height;
	if (shadow_width == 0 || shadow_height == 0) {
		width = 1024;
		height = 768;
	}
	int channels = 3;

	std::vector<float> depth(width * height);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glReadBuffer(GL_DEPTH_COMPONENT);
	glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth.data());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<unsigned char> img(width * height * 3);
	for (int i = 0; i < width * height; ++i) img[3*i] = img[3*i+1] = img[3*i+2] = depth[i] * 255;

	stbi_write_png(filename.c_str(), width, height, channels, img.data(), width * channels);
}

//Function to set up the shadow map
void initializeShadowMap() {
	// Create the depth framebuffer
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//Make the hovercars move
void updateHoverCars(std::vector<HoverCar>& hoverCars, float deltaTime, float tallestBuildingHeight) {
	//Base variables so they dont overlap
	float baseRadius = 100.0f; // The start radius
	float radiusIncrement= 75.0f; // Incremenet for each car
	float speedChange= 0.5f; // vary the speed of cars

	for (size_t i = 0; i < hoverCars.size(); ++i) {
		// Create motion along the y-axis
		hoverCars[i].position.y = tallestBuildingHeight + 20.0f + 5.0f * sin(glfwGetTime() + i);

		float radius = baseRadius + i * radiusIncrement; // Increase radius for each car
		float speed = speedChange * (1.0f + i * 0.2f); // Slightly vary speed for each car
		float phaseShift = i * glm::radians(45.0f);

		// Move cars in a circular path around the city center
		hoverCars[i].position.x = radius * cos(glfwGetTime() * speed+ phaseShift);
		hoverCars[i].position.z = radius * sin(glfwGetTime() * speed + phaseShift);
	}
}

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Lab 2", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to open a GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);

	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cerr << "Failed to initialize OpenGL context." << std::endl;
		return -1;
	}
	glEnable(GL_PROGRAM_POINT_SIZE);

	// Background
	glClearColor(0.2f, 0.2f, 0.25f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//Shadow Map init
	initializeShadowMap();
	//Shadow Map shader
	GLuint depthShaderProg =LoadShadersFromFile("../lab2/shaders/depth.vert", "../lab2/shaders/depth.frag");

	//The ground
	Floor floor;
	GLuint floorTexture = LoadTextureTileBox("../lab2/myTextures/ground.jpg");
	floor.initialize(floorTexture);
	float floorSize = 800.0f;

	//My buildings
	int rows =7;
	int cols = 7;
	float spacing = 65;

	std::vector<GLuint> textures;
	std::random_device rd;
	std::mt19937 gen(rd());
	textures.push_back(LoadTextureTileBox("../lab2/myTextures/nightCity.jpg"));
	textures.push_back(LoadTextureTileBox("../lab2/myTextures/cityBuilding.jpg"));
	textures.push_back(LoadTextureTileBox("../lab2/myTextures/nightCity3.jpg"));
	std::uniform_int_distribution<> texture_dist(0, textures.size() - 1);
	float x=0;
	float z=0;
	std::vector<Building> buildings;
	float offsetX = -((cols - 1) * spacing) / 2.0f;
	float offsetZ = -((rows - 1) * spacing) / 2.0f;

	// Loop to create the 7x7 grid
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			Building b;

			// Compute position of each building
			glm::vec3 position = glm::vec3(offsetX + i * spacing, 0.0f, offsetZ + j * spacing);

			// Set a random height for variety (you can use your desired range)
			float height = 30.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (150.0f - 30.0f)));
			glm::vec3 scale = glm::vec3(16.0f, height, 16.0f);

			// Adjust position.y to account for the height
			position.y = height;
			position.y = height;
			GLuint random_texture = textures[texture_dist(gen)];
			b.initialize(position, scale, random_texture);
			buildings.push_back(b);
		}
	}

	//Drone creation stemming from sphere Idea
	Drone drone;
	GLuint guinessTexture = LoadTextureTileBox("../lab2/myTextures/guinessAddFix.jpg");
	drone.initialize(glm::vec3(100.0f, -800.0f, 60.0f), guinessTexture);

	//Hover Car init
	GLuint carTexture = LoadTextureTileBox("../lab2/myTextures/hoverCar2.jpg");
	float tallestBuildingHeight=350.0f; // This is used to stop cars from being where buildings can be
	std::vector<HoverCar> hoverCars;
	for (int i = 0; i < 5; ++i) {
		HoverCar car;
		glm::vec3 carPosition = glm::vec3(-100.0f + i * 50.0f, 350.0f, -200.0f);
		glm::vec3 carScale = glm::vec3(10.0f, 5.0f, 20.0f);
		glm::vec3 carColor = glm::vec3(0.0f, 1.0f, 0.0f); // Green
		car.initialize(carPosition, carScale, carColor, carTexture);
		hoverCars.push_back(car);
	}

	//SkyBox
	glm::vec3 cityCenterSky = glm::vec3((rows - 1) * spacing / 2.0f, 0, (cols - 1) * spacing / 2.0f);
	SkyBox skybox;
	glm::vec3 skyboxScale(1000.0f, 1000.0f, 1000.0f);
	skybox.initialize(cityCenterSky, glm::vec3(rows * spacing, rows * spacing, rows * spacing), "../lab2/myTextures/SpaceMap.png");

	//Models
	animationModel bot;
	bot.initialize(glm::vec3(52.176f, 0.0f, -323.899f));
	bot.targetPosition=wayPoints[0];
	bot.currentWaypointIndex=0;
	bot.movementSpeed = 10.0f;
	bot.wayPoints=wayPoints;

	animationModel bot2;
	bot2.initialize(glm::vec3(-161.986f, 0.0f, -170.522f));
	bot2.targetPosition=wayPointsBot2[0];
	bot2.currentWaypointIndex=0;
	bot2.movementSpeed = 10.0f;
	bot2.wayPoints=wayPointsBot2;

	animationModel bot3;
	bot3.initialize(glm::vec3(261.423, 10, -251.474));
	bot3.targetPosition=wayPointsBot3[0];
	bot3.currentWaypointIndex=0;
	bot3.movementSpeed = 10.0f;
	bot3.wayPoints=wayPointsBot3;

	//Welcome Sign
	WelcomeSign sign;
	glm::vec3 signPosition = glm::vec3(56.4475f, 0.0f, -295.218f);
	sign.initialize(signPosition, guinessTexture);

	//Particle System:
	ParticleSystem particles;
	particles.initialize(3000, rows, cols, spacing);

	//Irish colour particle system surrounding UFO
	IrishParticleSystem irishParticles;
	irishParticles.initialize(2000, rows, cols, spacing, glm::vec3(56.4475f, 0.0f, -275.218f));
	irishParticles.setSwirlSpeed(2.0f);
	irishParticles.setConeHeight(50.0f);
	irishParticles.setConeBaseRadius(15.0f);

	//UFO object
	ObjModel myModel;
	myModel.initialize("../lab2/models/UFO/correctUFO.obj",glm::vec3(56.4475f, 10.0f, -275.218f), glm::vec3(10.0f));

	//Flag
	GLuint irishFlagTexture = LoadTextureTileBox("../lab2/myTextures/irishFlag.jpg");
	Flag flag;
	flag.initialize(glm::vec3(-0.3, 42.4078, -212.365), glm::vec3(30.0f, 20.0f, 1.0f), irishFlagTexture);

	//Lighting setup
	glm::vec3 lightPos = cityCenterSky + glm::vec3(200.0f, 800.0f, 200.0f);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);     // Green light
	glm::vec3 viewPos = eye_center;              // Camera position

	// Camera setup
	forward = glm::normalize(lookat - eye_center);
	right = glm::normalize(glm::cross(forward, up));
	glm::mat4 viewMatrix, projectionMatrix;
    glm::float32 FoV = 100.0f;
	glm::float32 zNear = 0.1f;
	glm::float32 zFar = 2000.0f;
	projectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, zNear, zFar);

	float time = 0.0f;

	// Time and frame rate tracking
	float fTime = 0.0f;			// Time for measuring fps
	unsigned long frames = 0;
	do
	{
		//Time set up
		float animationTime = 0.0f;
		const float animSpeed = 1.0f;
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - time;
		time = currentTime;

		//move bots
		bot.moveToTarget(deltaTime);
		bot2.moveToTarget(deltaTime);
		bot3.moveToTarget(deltaTime);
		//character update
		bot.update(time);
		bot2.update(time);
		bot3.update(time);
		//Particles
		particles.update(deltaTime);
		irishParticles.update(deltaTime);
		//Hover cars
		updateHoverCars(hoverCars, deltaTime, tallestBuildingHeight);

	// Shadow pass
		glViewport(0, 0, shadow_width, shadow_height);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		//Set up lightspace matrix
		glm::mat4 lightProjection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, 1.0f, 1000.0f);
		glm::mat4 lightView = glm::lookAt(lightPosition, lightLookAt, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		//Render depth Map for shadows
		floor.renderDepth(depthShaderProg, lightSpaceMatrix);
		for (auto& building : buildings) {
			building.renderDepth(depthShaderProg, lightSpaceMatrix);
		}
		sign.renderDepth(depthShaderProg, lightSpaceMatrix);

		//Render normally
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 1024, 768);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		viewMatrix = glm::lookAt(eye_center, lookat, up);
		glm::mat4 vp = projectionMatrix * viewMatrix;

		glDisable(GL_DEPTH_TEST);
		skybox.render(viewMatrix,projectionMatrix );
		glEnable(GL_DEPTH_TEST);
		floor.render(vp, eye_center, lightPosition, lightColor, eye_center, lightSpaceMatrix, depthMap);
		for (auto& building : buildings) {
			building.render(vp, lightPosition, lightColor, eye_center, lightSpaceMatrix, depthMap);
		}
		bot.render(vp);
		bot2.render(vp);
		bot3.render(vp);
		glDisable(GL_CULL_FACE);
		myModel.render(vp, deltaTime);
		glEnable(GL_CULL_FACE);
		drone.render(vp, lightPos, lightColor, eye_center, depthMap, lightSpaceMatrix);
		sign.render(vp);
		glDisable(GL_CULL_FACE);
		for (auto& car : hoverCars) {
			car.render(vp);
		}
		glEnable(GL_CULL_FACE);
		glDisable(GL_CULL_FACE);
		particles.render(vp);
		irishParticles.render(vp);
		glEnable(GL_CULL_FACE);
		flag.render(vp);

		//Frame rate calculation
		frames++;
		fTime += deltaTime;
		if (fTime > 2.0f) {
			float fps = frames / fTime;
			frames = 0;
			fTime = 0;

			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << "Future Emerald Isle | Frames per second (FPS): " << fps;
			glfwSetWindowTitle(window, stream.str().c_str());
		}

		//Debug the camera posititon so I can exactly choose where I want certain elements

		static float printTimer = 0.0f;
		printTimer += deltaTime;
		if (printTimer > 0.5f) {
			std::cout << "Camera position: ("
					  << eye_center.x << ", "
					  << eye_center.y << ", "
					  << eye_center.z << ")"
					  << std::endl;
			printTimer = 0.0f;
		}


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));

	//Clean up structs
	for (auto& building : buildings) {
		building.cleanup();
	}
	for (auto& car : hoverCars) {
		car.cleanup();
	}
	floor.cleanup();
	sign.cleanup();
	skybox.cleanup();
	bot.cleanup();
	bot2.cleanup();
	bot3.cleanup();
	particles.cleanup();
	drone.cleanup();
	flag.cleanup();
	irishParticles.cleanup();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

// Is called whenever a key is pressed/released via GLFW
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    // Reset the camera position and orientation
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        viewAzimuth = -90.0f;
        viewPolar = 0.0f;
        eye_center = glm::vec3(0.0f, 10.0f, 0.0f);
        lookat = glm::vec3(0.0f, 0.0f, 0.0f);
        std::cout << "Camera reset." << std::endl;
    }

    // Movement controls
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        eye_center += forward * movementSpeed;
        lookat += forward * movementSpeed;
    }
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        eye_center -= forward * movementSpeed;
        lookat -= forward * movementSpeed;
    }
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        eye_center -= right * movementSpeed;
        lookat -= right * movementSpeed;
    }
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        eye_center += right * movementSpeed;
        lookat += right * movementSpeed;
    }

    // Rotation controls
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        viewPolar -= glm::radians(rotationSpeed);
        if (viewPolar < -glm::radians(89.0f)) viewPolar = -glm::radians(89.0f); // Prevent looking too far up
    }
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        viewPolar += glm::radians(rotationSpeed);
        if (viewPolar > glm::radians(89.0f)) viewPolar = glm::radians(89.0f); // Prevent looking too far down
    }
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        viewAzimuth -= glm::radians(rotationSpeed);
    }
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        viewAzimuth += glm::radians(rotationSpeed);
    }

    // Update lookat direction based on new azimuth and polar angles
    lookat.x = eye_center.x + cos(viewPolar) * cos(viewAzimuth);
    lookat.y = eye_center.y + sin(viewPolar);
    lookat.z = eye_center.z + cos(viewPolar) * sin(viewAzimuth);

    // Update forward and right vectors
    forward = glm::normalize(lookat - eye_center);
    right = glm::normalize(glm::cross(forward, up));

    // Exit application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}
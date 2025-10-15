#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

enum movementDirection {
	FORWARD,
	LEFT,
	RIGHT,
	BACKWARD
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

// Abstract camera class
class Camera
{
public:

	// Camera vectors
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;
	glm::vec3 worldUp;
	// Euler angles
	float pitch;
	float yaw;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float fov;
	float lastX;
	float lastY;

	Camera(
		float SCR_WIDTH,
		float SCR_HEIGHT,
		glm::vec3 posistion = glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f));

	glm::mat4 getViewMatrix();

	void processKeyboard(movementDirection direction, float deltaTime);
	void processMouse(float xIn, float yIn);
	void processScrollWheel(float yoffset);
	void updateVectors();
	void pointAt(glm::vec3 target);

};

#endif
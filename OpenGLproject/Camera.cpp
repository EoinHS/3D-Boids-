#define GLM_ENABLE_EXPERIMENTAL

#include "Camera.h"

Camera::Camera(
    float SCR_WIDTH,
    float SCR_HEIGHT,
    glm::vec3 posistion,
	glm::vec3 up,
    glm::vec3 front)
{
	cameraPos = posistion;
	cameraFront = front;
    worldUp = up;
    cameraUp = up;

	yaw = YAW;
	pitch = PITCH;
    MovementSpeed = SPEED;
    fov = ZOOM;

    lastX = SCR_WIDTH / 2;
    lastY = SCR_HEIGHT / 2;
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::processKeyboard(movementDirection direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == FORWARD) {
		cameraPos += cameraFront * velocity;
	}
	else if (direction == BACKWARD) {
		cameraPos -= cameraFront * velocity;
	}
	else if (direction == RIGHT) {
		cameraPos += cameraRight * velocity;
	}
	else if (direction == LEFT) {
		cameraPos -= cameraRight * velocity;
	}
}

void Camera::processMouse(float xposIn, float yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch -= yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    updateVectors();
}

void Camera::processScrollWheel(float yoffset)
{
    fov -= yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void Camera::updateVectors()
{
    // Update front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
    // Update camera Up and Right
    cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
    cameraUp    = glm::normalize(glm::cross(cameraRight, cameraFront));
}

void Camera::pointAt(glm::vec3 target)
{
    cameraFront = target - cameraPos;
    cameraFront = glm::normalize(cameraFront);

    pitch = glm::degrees( glm::asin( cameraFront.y ) );
    yaw   = glm::degrees( glm::acos( cameraFront.x / glm::cos( glm::radians( pitch ) ) ) );

    if (cameraPos.z > 0.0f) { 
        yaw = -yaw; 
    }

    updateVectors();
}
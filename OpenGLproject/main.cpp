#define GLM_ENABLE_EXPERIMENTAL

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
#include <glm/gtc/random.hpp>
#include <vector>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "ShaderProgram.h"
#include "Camera.h"

// <---- Settings ---->
// Window
const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGHT = 600.0f;

// Time Logic
float deltaTime;
float lastFrame;
float currentFrame;

// Camera
Camera camera(800.0f, 600.0f);

// Simulation settings
float SIM_SPEED = 0.0001f;
float PROTECTED_RANGE = 5.0f;
float VISUAL_RANGE = 10.f;
float AVOID_FACTOR = 0.05f;
float CENTERING_FACTOR = 0.0005f;
float MATCHING_FACTOR = 0.005f;
float TURN_FACTOR = 0.1f;
float X_LIMIT = 50.0f;
float Y_LIMIT = 50.0f;
float Z_LIMIT = 50.0f;
float SPEED_LIMIT = 50.0f;
float ACCCELERATION_FACTOR = 2.5f;

bool TRACKING_LARGEST_FLOCK = false;

const int BOID_COUNT = 200;

float fov = 45.0f;

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yPosIn) {
    camera.processMouse((float)xposIn, (float)yPosIn);
}

glm::mat4 get_rotation_matrix(glm::vec3 orientation) {

    glm::vec3 up = glm::vec3( 0.0f, 1.0f, 0.0f );
    orientation = glm::normalize(orientation);

    glm::vec3 rotation_axis = glm::normalize(glm::cross(up, orientation));

    float cos_theta = glm::dot(up, orientation);
    float rotation_angle = glm::acos(cos_theta);
    
    glm::quat rotation = glm::angleAxis(rotation_angle, rotation_axis);

    glm::mat4 rotation_matrix = glm::toMat4(rotation);

    return rotation_matrix;
}

void process_keyboard(GLFWwindow* window) {
    // Speed setting
    //  camera moves twice as fast when shift is held
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        deltaTime *= 2.0f; 
    }
    // Window Controls
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    // Camera Controls
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.processKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.processKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        TRACKING_LARGEST_FLOCK = !TRACKING_LARGEST_FLOCK;
    }
}



int main() {

    // Initialising GLFW and GLAD
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    lastFrame = static_cast<float>(glfwGetTime());

    // CallBack functions
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Enables depth testing
    glEnable(GL_DEPTH_TEST);

    // Creating the shader program
    ShaderProgram shaderProgram("..\\..\\OpenGLProject\\default.vert", "..\\..\\OpenGLProject\\\default.frag");
    
    // Vertices for a pyramid
    GLfloat points[] = {
       // Points            // Colours         // Plane normals
        -0.5f,  0.0f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.0f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
         0.0f,  1.5f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,

        -0.5f,  0.0f,  0.5f,  1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.0f, -0.5f,  1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
         0.0f,  1.5f,  0.0f,  1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,

         0.5f,  0.0f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.0f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.0f,  1.5f,  0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,

        -0.5f,  0.0f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.0f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
         0.0f,  1.5f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f,

        -0.5f,  0.0f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
        -0.5f,  0.0f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
         0.5f,  0.0f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
         0.5f,  0.0f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
        -0.5f,  0.0f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
         0.5f,  0.0f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
    };

    // Vertices for axis lines
    GLfloat axis_points[] = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    // Generate random posistions and velocities for boids

    glm::vec3 objectPositions[BOID_COUNT];
    glm::vec3 boidVelocity[BOID_COUNT];

    for (int i = 0; i < (sizeof(objectPositions) / sizeof(objectPositions[0])); i++) {
        objectPositions[i] = glm::vec3(glm::linearRand(-X_LIMIT, X_LIMIT), glm::linearRand(-Y_LIMIT, Y_LIMIT), glm::linearRand(-Z_LIMIT, Z_LIMIT));
        boidVelocity[i] = glm::ballRand(X_LIMIT);
    }

    // VAO for boids
    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(points, sizeof(points));

    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 9 * sizeof(GLfloat), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 9 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    VAO1.LinkAttrib(VBO1, 2, 3, GL_FLOAT, 9 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

    VAO1.Unbind();
    VBO1.Unbind();

    // VAO for axis
    VAO VAO2;
    VAO2.Bind();
    
    VBO VBO2(axis_points, sizeof(axis_points));

    VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)0);
    VAO2.LinkAttrib(VBO2, 1, 3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    VAO2.Unbind();
    VBO2.Unbind();

    // Create projection matrix and pass it to the shader (doesn't change frequently so not needed every loop)
    shaderProgram.use();
    glm::mat4 projection = glm::perspective(glm::radians(fov), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 200.0f);
    shaderProgram.setMatrix("projection", projection);
    
    // Frame counter
    int frame_counter = 0;

    // Rendering Loop
    while (!glfwWindowShouldClose(window)) {
        // Time logic
        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        frame_counter += 1;

        // process Inputs
        process_keyboard(window);

        // rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // activate the shader program
        shaderProgram.use();
        // Get view matrix from the camera + send to shader
        glm::mat4 view = camera.getViewMatrix();
        shaderProgram.setMatrix("view", view);

        // <---- Boid Logic ---->

        // Running tally for largest flock and its posistion
        int largest_flock_count = 0;
        glm::vec3 largest_flock_center = glm::vec3(0.0f, 0.0f, 0.0f);

        for (int i = 0; i < sizeof(objectPositions) / sizeof(objectPositions[0]); i++)
        {
            // Collect velocities of boids within the protected and visual ranges
            
            glm::vec3 close_delta = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 av_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 av_posistion = glm::vec3(0.0f, 0.0f, 0.0f);
            float neighbouring_boids = 0.0f;
            for (int j = 0; j < sizeof(objectPositions) / sizeof(objectPositions[0]); j++)
            {
                if ( i == j ) { continue; }
                // calculate distance between boids
                glm::vec3 diff = objectPositions[i] - objectPositions[j];
                float distance = glm::length(diff);
                if (distance < PROTECTED_RANGE) {
                    close_delta += diff;
                }
                else if (distance < VISUAL_RANGE) {
                    av_velocity += boidVelocity[j];
                    av_posistion += objectPositions[j];
                    neighbouring_boids += 1.0f;
                }
            }

            // Cohesion (matching and centering)
            if (neighbouring_boids > 0.0f) {
                av_velocity = av_velocity / neighbouring_boids;
                av_posistion = av_posistion / neighbouring_boids;

                boidVelocity[i] += (av_posistion - objectPositions[i]) * CENTERING_FACTOR;
                boidVelocity[i] += (av_velocity - boidVelocity[i]) * MATCHING_FACTOR;
            }

            // Add avoidance
            boidVelocity[i] += (close_delta * AVOID_FACTOR);

            // Add turning factor
            if (objectPositions[i].x > X_LIMIT) {
                boidVelocity[i].x -= TURN_FACTOR;
            } else if (objectPositions[i].x < -X_LIMIT) {
                boidVelocity[i].x += TURN_FACTOR;
            }
            if (objectPositions[i].y > Y_LIMIT) {
                boidVelocity[i].y -= TURN_FACTOR;
            } else if (objectPositions[i].y < -Y_LIMIT) {
                boidVelocity[i].y += TURN_FACTOR;
            }
            if (objectPositions[i].z > Z_LIMIT) {
                boidVelocity[i].z -= TURN_FACTOR;
            } else if (objectPositions[i].z < -Z_LIMIT) {
                boidVelocity[i].z += TURN_FACTOR;
            }

            // Acceleration
            boidVelocity[i] *= ACCCELERATION_FACTOR;

            // Enforce speed limits
            float speed = glm::length(boidVelocity[i]);

            if (speed > SPEED_LIMIT) {
                boidVelocity[i] = glm::normalize(boidVelocity[i]) * SPEED_LIMIT;
            }

            // Move each boid according to their velocity
            objectPositions[i] = objectPositions[i] + (boidVelocity[i] * SIM_SPEED);

            // Update largest flock

            if (neighbouring_boids > largest_flock_count) { //&& frame_counter % 20 == 0) {
                largest_flock_count = neighbouring_boids;
                largest_flock_center = av_posistion;
            }
            
        }

        // <---- Camera Logic ---->

        if (TRACKING_LARGEST_FLOCK) {
            camera.pointAt(largest_flock_center);
        }

        // <---- Lighting ---->

        // shaderProgram.setVec3("lightPos", glm::vec3(X_LIMIT, Y_LIMIT, Z_LIMIT));
        shaderProgram.setVec3("lightPos", glm::vec3(0.0f, 0.0f, 0.0f));
        shaderProgram.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        // Rendering all the boids
        VAO1.Bind();
        for (int i = 0; i < sizeof(objectPositions) / sizeof(objectPositions[0]); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);

            glm::mat4 rotation_matrix = get_rotation_matrix(boidVelocity[i]);

            model = glm::translate(model, objectPositions[i]);

            model = model * rotation_matrix;
            shaderProgram.setMatrix("model", model);

            glDrawArrays(GL_TRIANGLES, 0, sizeof(points) / (sizeof(GLfloat) * 6));
        }

        VAO2.Bind();
        glm::mat4 model = glm::mat4(1.0f);
        shaderProgram.setMatrix("model", model);
        glDrawArrays(GL_LINE_STRIP, 0, sizeof(axis_points) / (sizeof(GLfloat) * 6));

        // check events and swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
	return 0;
}
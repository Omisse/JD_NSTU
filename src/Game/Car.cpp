#include "glm/common.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Car.hpp"
#include "Components.hpp"
#include "GameObject.hpp"

#include <cstdio>

namespace JustDrive {
    Car::Car(
        glm::vec3 position,
        glm::vec3 rotation_rad,
        float maxSpeedForward,
        float maxSpeedBackward,
        float acceleration,
        float tractionParallel,
        float tractionCross,
        float mWheelAngle
    ) : 
        maxSpeedForward(maxSpeedForward),
        maxSpeedBackward(maxSpeedBackward),
        acceleration(acceleration),
        tractionParallel(tractionParallel),
        tractionCross(tractionCross)
    {
        GameObject::transform = Components::Transform(position);
        GameObject::transform.setRotation(rotation_rad);
        maxWheelAngle = glm::radians(glm::abs(mWheelAngle));
        currentVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
        currentWheelAngle = 0.0f;
    }

    void Car::initialize(GLFWwindow* window) {
        return;
    }

    void Car::run(GLFWwindow* window, float deltaTime) {
        glm::vec2 userInput(
            (float)(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            -(float)(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS),
            (float)(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            -(float)(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        );

        float accelerationZ = 0.0f;
        if ((currentVelocity.z < maxSpeedForward && userInput.x > 0.0f)||(currentVelocity.z > -maxSpeedBackward && userInput.x < 0.0f)) {
            accelerationZ = userInput.x*deltaTime*acceleration;
        } else {
            accelerationZ = 0.0f;
        }

        currentVelocity.z += accelerationZ;
        glm::vec3 velocity = currentVelocity.z*transform.getRight();
        velocity.x = -velocity.x;

        transform.setPosition(transform.getPosition()+deltaTime*velocity);

        currentWheelAngle = maxWheelAngle*-userInput.y;

        transform.setRotation(transform.getRotation()+glm::vec3(
            0,
            glm::radians(currentWheelAngle*(currentVelocity.z*0.5f)),
            0
        ));
    }

    glm::mat4 Car::getTransform() {
        return glm::translate(glm::rotate(glm::scale(transform.getTransform(),glm::vec3(0.2f,0.2f,0.2f)), 3.1415f/2.0f, transform.getUp()), glm::vec3(0.0f, 0.4f, 0.0f));
    }
}
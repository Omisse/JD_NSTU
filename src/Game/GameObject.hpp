#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Components.hpp"

namespace JustDrive {
    class GameObject {
        public:
            GameObject();
            GameObject(glm::vec3 position);
            GameObject(Components::Transform transform);
            Components::Transform transform;
            virtual void initialize(GLFWwindow* window) {};
            virtual void run(GLFWwindow* window, float deltaTime) {};
    };
}

#endif
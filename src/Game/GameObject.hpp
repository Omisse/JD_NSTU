#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Components.hpp"

namespace JustDrive {
    class GameObject {
        public:
            GameObject(); //конструктор
            GameObject(glm::vec3 position); //конструктор с координатами
            GameObject(Components::Transform transform); //конструктор с целой трансформой
            //своя трансформа
            Components::Transform transform;
            //функции для потомков, поэтому тела нет.
            virtual void initialize(GLFWwindow* window) {};
            virtual void run(GLFWwindow* window, float deltaTime) {};
    };
}

#endif
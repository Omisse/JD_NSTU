#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "GameObject.hpp"
#include "Components.hpp"


#include <stdio.h>

namespace JustDrive {
    GameObject::GameObject() {
        transform = Components::Transform();
    }

    GameObject::GameObject(Components::Transform Transform) {
        transform = Transform;
    }

    GameObject::GameObject(glm::vec3 Position) {
        transform = Components::Transform(Position);
    }
}
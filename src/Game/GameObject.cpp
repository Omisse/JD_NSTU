#include "GameObject.hpp"
#include "Components.hpp"

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
#include "Components.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace JustDrive::Components {

    Transform::Transform() {
        position = glm::vec3(0.0f);
        rotation = glm::vec3(0.0f);
        scale = glm::vec3(1.0f,1.0f,1.0f);
        recountVectors();
    }

    Transform::Transform(glm::vec3 Position) {
        position = Position;
        rotation = glm::vec3(0.0f);
        scale = glm::vec3(1.0f,1.0f,1.0f);
        recountVectors();
    }

    glm::vec3 Transform::getPosition() {
        return position;
    }

    glm::vec3 Transform::getRotation() {
        return rotation;
    }

    glm::vec3 Transform::getScale() {
        return scale;
    }

    void Transform::setPosition(glm::vec3 new_position) {
        position = new_position;
    }

    void Transform::setRotation(glm::vec3 angles_radian) {
        rotation = angles_radian;
        recountVectors();
    }

    void Transform::setScale(glm::vec3 axis_scale) {
        scale = axis_scale;
        recountVectors();
    }

    glm::vec3 Transform::getForward() {
        return forward;
    }
    glm::vec3 Transform::getUp() {
        return up;
    }

    glm::vec3 Transform::getRight() {
        return right;
    }

    glm::mat4 Transform::getTransform() {
        glm::mat4 transform =  glm::scale(glm::mat4(1.0f), scale);
        transform = glm::translate(transform, position);
        transform = glm::rotate(transform, rotation.y, glm::vec3(0.0f,1.0f, 0.0f));
        transform = glm::rotate(transform, rotation.x, glm::vec3(1.0f,0.0f, 0.0f));
        transform = glm::rotate(transform, rotation.z, glm::vec3(0.0f,0.0f, 1.0f));
        return transform;

        //return glm::lookAt(position, position+forward, up);
    }   


    void Transform::recountVectors() {
        const glm::mat3 rotX(
            1.0f, 0.0f, 0.0f,
            0.0f, glm::cos(rotation.x), -glm::sin(rotation.x),
            0.0f, glm::sin(rotation.x), glm::cos(rotation.x)
        );
        const glm::mat3 rotY(
            glm::cos(rotation.y), 0.0f, -glm::sin(rotation.y),
            0.0f, 1.0f , 0.0f,
            glm::sin(rotation.y), 0.0f, glm::cos(rotation.y)
        );
        const glm::mat3 rotZ(
            glm::cos(rotation.z), -glm::sin(rotation.z), 0.0f,
            glm::sin(rotation.z), glm::cos(rotation.z), 0.0f,
            0.0f, 0.0f, 1.0f
        );
        forward = glm::vec3(1.0f,0.0f,0.0f)*rotY*rotX*rotZ;
        right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));
        up = glm::cross(right, forward);
    }
}
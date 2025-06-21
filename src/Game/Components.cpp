#include "Components.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace JustDrive::Components {

    /*
    выглядит логично
    */
    Transform::Transform() {
        position = glm::vec3(0.0f);
        rotation = glm::vec3(0.0f);
        scale = glm::vec3(1.0f,1.0f,1.0f);
        recountVectors();
    }

    /*
    выглядит логично
    */
    Transform::Transform(glm::vec3 Position) {
        position = Position;
        rotation = glm::vec3(0.0f);
        scale = glm::vec3(1.0f,1.0f,1.0f);
        recountVectors();
    }

    /*
    выглядит логично
    */
    glm::vec3 Transform::getPosition() {
        return position;
    }

    /*
    выглядит логично
    */
    glm::vec3 Transform::getRotation() {
        return rotation;
    }

    /*
    выглядит логично
    */
    glm::vec3 Transform::getScale() {
        return scale;
    }

    /*
    выглядит логично
    */
    void Transform::setPosition(glm::vec3 new_position) {
        position = new_position;
    }

    /*
    тут пересчитываем, чтобы держать нашу ориентацию правильной
    */
    void Transform::setRotation(glm::vec3 angles_radian) {
        rotation = angles_radian;
        recountVectors();
    }

    /*
    тут пересчитываем на всякий случай, но вообще - не надо
    если честно, это осталось с начала проекта двухмесячной давности
    */
    void Transform::setScale(glm::vec3 axis_scale) {
        scale = axis_scale;
        recountVectors();
    }
    /*
    выглядит логично
    */
    glm::vec3 Transform::getForward() {
        return forward;
    }
    /*
    выглядит логично
    */
    glm::vec3 Transform::getUp() {
        return up;
    }
    /*
    выглядит логично
    */
    glm::vec3 Transform::getRight() {
        return right;
    }
    /*
    как делаем матрицу:
    берём единичную, её масштабируем
    потом перемещаем, ведь нас интересует поворачиваться на месте, а не от центра
    потом _поворачиваем_, по трём осям в отдельности
    наверное, можно и вместе как-то, кватернионами к примеру
    но так оно понятнее выглядит
    */
    glm::mat4 Transform::getTransform() {
        glm::mat4 transform =  glm::scale(glm::mat4(1.0f), scale);
        transform = glm::translate(transform, position);
        transform = glm::rotate(transform, rotation.y, glm::vec3(0.0f,1.0f, 0.0f));
        transform = glm::rotate(transform, rotation.x, glm::vec3(1.0f,0.0f, 0.0f));
        transform = glm::rotate(transform, rotation.z, glm::vec3(0.0f,0.0f, 1.0f));
        return transform;
    }   


    void Transform::recountVectors() {
        /*
        наши любимые матрицы поворотов вдоль оси
        */
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
        /*
        ну и вектор вперёд - вектор вида 0,0,z, который
        мы крутим по нашим радианам поворота
        */
        forward = glm::vec3(0.0f,0.0f,1.0f)*rotY*rotX*rotZ;
        //русский не знаю, находим вектор, ортогональный "вперёд" и мировому "вверх"
        right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));
        /*
        получили вектор "вправо"
        ну а ортогональный ему и "вперёд", что логично - локальный "вверх"
        */
        up = glm::cross(right, forward);
    }
}
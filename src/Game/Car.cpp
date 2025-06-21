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
        /*
        то что выше мы уже видели, тут - инициализация трансформы
        */
        GameObject::transform = Components::Transform(position);
        GameObject::transform.setRotation(rotation_rad);
        //угол приводим к радианам, да и по модулю возьмём, а то вдруг приколы пишут
        maxWheelAngle = glm::radians(glm::abs(mWheelAngle));
        //текущая скорость - 0
        currentVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
        //текущий угол - 0
        currentWheelAngle = 0.0f;
    }

    void Car::initialize(GLFWwindow* window) {
        //нам это не надо, но понадобилось бы, если бы мы ждали инициализации других классов
        return;
    }

    void Car::run(GLFWwindow* window, float deltaTime) {
        /*
        вектор ввода пользователя:
        .x = w-s (вперёд - назад)
        .y = a-d (влево - вправо)
        */
        glm::vec2 userInput(
            (float)(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            -(float)(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS),
            (float)(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            -(float)(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        );

        float accelerationZ = 0.0f; //ускорение "в кадр"
        //тут также условия применения (вперёд меньше максимальной вперёд, аналогично назад)
        if ((currentVelocity.z < maxSpeedForward && userInput.x > 0.0f)||(currentVelocity.z > -maxSpeedBackward && userInput.x < 0.0f)) {
            accelerationZ = userInput.x*deltaTime*acceleration;
        } else {
            accelerationZ = 0.0f;
        }

        //ускоряемся (или замедляемся, если в другую сторону жмём)
        currentVelocity.z += accelerationZ;
        //скорость в мировых координатах (по вектору "вперёд")
        glm::vec3 velocity = currentVelocity.z*transform.getForward();
        //у неё .x в другую сторону, и я не знаю почему
        velocity.x = -velocity.x;

        //сменили позицию на +нашу скорость
        transform.setPosition(transform.getPosition()+deltaTime*velocity);

        //считаем угол
        currentWheelAngle = maxWheelAngle*userInput.y;
        currentWheelAngle *= (maxSpeedForward-glm::abs(currentVelocity.z));
        if (glm::abs(currentWheelAngle) < maxWheelAngle/3.0f) {
            currentWheelAngle = userInput.y*maxWheelAngle/3.0f;
        }
        //поворачиваемся вдоль оси Y
        if (currentVelocity.z >= maxSpeedForward/10.0f) //если она меньше, нет смысла считать, мы там никуда не уедем
        transform.setRotation(transform.getRotation()+glm::vec3(
            0,
            //Суть такова - чем медленнее, тем острее поворот, поэтому смотрим на дельту скорости и максимума вперёд
            //А домножаем на смешные числа потому что это _радианы_
            0.3f*currentWheelAngle*deltaTime,
            0
        ));
    }

    glm::mat4 Car::getTransform() {
        //Вот то, о чём я говорил - машинку приходится масштабировать, потом крутить, а после ещё и двигать вверх.
        return glm::translate(glm::rotate(glm::scale(transform.getTransform(),glm::vec3(0.2f,0.2f,0.2f)), 3.1415f/2.0f, transform.getUp()), glm::vec3(0.0f, 0.4f, 0.0f));
    }
}
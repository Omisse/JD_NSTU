#include <glm/ext/quaternion_common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"
#include "Components.hpp"
#include "GameObject.hpp"

namespace JustDrive {    
    Camera::Camera(glm::vec3 position, glm::vec3 rotation) {
        GameObject::transform = Components::Transform(position);
        GameObject::transform.setRotation(rotation);
        sensitivity = 0.003f;
        moveSpeed = 2.0f;
        yFov = 45.0f;
    }

    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(
            GameObject::transform.getPosition(),
            GameObject::transform.getPosition()+GameObject::transform.getForward(),
            GameObject::transform.getUp()
        );
    }

    void Camera::initialize(GLFWwindow* window) {
        return;
    }

    void Camera::run(GLFWwindow* window, float deltaTime) {
        static double xPosOld = 0.0f;
        static double yPosOld = 0.0f;

        float zAxisMovement = (float)(glfwGetKey(window, GLFW_KEY_W))-(float)(glfwGetKey(window, GLFW_KEY_S));
        float xAxisMovement = (float)-(glfwGetKey(window, GLFW_KEY_A))+(float)(glfwGetKey(window, GLFW_KEY_D));
        float yAxisMovement = (float)(glfwGetKey(window, GLFW_KEY_SPACE))-(float)(glfwGetKey(window, GLFW_KEY_C));

        glm::vec3 globalMovement = glm::vec3(0.0f);

        globalMovement += zAxisMovement*GameObject::transform.getForward();
        globalMovement += xAxisMovement*GameObject::transform.getRight();
        globalMovement += yAxisMovement*glm::vec3(0.0f, 1.0f, 0.0f);
        
        GameObject::transform.setPosition(GameObject::transform.getPosition()+moveSpeed*globalMovement*deltaTime);

        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        float xOffset = (xPos-xPosOld)*sensitivity;
        float yOffset = (yPos-yPosOld)*-sensitivity;
        
        glm::vec3 rotation = GameObject::transform.getRotation();
        rotation.y += xOffset;
        rotation.x += yOffset;
        const float xRotationAbs = glm::abs(rotation.x);

        if (xRotationAbs > 89.0f*3.1415f/180.f) {
            rotation.x = glm::sign(rotation.x)*89.0f*3.1415f/180.f;
        }

        xPosOld = xPos;
        yPosOld = yPos;

        GameObject::transform.setRotation(rotation);

        //printf("Rotation: (%.2f,%.2f,%.2f)\n", (GameObject::transform.getRotation().x)*180/3.1415f,GameObject::transform.getRotation().y*180/3.1415f,GameObject::transform.getRotation().z*180/3.1415f);
    }

    /*
    выглядит логично, поле обзора пока константой тыкнем
    */
    CarCamera::CarCamera(glm::vec3 position, float lerpMultiplier, float minDistance, Components::Transform* targetPosition) : lerpMultiplier(lerpMultiplier), minDistance(minDistance), targetPosition(targetPosition) {
        GameObject::transform = Components::Transform(position);
        yFov = 45.0f;
    }

    void CarCamera::initialize(GLFWwindow* window) {
        //нам не особо надо, а в целом - пригодится
        return;
    }

    void CarCamera::run(GLFWwindow* window, float deltaTime) {
        //наша позиция
        glm::vec3 camPos = GameObject::transform.getPosition();
        /*
        вектор в направлении цели
        +в конце это прикол,
        просто мы хотим смотреть не куда-то в трансмиссию, а желательно в область крыши машины.
        */
        glm::vec3 directionVector = targetPosition->getPosition()-camPos+glm::vec3(0.0f, 0.5f, 0.0f);
        //длина этого вектора - минимальная дистанция, сколько нужно покрыть
        float distanceToCover = glm::length(directionVector)-minDistance;
        //вектор перемещения - покрытие на нормализованный вектор направления, что удобно
        glm::vec3 movementVector = distanceToCover*glm::normalize(directionVector);
        //итоговая позиция цели
        glm::vec3 movementTargetPosition = camPos+movementVector;
        //позиция по итогам кадра, линейная интерполяция текущих и целевых координат по deltaTime*lerpMultiplier
        glm::vec3 frameMovement = camPos + (deltaTime*lerpMultiplier)*(movementTargetPosition-camPos); // actually /(1-0)
        //меняем только позицию, взгляд за нас getViewMatrix() вернёт.
        GameObject::transform.setPosition(frameMovement);
    }

    glm::mat4 CarCamera::getViewMatrix() {
        //Что я и говорил. В целом lookAt сомнительно справляется со своими задачами, но нам - подходит.
        return glm::lookAt(GameObject::transform.getPosition(), targetPosition->getPosition(), GameObject::transform.getUp());
    }
}
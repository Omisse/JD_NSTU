#include "Components.hpp"
#include "GameObject.hpp"

namespace JustDrive {
    class CarCamera : public GameObject {
        public:
            //конструктор
            CarCamera(glm::vec3 position, float lerpMultiplier, float minDistance, Components::Transform* targetPosition);
            //переписываем вещи из предка
            virtual void initialize(GLFWwindow* window) override;
            virtual void run(GLFWwindow* window, float deltaTime) override;
            //тутъ вернём нашу матрицу для шейдеров
            glm::mat4 getViewMatrix();
            //это пользуем в матрице проекции
            float yFov;
        private:
            //множитель интенсивности интерполяции
            float lerpMultiplier;
            //дистанция до цели, минимальная
            float minDistance;
            //указатель на трансформу цели (как удобно, знаем об изменениях!)
            Components::Transform* targetPosition;
    };
    
    class Camera: public GameObject {
        public:
            Camera(glm::vec3 position, glm::vec3 rotation);
            glm::mat4 getViewMatrix();

            virtual void initialize(GLFWwindow* window) override;
            virtual void run(GLFWwindow* window, float deltaTime) override;

            float sensitivity;
            float moveSpeed;
            float yFov = 45.0f;

    };
}
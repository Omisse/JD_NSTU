#include "Components.hpp"
#include "GameObject.hpp"

namespace JustDrive {
    class CarCamera : public GameObject {
        public:
            CarCamera(glm::vec3 position, float lerpMultiplier, float minDistance, Components::Transform* targetPosition);
            virtual void initialize(GLFWwindow* window) override;
            virtual void run(GLFWwindow* window, float deltaTime) override;
            glm::mat4 getViewMatrix();
            float yFov;
        private:
            float lerpMultiplier;
            float minDistance;
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
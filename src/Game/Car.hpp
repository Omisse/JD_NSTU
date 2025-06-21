#ifndef CAR_HPP
#define CAR_HPP

#include "GameObject.hpp"

namespace JustDrive {
    class Car : public GameObject {
        public:
            Car(
                glm::vec3 position,
                glm::vec3 rotation_rad,
                float maxSpeedForward,
                float maxSpeedbackward,
                float acceleration,
                float tractionParallel,
                float tractionCross,
                float maxWheelAngle
            );
            virtual void initialize(GLFWwindow* window) override;
            virtual void run(GLFWwindow* window, float deltaTime) override;
            glm::mat4 getTransform();
        private:
            float maxSpeedForward;
            float maxSpeedBackward;
            float acceleration;
            float tractionParallel;
            float tractionCross;
            float maxWheelAngle;
            glm::vec3 currentVelocity;
            float currentWheelAngle;
    };
}

#endif


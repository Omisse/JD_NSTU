#ifndef CAR_HPP
#define CAR_HPP

#include "GameObject.hpp"

namespace JustDrive {
    class Car : public GameObject {
        public:
            Car( //конструктор
                glm::vec3 position,
                glm::vec3 rotation_rad,
                float maxSpeedForward,
                float maxSpeedbackward,
                float acceleration,
                float tractionParallel,
                float tractionCross,
                float maxWheelAngle
            );
            //перезаписывам полученное от GameObject
            virtual void initialize(GLFWwindow* window) override;
            virtual void run(GLFWwindow* window, float deltaTime) override;
            /*
            тут вернули свою трансформу, потому что машинка - кривая
            с точки зрения ООП это делать неправильно, и вообще должно управляться не отсюда
            */
            glm::mat4 getTransform();
        private:
            float maxSpeedForward; //макс. скорость вперёд
            float maxSpeedBackward; //макс. скорость назад
            float acceleration; //ускорение
            float tractionParallel; //трение вдоль "переда"
            float tractionCross; //трение ортогонально "переду"
            float maxWheelAngle; //угол поворота колёс, максимальный
            glm::vec3 currentVelocity; //текущая скорость, локальная
            float currentWheelAngle; //текущий угол поворота колёс
    };
}

#endif


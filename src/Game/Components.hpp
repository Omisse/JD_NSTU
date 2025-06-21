#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <glm/glm.hpp>

namespace JustDrive::Components {
    class Transform {
        public:
            Transform(); //конструктор, отдающий нули
            Transform(glm::vec3 position); //конструктор, пишущий позицию сразу

            /*
            своего рода геттеры-сеттеры нужных переменных
            */
            glm::vec3 getPosition();
            void setPosition(glm::vec3 position);
            glm::vec3 getRotation();
            void setRotation(glm::vec3 radians);
            glm::vec3 getScale();
            void setScale(glm::vec3 axis_scale);

            /*
            геттеры векторов ориентации, сеттеры не предполагаются
            */
            glm::vec3 getForward();
            glm::vec3 getUp();
            glm::vec3 getRight();
            //Тут конструируем матрицу Model
            glm::mat4 getTransform();            

        private:
            glm::vec3 position;//позиция
            glm::vec3 rotation;//поворот
            glm::vec3 scale;//масштаб

            glm::vec3 forward;//вектор "вперёд"
            glm::vec3 up;//вектор "вправа" (считаем что "вверх" положителен)
            glm::vec3 right;//вектор "вверх"

            void recountVectors(); //это пересчитает векторы ориентации, вызываем из сеттеров.

    };
}


#endif
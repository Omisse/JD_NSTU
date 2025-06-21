#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <glm/glm.hpp>

namespace JustDrive::Components {
    class Transform {
        public:
            Transform();
            Transform(glm::vec3 position);

            glm::vec3 getPosition();
            void setPosition(glm::vec3 position);
            glm::vec3 getRotation();
            void setRotation(glm::vec3 radians);
            glm::vec3 getScale();
            void setScale(glm::vec3 axis_scale);

            glm::vec3 getForward();
            glm::vec3 getUp();
            glm::vec3 getRight();
            glm::mat4 getTransform();            

        private:
            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 scale;

            glm::vec3 forward;
            glm::vec3 up;
            glm::vec3 right;

            void recountVectors();

    };
}


#endif
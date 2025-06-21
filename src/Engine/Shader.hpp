#ifndef SHADER_HPP
#define SHADER_HPP

#include <cstdint>
#include <string>

#include <glm/glm.hpp>

namespace Engine::Render {
    class Shader {
        public:
            uint32_t ID; //адрес в GLFW-пространстве
            //конструктор
            Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
            //gl_useProgram(id), говорим что работаем сейчас с этим шейдером
            void use();
            //куча функций которые устанавливают uniform нужных типов
            //const подразумевает, что мы не поменяем данные класса
            void setBool(const std::string &name, bool value) const;
            void setInt(const std::string &name, int value) const;
            void setFloat(const std::string &name, float value) const;
            void setVec2(const std::string &name, const glm::vec2 &value) const;
            void setVec3(const std::string &name, const glm::vec3 &value) const;
            void setVec4(const std::string &name, const glm::vec4 &value) const;
            void setMat2(const std::string &name, const glm::mat2 &value) const;
            void setMat3(const std::string &name, const glm::mat3 &value) const;
            void setMat4(const std::string &name, const glm::mat4 &value) const;

        private:
            //маленький енум чтобы было приятно
            enum ShaderType {
                SHADER_T_VERT = 1 << 0,
                SHADER_T_FRAG = 1 << 1,
            };
            //читаем шейдер из файла
            std::string readShader(const char* path, ShaderType type);
    };
}
#endif
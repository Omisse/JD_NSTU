#ifndef SHADER_HPP
#define SHADER_HPP

#include <cstdint>
#include <string>

#include <glm/glm.hpp>

namespace Engine::Render {
    class Shader {
        public:
            uint32_t ID;
            Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
            void use();
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
            enum ShaderType {
                SHADER_T_VERT = 1 << 0,
                SHADER_T_FRAG = 1 << 1,
            };

            std::string readShader(const char* path, ShaderType type);
    };
}
#endif
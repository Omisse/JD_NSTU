#include <cstdint>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

#include <GLFW/glad/gl.h>

#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"

namespace Engine::Render {
    std::string Shader::readShader(const char* path, ShaderType type) {
        std::string code;
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            file.open(path);
            std::stringstream stream;
            stream << file.rdbuf();
            file.close();
            code = stream.str();
        } catch (std::ifstream::failure err) {
            fprintf(stderr, "Error: Shader: %s: Cannot read!\n", path);
            code = std::string("");
        }
        return code;
    }

    Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
        std::string vShaderCode = readShader(vertexShaderPath, Shader::ShaderType::SHADER_T_VERT);
        std::string fShaderCode = readShader(fragmentShaderPath, Shader::ShaderType::SHADER_T_FRAG);


        const char* vfS_cstring[2] = {vShaderCode.c_str(), fShaderCode.c_str()};
        uint32_t SID[2] = {};
        int success;
        char info[BUFSIZ];

        for(uint32_t i = 0; i < 2; i++) {
            SID[i] = glCreateShader(i == 0 ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
            glShaderSource(SID[i], 1, &(vfS_cstring[i]), NULL);
            glCompileShader(SID[i]);
            glGetShaderiv(SID[i], GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(SID[i], BUFSIZ, NULL, info);
                const char* patherr = i == 0 ? vertexShaderPath : fragmentShaderPath;
                fprintf(stderr, "Error: Shader: %s: Cannot compile!:\n%s\n", patherr, info);
                glDeleteShader(SID[i]);
            }
        }

        ID = glCreateProgram();
        glAttachShader(ID, SID[0]);
        glAttachShader(ID, SID[1]);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, BUFSIZ, NULL, info);
            fprintf(stderr, "Error: Shader: PID: %u: Cannot link!:\n%s\n", ID, info);
        }
    }



    void Shader::use() {
        glUseProgram(ID);
    }

    void Shader::setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    void Shader::setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    void Shader::setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
}


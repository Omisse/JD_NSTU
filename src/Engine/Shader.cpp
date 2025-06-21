#include <cstdint>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"

namespace Engine::Render {
    //чтение из файла
    std::string Shader::readShader(const char* path, ShaderType type) {
        //захотели поработать в плюсовых стрингах, разнообразия ради
        std::string code;
        //установили ээээ дескриптор потока, назвали file
        std::ifstream file;
        //сказали, что ошибками считать битость файла
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            //открыли
            file.open(path);
            //создали поток
            std::stringstream stream;
            //засунули буфер из файла в, по сути, basic_string
            stream << file.rdbuf();
            //закрыли
            file.close();
            //засунули из потока в, вот диво-то, basic_string
            code = stream.str();
        } catch (std::ifstream::failure err) {
            //если стало плохо, ругаемся
            fprintf(stderr, "Error: Shader: %s: Cannot read!\n", path);
            //и чистим что успело записаться, на всякий
            code = std::string("");
        }
        //вернули записанное
        return code;
    }

    //конструктор, компиляция
    Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
        /*
        получили коды, тип могли бы и не совать
        */
        std::string vShaderCode = readShader(vertexShaderPath, Shader::ShaderType::SHADER_T_VERT);
        std::string fShaderCode = readShader(fragmentShaderPath, Shader::ShaderType::SHADER_T_FRAG);

        //собрали массив указателей, так чуть проще работать
        const char* vfS_cstring[2] = {vShaderCode.c_str(), fShaderCode.c_str()};
        //собрали массив айдишников, аналогично
        uint32_t SID[2] = {};
        //флаг
        int success;
        //используем при проблемах
        char info[BUFSIZ];

        for(uint32_t i = 0; i < 2; i++) {
            //сказали, что наш айдишник - результат генерации места под шейдер нашего типа.
            SID[i] = glCreateShader(i == 0 ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
            //сказали, что текст брать из строки [i]
            glShaderSource(SID[i], 1, &(vfS_cstring[i]), NULL);
            //скомпилировали
            glCompileShader(SID[i]);
            //получаем инфу об ошибках компиляции
            glGetShaderiv(SID[i], GL_COMPILE_STATUS, &success);
            if (!success) {
                //если сломалось - пишем в info, что сломалось
                glGetShaderInfoLog(SID[i], BUFSIZ, NULL, info);
                //небольшой переключатель между двумя загруженными путями
                const char* patherr = i == 0 ? vertexShaderPath : fragmentShaderPath;
                //ругаемся
                fprintf(stderr, "Error: Shader: %s: Cannot compile!:\n%s\n", patherr, info);
                //удаляем лишнее сразу
                glDeleteShader(SID[i]);
            }
        }

        //айдишник основного шейдера привязываем тут
        ID = glCreateProgram();
        //к "программе" привязываем два наших шейдера
        glAttachShader(ID, SID[0]);
        glAttachShader(ID, SID[1]);
        //и привязываем программу к потоку отрисовки
        glLinkProgram(ID);
        //Спросили, всё ли ок
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            //если не ок - получаем инфу и ругаемся
            glGetProgramInfoLog(ID, BUFSIZ, NULL, info);
            fprintf(stderr, "Error: Shader: PID: %u: Cannot link!:\n%s\n", ID, info);
        }
        /*
        если ранее был успех-победа, то шейдеры можно удалять, они уже в программе
        */
        if (success) {
            glDeleteShader(SID[0]);
            glDeleteShader(SID[1]);
        }
    }


    //glUseProgram(id), говорим что работаем сейчас с этим шейдером
    void Shader::use() {
        glUseProgram(ID);
    }

    /*
    Следющие функции однотипны, придерживаются единой логики:
    установить_значение(по_адресу(шейдер, название_переменной), значение)
    */
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


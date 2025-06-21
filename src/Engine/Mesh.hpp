#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <cstdint>
#include <string>

#include <glm/glm.hpp>

#include "Shader.hpp"

namespace Engine::Render {
    /*
    Создадим enum, чтобы упростить нашу работу на момент импорта текстур.
    */
    enum TextureTypes { 
        TEXTURE_DIFFUSE_T = 1 << 0,
        TEXTURE_SPECULAR_T = 1 << 1,
        TEXTURE_NORMAL_T = 1 << 2,
        TEXTURE_HEIGHT_T = 1 << 3,
    };

    /*
    Структура данных точки(вертекса) модели
    */
    struct Vertex {
        glm::vec3 Pos; //позиция.xyz
        glm::vec3 Normal; //нормаль(импортирована).xyz
        glm::vec2 UV; //текстурные координаты .xy (.uv)
        glm::vec4 Color; //цвет вертекса (порой используется в создании модели).rgba
    };

    /*
    Структура данных текстуры в буфере GLFW
    */
    struct Texture {
        uint32_t id; //имя в адресном пространстве буфера
        TextureTypes type; //тип текстуры (согласно TextureTypes)
        int width, height, channels; //параметры размера импортированного изображения и количества цветовых каналов.
        std::string path; //путь до текстуры(используется при дедупликации текстур на этапе импорта)
    };


    class Mesh {
        public:
            std::vector<Vertex> vertices; //вектор точек
            std::vector<uint32_t> indices; //вектор очерёдности точек при построении сетки
            std::vector<Texture> textures; //вектор текстур
            /*
            конструктор класса
            */
            Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const std::vector<Texture> &textures);
            void Draw(Shader &shader); //функция отрисовки
            //Имя итогового массива вертексов(и их аттрибутов) в адресном пространстве GLFW, вынесено в public потому что иногда нужно
            uint32_t VAO;

        private:
            uint32_t VBO, EBO; //имена массивов позиций точек и их очерёдности
            void construct(); //вызывается в конструкторе.
    };
}

#endif
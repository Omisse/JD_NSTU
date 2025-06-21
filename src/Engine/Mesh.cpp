#include <cstdint>
#include <string>

#include <glad/glad.h>

#include "Mesh.hpp"

namespace Engine::Render {
    Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const std::vector<Texture> &textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        /*
        собираем из переданных параметров нашу сетку, вызываем construct()
        */
        construct();
    }


    void Mesh::construct() {
        glGenVertexArrays(1, &VAO); //выделить место под VAO в адресах GLFW
        glGenBuffers(1, &VBO); //выделить место под VBO
        glGenBuffers(1, &EBO); //выделить место под EBO

        glBindVertexArray(VAO); //==сейчас работаем с массивом под этим id
        //(помним, что typeof(VAO) == uint32_t)

        glBindBuffer(GL_ARRAY_BUFFER, VBO); //==сейчас работаем с этим буфером.
        /*
        как здорово, что в VBO можно сразу залить весь вектор, и он будет работать.
        STATIC_DRAW говорит что эти данные не будут меняться после.
        */
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //==сейчас работаем с этим буфером.
        /*
        Аналогичная операция для EBO.
        */
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);//включаем, условно, массив аттрибутов [0], в пространстве VAO.
        /*
        благодаря тому что вертекс хранит данные в виде
        различных видов glm::
        мы можем пользовать offsetof().
        Новые данные уже не загружаются, мы просто говорим как они расположены в VBO.
        */
        /*
        0 - индекс буфера, 3 - количество элементов, GL_FLOAT - тип элемента
        sizeof() - везде один и тот же, мы просто говорим,
        что следующий такой элемент встретится через n байт.
        offsetof - получаем указатель, который сдвинет нам память на сколько нужно от начала структуры
        */

        glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Pos));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, UV));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3,4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Color));

        //хорошей практикой считается освобождать привязку массивов после работы.
        glBindVertexArray(0);
    }

    void Mesh::Draw(Shader& shader) {
        /*
        шейдеры хранят uniform поимённо, поэтому мы прямо в момент отрисовки конструируем их имена.
        всё это основано только на замысле конкретного программиста, лишь бы работало
        */
        uint32_t diffuseNum = 1;
        uint32_t specularNum = 1;
        uint32_t normalNum = 1;
        uint32_t heightNum = 1;

        for (uint32_t i = 0; i < textures.size(); i++) {
            //говорим, что сейчас в шейдер пойдёт текстура за номером i(у шейдера свой layout)
            glActiveTexture(GL_TEXTURE0 + i);
            std::string uniformName;
            if (textures[i].type == TextureTypes::TEXTURE_DIFFUSE_T) {
                uniformName = std::string("texture_diffuse")+std::to_string(diffuseNum++);
            } else if (textures[i].type == TextureTypes::TEXTURE_SPECULAR_T) {
                uniformName = std::string("texture_specular")+std::to_string(specularNum++);;
            } else if (textures[i].type == TextureTypes::TEXTURE_NORMAL_T) {
                uniformName = std::string("texture_normal")+std::to_string(normalNum++);;
            } else if (textures[i].type == TextureTypes::TEXTURE_HEIGHT_T) {
                uniformName = std::string("texture_height")+std::to_string(heightNum++);;
            }
            //прибиваем в память названную текстуру
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
            /*
            говорим, что в шейдере на позиции uniformName расположена наша текстура.
            i не слишком важен, мы можем засунуть туда любое значение,
            лишь бы они отличались от uniform к uniform, поскольку
            данное значение теперь прибито гвоздями к нашей текстуре. (как и её.id, но в памяти шейдера)
            */
            shader.setInt(uniformName.c_str(), i);
        }
        //Говорим, что сейчас будем работать с массивом в GLFW-памяти, по адресу из нашего VAO.
        glBindVertexArray(this->VAO);
        /*
        Рисуем треугольники, точек будет столько, сколько есть в indices
        GL_UNSIGNED_INT - тип данных indices.
        */
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        //опять-таки, сбрасываем связь с массивом
        glBindVertexArray(0);
        //и с текстурами!
        glActiveTexture(GL_TEXTURE0);
    }
}



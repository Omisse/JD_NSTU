#ifndef MODEL_HPP
#define MODEL_HPP

#include <cstdint>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>


#include "Mesh.hpp"
#include "Shader.hpp"


namespace Engine::Render {
    class Model {
        public:
            Model(const char* path); //конструктор
            void Draw(Shader &shader); //"рисователь" модели, передаст всё в меши.
            std::vector<Mesh> meshes; //вектор из сеток данной модели
            
        private:
            std::vector<Texture> loadedTextures; //вектор текстур, загруженных при импорте
            std::string dir; //строка, использующаяся чтобы разрешать пути текстур

            void load(const char* path); //вызывается в конструкторе
            void processNode(aiNode* node, const aiScene* scene); //вызывается в load, работаем с assimp
            Mesh processMesh(aiMesh* mesh, const aiScene* scene); //вызывается в processNode, работаем с assimp
            /*
            вызывается в processMesh, работаем с assimp
            */
            std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureTypes engineType);
    };

    /*
    Вызывается в loadMaterialTextures, но оставлена вне модели
    т.к. де-юре не её часть.
    Работаем с assimp и stbi.
    */
    uint32_t LoadTexture(const char* path, const std::string& dir);
}

#endif
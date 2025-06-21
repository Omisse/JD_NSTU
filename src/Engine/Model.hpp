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
            Model(const char* path);
            void Draw(Shader &shader);
            std::vector<Mesh> meshes;
            
        private:
            std::vector<Texture> loadedTextures;
            std::string dir;

            void load(const char* path);
            void processNode(aiNode* node, const aiScene* scene);
            Mesh processMesh(aiMesh* mesh, const aiScene* scene);
            std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureTypes engineType);
    };

    uint32_t LoadTexture(const char* path, const std::string& dir);
}

#endif
#include <cstdint>
#include <string>
#include <cstdio>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <GLFW/glad/gl.h>

#include <stbi/stb_image.h>


#include "Model.hpp"
#include "Shader.hpp"
#include "assimp/color4.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/types.h"



namespace Engine::Render {
    Model::Model(const char* path) {
        Model::load(path);
    }

    void Model::Draw(Shader &shader) {
        for (uint32_t i = 0; i<meshes.size(); i++) {
            meshes[i].Draw(shader);
        }
    }

    void Model::load(const char* path) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate|aiProcess_FlipUVs|aiProcess_GenNormals|aiProcess_GenUVCoords|aiProcess_FindDegenerates|aiProcess_FindInvalidData|aiProcess_FixInfacingNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            fprintf(stderr, "Error: Assimp: %s\n", importer.GetErrorString());
            return;
        }

        dir = std::string(path);
        dir = dir.substr(0,dir.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode* node, const aiScene* scene) {
        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Texture> textures;

        for (uint32_t i = 0; i < mesh->mNumVertices; i++) { 
            Vertex vertex;
            vertex.Pos = glm::vec3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );
            vertex.Normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
            if (mesh->mTextureCoords[0]) {
                vertex.UV = glm::vec2(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                );
            } else {
                vertex.UV = glm::vec2(0.5f, 0.5f);
            }
            vertices.push_back(vertex);
        }

        for (uint32_t i = 0; i<mesh->mNumFaces; i++) {
            for (uint32_t j = 0; j<mesh->mFaces[i].mNumIndices; j++) {
                indices.push_back(static_cast<uint32_t>(mesh->mFaces[i].mIndices[j]));
            }
        }

        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            for (uint32_t i = 0; i < vertices.size(); i++) {
                aiColor4D color;
                aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color);
                vertices[i].Color.r = color.r;
                vertices[i].Color.g = color.g;
                vertices[i].Color.b = color.b;
            }
            std::vector<Texture> diffMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureTypes::TEXTURE_DIFFUSE_T);
            textures.insert(textures.end(), diffMaps.begin(), diffMaps.end());
            std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, TextureTypes::TEXTURE_NORMAL_T);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureTypes::TEXTURE_SPECULAR_T);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TextureTypes::TEXTURE_HEIGHT_T);
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());   
        }

        return Mesh(vertices,indices,textures);
    }



    std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureTypes engineType) {
        std::vector<Texture> textures;
        for(uint32_t i = 0; i < mat->GetTextureCount(aiType); i++) {
            aiString str;
            mat->GetTexture(aiType, i, &str);
            bool skip = false;
            for (uint32_t j = 0; j < loadedTextures.size() && !skip; j++) {
                if (std::strcmp(loadedTextures[j].path.data(), str.C_Str()) == 0) {
                    textures.push_back(loadedTextures[j]);
                    skip = true;
                }
            }
            if (!skip) {
                Texture texture;
                texture.id = LoadTexture(str.C_Str(), dir);
                texture.type = engineType;
                texture.path = str.C_Str();
                textures.push_back(texture);
                loadedTextures.push_back(texture);
            }
        }

        return textures;
    }

    uint32_t LoadTexture(const char* path, const std::string &dir) {
        std::string fname = std::string(path);
        fname = dir+'/'+fname;
        uint32_t textureID;
        glGenTextures(1, &textureID);
        int width, height, varAmount;
        unsigned char* data = stbi_load(fname.c_str(), &width, &height, &varAmount, 0);

        if (data) {
            GLenum format = varAmount == 1 ? GL_RED :
                            varAmount == 3 ? GL_RGB :
                            varAmount == 4 ? GL_RGBA:
                            GL_RG;
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            fprintf(stderr, "Error: stbi: failed to load: %s\n", path);
        }
        stbi_image_free(data);

        return textureID;
    }
}
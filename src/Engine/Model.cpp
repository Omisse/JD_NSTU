#include <cstdint>
#include <string>
#include <cstdio>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/color4.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/types.h>

#include <glad/glad.h>

#include <stbi/stb_image.h>


#include "Model.hpp"
#include "Shader.hpp"

namespace Engine::Render {
    Model::Model(const char* path) {
        //да, конструктор вызывает load и больше ничего, можно бы и сократить
        Model::load(path); 
    }

    void Model::Draw(Shader &shader) {
        /*
        тоже выглядит понятно, просто для каждой сетки вызываем draw и передаём шейдер.
        */
        for (uint32_t i = 0; i<meshes.size(); i++) {
            meshes[i].Draw(shader);
        }
    }

    void Model::load(const char* path) {
        Assimp::Importer importer; //ассимповский класс, дай боже здоровья
        /*
        модификаторы:
        aiProcess_Triangulate - если найдены полигоны более чем из трёх точек - сводим к трём.
        aiProcess_FlipUVs - специфическая история для GL-пространства, UV OpenGl и DX11 отличаются.
        aiProcess_GenNormals - если при экспорте потеряли, генерируем нормали
        aiProcess_GenUVCoords - если при экспорте потеряли, генерируем UV
        aiProcess_FindDegenerates - если кто-то забыл точки или сделал кривую сетку - правим
        aiProcess_FindInvalidData - если есть проблемы с структурой данных в файле - правим
        aiProcess_FixInfacingNormals - если забыли развернуть нормали - правим (работает только на кубах и подобном)
        */
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate|aiProcess_FlipUVs|aiProcess_GenNormals|aiProcess_GenUVCoords|aiProcess_FindDegenerates|aiProcess_FindInvalidData|aiProcess_FixInfacingNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            fprintf(stderr, "Error: Assimp: %s\n", importer.GetErrorString());
            //Если сцену не получилось загрузить - ругаемся и выходим.
            return;
        }

        dir = std::string(path);
        //штука для текстур, расскажем позже
        //а в целом - просто убираем название модели и говорим, что теперь работаем вот отсуда
        dir = dir.substr(0,dir.find_last_of('/'));
        
        //assimp загружает нам что-то в виде дерева, начинаем с корня обрабатывать полученное
        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode* node, const aiScene* scene) {
        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            //вытащили меш, сунули себе. processMesh будет дальше.
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            //обрабатываем потомков
            //рекурсия крута
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Texture> textures;

        /*
        собираем меш в данных glm:: из данных assimp::
        */
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
                //Если даже тут нет UV - тыкнем в середину, может хоть чего увидим
                vertex.UV = glm::vec2(0.5f, 0.5f);
            }
            vertices.push_back(vertex); //вертекс готов, суём его в вектор
        }

        //тут тоже всё понятно, разве что static_cast, опять же, приведёт ассимповский тип к нашему.
        for (uint32_t i = 0; i<mesh->mNumFaces; i++) {
            for (uint32_t j = 0; j<mesh->mFaces[i].mNumIndices; j++) {
                indices.push_back(static_cast<uint32_t>(mesh->mFaces[i].mIndices[j]));
            }
        }

        /*
        самое интересное
        материал - сложная штука, разный бывает и параметров немеряно
        тут - самый тупой вариант
        */
        if (mesh->mMaterialIndex >= 0) {
            /*
            вытащили из сцены материал по индексу
            он может повторяться, поэтому в aiNode не лежит.
            */
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            /*
            вытащили из материала цвет, если есть
            засунули его в цвет вертексов, что не совсем верно, но задачу выполняет
            */
            for (uint32_t i = 0; i < vertices.size(); i++) {
                aiColor4D color;
                aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color);
                vertices[i].Color.r = color.r;
                vertices[i].Color.g = color.g;
                vertices[i].Color.b = color.b;
            }
            //Загружаем текстуры, подсовывая их пачкой в вектор текстур.
            std::vector<Texture> diffMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureTypes::TEXTURE_DIFFUSE_T);
            textures.insert(textures.end(), diffMaps.begin(), diffMaps.end());
            std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, TextureTypes::TEXTURE_NORMAL_T);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureTypes::TEXTURE_SPECULAR_T);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TextureTypes::TEXTURE_HEIGHT_T);
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());   
        }

        //возвращаем меш, который нам отдаёт конструктор как rvalue. немного стыдно.
        return Mesh(vertices,indices,textures);
    }



    std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureTypes engineType) {
        std::vector<Texture> textures; //ещё один вектор текстур, потому что надо
        for(uint32_t i = 0; i < mat->GetTextureCount(aiType); i++) {
            aiString str;
            //вытащили название файла текстуры
            mat->GetTexture(aiType, i, &str);
            bool skip = false; //пользуем чтобы пропустить, если уже есть
            //среди загруженных
            for (uint32_t j = 0; j < loadedTextures.size() && !skip; j++) {
                //ищем такие, у которых путь совпадает с текущим
                if (std::strcmp(loadedTextures[j].path.data(), str.C_Str()) == 0) {
                    //если нашли - в наш вектор суётся уже загруженная, загрузку пропускаем
                    textures.push_back(loadedTextures[j]);
                    skip = true;
                }
            }
            //Если всё же не нашли
            if (!skip) {
                //создали текстуру
                Texture texture;
                //загрузили, получили GLFW-айдишник
                texture.id = LoadTexture(str.C_Str(), dir);
                //тип получили ещё на входе
                texture.type = engineType;
                //путь, ну,понятно
                texture.path = str.C_Str();
                //засунули в текущий вектор и в общий тоже
                textures.push_back(texture);
                loadedTextures.push_back(texture);
            }
        }

        //возвращаем вектор отсюда наверх
        return textures;
    }

    uint32_t LoadTexture(const char* path, const std::string &dir) {
        std::string fname = std::string(path);
        fname = dir+'/'+fname;//dir нужен именно за этим, нам отдаёт только название текстуры
        uint32_t textureID; //айди, ничего нового
        glGenTextures(1, &textureID); //говорим, что вот теперь хотим буфер GL-пространства под текстуру
        int width, height, varAmount; //завели переменные аналогичной логики что и в нашей структуре
        /*
        загрузили кучу символов, спасибо stbi
        */
        unsigned char* data = stbi_load(fname.c_str(), &width, &height, &varAmount, 0);

        if (data) {
            /*
            Тут определили, что у нас с каналами
            нам может прийти одноканальная хайтмапа
            двухканальное что-то, но я не могу придумать что
            ну и обычные текстурки в RGB и RGBA
            */
            GLenum format = varAmount == 1 ? GL_RED :
                            varAmount == 3 ? GL_RGB :
                            varAmount == 4 ? GL_RGBA:
                            GL_RG;
            glBindTexture(GL_TEXTURE_2D, textureID); //==сейчас работаем с буфером, который сделали ранее
            /*
            суём в этот буфер 2D!!! текстуру наших параметров
            GL_UNSIGNED_BYTE потому что дата - набор чаров.
            */
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            /*
            заставили GLFW придумать мипмапы для этой текстуры
            чтобы меньше места на видеокарте кушало
            в сущности - изображения в меньшем разрешении, в несколько этапов.
            */
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //сказали что повторяемся по одной оси
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); //сказали что повторяемся по другой
            /*
            говорим, что фильтрация пикселей вдали делается линейно, мипмапами
            вблизи просто линейно
            */
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            //если поломались - поругаемся
            fprintf(stderr, "Error: stbi: failed to load: %s\n", path);
        }
        //удаляем данные stbi, нам они уже не нужны и даром.
        stbi_image_free(data);
        //вернули айдишник, вся текстура уже в памяти
        return textureID;
    }
}
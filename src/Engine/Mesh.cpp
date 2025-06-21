#include <cstdint>
#include <string>

#include <GLFW/glad/gl.h>

#include "Mesh.hpp"

namespace Engine::Render {
    Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const std::vector<Texture> &textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        construct();
    }


    void Mesh::construct() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Pos));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, UV));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3,4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Color));

        glBindVertexArray(0);
    }

    void Mesh::Draw(Shader& shader) {
        uint32_t diffuseNum = 1;
        uint32_t specularNum = 1;
        uint32_t normalNum = 1;
        uint32_t heightNum = 1;

        for (uint32_t i = 0; i < textures.size(); i++) {
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
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
            shader.setInt(uniformName.c_str(), i);
        }
        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }
}



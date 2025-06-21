#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <cstdint>
#include <string>

#include <glm/glm.hpp>

#include "Shader.hpp"

namespace Engine::Render {
    enum TextureTypes { 
        TEXTURE_DIFFUSE_T = 1 << 0,
        TEXTURE_SPECULAR_T = 1 << 1,
        TEXTURE_NORMAL_T = 1 << 2,
        TEXTURE_HEIGHT_T = 1 << 3,
    };

    struct Vertex {
        glm::vec3 Pos;
        glm::vec3 Normal;
        glm::vec2 UV;
        glm::vec4 Color;
    };

    struct Texture {
        uint32_t id;
        TextureTypes type;
        int width, height, channels;
        std::string path;
    };


    class Mesh {
        public:
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            std::vector<Texture> textures;

            Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const std::vector<Texture> &textures);
            void Draw(Shader &shader);
            uint32_t VAO;

        private:
            uint32_t VBO, EBO;
            void construct();
    };
}

#endif



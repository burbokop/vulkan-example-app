#ifndef MESH_H
#define MESH_H

#include <filesystem>
#include <glm/glm.hpp>
#include <vector>

namespace e172vp {


struct Mesh {
    std::vector <glm::vec3> vertices;
    std::vector <glm::vec2> uvMap;
    std::vector <glm::vec3> normals;

    std::vector<std::uint32_t> vertexIndices;
    std::vector<std::uint32_t> uvIndices;
    std::vector<std::uint32_t> normalIndices;

    Mesh();
    static Mesh load(const std::filesystem::path &path);
};

}

#endif // MESH_H

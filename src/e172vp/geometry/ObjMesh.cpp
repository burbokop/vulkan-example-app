#include "ObjMesh.h"
#include <cstring>

namespace e172vp {

ObjMesh ObjMesh::load(const std::filesystem::path& path)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvMap;
    std::vector<glm::vec3> normals;
    std::vector<std::uint32_t> vertexIndices;
    std::vector<std::uint32_t> uvIndices;
    std::vector<std::uint32_t> normalIndices;

    FILE* file = fopen(path.c_str(), "r");
    if (file == NULL) {
        throw std::runtime_error("Failed to open file " + path.string() + ": " + ::strerror(errno));
    }

    while (true) {
        std::string line;
        if (fscanf(file, "%s", line.data()) == EOF)
            break;
        else if (std::strcmp(line.c_str(), "v") == 0) {
            float x = 0;
            float y = 0;
            float z = 0;
            fscanf(file, "%f %f %f\n", &x, &y, &z);
            vertices.push_back({ x, y, z });
        } else if (std::strcmp(line.c_str(), "vt") == 0) {
            float x = 0;
            float y = 0;
            fscanf(file, "%f %f\n", &x, &y);
            uvMap.push_back({ x, y });
        }

        else if (std::strcmp(line.c_str(), "vn") == 0) {
            float x = 0;
            float y = 0;
            float z = 0;
            fscanf(file, "%f %f %f\n", &x, &y, &z);
            normals.push_back({ x, y, z });
        } else if (std::strcmp(line.c_str(), "f") == 0) {
            uint32_t iv1x = 0;
            uint32_t iv1y = 0;
            uint32_t iv1z = 0;
            uint32_t iv2x = 0;
            uint32_t iv2y = 0;
            uint32_t iv2z = 0;
            uint32_t iv3x = 0;
            uint32_t iv3y = 0;
            uint32_t iv3z = 0;
            fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &iv1x, &iv1y, &iv1z, &iv2x, &iv2y, &iv2z, &iv3x, &iv3y, &iv3z);

            vertexIndices.push_back(iv1x - 1);
            vertexIndices.push_back(iv2x - 1);
            vertexIndices.push_back(iv3x - 1);

            uvIndices.push_back(iv1y - 1);
            uvIndices.push_back(iv2y - 1);
            uvIndices.push_back(iv3y - 1);

            normalIndices.push_back(iv1z - 1);
            normalIndices.push_back(iv2z - 1);
            normalIndices.push_back(iv3z - 1);
        }
    }
    return ObjMesh(
        vertices,
        uvMap,
        normals,
        vertexIndices,
        uvIndices,
        normalIndices);
}

}

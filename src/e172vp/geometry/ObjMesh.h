#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <vector>

namespace e172vp {

/// .obj format
class ObjMesh {
public:
    static ObjMesh load(const std::filesystem::path& path);

    ObjMesh(const ObjMesh&) = delete;
    ObjMesh& operator=(const ObjMesh&) = delete;
    ObjMesh(ObjMesh&&) = default;
    ObjMesh& operator=(ObjMesh&&) = default;

    ObjMesh(
        std::vector<glm::vec3> vertices,
        std::vector<glm::vec2> uvs,
        std::vector<glm::vec3> normals,
        std::vector<std::uint32_t> vertexIndices,
        std::vector<std::uint32_t> uvIndices,
        std::vector<std::uint32_t> normalIndices)
        : m_vertices(std::move(vertices))
        , m_uvs(std::move(uvs))
        , m_normals(std::move(normals))
        , m_vertexIndices(std::move(vertexIndices))
        , m_uvIndices(std::move(uvIndices))
        , m_normalIndices(std::move(normalIndices))
    {
    }

    const auto& vertices() const { return m_vertices; }
    const auto& uvs() const { return m_uvs; }
    const auto& normals() const { return m_normals; }
    const auto& vertexIndices() const { return m_vertexIndices; }
    const auto& uvIndices() const { return m_uvIndices; }
    const auto& normalIndices() const { return m_normalIndices; }

private:
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec2> m_uvs;
    std::vector<glm::vec3> m_normals;
    std::vector<std::uint32_t> m_vertexIndices;
    std::vector<std::uint32_t> m_uvIndices;
    std::vector<std::uint32_t> m_normalIndices;
};

}

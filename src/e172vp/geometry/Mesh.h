#pragma once

#include "Vertex.h"
#include <vector>

namespace e172vp {

class ObjMesh;

/// Internal engine mesh format
class Mesh {
public:
    Mesh(
        std::vector<Vertex> vertices,
        std::vector<std::uint32_t> indices)
        : m_vertices(std::move(vertices))
        , m_indices(std::move(indices))
    {
    }

    static Mesh fromObjMesh(const ObjMesh& mesh, glm::vec3 color);

    const auto& vertices() const { return m_vertices; }
    const auto& indices() const { return m_indices; }

private:
    std::vector<Vertex> m_vertices;
    std::vector<std::uint32_t> m_indices;
};

}

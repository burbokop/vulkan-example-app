#include "Mesh.h"

#include "ObjMesh.h"

#include <string.h>

namespace e172vp {

Mesh Mesh::fromObjMesh(const ObjMesh& mesh, glm::vec3 color)
{
    // TODO pass normales
    // TODO fix uvs indices
    std::vector<Vertex> vertices(mesh.vertices().size());
    for (std::size_t i = 0; i < mesh.vertices().size(); ++i) {
        vertices[i] = { mesh.vertices()[i], color, mesh.uvs()[i] };
    }
    return Mesh(vertices, mesh.vertexIndices());
}

}

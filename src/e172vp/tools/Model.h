#pragma once

#include "../geometry/Mesh.h"

namespace e172vp {

class Model {
public:
    Model(
        Mesh mesh,
        std::vector<std::uint8_t> vert,
        std::vector<std::uint8_t> frag)
        : m_mesh(std::move(mesh))
        , m_vert(std::move(vert))
        , m_frag(std::move(frag))
    {
    }

    const auto& mesh() const { return m_mesh; };
    const auto& vert() const { return m_vert; }
    const auto& frag() const { return m_frag; }

private:
    Mesh m_mesh;
    std::vector<std::uint8_t> m_vert;
    std::vector<std::uint8_t> m_frag;
};

}

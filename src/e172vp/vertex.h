#ifndef VERTEX_H
#define VERTEX_H

#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

namespace e172vp {

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;

    static std::vector<Vertex> fromGlm(const std::vector<glm::vec3> &vertices, glm::vec3 color);
    static vk::VertexInputBindingDescription bindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions();
};

}
#endif // VERTEX_H

#ifndef VERTEX_H
#define VERTEX_H

#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

namespace e172vp {

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static vk::VertexInputBindingDescription bindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions();
};

}
#endif // VERTEX_H

#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace e172vp {

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;

    static vk::VertexInputBindingDescription bindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions();
};

}

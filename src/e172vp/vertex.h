#ifndef VERTEX_H
#define VERTEX_H

#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

namespace e172vp {

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;

    static std::vector<Vertex> fromGlm(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec2> &texCoords = std::vector<glm::vec2>(), glm::vec3 color = glm::vec3(1.));
    static vk::VertexInputBindingDescription bindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions();
};

}
#endif // VERTEX_H

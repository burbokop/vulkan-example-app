#include "vertex.h"


std::array<vk::VertexInputAttributeDescription, 2> e172vp::Vertex::attributeDescriptions() {
    std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
    attributeDescriptions[0].offset = offsetof(Vertex, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    return attributeDescriptions;
}

std::vector<e172vp::Vertex> e172vp::Vertex::fromGlm(const std::vector<glm::vec3> &vertices, glm::vec3 color) {
    std::vector<e172vp::Vertex> result(vertices.size());
    for(size_t i = 0; i < vertices.size(); ++i) {
        result[i] = { vertices[i], color };
    }
    return result;
}

vk::VertexInputBindingDescription e172vp::Vertex::bindingDescription() {
    vk::VertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = vk::VertexInputRate::eVertex;

    return bindingDescription;
}

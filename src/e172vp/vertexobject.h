#ifndef VERTEXOBJECT_H
#define VERTEXOBJECT_H

#include "vertex.h"


namespace e172vp {
class GraphicsObject;
class VertexObject {
    GraphicsObject *m_graphicsObject = nullptr;
    glm::mat4 m_model;

    vk::Buffer m_vertexBuffer;
    vk::DeviceMemory m_vertexBufferMemory;
    vk::Buffer m_indexBuffer;
    vk::DeviceMemory m_indexBufferMemory;
    vk::Buffer m_uniformBuffer;
    vk::DeviceMemory m_uniformBufferMemory;

    std::vector<vk::DescriptorSet> m_descriptorSets;
public:
    VertexObject(const e172vp::GraphicsObject *graphicsObject, const vk::DescriptorSetLayout &descriptorSetLayout, const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices);
    ~VertexObject();
    glm::mat4 model() const;
    void setModel(const glm::mat4 &model);
    GraphicsObject *graphicsObject() const;
    std::vector<vk::DescriptorSet> descriptorSets() const;
};
}
#endif // VERTEXOBJECT_H

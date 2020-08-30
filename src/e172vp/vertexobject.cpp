#include "vertexobject.h"

#include "tools/buffer.h"
#include "graphicsobject.h"


glm::mat4 e172vp::VertexObject::model() const {
    return m_model;
}

void e172vp::VertexObject::setModel(const glm::mat4 &model) {
    m_model = model;
}

std::vector<vk::DescriptorSet> e172vp::VertexObject::descriptorSets() const {
    return m_descriptorSets;
}

e172vp::VertexObject::VertexObject(const e172vp::GraphicsObject *graphicsObject, const vk::DescriptorSetLayout &descriptorSetLayout, const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices) {
    Buffer::createVertexBuffer(graphicsObject, vertices, &m_vertexBuffer, &m_vertexBufferMemory);
    Buffer::createIndexBuffer(graphicsObject, indices, &m_indexBuffer, &m_indexBufferMemory);
    Buffer::createUniformBuffer<glm::mat4>(graphicsObject, &m_uniformBuffer, &m_uniformBufferMemory);
    Buffer::createUniformDescriptorSets<glm::mat4>(graphicsObject->logicalDevice(), graphicsObject->descriptorPool(), { m_uniformBuffer }, descriptorSetLayout, &m_descriptorSets);
    m_graphicsObject = const_cast<GraphicsObject*>(graphicsObject);
}

e172vp::GraphicsObject *e172vp::VertexObject::graphicsObject() const {
    return m_graphicsObject;
}

e172vp::VertexObject::~VertexObject() {
    m_graphicsObject->logicalDevice().destroyBuffer(m_vertexBuffer);
    m_graphicsObject->logicalDevice().freeMemory(m_vertexBufferMemory);
    m_graphicsObject->logicalDevice().destroyBuffer(m_indexBuffer);
    m_graphicsObject->logicalDevice().freeMemory(m_indexBufferMemory);
    m_graphicsObject->logicalDevice().destroyBuffer(m_uniformBuffer);
    m_graphicsObject->logicalDevice().freeMemory(m_uniformBufferMemory);
    m_graphicsObject->logicalDevice().freeDescriptorSets(m_graphicsObject->descriptorPool(), m_descriptorSets);
}

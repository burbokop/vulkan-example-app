#include "vertexobject.h"

#include "geometry/Mesh.h"
#include "graphicsobject.h"
#include "tools/buffer.h"

std::vector<vk::DescriptorSet> e172vp::VertexObject::descriptorSets() const {
    return m_descriptorSets;
}

e172vp::VertexObject::VertexObject(const e172vp::GraphicsObject* graphicsObject,
    size_t imageCount,
    const DescriptorSetLayout* descriptorSetLayout,
    const DescriptorSetLayout* samplerDescriptorSetLayout,
    const Mesh& mesh,
    const vk::ImageView& imageView,
    Shared<Pipeline> pipeline)
    : m_pipeline(std::move(pipeline))
{
    m_graphicsObject = const_cast<GraphicsObject*>(graphicsObject);
    Buffer::createVertexBuffer(graphicsObject, mesh.vertices(), &m_vertexBuffer, &m_vertexBufferMemory);
    Buffer::createIndexBuffer(graphicsObject, mesh.indices(), &m_indexBuffer, &m_indexBufferMemory);
    Buffer::createUniformBuffers<UniformBufferObject>(graphicsObject, imageCount, &m_uniformBuffers, &m_uniformBufferMemories);
    Buffer::createUniformDescriptorSets<UniformBufferObject>(graphicsObject->logicalDevice(), graphicsObject->descriptorPool(), m_uniformBuffers, descriptorSetLayout, &m_descriptorSets);
    Buffer::createSamplerDescriptorSets(graphicsObject->logicalDevice(),
                                        graphicsObject->descriptorPool(),
                                        imageView,
                                        graphicsObject->sampler(),
                                        imageCount,
                                        samplerDescriptorSetLayout,
                                        &m_textureDescriptorSets);
    m_indexCount = mesh.indices().size();
}

e172vp::GraphicsObject *e172vp::VertexObject::graphicsObject() const {
    return m_graphicsObject;
}

vk::Buffer e172vp::VertexObject::vertexBuffer() const {
    return m_vertexBuffer;
}

vk::Buffer e172vp::VertexObject::indexBuffer() const {
    return m_indexBuffer;
}

uint32_t e172vp::VertexObject::indexCount() const {
    return m_indexCount;
}

void e172vp::VertexObject::updateUbo(int imageIndex) {

    UniformBufferObject __ubo;
    __ubo.model = m_translation * m_rotation * m_scale;

    void* data;
    vkMapMemory(m_graphicsObject->logicalDevice(), m_uniformBufferMemories[imageIndex], 0, sizeof(UniformBufferObject), 0, &data);
    assert(data);
    memcpy(data, &__ubo, sizeof(UniformBufferObject));
    vkUnmapMemory(m_graphicsObject->logicalDevice(), m_uniformBufferMemories[imageIndex]);
}

glm::mat4 e172vp::VertexObject::rotation() const
{
    return m_rotation;
}

void e172vp::VertexObject::setRotation(const glm::mat4 &rotation)
{
    m_rotation = rotation;
}

glm::mat4 e172vp::VertexObject::translation() const
{
    return m_translation;
}

void e172vp::VertexObject::setTranslation(const glm::mat4 &translation)
{
    m_translation = translation;
}

glm::mat4 e172vp::VertexObject::scale() const
{
    return m_scale;
}

void e172vp::VertexObject::setScale(const glm::mat4 &scale)
{
    m_scale = scale;
}

std::vector<vk::DescriptorSet> e172vp::VertexObject::textureDescriptorSets() const
{
    return m_textureDescriptorSets;
}

e172vp::VertexObject::~VertexObject() {
    m_graphicsObject->logicalDevice().destroyBuffer(m_vertexBuffer);
    m_graphicsObject->logicalDevice().freeMemory(m_vertexBufferMemory);
    m_graphicsObject->logicalDevice().destroyBuffer(m_indexBuffer);
    m_graphicsObject->logicalDevice().freeMemory(m_indexBufferMemory);
    for(size_t i = 0; i < m_uniformBuffers.size(); ++i) {
        m_graphicsObject->logicalDevice().destroyBuffer(m_uniformBuffers[i]);
        m_graphicsObject->logicalDevice().freeMemory(m_uniformBufferMemories[i]);
    }
    m_graphicsObject->logicalDevice().freeDescriptorSets(m_graphicsObject->descriptorPool(), m_descriptorSets);
}

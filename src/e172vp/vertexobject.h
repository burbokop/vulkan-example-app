#ifndef VERTEXOBJECT_H
#define VERTEXOBJECT_H

#include "vertex.h"
#include "descriptorsetlayout.h"

namespace e172vp {
class GraphicsObject;
class Renderer;
class VertexObject {
    friend Renderer;
    GraphicsObject *m_graphicsObject = nullptr;

    static inline const glm::mat4 sm = {
               { 1, 0, 0, 0 },
               { 0, 1, 0, 0 },
               { 0, 0, 1, 0 },
               { 0, 0, 0, 1 }
           };

    glm::mat4 m_rotation = sm;
    glm::mat4 m_translation = sm;
    glm::mat4 m_scale = sm;

    struct ubo {
        glm::mat4 model;
    };


    vk::Buffer m_vertexBuffer;
    vk::DeviceMemory m_vertexBufferMemory;
    vk::Buffer m_indexBuffer;
    vk::DeviceMemory m_indexBufferMemory;
    std::vector<vk::Buffer> m_uniformBuffers;
    std::vector<vk::DeviceMemory> m_uniformBufferMemories;

    std::vector<vk::DescriptorSet> m_descriptorSets;
    uint32_t m_indexCount;
    ~VertexObject();
    VertexObject(const e172vp::GraphicsObject *graphicsObject, size_t imageCount, const e172vp::DescriptorSetLayout *descriptorSetLayout, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
public:
    GraphicsObject *graphicsObject() const;
    std::vector<vk::DescriptorSet> descriptorSets() const;
    vk::Buffer vertexBuffer() const;
    vk::Buffer indexBuffer() const;
    uint32_t indexCount() const;

    void updateUbo(int imageIndex);
    glm::mat4 rotation() const;
    void setRotation(const glm::mat4 &rotation);
    glm::mat4 translation() const;
    void setTranslation(const glm::mat4 &translation);
    glm::mat4 scale() const;
    void setScale(const glm::mat4 &scale);
};
}
#endif // VERTEXOBJECT_H

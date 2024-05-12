#pragma once

#include "descriptorsetlayout.h"
#include "utils/NoNull.h"
#include <glm/glm.hpp>
#include <memory>

namespace e172vp {

class GraphicsObject;
class Renderer;
class Pipeline;
class Mesh;

class VertexObject {
    friend Renderer;

    static constexpr glm::mat4 sm = {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    };

    ~VertexObject();
    VertexObject(
        const e172vp::GraphicsObject* graphicsObject,
        size_t imageCount,
        const DescriptorSetLayout* descriptorSetLayout,
        const DescriptorSetLayout* samplerDescriptorSetLayout,
        const Mesh& mesh,
        const vk::ImageView& imageView,
        Shared<Pipeline> pipeline);

private:
    struct UniformBufferObject {
        glm::mat4 model;
    };

public:
    GraphicsObject *graphicsObject() const;
    std::vector<vk::DescriptorSet> descriptorSets() const;
    vk::Buffer vertexBuffer() const;
    vk::Buffer indexBuffer() const;
    uint32_t indexCount() const;
    const auto& pipeline() const { return m_pipeline; }

    void updateUbo(int imageIndex);
    glm::mat4 rotation() const;
    void setRotation(const glm::mat4 &rotation);
    glm::mat4 translation() const;
    void setTranslation(const glm::mat4 &translation);
    glm::mat4 scale() const;
    void setScale(const glm::mat4 &scale);
    std::vector<vk::DescriptorSet> textureDescriptorSets() const;

private:
    glm::mat4 m_rotation = sm;
    glm::mat4 m_translation = sm;
    glm::mat4 m_scale = sm;

    GraphicsObject* m_graphicsObject = nullptr;

    vk::Buffer m_vertexBuffer;
    vk::DeviceMemory m_vertexBufferMemory;
    vk::Buffer m_indexBuffer;
    vk::DeviceMemory m_indexBufferMemory;
    std::vector<vk::Buffer> m_uniformBuffers;
    std::vector<vk::DeviceMemory> m_uniformBufferMemories;

    std::vector<vk::DescriptorSet> m_descriptorSets;
    std::vector<vk::DescriptorSet> m_textureDescriptorSets;
    std::uint32_t m_indexCount;
    Shared<Pipeline> m_pipeline;
};

}

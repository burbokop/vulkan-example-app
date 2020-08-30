#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan.hpp>
#include "../vertex.h"


namespace e172vp {

class GraphicsObject;
class Buffer {
    static vk::Device logicalDevice(const GraphicsObject *graphicsObject);
    static vk::PhysicalDevice physicalDevice(const GraphicsObject *graphicsObject);
public:
    static uint32_t findMemoryType(const vk::PhysicalDevice &physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    static void createAbstractBuffer(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer *buffer, vk::DeviceMemory *bufferMemory);
    static void copyBuffer(const vk::Device &logicalDevice, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const vk::Buffer &srcBuffer, const vk::Buffer &dstBuffer, const vk::DeviceSize &size);
    static void createVertexBuffer(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const std::vector<Vertex> &vertices, vk::Buffer *vertexBuffer, vk::DeviceMemory *vertexBufferMemory);
    static void createIndexBuffer(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const std::vector<uint16_t> &indices, vk::Buffer *indexBuffer, vk::DeviceMemory *indexBufferMemory);

    static void createAbstractBuffer(const GraphicsObject *graphicsObject, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer *buffer, vk::DeviceMemory *bufferMemory);
    static void copyBuffer(const GraphicsObject *graphicsObject, const vk::Buffer &srcBuffer, const vk::Buffer &dstBuffer, const vk::DeviceSize &size);
    static void createVertexBuffer(const GraphicsObject *graphicsObject, const std::vector<Vertex> &vertices, vk::Buffer *vertexBuffer, vk::DeviceMemory *vertexBufferMemory);
    static void createIndexBuffer(const GraphicsObject *graphicsObject, const std::vector<uint16_t> &indices, vk::Buffer *indexBuffer, vk::DeviceMemory *indexBufferMemory);

    template<typename T>
    static void createUniformBuffer(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, vk::Buffer *uniformBuffers, vk::DeviceMemory *uniformBuffersMemory) {
        createAbstractBuffer(logicalDevice, physicalDevice, sizeof(T), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffers, uniformBuffersMemory);
    }

    template<typename T>
    static void createUniformBuffer(const GraphicsObject *graphicsObject, vk::Buffer *uniformBuffers, vk::DeviceMemory *uniformBuffersMemory) {
        createAbstractBuffer(logicalDevice(graphicsObject), physicalDevice(graphicsObject), sizeof(T), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffers, uniformBuffersMemory);
    }

    static void createUniformDescriptorSets(const vk::Device &logicalDevice, const vk::DescriptorPool &descriptorPool, size_t structSize, const std::vector<vk::Buffer> &uniformBuffers, const vk::DescriptorSetLayout &descriptorSetLayout, std::vector<vk::DescriptorSet> *descriptorSets);

    template<typename T>
    static void createUniformDescriptorSets(const vk::Device &logicalDevice, const vk::DescriptorPool &descriptorPool, const std::vector<vk::Buffer> &uniformBuffers, const vk::DescriptorSetLayout &descriptorSetLayout, std::vector<vk::DescriptorSet> *descriptorSets) {
        createUniformDescriptorSets(logicalDevice, descriptorPool, sizeof (T), uniformBuffers, descriptorSetLayout, descriptorSets);
    }
};

}
#endif // BUFFER_H

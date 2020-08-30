#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan.hpp>

namespace e172vp {

struct Buffer {
    static uint32_t findMemoryType(const vk::PhysicalDevice &physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    static void createBuffer(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory);
    static void copyBuffer(const vk::Device &logicalDevice, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const vk::Buffer &srcBuffer, const vk::Buffer &dstBuffer, const vk::DeviceSize &size);
};

}
#endif // BUFFER_H

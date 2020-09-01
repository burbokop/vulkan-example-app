#include "buffer.h"

#include "../graphicsobject.h"

vk::Device e172vp::Buffer::logicalDevice(const e172vp::GraphicsObject *graphicsObject) {
    return graphicsObject->logicalDevice();
}

vk::PhysicalDevice e172vp::Buffer::physicalDevice(const e172vp::GraphicsObject *graphicsObject) {
    return graphicsObject->physicalDevice();
}

uint32_t e172vp::Buffer::findMemoryType(const vk::PhysicalDevice &physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    const auto memProperties = physicalDevice.getMemoryProperties();
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void e172vp::Buffer::createAbstractBuffer(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer *buffer, vk::DeviceMemory *bufferMemory) {
    vk::BufferCreateInfo bufferInfo;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    if (logicalDevice.createBuffer(&bufferInfo, nullptr, buffer) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create buffer!");
    }

    vk::MemoryRequirements memRequirements;
    logicalDevice.getBufferMemoryRequirements(*buffer, &memRequirements);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);



    const auto code = logicalDevice.allocateMemory(&allocInfo, nullptr, bufferMemory);
    if (code != vk::Result::eSuccess) {
        throw std::runtime_error("e172vp::Buffer::createAbstractBuffer: failed to allocate buffer memory: " + vk::to_string(code));
    }

    vkBindBufferMemory(logicalDevice, *buffer, *bufferMemory, 0);
}

void e172vp::Buffer::copyBuffer(const vk::Device &logicalDevice, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const vk::Buffer &srcBuffer, const vk::Buffer &dstBuffer, const vk::DeviceSize &size) {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    vk::CommandBuffer commandBuffer;
    logicalDevice.allocateCommandBuffers(&allocInfo, &commandBuffer);

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(&beginInfo);

    vk::BufferCopy copyRegion{};
    copyRegion.size = size;
    commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

    commandBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    graphicsQueue.submit(1, &submitInfo, vk::Fence());
    graphicsQueue.waitIdle();

    logicalDevice.freeCommandBuffers(commandPool, 1, &commandBuffer);
}

void e172vp::Buffer::createVertexBuffer(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const std::vector<Vertex> &vertices, vk::Buffer *vertexBuffer, vk::DeviceMemory *vertexBufferMemory) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    Buffer::createAbstractBuffer(logicalDevice, physicalDevice, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    Buffer::createAbstractBuffer(logicalDevice, physicalDevice, bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);
    Buffer::copyBuffer(logicalDevice, commandPool, graphicsQueue, stagingBuffer, *vertexBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void e172vp::Buffer::createIndexBuffer(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const std::vector<uint32_t> &indices, vk::Buffer *indexBuffer, vk::DeviceMemory *indexBufferMemory) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    Buffer::createAbstractBuffer(logicalDevice, physicalDevice, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    Buffer::createAbstractBuffer(logicalDevice, physicalDevice, bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);
    Buffer::copyBuffer(logicalDevice, commandPool, graphicsQueue, stagingBuffer, *indexBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void e172vp::Buffer::createAbstractBuffer(const e172vp::GraphicsObject *graphicsObject, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer *buffer, vk::DeviceMemory *bufferMemory) {
    createAbstractBuffer(graphicsObject->logicalDevice(), graphicsObject->physicalDevice(), size, usage, properties, buffer, bufferMemory);
}

void e172vp::Buffer::copyBuffer(const e172vp::GraphicsObject *graphicsObject, const vk::Buffer &srcBuffer, const vk::Buffer &dstBuffer, const vk::DeviceSize &size) {
    copyBuffer(graphicsObject->logicalDevice(), graphicsObject->commandPool(), graphicsObject->graphicsQueue(), srcBuffer, dstBuffer, size);
}

void e172vp::Buffer::createVertexBuffer(const e172vp::GraphicsObject *graphicsObject, const std::vector<e172vp::Vertex> &vertices, vk::Buffer *vertexBuffer, vk::DeviceMemory *vertexBufferMemory) {
    createVertexBuffer(graphicsObject->logicalDevice(), graphicsObject->physicalDevice(), graphicsObject->commandPool(), graphicsObject->graphicsQueue(), vertices, vertexBuffer, vertexBufferMemory);
}

void e172vp::Buffer::createIndexBuffer(const e172vp::GraphicsObject *graphicsObject, const std::vector<uint32_t> &indices, vk::Buffer *indexBuffer, vk::DeviceMemory *indexBufferMemory) {
    createIndexBuffer(graphicsObject->logicalDevice(), graphicsObject->physicalDevice(), graphicsObject->commandPool(), graphicsObject->graphicsQueue(), indices, indexBuffer, indexBufferMemory);
}


void e172vp::Buffer::createUniformDescriptorSets(const vk::Device &logicalDevice, const vk::DescriptorPool &descriptorPool, size_t structSize, const std::vector<vk::Buffer> &uniformBuffers, const e172vp::DescriptorSetLayout *descriptorSetLayout, std::vector<vk::DescriptorSet> *descriptorSets) {
    std::vector<vk::DescriptorSetLayout> layouts(uniformBuffers.size(), descriptorSetLayout->descriptorSetLayoutHandle());
    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(uniformBuffers.size());
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets->resize(uniformBuffers.size());

    const auto code = logicalDevice.allocateDescriptorSets(&allocInfo, descriptorSets->data());
    if (code != vk::Result::eSuccess) {
        throw std::runtime_error("failed to allocate descriptor sets: " + vk::to_string(code));
    }

    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        vk::DescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = structSize;

        vk::WriteDescriptorSet descriptorWrite;
        descriptorWrite.dstSet = descriptorSets->at(i);
        descriptorWrite.dstBinding = descriptorSetLayout->binding();
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = descriptorType;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        logicalDevice.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
    }
}


#include "commandpool.h"



vk::CommandPool e172vp::CommandPool::commandPullHandle() const {
    return m_commandPoolHandle;
}

bool e172vp::CommandPool::isValid() const {
    return m_isValid;
}

e172vp::CommandPool::CommandPool(const vk::Device &logicalDevice, const Hardware::QueueFamilies &queueFamilies, size_t buffersCount) {
    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.queueFamilyIndex = queueFamilies.graphicsFamily();

    auto code = logicalDevice.createCommandPool(&poolInfo, nullptr, &m_commandPoolHandle);
    if (code != vk::Result::eSuccess) {
        m_errors.push_back("[error] Failed to create command pool: " + vk::to_string(code));
        return;
    }

    m_commandBuffers.resize(buffersCount);
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = m_commandPoolHandle;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = (uint32_t) m_commandBuffers.size();

    code = logicalDevice.allocateCommandBuffers(&allocInfo, m_commandBuffers.data());
    if (code != vk::Result::eSuccess) {
        m_errors.push_back("[error] Failed to allocate command buffers: " + vk::to_string(code));
        return;
    }

    m_isValid = true;
}

std::vector<std::string> e172vp::CommandPool::pullErrors() {
    const auto r = m_errors;
    m_errors.clear();
    return r;
}

vk::CommandBuffer e172vp::CommandPool::commandBuffer(size_t index) const {
    if(index < m_commandBuffers.size())
        return m_commandBuffers[index];
    return vk::CommandBuffer();
}

size_t e172vp::CommandPool::commandBufferCount() const {
    return m_commandBuffers.size();
}

std::vector<vk::CommandBuffer> e172vp::CommandPool::commandBufferVector() const {
    return m_commandBuffers;
}

e172vp::CommandPool::operator vk::CommandPool() const {
    return m_commandPoolHandle;
}

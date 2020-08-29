#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H

#include <vulkan/vulkan.hpp>
#include "tools/hardware.h"

namespace e172vp {

class CommandPool {
    vk::CommandPool m_commandPoolHandle;
    std::vector<vk::CommandBuffer> m_commandBuffers;

    std::vector<std::string> m_errors;
    bool m_isValid = false;
public:
    CommandPool() {}
    CommandPool(const vk::Device &logicalDevice, const Hardware::QueueFamilies &queueFamilies, size_t buffersCount);

    vk::CommandPool commandPullHandle() const;
    operator vk::CommandPool() const;
    std::vector<std::string> pullErrors();

    vk::CommandBuffer commandBuffer(size_t index) const;
    size_t commandBufferCount() const;
    std::vector<vk::CommandBuffer> commandBufferVector() const;
    bool isValid() const;
};

}
#endif // COMMANDPOOL_H

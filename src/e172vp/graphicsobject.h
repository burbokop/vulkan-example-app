#ifndef GRAPHICSINSTANCE_H
#define GRAPHICSINSTANCE_H

#include "graphicsobjectcreateinfo.h"
#include "tools/hardware.h"

#include <vulkan/vulkan.hpp>
#include <queue>
#include <map>
#include "swapchain.h"
#include "renderpass.h"
#include "commandpool.h"

namespace e172vp {

class GraphicsObject {
    VkDebugReportCallbackEXT m_debugReportCallbackObject;

    vk::Instance m_vulkanInstance;
    vk::SurfaceKHR m_surface;
    vk::PhysicalDevice m_physicalDevice;
    vk::Device m_logicalDevice;

    vk::Queue m_graphicsQueue;
    vk::Queue m_presentQueue;

    e172vp::SwapChain m_swapChain;
    e172vp::RenderPass m_renderPass;
    e172vp::CommandPool m_commandPool;
    vk::DescriptorPool m_descriptorPool;
    vk::Sampler m_sampler;

    e172vp::Hardware::QueueFamilies m_queueFamilies;
    e172vp::SwapChain::Settings m_swapChainSettings;

    std::vector<std::string> m_enabledValidationLayers;
    bool m_debugEnabled = false;
    bool m_isValid = false;
    std::vector<std::string> m_errors;


public:
    void createTextureSampler(const vk::Device &logicalDevice, vk::Sampler *sampler);

    static void createDescriptorPool(const vk::Device &logicalDevice, size_t size, vk::DescriptorPool *uniformDescriptorPool, std::vector<std::string> *m_errors);
    GraphicsObject() {}
    GraphicsObject(const GraphicsObjectCreateInfo &createInfo);

    vk::Instance vulkanInstance() const;
    vk::PhysicalDevice physicalDevice() const;
    vk::Device logicalDevice() const;
    vk::SurfaceKHR surface() const;
    e172vp::Hardware::QueueFamilies queueFamilies() const;
    e172vp::SwapChain::Settings swapChainSettings() const;
    std::vector<std::string> enabledValidationLayers() const;
    vk::Queue graphicsQueue() const;
    vk::Queue presentQueue() const;
    e172vp::SwapChain swapChain() const;
    e172vp::CommandPool commandPool() const;
    e172vp::RenderPass renderPass() const;

    std::vector<std::string> pullErrors();
    bool isValid() const;
    bool debugEnabled() const;
    vk::DescriptorPool descriptorPool() const;
    vk::Sampler sampler() const;
};

}
#endif // GRAPHICSINSTANCE_H

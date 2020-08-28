#include "graphicsinstance.h"

#include "tools/hardware.h"
#include "tools/validation.h"
#include "tools/stringvector.h"
#include "tools/extensiontools.h"
#include "tools/vulkaninstancefactory.h"
#include "tools/logicdevicefactory.h"

#include <set>
#include <iostream>

vk::Instance e172vp::GraphicsInstance::vulkanInstance() const { return m_vulkanInstance; }
vk::PhysicalDevice e172vp::GraphicsInstance::physicalDevice() const { return m_physicalDevice; }
vk::Device e172vp::GraphicsInstance::logicalDevice() const { return m_logicalDevice; }
vk::SurfaceKHR e172vp::GraphicsInstance::surface() const { return m_surface; }
bool e172vp::GraphicsInstance::hasErrors() const { return m_errors.size() > 0; }
bool e172vp::GraphicsInstance::isValid() const { return m_isValid; }
e172vp::Hardware::QueueFamilies e172vp::GraphicsInstance::queueFamilies() const { return m_queueFamilies; }
std::vector<std::string> e172vp::GraphicsInstance::enabledValidationLayers() const { return m_enabledValidationLayers; }
vk::Queue e172vp::GraphicsInstance::graphicsQueue() const { return m_graphicsQueue; }
vk::Queue e172vp::GraphicsInstance::presentQueue() const { return m_presentQueue; }
vk::SurfaceFormatKHR e172vp::GraphicsInstance::surfaceFormat() const { return m_surfaceFormat; }
vk::Extent2D e172vp::GraphicsInstance::extent() const { return m_extent; }
vk::SwapchainKHR e172vp::GraphicsInstance::swapChain() const { return m_swapChain; }
vk::CommandPool e172vp::GraphicsInstance::commandPool() const { return m_commandPool; }
vk::RenderPass e172vp::GraphicsInstance::renderPass() const { return m_renderPass; }
std::vector<vk::Image> e172vp::GraphicsInstance::swapChainImages() const { return m_swapChainImages; }
std::vector<vk::ImageView> e172vp::GraphicsInstance::swapChainImageViews() const { return m_swapChainImageViews; }
std::vector<vk::Framebuffer> e172vp::GraphicsInstance::swapChainFramebuffers() const { return m_swapChainFramebuffers; }
std::vector<vk::CommandBuffer> e172vp::GraphicsInstance::commandBuffers() const { return m_commandBuffers; }
bool e172vp::GraphicsInstance::debugEnabled() const { return m_debugEnabled; }


std::string e172vp::GraphicsInstance::nextError() {
    const auto e = m_errors.front();
    m_errors.pop();
    return e;
}

vk::SurfaceFormatKHR e172vp::GraphicsInstance::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

vk::PresentModeKHR e172vp::GraphicsInstance::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D e172vp::GraphicsInstance::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        std::cout << "[warning] extent set manualy\n";
        VkExtent2D actualExtent = { 1080, 720 };

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

bool e172vp::GraphicsInstance::createSwapChain(const vk::PhysicalDevice &physicalDevice, const vk::Device &logicalDevice, const vk::SurfaceKHR &surface, const e172vp::Hardware::QueueFamilies &queueFamilies, vk::SurfaceFormatKHR *surfaceFormat, vk::Extent2D *extent, vk::SwapchainKHR *swapChain, std::queue<std::string> *error_queue) {
    const auto swapChainSupport = Hardware::querySwapChainSupport(physicalDevice, surface);

    *surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    *extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = 3;
    if (imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo { };
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat->format;
    createInfo.imageColorSpace = surfaceFormat->colorSpace;
    createInfo.imageExtent = *extent;
    createInfo.imageArrayLayers = 1;
    createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
    createInfo.setPNext(nullptr);

    uint32_t queueFamilyIndices[] = { queueFamilies.graphicsFamily(), queueFamilies.presentFamily()};

    if (queueFamilies.graphicsFamily() != queueFamilies.presentFamily()) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = {}; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = true;


    const auto code = logicalDevice.createSwapchainKHR(&createInfo, nullptr, swapChain);
    if (code != vk::Result::eSuccess) {
        error_queue->push("[error] Failed to create swap chain: " + vk::to_string(code));
        return false;
    }
    return true;
}

bool e172vp::GraphicsInstance::createImageViewes(const vk::Device &logicDevice, const std::vector<vk::Image> &swapChainImages, const vk::Format &swapChainImageFormat, std::vector<vk::ImageView> *swapChainImageViews, std::queue<std::string> *error_queue) {
    swapChainImageViews->resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        vk::ImageViewCreateInfo createInfo{};
        createInfo.image = swapChainImages[i];
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format = swapChainImageFormat;
        createInfo.components.r = vk::ComponentSwizzle::eIdentity;
        createInfo.components.g = vk::ComponentSwizzle::eIdentity;
        createInfo.components.b = vk::ComponentSwizzle::eIdentity;
        createInfo.components.a = vk::ComponentSwizzle::eIdentity;
        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        const auto code = logicDevice.createImageView(&createInfo, nullptr, &swapChainImageViews->operator[](i));
        if (code != vk::Result::eSuccess) {
            error_queue->push("[error] Failed to create image views: " + vk::to_string(code));
            return false;
        }
    }
    return true;
}



bool e172vp::GraphicsInstance::createRenderPass(const vk::Device &logicDevice, const vk::Format &swapChainImageFormat, vk::RenderPass *renderPass, std::queue<std::string> *error_queue) {
    vk::AttachmentDescription colorAttachment;
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    const auto code = logicDevice.createRenderPass(&renderPassInfo, nullptr, renderPass);
    if (code != vk::Result::eSuccess) {
        error_queue->push("[error] Failed to create render pass: " + vk::to_string(code));
        return false;
    }
    return true;
}

bool e172vp::GraphicsInstance::createFrameBuffers(const vk::Device &logicDevice, const vk::Extent2D &extent, const vk::RenderPass &renderPass, const std::vector<vk::ImageView> &swapChainImageViews, std::vector<vk::Framebuffer> *swapChainFramebuffers, std::queue<std::string> *error_queue) {
    swapChainFramebuffers->resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        vk::ImageView attachments[] = {
            swapChainImageViews[i]
        };

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        const auto code = logicDevice.createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers->operator[](i));
        if (code != vk::Result::eSuccess) {
            error_queue->push("[error] Failed to create framebuffer: " + vk::to_string(code));
            return false;
        }
    }
    return true;
}

bool e172vp::GraphicsInstance::createCommandPool(const vk::Device &logicDevice, const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface, vk::CommandPool *commandPool, std::queue<std::string> *error_queue) {
    const auto queueFamilies = Hardware::queryQueueFamilies(physicalDevice, surface);

    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.queueFamilyIndex = queueFamilies.graphicsFamily();

    auto code = logicDevice.createCommandPool(&poolInfo, nullptr, commandPool);
    if (code != vk::Result::eSuccess) {
        error_queue->push("[error] Failed to create command pool: " + vk::to_string(code));
        return false;
    }
    return true;
}

bool e172vp::GraphicsInstance::createCommandBuffers(const vk::Device &logicDevice, uint32_t count, const vk::CommandPool &commandPool, std::vector<vk::CommandBuffer> *commandBuffers, std::queue<std::string> *error_queue) {
    commandBuffers->resize(count);

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers->size();

    const auto code = logicDevice.allocateCommandBuffers(&allocInfo, commandBuffers->data());
    if (code != vk::Result::eSuccess) {
        error_queue->push("[error] Failed to allocate command buffers: " + vk::to_string(code));
        return false;
    }
    return  true;
}


e172vp::GraphicsInstance::GraphicsInstance(const GraphicsInstanceCreateInfo &createInfo) {    
    m_debugEnabled = createInfo.debugEnabled();
    e172vp::VulkanInstanceFactory vulkanInstanceFactory("test", VK_MAKE_VERSION(1, 0, 0));
    vulkanInstanceFactory.setRequiredExtensions(createInfo.requiredExtensions());
    vulkanInstanceFactory.setDebugEnabled(m_debugEnabled);
    m_vulkanInstance = vulkanInstanceFactory.create();
    if(!m_vulkanInstance) {
        m_errors.push("[error] Instance not created.");
    }

    if(createInfo.surfaceCreator()) {
        createInfo.surfaceCreator()(m_vulkanInstance, &m_surface);
        if(!m_surface) {
            m_errors.push("[error] Surface not created.");
            return;
        }
    } else {
        m_errors.push("[error] Surface creator not specified.");
        return;
    }

    m_physicalDevice = Hardware::findSuitablePhysicalDevice(m_vulkanInstance, m_surface, createInfo.requiredDeviceExtensions());
    if(!m_physicalDevice) {
        m_errors.push("[error] Suitable device not found.");
        return;
    }
    m_queueFamilies = e172vp::Hardware::queryQueueFamilies(m_physicalDevice, m_surface);

    e172vp::LogicDeviceFactory logicDeviceFactory;
    logicDeviceFactory.setQueueFamilies(m_queueFamilies);
    logicDeviceFactory.setValidationLayersEnabled(m_debugEnabled);
    logicDeviceFactory.setRequiredDeviceExtensions(createInfo.requiredDeviceExtensions());
    m_logicalDevice = logicDeviceFactory.create(m_physicalDevice);
    if(!m_logicalDevice) {
        m_errors.push("[error] Logic device not found.");
        return;
    }

    m_enabledValidationLayers = logicDeviceFactory.enabledValidationLayers();

    m_logicalDevice.getQueue(m_queueFamilies.graphicsFamily(), 0, &m_graphicsQueue);
    m_logicalDevice.getQueue(m_queueFamilies.presentFamily(), 0, &m_presentQueue);

    if(!createSwapChain(m_physicalDevice, m_logicalDevice, m_surface, m_queueFamilies, &m_surfaceFormat, &m_extent, &m_swapChain, &m_errors))
        return;
    m_swapChainImages = m_logicalDevice.getSwapchainImagesKHR(m_swapChain);
    if(!createImageViewes(m_logicalDevice, m_swapChainImages, m_surfaceFormat.format, &m_swapChainImageViews, &m_errors))
        return;
    if(!createRenderPass(m_logicalDevice, m_surfaceFormat.format, &m_renderPass, &m_errors))
        return;
    if(!createFrameBuffers(m_logicalDevice, m_extent, m_renderPass, m_swapChainImageViews, &m_swapChainFramebuffers, &m_errors))
        return;
    if(!createCommandPool(m_logicalDevice, m_physicalDevice, m_surface, &m_commandPool, &m_errors))
        return;
    if(!createCommandBuffers(m_logicalDevice, m_swapChainImages.size(), m_commandPool, &m_commandBuffers, &m_errors))
        return;


    m_isValid = true;
}


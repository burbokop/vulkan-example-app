#include "graphicsinstance.h"
#include "hardware.h"

#include <set>
#include "extensiontools.h"
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


std::string e172vp::GraphicsInstance::nextError() {
    const auto e = m_errors.front();
    m_errors.pop();
    return e;
}


vk::CommandPool e172vp::GraphicsInstance::commandPool() const {
    return m_commandPool;
}

vk::RenderPass e172vp::GraphicsInstance::renderPass() const {
    return m_renderPass;
}

std::vector<vk::Image> e172vp::GraphicsInstance::swapChainImages() const {
    return m_swapChainImages;
}

std::vector<vk::ImageView> e172vp::GraphicsInstance::swapChainImageViews() const {
    return m_swapChainImageViews;
}

std::vector<vk::Framebuffer> e172vp::GraphicsInstance::swapChainFramebuffers() const { return m_swapChainFramebuffers; }
std::vector<vk::CommandBuffer> e172vp::GraphicsInstance::commandBuffers() const { return m_commandBuffers; }

void e172vp::GraphicsInstance::initDebug(vk::Instance instance, VkDebugReportCallbackEXT *c, std::queue<std::string> *error_queue) {
    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    createInfo.pfnCallback = [](auto, auto, auto, auto, auto, auto, const char* msg, auto) -> VkBool32 {
        std::cerr << "VALIDATION LAYER MESSAGE: " << msg;
        return VK_FALSE;
    };

    PFN_vkCreateDebugReportCallbackEXT e =
            (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
                instance,
                "vkCreateDebugReportCallbackEXT"
                );

    if(e(instance, &createInfo, nullptr, c) != VK_SUCCESS) {
        error_queue->push("[warning] Debug report callback setup failed.");
    }
}

e172vp::GraphicsInstance::LogicDeviceCreationResult e172vp::GraphicsInstance::createLogicalDevice(
        const vk::PhysicalDevice &physicalDevice,
        const vk::SurfaceKHR &surface,
        const std::vector<std::string> &requiredDeviceExtensions,
        bool validationLayersEnabled,
        std::queue<std::string> *error_queue
        ) {
    LogicDeviceCreationResult result;
    result.queueFamilies = Hardware::queryQueueFamilies(physicalDevice, surface);

    if(!result.queueFamilies.isValid()) {
        error_queue->push("[error] Missing graphics or presentation family on any GPU.");
        return result;
    }



    const float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.setQueueFamilyIndex(result.queueFamilies.graphicsFamily());
    queueCreateInfo.setQueueCount(1);
    queueCreateInfo.setPQueuePriorities(&queuePriority);

    vk::PhysicalDeviceFeatures deviceFeatures;

    vk::DeviceCreateInfo createInfo;
    createInfo.setQueueCreateInfoCount(1);
    createInfo.setPQueueCreateInfos(&queueCreateInfo);
    createInfo.setPEnabledFeatures(&deviceFeatures);
    createInfo.setPEnabledExtensionNames(extensionsToVkArray(requiredDeviceExtensions));

    if(validationLayersEnabled) {
        result.enabledValidationLayers = presentValidationLayers();

        std::vector<const char*> vl_buffer(result.enabledValidationLayers.size());
        for(size_t i = 0; i < result.enabledValidationLayers.size(); ++i) {
            vl_buffer[i] = result.enabledValidationLayers[i].c_str();
        }

        createInfo.enabledLayerCount = vl_buffer.size();
        createInfo.ppEnabledLayerNames = vl_buffer.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }



    const auto code = physicalDevice.createDevice(&createInfo, nullptr, &result.logicalDevice);
    if (code != vk::Result::eSuccess) {
        error_queue->push("[error] Failed to create logical device: " + vk::to_string(code));
        return result;
    }


    result.is_valid = true;
    return result;
}

VkSurfaceFormatKHR e172vp::GraphicsInstance::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
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

VkExtent2D e172vp::GraphicsInstance::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) {
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
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;


    uint32_t queueFamilyIndices[] = { queueFamilies.graphicsFamily(), queueFamilies.presentFamily()};

    if (queueFamilies.graphicsFamily() != queueFamilies.presentFamily()) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = {}; // Optional
        std::cout << "[warning] graphics and presentation queue families has same indices.\n";
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = true;


    const auto code = logicalDevice.createSwapchainKHR(&createInfo, nullptr, swapChain);
    if (code != vk::Result::eSuccess) {
        error_queue->push("[error] Failed to create swap chain: " + vk::to_string(code));
        return true;
    }
    return false;
}

void e172vp::GraphicsInstance::createImages(vk::Device logicDevice, vk::SwapchainKHR swapChain, std::vector<vk::Image> *swapChainImages) {
    *swapChainImages = logicDevice.getSwapchainImagesKHR(swapChain);
}

bool e172vp::GraphicsInstance::createImageViewes(vk::Device logicDevice, const std::vector<vk::Image> &swapChainImages, vk::Format swapChainImageFormat, std::vector<vk::ImageView> *swapChainImageViews, std::queue<std::string> *error_queue) {
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

void e172vp::GraphicsInstance::createRenderPass(vk::Device logicDevice, vk::Format swapChainImageFormat, vk::RenderPass *renderPass, std::queue<std::string> *error_queue) {
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
    }

}

void e172vp::GraphicsInstance::createFrameBuffers(vk::Device logicDevice, vk::Extent2D extent, vk::RenderPass renderPass, const std::vector<vk::ImageView> &swapChainImageViews, std::vector<vk::Framebuffer> *swapChainFramebuffers, std::queue<std::string> *error_queue) {
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
        }
    }
}

void e172vp::GraphicsInstance::createCommandPool(vk::Device logicDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, vk::CommandPool *commandPool, std::queue<std::string> *error_queue) {
    const auto queueFamilies = Hardware::queryQueueFamilies(physicalDevice, surface);

    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.queueFamilyIndex = queueFamilies.graphicsFamily();

    auto code = logicDevice.createCommandPool(&poolInfo, nullptr, commandPool);
    if (code != vk::Result::eSuccess) {
        error_queue->push("[error] Failed to create command pool: " + vk::to_string(code));
    }
}

void e172vp::GraphicsInstance::createCommandBuffers(vk::Device logicDevice, uint32_t count, vk::CommandPool commandPool, std::vector<vk::CommandBuffer> *commandBuffers, std::queue<std::string> *error_queue) {
    commandBuffers->resize(count);

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers->size();

    const auto code = logicDevice.allocateCommandBuffers(&allocInfo, commandBuffers->data());
    if (code != vk::Result::eSuccess) {
        error_queue->push("[error] Failed to allocate command buffers: " + vk::to_string(code));
    }
}


e172vp::GraphicsInstance::GraphicsInstance(const GraphicsInstanceCreateInfo &createInfo) {
    vk::ApplicationInfo applicationInfo;
    applicationInfo.setPApplicationName(createInfo.applicationName().c_str());
    applicationInfo.setApplicationVersion(createInfo.applicationVersion());
    applicationInfo.setPEngineName("e172-presentation");
    applicationInfo.setEngineVersion(VK_MAKE_VERSION(0, 2, 0));
    applicationInfo.setApiVersion(VK_MAKE_VERSION(1, 0, 2));

    std::vector<std::string> internalRequires = { VK_KHR_SURFACE_EXTENSION_NAME };
    if(createInfo.debugEnabled())
        internalRequires.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    const auto requiredMergedExtensions = mergeExtensions(createInfo.requiredExtensions(), internalRequires);
    const auto missing = missingExtensions(presentExtensions(), requiredMergedExtensions);
    if(missing.size() > 0) {
        m_errors.push("[error] extensions missing: " + extensionsToString(missing) + ". instance is invalid.");
        return;
    }

    vk::InstanceCreateInfo instanceCreateInfo { };
    instanceCreateInfo.setPApplicationInfo(&applicationInfo);
    instanceCreateInfo.setPEnabledExtensionNames(extensionsToVkArray(requiredMergedExtensions));
    instanceCreateInfo.setEnabledLayerCount(0);

    const auto result = vk::createInstance(&instanceCreateInfo, nullptr, &m_vulkanInstance);
    if (result != vk::Result::eSuccess) {
        m_errors.push("[error] failed to create instance: " + vk::to_string(result));
        return;
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

    if(createInfo.debugEnabled())
        initDebug(m_vulkanInstance, &m_debugReportCallbackObject, &m_errors);


    m_physicalDevice = Hardware::findSuitablePhysicalDevice(m_vulkanInstance, m_surface, createInfo.requiredDeviceExtensions());
    if(!m_physicalDevice) {
        m_errors.push("[error] Suitable device not found.");
        return;
    }

    const auto ldcr = createLogicalDevice(m_physicalDevice, m_surface, createInfo.requiredDeviceExtensions(), createInfo.debugEnabled(), &m_errors);
    if(!ldcr.is_valid) {
        m_errors.push("[error] Logical device missing.");
        return;
    }

    m_logicalDevice = ldcr.logicalDevice;
    m_queueFamilies = ldcr.queueFamilies;
    m_enabledValidationLayers = ldcr.enabledValidationLayers;

    m_logicalDevice.getQueue(m_queueFamilies.graphicsFamily(), 0, &m_graphicsQueue);
    m_logicalDevice.getQueue(m_queueFamilies.presentFamily(), 0, &m_presentQueue);

    createSwapChain(m_physicalDevice, m_logicalDevice, m_surface, m_queueFamilies, &m_surfaceFormat, &m_extent, &m_swapChain, &m_errors);
    createImages(m_logicalDevice, m_swapChain, &m_swapChainImages);
    createImageViewes(m_logicalDevice, m_swapChainImages, m_surfaceFormat.format, &m_swapChainImageViews, &m_errors);
    createRenderPass(m_logicalDevice, m_surfaceFormat.format, &m_renderPass, &m_errors);
    createFrameBuffers(m_logicalDevice, m_extent, m_renderPass, m_swapChainImageViews, &m_swapChainFramebuffers, &m_errors);
    createCommandPool(m_logicalDevice, m_physicalDevice, m_surface, &m_commandPool, &m_errors);
    createCommandBuffers(m_logicalDevice, m_swapChainImages.size(), m_commandPool, &m_commandBuffers, &m_errors);


    m_isValid = true;
}

std::vector<std::string> e172vp::GraphicsInstance::presentValidationLayers() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    std::vector<std::string> result(layerCount);
    for(size_t i = 0; i < layerCount; ++i) {
        result[i] = availableLayers[i].layerName;
    }
    return result;
}


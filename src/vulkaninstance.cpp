#include "queuetools.h"
#include "vulkaninstance.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <set>
#include <fstream>
#include <math.h>
#include "e172vp/tools/extensiontools.h"
#include "e172vp/tools/validation.h"
#include "e172vp/tools/stringvector.h"
#include "e172vp/tools/vulkaninstancefactory.h"
#include "e172vp/tools/logicdevicefactory.h"

VulkanInstance::VulkanInstance(GLFWwindow *window) {
    auto glfwExt = glfwExtensions();
    std::cout << "glfw extensions: " << glfwExt << "\n";

    e172vp::VulkanInstanceFactory vulkanInstanceFactory("test", VK_MAKE_VERSION(1, 0, 0));
    vulkanInstanceFactory.setRequiredExtensions(e172vp::StringVector::mergePair(glfwExt, instanceExtensions));
    vulkanInstanceFactory.setDebugEnabled(true);
    vkInstance = vulkanInstanceFactory.create();


    createSurface(window);

    physicalDevice = e172vp::Hardware::findSuitablePhysicalDevice(vkInstance, surface, deviceExtensions);

    if(!physicalDevice)
        throw std::runtime_error("No suitable GPU found.");


    queueFamilies = e172vp::Hardware::queryQueueFamilies(physicalDevice, surface);

    e172vp::LogicDeviceFactory logicDeviceFactory;
    logicDeviceFactory.setQueueFamilies(queueFamilies);
    logicDeviceFactory.setValidationLayersEnabled(true);
    logicDeviceFactory.setRequiredDeviceExtensions(deviceExtensions);
    logicalDevice = logicDeviceFactory.create(physicalDevice);


    logicalDevice.getQueue(queueFamilies.graphicsFamily(), 0, &graphicsQueue);
    logicalDevice.getQueue(queueFamilies.presentFamily(), 0, &presentQueue);


    createSwapChain(physicalDevice, logicalDevice, surface, queueFamilies, &surfaceFormat, &extent, &swapChain, &m_errors);

    //createImages(logicalDevice, swapChain, &swapChainImages);
    swapChainImages = logicalDevice.getSwapchainImagesKHR(swapChain);

    createImageViewes(logicalDevice, swapChainImages, surfaceFormat.format, &swapChainImageViews, &m_errors);
    createRenderPass(logicalDevice, surfaceFormat.format, &renderPass, &m_errors);
    createFrameBuffers(logicalDevice, extent, renderPass, swapChainImageViews, &swapChainFramebuffers, &m_errors);
    createCommandPool(logicalDevice, physicalDevice, surface, &commandPool, &m_errors);
    createGraphicsPipeline(logicalDevice, extent, renderPass, &pipelineLayout, &graphicsPipeline);
    createCommandBuffers(logicalDevice, swapChainImages.size(), commandPool, &commandBuffers, &m_errors);
    createSyncObjects(logicalDevice, &imageAvailableSemaphore, &renderFinishedSemaphore);

    std::cout << "\nswapChainImages: " << swapChainImages.size() << "\n";
    std::cout << "swapChainImageViews: " << swapChainImageViews.size() << "\n";
    std::cout << "swapChainFramebuffers: " << swapChainFramebuffers.size() << "\n";
    std::cout << "commandBuffers: " << commandBuffers.size() << "\n\n";

    proceedCommandBuffers(renderPass, graphicsPipeline, extent, swapChainFramebuffers, commandBuffers);


    elapsedFromStart.reset();
    updateCommandBuffersTimer.setInterval(500);
    updateCommandBuffersTimer.reset();
}

void VulkanInstance::paint() {
    //resetCommandBuffers(m_graphicsInstance.commandBuffers(), m_graphicsInstance.graphicsQueue(), m_graphicsInstance.presentQueue());
    //CommandReciept reciept;
    //reciept.is_valid = true;
    //reciept.x = std::cos(elapsedFromStart.elapsed() * 0.001) / 2 + 0.5;
    //reciept.y = std::sin(elapsedFromStart.elapsed() * 0.001) / 2 + 0.5;
    //proceedCommandBuffers(m_graphicsInstance.renderPass(), graphicsPipeline, m_graphicsInstance.extent(), m_graphicsInstance.swapChainFramebuffers(), m_graphicsInstance.commandBuffers(), reciept);

    uint32_t imageIndex = 0;
    vk::Result returnCode;


    returnCode = vk::Device(logicalDevice).acquireNextImageKHR(vk::SwapchainKHR(swapChain), UINT64_MAX, imageAvailableSemaphore, {}, &imageIndex);
    if(returnCode != vk::Result::eSuccess)
        throw std::runtime_error("acquiring next image failed. code: " + vk::to_string(returnCode));

    vk::Semaphore waitSemaphores[] = { imageAvailableSemaphore };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    vk::SubmitInfo submitInfo{};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    vk::CommandBuffer cb(commandBuffers[imageIndex]);
    submitInfo.setCommandBuffers(cb);
    submitInfo.setSignalSemaphores(renderFinishedSemaphore);
    //submitInfo.signalSemaphoreCount = 1;
    //submitInfo.pSignalSemaphores = signalSemaphores;

    returnCode = vk::Queue(graphicsQueue).submit(1, &submitInfo, {});

    if (returnCode != vk::Result::eSuccess)
        throw std::runtime_error("failed to submit draw command buffer. code: " + vk::to_string(returnCode));

    vk::SwapchainKHR swapChains[] = { swapChain };

    vk::PresentInfoKHR presentInfo{};
    presentInfo.setWaitSemaphores(renderFinishedSemaphore);
    //presentInfo.waitSemaphoreCount = 1;
    //presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    returnCode = vk::Queue(presentQueue).presentKHR(&presentInfo);
    if(returnCode != vk::Result::eSuccess)
        throw std::runtime_error("present failed. code: " + vk::to_string(returnCode));
}


vk::SurfaceFormatKHR VulkanInstance::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

vk::PresentModeKHR VulkanInstance::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;

}

vk::Extent2D VulkanInstance::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) {
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


bool VulkanInstance::createSwapChain(const vk::PhysicalDevice &physicalDevice, const vk::Device &logicalDevice, const vk::SurfaceKHR &surface, const e172vp::Hardware::QueueFamilies &queueFamilies, vk::SurfaceFormatKHR *surfaceFormat, vk::Extent2D *extent, vk::SwapchainKHR *swapChain, std::queue<std::string> *error_queue) {
    const auto swapChainSupport = e172vp::Hardware::querySwapChainSupport(physicalDevice, surface);

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

bool VulkanInstance::createImageViewes(const vk::Device &logicalDevice, const std::vector<vk::Image> &swapChainImages, const vk::Format &swapChainImageFormat, std::vector<vk::ImageView> *swapChainImageViews, std::queue<std::string> *error_queue) {
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

        const auto code = logicalDevice.createImageView(&createInfo, nullptr, &swapChainImageViews->operator[](i));
        if (code != vk::Result::eSuccess) {
            error_queue->push("[error] Failed to create image views: " + vk::to_string(code));
            return false;
        }
    }
    return true;
}

bool VulkanInstance::createRenderPass(const vk::Device &logicDevice, const vk::Format &swapChainImageFormat, vk::RenderPass *renderPass, std::queue<std::string> *error_queue) {
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

bool VulkanInstance::createFrameBuffers(const vk::Device &logicalDevice, const vk::Extent2D &extent, const vk::RenderPass &renderPass, const std::vector<vk::ImageView> &swapChainImageViews, std::vector<vk::Framebuffer> *swapChainFramebuffers, std::queue<std::string> *error_queue) {
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

        const auto code = logicalDevice.createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers->operator[](i));
        if (code != vk::Result::eSuccess) {
            error_queue->push("[error] Failed to create framebuffer: " + vk::to_string(code));
            return false;
        }
    }
    return true;
}

bool VulkanInstance::createCommandPool(const vk::Device &logicDevice, const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface, vk::CommandPool *commandPool, std::queue<std::string> *error_queue) {
    const auto queueFamilies = e172vp::Hardware::queryQueueFamilies(physicalDevice, surface);

    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.queueFamilyIndex = queueFamilies.graphicsFamily();

    auto code = logicDevice.createCommandPool(&poolInfo, nullptr, commandPool);
    if (code != vk::Result::eSuccess) {
        error_queue->push("[error] Failed to create command pool: " + vk::to_string(code));
        return false;
    }
    return true;
}

void VulkanInstance::createSyncObjects(const vk::Device &logicalDevice, vk::Semaphore *imageAvailableSemaphore, vk::Semaphore *renderFinishedSemaphore) {
    vk::SemaphoreCreateInfo semaphoreInfo;
    if (
            logicalDevice.createSemaphore(&semaphoreInfo, nullptr, imageAvailableSemaphore) != vk::Result::eSuccess ||
            logicalDevice.createSemaphore(&semaphoreInfo, nullptr, renderFinishedSemaphore) != vk::Result::eSuccess
            )
        throw std::runtime_error("failed to create synchronization objects for a frame!");
}

void VulkanInstance::proceedCommandBuffers(vk::RenderPass renderPass, vk::Pipeline pipeline, vk::Extent2D extent, std::vector<vk::Framebuffer> swapChainFramebuffers, std::vector<vk::CommandBuffer> commandBuffers) {
    for (size_t i = 0; i < commandBuffers.size(); i++) {
        vk::CommandBufferBeginInfo beginInfo{};
        if (commandBuffers.at(i).begin(&beginInfo) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        vk::ClearValue clearColor;
        vk::Offset2D offset;

        clearColor = vk::ClearColorValue(std::array<float, 4> { 0.5f, 0.5f, 0.0f, 1.0f });
        offset = vk::Offset2D(0, 0);

        vk::RenderPassBeginInfo renderPassInfo {};
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = offset;
        renderPassInfo.renderArea.extent = extent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        commandBuffers.at(i).beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
        commandBuffers.at(i).bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
        commandBuffers.at(i).draw(3, 1, 0, 0);
        commandBuffers.at(i).endRenderPass();
        commandBuffers.at(i).end();
    }
}

void VulkanInstance::resetCommandBuffers(std::vector<VkCommandBuffer> commandBuffers, VkQueue graphicsQueue, VkQueue presentQueue) {
    vkQueueWaitIdle(graphicsQueue);
    vkQueueWaitIdle(presentQueue);

    for(auto b : commandBuffers) {
        vkResetCommandBuffer(b, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }
}

void VulkanInstance::createGraphicsPipeline(const vk::Device &logicDevice, const vk::Extent2D &extent, const vk::RenderPass &renderPass, vk::PipelineLayout *pipelineLayout, vk::Pipeline *graphicsPipline) {
    auto vertShaderCode = readFile("../shaders/vert.spv");
    auto fragShaderCode = readFile("../shaders/frag.spv");

    vk::ShaderModule vertShaderModule = createShaderModule(logicDevice, vertShaderCode);
    vk::ShaderModule fragShaderModule = createShaderModule(logicDevice, fragShaderCode);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo {};
    vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo {};
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    vk::Viewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) extent.width;
    viewport.height = (float) extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vk::Rect2D scissor {};
    scissor.offset = vk::Offset2D(0, 0);
    scissor.extent = extent;

    vk::PipelineViewportStateCreateInfo viewportState {};
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = VK_FALSE;

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask
            = vk::ColorComponentFlagBits::eR
            | vk::ColorComponentFlagBits::eG
            | vk::ColorComponentFlagBits::eB
            | vk::ColorComponentFlagBits::eA;

    colorBlendAttachment.blendEnable = false;

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (logicDevice.createPipelineLayout(&pipelineLayoutInfo, nullptr, pipelineLayout) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = *pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    if (logicDevice.createGraphicsPipelines(vk::PipelineCache(), 1, &pipelineInfo, nullptr, graphicsPipline) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(logicDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(logicDevice, vertShaderModule, nullptr);

}

VkShaderModule VulkanInstance::createShaderModule(VkDevice logicalDevice, const std::vector<char> &code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

bool VulkanInstance::createCommandBuffers(const vk::Device &logicalDevice, uint32_t count, const vk::CommandPool &commandPool, std::vector<vk::CommandBuffer> *commandBuffers, std::queue<std::string> *error_queue) {
    commandBuffers->resize(count);

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers->size();

    const auto code = logicalDevice.allocateCommandBuffers(&allocInfo, commandBuffers->data());
    if (code != vk::Result::eSuccess) {
        error_queue->push("[error] Failed to allocate command buffers: " + vk::to_string(code));
        return false;
    }
    return  true;
}

std::vector<char> VulkanInstance::readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}


void VulkanInstance::createSurface(GLFWwindow *window) {
    VkSurfaceKHR sss;
    if(glfwCreateWindowSurface(vkInstance, window, NULL, &sss) != VK_SUCCESS) {
        throw std::runtime_error("surface creating error");
    }
    surface = sss;
}

std::vector<std::string> VulkanInstance::glfwExtensions() {
    uint32_t extensionCount = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    if(extensions == nullptr)
        throw std::runtime_error("glfwGetRequiredInstanceExtensions result is NULL");

    std::vector<std::string> result(extensionCount);
    for(uint32_t i = 0; i < extensionCount; ++i) {
        result[i] = extensions[i];
    }
    return result;
}


VulkanInstance::~VulkanInstance() {

    vkDestroyInstance(vkInstance, nullptr);
}

std::ostream &operator<<(std::ostream &stream, const std::vector<std::string> &value) {
    size_t i = 0;
    stream << std::string("[");
    for(auto a : value) {
        stream << a << ((i == value.size() - 1) ? std::string() : ", ");
        ++i;
    }
    stream << std::string("]");
    return stream;
}

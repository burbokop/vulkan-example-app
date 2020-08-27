#include "queuetools.h"
#include "vulkaninstance.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <set>
#include <fstream>


VulkanInstance::VulkanInstance(GLFWwindow *window) {
    VkApplicationInfo appInfo { };
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 2);

    auto glfwExt = glfwExtensions();
    std::cout << "glfw extensions: " << glfwExt << "\n";
    createInstance(&vkInstance, &appInfo, mergeExtensions(glfwExt, instanceExtensions));

    initDebug(vkInstance, &VulkanInstance::debugVulkanCallback, &debugReportCallbackEXT);


    std::cout << "avl: " << availableValidationLayers() << "\n\n";

    std::string missing_layer;
    if(!checkValidationLayerSupport(validationLayers, &missing_layer))
        std::cout << "layer missed: " << missing_layer << "\n";


    createSurface(window);

    physicalDevice = findPhysicalDevice(vkInstance, surface, deviceExtensions);
    if(physicalDevice == VK_NULL_HANDLE)
        throw std::runtime_error("No suitable GPU found.");


    const auto queueFamilies = createLogicalDevice(&logicDevice, physicalDevice, surface, deviceExtensions, validationLayers);
    vkGetDeviceQueue(logicDevice, queueFamilies.graphicsFamily(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicDevice, queueFamilies.presentFamily(), 0, &presentQueue);

    createSwapChain(physicalDevice, logicDevice, surface, queueFamilies, &surfaceFormat, &extent, &swapChain);
    createImages(logicDevice, swapChain, &swapChainImages);
    createImageViewes(logicDevice, swapChainImages, surfaceFormat.format, &swapChainImageViews);
    createRenderPass(logicDevice, surfaceFormat.format, &renderPass);
    createFrameBuffers(logicDevice, extent, renderPass, swapChainImageViews, &swapChainFramebuffers);
    createCommandPool(logicDevice, physicalDevice, surface, &commandPool);
    createGraphicsPipeline(logicDevice, extent, renderPass, &pipelineLayout, &graphicsPipeline);
    createCommandBuffers(logicDevice, swapChainImages.size(), commandPool, &commandBuffers);
    createSyncObjects(logicDevice, &imageAvailableSemaphore, &renderFinishedSemaphore);

    std::cout << "\nswapChainImages: " << swapChainImages.size() << "\n";
    std::cout << "swapChainImageViews: " << swapChainImageViews.size() << "\n";
    std::cout << "swapChainFramebuffers: " << swapChainFramebuffers.size() << "\n";
    std::cout << "commandBuffers: " << commandBuffers.size() << "\n\n";


    proceedCommandBuffers(renderPass, graphicsPipeline, extent, swapChainFramebuffers, commandBuffers);


    elapsedFromStart.reset();
    updateCommandBuffersTimer.setInterval(4000);
    updateCommandBuffersTimer.reset();
}



void VulkanInstance::paint() {
    //if(updateCommandBuffersTimer.check()) {
    //    proceedCommandBuffers(renderPass, graphicsPipeline, extent, swapChainFramebuffers, commandBuffers);
    //    std::cout << "UPDATE COMMAND BUFFERS" << "\n";
    //}

    uint32_t imageIndex = 0;
    VkResult returnCode;

    returnCode = vkAcquireNextImageKHR(logicDevice, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    if(returnCode != VK_SUCCESS)
        throw std::runtime_error("acquiring next image failed. code: " + std::to_string(returnCode));

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;


    returnCode = vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    if (returnCode != VK_SUCCESS)
        throw std::runtime_error("failed to submit draw command buffer. code: " + std::to_string(returnCode));




    VkSwapchainKHR swapChains[] = { swapChain };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    returnCode = vkQueuePresentKHR(presentQueue, &presentInfo);
    if(returnCode != VK_SUCCESS)
        throw std::runtime_error("present failed. code: " + std::to_string(returnCode));
}




bool VulkanInstance::checkValidationLayerSupport(const std::vector<std::string> &validationLayers, std::string *missing) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (std::string layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (layerName == layerProperties.layerName) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            if(missing)
                *missing = layerName;
            return false;
        }
    }
    return true;
}

VkPhysicalDevice VulkanInstance::findPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, const std::vector<std::string> &requiredDeviceExtensions) {
    const auto devices = physicalDevices(instance);

    for(const auto d : devices) {
        if(isDeviceSuitable(d, surface, requiredDeviceExtensions))
            return d;
    }
    return nullptr;
}


std::vector<std::string> VulkanInstance::availableValidationLayers() {
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

void VulkanInstance::initDebug(VkInstance instance, PFN_vkDebugReportCallbackEXT callback, VkDebugReportCallbackEXT *c) {
    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    createInfo.pfnCallback = callback;

    PFN_vkCreateDebugReportCallbackEXT e =
            (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
                instance,
                "vkCreateDebugReportCallbackEXT"
                );

    if(e(instance, &createInfo, nullptr, c) != VK_SUCCESS) {
        throw std::runtime_error("[error] Debug init error");
    }
}


std::vector<VkPhysicalDevice> VulkanInstance::physicalDevices(VkInstance instance) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        return std::vector<VkPhysicalDevice>();
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    return devices;
}

bool VulkanInstance::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<std::string> &requiredDeviceExtensions) {
    if(device == nullptr)
        return false;

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);


    bool extensionsSupported = checkDeviceExtensionSupport(device, requiredDeviceExtensions);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return swapChainAdequate && extensionsSupported;
}

VulkanInstance::SwapChainSupportDetails VulkanInstance::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR VulkanInstance::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR VulkanInstance::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanInstance::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
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

void VulkanInstance::createSwapChain(VkPhysicalDevice physicalDevice, VkDevice logicDevice, VkSurfaceKHR surface, QueueTools::QueueFamilyIndices queueFamilies, VkSurfaceFormatKHR *surfaceFormat, VkExtent2D *extent, VkSwapchainKHR *swapChain) {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

    *surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    *extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = 3;
    if (imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo { };
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat->format;
    createInfo.imageColorSpace = surfaceFormat->colorSpace;
    createInfo.imageExtent = *extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


    uint32_t queueFamilyIndices[] = { queueFamilies.graphicsFamily(), queueFamilies.presentFamily()};

    if (queueFamilies.graphicsFamily() != queueFamilies.presentFamily()) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = {}; // Optional
        std::cout << "[warning] graphics and presentation queue families has same indices.\n";
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    const auto code = vkCreateSwapchainKHR(logicDevice, &createInfo, nullptr, swapChain);
    if (code != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain. code: " + std::to_string(code));
    }
}

void VulkanInstance::createImages(VkDevice logicDevice, VkSwapchainKHR swapChain, std::vector<VkImage> *swapChainImages) {
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(logicDevice, swapChain, &imageCount, nullptr);
    swapChainImages->resize(imageCount);
    vkGetSwapchainImagesKHR(logicDevice, swapChain, &imageCount, swapChainImages->data());
}

void VulkanInstance::createImageViewes(VkDevice logicDevice, const std::vector<VkImage> &swapChainImages, VkFormat swapChainImageFormat, std::vector<VkImageView> *swapChainImageViews) {
    swapChainImageViews->resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(logicDevice, &createInfo, nullptr, &swapChainImageViews->operator[](i)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void VulkanInstance::createRenderPass(VkDevice logicDevice, VkFormat swapChainImageFormat, VkRenderPass *renderPass) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;


    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(logicDevice, &renderPassInfo, nullptr, renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void VulkanInstance::createFrameBuffers(VkDevice logicDevice, VkExtent2D extent, VkRenderPass renderPass, const std::vector<VkImageView> &swapChainImageViews, std::vector<VkFramebuffer> *swapChainFramebuffers) {
    swapChainFramebuffers->resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {
            swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(logicDevice, &framebufferInfo, nullptr, &swapChainFramebuffers->operator[](i)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void VulkanInstance::createCommandPool(VkDevice logicDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool *commandPool) {
    QueueTools::QueueFamilyIndices queueFamilyIndices = QueueTools::findQueueFamilies(physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily();
    poolInfo.flags = 0; // Optional

    if (vkCreateCommandPool(logicDevice, &poolInfo, nullptr, commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void VulkanInstance::createCommandBuffers(VkDevice logicDevice, uint32_t count, VkCommandPool commandPool, std::vector<VkCommandBuffer> *commandBuffers) {
    commandBuffers->resize(count);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers->size();

    if (vkAllocateCommandBuffers(logicDevice, &allocInfo, commandBuffers->data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

}

void VulkanInstance::proceedCommandBuffers(VkRenderPass renderPass, VkPipeline pipeline, VkExtent2D extent, std::vector<VkFramebuffer> swapChainFramebuffers, std::vector<VkCommandBuffer> commandBuffers) {
    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers.at(i), &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = extent;

        VkClearValue clearColor = { 0.5f, 0.5f, 0.0f, 1.0f };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;


        vkCmdBeginRenderPass(commandBuffers.at(i), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffers.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdDraw(commandBuffers.at(i), 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffers.at(i));

        if (vkEndCommandBuffer(commandBuffers.at(i)) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
}

void VulkanInstance::createSyncObjects(VkDevice logicDevice, VkSemaphore *imageAvailableSemaphore, VkSemaphore *renderFinishedSemaphore) {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (
            vkCreateSemaphore(logicDevice, &semaphoreInfo, nullptr, imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(logicDevice, &semaphoreInfo, nullptr, renderFinishedSemaphore) != VK_SUCCESS
            )
        throw std::runtime_error("failed to create synchronization objects for a frame!");

}

VkShaderModule VulkanInstance::createShaderModule(VkDevice logicDevice, const std::vector<char> &code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(logicDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void VulkanInstance::createGraphicsPipeline(VkDevice logicDevice, VkExtent2D extent, VkRenderPass renderPass, VkPipelineLayout *pipelineLayout, VkPipeline *graphicsPipline) {
    auto vertShaderCode = readFile("../shaders/vert.spv");
    auto fragShaderCode = readFile("../shaders/frag.spv");

    VkShaderModule vertShaderModule = createShaderModule(logicDevice, vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(logicDevice, fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) extent.width;
    viewport.height = (float) extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;

    VkPipelineViewportStateCreateInfo viewportState {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(logicDevice, &pipelineLayoutInfo, nullptr, pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
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
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(logicDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, graphicsPipline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(logicDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(logicDevice, vertShaderModule, nullptr);
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

QueueTools::QueueFamilyIndices VulkanInstance::createLogicalDevice(VkDevice *device, VkPhysicalDevice d, VkSurfaceKHR surface, const std::vector<std::string> &requiredDeviceExtensions, const std::vector<std::string> &requiredValidationLayers) {
    QueueTools::QueueFamilyIndices indices = QueueTools::findQueueFamilies(d, surface);

    if(!indices.isValid())
        throw std::runtime_error("[error] missing graphics family on current gpu");

    std::vector<const char*> ext_buff(requiredDeviceExtensions.size());
    for(size_t i = 0; i < requiredDeviceExtensions.size(); ++i) {
        ext_buff[i] = requiredDeviceExtensions[i].c_str();
    }

    std::vector<const char*> valid_lay_buff(requiredValidationLayers.size());
    for(size_t i = 0; i < requiredValidationLayers.size(); ++i) {
        valid_lay_buff[i] = requiredValidationLayers[i].c_str();
    }

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily();
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = ext_buff.size();
    createInfo.ppEnabledExtensionNames = ext_buff.data();
    createInfo.enabledLayerCount = requiredValidationLayers.size();
    createInfo.ppEnabledLayerNames = valid_lay_buff.data();



    const auto code = vkCreateDevice(d, &createInfo, nullptr, device);
    if (code != VK_SUCCESS)
        throw std::runtime_error("failed to create logical device. code: " + std::to_string(code));

    return indices;
}

VkBool32 VulkanInstance::debugVulkanCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t messageCode,
        const char *pLayerPrefix,
        const char *msg,
        void *userData) {
    std::cout << "\nValidation layer report:\n"
              << "  " << pLayerPrefix << "\n"
              << "  " << msg << "\n";

    return VK_FALSE;
}


bool VulkanInstance::checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<std::string> &requeredExtensions) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> __requiredExtensions(requeredExtensions.begin(), requeredExtensions.end());
    for (const auto& extension : availableExtensions) {
        __requiredExtensions.erase(extension.extensionName);
    }
    return __requiredExtensions.empty();
}

void VulkanInstance::createSurface(GLFWwindow *window) {
    if(glfwCreateWindowSurface(vkInstance, window, NULL, &surface) != VK_SUCCESS) {
        throw std::runtime_error("surface creating error");
    }
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
std::vector<std::string> VulkanInstance::mergeExtensions(const std::vector<std::string> &e1, const std::vector<std::string> &e2) {
    std::vector<std::string> result = e1;
    for(auto e : e2) {
        if(std::find(result.begin(), result.end(), e) == result.end()) {
            result.push_back(e);
        }
    }
    return result;
}

void VulkanInstance::createInstance(VkInstance *instance, VkApplicationInfo *applicationInfo, const std::vector<std::string> &requiredExtensions) {
    uint32_t presentExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &presentExtensionCount, nullptr);
    std::vector<VkExtensionProperties> presentExtensions(presentExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &presentExtensionCount, presentExtensions.data());

    std::vector<std::string> presentExtensionNames(presentExtensionCount);
    for(uint32_t i = 0; i < presentExtensionCount; ++i) {
         presentExtensionNames[i] = presentExtensions[i].extensionName;
    }
    for(auto e : requiredExtensions) {
        if(std::find(presentExtensionNames.begin(), presentExtensionNames.end(), e) == presentExtensionNames.end()) {
            std::stringstream ss;
            ss << "[error] Extension not present: " << e << ".\n";
            ss << "Requered extensions: " << requiredExtensions << ".\n";
            ss << "Present extensions: " << presentExtensionNames << ".\n";
            throw std::runtime_error(ss.str().c_str());
        }
    }

    std::vector<const char*> ext_buff(requiredExtensions.size());
    for(size_t i = 0; i < requiredExtensions.size(); ++i) {
        ext_buff[i] = requiredExtensions[i].c_str();
    }
    VkInstanceCreateInfo createInfo { };
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = applicationInfo;
    createInfo.enabledExtensionCount = ext_buff.size();
    createInfo.ppEnabledExtensionNames = ext_buff.data();
    createInfo.enabledLayerCount = 0;

    const auto result = vkCreateInstance(&createInfo, nullptr, instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance. code: " + std::to_string(result));
    }
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

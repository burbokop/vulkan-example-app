#include "renderer.h"

#include <iostream>
#include <fstream>


std::vector<std::string> e172vp::Renderer::glfwExtensions() {
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

void e172vp::Renderer::proceedCommandBuffers(vk::RenderPass renderPass, vk::Pipeline pipeline, vk::Extent2D extent, std::vector<vk::Framebuffer> swapChainFramebuffers, std::vector<vk::CommandBuffer> commandBuffers, const e172vp::Renderer::CommandReciept &reciept) {
    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers.at(i), &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }


        VkClearValue clearColor;
        VkOffset2D offset;

        clearColor = { 0.5f, 0.5f, 0.0f, 1.0f };
        offset = { 0, 0 };

        VkRenderPassBeginInfo renderPassInfo {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = offset;
        renderPassInfo.renderArea.extent = extent;
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

void e172vp::Renderer::resetCommandBuffers(std::vector<vk::CommandBuffer> commandBuffers, vk::Queue graphicsQueue, vk::Queue presentQueue) {
    vkQueueWaitIdle(graphicsQueue);
    vkQueueWaitIdle(presentQueue);

    for(auto b : commandBuffers) {
        vkResetCommandBuffer(b, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }
}

void e172vp::Renderer::createGraphicsPipeline(vk::Device logicDevice, vk::Extent2D extent, vk::RenderPass renderPass, vk::PipelineLayout *pipelineLayout, vk::Pipeline *graphicsPipline) {
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

std::vector<char> e172vp::Renderer::readFile(const std::string &filename) {
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

void e172vp::Renderer::createSyncObjects(vk::Device logicDevice, vk::Semaphore *imageAvailableSemaphore, vk::Semaphore *renderFinishedSemaphore) {
    vk::SemaphoreCreateInfo semaphoreInfo;
    if (
            logicDevice.createSemaphore(&semaphoreInfo, nullptr, imageAvailableSemaphore) != vk::Result::eSuccess ||
            logicDevice.createSemaphore(&semaphoreInfo, nullptr, renderFinishedSemaphore) != vk::Result::eSuccess
            )
        throw std::runtime_error("failed to create synchronization objects for a frame!");
}

vk::ShaderModule e172vp::Renderer::createShaderModule(vk::Device logicDevice, const std::vector<char> &code) {
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


e172vp::Renderer::Renderer() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    GraphicsInstanceCreateInfo createInfo;
    createInfo.setRequiredExtensions(glfwExtensions());
    createInfo.setApplicationName("test-app");
    createInfo.setApplicationVersion(1);
    createInfo.setDebugEnabled(true);
    createInfo.setRequiredDeviceExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });
    createInfo.setSurfaceCreator([this](vk::Instance i, vk::SurfaceKHR *s) {
        VkSurfaceKHR ss;
        if(glfwCreateWindowSurface(i, m_window, NULL, &ss) != VK_SUCCESS) {
            throw std::runtime_error("surface creating error");
        }
        *s = ss;
    });

    m_graphicsInstance = GraphicsInstance(createInfo);

    if(!m_graphicsInstance.isValid()) {
        std::cout << "GRAPHICS OBJECT IS NOT CREATED BECAUSE OF FOLOWING ERRORS:\n\n";

    }

    while (m_graphicsInstance.hasErrors()) {
        std::cerr << m_graphicsInstance.nextError() << "\n";
    }

    createSyncObjects(m_graphicsInstance.logicalDevice(), &imageAvailableSemaphore, &renderFinishedSemaphore);
    createGraphicsPipeline(m_graphicsInstance.logicalDevice(), m_graphicsInstance.extent(), m_graphicsInstance.renderPass(), &pipelineLayout, &graphicsPipeline);

    proceedCommandBuffers(m_graphicsInstance.renderPass(), graphicsPipeline, m_graphicsInstance.extent(), m_graphicsInstance.swapChainFramebuffers(), m_graphicsInstance.commandBuffers(), CommandReciept());

}

void e172vp::Renderer::paintPoint(int x, int y) {

}

bool e172vp::Renderer::isAlive() const {
    glfwPollEvents();
    return !glfwWindowShouldClose(m_window);
}

void e172vp::Renderer::update() {
    //resetCommandBuffers(m_graphicsInstance.commandBuffers(), m_graphicsInstance.graphicsQueue(), m_graphicsInstance.presentQueue());

    //CommandReciept reciept;
    //reciept.is_valid = true;
    //reciept.x = std::cos(elapsedFromStart.elapsed() * 0.001) / 2 + 0.5;
    //reciept.y = std::sin(elapsedFromStart.elapsed() * 0.001) / 2 + 0.5;

    //proceedCommandBuffers(m_graphicsInstance.renderPass(), graphicsPipeline, m_graphicsInstance.extent(), m_graphicsInstance.swapChainFramebuffers(), m_graphicsInstance.commandBuffers(), reciept);

    uint32_t imageIndex = 0;
    vk::Result returnCode;

    returnCode = m_graphicsInstance.logicalDevice().acquireNextImageKHR(m_graphicsInstance.swapChain(), UINT64_MAX, imageAvailableSemaphore, vk::Fence(), &imageIndex);
    if(returnCode != vk::Result::eSuccess)
        throw std::runtime_error("acquiring next image failed. code: " + vk::to_string(returnCode));

    vk::Semaphore waitSemaphores[] = { imageAvailableSemaphore };
    vk::Semaphore signalSemaphores[] = { renderFinishedSemaphore };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    vk::SubmitInfo submitInfo{};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_graphicsInstance.commandBuffers()[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;


    returnCode = m_graphicsInstance.graphicsQueue().submit(1, &submitInfo, vk::Fence());
    if (returnCode != vk::Result::eSuccess)
        throw std::runtime_error("failed to submit draw command buffer. code: " + vk::to_string(returnCode));

    vk::SwapchainKHR swapChains[] = { m_graphicsInstance.swapChain() };

    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    returnCode = m_graphicsInstance.presentQueue().presentKHR(&presentInfo);
    if(returnCode != vk::Result::eSuccess)
        throw std::runtime_error("present failed. code: " + vk::to_string(returnCode));
}

e172vp::GraphicsInstance e172vp::Renderer::graphicsInstance() const {
    return m_graphicsInstance;
}

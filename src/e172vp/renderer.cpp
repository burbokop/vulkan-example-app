#include "renderer.h"

#include <iostream>
#include <fstream>
#include "tools/stringvector.h"
#include <math.h>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include "tools/buffer.h"

e172vp::Renderer::Renderer() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(WIDTH, HEIGHT, "test-app", nullptr, nullptr);

    GraphicsObjectCreateInfo createInfo;
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

    m_graphicsObject = GraphicsObject(createInfo);

    if(m_graphicsObject.debugEnabled())
        std::cout << "Used validation layers: " << StringVector::toString(m_graphicsObject.enabledValidationLayers()) << "\n";

    if(!m_graphicsObject.isValid())
        std::cout << "GRAPHICS OBJECT IS NOT CREATED BECAUSE OF FOLOWING ERRORS:\n\n";

    const auto errors = m_graphicsObject.pullErrors();
    if(errors.size())
        std::cerr << StringVector::toString(errors) << "\n";



    vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    indices = {
        0, 1, 2,
        2, 3, 0
    };




    createVertexBuffer(m_graphicsObject.logicalDevice(), m_graphicsObject.physicalDevice(), m_graphicsObject.commandPool(), m_graphicsObject.graphicsQueue(), vertices, &vertexBuffer, &vertexBufferMemory);
    createIndexBuffer(m_graphicsObject.logicalDevice(), m_graphicsObject.physicalDevice(), m_graphicsObject.commandPool(), m_graphicsObject.graphicsQueue(), indices, &indexBuffer, &indexBufferMemory);

    createDescriptorSetLayout(m_graphicsObject.logicalDevice(), &descriptorSetLayout);


    createUniformBuffers(m_graphicsObject.logicalDevice(), m_graphicsObject.physicalDevice(), m_graphicsObject.swapChain().imageCount(), uniformBuffers, uniformBuffersMemory);

    createDescriptorPool(m_graphicsObject.logicalDevice(), uniformBuffers.size(), &uniformDescriptorPool);
    createDescriptorSets(m_graphicsObject.logicalDevice(), uniformBuffers, descriptorSetLayout, uniformDescriptorPool, &uniformDescriptorSets);

    createGraphicsPipeline(m_graphicsObject.logicalDevice(), m_graphicsObject.swapChainSettings().extent, m_graphicsObject.renderPass(), descriptorSetLayout, &pipelineLayout, &graphicsPipeline);

    createSyncObjects(m_graphicsObject.logicalDevice(), &imageAvailableSemaphore, &renderFinishedSemaphore);


    for(size_t i = 0; i < m_graphicsObject.swapChain().imageCount(); ++i) {
        updateUniformBuffer(i);
    }


    elapsedFromStart.reset();
}

bool e172vp::Renderer::isAlive() const {
    glfwPollEvents();
    return !glfwWindowShouldClose(m_window);
}

void e172vp::Renderer::applyPresentation() {
    resetCommandBuffers(m_graphicsObject.commandPool().commandBufferVector(), m_graphicsObject.graphicsQueue(), m_graphicsObject.presentQueue());
    CommandReciept reciept;
    reciept.verticeCount = std::fmod(elapsedFromStart.elapsed() * 0.001, 5);
    reciept.verticeCount = vertices.size();
    reciept.indexCount = indices.size();
    reciept.offsetX = (std::cos(elapsedFromStart.elapsed() * 0.001)) * 100.;
    reciept.offsetY = (std::sin(elapsedFromStart.elapsed() * 0.001)) * 100.;
    proceedCommandBuffers(m_graphicsObject.renderPass(), graphicsPipeline, pipelineLayout, m_graphicsObject.swapChainSettings().extent, m_graphicsObject.renderPass().frameBufferVector(), m_graphicsObject.commandPool().commandBufferVector(), uniformDescriptorSets, vertexBuffer, indexBuffer, reciept);

    uint32_t imageIndex = 0;
    vk::Result returnCode;


    returnCode = m_graphicsObject.logicalDevice().acquireNextImageKHR(m_graphicsObject.swapChain(), UINT64_MAX, imageAvailableSemaphore, {}, &imageIndex);
    if(returnCode != vk::Result::eSuccess)
        throw std::runtime_error("acquiring next image failed. code: " + vk::to_string(returnCode));

    auto currentImageCommandBuffer = m_graphicsObject.commandPool().commandBuffer(imageIndex);

    vk::Semaphore waitSemaphores[] = { imageAvailableSemaphore };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };


    updateUniformBuffer(imageIndex);

    vk::SubmitInfo submitInfo{};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.setCommandBuffers(currentImageCommandBuffer);
    submitInfo.setSignalSemaphores(renderFinishedSemaphore);
    //submitInfo.signalSemaphoreCount = 1;
    //submitInfo.pSignalSemaphores = signalSemaphores;

    returnCode = m_graphicsObject.graphicsQueue().submit(1, &submitInfo, {});

    if (returnCode != vk::Result::eSuccess)
        throw std::runtime_error("failed to submit draw command buffer. code: " + vk::to_string(returnCode));

    vk::SwapchainKHR swapChains[] = { m_graphicsObject.swapChain() };

    vk::PresentInfoKHR presentInfo{};
    presentInfo.setWaitSemaphores(renderFinishedSemaphore);
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    returnCode = m_graphicsObject.presentQueue().presentKHR(&presentInfo);
    if(returnCode != vk::Result::eSuccess)
        throw std::runtime_error("present failed. code: " + vk::to_string(returnCode));

}

void e172vp::Renderer::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};

    glm::mat4 singleMatrix {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    };

    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //ubo.proj = glm::perspective(glm::radians(45.0f), m_graphicsObject.swapChainSettings().extent.width / (float) m_graphicsObject.swapChainSettings().extent.height, 0.1f, 10.0f);
    //ubo.proj[1][1] *= -1;

    //ubo.model = singleMatrix;
    ubo.view = singleMatrix;
    ubo.proj = singleMatrix;

    void* data;
    vkMapMemory(m_graphicsObject.logicalDevice(), uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(m_graphicsObject.logicalDevice(), uniformBuffersMemory[currentImage]);
}







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

void e172vp::Renderer::proceedCommandBuffers(const vk::RenderPass &renderPass, const vk::Pipeline &pipeline, const vk::PipelineLayout &pipelineLayout, const vk::Extent2D &extent, const std::vector<vk::Framebuffer> &swapChainFramebuffers, const std::vector<vk::CommandBuffer> &commandBuffers, const std::vector<vk::DescriptorSet> &uniformDescriptorSets, const vk::Buffer &vertexBuffer, const vk::Buffer &indexBuffer, const e172vp::Renderer::CommandReciept &reciept) {
    for (size_t i = 0; i < commandBuffers.size(); i++) {
        vk::CommandBufferBeginInfo beginInfo{};
        if (commandBuffers.at(i).begin(&beginInfo) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        const vk::ClearValue clearColor = vk::ClearColorValue(std::array<float, 4> { 0.5f, 0.5f, 0.0f, 1.0f });

        vk::RenderPassBeginInfo renderPassInfo {};
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = vk::Offset2D();
        renderPassInfo.renderArea.extent = extent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vk::Viewport viewport;
        viewport.setX(reciept.offsetX);
        viewport.setWidth(extent.width);
        viewport.setY(reciept.offsetY);
        viewport.setHeight(extent.height);
        viewport.setMinDepth(0.0f);
        viewport.setMaxDepth(1.0f);



        commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
        commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
        commandBuffers[i].setViewport(0, 1, &viewport);

        vk::Buffer vertexBuffers[] = { vertexBuffer };
        vk::DeviceSize offsets[] = { 0 };
        commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);

        commandBuffers[i].bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint16);
        commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &uniformDescriptorSets[i], 0, nullptr);
        commandBuffers[i].drawIndexed(static_cast<uint32_t>(reciept.indexCount), 1, 0, 0, 0);

        commandBuffers[i].endRenderPass();
        commandBuffers[i].end();
    }
}

void e172vp::Renderer::resetCommandBuffers(const std::vector<vk::CommandBuffer> &commandBuffers, const vk::Queue &graphicsQueue, const vk::Queue &presentQueue) {
    graphicsQueue.waitIdle();
    presentQueue.waitIdle();

    for(auto b : commandBuffers) {
        b.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    }
}

void e172vp::Renderer::createGraphicsPipeline(const vk::Device &logicDevice, const vk::Extent2D &extent, const vk::RenderPass &renderPass, const vk::DescriptorSetLayout& descriptorSetLayout, vk::PipelineLayout *pipelineLayout, vk::Pipeline *graphicsPipline) {
    bool useUniformBuffer = true;

    std::vector<char> vertShaderCode;
    if(useUniformBuffer) {
        vertShaderCode = readFile("../shaders/vertex_buffer.spv");
    } else {
        vertShaderCode = readFile("../shaders/vert.spv");
    }
    std::vector<char> fragShaderCode = readFile("../shaders/frag.spv");

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


    //vertex input
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo {};
    const auto bindingDescription = Vertex::bindingDescription();
    const auto attributeDescriptions = Vertex::attributeDescriptions();
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = false;

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
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

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

void e172vp::Renderer::createSyncObjects(const vk::Device &logicDevice, vk::Semaphore *imageAvailableSemaphore, vk::Semaphore *renderFinishedSemaphore) {
    vk::SemaphoreCreateInfo semaphoreInfo;
    if (
            logicDevice.createSemaphore(&semaphoreInfo, nullptr, imageAvailableSemaphore) != vk::Result::eSuccess ||
            logicDevice.createSemaphore(&semaphoreInfo, nullptr, renderFinishedSemaphore) != vk::Result::eSuccess
            )
        throw std::runtime_error("failed to create synchronization objects for a frame!");
}


void e172vp::Renderer::createVertexBuffer(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, const vk::CommandPool &commandPool, const vk::Queue &graphicsQueue, const std::vector<Vertex> &vertices, vk::Buffer *vertexBuffer, vk::DeviceMemory *vertexBufferMemory) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    Buffer::createBuffer(logicalDevice, physicalDevice, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    Buffer::createBuffer(logicalDevice, physicalDevice, bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, *vertexBuffer, *vertexBufferMemory);

    Buffer::copyBuffer(logicalDevice, commandPool, graphicsQueue, stagingBuffer, *vertexBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void e172vp::Renderer::createIndexBuffer(const vk::Device &logicalDevice,
                                         const vk::PhysicalDevice &physicalDevice,
                                         const vk::CommandPool &commandPool,
                                         const vk::Queue &graphicsQueue,
                                         const std::vector<uint16_t> &indices,
                                         vk::Buffer *indexBuffer,
                                         vk::DeviceMemory *indexBufferMemory
                                         ) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    Buffer::createBuffer(logicalDevice, physicalDevice, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    Buffer::createBuffer(logicalDevice, physicalDevice, bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, *indexBuffer, *indexBufferMemory);

    Buffer::copyBuffer(logicalDevice, commandPool, graphicsQueue, stagingBuffer, *indexBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void e172vp::Renderer::createDescriptorSetLayout(const vk::Device &logicalDevice, vk::DescriptorSetLayout *descriptorSetLayout) {
    vk::DescriptorSetLayoutBinding uboLayoutBinding;
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (logicalDevice.createDescriptorSetLayout(&layoutInfo, nullptr, descriptorSetLayout) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void e172vp::Renderer::createUniformBuffers(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, size_t count, std::vector<vk::Buffer> &uniformBuffers, std::vector<vk::DeviceMemory> &uniformBuffersMemory) {
    const vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(count);
    uniformBuffersMemory.resize(count);

    for (size_t i = 0; i < count; i++) {
        Buffer::createBuffer(logicalDevice, physicalDevice, bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, uniformBuffers[i], uniformBuffersMemory[i]);
    }
}

void e172vp::Renderer::createDescriptorPool(const vk::Device &logicalDevice, size_t size, vk::DescriptorPool *uniformDescriptorPool) {
    vk::DescriptorPoolSize poolSize;
    poolSize.type = vk::DescriptorType::eUniformBuffer;
    poolSize.descriptorCount = static_cast<uint32_t>(size);

    vk::DescriptorPoolCreateInfo poolInfo;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(size);

    if (logicalDevice.createDescriptorPool(&poolInfo, nullptr, uniformDescriptorPool) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void e172vp::Renderer::createDescriptorSets(const vk::Device &logicalDevice, const std::vector<vk::Buffer> &uniformBuffers, const vk::DescriptorSetLayout &descriptorSetLayout, const vk::DescriptorPool &descriptorPool, std::vector<vk::DescriptorSet> *descriptorSets) {
    std::vector<vk::DescriptorSetLayout> layouts(uniformBuffers.size(), descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(uniformBuffers.size());
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets->resize(uniformBuffers.size());
    if (logicalDevice.allocateDescriptorSets(&allocInfo, descriptorSets->data()) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < uniformBuffers.size(); i++) {
        vk::DescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = descriptorSets->at(i);
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        logicalDevice.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
    }
}

vk::ShaderModule e172vp::Renderer::createShaderModule(const vk::Device &logicDevice, const std::vector<char> &code) {
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.setCodeSize(code.size());
    createInfo.setPCode(reinterpret_cast<const uint32_t*>(code.data()));

    vk::ShaderModule shaderModule;
    if (logicDevice.createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

e172vp::GraphicsObject e172vp::Renderer::graphicsObject() const { return m_graphicsObject; }

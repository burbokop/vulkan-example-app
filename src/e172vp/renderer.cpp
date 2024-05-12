#include "renderer.h"

#include "tools/Model.h"
#include "tools/buffer.h"
#include "tools/stringvector.h"
#include <chrono>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../../image/stb_image.h"

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
    createInfo.setRequiredDeviceExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_EXT_memory_budget" });
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
    if (errors.size() > 0) {
        std::cerr << StringVector::toString(errors) << "\n";
    }

    m_globalDescriptorSetLayout = DescriptorSetLayout::createUniformDSL(m_graphicsObject.logicalDevice(), 0);
    m_objectDescriptorSetLayout = DescriptorSetLayout::createUniformDSL(m_graphicsObject.logicalDevice(), 1);
    m_samplerDescriptorSetLayout = DescriptorSetLayout::createSamplerDSL(m_graphicsObject.logicalDevice(), 2);

    Buffer::createUniformBuffers<GlobalUniformBufferObject>(
        &m_graphicsObject,
        m_graphicsObject.swapChain().imageCount(),
        &m_uniformBuffers,
        &m_uniformBuffersMemory);

    Buffer::createUniformDescriptorSets<GlobalUniformBufferObject>(
        m_graphicsObject.logicalDevice(),
        m_graphicsObject.descriptorPool(),
        m_uniformBuffers,
        &m_globalDescriptorSetLayout,
        &m_uniformDescriptorSets);

    //    bool useUniformBuffer = true;
    //    std::vector<char> vertShaderCode;
    //    if(useUniformBuffer) {
    //        vertShaderCode = readFile("shaders/vert_uniform.vert.spv");
    //    } else {
    //        vertShaderCode = readFile("shaders/shader.vert.spv");
    //    }
    //    std::vector<char> fragShaderCode = readFile("shaders/shader.frag.spv");

    //    pipeline = new Pipeline(m_graphicsObject.logicalDevice(),
    //        m_graphicsObject.swapChainSettings().extent,
    //        m_graphicsObject.renderPass(),
    //        { globalDescriptorSetLayout.descriptorSetLayoutHandle(),
    //            objectDescriptorSetLayout.descriptorSetLayoutHandle(),
    //            samplerDescriptorSetLayout.descriptorSetLayoutHandle() },
    //        vertShaderCode,
    //        fragShaderCode,
    //        vk::PrimitiveTopology::eTriangleList);

    createSyncObjects(m_graphicsObject.logicalDevice(), &m_imageAvailableSemaphore, &m_renderFinishedSemaphore);

    m_font = new Font(m_graphicsObject.logicalDevice(),
        m_graphicsObject.physicalDevice(),
        m_graphicsObject.commandPool(),
        m_graphicsObject.graphicsQueue(),
        "fonts/ZCOOL.ttf",
        128);

    m_elapsedFromStart.reset();
}

std::shared_ptr<e172vp::Pipeline> e172vp::Renderer::createPipeline(const std::vector<std::uint8_t>& vertShaderCode, const std::vector<std::uint8_t>& fragShaderCode)
{
    return std::make_shared<Pipeline>(m_graphicsObject.logicalDevice(),
        m_graphicsObject.swapChainSettings().extent,
        m_graphicsObject.renderPass(),
        std::vector { m_globalDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_objectDescriptorSetLayout.descriptorSetLayoutHandle(),
            m_samplerDescriptorSetLayout.descriptorSetLayoutHandle() },
        vertShaderCode,
        fragShaderCode,
        vk::PrimitiveTopology::eTriangleList);
}

bool e172vp::Renderer::isAlive() const {
    glfwPollEvents();
    return !glfwWindowShouldClose(m_window);
}

void e172vp::Renderer::applyPresentation() {
    resetCommandBuffers(m_graphicsObject.commandPool().commandBufferVector(), m_graphicsObject.graphicsQueue(), m_graphicsObject.presentQueue());
    proceedCommandBuffers(m_graphicsObject.renderPass(),
        m_graphicsObject.swapChainSettings().extent,
        m_graphicsObject.renderPass().frameBufferVector(),
        m_graphicsObject.commandPool().commandBufferVector(),
        m_uniformDescriptorSets,
        m_vertexObjects);

    std::uint32_t imageIndex = 0;
    vk::Result returnCode;

    returnCode = m_graphicsObject.logicalDevice().acquireNextImageKHR(m_graphicsObject.swapChain(), UINT64_MAX, m_imageAvailableSemaphore, {}, &imageIndex);
    if(returnCode != vk::Result::eSuccess)
        throw std::runtime_error("acquiring next image failed. code: " + vk::to_string(returnCode));

    auto currentImageCommandBuffer = m_graphicsObject.commandPool().commandBuffer(imageIndex);

    vk::Semaphore waitSemaphores[] = { m_imageAvailableSemaphore };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };


    updateUniformBuffer(imageIndex);

    for (auto& o : m_vertexObjects) {
        o->updateUbo(imageIndex);
    }

    vk::SubmitInfo submitInfo;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.setCommandBuffers(currentImageCommandBuffer);
    submitInfo.setSignalSemaphores(m_renderFinishedSemaphore);

    returnCode = m_graphicsObject.graphicsQueue().submit(1, &submitInfo, {});


    if (returnCode != vk::Result::eSuccess)
        throw std::runtime_error("failed to submit draw command buffer. code: " + vk::to_string(returnCode));

    vk::SwapchainKHR swapChains[] = { m_graphicsObject.swapChain() };

    vk::PresentInfoKHR presentInfo;
    presentInfo.setWaitSemaphores(m_renderFinishedSemaphore);
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
    GlobalUniformBufferObject ubo;
    // ubo.offset = { std::cos(time * 0.2) * 0.2, std::sin(time * 0.2) * 0.2 };
    ubo.offset = { 0, 0 };
    ubo.currentTime = time;

    double xpos, ypos;
    // getting cursor position
    glfwGetCursorPos(m_window, &xpos, &ypos);

    int w, h;
    glfwGetWindowSize(m_window, &w, &h);

    std::cout << "Cursor Position at (" << xpos << " : " << ypos << std::endl;

    ubo.mousePosition = { xpos / w, ypos / h };
    void* data;
    vkMapMemory(m_graphicsObject.logicalDevice(), m_uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(m_graphicsObject.logicalDevice(), m_uniformBuffersMemory[currentImage]);
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

void e172vp::Renderer::proceedCommandBuffers(
    const vk::RenderPass& renderPass,
    const vk::Extent2D& extent,
    const std::vector<vk::Framebuffer>& swapChainFramebuffers,
    const std::vector<vk::CommandBuffer>& commandBuffers,
    const std::vector<vk::DescriptorSet>& uniformDescriptorSets,
    const std::list<VertexObject*>& vertexObjects)
{
    for (size_t i = 0; i < commandBuffers.size(); i++) {
        vk::CommandBufferBeginInfo beginInfo{};
        if (commandBuffers[i].begin(&beginInfo) != vk::Result::eSuccess) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        //#1A0033
        const vk::ClearValue clearColor = vk::ClearColorValue(std::array<float, 4> {
                                                                  0x1a / 256.,
                                                                  0x00 / 256.,
                                                                  0x33 / 256.,
                                                                  0.4f
                                                              });

        vk::RenderPassBeginInfo renderPassInfo;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = vk::Offset2D();
        renderPassInfo.renderArea.extent = extent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vk::Viewport viewport;
        viewport.setX(0);
        viewport.setWidth(extent.width);
        viewport.setY(0);
        viewport.setHeight(extent.height);
        viewport.setMinDepth(0.0f);
        viewport.setMaxDepth(1.0f);

        commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
        commandBuffers[i].setViewport(0, 1, &viewport);

        for (auto object : vertexObjects) {
            vk::Buffer vb[] = { object->vertexBuffer() };
            vk::DeviceSize offsets[] = { 0 };

            commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, object->pipeline()->handle());

            commandBuffers[i].bindVertexBuffers(0, 1, vb, offsets);
            commandBuffers[i].bindIndexBuffer(object->indexBuffer(), 0, vk::IndexType::eUint32);

            commandBuffers[i].bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                object->pipeline()->pipelineLayout(),
                0,
                { uniformDescriptorSets[i],
                    object->descriptorSets()[i],
                    object->textureDescriptorSets()[i] },
                {});
            commandBuffers[i].drawIndexed(object->indexCount(), 1, 0, 0, 0);
        }

        //        vk::ImageBlit blit;
        //        commandBuffers[i].blitImage(fgImage, vk::ImageLayout::eUndefined, swapChainImages[i], vk::ImageLayout::eTransferDstOptimal, { blit }, vk::Filter::eLinear);
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

e172vp::VertexObject* e172vp::Renderer::addObject(const e172vp::Mesh& mesh, Shared<Pipeline> pipeline)
{
    const auto r = new VertexObject(
        &m_graphicsObject,
        m_graphicsObject.swapChain().imageCount(),
        &m_objectDescriptorSetLayout,
        &m_samplerDescriptorSetLayout,
        mesh,
        m_font->character('F').imageView(),
        pipeline);
    m_vertexObjects.push_back(r);
    return r;
}

e172vp::VertexObject* e172vp::Renderer::addCharacter(char c, std::shared_ptr<Pipeline> pipeline)
{
    const static std::vector<e172vp::Vertex> v = {
        { { -0.1f, -0.1f, 0 }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
        { { 0.1f, -0.1f, 0 }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
        { { 0.1f, 0.1f, 0 }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
        { { -0.1f, 0.1f, 0 }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } }
    };
    const static std::vector<uint32_t> i = {
        0, 1, 2,
        2, 3, 0
    };

    const auto r = new VertexObject(
        &m_graphicsObject,
        m_graphicsObject.swapChain().imageCount(),
        &m_objectDescriptorSetLayout,
        &m_samplerDescriptorSetLayout,
        Mesh(v, i),
        m_font->character(c).imageView(), pipeline);
    m_vertexObjects.push_back(r);
    return r;
}

e172vp::VertexObject* e172vp::Renderer::addObject(const Model& model)
{
    return addObject(model.mesh(), createPipeline(model.vert(), model.frag()));
}

bool e172vp::Renderer::removeVertexObject(e172vp::VertexObject *vertexObject) {
    const auto it = std::find(m_vertexObjects.begin(), m_vertexObjects.end(), vertexObject);
    if (it != m_vertexObjects.end()) {
        delete vertexObject;
        m_vertexObjects.erase(it);
        return true;
    }
    return true;
}

void e172vp::Renderer::createSyncObjects(const vk::Device &logicDevice, vk::Semaphore *imageAvailableSemaphore, vk::Semaphore *renderFinishedSemaphore) {
    vk::SemaphoreCreateInfo semaphoreInfo;
    if (
            logicDevice.createSemaphore(&semaphoreInfo, nullptr, imageAvailableSemaphore) != vk::Result::eSuccess ||
            logicDevice.createSemaphore(&semaphoreInfo, nullptr, renderFinishedSemaphore) != vk::Result::eSuccess
            )
        throw std::runtime_error("failed to create synchronization objects for a frame!");
}

e172vp::GraphicsObject e172vp::Renderer::graphicsObject() const { return m_graphicsObject; }

#ifndef RENDERER_H
#define RENDERER_H

#include "graphicsobject.h"
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#include <GLFW/glfw3.h>
#include "time/elapsedtimer.h"
#include "vertex.h"

namespace e172vp {


class Renderer {
    GraphicsObject m_graphicsObject;
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    GLFWwindow *m_window = nullptr;

    vk::Semaphore imageAvailableSemaphore;
    vk::Semaphore renderFinishedSemaphore;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;

    vk::Buffer vertexBuffer;
    vk::DeviceMemory vertexBufferMemory;
    vk::Buffer indexBuffer;
    vk::DeviceMemory indexBufferMemory;

    e172vp::ElapsedTimer elapsedFromStart;


    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    vk::DescriptorSetLayout descriptorSetLayout;
    std::vector<vk::Buffer> uniformBuffers;
    vk::DescriptorPool uniformDescriptorPool;
    std::vector<vk::DescriptorSet> uniformDescriptorSets;


    std::vector<vk::DeviceMemory> uniformBuffersMemory;

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };
public:
    static std::vector<std::string> glfwExtensions();

    struct CommandReciept {
        float offsetX;
        float offsetY;
        size_t verticeCount = 0;
        size_t indexCount = 0;
        std::vector<vk::Offset2D> points;
        std::vector<vk::Rect2D> rects;
    };

    static void proceedCommandBuffers(const vk::RenderPass &renderPass, const vk::Pipeline &pipeline, const vk::PipelineLayout &pipelineLayout, const vk::Extent2D &extent, const std::vector<vk::Framebuffer> &swapChainFramebuffers, const std::vector<vk::CommandBuffer> &commandBuffers, const std::vector<vk::DescriptorSet> &uniformDescriptorSets, const vk::Buffer &vertexBuffer, const vk::Buffer &indexBuffer, const CommandReciept &reciept);
    static void resetCommandBuffers(const std::vector<vk::CommandBuffer> &commandBuffers, const vk::Queue &graphicsQueue, const vk::Queue &presentQueue);
    static void createGraphicsPipeline(const vk::Device &logicDevice, const vk::Extent2D &extent, const vk::RenderPass &renderPass, const vk::DescriptorSetLayout &descriptorSetLayout, vk::PipelineLayout *pipelineLayout, vk::Pipeline *graphicsPipline);
    static void createSyncObjects(const vk::Device &logicDevice, vk::Semaphore *imageAvailableSemaphore, vk::Semaphore *renderFinishedSemaphore);

    void createVertexBuffer(
            const vk::Device &logicalDevice,
            const vk::PhysicalDevice &physicalDevice,
            const vk::CommandPool &commandPool,
            const vk::Queue &graphicsQueue,
            const std::vector<Vertex> &vertices,
            vk::Buffer *vertexBuffer,
            vk::DeviceMemory *vertexBufferMemory
            );

    void createIndexBuffer(
            const vk::Device &logicalDevice,
            const vk::PhysicalDevice &physicalDevice,
            const vk::CommandPool &commandPool,
            const vk::Queue &graphicsQueue,
            const std::vector<uint16_t> &indices,
            vk::Buffer *indexBuffer,
            vk::DeviceMemory *indexBufferMemory
            );

    static void createDescriptorSetLayout(const vk::Device &logicalDevice, vk::DescriptorSetLayout *descriptorSetLayout);

    static void createUniformBuffers(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, size_t count, std::vector<vk::Buffer> &uniformBuffers, std::vector<vk::DeviceMemory> &uniformBuffersMemory);

    static void createDescriptorPool(const vk::Device &logicalDevice, size_t size, vk::DescriptorPool *uniformDescriptorPool);
    static void createDescriptorSets(const vk::Device &logicalDevice, const std::vector<vk::Buffer> &uniformBuffers, const vk::DescriptorSetLayout &descriptorSetLayout, const vk::DescriptorPool &descriptorPool, std::vector<vk::DescriptorSet> *descriptorSets);

    static vk::ShaderModule createShaderModule(const vk::Device &logicDevice, const std::vector<char> &code);
    static std::vector<char> readFile(const std::string &filename);
    Renderer();

    bool isAlive() const;
    void applyPresentation();
    void updateUniformBuffer(uint32_t currentImage);

    GraphicsObject graphicsObject() const;
};

}

#endif // RENDERER_H

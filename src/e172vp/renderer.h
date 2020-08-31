#ifndef RENDERER_H
#define RENDERER_H

#include "graphicsobject.h"
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#include <GLFW/glfw3.h>
#include "time/elapsedtimer.h"
#include "vertex.h"
#include <list>
#include "vertexobject.h"
#include "descriptorsetlayout.h"

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

    e172vp::DescriptorSetLayout globalDescriptorSetLayout;
    e172vp::DescriptorSetLayout objectDescriptorSetLayout;


    std::vector<vk::Buffer> uniformBuffers;
    std::vector<vk::DeviceMemory> uniformBuffersMemory;
    std::vector<vk::DescriptorSet> uniformDescriptorSets;


    std::vector<vk::Buffer> objectUniformBuffers;
    std::vector<vk::DeviceMemory> objectUniformBuffersMemory;
    std::vector<vk::DescriptorSet> objectUniformDescriptorSets;


    struct GlobalUniformBufferObject {
        glm::vec2 offset;
    };

    struct VOUniformBufferObject {
        glm::mat4 model;
    };

    std::list<VertexObject*> vertexObjects;
public:
    static std::vector<std::string> glfwExtensions();

    static void proceedCommandBuffers(
            const vk::RenderPass &renderPass,
            const vk::Pipeline &pipeline,
            const vk::PipelineLayout &pipelineLayout,
            const vk::Extent2D &extent,
            const std::vector<vk::Framebuffer> &swapChainFramebuffers,
            const std::vector<vk::CommandBuffer> &commandBuffers,
            const std::vector<vk::DescriptorSet> &uniformDescriptorSets,
            const std::list<VertexObject *> &vertexObjects
            );

    static void resetCommandBuffers(const std::vector<vk::CommandBuffer> &commandBuffers, const vk::Queue &graphicsQueue, const vk::Queue &presentQueue);
    static void createGraphicsPipeline(const vk::Device &logicDevice, const vk::Extent2D &extent, const vk::RenderPass &renderPass, const std::vector<vk::DescriptorSetLayout> &descriptorSetLayouts, vk::PipelineLayout *pipelineLayout, vk::Pipeline *graphicsPipline);
    static void createSyncObjects(const vk::Device &logicDevice, vk::Semaphore *imageAvailableSemaphore, vk::Semaphore *renderFinishedSemaphore);

    static vk::ShaderModule createShaderModule(const vk::Device &logicDevice, const std::vector<char> &code);
    static std::vector<char> readFile(const std::string &filename);

    VertexObject *addVertexObject(const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices);
    bool removeVertexObject(VertexObject *vertexObject);
    Renderer();

    bool isAlive() const;
    void applyPresentation();
    void updateUniformBuffer(uint32_t currentImage);

    GraphicsObject graphicsObject() const;
};

}

#endif // RENDERER_H

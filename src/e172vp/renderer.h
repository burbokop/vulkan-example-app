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
#include "tools/mesh.h"
#include "pipeline.h"

#include "font.h"

namespace e172vp {


class Renderer {
    GraphicsObject m_graphicsObject;
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    GLFWwindow *m_window = nullptr;

    vk::Semaphore imageAvailableSemaphore;
    vk::Semaphore renderFinishedSemaphore;

    Pipeline *pipeline = nullptr;

    vk::Buffer vertexBuffer;
    vk::DeviceMemory vertexBufferMemory;
    vk::Buffer indexBuffer;
    vk::DeviceMemory indexBufferMemory;

    e172vp::ElapsedTimer elapsedFromStart;

    e172vp::DescriptorSetLayout globalDescriptorSetLayout;
    e172vp::DescriptorSetLayout objectDescriptorSetLayout;
    e172vp::DescriptorSetLayout samplerDescriptorSetLayout;


    std::vector<vk::Buffer> uniformBuffers;
    std::vector<vk::DeviceMemory> uniformBuffersMemory;
    std::vector<vk::DescriptorSet> uniformDescriptorSets;

    Font *font = nullptr;

    struct GlobalUniformBufferObject {
        glm::vec2 offset;
        float currentTime;
    };

    std::list<VertexObject*> vertexObjects;
public:
    static std::vector<std::string> glfwExtensions();

    static void proceedCommandBuffers(const vk::RenderPass &renderPass,
            const vk::Pipeline &pipeline,
            const vk::PipelineLayout &pipelineLayout,
            const vk::Extent2D &extent,
            const std::vector<vk::Framebuffer> &swapChainFramebuffers,
            const std::vector<vk::CommandBuffer> &commandBuffers,
            const std::vector<vk::DescriptorSet> &uniformDescriptorSets,
            const std::list<VertexObject *> &vertexObjects);

    static void resetCommandBuffers(const std::vector<vk::CommandBuffer> &commandBuffers, const vk::Queue &graphicsQueue, const vk::Queue &presentQueue);
    static void createSyncObjects(const vk::Device &logicDevice, vk::Semaphore *imageAvailableSemaphore, vk::Semaphore *renderFinishedSemaphore);

    static std::vector<char> readFile(const std::string &filename);

    VertexObject *addVertexObject(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
    VertexObject *addCharacter(char c);

    VertexObject *addVertexObject(const Mesh &mesh);
    bool removeVertexObject(VertexObject *vertexObject);
    Renderer();

    bool isAlive() const;
    void applyPresentation();
    void updateUniformBuffer(uint32_t currentImage);

    GraphicsObject graphicsObject() const;
};

}

#endif // RENDERER_H

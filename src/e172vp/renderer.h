#ifndef RENDERER_H
#define RENDERER_H

#include "graphicsinstance.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace e172vp {


class Renderer {
    GraphicsInstance m_graphicsInstance;
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    GLFWwindow *m_window = nullptr;

    vk::Semaphore imageAvailableSemaphore;
    vk::Semaphore renderFinishedSemaphore;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;

public:
    static std::vector<std::string> glfwExtensions();

    struct CommandReciept {

    };

    static void proceedCommandBuffers(const vk::RenderPass &renderPass, const vk::Pipeline &pipeline, const vk::Extent2D &extent, const std::vector<vk::Framebuffer> &swapChainFramebuffers, const std::vector<vk::CommandBuffer> &commandBuffers, const CommandReciept &reciept);
    static void resetCommandBuffers(const std::vector<vk::CommandBuffer> &commandBuffers, const vk::Queue &graphicsQueue, const vk::Queue &presentQueue);
    static void createGraphicsPipeline(const vk::Device &logicDevice, const vk::Extent2D &extent, const vk::RenderPass &renderPass, vk::PipelineLayout *pipelineLayout, vk::Pipeline *graphicsPipline);
    static void createSyncObjects(const vk::Device &logicDevice, vk::Semaphore *imageAvailableSemaphore, vk::Semaphore *renderFinishedSemaphore);
    static vk::ShaderModule createShaderModule(const vk::Device &logicDevice, const std::vector<char> &code);

    static std::vector<char> readFile(const std::string &filename);

    Renderer();

    //void paintPoint(int x, int y);
    bool isAlive() const;

    void update();

    GraphicsInstance graphicsInstance() const;
};

}

#endif // RENDERER_H

#ifndef RENDERER_H
#define RENDERER_H

#include "graphicsinstance.h"
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

    static void proceedCommandBuffers(vk::RenderPass renderPass, vk::Pipeline pipeline, vk::Extent2D extent, std::vector<vk::Framebuffer> swapChainFramebuffers, std::vector<vk::CommandBuffer> commandBuffers, const CommandReciept &reciept);
    static void resetCommandBuffers(std::vector<vk::CommandBuffer> commandBuffers, vk::Queue graphicsQueue, vk::Queue presentQueue);
    static void createGraphicsPipeline(vk::Device logicDevice, vk::Extent2D extent, vk::RenderPass renderPass, vk::PipelineLayout *pipelineLayout, vk::Pipeline *graphicsPipline);
    static void createSyncObjects(vk::Device logicDevice, vk::Semaphore *imageAvailableSemaphore, vk::Semaphore *renderFinishedSemaphore);
    static vk::ShaderModule createShaderModule(vk::Device logicDevice, const std::vector<char> &code);

    static std::vector<char> readFile(const std::string &filename);

    Renderer();

    void paintPoint(int x, int y);
    bool isAlive() const;

    void update();

    GraphicsInstance graphicsInstance() const;
};

}

#endif // RENDERER_H

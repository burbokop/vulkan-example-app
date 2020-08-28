#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#include "queuetools.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <GLFW/glfw3.h>
#include <string>
#include "time/elapsedtimer.h"
#include <queue>
#include <vulkan/vulkan.hpp>
#include "e172vp/tools/hardware.h"


std::ostream &operator<<(std::ostream& stream, const std::vector<std::string>& value);

class VulkanInstance {
    friend class WindowInstance;

    std::queue<std::string> m_errors;
    VkDebugReportCallbackEXT debugReportCallbackEXT = VK_NULL_HANDLE;

    vk::Instance vkInstance;
    vk::SurfaceKHR surface;
    vk::PhysicalDevice physicalDevice;
    vk::Device logicalDevice;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    vk::SwapchainKHR swapChain;
    vk::CommandPool commandPool;
    vk::RenderPass renderPass;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;

    vk::SurfaceFormatKHR surfaceFormat;
    vk::Extent2D extent;

    e172vp::Hardware::QueueFamilies queueFamilies;

    std::vector<vk::Image> swapChainImages;
    std::vector<vk::ImageView> swapChainImageViews;
    std::vector<vk::Framebuffer> swapChainFramebuffers;
    std::vector<vk::CommandBuffer> commandBuffers;
    vk::Semaphore imageAvailableSemaphore;
    vk::Semaphore renderFinishedSemaphore;

    const std::vector<std::string> instanceExtensions = {};

    const std::vector<std::string> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    e172::ElapsedTimer elapsedFromStart;
    e172::ElapsedTimer updateCommandBuffersTimer;
public:
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
    struct CommandReciept {
        bool is_valid = false;
        float x = 0;
        float y = 0;
    };


    VulkanInstance(GLFWwindow *window);
    void createSurface(GLFWwindow *window);

    void paint();

    static std::vector<std::string> glfwExtensions();

    static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    static vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

    static bool createSwapChain(const vk::PhysicalDevice &physicalDevice, const vk::Device &logicalDevice, const vk::SurfaceKHR &surface, const e172vp::Hardware::QueueFamilies &queueFamilies, vk::SurfaceFormatKHR *surfaceFormat, vk::Extent2D *extent, vk::SwapchainKHR *swapChain, std::queue<std::string> *error_queue);
    static bool createImageViewes(const vk::Device &logicalDevice, const std::vector<vk::Image> &swapChainImages, const vk::Format &swapChainImageFormat, std::vector<vk::ImageView> *swapChainImageViews, std::queue<std::string> *error_queue);
    static bool createRenderPass(const vk::Device &logicDevice, const vk::Format &swapChainImageFormat, vk::RenderPass *renderPass, std::queue<std::string> *error_queue);
    static bool createFrameBuffers(const vk::Device &logicalDevice, const vk::Extent2D &extent, const vk::RenderPass &renderPass, const std::vector<vk::ImageView> &swapChainImageViews, std::vector<vk::Framebuffer> *swapChainFramebuffers, std::queue<std::string> *error_queue);
    static bool createCommandPool(const vk::Device &logicDevice, const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface, vk::CommandPool *commandPool, std::queue<std::string> *error_queue);
    static void createSyncObjects(const vk::Device &logicalDevice, vk::Semaphore *imageAvailableSemaphore, vk::Semaphore *renderFinishedSemaphore);
    static bool createCommandBuffers(const vk::Device &logicalDevice, uint32_t count, const vk::CommandPool &commandPool, std::vector<vk::CommandBuffer> *commandBuffers, std::queue<std::string> *error_queue);
    static void proceedCommandBuffers(vk::RenderPass renderPass, vk::Pipeline pipeline, vk::Extent2D extent, std::vector<vk::Framebuffer> swapChainFramebuffers, std::vector<vk::CommandBuffer> commandBuffers);
    static VkShaderModule createShaderModule(VkDevice logicalDevice, const std::vector<char> &code);
    static void resetCommandBuffers(std::vector<VkCommandBuffer> commandBuffers, VkQueue, VkQueue presentQueue);

    static void createGraphicsPipeline(const vk::Device &logicDevice, const vk::Extent2D &extent, const vk::RenderPass &renderPass, vk::PipelineLayout *pipelineLayout, vk::Pipeline *graphicsPipline);
    static std::vector<char> readFile(const std::string& filename);


    ~VulkanInstance();
};

#endif // INITIALIZER_H

#ifndef GRAPHICSINSTANCE_H
#define GRAPHICSINSTANCE_H

#include "graphicsinstancecreateinfo.h"
#include "hardware.h"

#include <vulkan/vulkan.hpp>
#include <queue>
#include <map>

namespace e172vp {



class GraphicsInstance {
    VkDebugReportCallbackEXT m_debugReportCallbackObject;

    vk::Instance m_vulkanInstance;
    vk::SurfaceKHR m_surface;
    vk::PhysicalDevice m_physicalDevice;
    vk::Device m_logicalDevice;
    e172vp::Hardware::QueueFamilies m_queueFamilies;
    vk::Queue m_graphicsQueue;
    vk::Queue m_presentQueue;

    vk::SurfaceFormatKHR m_surfaceFormat;
    vk::Extent2D m_extent;
    vk::SwapchainKHR m_swapChain;

    vk::CommandPool m_commandPool;

    vk::RenderPass m_renderPass;

    std::vector<vk::Image> m_swapChainImages;
    std::vector<vk::ImageView> m_swapChainImageViews;
    std::vector<vk::Framebuffer> m_swapChainFramebuffers;
    std::vector<vk::CommandBuffer> m_commandBuffers;


    std::vector<std::string> m_enabledValidationLayers;

    bool m_isValid = false;
    std::queue<std::string> m_errors;

    static void initDebug(vk::Instance instance, VkDebugReportCallbackEXT *c, std::queue<std::string> *error_queue);

public:
    struct LogicDeviceCreationResult {
        bool is_valid = false;
        vk::Device logicalDevice;
        e172vp::Hardware::QueueFamilies queueFamilies;
        std::vector<std::string> enabledValidationLayers;
    };
private:
    static LogicDeviceCreationResult createLogicalDevice(const vk::PhysicalDevice &physicalDevice,
            const vk::SurfaceKHR &surface,
            const std::vector<std::string> &requiredDeviceExtensions,
            bool validationLayersEnabled,
            std::queue<std::string> *error_queue
            );

    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    static VkExtent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
    static bool createSwapChain(const vk::PhysicalDevice &physicalDevice, const vk::Device &logicalDevice, const vk::SurfaceKHR &surface, const Hardware::QueueFamilies &queueFamilies, vk::SurfaceFormatKHR *surfaceFormat, vk::Extent2D *extent, vk::SwapchainKHR *swapChain, std::queue<std::string> *error_queue);

    static void createImages(vk::Device logicDevice, vk::SwapchainKHR swapChain, std::vector<vk::Image> *swapChainImages);
    static bool createImageViewes(vk::Device logicDevice, const std::vector<vk::Image> &swapChainImages, vk::Format swapChainImageFormat, std::vector<vk::ImageView> *swapChainImageViews, std::queue<std::string> *error_queue);
    static void createRenderPass(vk::Device logicDevice, vk::Format swapChainImageFormat, vk::RenderPass *renderPass, std::queue<std::string> *error_queue);
    static void createFrameBuffers(vk::Device logicDevice, vk::Extent2D extent, vk::RenderPass renderPass, const std::vector<vk::ImageView> &swapChainImageViews, std::vector<vk::Framebuffer> *swapChainFramebuffers, std::queue<std::string> *error_queue);
    static void createCommandPool(vk::Device logicDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, vk::CommandPool *commandPool, std::queue<std::string> *error_queue);
    static void createCommandBuffers(vk::Device logicDevice, uint32_t count, vk::CommandPool commandPool, std::vector<vk::CommandBuffer> *commandBuffers, std::queue<std::string> *error_queue);

public:


    GraphicsInstance(const GraphicsInstanceCreateInfo &createInfo);
    static std::vector<std::string> presentValidationLayers();


    vk::Instance vulkanInstance() const;
    vk::PhysicalDevice physicalDevice() const;
    vk::Device logicalDevice() const;
    vk::SurfaceKHR surface() const;
    bool isValid() const;

    bool hasErrors() const;
    std::string nextError();
    e172vp::Hardware::QueueFamilies queueFamilies() const;
    std::vector<std::string> enabledValidationLayers() const;
    vk::Queue graphicsQueue() const;
    vk::Queue presentQueue() const;
    vk::SurfaceFormatKHR surfaceFormat() const;
    vk::Extent2D extent() const;
    vk::SwapchainKHR swapChain() const;
    vk::CommandPool commandPool() const;
    vk::RenderPass renderPass() const;
    vk::PipelineLayout pipelineLayout() const;
    vk::Pipeline graphicsPipeline() const;
    std::vector<vk::Image> swapChainImages() const;
    std::vector<vk::ImageView> swapChainImageViews() const;
    std::vector<vk::Framebuffer> swapChainFramebuffers() const;
    std::vector<vk::CommandBuffer> commandBuffers() const;
};

}
#endif // GRAPHICSINSTANCE_H

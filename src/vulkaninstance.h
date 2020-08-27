#ifndef VULKANINSTANCE_H
#define VULKANINSTANCE_H

#include "queuetools.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <GLFW/glfw3.h>
#include <string>
#include "time/elapsedtimer.h"

std::ostream &operator<<(std::ostream& stream, const std::vector<std::string>& value);



class VulkanInstance {
    friend class WindowInstance;

    VkInstance vkInstance = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicDevice = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkDebugReportCallbackEXT debugReportCallbackEXT = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline = VK_NULL_HANDLE;

    VkSurfaceFormatKHR surfaceFormat;
    VkExtent2D extent;

    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    std::vector<VkCommandBuffer> commandBuffers;
    VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;

    const std::vector<std::string> instanceExtensions = {
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME
    };

    const std::vector<std::string> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    const std::vector<std::string> validationLayers = {
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_LUNARG_gfxreconstruct",
        "VK_LAYER_LUNARG_device_simulation",
        "VK_LAYER_LUNARG_api_dump",
        "VK_LAYER_LUNARG_monitor",
        "VK_LAYER_LUNARG_screenshot"
    };

    e172::ElapsedTimer elapsedFromStart;
    e172::ElapsedTimer updateCommandBuffersTimer;


    static VKAPI_ATTR VkBool32 VKAPI_CALL debugVulkanCallback(
                    VkDebugReportFlagsEXT flags,
                    VkDebugReportObjectTypeEXT objType,
                    uint64_t obj,
                    size_t location,
                    int32_t messageCode,
                    const char* pLayerPrefix,
                    const char* msg,
                    void* userData);
public:
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };


    VulkanInstance(GLFWwindow *window);
    void createSurface(GLFWwindow *window);


    void paint();

    static std::vector<std::string> availableValidationLayers();
    static void initDebug(VkInstance instance, PFN_vkDebugReportCallbackEXT callback, VkDebugReportCallbackEXT *c);
    static bool checkValidationLayerSupport(const std::vector<std::string> &validationLayers, std::string *missing = nullptr);
    static VkPhysicalDevice findPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, const std::vector<std::string> &requiredDeviceExtensions);
    static QueueTools::QueueFamilyIndices createLogicalDevice(VkDevice *device, VkPhysicalDevice d, VkSurfaceKHR surface, const std::vector<std::string> &requiredDeviceExtensions, const std::vector<std::string> &requiredValidationLayers);
    static bool checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<std::string> &requeredExtensions);
    static std::vector<std::string> glfwExtensions();
    static std::vector<std::string> mergeExtensions(const std::vector<std::string> &e1, const std::vector<std::string> &e2);
    static void createInstance(VkInstance *instance, VkApplicationInfo *applicationInfo, const std::vector<std::string> &requiredExtensions);
    static std::vector<VkPhysicalDevice> physicalDevices(VkInstance instance);
    static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<std::string> &requiredDeviceExtensions);
    static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    static void createSwapChain(VkPhysicalDevice physicalDevice, VkDevice logicDevice, VkSurfaceKHR surface, QueueTools::QueueFamilyIndices queueFamilies, VkSurfaceFormatKHR *surfaceFormat, VkExtent2D *extent, VkSwapchainKHR *swapChain);
    static void createImages(VkDevice logicDevice, VkSwapchainKHR swapChain, std::vector<VkImage> *swapChainImages);
    static void createImageViewes(VkDevice logicDevice, const std::vector<VkImage> &swapChainImages, VkFormat swapChainImageFormat, std::vector<VkImageView> *swapChainImageViews);
    static void createRenderPass(VkDevice logicDevice, VkFormat swapChainImageFormat, VkRenderPass *renderPass);
    static void createFrameBuffers(VkDevice logicDevice, VkExtent2D extent, VkRenderPass renderPass, const std::vector<VkImageView> &swapChainImageViews, std::vector<VkFramebuffer> *swapChainFramebuffers);
    static void createCommandPool(VkDevice logicDevice, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkCommandPool *commandPool);
    static void createSyncObjects(VkDevice logicDevice, VkSemaphore *imageAvailableSemaphore, VkSemaphore *renderFinishedSemaphore);
    static VkShaderModule createShaderModule(VkDevice logicDevice, const std::vector<char> &code);

    static void createCommandBuffers(VkDevice logicDevice, uint32_t count, VkCommandPool commandPool, std::vector<VkCommandBuffer> *commandBuffers);
    static void proceedCommandBuffers(VkRenderPass renderPass, VkPipeline pipeline, VkExtent2D extent, std::vector<VkFramebuffer> swapChainFramebuffers, std::vector<VkCommandBuffer> commandBuffers);
    static void createGraphicsPipeline(VkDevice logicDevice, VkExtent2D extent, VkRenderPass renderPass, VkPipelineLayout *pipelineLayout, VkPipeline *graphicsPipline);

    static std::vector<char> readFile(const std::string& filename);


    ~VulkanInstance();
};

#endif // INITIALIZER_H

#ifndef HARDWARE_H
#define HARDWARE_H

#include <vulkan/vulkan.hpp>




namespace e172vp {

struct Hardware {
    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    struct QueueFamilies {
        std::optional<uint32_t> m_graphicsFamily;
        std::optional<uint32_t> m_presentFamily;
        uint32_t graphicsFamily() const { return m_graphicsFamily.value(); }
        uint32_t presentFamily() const { return m_presentFamily.value(); }
        bool isValid() const { return m_graphicsFamily.has_value() && m_presentFamily.has_value(); }
    };

    static bool isDeviceSuitable(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, const std::vector<std::string> &requiredDeviceExtensions);
    static SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice &physicalDevice, vk::SurfaceKHR surface);
    static vk::PhysicalDevice findSuitablePhysicalDevice(const vk::Instance &instance, vk::SurfaceKHR surface, const std::vector<std::string> &requiredDeviceExtensions);
    static QueueFamilies queryQueueFamilies(const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface);

    static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    static vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, vk::Extent2D defaultExtent = vk::Extent2D());

};

}
#endif // HARDWARE_H

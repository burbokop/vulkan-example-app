#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.hpp>
#include "tools/hardware.h"

namespace e172vp {

class SwapChain {
    vk::SwapchainKHR m_swapChainHandle;
    std::vector<vk::Image> m_images;
    std::vector<vk::ImageView> m_imageViewes;

    bool m_isValid = false;
    std::vector<std::string> m_errors;
public:
    struct Settings {
        vk::SurfaceFormatKHR surfaceFormat;
        vk::PresentModeKHR presentMode;
        vk::Extent2D extent;
        e172vp::Hardware::SwapChainSupportDetails supportDetails;
    };
private:
    Settings m_settings;
public:

    static vk::SurfaceFormatKHR chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
    static vk::PresentModeKHR choosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);
    static vk::Extent2D chooseExtent(const vk::SurfaceCapabilitiesKHR &capabilities, const vk::Extent2D &defaultExtent = vk::Extent2D());
    static Settings createSettings(const e172vp::Hardware::SwapChainSupportDetails &supportDetails, const vk::Extent2D &defaultExtent = vk::Extent2D());

    static bool createImageViewes(const vk::Device &logicDevice, const std::vector<vk::Image> &swapChainImages, const vk::Format &swapChainImageFormat, std::vector<vk::ImageView> *swapChainImageViews, std::vector<std::string> *error_queue);

    static vk::ImageView createImageView(const vk::Device &logicalDevice, vk::Image image, vk::Format format, std::vector<std::string> *error_queue = nullptr);




    SwapChain();
    SwapChain(const vk::Device &logicalDevice, const vk::SurfaceKHR &surface, const Hardware::QueueFamilies &queueFamilies, const Settings &settings);
    vk::SwapchainKHR swapChainHandle() const;
    operator vk::SwapchainKHR () const;

    std::vector<std::string> pullErrors();
    bool isValid() const;

    vk::Image image(size_t index) const;

    size_t imageCount() const;
    vk::ImageView imageView(size_t index) const;
    size_t imageViewCount() const;
    std::vector<vk::Image> imageVector() const;
    std::vector<vk::ImageView> imageViewVector() const;
    Settings settings() const;
};

}
#endif // SWAPCHAIN_H

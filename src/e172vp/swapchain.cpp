#include "swapchain.h"


vk::SwapchainKHR e172vp::SwapChain::swapChainHandle() const {
    return m_swapChainHandle;
}

e172vp::SwapChain::operator vk::SwapchainKHR() const {
    return m_swapChainHandle;
}

std::vector<std::string> e172vp::SwapChain::pullErrors() {
    const auto r = m_errors;
    m_errors.clear();
    return r;
}

bool e172vp::SwapChain::isValid() const {
    return m_isValid;
}

vk::Image e172vp::SwapChain::image(size_t index) const {
    if(index < m_images.size())
        return m_images[index];
    return vk::Image();
}

size_t e172vp::SwapChain::imageCount() const {
    return m_images.size();
}

vk::ImageView e172vp::SwapChain::imageView(size_t index) const {
    if(index < m_imageViewes.size())
        return m_imageViewes[index];
    return vk::ImageView();
}

size_t e172vp::SwapChain::imageViewCount() const {
    return m_imageViewes.size();
}

std::vector<vk::Image> e172vp::SwapChain::imageVector() const {
    return m_images;
}

std::vector<vk::ImageView> e172vp::SwapChain::imageViewVector() const {
    return m_imageViewes;
}

e172vp::SwapChain::Settings e172vp::SwapChain::settings() const {
    return m_settings;
}

vk::SurfaceFormatKHR e172vp::SwapChain::chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

vk::PresentModeKHR e172vp::SwapChain::choosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D e172vp::SwapChain::chooseExtent(const vk::SurfaceCapabilitiesKHR &capabilities, const vk::Extent2D &defaultExtent) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = defaultExtent;

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

e172vp::SwapChain::Settings e172vp::SwapChain::createSettings(const e172vp::Hardware::SwapChainSupportDetails &supportDetails, const vk::Extent2D &defaultExtent) {
    Settings result;
    result.surfaceFormat = chooseSurfaceFormat(supportDetails.formats);
    result.presentMode = choosePresentMode(supportDetails.presentModes);
    result.extent = chooseExtent(supportDetails.capabilities, defaultExtent);
    result.supportDetails = supportDetails;
    return result;
}

bool e172vp::SwapChain::createImageViewes(const vk::Device &logicDevice, const std::vector<vk::Image> &swapChainImages, const vk::Format &swapChainImageFormat, std::vector<vk::ImageView> *swapChainImageViews, std::vector<std::string> *error_queue) {
    swapChainImageViews->resize(swapChainImages.size());

    for (uint32_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews->operator[](i) = createImageView(logicDevice, swapChainImages[i], swapChainImageFormat, error_queue);
        if(!swapChainImageViews->operator[](i))
            return false;
    }
    return true;
}

vk::ImageView e172vp::SwapChain::createImageView(const vk::Device &logicalDevice, vk::Image image, vk::Format format, std::vector<std::string> *error_queue) {
    vk::ImageViewCreateInfo viewInfo;
    viewInfo.image = image;
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    vk::ImageView imageView;
    if (logicalDevice.createImageView(&viewInfo, nullptr, &imageView) != vk::Result::eSuccess) {
        if(error_queue)
            error_queue->push_back("failed to create texture image view");
    }

    return imageView;
}

e172vp::SwapChain::SwapChain() {

}

e172vp::SwapChain::SwapChain(const vk::Device &logicalDevice, const vk::SurfaceKHR &surface, const Hardware::QueueFamilies &queueFamilies, const Settings &settings) {
    uint32_t imageCount = 3;
    if (imageCount > settings.supportDetails.capabilities.maxImageCount) {
        imageCount = settings.supportDetails.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = settings.surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = settings.surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = settings.extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
    swapchainCreateInfo.setPNext(nullptr);

    uint32_t queueFamilyIndices[] = { queueFamilies.graphicsFamily(), queueFamilies.presentFamily()};

    if (queueFamilies.graphicsFamily() != queueFamilies.presentFamily()) {
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = {};
    }

    swapchainCreateInfo.preTransform = settings.supportDetails.capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapchainCreateInfo.presentMode = settings.presentMode;
    swapchainCreateInfo.clipped = true;


    m_swapChainHandle = logicalDevice.createSwapchainKHR(swapchainCreateInfo);
    m_isValid = m_swapChainHandle;
    if(m_isValid) {
        m_images = logicalDevice.getSwapchainImagesKHR(m_swapChainHandle);
        m_isValid = createImageViewes(logicalDevice, m_images, settings.surfaceFormat.format, &m_imageViewes, &m_errors);
        if(m_isValid)
            m_settings = settings;
    }
}

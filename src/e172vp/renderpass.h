#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vulkan/vulkan.hpp>
#include "swapchain.h"

namespace e172vp {


class RenderPass {
    vk::RenderPass m_renderPathHandle;
    std::vector<vk::Framebuffer> m_frameBuffers;
    std::vector<std::string> m_errors;
    bool m_isValid = false;
public:
    static bool createFrameBuffers(const vk::Device &logicalDevice, const vk::Extent2D &extent, const vk::RenderPass &renderPass, const std::vector<vk::ImageView> &swapChainImageViews, std::vector<vk::Framebuffer> *swapChainFramebuffers, std::vector<std::string> *error_queue);

    RenderPass() {}
    RenderPass(const vk::Device &logicalDevice, const SwapChain &swapchain);
    vk::RenderPass renderPathHandle() const;
    operator vk::RenderPass() const;
    operator VkRenderPass() const;
    std::vector<std::string> pullErrors();
    bool isValid() const;

    std::vector<vk::Framebuffer> frameBufferVector() const;
    vk::Framebuffer frameBuffer(size_t index) const;
    size_t frameBufferCount() const;
};

}
#endif // RENDERPASS_H

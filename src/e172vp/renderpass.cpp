#include "renderpass.h"

vk::RenderPass e172vp::RenderPass::renderPathHandle() const { return m_renderPathHandle; }

e172vp::RenderPass::operator VkRenderPass() const {
    return m_renderPathHandle;
}

bool e172vp::RenderPass::isValid() const { return m_isValid; }

std::vector<vk::Framebuffer> e172vp::RenderPass::frameBufferVector() const {
    return m_frameBuffers;
}

vk::Framebuffer e172vp::RenderPass::frameBuffer(size_t index) const {
    if(index < m_frameBuffers.size())
        return m_frameBuffers[index];
    return vk::Framebuffer();
}

size_t e172vp::RenderPass::frameBufferCount() const {
    return m_frameBuffers.size();
}

bool e172vp::RenderPass::createFrameBuffers(const vk::Device &logicalDevice, const vk::Extent2D &extent, const vk::RenderPass &renderPass, const std::vector<vk::ImageView> &swapChainImageViews, std::vector<vk::Framebuffer> *swapChainFramebuffers, std::vector<std::string> *error_queue) {
    swapChainFramebuffers->resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        vk::ImageView attachments[] = {
            swapChainImageViews[i]
        };

        vk::FramebufferCreateInfo framebufferInfo;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        const auto code = logicalDevice.createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers->operator[](i));
        if (code != vk::Result::eSuccess) {
            error_queue->push_back("[error] Failed to create framebuffer: " + vk::to_string(code));
            return false;
        }
    }
    return true;
}

e172vp::RenderPass::RenderPass(const vk::Device &logicalDevice, const SwapChain &swapchain) {
    vk::AttachmentDescription colorAttachment;
    colorAttachment.format = swapchain.settings().surfaceFormat.format;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentReference colorAttachmentRef;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    const auto code = logicalDevice.createRenderPass(&renderPassInfo, nullptr, &m_renderPathHandle);
    if (code != vk::Result::eSuccess) {
        m_errors.push_back("[error] Failed to create render pass: " + vk::to_string(code));
        return;
    }

    if(!createFrameBuffers(
                logicalDevice,
                swapchain.settings().extent,
                m_renderPathHandle,
                swapchain.imageViewVector(),
                &m_frameBuffers,
                &m_errors
                )) {
        return;
    }

    m_isValid = true;
}

std::vector<std::string> e172vp::RenderPass::pullErrors() {
    const auto r = m_errors;
    m_errors.clear();
    return r;
}

e172vp::RenderPass::operator vk::RenderPass() const {
    return m_renderPathHandle;
}

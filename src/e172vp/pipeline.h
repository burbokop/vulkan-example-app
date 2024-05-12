#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.hpp>

namespace e172vp {

class Pipeline final {
public:
    Pipeline(
        const vk::Device& logicalDevice,
        const vk::Extent2D& extent,
        const vk::RenderPass& renderPass,
        const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts,
        const std::vector<std::uint8_t>& vertexShader,
        const std::vector<std::uint8_t>& fragmentShader,
        vk::PrimitiveTopology topology);

    Pipeline() = delete;
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline(Pipeline&&) = delete;
    Pipeline& operator=(Pipeline&&) = delete;

    vk::Pipeline handle() const;
    vk::PipelineLayout pipelineLayout() const;
    vk::Device logicalDevice() const;

    ~Pipeline();

private:
    static vk::ShaderModule createShaderModule(const vk::Device& logicDevice, const std::vector<std::uint8_t>& code);

private:
    vk::Pipeline m_handle;
    vk::PipelineLayout m_pipelineLayout;
    vk::Device m_logicalDevice;
};
}

#endif // PIPELINE_H

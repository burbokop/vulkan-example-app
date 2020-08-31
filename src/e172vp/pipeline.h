#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.hpp>

namespace e172vp {

class Pipeline {
    vk::Pipeline m_handle;
    vk::PipelineLayout m_pipelineLayout;
    vk::Device m_logicalDevice;
public:
    static vk::ShaderModule createShaderModule(const vk::Device &logicDevice, const std::vector<char> &code);

    Pipeline(const vk::Device &logicalDevice, const vk::Extent2D &extent, const vk::RenderPass &renderPass, const std::vector<vk::DescriptorSetLayout> &descriptorSetLayouts, const std::vector<char> &vertexShader, const std::vector<char> &fragmentShader, vk::PrimitiveTopology topology);

    Pipeline() = default;
    Pipeline(const Pipeline &) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    vk::Pipeline handle() const;
    vk::PipelineLayout pipelineLayout() const;
    ~Pipeline();
    vk::Device logicalDevice() const;
};

}

#endif // PIPELINE_H

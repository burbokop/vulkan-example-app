#include "descriptorsetlayout.h"

bool e172vp::DescriptorSetLayout::isValid() const {
    return m_isValid;
}

e172vp::DescriptorSetLayout e172vp::DescriptorSetLayout::createUniformDSL(const vk::Device &logicalDevice, uint32_t binding) {
    e172vp::DescriptorSetLayout result;
    if(logicalDevice) {

        vk::DescriptorSetLayoutBinding uboLayoutBinding;
        uboLayoutBinding.binding = binding;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

        vk::DescriptorSetLayoutCreateInfo layoutInfo;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (logicalDevice.createDescriptorSetLayout(&layoutInfo, nullptr, &result.m_descriptorSetLayoutHandle) != vk::Result::eSuccess) {
            throw std::runtime_error("[error] e172vp::DescriptorSetLayout: failed to create descriptor set layout!");
        }
        result.m_logicalDevice = logicalDevice;
        result.m_binding = binding;
        result.m_isValid = true;
    }
    return result;
}

e172vp::DescriptorSetLayout e172vp::DescriptorSetLayout::createSamplerDSL(const vk::Device &logicalDevice, uint32_t binding) {
    e172vp::DescriptorSetLayout result;
    if(logicalDevice) {

        vk::DescriptorSetLayoutBinding layoutBinding;
        layoutBinding.binding = binding;
        layoutBinding.descriptorCount = 1;
        layoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        layoutBinding.pImmutableSamplers = nullptr;
        layoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

        vk::DescriptorSetLayoutCreateInfo layoutInfo;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &layoutBinding;

        if (logicalDevice.createDescriptorSetLayout(&layoutInfo, nullptr, &result.m_descriptorSetLayoutHandle) != vk::Result::eSuccess) {
            throw std::runtime_error("[error] e172vp::DescriptorSetLayout: failed to create descriptor set layout!");
        }
        result.m_logicalDevice = logicalDevice;
        result.m_binding = binding;
        result.m_isValid = true;
    }
    return result;
}

void e172vp::DescriptorSetLayout::destroy(const e172vp::DescriptorSetLayout &descriptorSetLayout) {
    if(descriptorSetLayout.m_logicalDevice && descriptorSetLayout.m_descriptorSetLayoutHandle) {
        descriptorSetLayout.m_logicalDevice.destroyDescriptorSetLayout(descriptorSetLayout.m_descriptorSetLayoutHandle);
    }
}

uint32_t e172vp::DescriptorSetLayout::binding() const {
    return m_binding;
}

vk::DescriptorSetLayout e172vp::DescriptorSetLayout::descriptorSetLayoutHandle() const {
    return m_descriptorSetLayoutHandle;
}


vk::Device e172vp::DescriptorSetLayout::logicalDevice() const {
    return m_logicalDevice;
}

#ifndef DESCRIPTORSETLAYOUT_H
#define DESCRIPTORSETLAYOUT_H

#include <vulkan/vulkan.hpp>

namespace e172vp {

class DescriptorSetLayout {
    vk::DescriptorSetLayout m_descriptorSetLayoutHandle;
    vk::Device m_logicalDevice;
    uint32_t m_binding = -1;
    bool m_isValid = false;
public:
    static DescriptorSetLayout createUniformDSL(const vk::Device &logicalDevice, uint32_t binding);
    static DescriptorSetLayout createSamplerDSL(const vk::Device &logicalDevice, uint32_t binding);
    static void destroy(const DescriptorSetLayout &descriptorSetLayout);

    DescriptorSetLayout() {}

    uint32_t binding() const;
    vk::DescriptorSetLayout descriptorSetLayoutHandle() const;
    vk::Device logicalDevice() const;
    bool isValid() const;
};

}
#endif // DESCRIPTORSETLAYOUT_H

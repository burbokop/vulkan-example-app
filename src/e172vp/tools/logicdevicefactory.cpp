#include "logicdevicefactory.h"

#include "stringvector.h"
#include "validation.h"

std::vector<std::string> e172vp::LogicDeviceFactory::errors() const { return m_errors; }
std::vector<std::string> e172vp::LogicDeviceFactory::requiredDeviceExtensions() const { return m_requiredDeviceExtensions; }
void e172vp::LogicDeviceFactory::setRequiredDeviceExtensions(const std::vector<std::string> &requiredDeviceExtensions) { m_requiredDeviceExtensions = requiredDeviceExtensions; }
bool e172vp::LogicDeviceFactory::validationLayersEnabled() const { return m_validationLayersEnabled; }
void e172vp::LogicDeviceFactory::setValidationLayersEnabled(bool validationLayersEnabled) { m_validationLayersEnabled = validationLayersEnabled; }
std::vector<std::string> e172vp::LogicDeviceFactory::enabledValidationLayers() const { return m_enabledValidationLayers; }
e172vp::Hardware::QueueFamilies e172vp::LogicDeviceFactory::queueFamilies() const { return m_queueFamilies; }
void e172vp::LogicDeviceFactory::setQueueFamilies(const Hardware::QueueFamilies &queueFamilies) { m_queueFamilies = queueFamilies; }

e172vp::LogicDeviceFactory::LogicDeviceFactory() {}

vk::Device e172vp::LogicDeviceFactory::create(const vk::PhysicalDevice &physicalDevice) {
    if(!m_queueFamilies.isValid()) {
        m_errors.push_back("[error] e172vp::LogicDeviceFactory: Missing graphics or presentation family on any GPU.");
        return vk::Device();
    }

    const float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.setQueueFamilyIndex(m_queueFamilies.graphicsFamily());
    queueCreateInfo.setQueueCount(1);
    queueCreateInfo.setPQueuePriorities(&queuePriority);

    vk::PhysicalDeviceFeatures deviceFeatures;

    std::vector<const char*> __rme;
    StringVector::fillCStrContainer(m_requiredDeviceExtensions, __rme);

    vk::DeviceCreateInfo createInfo;
    createInfo.setQueueCreateInfoCount(1);
    createInfo.setPQueueCreateInfos(&queueCreateInfo);
    createInfo.setPEnabledFeatures(&deviceFeatures);
    createInfo.setPEnabledExtensionNames(__rme);

    if(m_validationLayersEnabled) {
        m_enabledValidationLayers = Validation::presentLayers();
        std::vector<const char*> vl_buffer;
        StringVector::fillCStrContainer(m_enabledValidationLayers, vl_buffer);
        createInfo.enabledLayerCount = vl_buffer.size();
        createInfo.ppEnabledLayerNames = vl_buffer.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    vk::Device result_device;
    const auto code = physicalDevice.createDevice(&createInfo, nullptr, &result_device);
    if (code != vk::Result::eSuccess) {
        m_errors.push_back("[error] e172vp::LogicDeviceFactory: Failed to create logical device: " + vk::to_string(code));
        return vk::Device();
    }

    return result_device;
}


#ifndef LOGICDEVICEFACTORY_H
#define LOGICDEVICEFACTORY_H

#include <vulkan/vulkan.hpp>
#include "hardware.h"

namespace e172vp {



class LogicDeviceFactory {
    Hardware::QueueFamilies m_queueFamilies;
    std::vector<std::string> m_requiredDeviceExtensions;
    bool m_validationLayersEnabled = false;

    std::vector<std::string> m_errors;
    std::vector<std::string> m_enabledValidationLayers;

public:
    LogicDeviceFactory();
    vk::Device create(const vk::PhysicalDevice &physicalDevice);
    Hardware::QueueFamilies queueFamilies() const;
    void setQueueFamilies(const Hardware::QueueFamilies &queueFamilies);
    std::vector<std::string> errors() const;
    std::vector<std::string> requiredDeviceExtensions() const;
    void setRequiredDeviceExtensions(const std::vector<std::string> &requiredDeviceExtensions);
    bool validationLayersEnabled() const;
    void setValidationLayersEnabled(bool validationLayersEnabled);
    std::vector<std::string> enabledValidationLayers() const;
};
}
#endif // LOGICDEVICEFACTORY_H

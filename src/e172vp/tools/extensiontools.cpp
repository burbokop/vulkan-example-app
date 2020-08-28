#include "extensiontools.h"




std::vector<std::string> e172vp::Extension::presentInstanceExtensions() {
    uint32_t presentExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &presentExtensionCount, nullptr);
    std::vector<VkExtensionProperties> presentExtensions(presentExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &presentExtensionCount, presentExtensions.data());

    std::vector<std::string> result(presentExtensionCount);
    for(size_t i = 0; i < presentExtensionCount; ++i) {
        result[i] = presentExtensions[i].extensionName;
    }
    return result;
}





std::vector<std::string> e172vp::Extension::presentDeviceExtensions(vk::PhysicalDevice physicalDevice) {
    uint32_t presentExtensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &presentExtensionCount, nullptr);

    std::vector<VkExtensionProperties> presentExtensions(presentExtensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &presentExtensionCount, presentExtensions.data());

    std::vector<std::string> result(presentExtensionCount);
    for(size_t i = 0; i < presentExtensionCount; ++i) {
        result[i] = presentExtensions[i].extensionName;
    }
    return result;
}

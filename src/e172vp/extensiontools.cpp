#include "extensiontools.h"


#include <algorithm>
#include <set>
#include <sstream>

std::vector<std::string> e172vp::mergeExtensions(const std::vector<std::string> &extension0, const std::vector<std::string> &extension1) {
    std::vector<std::string> result = extension0;
    for(auto e : extension1) {
        if(std::find(result.begin(), result.end(), e) == result.end()) {
            result.push_back(e);
        }
    }
    return result;
}

std::vector<std::string> e172vp::missingExtensions(const std::vector<std::string> &presentExtensions, const std::vector<std::string> &requiredExtensions) {
    std::set<std::string> __requiredExtensions(requiredExtensions.begin(), requiredExtensions.end());
    for (const auto& extension : presentExtensions) {
        __requiredExtensions.erase(extension);
    }
    return std::vector<std::string>(__requiredExtensions.begin(), __requiredExtensions.end());
}

std::vector<std::string> e172vp::presentExtensions() {
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

vk::ArrayProxyNoTemporaries<const char *const> e172vp::extensionsToVkArray(const std::vector<std::string> &ext) {
    std::vector<const char*> f(ext.size());
    for(size_t i = 0; i < ext.size(); ++i) {
        f[i] = ext[i].c_str();
    }
    return f;
}

std::string e172vp::extensionsToString(const std::vector<std::string> &ext) {
    std::stringstream stream;

    size_t i = 0;
    stream << std::string("[");
    for(auto a : ext) {
        stream << a << ((i == ext.size() - 1) ? std::string() : ", ");
        ++i;
    }
    stream << std::string("]");
    return stream.str();
}



std::vector<std::string> e172vp::devicePresentExtensions(vk::PhysicalDevice physicalDevice) {
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

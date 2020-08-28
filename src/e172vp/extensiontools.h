#ifndef EXTENSIONTOOLS_H
#define EXTENSIONTOOLS_H

#include <vector>
#include <string>
#include <vulkan/vulkan.hpp>

namespace e172vp {
    std::vector<std::string> mergeExtensions(const std::vector<std::string> &extension0, const std::vector<std::string> &extension1);
    std::vector<std::string> missingExtensions(const std::vector<std::string> &presentExtensions, const std::vector<std::string> &requiredExtensions);
    std::vector<std::string> presentExtensions();

    std::vector<std::string> devicePresentExtensions(vk::PhysicalDevice physicalDevice);


    vk::ArrayProxyNoTemporaries<const char * const> extensionsToVkArray(const std::vector<std::string> &ext);

    template<typename T>
    void extensionsFillContainer(const std::vector<std::string> &ext, T& container) {
        container.resize(ext.size());
        for(size_t i = 0; i < ext.size(); ++i) {
            container[i] = ext[i].c_str();
        }
    }
    std::string to_string(const std::vector<std::string> &vector);
}
#endif // EXTENSIONTOOLS_H

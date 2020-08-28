#ifndef EXTENSIONTOOLS_H
#define EXTENSIONTOOLS_H

#include <vector>
#include <string>
#include <vulkan/vulkan.hpp>

namespace e172vp {

struct Extension {
    static std::vector<std::string> presentInstanceExtensions();
    static std::vector<std::string> presentDeviceExtensions(vk::PhysicalDevice physicalDevice);
};

}
#endif // EXTENSIONTOOLS_H

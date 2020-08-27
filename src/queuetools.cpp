#include "queuetools.h"
#include <vector>

QueueTools::QueueFamilyIndices QueueTools::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface, bool unique) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if(presentSupport)
            indices.m_presentFamily = i;

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.m_graphicsFamily = i;

        if(indices.isValid())
            break;

        i++;
    }

    return indices;
}

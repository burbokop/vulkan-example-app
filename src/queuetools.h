#ifndef QUEUETOOLS_H
#define QUEUETOOLS_H

#include <optional>
#include <vulkan/vulkan.h>

class QueueTools {
public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> m_graphicsFamily;
        std::optional<uint32_t> m_presentFamily;
        uint32_t graphicsFamily() const { return m_graphicsFamily.value(); }
        uint32_t presentFamily() const { return m_presentFamily.value(); }
        bool isValid() const { return m_graphicsFamily.has_value() && m_presentFamily.has_value(); }
    };

    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface, bool unique = false);
};

#endif // QUEUETOOLS_H

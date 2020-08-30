#ifndef GRAPHICSINSTANCECREATEINFO_H
#define GRAPHICSINSTANCECREATEINFO_H

#include <string>
#include <vector>
#include <functional>
#include <vulkan/vulkan.hpp>

namespace e172vp {


class GraphicsObjectCreateInfo {
    std::string m_applicationName;
    uint32_t m_applicationVersion;
    std::vector<std::string> m_requiredExtensions;
    std::vector<std::string> m_requiredDeviceExtensions;
    std::function<void(vk::Instance, vk::SurfaceKHR*)> m_surfaceCreator;
    bool m_debugEnabled = false;
public:
    GraphicsObjectCreateInfo();

    const std::string applicationName() const;
    void setApplicationName(const std::string &applicationName);
    uint32_t applicationVersion() const;
    void setApplicationVersion(const uint32_t &applicationVersion);
    const std::vector<std::string> requiredExtensions() const;
    void setRequiredExtensions(const std::vector<std::string> &requiredExtensions);
    std::function<void (vk::Instance, vk::SurfaceKHR *)> surfaceCreator() const;
    void setSurfaceCreator(const std::function<void (vk::Instance, vk::SurfaceKHR *)> &surfaceCreator);
    bool debugEnabled() const;
    void setDebugEnabled(bool debugEnabled);
    std::vector<std::string> requiredDeviceExtensions() const;
    void setRequiredDeviceExtensions(const std::vector<std::string> &requiredDeviceExtensions);
};

}
#endif // GRAPHICSINSTANCECREATEINFO_H

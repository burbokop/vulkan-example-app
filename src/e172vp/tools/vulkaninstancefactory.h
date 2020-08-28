#ifndef VULKANINSTANCEFACTORY_H
#define VULKANINSTANCEFACTORY_H

#include <vulkan/vulkan.hpp>
#include <map>

namespace e172vp {

class VulkanInstanceFactory {
    std::string m_applicationName;
    uint32_t m_applicationVersion;
    static inline const std::string m_engineName = "e172vp";
    static inline const uint32_t m_engineVersion = VK_MAKE_VERSION(0, 2, 0);
    static inline const uint32_t m_apiVersion = VK_MAKE_VERSION(1, 0, 2);
    bool m_debugEnabled = false;

    //now no used
    std::string m_instanceDebugId;
    std::vector<std::string> m_requiredExtensions;

    std::vector<std::string> m_errors;

    VkDebugReportCallbackEXT m_debugReportCallbackObject;

    void initDebugReportCallback(const vk::Instance &instance, VkDebugReportCallbackEXT *callbackObject, std::vector<std::string> *error_queue);

    static inline size_t m_nextInstanceIndex = 0;
    static inline std::map<size_t, std::string> m_instanceDebugIdMap;
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(
                    VkDebugReportFlagsEXT,
                    VkDebugReportObjectTypeEXT,
                    uint64_t,
                    size_t,
                    int32_t,
                    const char*,
                    const char* message,
                    void*
            );

public:
    VulkanInstanceFactory(const std::string &applicationName = std::string(), uint32_t applicationVersion = 0);
    vk::Instance create();
    void setApplicationName(const std::string &applicationName);
    std::string applicationName() const;
    uint32_t applicationVersion() const;
    void setApplicationVersion(const uint32_t &applicationVersion);
    static std::string engineName();
    static uint32_t engineVersion();
    static uint32_t apiVersion();
    bool debugEnabled() const;
    void setDebugEnabled(bool debugEnabled);
    std::vector<std::string> requiredExtensions() const;
    void setRequiredExtensions(const std::vector<std::string> &requiredExtensions);
    std::vector<std::string> errors() const;
    VkDebugReportCallbackEXT debugReportCallbackObject() const;
    std::string instanceDebugId() const;
    void setInstanceDebugId(const std::string &instanceDebugId);
};

}

#endif // VULKANINSTANCEFACTORY_H

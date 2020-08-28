#include "vulkaninstancefactory.h"

#include "stringvector.h"
#include "extensiontools.h"
#include <iostream>

void e172vp::VulkanInstanceFactory::setApplicationName(const std::string &applicationName) { m_applicationName = applicationName; }
std::string e172vp::VulkanInstanceFactory::applicationName() const { return m_applicationName; }
uint32_t e172vp::VulkanInstanceFactory::applicationVersion() const { return m_applicationVersion; }
void e172vp::VulkanInstanceFactory::setApplicationVersion(const uint32_t &applicationVersion) { m_applicationVersion = applicationVersion; }
std::string e172vp::VulkanInstanceFactory::engineName() { return m_engineName; }
uint32_t e172vp::VulkanInstanceFactory::engineVersion() { return m_engineVersion; }
uint32_t e172vp::VulkanInstanceFactory::apiVersion() { return m_apiVersion; }
bool e172vp::VulkanInstanceFactory::debugEnabled() const { return m_debugEnabled; }
void e172vp::VulkanInstanceFactory::setDebugEnabled(bool debugEnabled) { m_debugEnabled = debugEnabled; }
std::vector<std::string> e172vp::VulkanInstanceFactory::requiredExtensions() const { return m_requiredExtensions; }
void e172vp::VulkanInstanceFactory::setRequiredExtensions(const std::vector<std::string> &requiredExtensions) { m_requiredExtensions = requiredExtensions; }
std::vector<std::string> e172vp::VulkanInstanceFactory::errors() const { return m_errors; }
VkDebugReportCallbackEXT e172vp::VulkanInstanceFactory::debugReportCallbackObject() const { return m_debugReportCallbackObject; }
std::string e172vp::VulkanInstanceFactory::instanceDebugId() const { return m_instanceDebugId; }
void e172vp::VulkanInstanceFactory::setInstanceDebugId(const std::string &instanceDebugId) { m_instanceDebugId = instanceDebugId; }

void e172vp::VulkanInstanceFactory::initDebugReportCallback(const vk::Instance &instance, VkDebugReportCallbackEXT *callbackObject, std::vector<std::string> *error_queue) {
    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    createInfo.pfnCallback = VulkanInstanceFactory::debugReportCallback;
    PFN_vkCreateDebugReportCallbackEXT e =
            (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
                instance,
                "vkCreateDebugReportCallbackEXT"
                );

    if(e(instance, &createInfo, nullptr, callbackObject) != VK_SUCCESS) {
        error_queue->push_back("[warning] VulkanInstanceFactory: debug report callback setup failed.");
    }
}

VkBool32 e172vp::VulkanInstanceFactory::debugReportCallback(
        VkDebugReportFlagsEXT,
        VkDebugReportObjectTypeEXT,
        uint64_t,
        size_t,
        int32_t,
        const char *,
        const char *message,
        void *) {
    std::cerr << "VALIDATION LAYER MESSAGE: " << message;
    return VK_FALSE;
}

e172vp::VulkanInstanceFactory::VulkanInstanceFactory(const std::string &applicationName, uint32_t applicationVersion) {
    m_applicationName = applicationName;
    m_applicationVersion = applicationVersion;
}

vk::Instance e172vp::VulkanInstanceFactory::create() {
    vk::ApplicationInfo applicationInfo;
    applicationInfo.setPApplicationName(m_applicationName.c_str());
    applicationInfo.setApplicationVersion(m_applicationVersion);
    applicationInfo.setPEngineName(m_engineName.c_str());
    applicationInfo.setEngineVersion(m_engineVersion);
    applicationInfo.setApiVersion(m_apiVersion);

    std::vector<std::string> internalRequires = { VK_KHR_SURFACE_EXTENSION_NAME };
    if(m_debugEnabled)
        internalRequires.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    const auto requiredMergedExtensions = StringVector::mergePair(m_requiredExtensions, internalRequires);
    const auto missing = StringVector::subtract(requiredMergedExtensions, Extension::presentInstanceExtensions());
    if(missing.size() > 0) {
        m_errors.push_back("[error] e172vp::VulkanInstanceFactory: extensions missing: " + StringVector::toString(missing));
        return vk::Instance();
    }

    std::vector<const char*> __rme;
    StringVector::fillCStrContainer(requiredMergedExtensions, __rme);

    vk::InstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.setPApplicationInfo(&applicationInfo);
    instanceCreateInfo.setPEnabledExtensionNames(__rme);
    instanceCreateInfo.setEnabledLayerCount(0);


    vk::Instance result_instance;
    const auto creationResult = vk::createInstance(&instanceCreateInfo, nullptr, &result_instance);
    if (creationResult != vk::Result::eSuccess) {
        m_errors.push_back("[error] e172vp::VulkanInstanceFactory: failed to create instance: " + vk::to_string(creationResult));
        return vk::Instance();
    }

    m_instanceDebugIdMap[m_nextInstanceIndex++] = m_instanceDebugId;

    if(m_debugEnabled)
        initDebugReportCallback(result_instance, &m_debugReportCallbackObject, &m_errors);

    return result_instance;
}

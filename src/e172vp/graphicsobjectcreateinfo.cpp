#include "graphicsobjectcreateinfo.h"


const std::string e172vp::GraphicsObjectCreateInfo::applicationName() const {
    return m_applicationName;
}

void e172vp::GraphicsObjectCreateInfo::setApplicationName(const std::string &applicationName) {
    m_applicationName = applicationName;
}

uint32_t e172vp::GraphicsObjectCreateInfo::applicationVersion() const {
    return m_applicationVersion;
}

void e172vp::GraphicsObjectCreateInfo::setApplicationVersion(const uint32_t &applicationVersion) {
    m_applicationVersion = applicationVersion;
}

const std::vector<std::string> e172vp::GraphicsObjectCreateInfo::requiredExtensions() const {
    return m_requiredExtensions;
}

void e172vp::GraphicsObjectCreateInfo::setRequiredExtensions(const std::vector<std::string> &requiredExtensions) {
    m_requiredExtensions = requiredExtensions;
}

std::function<void (vk::Instance, vk::SurfaceKHR *)> e172vp::GraphicsObjectCreateInfo::surfaceCreator() const {
    return m_surfaceCreator;
}

void e172vp::GraphicsObjectCreateInfo::setSurfaceCreator(const std::function<void (vk::Instance, vk::SurfaceKHR *)> &surfaceCreator) {
    m_surfaceCreator = surfaceCreator;
}

bool e172vp::GraphicsObjectCreateInfo::debugEnabled() const {
    return m_debugEnabled;
}

void e172vp::GraphicsObjectCreateInfo::setDebugEnabled(bool debugEnabled) {
    m_debugEnabled = debugEnabled;
}

std::vector<std::string> e172vp::GraphicsObjectCreateInfo::requiredDeviceExtensions() const {
    return m_requiredDeviceExtensions;
}

void e172vp::GraphicsObjectCreateInfo::setRequiredDeviceExtensions(const std::vector<std::string> &requiredDeviceExtensions) {
    m_requiredDeviceExtensions = requiredDeviceExtensions;
}



size_t e172vp::GraphicsObjectCreateInfo::descriptorPoolSize() const {
    return m_descriptorPoolSize;
}

void e172vp::GraphicsObjectCreateInfo::setDescriptorPoolSize(const size_t &descriptorPoolSize) {
    m_descriptorPoolSize = descriptorPoolSize;
}

e172vp::GraphicsObjectCreateInfo::GraphicsObjectCreateInfo() {}

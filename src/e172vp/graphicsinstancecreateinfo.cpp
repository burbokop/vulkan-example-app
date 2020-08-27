#include "graphicsinstancecreateinfo.h"


const std::string e172vp::GraphicsInstanceCreateInfo::applicationName() const {
    return m_applicationName;
}

void e172vp::GraphicsInstanceCreateInfo::setApplicationName(const std::string &applicationName) {
    m_applicationName = applicationName;
}

uint32_t e172vp::GraphicsInstanceCreateInfo::applicationVersion() const {
    return m_applicationVersion;
}

void e172vp::GraphicsInstanceCreateInfo::setApplicationVersion(const uint32_t &applicationVersion) {
    m_applicationVersion = applicationVersion;
}

const std::vector<std::string> e172vp::GraphicsInstanceCreateInfo::requiredExtensions() const {
    return m_requiredExtensions;
}

void e172vp::GraphicsInstanceCreateInfo::setRequiredExtensions(const std::vector<std::string> &requiredExtensions) {
    m_requiredExtensions = requiredExtensions;
}

std::function<void (vk::Instance, vk::SurfaceKHR *)> e172vp::GraphicsInstanceCreateInfo::surfaceCreator() const {
    return m_surfaceCreator;
}

void e172vp::GraphicsInstanceCreateInfo::setSurfaceCreator(const std::function<void (vk::Instance, vk::SurfaceKHR *)> &surfaceCreator) {
    m_surfaceCreator = surfaceCreator;
}

bool e172vp::GraphicsInstanceCreateInfo::debugEnabled() const {
    return m_debugEnabled;
}

void e172vp::GraphicsInstanceCreateInfo::setDebugEnabled(bool debugEnabled) {
    m_debugEnabled = debugEnabled;
}

std::vector<std::string> e172vp::GraphicsInstanceCreateInfo::requiredDeviceExtensions() const {
    return m_requiredDeviceExtensions;
}

void e172vp::GraphicsInstanceCreateInfo::setRequiredDeviceExtensions(const std::vector<std::string> &requiredDeviceExtensions) {
    m_requiredDeviceExtensions = requiredDeviceExtensions;
}

e172vp::GraphicsInstanceCreateInfo::GraphicsInstanceCreateInfo() {}

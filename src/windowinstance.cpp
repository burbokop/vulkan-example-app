#include "windowinstance.h"

GLFWwindow *WindowInstance::window() const
{
    return m_window;
}

WindowInstance::WindowInstance() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

bool WindowInstance::alive() {
    return !glfwWindowShouldClose(m_window);
}

void WindowInstance::update() {
    glfwPollEvents();
}


WindowInstance::~WindowInstance() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

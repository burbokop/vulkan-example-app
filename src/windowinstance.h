#ifndef WINDOWINSTANCE_H
#define WINDOWINSTANCE_H

#include "vulkaninstance.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

class WindowInstance {
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    GLFWwindow *m_window = nullptr;
public:
    WindowInstance();


    bool alive();
    void update();

    ~WindowInstance();
    GLFWwindow *window() const;
};

#endif // WINDOWINSTANCE_H

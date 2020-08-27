#include "vulkaninstance.h"
#include "windowinstance.h"

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN


#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include "e172vp/renderer.h"

int main() {
    //WindowInstance app;
    //VulkanInstance vulkanInstance(app.window());

    e172vp::Renderer renderer;
    while (renderer.isAlive()) {
        renderer.update();
    }
    return 0;
}

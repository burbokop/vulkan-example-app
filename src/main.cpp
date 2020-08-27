#include "vulkaninstance.h"
#include "windowinstance.h"

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN


#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>


int main() {
    WindowInstance app;
    VulkanInstance vulkanInstance(app.window());


    try {

        while (app.alive()) {
            app.update();
            vulkanInstance.paint();
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

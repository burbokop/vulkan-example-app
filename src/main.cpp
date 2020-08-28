#include "vulkaninstance.h"
#include "windowinstance.h"


#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include "e172vp/renderer.h"

int main() {
    bool c_version = true;

    if(c_version) {
        WindowInstance app;
        VulkanInstance vulkanInstance(app.window());

        while (app.alive()) {
            app.update();
            vulkanInstance.paint();
        }
    } else {

        e172vp::Renderer renderer;
        while (renderer.isAlive()) {
            renderer.update();
        }
    }
    return 0;
}

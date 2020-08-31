
#include "e172vp/renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>

int main() {
    e172vp::Renderer renderer;

    renderer.addVertexObject({
                                 { {-0.1f, -0.1f}, { 1.0f, 0.0f, 0.0f } },
                                 { {0.1f, -0.1f}, { 0.0f, 1.0f, 0.0f } },
                                 { {0.1f, 0.1f}, { 0.0f, 0.0f, 1.0f } },
                                 { {-0.1f, 0.1f}, { 1.0f, 1.0f, 1.0f } }
                             }, {
                                 0, 1, 2,
                                 2, 3, 0
                             });



    //std::cout << obj->model();

    auto obj = renderer.addVertexObject({
                                            { {-0.1f, -0.1f}, { 1.0f, 0.0f, 0.0f } },
                                            { {0.1f, -0.1f}, { 0.0f, 1.0f, 0.0f } },
                                            { {0.1f, 0.1f}, { 0.0f, 0.0f, 1.0f } },
                                            { {-0.1f, 0.1f}, { 1.0f, 1.0f, 1.0f } }
                                        }, {
                                            0, 1, 2,
                                            2, 3, 0
                                        });

    obj->setTranslation(glm::translate(glm::mat4(1.), glm::vec3(0.4, 0.4, 0.)));


    auto startTime = std::chrono::high_resolution_clock::now();


    while (renderer.isAlive()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        obj->setRotation(glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

        renderer.applyPresentation();
    }
    return 0;
}


#include "e172vp/renderer.h"
#include "resources.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>
#include "e172vp/tools/mesh.h"

int main() {
    e172vp::Renderer renderer;

    const auto shipMash = e172vp::Mesh::load("../models/ship.obj");
    const auto cubeMash = e172vp::Mesh::load("../models/cube.obj");


    auto plate = renderer.addVertexObject(Resources::vertices("plate"), Resources::indices("plate"));

    auto ship0 = renderer.addVertexObject(Resources::vertices(""), Resources::indices(""));
    auto ship1 = renderer.addVertexObject(shipMash);
    auto cube = renderer.addVertexObject(cubeMash);


    ship0->setTranslation(glm::translate(glm::mat4(1.), glm::vec3(0.4, 0.4, 0.)));
    ship0->setScale(glm::scale(glm::mat4(1.), glm::vec3(0.01, 0.01, 0.01)));

    ship1->setRotation(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    ship1->setScale(glm::scale(glm::mat4(1.), glm::vec3(0.02, 0.02, 0.02)));

    cube->setScale(glm::scale(glm::mat4(1.), glm::vec3(0.1)));
    cube->setTranslation(glm::translate(glm::mat4(1.), glm::vec3(-0.4, -0.4, 0.)));

    plate->setTranslation(glm::translate(glm::mat4(1.), glm::vec3(0.4, -0.4, 0.)));

    auto startTime = std::chrono::high_resolution_clock::now();


    while (renderer.isAlive()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


        ship0->setRotation(glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        cube->setRotation(glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.5f, 0.5f, 0.0f)));

        renderer.applyPresentation();
    }
    return 0;
}

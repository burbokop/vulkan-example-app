
#include "e172vp/geometry/Mesh.h"
#include "e172vp/geometry/ObjMesh.h"
#include "e172vp/renderer.h"
#include "e172vp/tools/Model.h"
#include "e172vp/utils/Fs.h"
#include "resources.h"
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

int main()
{
    const auto defaultVertShaderCode = e172vp::Fs::readBinary("shaders/vert_uniform.vert.spv").value();
    const auto defaultFragShaderCode = e172vp::Fs::readBinary("shaders/shader.frag.spv").value();

    e172vp::Renderer renderer;

    const auto shipMash = e172vp::Mesh::fromObjMesh(e172vp::ObjMesh::load("models/ship.obj"), { 0, 0, 0 });
    const auto cubeMash = e172vp::Mesh::fromObjMesh(e172vp::ObjMesh::load("models/cube.obj"), { 0, 0, 0 });
    const auto ship2Mash = e172vp::Mesh::fromObjMesh(e172vp::ObjMesh::load("models/ship2.obj"), { 0, 0, 0 });

    auto plate = renderer.addObject(
        e172vp::Model(Resources::mesh("plate"),
            e172vp::Fs::readBinary("shaders/vert_uniform.vert.spv").value(),
            e172vp::Fs::readBinary("shaders/plane.frag.spv").value()));

    auto ship0 = renderer.addObject(e172vp::Model(ship2Mash, defaultVertShaderCode, defaultFragShaderCode));
    auto ship1 = renderer.addObject(e172vp::Model(shipMash, defaultVertShaderCode, defaultFragShaderCode));
    auto cube = renderer.addObject(e172vp::Model(cubeMash, defaultVertShaderCode, defaultFragShaderCode));

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


        ship0->setRotation(glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        cube->setRotation(glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.5f, 0.5f, 0.0f)));
        // plate->setScale(glm::scale(glm::mat4(1.), glm::vec3(std::sin(time))));

        renderer.applyPresentation();
    }
    return 0;
}

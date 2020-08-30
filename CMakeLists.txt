cmake_minimum_required(VERSION 3.5)

project(vulkan-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)






add_executable(vulkan-app
    src/e172vp/graphicsobject.cpp
    src/e172vp/graphicsobject.h
    src/e172vp/graphicsobjectcreateinfo.cpp
    src/e172vp/graphicsobjectcreateinfo.h
    src/e172vp/tools/hardware.cpp
    src/e172vp/tools/hardware.h
    src/e172vp/tools/extensiontools.cpp
    src/e172vp/tools/extensiontools.h
    src/e172vp/tools/validation.cpp
    src/e172vp/tools/validation.h
    src/e172vp/tools/stringvector.cpp
    src/e172vp/tools/stringvector.h
    src/e172vp/renderer.cpp
    src/e172vp/renderer.h
    src/e172vp/tools/vulkaninstancefactory.cpp
    src/e172vp/tools/vulkaninstancefactory.h
    src/e172vp/tools/logicdevicefactory.cpp
    src/e172vp/tools/logicdevicefactory.h
    src/e172vp/swapchain.cpp
    src/e172vp/swapchain.h
    src/e172vp/renderpass.cpp
    src/e172vp/renderpass.h
    src/e172vp/commandpool.cpp
    src/e172vp/commandpool.h
    src/e172vp/vertex.cpp
    src/e172vp/vertex.h
    src/e172vp/tools/buffer.cpp
    src/e172vp/tools/buffer.h

    src/e172vp/time/time.h
    src/e172vp/time/time.cpp
    src/e172vp/time/elapsedtimer.h
    src/e172vp/time/elapsedtimer.cpp
    src/main.cpp
    )




find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(vulkan-app PRIVATE
    Vulkan::Vulkan
    glfw
    )


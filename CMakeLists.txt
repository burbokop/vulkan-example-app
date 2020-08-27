cmake_minimum_required(VERSION 3.5)

project(vulkan-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)






add_executable(vulkan-app
    src/e172vp/graphicsinstance.cpp
    src/e172vp/graphicsinstance.h
    src/e172vp/graphicsinstancecreateinfo.cpp
    src/e172vp/graphicsinstancecreateinfo.h
    src/e172vp/hardware.cpp
    src/e172vp/hardware.h
    src/e172vp/extensiontools.cpp
    src/e172vp/extensiontools.h
    src/e172vp/renderer.cpp
    src/e172vp/renderer.h

    src/time/time.h
    src/time/time.cpp
    src/time/elapsedtimer.h
    src/time/elapsedtimer.cpp
    src/queuetools.h
    src/windowinstance.h
    src/vulkaninstance.h
    src/queuetools.cpp
    src/windowinstance.cpp
    src/vulkaninstance.cpp
    src/main.cpp
    )




find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(vulkan-app PRIVATE
    Vulkan::Vulkan
    glfw
    )


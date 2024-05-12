cmake_minimum_required(VERSION 3.5)

project(vulkan-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(__cpp_concepts=202002L)

add_executable(
  vulkan-app
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
  src/e172vp/geometry/Vertex.cpp
  src/e172vp/geometry/Vertex.h
  src/e172vp/tools/buffer.cpp
  src/e172vp/tools/buffer.h
  src/e172vp/vertexobject.cpp
  src/e172vp/vertexobject.h
  src/e172vp/descriptorsetlayout.cpp
  src/e172vp/descriptorsetlayout.h
  src/e172vp/time/time.h
  src/e172vp/time/time.cpp
  src/e172vp/time/elapsedtimer.h
  src/e172vp/time/elapsedtimer.cpp
  src/e172vp/geometry/Mesh.cpp
  src/e172vp/geometry/Mesh.h
  src/e172vp/pipeline.cpp
  src/e172vp/pipeline.h
  src/e172vp/font.cpp
  src/e172vp/font.h
  src/main.cpp
  src/resources.cpp
  image/stb_image.h
  src/e172vp/utils/NoNull.h
  src/e172vp/tools/Model.h
  src/e172vp/tools/Model.cpp
  src/e172vp/geometry/ObjMesh.h
  src/e172vp/geometry/ObjMesh.cpp
  src/e172vp/utils/Fs.h
  src/e172vp/utils/Fs.cpp
  src/e172vp/utils/Error.h
  src/e172vp/utils/Error.cpp)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(vulkan-app PRIVATE Vulkan::Vulkan glfw freetype)

target_include_directories(vulkan-app
                           PUBLIC $<BUILD_INTERFACE:/usr/include/freetype2>)

include(cmake/shader_target.cmake)

add_shader_target(
  shaders
  OUTPUT_DIR
  ${CMAKE_CURRENT_BINARY_DIR}/shaders
  SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/shaders/shader.frag
  ${CMAKE_CURRENT_LIST_DIR}/shaders/plane.frag
  ${CMAKE_CURRENT_LIST_DIR}/shaders/shader.vert
  ${CMAKE_CURRENT_LIST_DIR}/shaders/vert_uniform.vert)

add_dependencies(vulkan-app shaders)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/fonts
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_LIST_DIR}/image
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_LIST_DIR}/models
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#include "validation.h"

#include <vulkan/vulkan.h>

std::vector<std::string> e172vp::Validation::presentLayers() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    std::vector<std::string> result(layerCount);
    for(size_t i = 0; i < layerCount; ++i) {
        result[i] = availableLayers[i].layerName;
    }
    return result;
}

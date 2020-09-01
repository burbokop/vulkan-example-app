#ifndef FONT_H
#define FONT_H

#include <ft2build.h>
#include <freetype/freetype.h>
#include <glm/glm.hpp>
#include <string>
#include <map>
#include <vulkan/vulkan.hpp>

namespace e172vp {

class Font {
    static inline FT_Library ft;
    static inline bool libraryInitialized = false;

    FT_Face face;


    struct Character {
        vk::Image image;
        vk::DeviceMemory imageMemory;
        vk::Format imageFormat;

        glm::ivec2   size;       // Size of glyph
        glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
        unsigned int advance;    // Offset to advance to next glyph
    };

    std::map<char, Character> characters;

public:
    static bool createTextureImage32(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, const vk::CommandPool &commandPool, const vk::Queue &copyQueue, void *pixels, size_t w, size_t h, vk::Format format, vk::Image *image, vk::DeviceMemory *imageMemory);
    static void createImage(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image *image, vk::DeviceMemory *imageMemory);
    static void transitionImageLayout(const vk::Device &logicalDevice, const vk::CommandPool &commandPool, const vk::Queue &queue, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    static vk::CommandBuffer beginSingleTimeCommands(const vk::Device &logicalDevice, const vk::CommandPool &commandPool);
    static void endSingleTimeCommands(const vk::Device &logicalDevice, const vk::CommandPool &commandPool, const vk::Queue &queue, vk::CommandBuffer commandBuffer);
    static void copyBufferToImage(const vk::Device &logicalDevice, const vk::CommandPool &commandPool, const vk::Queue &queue, vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
    static vk::ImageView createImageView(const vk::Device &logicalDevice, vk::Image image, vk::Format format);


    Font(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, const vk::CommandPool &commandPool, const vk::Queue &copyQueue, const std::string &path);


    Character character(char c) const;
};

}

#endif // FONT_H

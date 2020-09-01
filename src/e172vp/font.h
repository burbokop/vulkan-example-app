#ifndef FONT_H
#define FONT_H

#include <glm/glm.hpp>
#include <string>
#include <map>
#include <vulkan/vulkan.hpp>

namespace e172vp {

class Font {

    vk::Device m_logicalDevice;


    class Character {
        friend Font;
        vk::DeviceMemory m_imageMemory;
        vk::Image m_image;
        vk::ImageView m_imageView;
        vk::Format m_imageFormat;

        glm::ivec2 m_size;
        glm::ivec2 m_bearing;
        unsigned int m_advance;
        bool m_isValid = false;
    public:
        vk::Image image() const;
        vk::ImageView imageView() const;
        vk::Format imageFormat() const;
        glm::ivec2 size() const;
        glm::ivec2 bearing() const;
        unsigned int advance() const;
        bool isValid() const;
    };

    std::map<char, Character> characters;

public:
    static bool createTextureImage32(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, const vk::CommandPool &commandPool, const vk::Queue &copyQueue, void *pixels, size_t w, size_t h, vk::Format format, vk::Image *image, vk::DeviceMemory *imageMemory);
    static void createImage(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image *image, vk::DeviceMemory *imageMemory);
    static void transitionImageLayout(const vk::Device &logicalDevice, const vk::CommandPool &commandPool, const vk::Queue &queue, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    static vk::CommandBuffer beginSingleTimeCommands(const vk::Device &logicalDevice, const vk::CommandPool &commandPool);
    static void endSingleTimeCommands(const vk::Device &logicalDevice, const vk::CommandPool &commandPool, const vk::Queue &queue, vk::CommandBuffer commandBuffer);
    static void copyBufferToImage(const vk::Device &logicalDevice, const vk::CommandPool &commandPool, const vk::Queue &queue, vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

    Font(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, const vk::CommandPool &commandPool, const vk::Queue &copyQueue, const std::string &path, size_t size);

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    ~Font();

    Character character(char c) const;
};

}

#endif // FONT_H

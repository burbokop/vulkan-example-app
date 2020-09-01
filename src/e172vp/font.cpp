#include "font.h"
#include "swapchain.h"

#include <iostream>
#include "tools/buffer.h"
#include <thread>
#include <ft2build.h>
#include <freetype/freetype.h>



bool e172vp::Font::createTextureImage32(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, const vk::CommandPool &commandPool, const vk::Queue &copyQueue, void* pixels, size_t w, size_t h, vk::Format format, vk::Image *image, vk::DeviceMemory *imageMemory) {
    int channelCount = 0;
    if(format == vk::Format::eR8G8B8Srgb) {
        channelCount = 3;
    } else if(format == vk::Format::eR8G8B8A8Srgb) {
        channelCount = 4;
    } else if(format == vk::Format::eR8Srgb) {
        channelCount = 1;
    } else {
        std::cerr << "unsupported image format\n";
        return false;
    }
    VkDeviceSize imageSize = w * h * channelCount;

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;
    if(!Buffer::createAbstractBuffer(logicalDevice, physicalDevice, imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, &stagingBuffer, &stagingBufferMemory))
        return false;

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    createImage(logicalDevice, physicalDevice, w, h, format, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, image, imageMemory);

    transitionImageLayout(logicalDevice, commandPool, copyQueue, *image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    copyBufferToImage(logicalDevice, commandPool, copyQueue, stagingBuffer, *image, static_cast<uint32_t>(w), static_cast<uint32_t>(h));
    transitionImageLayout(logicalDevice, commandPool, copyQueue, *image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    logicalDevice.destroyBuffer(stagingBuffer);
    logicalDevice.freeMemory(stagingBufferMemory);
    return true;
}

void e172vp::Font::createImage(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Image *image, vk::DeviceMemory *imageMemory) {
    vk::ImageCreateInfo imageInfo{};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = usage;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;

    if (logicalDevice.createImage(&imageInfo, nullptr, image) != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create image!");
    }

    vk::MemoryRequirements memRequirements;
    logicalDevice.getImageMemoryRequirements(*image, &memRequirements);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Buffer::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    const auto code = logicalDevice.allocateMemory(&allocInfo, nullptr, imageMemory);
    if (code != vk::Result::eSuccess) {
        throw std::runtime_error("failed to allocate image memory: " + vk::to_string(code));
    }

    logicalDevice.bindImageMemory(*image, *imageMemory, 0);
}

void e172vp::Font::transitionImageLayout(const vk::Device &logicalDevice, const vk::CommandPool &commandPool, const vk::Queue &queue, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands(logicalDevice, commandPool);

    vk::ImageMemoryBarrier barrier;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    commandBuffer.pipelineBarrier(
                sourceStage,
                destinationStage,
                vk::DependencyFlags(),
                0, nullptr,
                0, nullptr,
                1, &barrier
                );

    endSingleTimeCommands(logicalDevice, commandPool, queue, commandBuffer);
}

vk::CommandBuffer e172vp::Font::beginSingleTimeCommands(const vk::Device &logicalDevice, const vk::CommandPool &commandPool) {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    vk::CommandBuffer commandBuffer;
    logicalDevice.allocateCommandBuffers(&allocInfo, &commandBuffer);

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(&beginInfo);

    return commandBuffer;
}

void e172vp::Font::endSingleTimeCommands(const vk::Device &logicalDevice, const vk::CommandPool &commandPool, const vk::Queue &queue, vk::CommandBuffer commandBuffer) {
    commandBuffer.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    queue.submit(1, &submitInfo, vk::Fence());
    queue.waitIdle();

    logicalDevice.freeCommandBuffers(commandPool, 1, &commandBuffer);
}

void e172vp::Font::copyBufferToImage(const vk::Device &logicalDevice, const vk::CommandPool &commandPool, const vk::Queue &queue, vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height) {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands(logicalDevice, commandPool);

    vk::BufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = vk::Offset3D {0, 0, 0};
    region.imageExtent = vk::Extent3D {
        width,
        height,
        1
    };

    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

    endSingleTimeCommands(logicalDevice, commandPool, queue, commandBuffer);
}

e172vp::Font::Font(const vk::Device &logicalDevice, const vk::PhysicalDevice &physicalDevice, const vk::CommandPool &commandPool, const vk::Queue &copyQueue, const std::string &path, size_t size) {
    m_logicalDevice = logicalDevice;

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, path.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, size);



    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        Character character;


        //face->glyph->subglyphs

        std::string f = {
            static_cast<char>(face->glyph->format >> 24),
            static_cast<char>(face->glyph->format >> 16),
            static_cast<char>(face->glyph->format >> 8),
            static_cast<char>(face->glyph->format >> 0)
        };






        if(face->glyph->bitmap.width > 0 && face->glyph->bitmap.rows > 0) {
            character.m_imageFormat = vk::Format::eR8G8B8A8Srgb;

            uint32_t rgba_buffer[face->glyph->bitmap.rows * face->glyph->bitmap.width];
            auto p = reinterpret_cast<uint8_t*>(face->glyph->bitmap.buffer);
            for(size_t y = 0; y < face->glyph->bitmap.rows; ++y) {
                for(size_t x = 0; x < face->glyph->bitmap.width; ++x) {
                    uint8_t c = p[x + y * face->glyph->bitmap.width];

                    uint32_t c32 = static_cast<uint32_t>(c);
                    uint32_t color = 0xff0088ff;
                    uint32_t gray = color & c32;

                    rgba_buffer[x + y * face->glyph->bitmap.width] = gray;
                }
            }

            if(!createTextureImage32(logicalDevice, physicalDevice, commandPool, copyQueue, &rgba_buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, character.m_imageFormat, &character.m_image, &character.m_imageMemory))
                break;

            character.m_imageView = SwapChain::createImageView(logicalDevice, character.m_image, character.m_imageFormat);
            character.m_size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
            character.m_bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
            character.m_advance = face->glyph->advance.x;
            character.m_isValid = true;

            characters.insert(std::pair<char, Character>(c, character));
        }

    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

e172vp::Font::~Font() {
    if(m_logicalDevice) {
        for(auto c : characters) {
            if(c.second.m_imageView)
                m_logicalDevice.destroyImageView(c.second.m_imageView);
            if(c.second.m_image)
                m_logicalDevice.destroyImage(c.second.m_image);
            if(c.second.m_imageMemory)
                m_logicalDevice.freeMemory(c.second.m_imageMemory);
        }
    }
}

e172vp::Font::Character e172vp::Font::character(char c) const {
    return characters.at(c);
}


vk::Image e172vp::Font::Character::image() const { return m_image; }
vk::ImageView e172vp::Font::Character::imageView() const { return m_imageView; }
vk::Format e172vp::Font::Character::imageFormat() const { return m_imageFormat; }
glm::ivec2 e172vp::Font::Character::size() const { return m_size; }
glm::ivec2 e172vp::Font::Character::bearing() const { return m_bearing; }
unsigned int e172vp::Font::Character::advance() const { return m_advance; }
bool e172vp::Font::Character::isValid() const { return m_isValid; }

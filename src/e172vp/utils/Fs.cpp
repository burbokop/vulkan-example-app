#include "Fs.h"

#include <cstring>
#include <fstream>

namespace e172vp::Fs {

std::vector<std::uint8_t> charToUInt8(std::vector<char> d)
{
    return std::vector<std::uint8_t>(reinterpret_cast<std::uint8_t*>(d.data()), reinterpret_cast<std::uint8_t*>(d.data()) + d.size());
}

Expected<Bytes> readBinary(const std::filesystem::path& path) noexcept
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        return unexpected("Failed to open file \"" + path.string() + "\": " + std::strerror(errno));
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return charToUInt8(buffer);
}

}

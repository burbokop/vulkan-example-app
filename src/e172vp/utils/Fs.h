#pragma once

#include "Error.h"
#include <cstdint>
#include <expected>
#include <filesystem>
#include <vector>

namespace e172vp::Fs {

using Bytes = std::vector<std::uint8_t>;

Expected<Bytes> readBinary(const std::filesystem::path& path) noexcept;

}

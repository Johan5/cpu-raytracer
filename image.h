#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include "thirdparty/stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "thirdparty/stb/stb_image_write.h"
#include "Eigen/Core"
#include "rgb.h"

#include <vector>
#include <filesystem>

namespace internal {
    void appendToVec(std::vector<uint8_t> &vec, Eigen::Vector3d rgb) {
        auto ri = static_cast<uint8_t>(255.9 * rgb.x());
        auto gi = static_cast<uint8_t>(255.9 * rgb.y());
        auto bi = static_cast<uint8_t>(255.9 * rgb.z());
        vec.push_back(ri);
        vec.push_back(gi);
        vec.push_back(bi);
    }
}

template<typename PixelT>
class Image {
public:
    Image() = default;

    Image(std::vector<PixelT> &&data, int64_t width, int64_t height, int32_t channels) : _data(std::move(data)),
                                                                                         _width(width),
                                                                                         _height(height),
                                                                                         _channels(channels) {
        assert(_data.size() == width * height);
    }

    /// @return true iff success
    bool writeToFile(const std::filesystem::path &path) const {
        std::vector<uint8_t> rawData;
        rawData.reserve(_data.size() * sizeof(PixelT));
        for (PixelT pixel : _data) {
            internal::appendToVec(rawData, pixel);
        }
        int error = stbi_write_png("test.png", _width, _height, _channels, rawData.data(), _width * _channels);

        return error == 1;
    }

private:
    std::vector<PixelT> _data;
    int64_t _height;
    int64_t _width;
    int32_t _channels;
};
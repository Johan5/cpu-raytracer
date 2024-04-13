#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include "thirdparty/stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "thirdparty/stb/stb_image_write.h"
#include "vec.h"

#include <vector>
#include <filesystem>

namespace internal {
    /// Transforms given pixel from linear space to gamma space, using gamma 2 inverse
    Vec3d calcGamma2Correction(const Vec3d& rgb) {
        auto linearToGamma = [](double c) {
            if (c > 0.0) {
                return sqrt(c);
            }
            return 0.0;
        };
        return Vec3d{linearToGamma(rgb.x()), linearToGamma(rgb.y()), linearToGamma(rgb.z())};
    }

    void appendToVec(std::vector<uint8_t> &vec, const Vec3d& rgb) {
        auto ri = static_cast<uint8_t>(256 * std::clamp(rgb.x(), 0.0, 0.999));
        auto gi = static_cast<uint8_t>(256 * std::clamp(rgb.y(), 0.0, 0.999));
        auto bi = static_cast<uint8_t>(256 * std::clamp(rgb.z(), 0.0, 0.999));
        vec.push_back(ri);
        vec.push_back(gi);
        vec.push_back(bi);
    }
}

template<typename PixelT>
class Image {
public:
    Image() = default;

    /// The given image data must be in linear space (it will be gamma-corrected before writing to file)
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
            PixelT gammaCorrectedPixel = internal::calcGamma2Correction(pixel);
            internal::appendToVec(rawData, gammaCorrectedPixel);
        }
        int error = stbi_write_png("../img_out/test.png", _width, _height, _channels, rawData.data(), _width * _channels);

        return error == 1;
    }

private:


    std::vector<PixelT> _data;
    int64_t _height;
    int64_t _width;
    int32_t _channels;
};
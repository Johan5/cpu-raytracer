#include <iostream>

#include "image.h"
#include "rgb.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    int32_t width = 256;
    int32_t height = 256;
    std::vector<Rgb> pixels;
    for (int32_t row = 0; row < height; ++row) {
        std::cout << "\rScanlines remaining: " << (height - row) << ' ' << std::flush;
        for (int32_t col = 0; col < width; ++col) {
            double r = static_cast<double>(col) / (width - 1);
            double g = static_cast<double>(row) / (height - 1);
            double b = 0.0;
            auto ri = static_cast<uint8_t>(255.9 * r);
            auto gi = static_cast<uint8_t>(255.9 * g);
            auto bi = static_cast<uint8_t>(255.9 * b);
            pixels.push_back(Rgb{ri, gi, bi});
        }
    }
    std::cout << "\rDone.                 \n";

    Image<Rgb> image{std::move(pixels), width, height, 3};
    image.writeToFile(std::filesystem::path{"./"});

    return 0;
}

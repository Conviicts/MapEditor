#include "png_loader.h"


unsigned char* load_png(const std::string& filename, unsigned int* width, unsigned int* height) {
    std::vector<unsigned char> image;

    if (const unsigned error = lodepng::decode(image, *width, *height, filename, LCT_RGBA, 8)) {
        std::cerr << lodepng_error_text(error) << std::endl;
        return nullptr;
    }
    auto* image_data = new unsigned char[image.size()];
    std::copy(image.begin(), image.end(), image_data);

    return image_data;
}

void free_image_data(const unsigned char* data) {
    delete[] data;
}

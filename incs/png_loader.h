#pragma once

#include <png.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "lodepng.h"

unsigned char* load_png(const std::string& filename, unsigned int* width, unsigned int* height);
void free_image_data(const unsigned char* data);

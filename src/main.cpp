#include <iostream>
#include <fstream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb/stb_image.h"


int main() {
    int width, height, channels;
    unsigned char *data = stbi_load("image.jpg", &width, &height, &channels, 0);
    // ... use data ...
    stbi_image_free(data);
    return 0;
}
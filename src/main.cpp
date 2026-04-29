#include <iostream>
#include <string>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb/stb_image.h"


int main() {
    //Take in a user input for the file path of the image to load
    std::string filePath;
    unsigned char *data = nullptr;
    int width, height, channels;


    //Loop until a valid file path is provided
    while (true) {
        std::cout << "Enter the full file path (or 'q' to quit): ";
        std::getline(std::cin, filePath);

        if (filePath == "q") {
            return 0;
        }

        std::filesystem::path path(filePath);
        if (!std::filesystem::exists(path)) {
            std::cerr << "Error: File does not exist: " << filePath << std::endl;
            continue;
        }

        if (!std::filesystem::is_regular_file(path)) {
            std::cerr << "Error: Not a regular file: " << filePath << std::endl;
            continue;
        }

        data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
        if (!data) {
            std::cerr << "Error: Failed to load image: "
                      << stbi_failure_reason() << std::endl;
            continue;
        }

        std::cout << "Loaded image successfully: " << std::endl;

        

        stbi_image_free(data);
        break;
    }

    return 0;
}
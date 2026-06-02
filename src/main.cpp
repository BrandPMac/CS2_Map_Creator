#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb/stb_image_write.h"

// Constants for CS2 heightmap
const int CS2_HEIGHTMAP_WIDTH = 4096;
const int CS2_HEIGHTMAP_HEIGHT = 4096;

// Function to resize image using bilinear interpolation
unsigned char* resizeImage(unsigned char* sourceData, int srcWidth, int srcHeight, 
                           int srcChannels, int dstWidth, int dstHeight) {
    unsigned char* dstData = new unsigned char[dstWidth * dstHeight * srcChannels];
    
    for (int y = 0; y < dstHeight; y++) {
        for (int x = 0; x < dstWidth; x++) {
            // Calculate corresponding position in source image
            float srcX = (x / (float)dstWidth) * srcWidth;
            float srcY = (y / (float)dstHeight) * srcHeight;
            
            // Clamp to valid range
            srcX = std::max(0.0f, std::min(srcX, (float)srcWidth - 1.001f));
            srcY = std::max(0.0f, std::min(srcY, (float)srcHeight - 1.001f));
            
            int x0 = (int)srcX;
            int y0 = (int)srcY;
            int x1 = std::min(x0 + 1, srcWidth - 1);
            int y1 = std::min(y0 + 1, srcHeight - 1);
            
            float fx = srcX - x0;
            float fy = srcY - y0;
            
            // Bilinear interpolation for each channel
            for (int c = 0; c < srcChannels; c++) {
                float v00 = sourceData[(y0 * srcWidth + x0) * srcChannels + c];
                float v10 = sourceData[(y0 * srcWidth + x1) * srcChannels + c];
                float v01 = sourceData[(y1 * srcWidth + x0) * srcChannels + c];
                float v11 = sourceData[(y1 * srcWidth + x1) * srcChannels + c];
                
                float v0 = v00 * (1 - fx) + v10 * fx;
                float v1 = v01 * (1 - fx) + v11 * fx;
                float v = v0 * (1 - fy) + v1 * fy;
                
                dstData[(y * dstWidth + x) * srcChannels + c] = (unsigned char)(v + 0.5f);
            }
        }
    }
    
    return dstData;
}


int main() {
    //Take in a user input for the file path of the image to load
    std::string filePath;
    unsigned char *data = nullptr;
    int width, height, channels;


    //Loop until a valid file path is provided
    while (true) {
        std::cout << "Enter the full file path (or 'q' to quit): ";
        std::getline(std::cin, filePath);

        // Remove surrounding quotes if present
        if (filePath.length() >= 2 && filePath.front() == '"' && filePath.back() == '"') {
            filePath = filePath.substr(1, filePath.length() - 2);
        }

        if (filePath == "q") {
            return 0;
        }

        // If just a filename is given, default to inputs folder
        if (filePath.find('\\') == std::string::npos && filePath.find('/') == std::string::npos) {
            filePath = "inputs/" + filePath;
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
        
        // Check if image needs resizing
        if (width != CS2_HEIGHTMAP_WIDTH || height != CS2_HEIGHTMAP_HEIGHT) {
            unsigned char* resizedData = resizeImage(data, width, height, channels, 
                                                     CS2_HEIGHTMAP_WIDTH, CS2_HEIGHTMAP_HEIGHT);
            stbi_image_free(data);
            data = resizedData;
            width = CS2_HEIGHTMAP_WIDTH;
            height = CS2_HEIGHTMAP_HEIGHT;
            
        }

        // Create outputs folder if it doesn't exist
        std::filesystem::path outputDir("outputs");
        std::filesystem::create_directories(outputDir);

        // Generate output filename
        std::string filename = path.stem().string() + "_cs2_heightmap.png";
        std::filesystem::path outputPath = outputDir / filename;

        // Save the image
        int success = stbi_write_png(outputPath.string().c_str(), width, height, channels, data, width * channels);
        if (success) {
            std::cout << "Image exported successfully to: " << outputPath.string() << std::endl;
        } else {
            std::cerr << "Error: Failed to save image to: " << outputPath.string() << std::endl;
        }

        stbi_image_free(data);
        break;
    }

    return 0;
}
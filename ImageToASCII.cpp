#include <stdio.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "stb_image_write.h"
#pragma clang diagnostic pop

// boxHeight and boxWidth specify the number of pixels in the height and width of the box that an ASCII character will
// replace. Since chars are twice as tall as they are wide, it's best to set the height to twice the size of the width
#define boxWidth 3
#define boxHeight 6
#define inputFileName "Revolver.png"
#define outputFileName "Revolver.jpg"

char characters[70] = "@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";

int main() {
    int width, height, channels;

    // Loads image and stores pixel info in the data char array
    unsigned char* data = stbi_load(inputFileName, &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Loading image failed\n";
        return 1;
    }

    std::cout << "Image loaded: " << width << "x" << height << " with " << channels << " channels\n";

    // Grayscale each pixel of the image
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int index = (row * width + col) * channels;
            
            unsigned char r = data[index + 0];
            unsigned char g = data[index + 1];
            unsigned char b = data[index + 2];

            unsigned char gray = 0.3 * r + 0.59 * g + 0.11 * b;

            data[index + 0] = data[index + 1] = data[index + 2] = gray;
        }
    }

    char ASCIIART[height / boxHeight][width / boxWidth];
    // 
    for (int row = 0; row <= height - boxHeight; row += boxHeight) {
        for (int col = 0; col <= width - boxWidth; col += boxWidth) {
            int avg = 0;

            // Find the average color of the box by averaging every pixel value in the box
            for (int i = row; i < row + boxHeight; i++) {
                for (int j = col; j < col + boxWidth; j++) {
                    //get current index to get the color of the box
                    int index = (i * width + j) * channels;

                    // since it's grayscaled (r,g,b are the same), 
                    // you don't need r, g, and b, just one of them
                    avg += data[index];
                }
            }
            avg = (avg / (boxHeight * boxWidth));

            //std::cout << "Avg: " << avg << "\n";
            //std::cout << "Character index: " << (int)(avg * (((double)68 / 255)/1.3)) << std::endl;
            //std::cout << "Character to be printed: " << characters[(int)(avg * (((double)68 / 255)/1.3))] << std::endl;

            ASCIIART[row / boxHeight][col / boxWidth] = characters[(int)(avg * ((double)68 / 255))];

            // Make each value in the box equal to the average
            for (int i = row; i < row + boxHeight; i++) {
                for (int j = col; j < col + boxWidth; j++) {
                    //get current index
                    int index = (i * width + j) * channels;

                    data[index + 0] = avg;
                    data[index + 1] = avg;
                    data[index + 2] = avg;
                }
            }
        }
    }

    // Print the ASCII art
    for (int i = 0; i < height / boxHeight; i++) {
        for (int j = 0; j < width / boxWidth; j++) {
            std::cout << ASCIIART[i][j];
        }
        std::cout << std::endl;
    }
    
    stbi_write_jpg(outputFileName, width, height, channels, data, 100);

    // Free memory allocated for data
    stbi_image_free(data);

    return 0;
}
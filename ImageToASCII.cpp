#include <stdio.h>
#include <iostream>

#include "ImageToASCII.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "stb_image_write.h"
#pragma clang diagnostic pop

char characters[70] = "@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";

// boxHeight and boxWidth specify the number of pixels in the height and width of the box that an ASCII character will
// replace. Since chars are twice as tall as they are wide, it's best to set the height to twice the size of the width
int boxWidth = 3;
int boxHeight = 6;
const char* inputFileName = "Help!.png";
const char* outputFileName = "Help! with gaussian filter.jpg";

// Grayscales the image based on image's pixel data, height, width, and channels
void GrayScaleImage(unsigned char* data, int height, int width, int channels) {
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
}

// Takes image pixel data, a 2D array ASCIIArt, image height and width, and channel count
// and converts the image into ASCII art
void ImageToASCII(unsigned char* data, char ASCIIArt[], int height, int width, int channels) {
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

            ASCIIArt[(row / boxHeight) * (width / boxWidth) + col / boxWidth] = characters[(int)(avg * ((double)68 / 255))];

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
}

// Takes ASCIIArt array and the height and width of the original image
// and uses them and the boxHeight and boxWidth to print the array
void PrintASCIIArt(char ASCIIArt[], int height, int width) {
    // Print the ASCII art
    for (int i = 0; i < (height / boxHeight) * (width / boxWidth); i++) {
        std::cout << ASCIIArt[i];
        if(i % (width / boxWidth) == 0) {
            std::cout << std::endl;
        }
    }
}

// Takes image pixel data, image height, image width, and number of channels
// and performs a gaussian filter. Only works properly on gray-scaled image
void GaussianFilter(unsigned char* data, int height, int width, int channels) {
    // kernel and kernelSum according to wikipedia's page on Canny edge detection
    int kernel[5][5] = {
        {2,  4,  5,  4, 2},
        {4, 9, 12, 9, 4},
        {5, 12, 15, 12, 5},
        {4, 9, 12, 9, 4},
        {2,  4,  5,  4, 2}
    };
    int kernelSum = 159;

    for (int row = 2; row < height - 2; row++) {
        for (int col = 2; col < width - 2; col++) {
            int index = (row * width + col) * channels;
            
            // Is the sum of the products surrounding the current pixel
            double gaussSum = 0;

            // for each pixel around the current pixel, multiply it by the 
            // corresponding kernel value and add it to sum
            for (int i = -2; i < 3; i++) {
                for (int j = -2; j < 3; j++) {
                    int curindex = ((row + i) * width + (col + j)) * channels;

                    gaussSum += data[curindex] * kernel[i + 2][j + 2];
                }
            }

            // Calculate final gaussian value
            int gaussValue = gaussSum / 159;

            /*unsigned char r = data[index + 0];
            unsigned char g = data[index + 1];
            unsigned char b = data[index + 2];

            unsigned char gray = 0.3 * r + 0.59 * g + 0.11 * b;*/

            data[index + 0] = data[index + 1] = data[index + 2] = gaussValue;
        }
    }
}

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
    GrayScaleImage(data, height, width, channels);

    /*
    // Create ASCIIArt Array
    char ASCIIArt[(height / boxHeight) * (width / boxWidth)];

    ImageToASCII(data, ASCIIArt, height, width, channels);

    PrintASCIIArt(ASCIIArt, height, width);

    */

    GaussianFilter(data, height, width, channels);
    
    stbi_write_jpg(outputFileName, width, height, channels, data, 100);

    // Free memory allocated for data
    stbi_image_free(data);

    return 0;
}
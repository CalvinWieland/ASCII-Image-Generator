#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "ImageToASCII.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "stb_image_write.h"
#pragma clang diagnostic pop

char characters[70] = "@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
char charactersReversed[70] = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@";

// boxHeight and boxWidth specify the number of pixels in the height and width of the box that an ASCII character will
// replace. Since chars are twice as tall as they are wide, it's best to set the height to twice the size of the width

const char* outputFileName = "gaussian.jpg";
const char* outputFileName1 = "sobel X.jpg";
const char* outputFileName2 = "sobel Y.jpg";
const char* outputFileName3 = "gradient G.jpg";
const char* outputFileName4 = "NMS.jpg";
const char* outputFileName5 = "double threshold and pixelation.jpg";

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
void PixelDataToASCII(unsigned char* data, char ASCIIArt[], int height, int width, int channels, int boxHeight, int boxWidth) {
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

            ASCIIArt[(row / boxHeight) * (width / boxWidth) + col / boxWidth] = charactersReversed[(int)(avg * ((double)68 / 255))];

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
void PrintASCIIArt(char ASCIIArt[], int height, int width, int boxHeight, int boxWidth) {
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

            data[index + 0] = data[index + 1] = data[index + 2] = gaussValue;
        }
    }
}

// Performs the Sobel operater on the original image, data. other parameters are
// image height, width, number of channels, and the Sobel output
void SobelOperator(unsigned char* data, int height, int width, int channels, int* resX, int* resY) {
    // sobel kernels
    int sobelKernelX[3][3] = {
        {-1,  0,  1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    int sobelKernelY[3][3] = {
        {1,  2,  1},
        {0, 0, 0},
        {-1, -2, -1}
    };

    for (int row = 1; row < height - 1; row++) {
        for (int col = 1; col < width - 1; col++) {
            int index = (row * width + col) * channels;
            
            // Is the sum of the products surrounding the current pixel
            double sobelSumX = 0;
            double sobelSumY = 0;

            // for each pixel around the current pixel, multiply it by the 
            // corresponding kernel value and add it to sum
            for (int i = -1; i < 2; i++) {
                for (int j = -1; j < 2; j++) {
                    int curindex = ((row + i) * width + (col + j)) * channels;

                    sobelSumX += data[curindex] * sobelKernelX[i + 1][j + 1];
                    sobelSumY += data[curindex] * sobelKernelY[i + 1][j + 1];
                }
            }

            resX[index + 0] = resX[index + 1] = resX[index + 2] = sobelSumX;
            resY[index + 0] = resY[index + 1] = resY[index + 2] = sobelSumY;
        }
    }
}

// Takes an image's height and width and its X and Y gradients and populates the angleInfo and gradient (G) arrays
void GetGradientAndAngleInfo(int* gradX, int* gradY, int width, int height, int channels, 
    unsigned char* G, int* angleInfo) {
    int num0=0, num45=0, num90=0, num135=0;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int gradientIndex = (row * width + col) * channels;
            int angleInfoIndex = row * width + col;

            int Gx = gradX[gradientIndex];
            int Gy = gradY[gradientIndex];

            unsigned char GPixelValue = sqrt(pow(Gx, 2) + pow(Gy, 2));
            double angle = atan2(Gy, Gx);

            G[gradientIndex + 0] = GPixelValue;
            G[gradientIndex + 1] = GPixelValue;
            G[gradientIndex + 2] = GPixelValue;
            angle *= (180 / M_PI);

            if (angle < 0) angle += 180;

            int direction = -1;

            // NOTE: The following rounding code is generated by ChatGPT
            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5 && angle <= 180))
                direction = 0, num0++;
            else if (angle >= 22.5 && angle < 67.5)
                direction = 45, num45++;
            else if (angle >= 67.5 && angle < 112.5)
                direction = 90, num90++;
            else // angle in [112.5, 157.5)
                direction = 135, num135++;

            angleInfo[angleInfoIndex] = direction;
        }
    }
    std::cout << "Number of 0s: " << num0 << " Nummber of 45s: " << num45 << " Number of 90s: " << num90 << " Number of 135s: " << num135 << std::endl;
}

// Takes gradient of image G and angular information and applies non-maximum suppression on G
void NonMaximumSupression(unsigned char* G, unsigned char* NMS, int height, int width, int channels, int* angleInfo) {
    for (int row = 1; row < height - 1; row++) {
        for (int col = 1; col < width - 1; col++) {
            int gradientIndex = (row * width + col) * channels;
            int angleInfoIndex = row * width + col;

            // If the angle at that pixel is 0
            if (angleInfo[angleInfoIndex] == 0) {
                int neighbor1 = (row * width + (col + 1)) * channels;
                int neighbor2 = (row * width + (col - 1)) * channels;

                // If pixel is larger than those to left and right, count them in NMS
                if (G[gradientIndex] > G[neighbor1] && G[gradientIndex] > G[neighbor2]) {
                    NMS[gradientIndex + 0] = G[gradientIndex + 0];
                    NMS[gradientIndex + 1] = G[gradientIndex + 1];
                    NMS[gradientIndex + 2] = G[gradientIndex + 2];
                }
            }
            // If the angle at that pixel is 45
            else if (angleInfo[angleInfoIndex] == 45) {
                int neighbor1 = ((row - 1) * width + (col + 1)) * channels;
                int neighbor2 = ((row + 1) * width + (col - 1)) * channels;

                // If pixel is larger than those to top-right and bottom-right, count them in NMS
                if (G[gradientIndex] > G[neighbor1] && G[gradientIndex] > G[neighbor2]) {
                    NMS[gradientIndex + 0] = G[gradientIndex + 0];
                    NMS[gradientIndex + 1] = G[gradientIndex + 1];
                    NMS[gradientIndex + 2] = G[gradientIndex + 2];
                }
            }
            // If the angle at that pixel is 90
            else if (angleInfo[angleInfoIndex] == 90) {
                int neighbor1 = ((row + 1) * width) * channels;
                int neighbor2 = ((row - 1) * width) * channels;

                // If pixel is larger than those above and below it, count them in NMS
                if (G[gradientIndex] > G[neighbor1] && G[gradientIndex] > G[neighbor2]) {
                    NMS[gradientIndex + 0] = G[gradientIndex + 0];
                    NMS[gradientIndex + 1] = G[gradientIndex + 1];
                    NMS[gradientIndex + 2] = G[gradientIndex + 2];
                }
            }
            // If the angle at that pixel is 135
            else {
                int neighbor1 = ((row + 1) * width + (col + 1)) * channels;
                int neighbor2 = ((row - 1) * width + (col - 1)) * channels;

                // If pixel is larger than those to top-left and bottom-right, count them in NMS
                if (G[gradientIndex] > G[neighbor1] && G[gradientIndex] > G[neighbor2]) {
                    NMS[gradientIndex + 0] = G[gradientIndex + 0];
                    NMS[gradientIndex + 1] = G[gradientIndex + 1];
                    NMS[gradientIndex + 2] = G[gradientIndex + 2];
                }
            }
        }
    }
}

// Takes a black/white photo and its dimensions, compares each pixel (not on border) value to min/max.
// if the value is between min and max, it must check surrounding pixels and see if it's connected to a > max pixel
void DoubleThresholdAndHysteresis(unsigned char* original, unsigned char* result, double min, double max, int height, int width, int channels) {
    // check each non-edge pixel
    for (int row = 1; row < height - 1; row++) {
        for (int col = 1; col < width - 1; col++) {
            int index = (row * width + col) * channels;

            // if the pixel value is >= min but < max, only keep if a >max pixel is in its surrounding
            if (original[index] >= (int)(min * 255) && original[index] < (int)(max * 255)) {
                int maxFound = 0;
                for (int i = -1; i < 2; i++) {
                    for (int j = -1; j < 2; j++) {
                        int surroundIndex = ((row + i) * width + (col + j)) * channels;

                        if (original[surroundIndex] >= max) {
                            maxFound = 1;
                        }
                    }
                }

                // if a >max value was found, write the pixel to result
                if (maxFound == 1) {
                    result[index + 0] = 255;
                    result[index + 1] = 255;
                    result[index + 2] = 255;
                }
            }
            // if the pixel value is larger than max, write it to the result
            else if (original[index] >= (int)(max * 255)) {
                result[index + 0] = 255;
                result[index + 1] = 255;
                result[index + 2] = 255;
            }
        }
    }
}

// Takes an image file path and returns (?)
char* ImageToASCII(const char* inputFileName, int boxHeight, int boxWidth, double doubleThresholdMin, double doubleThresholdMax) {
    int width, height, channels;

    // Loads image and stores pixel info in the data char array
    unsigned char* data = stbi_load(inputFileName, &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "Loading image failed\n";
        return NULL;
    }

    //char ASCIIArt[(height / boxHeight) * (width / boxWidth)];
    char* ASCIIArt = (char *)malloc((height / boxHeight) * (width / boxWidth));
    int* SobelX = (int*)malloc(sizeof(int) * width * height * channels);
    int* SobelY = (int*)malloc(sizeof(int) * width * height * channels);

    // For each pixel, create a char to represent its gradient value and the angle
    int* angleInfo = (int*)malloc(sizeof(int) * (height * width));
    unsigned char* G = (unsigned char*)malloc(width * height * channels);
    unsigned char* NMS = (unsigned char*)malloc(width * height * channels);
    unsigned char* doubleThreshold = (unsigned char*)malloc(width * height * channels);

    std::cout << "Image loaded: " << width << "x" << height << " with " << channels << " channels\n";

    // Grayscale each pixel of the image
    GrayScaleImage(data, height, width, channels);

    GaussianFilter(data, height, width, channels);
    SobelOperator(data, height, width, channels, SobelX, SobelY);

    GetGradientAndAngleInfo(SobelX, SobelY, width, height, channels, G, angleInfo);

    NonMaximumSupression(G, NMS, height, width, channels, angleInfo);

    // set doubleThreshold values
    //double min = .65, max = .85;
    DoubleThresholdAndHysteresis(NMS, doubleThreshold, doubleThresholdMin, doubleThresholdMax, height, width, channels);

    PixelDataToASCII(doubleThreshold, ASCIIArt, height, width, channels, boxHeight, boxWidth);

    PrintASCIIArt(ASCIIArt, height, width, boxHeight, boxWidth);
    
    stbi_write_jpg(outputFileName, width, height, channels, data, 100);
    stbi_write_jpg(outputFileName1, width, height, channels, SobelX, 100);
    stbi_write_jpg(outputFileName2, width, height, channels, SobelY, 100);
    stbi_write_jpg(outputFileName3, width, height, channels, G, 100);
    stbi_write_jpg(outputFileName4, width, height, channels, NMS, 100);
    stbi_write_jpg(outputFileName5, width, height, channels, doubleThreshold, 100);

    // Free memory allocated for data
    stbi_image_free(data);
    free(SobelX);
    free(SobelY);
    free(angleInfo);
    free(G);
    free(NMS);
    free(doubleThreshold);

    return ASCIIArt;
}

int main(int argc, char *argv[]) {
    if (argc < 6) {
        std::cout << "Not enough arguments.\nArguments: \n  1. Path to your image\n  (2.) pixel height and (3.) width of each ASCII character";
        std::cout << " (meaning, your image will be split into blocks of height x width \n  blocks and one character will represent each block)";
        std::cout << ". It's recommended that the width is approximately half of the height,\n  since ASCII character slots are twice as tall as they are wide.";
        std::cout << "  A good default is a width of 6 and a height of 12.\n";
        std::cout << "  (4.) The low and (5.) high values of the double threshold step. The lower the values are, the more edges there will be,\n";
        std::cout << "  and the higher they are, the more strict the edge detection algorithm will be, meaning less edges.\n  Good defaults are low = .3 and high = .5\n";
        std::cout << "Sample input: ./ImageToASCII input.png 12 6 .3 .5";

        return 0;
    }
    const char* inputFileName = argv[1];
    //inputFileName = "./images/inspiration.png";

    int boxHeight = atoi(argv[2]);
    int boxWidth = atoi(argv[3]);
    double low = strtod(argv[4], NULL);
    double high = strtod(argv[5], NULL);

    ImageToASCII(inputFileName, boxHeight, boxWidth, low, high);

    return 0;
}
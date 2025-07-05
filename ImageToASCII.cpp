#include <stdio.h>
#include <iostream>

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

// boxHeight and boxWidth specify the number of pixels in the height and width of the box that an ASCII character will
// replace. Since chars are twice as tall as they are wide, it's best to set the height to twice the size of the width
int boxWidth = 3;
int boxHeight = 6;
const char* inputFileName = "lizard.jpg";
const char* outputFileName = "lizard gaussian.jpg";
const char* outputFileName1 = "lizard sobel X.jpg";
const char* outputFileName2 = "lizard sobel Y.jpg";
const char* outputFileName3 = "lizard gradient G.jpg";

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

int main() {
    int width, height, channels;

    // Loads image and stores pixel info in the data char array
    unsigned char* data = stbi_load(inputFileName, &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "Loading image failed\n";
        return 1;
    }


    int* SobelX = (int*)malloc(sizeof(int) * width * height * channels);
    int* SobelY = (int*)malloc(sizeof(int) * width * height * channels);

    // For each pixel, create a char to represent its gradient value and the angle
    int* angleInfo = (int*)malloc(sizeof(int) * (height * width));
    unsigned char* G = (unsigned char*)malloc(sizeof(double) * width * height * channels);

    std::cout << "Image loaded: " << width << "x" << height << " with " << channels << " channels\n";

    // Grayscale each pixel of the image
    GrayScaleImage(data, height, width, channels);

    //GaussianFilter(data, height, width, channels);
    SobelOperator(data, height, width, channels, SobelX, SobelY);

    GetGradientAndAngleInfo(SobelX, SobelY, width, height, channels, G, angleInfo);
    
    stbi_write_jpg(outputFileName, width, height, channels, data, 100);
    stbi_write_jpg(outputFileName1, width, height, channels, SobelX, 100);
    stbi_write_jpg(outputFileName2, width, height, channels, SobelY, 100);
    stbi_write_jpg(outputFileName3, width, height, channels, G, 100);

    // Free memory allocated for data
    stbi_image_free(data);
    stbi_image_free(SobelX);
    stbi_image_free(SobelY);
    free(angleInfo);
    free(G);

    return 0;
}
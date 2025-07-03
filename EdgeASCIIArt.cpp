#include <stdio.h>
#include <iostream>
#include "ImageToASCII.h"

#include "stb_image.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "stb_image_write.h"
#pragma clang diagnostic pop

// boxHeight and boxWidth specify the number of pixels in the height and width of the box that an ASCII character will
// replace. Since chars are twice as tall as they are wide, it's best to set the height to twice the size of the width
#define boxWidth 3
#define boxHeight 6
#define inputFileName "Please Please Me.png"
#define outputFileName "Please Please Me.jpg"

/*
int main(void) {
    int width, height, channels;

    // Loads image and stores pixel info in the data char array
    unsigned char* data = stbi_load(inputFileName, &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Loading image failed\n";
        return 1;
    }

    std::cout << "Image loaded: " << width << "x" << height << " with " << channels << " channels\n";

    GrayScaleImage(data, height, width, channels);

    stbi_write_jpg(outputFileName, width, height, channels, data, 100);

    return 0;
}*/
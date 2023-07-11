// MIT License
//
// Copyright (c) 2023 Ferhat Geçdoğan All Rights Reserved.
// Distributed under the terms of the MIT License.
//
//

/*
MIT License

Copyright (c) 2023 Ferhat Geçdoğan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <X11/Xlib.h>

typedef enum {
    JPG,
    JPEG,
    PNG,
    TGA,
    BMP
} fromatType;

void screenshot_to_stream(XImage* image_data,
                            const char* file_name,
                            fromatType file_format,
                            unsigned int width, 
                            unsigned int height) {

    if((width == 0) || (height == 0)) return;

    unsigned char arr[width * height * 3];
    
    unsigned long red_mask = image_data->red_mask;
    unsigned long green_mask = image_data->green_mask;
    unsigned long blue_mask = image_data->blue_mask;

    size_t width_index, height_index;

    for(width_index = 0; width_index < width; ++width_index) {
        for(height_index = 0; height_index < height; ++height_index) {
            unsigned long pixel = XGetPixel(image_data, width_index, height_index);

            arr[(width_index + width * height_index) * 3] = ((pixel & red_mask) >> 16);
            arr[(width_index + width * height_index) * 3 + 1] = ((pixel & green_mask) >> 8);
            arr[(width_index + width * height_index) * 3 + 2] = (pixel & blue_mask);
        }
    }  

    switch (file_format) {
        case JPG:
            stbi_write_jpg(file_name, width, height, 3, arr, width * 3);
            break;
        case PNG:
            stbi_write_png(file_name, width, height, 3, arr, width * 3);
            break;
        case TGA:
            stbi_write_tga(file_name, width, height, 3, arr);
            break;
        case BMP:
            stbi_write_bmp(file_name, width, height, 3, arr);
            break;
        default:
            stbi_write_jpg(file_name, width, height, 3, arr, width * 3);
            break;
    }
}
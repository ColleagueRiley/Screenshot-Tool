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
#include <stb_image_write.h>
#include <alloca.h>
#include <X11/Xlib.h>

unsigned char* screenshot_to_stream(XImage* image_data,
                            unsigned int x,
                            unsigned int y,
                            unsigned int width, 
                            unsigned int height) {

    if((width == 0) || (height == 0)) return;

    unsigned char* arr = alloca(sizeof(unsigned char) * width * height * 3);
    
    unsigned long red_mask = image_data->red_mask;
    unsigned long green_mask = image_data->green_mask;
    unsigned long blue_mask = image_data->blue_mask;

    size_t width_index, height_index;

    for(width_index = 0; width_index < width; width_index++) {
        for(height_index = 0; height_index < height; height_index++) {
            unsigned long pixel = XGetPixel(image_data, x + width_index, y + height_index);

            arr[(width_index + width * height_index) * 3] = ((pixel & red_mask) >> 16);
            arr[(width_index + width * height_index) * 3 + 1] = ((pixel & green_mask) >> 8);
            arr[(width_index + width * height_index) * 3 + 2] = (pixel & blue_mask);
        }
    }  

    return arr;
}
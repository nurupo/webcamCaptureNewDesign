/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H
#include <format.h>
#include <stdint.h>

namespace webcam_capture  {
    /**
     *  Contains video frame data.
     */
    struct PixelBuffer{
       /**
        * @param w Sets the width of the frame.
        * @param h Sets the height of the frame.
        * @param fmt Sets the pixel format of the frame.
        */
       void setup(int w, int h, Format fmt);

       /**
        *  When data is one continuous block of member you can use this, otherwise it points to the same location as plane[0].
        */
       uint8_t* pixels;

       /**
        * Pointers to the pixel data.
        * Planar pixel formats have all three pointers set, packet have only the first one set.
        */
       uint8_t* plane[3];

       /**
        * The number of bytes you should jump per row when reading the pixel data.
        * Note that some buffer may have extra bytse at the end for memory alignment.
        */
       size_t stride[3];

       /**
        * The width.
        * Planar pixel formats have all three pointers set, packet have only the first one set.
        */
       size_t width[3];

       /**
        * The height.
        * Planar pixel formats have all three pointers set, packet have only the first one set.
        */
       size_t height[3];

       /**
        * Planar pixel formats have the byte offsets from the first byte / plane set.
        */
       size_t offset[3];

       /**
        * The total number of bytes that make up the frame.
        * This doesn't have to be one continuous array when the pixel format is planar.
        */
       size_t nbytes;

       /**
        * The pixel format of the frame.
        */
       Format pixel_format;
    };

} // namespace webcam_capture

#endif // PIXEL_BUFFER_H


/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef FRAME_H
#define FRAME_H

#include <pixel_format.h>

#ifdef _WIN32
    #include <webcam_capture_export.h>
#elif __APPLE__
    //nothing to include
#endif

#include <cstdint>

namespace webcam_capture {
/**
 *  Provides video frame data.
 */
#ifdef _WIN32
    struct WEBCAM_CAPTURE_EXPORT Frame
#elif __APPLE__
    struct Frame
#endif
{

    /**
     * Pointers to the pixel data.
     * Planar pixel formats have all three pointers set, packet have only the first one set.
     */
    uint8_t *plane[3];

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
    size_t bytes;

    /**
     * The pixel format of the frame.
     */
    PixelFormat pixelFormat;
};

} // namespace webcam_capture

#endif // FRAME_H


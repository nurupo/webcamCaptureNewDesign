#ifndef IMAGE_FORMAT_CONVERTER
#define IMAGE_FORMAT_CONVERTER
#include <pixel_buffer.h>

namespace webcam_capture {

    /**
     * Handles conversion of pixel formats.
     */
    class ImageFormatConverter{
    public:
        /**
         * Converts a video frame to RGB pixel format.
         * @param Frame to convert.
         * @return RGB version of the frame.
         */
        static PixelBuffer convertToRGB(const PixelBuffer&);

        /**
         * Converts a video frame to YUV pixel format.
         * @param Frame to convert.
         * @return YUV version of the frame.
         */
        static PixelBuffer convertToYUV(const PixelBuffer&);

    private:
        ImageFormatConverter();
    };

} // namespace webcam_capture

#endif // IMAGE_FORMAT_CONVERTER


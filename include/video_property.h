#ifndef VIDEO_PROPERTY_H
#define VIDEO_PROPERTY_H

#include <webcam_capture_export.h>

namespace webcam_capture  {

/**
 *  Supported video properties.
 */
enum class WEBCAM_CAPTURE_EXPORT VideoProperty {
    Brightness,
    Contrast,
    Saturation
};

} // namespace webcam_capture

#endif // VIDEO_PROPERTY_H


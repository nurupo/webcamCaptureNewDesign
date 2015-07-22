#ifndef BACKEND_IMPLEMENTATION_H
#define BACKEND_IMPLEMENTATION_H

#include <webcam_capture_export.h>

namespace webcam_capture  {

/**
 * Supported video capture frameworks.
 */
enum class WEBCAM_CAPTURE_EXPORT BackendImplementation {
    MediaFoundation,
    DirectShow,
    v4l, //TODO to fix v4l name. (maybe it using v4l2???)
    AVFoundation
};

} // namespace webcam_capture


#endif // BACKEND_IMPLEMENTATION_H


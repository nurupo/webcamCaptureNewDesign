#ifndef BACKEND_IMPLEMENTATION_H
#define BACKEND_IMPLEMENTATION_H

namespace webcam_capture  {

    /**
     * Supported video capture frameworks.
     */
    enum class BackendImplementation {
        MediaFoundation,
        v4l,  //TODO to fix v4l name. (maybe it using v4l2???)
        AVFoundation
    };

} // namespace webcam_capture


#endif // BACKEND_IMPLEMENTATION_H


#include <backend_factory.h>

#ifdef WEBCAM_CAPTURE_BACKEND_MEDIA_FOUNDATION
#include "../src/media_foundation/media_foundation_backend.h"
#endif

#ifdef WEBCAM_CAPTURE_BACKEND_DIRECT_SHOW
#include "../src/direct_show/direct_show_backend.h"
#endif

#ifdef V4L
#endif

#ifdef AV_FOUNDATION
#endif


namespace webcam_capture {

std::unique_ptr<BackendInterface> BackendFactory::getBackend(BackendImplementation implementation)
{
    switch (implementation) {
#ifdef WEBCAM_CAPTURE_BACKEND_MEDIA_FOUNDATION

        case BackendImplementation::MediaFoundation: {
            return std::make_unique<MediaFoundation_Backend>();
        }

#endif

#ifdef WEBCAM_CAPTURE_BACKEND_DIRECT_SHOW

        case BackendImplementation::DirectShow: {
            return std::make_unique<DirectShow_Backend>();
        }

#endif

#ifdef V4L

        case BackendImplementation::v4l : {
            return std::make_unique<V4L_Backend>();
        }

#endif

#ifdef AV_FOUNDATION

        case BackendImplementation::AVFoundation : {
            return std::make_unique<AV_Foundation_Backend>();
        }

#endif

        default:
            return nullptr;
    }
}

std::vector<BackendImplementation> BackendFactory::getAvailableBackends()
{
    return {

#ifdef WEBCAM_CAPTURE_BACKEND_MEDIA_FOUNDATION
        BackendImplementation::MediaFoundation,
#endif

#ifdef WEBCAM_CAPTURE_BACKEND_DIRECT_SHOW
        BackendImplementation::DirectShow,
#endif

#ifdef V4L
        BackendImplementation::v4l,
#endif

#ifdef AV_FOUNDATION
        BackendImplementation::AVFoundation,
#endif

    };
}

} // namespace webcam_capture

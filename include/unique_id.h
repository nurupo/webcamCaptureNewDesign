#ifndef UNIQUE_ID
#define UNIQUE_ID
#include <backend_implementation.h>

namespace webcam_capture {

    /**
     * Uniquely identifies a camera.
     * While the actual data is hidden by the backend implementations, you can still use it for comparison.
     */
    class UniqueId {
    public:
        UniqueId(BackendImplementation implementation);
        virtual ~UniqueId();

        virtual bool operator==(const UniqueId& other);
        virtual bool operator!=(const UniqueId& other);

    protected:
        BackendImplementation implementation;
    };
}

#endif // UNIQUE_ID

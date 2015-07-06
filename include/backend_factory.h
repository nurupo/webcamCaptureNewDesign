#ifndef BACKEND_FACTORY
#define BACKEND_FACTORY
#include <backend_interface.h>
#include <backend_implementation.h>

namespace webcam_capture {

    /**
     * Provides access to backends and information of their availability.
     */
    class BackendFactory {
    public:
        /**
         * Creates a backend instance backed by a certain backend implementation.
         * You are responsible for deleting the returned object.
         * @param Implementation backing the backend.
         * @return BackentInterface instance backed by specified backend implementation on success, null on failure.
         */      
        static BackendInterface* getBackend(BackendImplementation implementation);

        /**
         * @return List of backends the library was built with support of.
         */
        static std::vector<BackendImplementation> getAvailableBackends();

    private:
        BackendFactory();
    };

} // namespace webcam_capture

#endif // BACKEND_FACTORY


#ifndef BACKEND_FACTORY_H
#define BACKEND_FACTORY_H

#include <backend_implementation.h>

#include <webcam_capture_export.h>

#include <memory>
#include <vector>

namespace webcam_capture {

class BackendInterface;

/**
 * Provides access to backends and information of their availability.
 */
class WEBCAM_CAPTURE_EXPORT BackendFactory
{
public:
    /**
     * Creates a backend instance backed by a certain backend implementation.
     * @param implementation Implementation backing the backend.
     * @return BackentInterface instance backed by specified backend implementation on success, null on failure.
     */
    static std::unique_ptr<BackendInterface> getBackend(BackendImplementation implementation);

    /**
     * @return List of backends the library was built with support of.
     */
    static std::vector<BackendImplementation> getAvailableBackends();

private:
    BackendFactory() = delete;
};

} // namespace webcam_capture

#endif // BACKEND_FACTORY_H

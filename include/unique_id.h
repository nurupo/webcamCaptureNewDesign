#ifndef UNIQUE_ID_H
#define UNIQUE_ID_H

#include <backend_implementation.h>

#include <webcam_capture_export.h>

namespace webcam_capture {

/**
 * Uniquely identifies a camera.
 * While the actual data is hidden by the backend implementations, you can still use it for comparison.
 */
class WEBCAM_CAPTURE_EXPORT UniqueId
{
public:
    virtual ~UniqueId();

    bool operator==(const UniqueId &other);
    bool operator!=(const UniqueId &other);

protected:
    UniqueId(BackendImplementation implementation);

    virtual bool equals(const UniqueId &other) const;

    const BackendImplementation implementation;

private:
    UniqueId &operator=(const UniqueId &) = delete;
    UniqueId(const UniqueId &) = delete;
};

} // namespace webcam_capture

#endif // UNIQUE_ID_H

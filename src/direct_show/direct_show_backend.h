#ifndef DIRECT_SHOW_BACKEND_H
#define DIRECT_SHOW_BACKEND_H

#include <string>
#include <vector>
#include <memory>               /* std::shared_ptr include */
#include <functional>

#include "../utils.h"
#include <backend_interface.h>
#include <camera_interface.h>
#include <camera_information.h>

#include "../winapi_shared/winapi_shared_camera_notifications.h"
//#include "media_foundation_utils.h"
//#include "media_foundation_callback.h"
//#include "media_foundation_camera_notifications.h"

namespace webcam_capture {

class DirectShow_Backend : public BackendInterface
{
public:
    DirectShow_Backend(); // creates std::shared_ptr<void*> mfDeinitializer
    ~DirectShow_Backend();
    static void DeinitBackend(void *);
    std::vector<CameraInformation> getAvailableCameras() const;
    std::unique_ptr<CameraInterface> getCamera(const CameraInformation &information) const;
    int setAvaliableCamerasChangedCallback(ConnectionStatusCallback callback);
private:
    std::shared_ptr<void> mfDeinitializer;
    WinapiShared_CameraNotifications notificationManager;
};
} // namespace webcam_capture

#endif

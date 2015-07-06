/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef BACKEND_INTERFACE_H
#define BACKEND_INTERFACE_H

#include <stdio.h>
#include <string>
#include <vector>
#include <functional>

#include <camera_interface.h>
#include <camera_information.h>

namespace webcam_capture {

    // FIXME(nurupo): rename, move into separate file?
    enum class CameraPlugStatus {
        CAMERA_CONNECTED,
        CAMERA_DISCONNECTED
    };

    // FIXME(nurupo): rename, fix formatting
    /**
    * notifications_callback Typedef for nnotifications callback
    */
    typedef std::function<void(CameraInformation* information, CameraPlugStatus status)> notifications_callback;

    /**
     * Common interface of backend implementations.
     * Provides access to cameras and information of their availability.
     */
    class BackendInterface {
    public:
        BackendInterface() {}
        virtual ~BackendInterface() {}

        /**
         * @return Short information about available cameras.
         */
        virtual std::vector<CameraInformation*> getAvailableCameras() const = 0;

        /**
         * Creates a camera instance representing a specific camera.
         * You are responsible for deleting the returned object.
         * Camera instance will function properly with its backend instance deleted.
         * Backend and camera instances should be created in the same thread and can be used only within the thread.
         * @param Previously received camera information for the camera you want to access.
         * @return CameraInterface instance representing a specific camera on success, null on failure.
         */
        virtual CameraInterface* getCamera(const CameraInformation &information) const = 0;

        /**
         * Sets a callback function that is be called when a camera is connected/disconencted to/from the system.
         * You can use the UniqueId member of CameraInformation for comparison against your camera instances in order
         * to check if any of them was removed from the system.
         * @param Callback function.
         * @return FIXME(nurupo): the return value should be wither an enum or bool.
         */
        virtual int setAvaliableCamerasChangedCallback(notifications_callback n_callback) = 0;
    };

} // namespace webcam_capture

#endif

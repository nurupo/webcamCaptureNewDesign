#ifndef WINAPI_SHARED_CAMERA_NOTIFICATIONS_H
#define WINAPI_SHARED_CAMERA_NOTIFICATIONS_H

#include <backend_interface.h>

#include <thread>

#include <Windows.h>
#include <dbt.h>

namespace webcam_capture {

class MediaFoundation_Backend;

class WinapiShared_CameraNotifications
{
public:
    WinapiShared_CameraNotifications(BackendImplementation implementation);
    ~WinapiShared_CameraNotifications();

    /**
     * Starts listening for device state change events.
     * @param Callback to call when device state changes.
     * @return true on success, false if already running (was previously stated but wasn't stopped).
     */
    bool start(ConnectionStatusCallback cb);

    /**
     * Stops listening for device state change events.
     * @return true on success, false if already stopped or never was started.
     */
    bool stop();

private:
    /**
     * Silently fails, preserving the internal state of the thread being running.
     */
    void MessageLoop();

    static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void CameraWasRemoved(DEV_BROADCAST_HDR *pHdr);
    void CameraWasConnected(DEV_BROADCAST_HDR *pHdr);


    BackendImplementation implementation;
    std::unique_ptr<BackendInterface> backend;
    std::vector<CameraInformation> devicesVector;
    HDEVNOTIFY hDevNotify;
    WNDCLASS windowClass;
    HWND messageWindow;

    bool threadIsRunning;
    ConnectionStatusCallback notif_cb;    //TODO to make threads synchronization
    std::thread messageLoopThread;
};
}

#endif // MEDIA_FONDATION_CAMERA_NOTIFICATIONS_H

#include "../utils.h"
#include "winapi_shared_camera_notifications.h"
#include "winapi_shared_unique_id.h"

#include <backend_factory.h>

#include <algorithm>

#include <ks.h>
#include <Ksmedia.h>
#include <wctype.h>

#define QUIT_FROM_NOTIFICATIONS_LOOP  0x01

namespace webcam_capture {

LRESULT CALLBACK WinapiShared_CameraNotifications::WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam,
        LPARAM lParam)
{
    WinapiShared_CameraNotifications *pThis;

    switch (uMsg) {
        // guaranteed to be called only once, at the startup
        case WM_CREATE: {
            pThis = static_cast<WinapiShared_CameraNotifications *>(reinterpret_cast<CREATESTRUCT *>(lParam)->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

            // initialize backend in new thread
            pThis->backend = BackendFactory::getBackend(pThis->implementation);
            if (!pThis->backend) {
                DEBUG_PRINT("Couldn't initialize backend.\n");
                break;
            }

            pThis->devicesVector = pThis->backend->getAvailableCameras();
            break;
        }

        // called whenever a device that belongs to a group we have previously
        // registered to receive notifications of changes its state
        case WM_DEVICECHANGE: {
            pThis = reinterpret_cast<WinapiShared_CameraNotifications *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

            if (!pThis) {
                DEBUG_PRINT("Couldn't get either this pointer or backend.\n");
                break;
            }

            if (!pThis->backend) {
                DEBUG_PRINT("Backend is still not initialized.\n");
                break;
            }

            if (wParam == DBT_DEVICEARRIVAL) {
                DEBUG_PRINT("A webcam device has been inserted and is now available.\n");
                pThis->CameraWasConnected(reinterpret_cast<PDEV_BROADCAST_HDR>(lParam));
            }

            if (wParam == DBT_DEVICEREMOVECOMPLETE) {
                DEBUG_PRINT("A webcam device has been removed.\n");
                pThis->CameraWasRemoved(reinterpret_cast<PDEV_BROADCAST_HDR>(lParam));
            }

            break;
        }

        // called by us when WinapiShared_CameraNotifications::stop is called,
        // the rest of void WinapiShared_CameraNotifications::MessageLoop is being executed after we PostQuitMessage.
        case WM_USER: {
            if (lParam == QUIT_FROM_NOTIFICATIONS_LOOP) {
                DEBUG_PRINT("Received request to stop the loop.\n");
                PostQuitMessage(0);
                break;
            }
        }

        // guaranteed to be called only once, at the end of lifecycle when we call DestroyWindow on the window
        case WM_DESTROY: {
            pThis = reinterpret_cast<WinapiShared_CameraNotifications *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

            pThis->backend.reset();

            DEBUG_PRINT("Received destroy message.\n");

            break;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

WinapiShared_CameraNotifications::WinapiShared_CameraNotifications(BackendImplementation implementation)
    : implementation(implementation),
      backend(nullptr),
      notif_cb(nullptr),
      threadIsRunning(false)
{    
}

WinapiShared_CameraNotifications::~WinapiShared_CameraNotifications()
{
    stop();
}

bool WinapiShared_CameraNotifications::start(ConnectionStatusCallback cb)
{
    if (threadIsRunning) {
        return false;
    }

    notif_cb = cb;
    messageLoopThread = std::thread(&WinapiShared_CameraNotifications::MessageLoop, this);
    threadIsRunning = true;
    DEBUG_PRINT("Notifications capturing was started.\n");

    return true;
}

bool WinapiShared_CameraNotifications::stop()
{
    if (!threadIsRunning) {
        return false;
    }

    //TODO executes 2 times - to fix this.
    SendMessage(messageWindow, WM_USER, 0, QUIT_FROM_NOTIFICATIONS_LOOP);

    if (messageLoopThread.joinable()) {
        messageLoopThread.join();
    }

    threadIsRunning = false;

    devicesVector.clear();

    DEBUG_PRINT("Notifications capturing was stopped.\n");

    return true;
}

void WinapiShared_CameraNotifications::MessageLoop()
{
    windowClass = {};
    windowClass.lpfnWndProc = &WinapiShared_CameraNotifications::WindowProcedure;

    LPCSTR windowClassName = "CameraNotificationsMessageOnlyWindow";
    windowClass.lpszClassName = windowClassName;

    if (!RegisterClass(&windowClass)) {
        DEBUG_PRINT("Failed to register window class.\n");
        goto done;
    }

    messageWindow = CreateWindow(windowClassName, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, this);

    if (!messageWindow) {
        DEBUG_PRINT("Failed to create message-only window.\n");
        goto unregister_window_class;
    }

    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));

    NotificationFilter.dbcc_size = sizeof(NotificationFilter);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    //can be used KSCATEGORY_CAPTURE. But KSCATEGORY_CAPTURE - gets audio/video messages device arrival and removed (2 messages)
    NotificationFilter.dbcc_classguid = KSCATEGORY_VIDEO;


    hDevNotify = RegisterDeviceNotificationW(messageWindow, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

    if (!hDevNotify) {
        DEBUG_PRINT("Failed to register for device notifications.\n");
        goto destroy_window;
    }

    MSG msg;

    while (GetMessage(&msg, messageWindow, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

unregister_notifications:
    UnregisterDeviceNotification(hDevNotify);

destroy_window:
    DestroyWindow(messageWindow);

unregister_window_class:
    UnregisterClass(windowClass.lpszClassName, NULL);

done:
    notif_cb = nullptr;

    DEBUG_PRINT("MessageLoop exited.\n");
}

void WinapiShared_CameraNotifications::CameraWasRemoved(DEV_BROADCAST_HDR *pHdr)
{
    if (pHdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE) {
        DEBUG_PRINT("Unexpected device type.\n");
        return;
    }

    //FIXME(nurupo): use _W version of the struct
//    if (pHdr->dbch_size != sizeof(DEV_BROADCAST_DEVICEINTERFACE_W)) {
//        DEBUG_PRINT("Unexpected struct.\n");
//        return;
//    }

    DEV_BROADCAST_DEVICEINTERFACE *pDi = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(pHdr);
    size_t nameLen = strlen(pDi->dbcc_name);
    std::unique_ptr<WCHAR[]> name(new WCHAR[nameLen]);
    mbstowcs(name.get(), pDi->dbcc_name, nameLen);
    WinapiShared_UniqueId uniqId(name.get(), implementation);

    for (int i = 0; i < devicesVector.size(); i++) {
        if (uniqId == *devicesVector.at(i).getUniqueId()) {
            notif_cb(devicesVector.at(i), CameraConnectionStatus::Disconnected);
            devicesVector.erase(devicesVector.begin() + i);
            break;
        }
    }
}

void WinapiShared_CameraNotifications::CameraWasConnected(DEV_BROADCAST_HDR *pHdr)
{
    if (pHdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE) {
        DEBUG_PRINT("Unexpected device type.\n");
        return;
    }

    //FIXME(nurupo): use _W version of the struct
//    if (pHdr->dbch_size != sizeof(DEV_BROADCAST_DEVICEINTERFACE_W)) {
//        DEBUG_PRINT("Unexpected struct.\n");
//        return;
//    }

    DEV_BROADCAST_DEVICEINTERFACE *pDi = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(pHdr);
    size_t nameLen = strlen(pDi->dbcc_name);
    std::unique_ptr<WCHAR[]> name(new WCHAR[nameLen]);
    mbstowcs(name.get(), pDi->dbcc_name, nameLen);
    WinapiShared_UniqueId uniqId(name.get(), implementation);

    std::vector<CameraInformation> camerasBuf = backend->getAvailableCameras();

    for (auto&& cameraInfo : camerasBuf) {
        if (*cameraInfo.getUniqueId() == uniqId) {
            notif_cb(cameraInfo, CameraConnectionStatus::Connected);
            devicesVector.push_back(cameraInfo);
            break;
        }
    }
}

} // namespace webcam_capture


/**
    This source is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#include "media_foundation_backend.h"

#include "../utils.h"
#include "../winapi_shared/winapi_shared_unique_id.h"
#include "media_foundation_camera.h"
#include "media_foundation_utils.h"

#include <string>

#include <windows.h>
#include <mfapi.h>
#include <mferror.h>            /* MediaFoundation error codes, MF_E_* */
#include <mfidl.h>              /* e.g. MFEnumDeviceSources */
#include <mfplay.h>
#include <mfreadwrite.h>
#include <shlwapi.h>

namespace webcam_capture {

MediaFoundation_Backend::MediaFoundation_Backend()
    : BackendInterface(BackendImplementation::MediaFoundation),
      mfDeinitializer(this, MediaFoundation_Backend::DeinitBackend),
      notificationManager(BackendImplementation::MediaFoundation)
{
    // Initialize COM
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot intialize COM.\n");  //or The COM is already initialized.
    }

    // Initialize MediaFoundation
    hr = MFStartup(MF_VERSION);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot start the Media Foundation.\n");
    }
}

void MediaFoundation_Backend::DeinitBackend(void *)
{
    /* Shutdown MediaFoundation */
    HRESULT hr = MFShutdown();

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: failed to shutdown the MediaFoundation.\n");
    }

    /* Shutdown COM */
    CoUninitialize();
    DEBUG_PRINT("MediaFoundation_Backend Successfully deinited.\n");
}

std::vector<CameraInformation> MediaFoundation_Backend::getAvailableCameras() const
{

    std::vector<CameraInformation> result;
    UINT32 count = 0;
    IMFAttributes *config = NULL;
    IMFActivate **devices = NULL;

    HRESULT hr = MFCreateAttributes(&config, 1);

    if (FAILED(hr)) {
        goto done;
    }

    // Filter capture devices.
    hr = config->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,  MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);

    if (FAILED(hr)) {
        goto done;
    }

    // Enumerate devices
    hr = MFEnumDeviceSources(config, &devices, &count);

    if (FAILED(hr)) {
        goto done;
    }

    if (count == 0) {
        goto done;
    }

    for (DWORD i = 0; i < count; ++i) {
        HRESULT hr1 = S_OK;
        HRESULT hr2 = S_OK;
        WCHAR *friendly_name = NULL;
        WCHAR *symbolic_link = NULL;
        UINT32 friendly_name_len = 0;
        UINT32 symbolic_link_len = 0;

        hr1 = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &friendly_name, &friendly_name_len);
        hr2 = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                                             &symbolic_link, &symbolic_link_len);

        if (SUCCEEDED(hr1) && SUCCEEDED(hr2)) {
            std::string name = string_cast<std::string>(friendly_name);
            result.push_back({std::make_shared<WinapiShared_UniqueId>(symbolic_link, BackendImplementation::MediaFoundation),
                              name});
        }

        CoTaskMemFree(symbolic_link);
        CoTaskMemFree(friendly_name);
    }

done:
    safeReleaseMediaFoundation(&config);

    for (DWORD i = 0; i < count; ++i) {
        safeReleaseMediaFoundation(&devices[i]);
    }

    CoTaskMemFree(devices);

    return result;
}

std::unique_ptr<webcam_capture::CameraInterface> MediaFoundation_Backend::getCamera(const CameraInformation &information) const
{
    return MediaFoundation_Camera::createCamera(mfDeinitializer, information);
}

int MediaFoundation_Backend::setAvaliableCamerasChangedCallback(ConnectionStatusCallback callback)
{
    //IF n_callback is null_ptr or n_callback function is empty
    if (!callback) {
        notificationManager.stop();
        return -1;      //TODO Err code
    }

    notificationManager.start(callback);
    return 1; //TODO ERR code (success)
}

} // namespace webcam_capture

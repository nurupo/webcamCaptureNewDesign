#include <windows.h>
#include <dshow.h>

#include "direct_show_backend.h"
#include "direct_show_camera.h"
#include "../winapi_shared/winapi_shared_unique_id.h"

namespace webcam_capture {

DirectShow_Backend::DirectShow_Backend()
    : BackendInterface(BackendImplementation::DirectShow),
      mfDeinitializer(this, DirectShow_Backend::DeinitBackend),
      notificationManager(BackendImplementation::DirectShow)
{
    // Initialize COM
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot intialize COM.\n");  //or The COM is already initialized.
    }
}

DirectShow_Backend::~DirectShow_Backend()
{
}

void DirectShow_Backend::DeinitBackend(void *)
{
    /* Shutdown COM */
    CoUninitialize();
    DEBUG_PRINT("DirectShow_Backend Successfully deinited.\n");
}

std::vector<CameraInformation> DirectShow_Backend::getAvailableCameras() const
{

    std::vector<CameraInformation> result;

    IEnumMoniker *pEnum;
    ICreateDevEnum *pDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
                                  CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

    if (FAILED(hr)) {
        DEBUG_PRINT("Error during CoCreateInstance.\n");
        return result;
    }

    // Create an enumerator for the category Video capture devices.
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
    if (hr == S_FALSE) {
        DEBUG_PRINT("There is no available devices.\n");
        hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
    }
    pDevEnum->Release();

    IMoniker *pMoniker = NULL;
    while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {
        IPropertyBag *pPropBag;
        HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
        if (FAILED(hr)) {
            pMoniker->Release();
            continue;
        }
        VARIANT var;
        VariantInit(&var);


        // Get description or friendly name.
        hr = pPropBag->Read(L"Description", &var, 0);
        if (FAILED(hr)) {
            hr = pPropBag->Read(L"FriendlyName", &var, 0);
        }

        std::wstring wname;
        if (SUCCEEDED(hr)) {
            wname = var.bstrVal;
            VariantClear(&var);
        }
        hr = pPropBag->Read(L"DevicePath", &var, 0);
        if (SUCCEEDED(hr)) {
            /// The device path is not intended for display.
            result.push_back({std::make_shared<WinapiShared_UniqueId>(var.bstrVal, BackendImplementation::DirectShow),
                              {wname.begin(), wname.end()}});
            VariantClear(&var);
        }

        pPropBag->Release();
        pMoniker->Release();
    }
    pEnum->Release();

    return result;
}

std::unique_ptr<CameraInterface> DirectShow_Backend::getCamera(const CameraInformation &information) const
{    
    return DirectShow_Camera::createCamera(mfDeinitializer, information);
}

int DirectShow_Backend::setAvaliableCamerasChangedCallback(ConnectionStatusCallback callback)
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

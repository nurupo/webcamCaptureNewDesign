#ifndef DIRECT_SHOW_CALLBACK_H
#define DIRECT_SHOW_CALLBACK_H

#include <windows.h>
#include "sample_grabber.h"

namespace webcam_capture {

class DirectShow_Camera;

class DirectShow_Callback : public ISampleGrabberCB
{
public:
    DirectShow_Callback (DirectShow_Camera *cam); //in the future - to send DirectShow_Camera object

    STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {
        if (NULL == ppv) return E_POINTER;
        if (riid == __uuidof(IUnknown)) {
            *ppv = static_cast<IUnknown*>(this);
             return S_OK;
        }
        if (riid == __uuidof(ISampleGrabberCB))   {
            *ppv = static_cast<ISampleGrabberCB*>(this);
             return S_OK;
        }
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() {    return S_OK;  }
    STDMETHODIMP_(ULONG) Release() {   return S_OK;  }

    //ISampleGrabberCB
    virtual STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample);
    STDMETHODIMP BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen) { return S_OK; }

private:
    virtual ~DirectShow_Callback() {}

private:
    DirectShow_Camera *ds_camera;
};
} // namespace webcam_capture


#endif

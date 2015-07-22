#include "direct_show_callback.h"
#include "direct_show_camera.h"

namespace webcam_capture {

DirectShow_Callback::DirectShow_Callback(DirectShow_Camera *cam) :
    ds_camera(cam)
{
}

STDMETHODIMP DirectShow_Callback::SampleCB(double SampleTime, IMediaSample *pSample) {
    BYTE            *sampleBuffer;
    pSample->GetPointer(&sampleBuffer);
    ds_camera->frame.nbytes = pSample->GetActualDataLength();
    ds_camera->frame.plane[0] = sampleBuffer;
    ds_camera->cb_frame(ds_camera->frame);

    return S_OK;
}

} // namespace webcam_capture

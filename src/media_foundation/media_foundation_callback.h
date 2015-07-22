/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef MEDIA_FOUNDATION_CALLBACK_H
#define MEDIA_FOUNDATION_CALLBACK_H

#include <windows.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <shlwapi.h>


namespace webcam_capture {

class MediaFoundation_Camera;

class MediaFoundation_Callback : public IMFSourceReaderCallback
{
public:
    static bool createInstance(MediaFoundation_Camera *cam, MediaFoundation_Callback **cb);

    STDMETHODIMP QueryInterface(REFIID iid, void **v);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    STDMETHODIMP OnReadSample(HRESULT hr, DWORD streamIndex, DWORD streamFlags, LONGLONG timestamp, IMFSample *sample);
    STDMETHODIMP OnEvent(DWORD streamIndex, IMFMediaEvent *event);
    STDMETHODIMP OnFlush(DWORD streamIndex);

    HRESULT Wait(DWORD *streamFlags, LONGLONG *timestamp, IMFSample *sample)
    {
        return S_OK;
    }
    HRESULT Cancel()
    {
        return S_OK;
    }

private:
    MediaFoundation_Callback(MediaFoundation_Camera *cam);
    virtual ~MediaFoundation_Callback();

private:
    MediaFoundation_Camera *cam;
    long ref_count;
    CRITICAL_SECTION crit_sec;
};
} // namespace webcam_capture


#endif

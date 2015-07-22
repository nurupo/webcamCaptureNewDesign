/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef MEDIA_FOUNDATION_CAMERA_H
#define MEDIA_FOUNDATION_CAMERA_H

/* States (may be be used by implementations) */
#define CA_STATE_NONE 0x00                                                         /* Default state */
#define CA_STATE_CAPTURING 0x01                                                   /* The user started captureing */

#include <windows.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfidl.h>                                                                   /* e.g. MFEnumDeviceSources */
#include <mfreadwrite.h>
#include <mferror.h>                                                                 /* MediaFoundation error codes, MF_E_*  */
#include <shlwapi.h>
#include <string>
#include <vector>
#include <memory>  //std::shared_ptr include

#include "../utils.h"
#include "../include/camera_interface.h"
#include "../include/camera_information.h"
#include "../include/capability.h"
#include "../include/frame.h"
#include "../include/video_property.h"
#include "../include/video_property_range.h"
#include "media_foundation_utils.h"
#include "media_foundation_callback.h"

namespace webcam_capture {

class MediaFoundation_Camera : public CameraInterface
{
public:
    ~MediaFoundation_Camera();
    static std::unique_ptr<CameraInterface> createCamera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information);

    int start(PixelFormat pixelFormat, int width, int height, float fps, FrameCallback cb);
    int stop();
    std::unique_ptr<Frame> CaptureFrame();  //TODO
    // ---- Capabilities ----
    bool getPropertyRange(VideoProperty property, VideoPropertyRange &videoPropRange);
    int getProperty(VideoProperty property);
    bool setProperty(const VideoProperty property, const int value);
    std::vector<CapabilityFormat> getCapabilities();

private:
    MediaFoundation_Camera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information,
                           IMFMediaSource *mediaSource);

    //// SDK functions
    int createSourceReader(IMFMediaSource *mediaSource, IMFSourceReaderCallback *callback,
                           IMFSourceReader **sourceReader) const;
    static int createVideoDeviceSource(const int device, IMFMediaSource **source); //TODO outdated method - to remove
    static int createVideoDeviceSource(const std::wstring &pszSymbolicLink, IMFMediaSource **ppSource);
    int getVideoCapabilities(IMFMediaSource *source, std::vector<CapabilityFormat> &capFormatVector) const;
    int setDeviceFormat(IMFMediaSource *source, const int width, const int height, const PixelFormat pixelFormat,
                        const float fps) const;
    int setReaderFormat(IMFSourceReader *reader, const int width, const int height, const PixelFormat pixelFormat, const float fps) const;

public:
    std::shared_ptr<void> mfDeinitializer;
    Frame frame;
    MediaFoundation_Callback *mf_callback;
    IMFMediaSource *imf_media_source;
    IMFSourceReader *imf_source_reader;
    int state;
    CameraInformation information;
    FrameCallback cb_frame;
};

} // namespace webcam_capture
#endif

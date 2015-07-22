#ifndef DIRECT_SHOW_CAMERA_H
#define DIRECT_SHOW_CAMERA_H

/* States (may be be used by implementations) */
#define CA_STATE_NONE 0x00          /* Default state */
#define CA_STATE_CAPTURING 0x01     /* The user started captureing */

#include <windows.h>
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
//#include "media_foundation_utils.h"
#include "direct_show_callback.h"

namespace webcam_capture {

class DirectShow_Camera : public CameraInterface
{
public:
    ~DirectShow_Camera();
    static std::unique_ptr<CameraInterface> createCamera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information);

    int start(PixelFormat pixelFormat, int width, int height, float fps, FrameCallback cb);
    int stop();
    std::unique_ptr<Frame> CaptureFrame();
    // ---- Capabilities ----
    bool getPropertyRange(VideoProperty property, VideoPropertyRange &videoPropRange);
    int getProperty(VideoProperty property);
    bool setProperty(const VideoProperty property, const int value);
    std::vector<CapabilityFormat> getCapabilities();

private:
    DirectShow_Camera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information);

    /***** SDK FUNCTIONS *****/
    IMoniker* getIMonikerByUniqueId(std::shared_ptr<UniqueId> &uniqueId);    
    int setCapabilities(ICaptureGraphBuilder2 *pBuild, IBaseFilter *pBaseFilter, PixelFormat pixelFormat, int width, int height, float fps);

public:
    std::shared_ptr<void> mfDeinitializer;
    Frame frame;
    DirectShow_Callback *ds_callback;
    int state;
    CameraInformation information;

    IMoniker *pVideoSel;
    IBaseFilter *pVCap;
    ICaptureGraphBuilder2 *pBuild;
    IGraphBuilder *pGraph;
    IMediaControl *pControl;
    IMediaEvent *pEvent;
    IBaseFilter *pGrabberFilter;
    ISampleGrabber *pSampleGrabber;
    ISampleGrabberCB *pGrabberCB;
    IBaseFilter *pNullRenderer;

    FrameCallback cb_frame;
};

} // namespace webcam_capture
#endif

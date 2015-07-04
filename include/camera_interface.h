/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef CAMERA_INTERFACE_H
#define CAMERA_INTERFACE_H

#include <functional>
#include <memory>
#include <vector>

#include <camera_information.h>
#include <capability.h>
#include <pixel_buffer.h>
#include <video_property.h>
#include <video_property_range.h>
#include <webcam_capture_export.h>

namespace webcam_capture {
    /**
     * frame_callback Typedef for frames callback
     */
    typedef std::function<void(PixelBuffer& buffer)> frame_callback;

    /**
     * Contains Interface for Camera realization
     */
    class WEBCAM_CAPTURE_EXPORT CameraInterface {
    public:
        CameraInterface() {}
        virtual ~CameraInterface() {}

        /**
         * start Start capturing. Captured frames thows in frame callback
         * @return Status code
         */
        virtual int start(const CapabilityFormat &capabilityFormat, const CapabilityResolution &capabilityResolution,
                          const CapabilityFps &capabilityFps, frame_callback cb) = 0;
        /**
         * stop Stop capturing
         * @return Status code
         */
        virtual int stop() = 0;         //TODO to add enum with error codes
        /**
         * CaptureFrame Capture one single frame
         * @return Captured frame
         */
        virtual PixelBuffer* CaptureFrame() = 0;  //TODO

        // ----Capabilities----
        /**
         * @param property Property is one of the VideoPropery enum
         * @return Video property range
         */
        virtual bool getPropertyRange(VideoProperty property, VideoPropertyRange * videoPropRange) = 0; // TODO

        /**
         * @param property Property is one of the VideoPropery enum
         * @return Property current value
         */
        virtual int getProperty(VideoProperty property) = 0; //TODO       //TODO to add enum with error codes

        /**
         * @param property Property is one of the VideoPropery enum
         * @param value Property value
         * @return Success status
         */
        virtual bool setProperty(const VideoProperty property,const int value) = 0; //TODO

        /**
         * @return Capabilities vector
         */
        virtual std::vector<CapabilityFormat> getCapabilities() = 0;
    };

} // namespace webcam_capture

#endif

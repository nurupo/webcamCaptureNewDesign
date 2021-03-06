#ifndef MEDIA_FOUNDATION_PIXEL_FORMAT_TRANSFORM_H
#define MEDIA_FOUNDATION_PIXEL_FORMAT_TRANSFORM_H

#include <atlbase.h>
#include <memory>
#include <pixel_format.h>
#include <windows.h>

/**
 * A simple wrapper for IMFTransforms that do only pixel format conversion.
 * It can be further modified to account for resolution conversions and so on, if needed.
 * This class contains general funationality pixel format converting IMFTransforms would need to preform,
 * so that it would be repeated in every IMFTransform wrapper, and as such this class is supposed to be subclassed
 * by particular IMFTransform wrappers.
 */

class IMFTransform;
class IMFSample;
class IMFMediaBuffer;

struct IMFMediaType;

namespace webcam_capture {

class MediaFoundation_PixelFormatTransform
{
public:

    enum class RESULT {
        OK, // success
        UNSUPPORTED_INPUT, // no conversion from the input pixel format is supported
        UNSUPPORTED_OUTPUT_FOR_INPUT, // conversion from the input pixel format is supported, but not to that particular output pixel format
        FAILURE // something else failed
    };

    virtual ~MediaFoundation_PixelFormatTransform();

    /**
     * Preforms the pixel format conversion.
     * @param inputSample Input sample to convert.
     * @param outputSample Pointer to where the converted sample will be stored. You must not release the sample.
     * @return true on success, false on failure.
     */
    bool convert(IMFSample *inputSample, IMFSample **outputSample);

protected:
    static std::unique_ptr<MediaFoundation_PixelFormatTransform> getInstance(CComPtr<IMFTransform> &transform, int width, int height, PixelFormat inputPixelFormat, PixelFormat outputPixelFormat, RESULT &result);
    MediaFoundation_PixelFormatTransform(MediaFoundation_PixelFormatTransform &&other);

private:

    enum class PRIVATE_RESULT {
        OK,
        UNSUPPORTED_PIXEL_FORMAT,
        FAILURE
    };

    MediaFoundation_PixelFormatTransform(CComPtr<IMFTransform> &transform, DWORD inputStreamId, DWORD outputStreamId, bool weManageAllocation, CComPtr<IMFSample> &outputSample, CComPtr<IMFMediaBuffer> &outputSampleBuffer);

    static void getStreamIds(IMFTransform *transform, DWORD &inputStreamId, DWORD &outputStreamId);
    static PRIVATE_RESULT getSubtypeForPixelFormat(IMFTransform *transform, HRESULT (IMFTransform::*getAvailableType)(DWORD, DWORD, IMFMediaType**), PixelFormat pixelFormat, DWORD streamId, GUID &subtype);
    static PRIVATE_RESULT setSubtypeMediaType(IMFTransform *transform, HRESULT (IMFTransform::*setType)(DWORD, IMFMediaType*, DWORD), int width, int height, DWORD streamId, const GUID &subtype);
    static void releaseSampleBuffers(IMFSample* sample);

    CComPtr<IMFTransform> transform;
    DWORD inputStreamId;
    DWORD outputStreamId;
    bool weAllocateOutputSample;
    CComPtr<IMFSample> outputSample;
    CComPtr<IMFMediaBuffer> outputSampleBuffer;
};

} // namespace webcam_capture

#endif // MEDIA_FOUNDATION_PIXEL_FORMAT_TRANSFORM_H

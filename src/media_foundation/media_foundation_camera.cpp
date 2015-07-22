#include "../capability_tree_builder.h"
#include "media_foundation_camera.h"
#include "../winapi_shared/winapi_shared_unique_id.h"

namespace webcam_capture {

MediaFoundation_Camera::MediaFoundation_Camera(std::shared_ptr<void> mfDeinitializer,
        const CameraInformation &information, IMFMediaSource *mediaSource)
    : information(information)
    , mfDeinitializer(mfDeinitializer)
    , state(CA_STATE_NONE)
    , imf_media_source(mediaSource)
    , mf_callback(NULL)
    , imf_source_reader(NULL)
{

}

std::unique_ptr<CameraInterface> MediaFoundation_Camera::createCamera(std::shared_ptr<void> mfDeinitializer,
        const CameraInformation &information)
{
    IMFMediaSource *mediaSource = NULL;
    // Create the MediaSource

    const std::wstring& symbolicLink = static_cast<WinapiShared_UniqueId *>(information.getUniqueId().get())->getId();

    if (MediaFoundation_Camera::createVideoDeviceSource(symbolicLink, &mediaSource) < 0) {
        DEBUG_PRINT("Error: cannot create the media device source.\n");
        return nullptr;
    }

    // TODO(nurupo): get std::make_unique to work without makeing code uglier
    return std::unique_ptr<MediaFoundation_Camera>(new MediaFoundation_Camera(mfDeinitializer, information, mediaSource));
}

MediaFoundation_Camera::~MediaFoundation_Camera()
{
    // Stop capturing
    if (state & CA_STATE_CAPTURING) {
        stop();
    }

    //Release mediaSource
    safeReleaseMediaFoundation(&imf_media_source);
}

int MediaFoundation_Camera::start(PixelFormat pixelFormat,
                                  int width,
                                  int height, float fps,
                                  FrameCallback cb)
{
    if (!cb) {
        DEBUG_PRINT("Error: The callback function is empty. Capturing was not started.\n");
        return -1;      //TODO Err code
    }

    if (state & CA_STATE_CAPTURING) {
        DEBUG_PRINT("Error: cannot start capture because we are already capturing.\n");
        return -2;      //TODO Err code
    }

    cb_frame = cb;

///WTF the currentFpsBuf variable int setDeviceFormat and in setReaderFormat shows that
/// FPS is 3000 1 time and second time 500 - in real time - it doesn't changes
/// Looks like if you want to change FPS - we need to delete imm_media_source and create it again
    //"to test just comment this"
    safeReleaseMediaFoundation(&imf_media_source);
    // Create the MediaSource
    const std::wstring& symbolicLink = static_cast<WinapiShared_UniqueId *>(information.getUniqueId().get())->getId();

    if (createVideoDeviceSource(symbolicLink, &imf_media_source) < 0) {
        DEBUG_PRINT("Error: cannot create the media device source.\n");
        return NULL;
    }

    //End of "to test comment this"

    if (pixelFormat == PixelFormat::UNKNOWN) {
        DEBUG_PRINT("Error: cannot set a pixel format for UNKNOWN.\n");
        return -8;      //TODO Err code
    }

    if (setDeviceFormat(imf_media_source, width,
                        height,
                        pixelFormat,
                        fps) < 0) {
        DEBUG_PRINT("Error: cannot set the device format.\n");
        return -9;      //TODO Err code
    }

    // Create the source reader.
    MediaFoundation_Callback::createInstance(this, &mf_callback);

    if (createSourceReader(imf_media_source, mf_callback, &imf_source_reader) < 0) {
        DEBUG_PRINT("Error: cannot create the source reader.\n");
        safeReleaseMediaFoundation(&mf_callback);
        return -10;      //TODO Err code
    }

    // Set the source reader format.
    if (setReaderFormat(imf_source_reader, width,
                        height,
                        pixelFormat,
                        fps) < 0) {
        DEBUG_PRINT("Error: cannot set the reader format.\n");
        safeReleaseMediaFoundation(&mf_callback);
        safeReleaseMediaFoundation(&imf_source_reader);
        return -11;      //TODO Err code
    }

    frame.width[0] = width;
    frame.height[0] = height;
    frame.pixel_format = pixelFormat;

    // Kick off the capture stream.
    HRESULT hr = imf_source_reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);

    if (FAILED(hr)) {
        if (hr == MF_E_INVALIDREQUEST) {
            DEBUG_PRINT("ReadSample returned MF_E_INVALIDREQUEST.\n");
        } else if (hr == MF_E_INVALIDSTREAMNUMBER) {
            DEBUG_PRINT("ReadSample returned MF_E_INVALIDSTREAMNUMBER.\n");
        } else if (hr == MF_E_NOTACCEPTING) {
            DEBUG_PRINT("ReadSample returned MF_E_NOTACCEPTING.\n");
        } else if (hr == E_INVALIDARG) {
            DEBUG_PRINT("ReadSample returned E_INVALIDARG.\n");
        } else if (hr == E_POINTER) {
            DEBUG_PRINT("ReadSample returned E_POINTER.\n");
        } else {
            DEBUG_PRINT("ReadSample - unhandled result.\n");
        }

        DEBUG_PRINT("Error: while trying to ReadSample() on the imf_source_reader. \n");
        return -4;      //TODO Err code
    }

    state |= CA_STATE_CAPTURING;
    return 1;      //TODO Err code
}

int MediaFoundation_Camera::stop()
{
    if (!state & CA_STATE_CAPTURING) {
        DEBUG_PRINT("Error: Cannot stop capture because we're not capturing yet.\n");
        return -1;    //TODO Err code
    }

    if (!imf_source_reader) {
        DEBUG_PRINT("Error: Cannot stop capture because sourceReader is empty yet.\n");
        return -2;    //TODO Err code
    }

    state &= ~CA_STATE_CAPTURING;

    safeReleaseMediaFoundation(&imf_source_reader);
    safeReleaseMediaFoundation(&mf_callback);

    return 1;   //TODO Err code
}

std::unique_ptr<Frame> MediaFoundation_Camera::CaptureFrame()
{
    //TODO to realise method
    return nullptr;
}

// ---- Capabilities ----
std::vector<CapabilityFormat> MediaFoundation_Camera::getCapabilities()
{
    std::vector<CapabilityFormat> result;
    getVideoCapabilities(imf_media_source, result);

    return result;
}



bool MediaFoundation_Camera::getPropertyRange(VideoProperty property, VideoPropertyRange &videoPropRange)
{
    IAMVideoProcAmp *pProcAmp = NULL;
    VideoProcAmpProperty ampProperty;
    long lMin, lMax, lStep, lDefault, lCaps;

    HRESULT hr = imf_media_source->QueryInterface(IID_PPV_ARGS(&pProcAmp));

    if (FAILED(hr)) {
        DEBUG_PRINT("Can't get IAMVideoProcAmp object. GetPropertyRange failed.\n");
        return false;
    }

    switch (property) {
        case VideoProperty::Brightness : {
            ampProperty = VideoProcAmp_Brightness;
            break;
        }

        case VideoProperty::Contrast : {
            ampProperty = VideoProcAmp_Contrast;
            break;
        }

        case VideoProperty::Saturation : {
            ampProperty = VideoProcAmp_Saturation;
            break;
        }

        default: {
            DEBUG_PRINT("Unsupported VideoPropertyValue. GetPropertyRange failed.\n");
            return false;
        }
    }

    hr = pProcAmp->GetRange(ampProperty, &lMin, &lMax, &lStep, &lDefault, &lCaps);

    if (FAILED(hr)) {
        DEBUG_PRINT("Unsupported VideoPropertyValue. GetPropertyRange failed.\n");
        return false;
    }

    videoPropRange = VideoPropertyRange(lMin, lMax, lStep, lDefault);

    return true;
}



int MediaFoundation_Camera::getProperty(VideoProperty property)
{

    IAMVideoProcAmp *pProcAmp = NULL;
    VideoProcAmpProperty ampProperty;
    HRESULT hr = imf_media_source->QueryInterface(IID_PPV_ARGS(&pProcAmp));

    if (FAILED(hr)) {
        DEBUG_PRINT("Can't get IAMVideoProcAmp object. GetPropertyRange failed.\n");
        return -99999;///TODO to return error value
    }

    switch (property) {
        case VideoProperty::Brightness : {
            ampProperty = VideoProcAmp_Brightness;
            break;
        }

        case VideoProperty::Contrast : {
            ampProperty = VideoProcAmp_Contrast;
            break;
        }

        case VideoProperty::Saturation : {
            ampProperty = VideoProcAmp_Saturation;
            break;
        }

        default: {
            DEBUG_PRINT("Unsupported VideoPropertyValue. GetPropertyRange failed.\n");
            return -99999; ///TODO to return error value
        }
    }

    long value;
    long flags;
    hr = pProcAmp->Get(ampProperty, &value, &flags);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error during IAMVideoProcAmp->Get. SetProperty failed.\n");
        return -99999;
    }

    return value;
}



bool MediaFoundation_Camera::setProperty(const VideoProperty property, const int value)
{
    IAMVideoProcAmp *pProcAmp = NULL;
    VideoProcAmpProperty ampProperty;
    HRESULT hr = imf_media_source->QueryInterface(IID_PPV_ARGS(&pProcAmp));

    if (FAILED(hr)) {
        DEBUG_PRINT("Can't get IAMVideoProcAmp object. SetProperty failed.\n");
        return false;
    }

    switch (property) {
        case VideoProperty::Brightness : {
            ampProperty = VideoProcAmp_Brightness;
            break;
        }

        case VideoProperty::Contrast : {
            ampProperty = VideoProcAmp_Contrast;
            break;
        }

        case VideoProperty::Saturation : {
            ampProperty = VideoProcAmp_Saturation;
            break;
        }

        default: {
            return 0; ///TODO to return error value
        }
    }

    long val;
    long flags;
    hr = pProcAmp->Get(ampProperty, &val, &flags);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error during IAMVideoProcAmp->Get. SetProperty failed.\n");
        return false;
    }

    hr = pProcAmp->Set(ampProperty, value, flags);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error during IAMVideoProcAmp->Set. SetProperty failed.\n");
        return false;
    }

    return true;
}

/* PLATFORM SDK SPECIFIC */
/* -------------------------------------- */

int MediaFoundation_Camera::setDeviceFormat(IMFMediaSource *source, const int width, const int height,
        const PixelFormat pixelFormat, const float fps) const
{

    IMFPresentationDescriptor *pres_desc = NULL;
    IMFStreamDescriptor *stream_desc = NULL;
    IMFMediaTypeHandler *media_handler = NULL;
    IMFMediaType *type = NULL;
    int result = 1; //TODO Err code

    HRESULT hr = source->CreatePresentationDescriptor(&pres_desc);

    if (FAILED(hr)) {
        DEBUG_PRINT("source->CreatePresentationDescriptor() failed.\n");
        result = -1;        //TODO Err code
        goto done;
    }

    BOOL selected;
    hr = pres_desc->GetStreamDescriptorByIndex(0, &selected, &stream_desc);

    if (FAILED(hr)) {
        DEBUG_PRINT("pres_desc->GetStreamDescriptorByIndex failed.\n");
        result = -2;        //TODO Err code
        goto done;
    }

    hr = stream_desc->GetMediaTypeHandler(&media_handler);

    if (FAILED(hr)) {
        DEBUG_PRINT("stream_desc->GetMediaTypehandler() failed.\n");
        result = -3;        //TODO Err code
        goto done;
    }

    DWORD types_count = 0;
    hr = media_handler->GetMediaTypeCount(&types_count);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get media type count.\n");
        result = -4;        //TODO Err code
        goto done;
    }

    PROPVARIANT var;

    bool setType = false;

    for (DWORD i = 0; i < types_count; ++i) {
        PixelFormat pixelFormatBuf = PixelFormat::UNKNOWN;
        int widthBuf = 0;
        int heightBuf = 0;
        UINT32 high = 0;
        UINT32 low =  0;

        hr = media_handler->GetMediaTypeByIndex(i, &type);

        if (FAILED(hr)) {
            DEBUG_PRINT("Error: cannot get media type by index.\n");
            result = -5;        //TODO Err code
            goto done;
        }

        UINT32 attr_count = 0;
        hr = type->GetCount(&attr_count);

        if (FAILED(hr)) {
            DEBUG_PRINT("Error: cannot type param count.\n");
            result = -6;        //TODO Err code
            goto done;
        }

        if (attr_count > 0) {
            for (UINT32 j = 0; j < attr_count; ++j) {
                GUID guid = { 0 };
                PropVariantInit(&var);

                hr = type->GetItemByIndex(j, &guid, &var);

                if (FAILED(hr)) {
                    DEBUG_PRINT("Error: cannot get item by index.\n");
                    result = -7;        //TODO Err code
                    PropVariantClear(&var);
                    goto done;
                }

                if (guid == MF_MT_SUBTYPE && var.vt == VT_CLSID) {
                    pixelFormatBuf = media_foundation_video_format_to_capture_format(*var.puuid);
                } else if (guid == MF_MT_FRAME_SIZE) {
                    Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                    widthBuf = (int)high;
                    heightBuf = (int)low;
                }

                PropVariantClear(&var);
            }

            // Wait till we get the format and resolution we are looking for
            if (widthBuf != width || heightBuf != height || pixelFormatBuf != pixelFormat) {
                safeReleaseMediaFoundation(&type);
                continue;
            }

            // figure out fps
            // we don't read fps above because we need to set it on type, which requires PROPVARIANT of that fps

            auto trySetFps = [&type, &var, &hr, &high, &low, fps](REFGUID GUID) {
                bool setFps = false;

                PropVariantInit(&var);
                hr = type->GetItem(GUID, &var);
                if (SUCCEEDED(hr)) {
                    Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                    float fpsBuf = FPS_FROM_RATIONAL(high, low);

                    if (FPS_EQUAL(fps, fpsBuf)) {
                        hr = type->SetItem(MF_MT_FRAME_RATE, var);
                        setFps = SUCCEEDED(hr);
                    }
                }
                PropVariantClear(&var);

                return setFps;
            };

            // if we found the fps we are looking for and set it on the type
            if (trySetFps(MF_MT_FRAME_RATE) || trySetFps(MF_MT_FRAME_RATE_RANGE_MAX) || trySetFps(MF_MT_FRAME_RATE_RANGE_MIN)) {

                hr = media_handler->SetCurrentMediaType(type);
                if (FAILED(hr)) {
                    result = -7;
                    DEBUG_PRINT("Error: Failed to set the current media type for the given settings.\n");
                } else {
                    setType = true;
                }
                safeReleaseMediaFoundation(&type);
                break;
            }

            safeReleaseMediaFoundation(&type);
        }
    }

    if (!setType) {
        result = -8;
    }

done:
    safeReleaseMediaFoundation(&pres_desc);
    safeReleaseMediaFoundation(&stream_desc);
    safeReleaseMediaFoundation(&media_handler);
    safeReleaseMediaFoundation(&type);

    return result;
}

int MediaFoundation_Camera::createSourceReader(IMFMediaSource *mediaSource,  IMFSourceReaderCallback *callback,
        IMFSourceReader **sourceReader) const
{

    if (mediaSource == NULL) {
        DEBUG_PRINT("Error: Cannot create a source reader because the IMFMediaSource passed into this function is not valid.\n");
        return -1;         //TODO Err code
    }

    if (callback == NULL) {
        DEBUG_PRINT("Error: Cannot create a source reader because the calls back passed into this function is not valid.\n");
        return -2;        //TODO Err code
    }

    HRESULT hr = S_OK;
    IMFAttributes *attrs = NULL;
    int result = 1;        //TODO Err code

    hr = MFCreateAttributes(&attrs, 2);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot create attributes for the media source reader.\n");
        result = -3;        //TODO Err code
        goto done;
    }

    hr = attrs->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, callback);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: SetUnknown() failed on the source reader");
        result = -4;        //TODO Err code
        goto done;
    }

    /// This attribute gives such result - source reader does not shut down the media source.
    hr = attrs->SetUINT32(MF_SOURCE_READER_DISCONNECT_MEDIASOURCE_ON_SHUTDOWN, TRUE);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: SetUINT32() failed on the source reader");
        result = -5;        //TODO Err code
        goto done;
    }

    // Create a source reader which sets up the pipeline for us so we get access to the pixels
    hr = MFCreateSourceReaderFromMediaSource(mediaSource, attrs, sourceReader);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: while creating a source reader.\n");
        result = -6;        //TODO Err code
        goto done;
    }

done:
    safeReleaseMediaFoundation(&attrs);
    return result;
}

int MediaFoundation_Camera::setReaderFormat(IMFSourceReader *reader, const int width, const int height,
        const PixelFormat pixelFormat, const float fps) const
{

    int result = -1;        //TODO Err code
    HRESULT hr = S_OK;
    float currentFpsBuf = 0;

    for (DWORD media_type_index = 0; true; ++media_type_index) {
        PixelFormat pixelFormatBuf = PixelFormat::UNKNOWN;
        int widthBuf = 0;
        int heightBuf = 0;
        UINT32 high = 0;
        UINT32 low =  0;
        IMFMediaType *type = NULL;

        hr = reader->GetNativeMediaType(0, media_type_index, &type);

        if (FAILED(hr)) {
            break;
        }

        // PIXELFORMAT
        PROPVARIANT var;
        PropVariantInit(&var);
        hr = type->GetItem(MF_MT_SUBTYPE, &var);

        if (SUCCEEDED(hr)) {
            pixelFormatBuf = media_foundation_video_format_to_capture_format(*var.puuid);
        }

        PropVariantClear(&var);

        // SIZE
        PropVariantInit(&var);
        hr = type->GetItem(MF_MT_FRAME_SIZE, &var);

        if (SUCCEEDED(hr)) {
            Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
            widthBuf = high;
            heightBuf = low;
        }

        PropVariantClear(&var);

        PropVariantInit(&var);
        hr = type->GetItem(MF_MT_FRAME_RATE, &var);

        if (SUCCEEDED(hr)) {
            Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
            currentFpsBuf = FPS_FROM_RATIONAL(high, low);
        }

        PropVariantClear(&var);

        // When the output media type of the source reader matches our specs, set it!
        if (widthBuf == width &&
                heightBuf == height &&
                pixelFormatBuf == pixelFormat &&
                FPS_EQUAL(currentFpsBuf, fps)) {

            hr = reader->SetCurrentMediaType(0, NULL, type);

            if (FAILED(hr)) {
                DEBUG_PRINT("Error: Failed to set the current media type for the given settings.\n");
            } else {
                hr = S_OK;
                result = 1;        //TODO Err code
            }
            safeReleaseMediaFoundation(&type);
            break;
        }

        safeReleaseMediaFoundation(&type);
    }

    return result;
}



/**
 * Get capabilities for the given IMFMediaSource which represents
 * a video capture device.
 *
 * @param IMFMediaSource* source [in]               Pointer to the video capture source.
 * @param std::vector<AVCapability>& caps [out]     This will be filled with capabilites
 */
int MediaFoundation_Camera::getVideoCapabilities(IMFMediaSource *source,
        std::vector<CapabilityFormat> &capFormatVector) const
{

    IMFPresentationDescriptor *presentation_desc = NULL;
    IMFStreamDescriptor *stream_desc = NULL;
    IMFMediaTypeHandler *media_handler = NULL;
    IMFMediaType *type = NULL;
    int result = 1;        //TODO Err code

    HRESULT hr = source->CreatePresentationDescriptor(&presentation_desc);

    if (hr == MF_E_SHUTDOWN) {
        DEBUG_PRINT("Error: The media source's Shutdown method has been called.\n");
        goto done;
    }

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get presentation descriptor.\n");
        result = -1;        //TODO Err code
        goto done;
    }

    BOOL selected;
    hr = presentation_desc->GetStreamDescriptorByIndex(0, &selected, &stream_desc);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get stream descriptor.\n");
        result = -2;        //TODO Err code
        goto done;
    }

    hr = stream_desc->GetMediaTypeHandler(&media_handler);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get media type handler.\n");
        result = -3;        //TODO Err code
        goto done;
    }

    DWORD types_count = 0;
    hr = media_handler->GetMediaTypeCount(&types_count);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get media type count.\n");
        result = -4;        //TODO Err code
        goto done;
    }

#if 0
    // The list of supported types is not garantueed to return everything :)
    // this was a test to check if some types that are supported by my test-webcam
    // were supported when I check them manually. (they didn't).
    // See the Remark here for more info: http://msdn.microsoft.com/en-us/library/windows/desktop/bb970473(v=vs.85).aspx
    IMFMediaType *test_type = NULL;
    MFCreateMediaType(&test_type);

    if (test_type) {
        GUID types[] = { MFVideoFormat_UYVY,
                         MFVideoFormat_I420,
                         MFVideoFormat_IYUV,
                         MFVideoFormat_NV12,
                         MFVideoFormat_YUY2,
                         MFVideoFormat_Y42T,
                         MFVideoFormat_RGB24
                       } ;

        test_type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);

        for (int i = 0; i < 7; ++i) {
            test_type->SetGUID(MF_MT_SUBTYPE, types[i]);
            hr = media_handler->IsMediaTypeSupported(test_type, NULL);

            if (hr != S_OK) {
                DEBUG_PRINT("> Not supported: %d\n");
            } else {
                DEBUG_PRINT("> Yes, supported: %d\n", i);
            }
        }
    }

    safeReleaseMediaFoundation(&test_type);
#endif
    {
        CapabilityTreeBuilder capabilityBuilder;

        // Loop over all the types
        PROPVARIANT var;

        for (DWORD i = 0; i < types_count; ++i) {

            PixelFormat pixelFormat = PixelFormat::UNKNOWN;
            int width = 0;
            int height = 0;
            float minFps = 0;
            float maxFps = 0;
            float currentFps = 0;
            UINT32 high = 0;
            UINT32 low =  0;

            hr = media_handler->GetMediaTypeByIndex(i, &type);

            if (FAILED(hr)) {
                DEBUG_PRINT("Error: cannot get media type by index.\n");
                safeReleaseMediaFoundation(&type);
                continue;
            }

            UINT32 attr_count = 0;
            hr = type->GetCount(&attr_count);

            if (FAILED(hr)) {
                DEBUG_PRINT("Error: cannot type param count.\n");
                safeReleaseMediaFoundation(&type);
                continue;
            }

            if (attr_count > 0) {
                for (UINT32 j = 0; j < attr_count; ++j) {

                    GUID guid = { 0 };
                    PropVariantInit(&var);

                    hr = type->GetItemByIndex(j, &guid, &var);

                    if (FAILED(hr)) {
                        DEBUG_PRINT("Error: cannot get item by index.\n");
                        PropVariantClear(&var);
                        continue;
                    }

                    if (guid == MF_MT_SUBTYPE && var.vt == VT_CLSID) {
                        pixelFormat = media_foundation_video_format_to_capture_format(*var.puuid);
                    } else if (guid == MF_MT_FRAME_SIZE) {
                        Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                        width = (int)high;
                        height = (int)low;
                    } else if (guid == MF_MT_FRAME_RATE_RANGE_MIN) {
                        Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                        minFps = FPS_FROM_RATIONAL(high, low);
                    } else if (guid == MF_MT_FRAME_RATE_RANGE_MAX) {
                        Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                        maxFps = FPS_FROM_RATIONAL(high, low);
                    } else if (guid == MF_MT_FRAME_RATE) {
                        Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                        currentFps = FPS_FROM_RATIONAL(high, low);
                    }

                    PropVariantClear(&var);
                }

                // check that all required fields were set
                if (pixelFormat == PixelFormat::UNKNOWN || !width || !height || !minFps || !maxFps || !currentFps) {
                    continue;
                }

                capabilityBuilder.addCapability(pixelFormat, width, height, {minFps, maxFps, currentFps});
            }

            safeReleaseMediaFoundation(&type);
        }


        capFormatVector = capabilityBuilder.build();
    }

done:
    safeReleaseMediaFoundation(&presentation_desc);
    safeReleaseMediaFoundation(&stream_desc);
    safeReleaseMediaFoundation(&media_handler);

    return result;
}

/**
 * Create and active the given `device`.
 *
 * @param WCHAR *pszSymbolicLink[in] The device symbolic Link, which is unique identifier of camera device
 *                                   for which you want to get an activated IMFMediaSource object. This function
 *                                   allocates this object and increases the reference
 *                                   count. When you're ready with this object, make sure
 *                                   to call `safeReleaseMediaFoundation(&source)`
 *
 * @param IMFMediaSource** [out]     We allocate and activate the device for the
 *                                   given `device` parameter. When ready, call
 *                                   `safeReleaseMediaFoundation(&source)` to free memory.
 */
int MediaFoundation_Camera::createVideoDeviceSource(const std::wstring &pszSymbolicLink, IMFMediaSource **ppSource)
{
    *ppSource = NULL;
    IMFAttributes *pAttributes = NULL;

    HRESULT hr = MFCreateAttributes(&pAttributes, 2);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot create MFCreateAttributes.\n");
        safeReleaseMediaFoundation(&pAttributes);
        return -1;        //TODO Err code
    }

    // Set the device type to video.
    hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                              MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot set the device type to video.\n");
        safeReleaseMediaFoundation(&pAttributes);
        return -2;        //TODO Err code
    }

    // Set the symbolic link.
    hr = pAttributes->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                                pszSymbolicLink.c_str());

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot set the symbolic link.\n");
        safeReleaseMediaFoundation(&pAttributes);
        return -3;        //TODO Err code
    }

    hr = MFCreateDeviceSource(pAttributes, ppSource);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot crete MF Device Source.\n");
        safeReleaseMediaFoundation(&pAttributes);
        return -4;        //TODO Err code
    }

    safeReleaseMediaFoundation(&pAttributes);
    return 1; //TODO Err code
}
} // namespace webcam_capture

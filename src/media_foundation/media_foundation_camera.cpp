#include "media_foundation_camera.h"
#include <iostream>

namespace webcam_capture {

    MediaFoundation_Camera::MediaFoundation_Camera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information, IMFMediaSource *mediaSource)
        :information(information)
        ,mfDeinitializer(mfDeinitializer)
        ,state(CA_STATE_NONE)
        ,imf_media_source(mediaSource)
        ,mf_callback(NULL)
        ,imf_source_reader(NULL)
    {
    }

    CameraInterface* MediaFoundation_Camera::createCamera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information) {
        IMFMediaSource * mediaSource = NULL;
        // Create the MediaSource
        if(MediaFoundation_Camera::createVideoDeviceSource(information.getDeviceId(), &mediaSource) < 0) {
            DEBUG_PRINT("Error: cannot create the media device source.\n");
            return NULL;
        }

        return new MediaFoundation_Camera(mfDeinitializer, information, mediaSource);
    }

    MediaFoundation_Camera::~MediaFoundation_Camera(){
        // Stop capturing
        if(state & CA_STATE_CAPTURING) {
            stop();
        }
        //Release mediaSource
        safeReleaseMediaFoundation(&imf_media_source);
    }

    int MediaFoundation_Camera::start(const CapabilityFormat &capabilityFormat,
                                      const CapabilityResolution &capabilityResolution,
                                      const CapabilityFps &capabilityFps,
                                      frame_callback cb){
        if ( !cb ) {
            DEBUG_PRINT("Error: The callback function is empty. Capturing was not started.\n");
            return -1;      //TODO Err code
        }

        if(state & CA_STATE_CAPTURING) {
          DEBUG_PRINT("Error: cannot start capture because we are already capturing.\n");
          return -2;      //TODO Err code
        }

        cb_frame = cb;

//I KNOW THAT it is stupid way, but fps setting works only in this way. At first delete media source then create
//if you wan't do it, after stopping the capturing, you won't be availiable to set another fps.
        safeReleaseMediaFoundation(&imf_media_source);
        if(MediaFoundation_Camera::createVideoDeviceSource(information.getDeviceId(), &imf_media_source) < 0) {
            DEBUG_PRINT("Error: cannot create the media device source.\n");
            return -3;
        }
// /////////


        // Set the media format, width, height
        std::vector<CapabilityFormat> capabilities;
        if(getVideoCapabilities(imf_media_source, capabilities) < 0) {
            DEBUG_PRINT("Error: cannot create the capabilities list to start capturing. Capturing was not started.\n");
            return -4;      //TODO Err code
        }

///Check of "capabilities" have inputed params
//check format
        bool isFormatValid = false;
        int formatIndex = 0;
        for (int i = 0; i < capabilities.size(); i++){
            if ( capabilities.at(i).getPixelFormat() == capabilityFormat.getPixelFormat() )
            {
                formatIndex = i;
                isFormatValid = true;
                break;
            }
        }
        if (!isFormatValid){
            DEBUG_PRINT("Error: cannot found such capabilityFormat in capabilities.\n");
            return -5;
        }

//chech resolution
        const std::vector<CapabilityResolution> &resolutionVectorBuf = capabilities.at(formatIndex).getResolutions();
        bool isResolutionValid = false;
        int resolutionsIndex = 0;
        for (int j = 0; j < resolutionVectorBuf.size(); j++) {
            if (resolutionVectorBuf.at(j).getHeight()  == capabilityResolution.getHeight() &&
                resolutionVectorBuf.at(j).getWidth() == capabilityResolution.getWidth() )
            {
                resolutionsIndex = j;
                isResolutionValid = true;
                break;
            }
        }
        if ( !isResolutionValid ) {
            DEBUG_PRINT("Error: cannot found such capabilityResolution in capabilities.\n");
            return -6;
        }

//check fps
        const std::vector<CapabilityFps> &fpsVectorBuf = resolutionVectorBuf.at(resolutionsIndex).getFpses();
        bool isFpsValid = false;
        for (int k = 0; k < fpsVectorBuf.size(); k++) {
            if (fpsVectorBuf.at(k).getFps() == capabilityFps.getFps() ){
                isFpsValid = true;
            }
        }
        if ( !isFpsValid ) {
            DEBUG_PRINT("Error: cannot found such capabilityFps in capabilities.\n");
            return -7;
        }
//END OF Check of "capabilities" have inputed params

        if(capabilityFormat.getPixelFormat() == Format::UNKNOWN) {
            DEBUG_PRINT("Error: cannot set a pixel format for UNKNOWN.\n");
            return -8;      //TODO Err code
        }

        if(setDeviceFormat(imf_media_source, capabilityResolution.getWidth(),
                           capabilityResolution.getHeight(),
                           capabilityFormat.getPixelFormat(),
                           capabilityFps.getFps() ) < 0) {
            DEBUG_PRINT("Error: cannot set the device format.\n");
            return -9;      //TODO Err code
        }

        // Create the source reader.
        MediaFoundation_Callback::createInstance(this, &mf_callback);
        if(createSourceReader(imf_media_source, mf_callback, &imf_source_reader) < 0) {
            DEBUG_PRINT("Error: cannot create the source reader.\n");
            safeReleaseMediaFoundation(&mf_callback);            
            return -10;      //TODO Err code
        }

        // Set the source reader format.
        if(setReaderFormat(imf_source_reader, capabilityResolution.getWidth(),
                           capabilityResolution.getHeight(),
                           capabilityFormat.getPixelFormat(),
                           capabilityFps.getFps() ) < 0) {
            DEBUG_PRINT("Error: cannot set the reader format.\n");
            safeReleaseMediaFoundation(&mf_callback);
            safeReleaseMediaFoundation(&imf_source_reader);
            return -11;      //TODO Err code
        }

//        if (setDeviceFps(imf_media_source, capabilityFps.getFps()) < 0) {
//            DEBUG_PRINT("Error: cannot set device frame rate.\n");
//        }

        pixel_buffer.height[0] = capabilityResolution.getHeight();
        pixel_buffer.width[0] = capabilityResolution.getWidth();
        pixel_buffer.pixel_format = capabilityFormat.getPixelFormat();


        // Kick off the capture stream.
        HRESULT hr = imf_source_reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);
        if(FAILED(hr)) {
          if(hr == MF_E_INVALIDREQUEST) {
            DEBUG_PRINT("ReadSample returned MF_E_INVALIDREQUEST.\n");
          }
          else if(hr == MF_E_INVALIDSTREAMNUMBER) {
            DEBUG_PRINT("ReadSample returned MF_E_INVALIDSTREAMNUMBER.\n");
          }
          else if(hr == MF_E_NOTACCEPTING) {
            DEBUG_PRINT("ReadSample returned MF_E_NOTACCEPTING.\n");
          }
          else if(hr == E_INVALIDARG) {
            DEBUG_PRINT("ReadSample returned E_INVALIDARG.\n");
          }
          else if(hr == E_POINTER) {
            DEBUG_PRINT("ReadSample returned E_POINTER.\n");
          }
          else {
            DEBUG_PRINT("ReadSample - unhandled result.\n");
          }
          DEBUG_PRINT("Error: while trying to ReadSample() on the imf_source_reader. \n");
          #ifdef DEBUG_VERSION
            std::cout << "Error: " << std::hex << hr << std::endl;
          #endif
          return -4;      //TODO Err code
        }

        state |= CA_STATE_CAPTURING;

        return 1;      //TODO Err code
    }

    int MediaFoundation_Camera::stop(){
        if(!state & CA_STATE_CAPTURING) {
          DEBUG_PRINT("Error: Cannot stop capture because we're not capturing yet.\n");
          return -1;    //TODO Err code
        }

        if(!imf_source_reader) {
          DEBUG_PRINT("Error: Cannot stop capture because sourceReader is empty yet.\n");
          return -2;    //TODO Err code
        }

        state &= ~CA_STATE_CAPTURING;

        safeReleaseMediaFoundation(&imf_source_reader);
        safeReleaseMediaFoundation(&mf_callback);

        return 1;   //TODO Err code
    }

    PixelBuffer* MediaFoundation_Camera::CaptureFrame(){
        //TODO to realise method
        return NULL;
    }

// ---- Capabilities ----
    std::vector<CapabilityFormat> MediaFoundation_Camera::getCapabilities(){
        std::vector<CapabilityFormat> result;
        getVideoCapabilities(imf_media_source, result);

        return result;
    }



    bool MediaFoundation_Camera::getPropertyRange(VideoProperty property, VideoPropertyRange *videoPropRange){
        IAMVideoProcAmp *pProcAmp = NULL;
        VideoProcAmpProperty ampProperty;
        long lMin, lMax, lStep, lDefault, lCaps;

        HRESULT hr = imf_media_source->QueryInterface(IID_PPV_ARGS(&pProcAmp));
        if (FAILED(hr)){
            DEBUG_PRINT("Can't get IAMVideoProcAmp object. GetPropertyRange failed.\n");            
            return false;
        }
        switch (property){
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
       if (FAILED(hr)){
           DEBUG_PRINT("Unsupported VideoPropertyValue. GetPropertyRange failed.\n");
           return false;
       }

       videoPropRange->setMaxValue(lMax);
       videoPropRange->setMinValue(lMin);
       videoPropRange->setStepValue(lStep);
       videoPropRange->setDefaultValue(lDefault);

       return true;
    }



    int MediaFoundation_Camera::getProperty(VideoProperty property){

        IAMVideoProcAmp *pProcAmp = NULL;
        VideoProcAmpProperty ampProperty;
        HRESULT hr = imf_media_source->QueryInterface(IID_PPV_ARGS(&pProcAmp));
        if (FAILED(hr)){
            DEBUG_PRINT("Can't get IAMVideoProcAmp object. GetPropertyRange failed.\n");
            return -99999;///TODO to return error value
        }

        switch (property){
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
        if (FAILED(hr)){
            DEBUG_PRINT("Error during IAMVideoProcAmp->Get. SetProperty failed.\n");            
            return -99999;
        }        
        return value;
    }



    bool MediaFoundation_Camera::setProperty(const VideoProperty property, const int value){

        IAMVideoProcAmp *pProcAmp = NULL;
        VideoProcAmpProperty ampProperty;
        HRESULT hr = imf_media_source->QueryInterface(IID_PPV_ARGS(&pProcAmp));
        if (FAILED(hr)){
            DEBUG_PRINT("Can't get IAMVideoProcAmp object. SetProperty failed.\n");            
            return false;
        }
        switch (property){
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
        if (FAILED(hr)){
            DEBUG_PRINT("Error during IAMVideoProcAmp->Get. SetProperty failed.\n");            
            return false;
        }

        hr = pProcAmp->Set(ampProperty, value, flags);
        if (FAILED(hr)){
            DEBUG_PRINT("Error during IAMVideoProcAmp->Set. SetProperty failed.\n");            
            return false;
        }        
        return true;
    }

    /* PLATFORM SDK SPECIFIC */
    /* -------------------------------------- */

    const int MediaFoundation_Camera::setDeviceFormat(IMFMediaSource* source, const int width, const int height, const Format pixelFormat, const int fps) {

      IMFPresentationDescriptor* pres_desc = NULL;
      IMFStreamDescriptor* stream_desc = NULL;
      IMFMediaTypeHandler* media_handler = NULL;
      IMFMediaType* type = NULL;
      int result = 1; //TODO Err code

      HRESULT hr = source->CreatePresentationDescriptor(&pres_desc);
      if(FAILED(hr)) {
        DEBUG_PRINT("source->CreatePresentationDescriptor() failed.\n");
        result = -1;        //TODO Err code
        goto done;
      }

      BOOL selected;
      hr = pres_desc->GetStreamDescriptorByIndex(0, &selected, &stream_desc);
      if(FAILED(hr)) {
        DEBUG_PRINT("pres_desc->GetStreamDescriptorByIndex failed.\n");
        result = -2;        //TODO Err code
        goto done;
      }

      hr = stream_desc->GetMediaTypeHandler(&media_handler);
      if(FAILED(hr)) {
        DEBUG_PRINT("stream_desc->GetMediaTypehandler() failed.\n");
        result = -3;        //TODO Err code
        goto done;
      }

      DWORD types_count = 0;
      hr = media_handler->GetMediaTypeCount(&types_count);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get media type count.\n");
        result = -4;        //TODO Err code
        goto done;
      }

      PROPVARIANT var;
      for(DWORD i = 0; i < types_count; ++i) {

        Format pixelFormatBuf;
        int widthBuf;
        int heightBuf;
        int fpsBuf;

        hr = media_handler->GetMediaTypeByIndex(i, &type);

        if(FAILED(hr)) {
          DEBUG_PRINT("Error: cannot get media type by index.\n");
          result = -5;        //TODO Err code
          goto done;
        }

        UINT32 attr_count = 0;
        hr = type->GetCount(&attr_count);
        if(FAILED(hr)) {
          DEBUG_PRINT("Error: cannot type param count.\n");
          result = -6;        //TODO Err code
          goto done;
        }

        if(attr_count > 0) {
          for(UINT32 j = 0; j < attr_count; ++j) {

            GUID guid = { 0 };
            PropVariantInit(&var);

            hr = type->GetItemByIndex(j, &guid, &var);
            if(FAILED(hr)) {
              DEBUG_PRINT("Error: cannot get item by index.\n");
              result = -7;        //TODO Err code
              goto done;
            }

            if(guid == MF_MT_SUBTYPE && var.vt == VT_CLSID) {
              pixelFormatBuf = media_foundation_video_format_to_capture_format(*var.puuid);
            }
            else if(guid == MF_MT_FRAME_SIZE) {
              UINT32 high = 0;
              UINT32 low =  0;
              Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
              widthBuf = (int)high;
              heightBuf = (int)low;
            }
            PropVariantClear(&var);
          }

        // When the output media type of the source reader matches our specs, set it!
        if( widthBuf == width &&
            heightBuf == height &&
            pixelFormatBuf == pixelFormat) {

              //Compare input fps with max\min fpses and preset it
              PropVariantInit(&var);
              {
                hr = type->GetItem(MF_MT_FRAME_RATE_RANGE_MAX, &var);
                if(SUCCEEDED(hr)) {
                  UINT32 high = 0;
                  UINT32 low =  0;
                  Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                  fpsBuf = fps_from_rational(low, high);
                  if ( fpsBuf == fps ) {
                      hr = type->SetItem(MF_MT_FRAME_RATE, var);
                  }
                }
              }
              PropVariantClear(&var);

              PropVariantInit(&var);
              {
                hr = type->GetItem(MF_MT_FRAME_RATE_RANGE_MIN, &var);
                if(SUCCEEDED(hr)) {
                  UINT32 high = 0;
                  UINT32 low =  0;
                  Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                  fpsBuf = fps_from_rational(low, high);
                  if ( fpsBuf == fps ) {
                      hr = type->SetItem(MF_MT_FRAME_RATE, var);
                  }
                }
              }
              PropVariantClear(&var);


              hr = media_handler->SetCurrentMediaType(type);
              if(FAILED(hr)) {
                  DEBUG_PRINT("Error: Failed to set the current media type for the given settings.\n");
              } else { break; }
           }
        }
        safeReleaseMediaFoundation(&type);
      }

    done:
      safeReleaseMediaFoundation(&pres_desc);
      safeReleaseMediaFoundation(&stream_desc);
      safeReleaseMediaFoundation(&media_handler);
      safeReleaseMediaFoundation(&type);
      PropVariantClear(&var);

      return result;
    }

    const int MediaFoundation_Camera::createSourceReader(IMFMediaSource* mediaSource,  IMFSourceReaderCallback* callback, IMFSourceReader** sourceReader) {

      if(mediaSource == NULL) {
        DEBUG_PRINT("Error: Cannot create a source reader because the IMFMediaSource passed into this function is not valid.\n");
        return -1;         //TODO Err code
      }

      if(callback == NULL) {
        DEBUG_PRINT("Error: Cannot create a source reader because the calls back passed into this function is not valid.\n");
        return -2;        //TODO Err code
      }

      HRESULT hr = S_OK;
      IMFAttributes* attrs = NULL;
      int result = 1;        //TODO Err code

      hr = MFCreateAttributes(&attrs, 2);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot create attributes for the media source reader.\n");
        result = -3;        //TODO Err code
        goto done;
      }

      hr = attrs->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, callback);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: SetUnknown() failed on the source reader");
        result = -4;        //TODO Err code
        goto done;
      }

      /// This attribute gives such result - source reader does not shut down the media source.
      hr = attrs->SetUINT32(MF_SOURCE_READER_DISCONNECT_MEDIASOURCE_ON_SHUTDOWN, TRUE);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: SetUINT32() failed on the source reader");
        result = -5;        //TODO Err code
        goto done;
      }

      // Create a source reader which sets up the pipeline for us so we get access to the pixels
      hr = MFCreateSourceReaderFromMediaSource(mediaSource, attrs, sourceReader);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: while creating a source reader.\n");
        result = -6;        //TODO Err code
        goto done;
      }

    done:
      safeReleaseMediaFoundation(&attrs);
      return result;
    }

    const int MediaFoundation_Camera::setReaderFormat(IMFSourceReader* reader, const int width, const int height, const Format pixelFormat, const int fps) {

      DWORD media_type_index = 0;
      int result = -1;        //TODO Err code
      HRESULT hr = S_OK;

      while(SUCCEEDED(hr)) {

        Format pixelFormatBuf;
        int widthBuf;
        int heightBuf;


        IMFMediaType* type = NULL;
        hr = reader->GetNativeMediaType(0, media_type_index, &type);

        if(SUCCEEDED(hr)) {

          // PIXELFORMAT
          PROPVARIANT var;
          PropVariantInit(&var);
          {
            hr = type->GetItem(MF_MT_SUBTYPE, &var);
            if(SUCCEEDED(hr)) {
              pixelFormatBuf = media_foundation_video_format_to_capture_format(*var.puuid);
            }
          }
          PropVariantClear(&var);

          // SIZE
          PropVariantInit(&var);
          {
            hr = type->GetItem(MF_MT_FRAME_SIZE, &var);
            if(SUCCEEDED(hr)) {
              UINT32 high = 0;
              UINT32 low =  0;
              Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
              widthBuf = high;
              heightBuf = low;
            }
          }
          PropVariantClear(&var);

          // When the output media type of the source reader matches our specs, set it!
          if( widthBuf == width &&
              heightBuf == height &&
              pixelFormatBuf == pixelFormat) {
                int fpsBuf;
                //Compare input fps with max\min fpses and preset it
                PropVariantInit(&var);
                {
                  hr = type->GetItem(MF_MT_FRAME_RATE_RANGE_MAX, &var);
                  if(SUCCEEDED(hr)) {
                    UINT32 high = 0;
                    UINT32 low =  0;
                    Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                    fpsBuf = fps_from_rational(low, high);
                    if ( fpsBuf == fps ) {
                        hr = type->SetItem(MF_MT_FRAME_RATE, var);
                    }
                  }
                }
                PropVariantClear(&var);

                PropVariantInit(&var);
                {
                  hr = type->GetItem(MF_MT_FRAME_RATE_RANGE_MIN, &var);
                  if(SUCCEEDED(hr)) {
                    UINT32 high = 0;
                    UINT32 low =  0;
                    Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                    fpsBuf = fps_from_rational(low, high);
                    if ( fpsBuf == fps ) {
                        hr = type->SetItem(MF_MT_FRAME_RATE, var);
                    }
                  }
                }
                PropVariantClear(&var);


                hr = reader->SetCurrentMediaType(0, NULL, type);
                if(FAILED(hr)) {
                    DEBUG_PRINT("Error: Failed to set the current media type for the given settings.\n");
                }
                else {
                hr = S_OK;
                result = 1;        //TODO Err code
                }
            }
        }
        else {
          break;
        }

        safeReleaseMediaFoundation(&type);

        ++media_type_index;
      }

      return result;
    }


    const int MediaFoundation_Camera::setDeviceFps(IMFMediaSource *source, int fps) {
        int result = 1;
        IMFPresentationDescriptor *pPD = NULL;
        IMFStreamDescriptor *pSD = NULL;
        IMFMediaTypeHandler *pHandler = NULL;
        IMFMediaType *pType = NULL;

        HRESULT hr = source->CreatePresentationDescriptor(&pPD);
        if (FAILED(hr))
        {
            DEBUG_PRINT("Error: Failed to Create Presentation Descriptor.\n");
            result = -1;
            goto done;
        }

// Debug info (to get know how many stream Desctiptors)
//        DWORD streamDesctiptorsCount;
//        hr = pPD->GetStreamDescriptorCount(&streamDesctiptorsCount);

        BOOL fSelected;
        hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
        if (FAILED(hr))
        {
            DEBUG_PRINT("Error: Failed to Get Stream Descriptor By Index.\n");
            result = -2;
            goto done;
        }

        hr = pSD->GetMediaTypeHandler(&pHandler);
        if (FAILED(hr))
        {
            DEBUG_PRINT("Error: Failed to Get Media Type Handler.\n");
            result = -3;
            goto done;
        }

        hr = pHandler->GetCurrentMediaType(&pType);
        if (FAILED(hr))
        {
            DEBUG_PRINT("Error: Failed to Get Current Media Type.\n");
            result = -4;
            goto done;
        }

        // Get the maximum frame rate for the selected capture format.

        // Note: To get the minimum frame rate, use the
        // MF_MT_FRAME_RATE_RANGE_MIN attribute instead.

        int fpsBuf;
        PROPVARIANT var;
        if (SUCCEEDED(pType->GetItem(MF_MT_FRAME_RATE_RANGE_MAX, &var)))
        {
            if(SUCCEEDED(hr)) {
                UINT32 high = 0;
                UINT32 low =  0;
                Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                fpsBuf = fps_from_rational(low, high);
            }
            if ( fpsBuf == fps ) {
                hr = pType->SetItem(MF_MT_FRAME_RATE, var);
            }
        }
        PropVariantClear(&var);
        if (FAILED(hr))
        {
            DEBUG_PRINT("Error: Failed to set the current frame rate.\n");
            goto done;
        }

        if (SUCCEEDED(pType->GetItem(MF_MT_FRAME_RATE_RANGE_MIN, &var)))
        {
            if(SUCCEEDED(hr)) {
                UINT32 high = 0;
                UINT32 low =  0;
                Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                fpsBuf = fps_from_rational(low, high);
            }
            if ( fpsBuf == fps ) {
                hr = pType->SetItem(MF_MT_FRAME_RATE, var);
            }
        }
        PropVariantClear(&var);
        if (FAILED(hr))
        {
            DEBUG_PRINT("Error: Failed to set the current frame rate.\n");
            goto done;
        }

        hr = pHandler->SetCurrentMediaType(pType);
        if (FAILED(hr)) {
            DEBUG_PRINT("Error: Can't set current media type.\n");
            result = -6;
        }

    done:
        safeReleaseMediaFoundation(&pPD);
        safeReleaseMediaFoundation(&pSD);
        safeReleaseMediaFoundation(&pHandler);
        safeReleaseMediaFoundation(&pType);

        return result;
    }

    /**
     * Get capabilities for the given IMFMediaSource which represents
     * a video capture device.
     *
     * @param IMFMediaSource* source [in]               Pointer to the video capture source.
     * @param std::vector<AVCapability>& caps [out]     This will be filled with capabilites
     */
    const int MediaFoundation_Camera::getVideoCapabilities(IMFMediaSource* source, std::vector<CapabilityFormat>& capFormatVector) {

      IMFPresentationDescriptor* presentation_desc = NULL;
      IMFStreamDescriptor* stream_desc = NULL;
      IMFMediaTypeHandler* media_handler = NULL;
      IMFMediaType* type = NULL;
      int result = 1;        //TODO Err code

      HRESULT hr = source->CreatePresentationDescriptor(&presentation_desc);
      if (hr == MF_E_SHUTDOWN)
      {
         DEBUG_PRINT("Error: The media source's Shutdown method has been called.\n");
         goto done;
      }
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get presentation descriptor.\n");
        result = -1;        //TODO Err code
        goto done;
      }

      BOOL selected;
      hr = presentation_desc->GetStreamDescriptorByIndex(0, &selected, &stream_desc);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get stream descriptor.\n");
        result = -2;        //TODO Err code
        goto done;
      }

      hr = stream_desc->GetMediaTypeHandler(&media_handler);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get media type handler.\n");
        result = -3;        //TODO Err code
        goto done;
      }

      DWORD types_count = 0;
      hr = media_handler->GetMediaTypeCount(&types_count);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get media type count.\n");
        result = -4;        //TODO Err code
        goto done;
      }

  #if 0
      // The list of supported types is not garantueed to return everything :)
      // this was a test to check if some types that are supported by my test-webcam
      // were supported when I check them manually. (they didn't).
      // See the Remark here for more info: http://msdn.microsoft.com/en-us/library/windows/desktop/bb970473(v=vs.85).aspx
      IMFMediaType* test_type = NULL;
      MFCreateMediaType(&test_type);
      if(test_type) {
        GUID types[] = { MFVideoFormat_UYVY,
                         MFVideoFormat_I420,
                         MFVideoFormat_IYUV,
                         MFVideoFormat_NV12,
                         MFVideoFormat_YUY2,
                         MFVideoFormat_Y42T,
                         MFVideoFormat_RGB24 } ;

        test_type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
        for(int i = 0; i < 7; ++i) {
          test_type->SetGUID(MF_MT_SUBTYPE, types[i]);
          hr = media_handler->IsMediaTypeSupported(test_type, NULL);
          if(hr != S_OK) {
            DEBUG_PRINT("> Not supported: %d\n" );
          }
          else {
            DEBUG_PRINT("> Yes, supported: %d\n", i);
          }
        }
      }
      safeReleaseMediaFoundation(&test_type);
  #endif

      // Loop over all the types
      PROPVARIANT var;




      for(DWORD i = 0; i < types_count; ++i) {

        Format pixelFormat;
        int width;
        int height;
        int minFps;
        int maxFps;
        int currentFps;

        hr = media_handler->GetMediaTypeByIndex(i, &type);

        if(FAILED(hr)) {
          DEBUG_PRINT("Error: cannot get media type by index.\n");
          result = -5;        //TODO Err code
          goto done;
        }

        UINT32 attr_count = 0;
        hr = type->GetCount(&attr_count);
        if(FAILED(hr)) {
          DEBUG_PRINT("Error: cannot type param count.\n");
          result = -6;        //TODO Err code
          goto done;
        }

        if(attr_count > 0) {
          for(UINT32 j = 0; j < attr_count; ++j) {

            GUID guid = { 0 };
            PropVariantInit(&var);

            hr = type->GetItemByIndex(j, &guid, &var);
            if(FAILED(hr)) {
              DEBUG_PRINT("Error: cannot get item by index.\n");
              result = -7;        //TODO Err code
              goto done;
            }

            if(guid == MF_MT_SUBTYPE && var.vt == VT_CLSID) {
              pixelFormat = media_foundation_video_format_to_capture_format(*var.puuid);
            }
            else if(guid == MF_MT_FRAME_SIZE) {
              UINT32 high = 0;
              UINT32 low =  0;
              Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
              width = (int)high;
              height = (int)low;
            }
            else if( guid == MF_MT_FRAME_RATE_RANGE_MIN ) {
                UINT32 high = 0;
                UINT32 low =  0;
                Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                minFps = fps_from_rational(low, high);
            }
            else if ( guid == MF_MT_FRAME_RATE_RANGE_MAX ) {
                UINT32 high = 0;
                UINT32 low =  0;
                Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                maxFps = fps_from_rational(low, high);
            }
            else if ( guid == MF_MT_FRAME_RATE )
            {
              UINT32 high = 0;
              UINT32 low =  0;
              Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
              currentFps = fps_from_rational(low, high);
              currentFps = j;
            }

            PropVariantClear(&var);
          }

//Filling the Capability info
          bool isFormatInList = false;
          bool isResolutionInList = false;
          int formatIndexInList = 0;

          for (int i = 0; i < capFormatVector.size(); i++){
              if ( capFormatVector.at(i).getPixelFormat() == pixelFormat ) {
                  isFormatInList = true;
                  formatIndexInList = i;

                  std::vector<CapabilityResolution> resolutionsBuf = capFormatVector.at(i).getResolutions();
                  for (int j = 0; j < resolutionsBuf.size(); j++) {
                      if ( resolutionsBuf.at(j).getWidth() == width &&
                           resolutionsBuf.at(j).getHeight() == height ) {
                          isResolutionInList = true;

                          std::vector<CapabilityFps> fpsesBuf = resolutionsBuf.at(j).getFpses();
                          bool needPush = false;
                          for (int k = 0; k < fpsesBuf.size(); k++) {
                              if ( fpsesBuf.at(k).getFps() == minFps ) {
                                  needPush = true;
                              }
                          }
                          CapabilityFps newMinFps(minFps);
                          fpsesBuf.push_back(newMinFps);

                          needPush = false;
                          for (int k = 0; k < fpsesBuf.size(); k++) {
                              if ( fpsesBuf.at(k).getFps() == maxFps ) {
                                  needPush = true;
                              }
                          }
                          CapabilityFps newMaxFps(maxFps);
                          fpsesBuf.push_back(newMaxFps);
                      }
                  }
              }
          }

          if ( !isFormatInList ) {

              //init fps vector
              std::vector<CapabilityFps> capFpsVector;
              if (minFps != maxFps) {
                  CapabilityFps capMinFps(minFps);
                  capFpsVector.push_back(capMinFps);
              }
              CapabilityFps capMaxFps(maxFps);
              capFpsVector.push_back(capMaxFps);

              //init capabilityVector
              CapabilityResolution capRes(width, height, capFpsVector);
              std::vector<CapabilityResolution> capResVector;             
              capResVector.push_back(capRes);

              //init capabilityFormat to push in main vector
              CapabilityFormat capFormat(pixelFormat, capResVector);
              capFormatVector.push_back(capFormat);

          } else if ( !isResolutionInList && isFormatInList ) {
              //init fps vector
              std::vector<CapabilityFps> capFpsVector;              
              if (minFps != maxFps) {
                  CapabilityFps capMinFps(minFps);
                  capFpsVector.push_back(capMinFps);
              }
              CapabilityFps capMaxFps(maxFps);
              capFpsVector.push_back(capMaxFps);

              //init capabilityVector
              CapabilityResolution capRes(width, height, capFpsVector);
              capFormatVector.at(formatIndexInList).resolutions.push_back(capRes);
          }
        }
//END OF Filling the Capability info

        safeReleaseMediaFoundation(&type);
      }

    done:
      safeReleaseMediaFoundation(&presentation_desc);
      safeReleaseMediaFoundation(&stream_desc);
      safeReleaseMediaFoundation(&media_handler);
      safeReleaseMediaFoundation(&type);
      PropVariantClear(&var);
      return result;
    }

    /**
     * Create and active the given `device`.
     *
     * @param int device [in]            The device index for which you want to get an
     *                                   activated IMFMediaSource object. This function
     *                                   allocates this object and increases the reference
     *                                   count. When you're ready with this object, make sure
     *                                   to call `safeReleaseMediaFoundation(&source)`
     *
     * @param IMFMediaSource** [out]     We allocate and activate the device for the
     *                                   given `device` parameter. When ready, call
     *                                   `safeReleaseMediaFoundation(&source)` to free memory.
     */
    int MediaFoundation_Camera::createVideoDeviceSource(const int device, IMFMediaSource** source) {

      int result = 1;  //TODO Err code
      IMFAttributes* config = NULL;
      IMFActivate** devices = NULL;
      UINT32 count = 0;

      HRESULT hr = MFCreateAttributes(&config, 1);
      if(FAILED(hr)) {
        result = -1;        //TODO Err code
        goto done;
      }

      // Filter on capture devices
      hr = config->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                           MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot set the GUID on the IMFAttributes*.\n");
        result = -2;        //TODO Err code
        goto done;
      }

      // Enumerate devices.
      hr = MFEnumDeviceSources(config, &devices, &count);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get EnumDeviceSources.\n");
        result = -3;        //TODO Err code
        goto done;
      }
      if(count == 0 || device > count) { //TODO bug (>= count)becouse devices numerates from 0.
        result = -4;        //TODO Err code
        goto done;
      }

      // Make sure the given source is free/released.
      safeReleaseMediaFoundation(source);

      // Activate the capture device.
      hr = devices[device]->ActivateObject(IID_PPV_ARGS(source));
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot activate the object.");
        result = -5;        //TODO Err code
        goto done;
      }

      result = true;

    done:

      safeReleaseMediaFoundation(&config);
      for(DWORD i = 0; i < count; ++i) {
        safeReleaseMediaFoundation(&devices[i]);
      }
      CoTaskMemFree(devices);

      return result;
    }

} // namespace webcam_capture

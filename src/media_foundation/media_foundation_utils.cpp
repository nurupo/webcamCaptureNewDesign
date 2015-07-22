#include "media_foundation_utils.h"
#include <pixel_format.h>

namespace webcam_capture {

// Convert the MF format to one we can use.
PixelFormat media_foundation_video_format_to_capture_format(GUID guid)
{
    ///Uncompressed RGB Formats
    if (IsEqualGUID(guid, MFVideoFormat_RGB8))     {
        return PixelFormat::RGB8;
    } else if (IsEqualGUID(guid, MFVideoFormat_RGB555)) {
        return PixelFormat::RGB555;
    } else if (IsEqualGUID(guid, MFVideoFormat_RGB565)) {
        return PixelFormat::RGB565;
    } else if (IsEqualGUID(guid, MFVideoFormat_RGB24)) {
        return PixelFormat::RGB24;
    } else if (IsEqualGUID(guid, MFVideoFormat_RGB32)) {
        return PixelFormat::RGB32;
    } else if (IsEqualGUID(guid, MFVideoFormat_ARGB32)) {
        return PixelFormat::ARGB32;
    }
    ///YUV Formats: 8-Bit and Palettized
    else if (IsEqualGUID(guid, MFVideoFormat_AI44)) {
        return PixelFormat::AI44;
    } else if (IsEqualGUID(guid, MFVideoFormat_AYUV)) {
        return PixelFormat::AYUV;
    } else if (IsEqualGUID(guid, MFVideoFormat_I420)) {
        return PixelFormat::I420;
    } else if (IsEqualGUID(guid, MFVideoFormat_IYUV)) {
        return PixelFormat::IYUV;
    } else if (IsEqualGUID(guid, MFVideoFormat_NV11)) {
        return PixelFormat::NV11;
    } else if (IsEqualGUID(guid, MFVideoFormat_NV12)) {
        return PixelFormat::NV12;
    } else if (IsEqualGUID(guid, MFVideoFormat_UYVY)) {
        return PixelFormat::UYVY;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y41P)) {
        return PixelFormat::Y41P;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y41T)) {
        return PixelFormat::Y41T;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y42T)) {
        return PixelFormat::Y42T;
    } else if (IsEqualGUID(guid, MFVideoFormat_YUY2)) {
        return PixelFormat::YUY2;
    } else if (IsEqualGUID(guid, MFVideoFormat_YV12)) {
        return PixelFormat::YV12;
    }
    ///YUV Formats: 10-Bit and 16-Bit
    else if (IsEqualGUID(guid, MFVideoFormat_P010)) {
        return PixelFormat::P010;
    } else if (IsEqualGUID(guid, MFVideoFormat_P016)) {
        return PixelFormat::P016;
    } else if (IsEqualGUID(guid, MFVideoFormat_P210)) {
        return PixelFormat::P210;
    } else if (IsEqualGUID(guid, MFVideoFormat_P216)) {
        return PixelFormat::P216;
    } else if (IsEqualGUID(guid, MFVideoFormat_v210)) {
        return PixelFormat::v210;
    } else if (IsEqualGUID(guid, MFVideoFormat_v216)) {
        return PixelFormat::v216;
    } else if (IsEqualGUID(guid, MFVideoFormat_v410)) {
        return PixelFormat::v40;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y210)) {
        return PixelFormat::Y210;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y216)) {
        return PixelFormat::Y216;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y410)) {
        return PixelFormat::Y40;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y416)) {
        return PixelFormat::Y416;
    }
    ///Encoded Video Types
    else if (IsEqualGUID(guid, MFVideoFormat_DV25)) {
        return PixelFormat::DV25;
    } else if (IsEqualGUID(guid, MFVideoFormat_DV50)) {
        return PixelFormat::DV50;
    } else if (IsEqualGUID(guid, MFVideoFormat_DVC)) {
        return PixelFormat::DVC;
    } else if (IsEqualGUID(guid, MFVideoFormat_DVH1)) {
        return PixelFormat::DVH1;
    } else if (IsEqualGUID(guid, MFVideoFormat_DVHD)) {
        return PixelFormat::DVHD;
    } else if (IsEqualGUID(guid, MFVideoFormat_DVSD)) {
        return PixelFormat::DVSD;
    } else if (IsEqualGUID(guid, MFVideoFormat_DVSL)) {
        return PixelFormat::DVSL;
    }

#if (WINVER >= _WIN32_WINNT_WIN8)
    else if (IsEqualGUID(guid, MFVideoFormat_H263)) {
        return PixelFormat::H263;
    }

#endif
    else if (IsEqualGUID(guid, MFVideoFormat_H264)) {
        return PixelFormat::H264;
    } else if (IsEqualGUID(guid, MFVideoFormat_H264_ES)) {
        return PixelFormat::H264_ES;
    } else if (IsEqualGUID(guid, MFVideoFormat_HEVC)) {
        return PixelFormat::HEVC;
    } else if (IsEqualGUID(guid, MFVideoFormat_HEVC_ES)) {
        return PixelFormat::HEVC_ES;
    } else if (IsEqualGUID(guid, MFVideoFormat_M4S2)) {
        return PixelFormat::M4S2;
    } else if (IsEqualGUID(guid, MFVideoFormat_MJPG)) {
        return PixelFormat::MJPG;
    } else if (IsEqualGUID(guid, MFVideoFormat_MP43)) {
        return PixelFormat::MP43;
    } else if (IsEqualGUID(guid, MFVideoFormat_MP4S)) {
        return PixelFormat::MP4S;
    } else if (IsEqualGUID(guid, MFVideoFormat_MP4V)) {
        return PixelFormat::MP4V;
    } else if (IsEqualGUID(guid, MFVideoFormat_MPEG2)) {
        return PixelFormat::MPEG2;
    } else if (IsEqualGUID(guid, MFVideoFormat_MPG1)) {
        return PixelFormat::MPG1;
    } else if (IsEqualGUID(guid, MFVideoFormat_MSS1)) {
        return PixelFormat::MSS1;
    } else if (IsEqualGUID(guid, MFVideoFormat_MSS2)) {
        return PixelFormat::MSS2;
    } else if (IsEqualGUID(guid, MFVideoFormat_WMV1)) {
        return PixelFormat::WMV1;
    } else if (IsEqualGUID(guid, MFVideoFormat_WMV2)) {
        return PixelFormat::WMV2;
    } else if (IsEqualGUID(guid, MFVideoFormat_WMV3)) {
        return PixelFormat::WMV3;
    } else if (IsEqualGUID(guid, MFVideoFormat_WVC1)) {
        return PixelFormat::WVC1;
    } else if (IsEqualGUID(guid, MFVideoFormat_420O)) {
        return PixelFormat::O420;
    } else {
        return PixelFormat::UNKNOWN;
    }
}
} // namespace webcam_capture

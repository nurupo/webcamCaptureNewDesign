#include "direct_show_utils.h"

#include <pixel_format.h>

#include <dshow.h>
#include <wmcodecdsp.h>

namespace webcam_capture {

// available formats - https://msdn.microsoft.com/en-us/library/windows/desktop/dd407353(v=vs.85).aspx
// Convert the DirectShow format to one we can use.
PixelFormat direct_show_video_format_to_capture_format(GUID guid)
{
    /// Analog Video Subtypes
    if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_NTSC_M)) {
        return PixelFormat::AnalogVideo_NTSC_M;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_B)) {
        return PixelFormat::AnalogVideo_PAL_B;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_D)) {
        return PixelFormat::AnalogVideo_PAL_D;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_G)) {
        return PixelFormat::AnalogVideo_PAL_G;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_H)) {
        return PixelFormat::AnalogVideo_PAL_H;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_I)) {
        return PixelFormat::AnalogVideo_PAL_I;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_M)) {
        return PixelFormat::AnalogVideo_PAL_M;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_N)) {
        return PixelFormat::AnalogVideo_PAL_N;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_B)) {
        return PixelFormat::AnalogVideo_SECAM_B;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_D)) {
        return PixelFormat::AnalogVideo_SECAM_D;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_G)) {
        return PixelFormat::AnalogVideo_SECAM_G;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_H)) {
        return PixelFormat::AnalogVideo_SECAM_H;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_K)) {
        return PixelFormat::AnalogVideo_SECAM_K;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_K1)) {
        return PixelFormat::AnalogVideo_SECAM_K1;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_L)) {
        return PixelFormat::AnalogVideo_SECAM_L;
    }

    /// DirectX Video Acceleration Video Subtypes
    else if (IsEqualGUID(guid, MEDIASUBTYPE_AI44)) {
        return PixelFormat::AI44;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IA44)) {
        return PixelFormat::IA44;
    }

    /// DV Video Subtypes
    //  Consumer Formats
    else if (IsEqualGUID(guid, MEDIASUBTYPE_dvsl)) {
        return PixelFormat::DVSL;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_dvsd)) {
        return PixelFormat::DVSD;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_dvhd)) {
        return PixelFormat::DVHD;
    }
    //  Professional Formats
    else if (IsEqualGUID(guid, MEDIASUBTYPE_dv25)) {
        return PixelFormat::DV25;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_dv50)) {
        return PixelFormat::DV50;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_dvh1)) {
        return PixelFormat::DVH1;
    }

    ///H.264 Video Types
    else if (IsEqualGUID(guid, MEDIASUBTYPE_H264)) {
        return PixelFormat::H264;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AVC1)) {
        return PixelFormat::AVC1;
    //Equivalent to MEDIASUBTYPE_H264, with a different FOURCC.
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_h264)) {
        return PixelFormat::H264;
    //Equivalent to MEDIASUBTYPE_H264, with a different FOURCC.
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_X264)) {
        return PixelFormat::H264;
    //Equivalent to MEDIASUBTYPE_H264, with a different FOURCC.
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_x264)) {
        return PixelFormat::H264;
    }

    ///Uncompressed RGB Video Subtypes
    // Uncompressed RGB formats with no alpha channel.
    else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB1)) {
        return PixelFormat::RGB1;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB4)) {
        return PixelFormat::RGB4;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB8)) {
        return PixelFormat::RGB8;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB555)) {
        return PixelFormat::RGB555;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB565)) {
        return PixelFormat::RGB565;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB24)) {
        return PixelFormat::RGB24;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB32)) {
        return PixelFormat::RGB32;
    }
    //  Uncompressed RGB formats with alpha channel.
    else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB1555)) {
        return PixelFormat::ARGB1555;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB32)) {
        return PixelFormat::ARGB32;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB4444)) {
        return PixelFormat::ARGB4444;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_A2R10G10B10)) {
        return PixelFormat::A2R10G10B10;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_A2B10G10R10)) {
        return PixelFormat::A2B10G10R10;
    }



    ///Video Mixing Renderer Video Subtypes
    // VMR-7 Subtypes
    else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB32_D3D_DX7_RT)) {
        return PixelFormat::RGB32_D3D_DX7_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB16_D3D_DX7_RT)) {
        return PixelFormat::RGB16_D3D_DX7_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB32_D3D_DX7_RT)) {
        return PixelFormat::ARGB32_D3D_DX7_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB4444_D3D_DX7_RT)) {
        return PixelFormat::ARGB4444_D3D_DX7_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB1555_D3D_DX7_RT)) {
        return PixelFormat::ARGB1555_D3D_DX7_RT;
    // VMR-9 Subtypes
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB32_D3D_DX9_RT)) {
        return PixelFormat::RGB32_D3D_DX9_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB16_D3D_DX9_RT)) {
        return PixelFormat::RGB16_D3D_DX9_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB32_D3D_DX9_RT)) {
        return PixelFormat::ARGB32_D3D_DX9_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB4444_D3D_DX9_RT)) {
        return PixelFormat::ARGB4444_D3D_DX9_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB1555_D3D_DX9_RT)) {
        return PixelFormat::ARGB1555_D3D_DX9_RT;
    }


    /// YUV Video Subtypes
    // Recommended YUV Formats
    else if (IsEqualGUID(guid, MEDIASUBTYPE_AYUV))     {
        return PixelFormat::AYUV;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_YUY2)) {
        return PixelFormat::YUY2;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_UYVY)) {
        return PixelFormat::UYVY;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IMC1)) {
        return PixelFormat::IMC1;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IMC2)) {
        return PixelFormat::IMC2;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IMC3)) {
        return PixelFormat::IMC3;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IMC4)) {
        return PixelFormat::IMC4;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_YV12)) {
        return PixelFormat::YV12;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_NV12)) {
        return PixelFormat::NV12;

    // Other YUV Format Types
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_I420)) {
        return PixelFormat::I420;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IF09)) {
        return PixelFormat::IF09;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IYUV)) {
        return PixelFormat::IYUV;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_Y211)) {
        return PixelFormat::Y211;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_Y411)) {
        return PixelFormat::Y411;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_Y41P)) {
        return PixelFormat::Y41P;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_YVU9)) {
        return PixelFormat::YVU9;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_YVYU)) {
        return PixelFormat::YVYU;

    /// Miscellaneous Video Subtypes
    // The following table lists some additional video subtypes.
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_CFCC)) {
        return PixelFormat::CFCC;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_CLJR)) {
        return PixelFormat::CLJR;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_CPLA)) {
        return PixelFormat::CPLA;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_CLPL)) {
        return PixelFormat::CLPL;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IJPG)) {
        return PixelFormat::IJPG;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_MDVF)) {
        return PixelFormat::MDVF;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_MJPG)) {
        return PixelFormat::MJPG;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_Overlay)) {
        return PixelFormat::Overlay;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_Plum)) {
        return PixelFormat::Plum;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_QTJpeg)) {
        return PixelFormat::QTJpeg;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_QTMovie)) {
        return PixelFormat::QTMovie;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_QTRle)) {
        return PixelFormat::QTRle;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_QTRpza)) {
        return PixelFormat::QTRpza;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_QTSmc)) {
        return PixelFormat::QTSmc;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_TVMJ)) {
        return PixelFormat::TVMJ;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_VPVBI)) {
        return PixelFormat::VPVBI;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_VPVideo)) {
        return PixelFormat::VPVideo;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_WAKE)) {
        return PixelFormat::WAKE;
    }

    else {
        return PixelFormat::UNKNOWN;
    }
}
} // namespace webcam_capture

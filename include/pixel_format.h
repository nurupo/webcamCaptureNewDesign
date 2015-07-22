#ifndef PIXEL_FORMAT_H
#define PIXEL_FORMAT_H

#include <webcam_capture_export.h>

/* Media Foundation
 *   - Formats: https://msdn.microsoft.com/en-us/library/windows/desktop/aa370819(v=vs.85).aspx
 *   - Description: https://msdn.microsoft.com/en-us/library/windows/desktop/dd206750(v=vs.85).aspx
 * Direct Show
 *   - https://msdn.microsoft.com/en-us/library/windows/desktop/dd407353(v=vs.85).aspx
 */

namespace webcam_capture {

/**
 * Supported pixel formats.
 */
enum class WEBCAM_CAPTURE_EXPORT PixelFormat {
    //Uncompressed RGB Formats
    RGB1,           /* RGB, 1 bit per pixel (bpp), palettized*/
    RGB4,           /* RGB, 4 bpp, palettized*/
    RGB8,           /* RGB, 8 bits per pixel (bpp)*/
    RGB555,         /* RGB 555, 16 bpp*/
    RGB565,         /* RGB 565, 16 bpp*/
    RGB24,          /* RGB, 24 bpp*/
    RGB32,          /* RGB, 32 bpp*/
    ARGB1555,       /* RGB 555 with alpha channel*/
    ARGB32,         /* RGB, 32 bpp with alpha channel*/
    ARGB4444,       /* 16-bit RGB with alpha channel; 4 bits per channel*/
    A2R10G10B10,    /* 32-bit RGB with alpha channel; 10 bits per RGB channel plus 2 bits for alpha.*/
    A2B10G10R10,    /* 32-bit RGB with alpha channel; 10 bits per RGB channel plus 2 bits for alpha.*/
    //YUV Formats: 8-Bit and Palettized
    IA44,           /* For subpicture and text overlays. See previous description.*/
    AI44,           /* AI44, Sampling 4:4:4, Packed, Bits per channel - Palettized*/
    AYUV,           /* AYUV, Sampling 4:4:4, Packed, Bits per channel - 8*/
    I420,           /* I420, Sampling 4:2:0, Planar, Bits per channel - 8*/
    IYUV,           /* IYUV, Sampling 4:2:0, Planar, Bits per channel - 8*/
    NV11,           /* NV11, Sampling 4:1:1, Planar, Bits per channel - 8*/
    NV12,           /* NV12, Sampling 4:2:0, Planar, Bits per channel - 8*/
    UYVY,           /* UYVY, Sampling 4:2:2, Packed, Bits per channel - 8*/
    Y211,           /* Y211, Sampling "See remarks.", Packed, Bits per channel - 8*/
    Y411,           /* Y411, Sampling 4:1:1, Packed, Bits per channel - 8*/
    Y41P,           /* Y41P, Sampling 4:1:1, Packed, Bits per channel - 8*/
    Y41T,           /* Y41T, Sampling 4:1:1, Packed, Bits per channel - 8*/
    Y42T,           /* Y42T, Sampling 4:2:2, Packed, Bits per channel - 8*/
    YUY2,           /* YUY2, Sampling 4:2:2, Packed, Bits per channel - 8*/
    YV12,           /* YV12, Sampling 4:2:0, Planar, Bits per channel - 8*/
    IMC1,           /* IMC1, Sampling 4:2:0, Planar, Bits per channel - 8*/
    IMC2,           /* IMC2, Sampling 4:2:0, Planar, Bits per channel - 8*/
    IMC3,           /* IMC3, Sampling 4:2:0, Planar, Bits per channel - 8*/
    IMC4,           /* IMC4, Sampling 4:2:0, Planar, Bits per channel - 8*/
    IF09,           /* IF09, Sampling 4:2:0, Planar, Bits per channel - 8*/
    YVU9,           /* Y211, Sampling "See remarks.", Planar, Bits per channel - 8*/
    YVYU,           /* YVYU, Sampling 4:2:2, Packed, Bits per channel - 8*/
    //YUV Formats: 10-Bit and 16-Bit
    P010,           /* P010, Sampling 4:2:0, Planar, Bits per channel - 10*/
    P016,           /* P016, Sampling 4:2:0, Planar, Bits per channel - 16*/
    P210,           /* P210, Sampling 4:2:2, Planar, Bits per channel - 10*/
    P216,           /* P216, Sampling 4:2:2, Planar, Bits per channel - 16*/
    v210,           /* v210, Sampling 4:2:2, Packed, Bits per channel - 10*/
    v216,           /* v216, Sampling 4:2:2, Packed, Bits per channel - 16*/
    v40,            /* v410 or (v40), Sampling 4:4:4, Packed, Bits per channel - 10*/
    Y210,           /* Y210, Sampling 4:2:2, Packed, Bits per channel - 10*/
    Y216,           /* Y216, Sampling 4:2:2, Packed, Bits per channel - 16*/
    Y40,            /* Y410 or (Y40), Sampling 4:4:4, Packed, Bits per channel - 10*/
    Y416,           /* Y416, Sampling 4:4:4, Packed, Bits per channel - 16*/
    //Encoded Video Types
    DV25,           /* DVCPRO 25 (525-60 or 625-50)*/
    DV50,           /* DVCPRO 50 (525-60 or 625-50)*/
    DVC,            /* DVC/DV Video*/
    DVH1,           /* DVCPRO 100 (1080/60i, 1080/50i, or 720/60P)*/
    DVHD,           /* HD-DVCR (1125-60 or 1250-50)*/
    DVSD,           /* SDL-DVCR (525-60 or 625-50)*/
    DVSL,           /* SD-DVCR (525-60 or 625-50)*/
    H263,           /* H.263 video*/
    AVC1,           /* H.264 bitstream without start codes.*/
    H264,           /* H.264 video*/
    H264_ES,        /* H.264 elementary stream*/
    HEVC,           /* The HEVC Main profile and Main Still Picture profile.
                                            Each sample contains one complete picture*/
    HEVC_ES,        /* This media type is the same as HEVC, except media samples contain a fragmented HEVC bitstream.*/
    M4S2,           /* MPEG-4 part 2 video*/
    MJPG,           /* Motion JPEG*/
    MP43,           /* Microsoft MPEG 4 codec version 3. This codec is no longer supported*/
    MP4S,           /* ISO MPEG 4 codec version 1*/
    MP4V,           /* MPEG-4 part 2 video*/
    MPEG2,          /* MPEG-2 video*/
    MPG1,           /* MPEG-1 video*/
    MSS1,           /* Windows Media Screen codec version 1*/
    MSS2,           /* Windows Media Video 9 Screen codec*/
    WMV1,           /* Windows Media Video codec version 7*/
    WMV2,           /* Windows Media Video 8 codec*/
    WMV3,           /* Windows Media Video 9 codec*/
    WVC1,           /* SMPTE 421M ("VC-1")*/
    O420,           /* "420O" 8-bit per channel planar YUV 4:2:0 video*/

    ///Video Mixing Renderer Video Subtypes
    //VMR-7 Subtypes
    RGB32_D3D_DX7_RT,       /* 32-bit RGB render target.*/
    RGB16_D3D_DX7_RT,       /* 16-bit RGB render target.*/
    ARGB32_D3D_DX7_RT,      /* 32-bit ARGB render target.*/
    ARGB4444_D3D_DX7_RT,    /* ARGB4444 render target. For subpicture graphics.*/
    ARGB1555_D3D_DX7_RT,    /* ARGB1555 render target. For subpicture graphics.*/
    //VMR-9 Subtypes
    RGB32_D3D_DX9_RT,       /* 32-bit RGB render target.*/
    RGB16_D3D_DX9_RT,       /* 16-bit RGB render target.*/
    ARGB32_D3D_DX9_RT,      /* 32-bit ARGB render target.*/
    ARGB4444_D3D_DX9_RT,    /* ARGB4444 render target. For subpicture graphics.*/
    ARGB1555_D3D_DX9_RT,    /* ARGB1555 render target. For subpicture graphics.*/

    ///Miscellaneous Video Subtypes
    CFCC,           /* MJPG format produced by some cards. (FOURCC 'CFCC')*/
    CLJR,           /* Cirrus Logic CLJR format. (FOURCC 'CLJR')*/
    CPLA,           /* Cinepak UYVY format. (FOURCC 'CPLA')*/
    CLPL,           /* A YUV format supported by some Cirrus Logic drivers. (FOURCC 'CLPL')*/
    IJPG,           /* Intergraph JPEG format. (FOURCC 'IJPG')*/
    MDVF,           /* A DV encoding format. (FOURCC 'MDVF')*/
    Overlay,        /* Video delivered using hardware overlay.*/
    Plum,           /* Plum MJPG format. (FOURCC 'Plum')*/
    QTJpeg,         /* QuickTime JPEG compressed data.*/
    QTMovie,        /* Apple® QuickTime® compression.*/
    QTRle,          /* QuickTime RLE compressed data.*/
    QTRpza,         /* QuickTime RPZA compressed data.*/
    QTSmc,          /* QuickTime SMC compressed data.*/
    TVMJ,           /* TrueVision MJPG format. (FOURCC 'TVMJ')*/
    VPVBI,          /* Video port vertical blanking interval (VBI) data.*/
    VPVideo,        /* Video port video data.*/
    WAKE,           /* MJPG format produced by some cards. (FOURCC 'WAKE')*/

    /// Analog Video Subtypes
    AnalogVideo_NTSC_M,     /*(M) NTSC*/
    AnalogVideo_PAL_B,      /*(B) PAL*/
    AnalogVideo_PAL_D,      /*(D) PAL*/
    AnalogVideo_PAL_G,      /*(G) PAL*/
    AnalogVideo_PAL_H,      /*(H) PAL*/
    AnalogVideo_PAL_I,      /*(I) PAL*/
    AnalogVideo_PAL_M,      /*(M) PAL*/
    AnalogVideo_PAL_N,      /*(N) PAL*/
    AnalogVideo_SECAM_B,    /*(B) SECAM*/
    AnalogVideo_SECAM_D,    /*(D) SECAM*/
    AnalogVideo_SECAM_G,    /*(G) SECAM*/
    AnalogVideo_SECAM_H,    /*(H) SECAM*/
    AnalogVideo_SECAM_K,    /*(K) SECAM*/
    AnalogVideo_SECAM_K1,   /*(K1) SECAM*/
    AnalogVideo_SECAM_L,    /*(L) SECAM*/

    UNKNOWN         /* Unknown format*/
};

} // namespace webcam_capture

#endif // PIXEL_FORMAT_H


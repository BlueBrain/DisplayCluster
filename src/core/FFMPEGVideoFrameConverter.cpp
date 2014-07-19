/*********************************************************************/
/* Copyright (c) 2014, EPFL/Blue Brain Project                       */
/*                     Raphael Dumusc <raphael.dumusc@epfl.ch>       */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of The University of Texas at Austin.                 */
/*********************************************************************/

#include "FFMPEGVideoFrameConverter.h"

#include "log.h"

FFMPEGVideoFrameConverter::FFMPEGVideoFrameConverter(const AVCodecContext& videoCodecContext,
                                                     const PixelFormat targetFormat)
    : swsContext_(0)
    , avFrameRGB_(0)
{
    // allocate video frame for RGB conversion
#if(LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,0))
    avFrameRGB_ = avcodec_alloc_frame();
#else
    avFrameRGB_ = av_frame_alloc();
#endif

    if( !avFrameRGB_ )
    {
        put_flog(LOG_ERROR, "Error allocating frame");
        return;
    }

    // alloc buffer for avFrameRGB_
    if (avpicture_alloc( (AVPicture *)avFrameRGB_, targetFormat,
                         videoCodecContext.width, videoCodecContext.height ) != 0)
    {
        put_flog(LOG_ERROR, "Error allocating frame");
        return;
    }

    // create sws scaler context
    swsContext_ = sws_getContext(videoCodecContext.width, videoCodecContext.height,
                                 videoCodecContext.pix_fmt, videoCodecContext.width,
                                 videoCodecContext.height, targetFormat, SWS_FAST_BILINEAR,
                                 NULL, NULL, NULL);
    if( !swsContext_ )
    {
        put_flog(LOG_ERROR, "Error allocating an SwsContext");
        return;
    }
}

FFMPEGVideoFrameConverter::~FFMPEGVideoFrameConverter()
{
    sws_freeContext(swsContext_);

    avpicture_free( (AVPicture *)avFrameRGB_ );
    av_free(avFrameRGB_);
}

bool FFMPEGVideoFrameConverter::convert(const AVFrame* srcFrame)
{
    const int output_height = sws_scale(swsContext_, srcFrame->data,
                                        srcFrame->linesize, 0, srcFrame->height,
                                        avFrameRGB_->data,
                                        avFrameRGB_->linesize);
    return output_height == srcFrame->height;
}

const uint8_t* FFMPEGVideoFrameConverter::getData() const
{
    return avFrameRGB_->data[0];
}

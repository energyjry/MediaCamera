#include "DecodeMjpeg.h"

bool DecodeMjpeg::InitMjpegDecoder(int width, int height)
{
    nVideoWidth = width;
    nVideoHeight = height;

    const AVCodec* decodec = avcodec_find_decoder(AV_CODEC_ID_MJPEG);;
    if (!decodec)
    {
        printf("Failed to create MJPEG Decoder!\n");
        return false;
    }
    pMjpegDecodecCtx = avcodec_alloc_context3(decodec);
    if (!pMjpegDecodecCtx)
    {
        printf("Failed to create MJPEG Decoder Ctx!\n");
        return false;
    }
    pMjpegDecodecCtx->width = width;
    pMjpegDecodecCtx->height = height;
    
    int ret = avcodec_open2(pMjpegDecodecCtx, decodec, NULL);
    if (ret < 0)
    {
        printf("Failed to open the codec\n");
        return false;
    }

    pMjpegPacket = av_packet_alloc();
    if (!pMjpegPacket)
    {
        printf("Could not open H265 codec!\n");
        return false;
    }
    pYuvFrame = av_frame_alloc();
    if (!pYuvFrame)
    {
        printf("av_frame_alloc failed!\n");
        return false;
    }

    pSwsCtx = sws_getContext(nVideoWidth, nVideoHeight, AV_PIX_FMT_YUVJ420P, nVideoWidth, nVideoHeight, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    if (!pSwsCtx)
    {
        printf("sws_getContext failed\n");
        return false;
    }

    nYuvSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, nVideoWidth, nVideoHeight, 1);
    pYuvBuffer = (uint8_t*)av_malloc(nYuvSize);

    mYuvFile = fopen("usb.yuv", "wb");
    if (mYuvFile == nullptr) {
        printf("open %s failed!\n", "rtsp.yuv");
        return false;
    }

    return true;
}

bool DecodeMjpeg::GetYuvData(char* mjpegData, int mjpegSize, AVFrame* pYuv420pFrame)
{
    int ret = 0;
    pMjpegPacket->size = mjpegSize;
    pMjpegPacket->data = (uint8_t*)av_malloc(pMjpegPacket->size);
    memcpy(pMjpegPacket->data, mjpegData, mjpegSize);
    ret = av_packet_from_data(pMjpegPacket, pMjpegPacket->data, pMjpegPacket->size);
    if (ret < 0)
    {
        printf("av_packet_from_data error!\n");
        av_free(pMjpegPacket->data);
        return false;
    }
    ret = avcodec_send_packet(pMjpegDecodecCtx, pMjpegPacket);
    if (ret < 0)
    {
        printf("avcodec_send_packet error!\n");
        av_free(pMjpegPacket->data);
        return false;
    }
    ret = avcodec_receive_frame(pMjpegDecodecCtx, pYuvFrame);
    if (ret < 0)
    {
        printf("avcodec_receive_frame error!\n");
        return false;
    }

    if (pYuvFrame->format == AV_PIX_FMT_YUVJ420P)
    {
        av_image_fill_arrays(pYuv420pFrame->data, pYuv420pFrame->linesize, pYuvBuffer, AV_PIX_FMT_YUV420P, nVideoWidth, nVideoHeight, 1);
        sws_scale(pSwsCtx, pYuvFrame->data, pYuvFrame->linesize, 0, nVideoHeight, pYuv420pFrame->data, pYuv420pFrame->linesize);
        fwrite((char*)pYuvBuffer, 1, nYuvSize, mYuvFile);
        //av_free(sw_yuvbuf);
    }
    av_packet_unref(pMjpegPacket);
    return true;
}

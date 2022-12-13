// RtspCamera.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "RtspReader.h"
#include "DecodeH264.h"
#include "DecodeH265.h"
#include "EncodeH264.h"
#include "DecodeAac.h"
#include "EncodeAac.h"
#include "UsbReader.h"
#include "DecodeMjpeg.h"
#include "OpencvReader.h"
#include "YuvConverter.h"
#include "Mp4Reader.h"

int testH264_H264()
{
    std::string RTSP_URL = "rtsp://admin:l1234567@10.0.16.118:554/Streaming/Channels/101";
    RtspReader mRtspReader;
    mRtspReader.InitRtspReader(RTSP_URL);
    int h264Width = 0;
    int h264Height = 0;
    AVCodecID nCodecId;
    mRtspReader.GetVideoInfo(h264Width, h264Height, nCodecId);
    char* videoData = new char[1024 * 1000];
    memset(videoData, 0, 1024 * 1000);
    int videoSize = 0;


    DecodeH264 mDecodeH264;
    mDecodeH264.InitH264Decoder(h264Width, h264Height);
    AVFrame* pYuv420pFrame = av_frame_alloc();
    pYuv420pFrame->format = AV_PIX_FMT_YUV420P;
    pYuv420pFrame->width = h264Width;
    pYuv420pFrame->height = h264Height;
    int ret = av_frame_get_buffer(pYuv420pFrame, 1);
    if (ret < 0)
    {
        printf("Failed to get Frame Buffer!\n");
        av_frame_free(&pYuv420pFrame);
        return -1;
    }

    EncodeH264 mEncoderH264;
    mEncoderH264.InitH264Encoder(h264Width, h264Height, 25);
    char* h264Data = new char[1024 * 1000];
    memset(h264Data, 0, 1024 * 1000);
    int h264Size = 0;

    while (true)
    {
        mRtspReader.GetVideoData(videoData, videoSize);
        printf("get video data:%d\n", videoSize);
        mDecodeH264.GetYuvData(videoData, videoSize, pYuv420pFrame);
        mEncoderH264.GetH264Data(pYuv420pFrame, h264Data, h264Size);
        printf("encode h264 data:%d\n", h264Size);
        int n = 0;
    }
}

int testH265_H264()
{
    std::string RTSP_URL = "rtsp://admin:aykj1234@10.0.16.190:554/Streaming/Channels/101";
    RtspReader mRtspReader;
    mRtspReader.InitRtspReader(RTSP_URL);
    int h265Width = 0;
    int h265Height = 0;
    AVCodecID nCodecId;
    mRtspReader.GetVideoInfo(h265Width, h265Height, nCodecId);
    char* videoData = new char[1024 * 1000];
    memset(videoData, 0, 1024 * 1000);
    int videoSize = 0;


    DecodeH265 mDecodeH265;
    mDecodeH265.InitH265Decoder(h265Width, h265Height);
    AVFrame* pYuv420pFrame = av_frame_alloc();
    pYuv420pFrame->format = AV_PIX_FMT_YUV420P;
    pYuv420pFrame->width = h265Width;
    pYuv420pFrame->height = h265Height;
    int ret = av_frame_get_buffer(pYuv420pFrame, 1);
    if (ret < 0)
    {
        printf("Failed to get Frame Buffer!\n");
        av_frame_free(&pYuv420pFrame);
        return -1;
    }

    EncodeH264 mEncoderH264;
    mEncoderH264.InitH264Encoder(h265Width, h265Height, 25);
    char* h264Data = new char[1024 * 1000];
    memset(h264Data, 0, 1024 * 1000);
    int h264Size = 0;

    while (true)
    {
        mRtspReader.GetVideoData(videoData, videoSize);
        printf("get video data:%d\n", videoSize);
        mDecodeH265.GetYuvData(videoData, videoSize, pYuv420pFrame);
        mEncoderH264.GetH264Data(pYuv420pFrame, h264Data, h264Size);
        printf("encode h264 data:%d\n", h264Size);
        int n = 0;
    }
}

int testAAC_AAC()
{
    std::string RTSP_URL = "rtsp://admin:aykj1234@10.0.16.190:554/Streaming/Channels/101";
   
    RtspReader mRtspReader;
    mRtspReader.InitRtspReader(RTSP_URL);
    int nSampleRate;
    int nSampleChannels;
    mRtspReader.GetAudioInfo(nSampleRate, nSampleChannels);
    
    DecodeAac mDecodeAac;
    mDecodeAac.InitAacDecoder(nSampleRate, nSampleChannels);
    char* audioData = new char[1024 * 1000];
    memset(audioData, 0, 1024 * 1000);
    int audioSize = 0;

    char* pcmData = new char[1024 * 1000];
    memset(pcmData, 0, 1024 * 1000);
    int pcmSize = 0;

    EncodeAac mEncodeAac;
    mEncodeAac.InitAacEncoder(nSampleRate, nSampleChannels);

    while (true)
    {
        mRtspReader.GetAudioData(audioData, audioSize);
        printf("get audio data:%d\n", audioSize);
        mDecodeAac.GetPcmData(audioData, audioSize, pcmData, pcmSize);
        printf("encode pcm data:%d\n", pcmSize);
    }
    return 0;
}

int testMjpeg_H264()
{
    UsbReader mUsbReader;
    mUsbReader.InitUsbVideo("FINGERS 720 Hi-Res Webcam");
    int mjpegWidth = 0;
    int mjpegHeight = 0;
    AVCodecID nCodecId;
    mUsbReader.GetVideoInfo(mjpegWidth, mjpegHeight, nCodecId);
    char* videoData = new char[1024 * 10000];
    memset(videoData, 0, 1024 * 10000);
    int videoSize = 0;

    DecodeMjpeg mDecodeMjpeg;
    mDecodeMjpeg.InitMjpegDecoder(mjpegWidth, mjpegHeight);
    AVFrame* pYuv420pFrame = av_frame_alloc();
    pYuv420pFrame->format = AV_PIX_FMT_YUV420P;
    pYuv420pFrame->width = mjpegWidth;
    pYuv420pFrame->height = mjpegHeight;
    int ret = av_frame_get_buffer(pYuv420pFrame, 1);
    if (ret < 0)
    {
        printf("Failed to get Frame Buffer!\n");
        av_frame_free(&pYuv420pFrame);
        return -1;
    }

    EncodeH264 mEncoderH264;
    mEncoderH264.InitH264Encoder(mjpegWidth, mjpegHeight, 25);
    char* h264Data = new char[1024 * 1000];
    memset(h264Data, 0, 1024 * 1000);
    int h264Size = 0;

    while (true)
    {
        mUsbReader.GetVideoData(videoData, videoSize);
        printf("get video data:%d\n", videoSize);
        mDecodeMjpeg.GetYuvData(videoData, videoSize, pYuv420pFrame);
        printf("get yuv data:%d\n", videoSize);
        mEncoderH264.GetH264Data(pYuv420pFrame, h264Data, h264Size);
        printf("encode h264 data:%d\n", h264Size);
        int n = 0;
    }
    return 1;
}

int testPcm_AAC()
{
    UsbReader mUsbReader;
    mUsbReader.InitUsbAudio("麦克风 (USB Microphone)");
    int nSampleRate;
    int nSampleChannels;
    mUsbReader.GetAudioInfo(nSampleRate, nSampleChannels);

    char* audioData = new char[1024 * 1000];
    memset(audioData, 0, 1024 * 1000);
    int audioSize = 0;

    while (true)
    {
        mUsbReader.GetAudioData(audioData, audioSize);
        printf("get audio data:%d\n", audioSize);
    }

    return 0;
}

int testCV_H264()
{
    OpencvReader mOpencvReader;
    mOpencvReader.InitOpencv("rtsp://admin:admin1234@10.0.30.31:554/Streaming/Channels/101");
    int mjpegWidth = 0;
    int mjpegHeight = 0;
    mOpencvReader.GetVideoInfo(mjpegWidth, mjpegHeight);
    char* videoData = new char[1024 * 10000];
    memset(videoData, 0, 1024 * 10000);
    int videoSize = 0;

    AVFrame* pYuv420pFrame = av_frame_alloc();
    pYuv420pFrame->format = AV_PIX_FMT_YUV420P;
    pYuv420pFrame->width = mjpegWidth;
    pYuv420pFrame->height = mjpegHeight;
    int ret = av_frame_get_buffer(pYuv420pFrame, 1);
    if (ret < 0)
    {
        printf("Failed to get Frame Buffer!\n");
        av_frame_free(&pYuv420pFrame);
        return -1;
    }

    EncodeH264 mEncoderH264;
    mEncoderH264.InitH264Encoder(mjpegWidth, mjpegHeight, 25);
    char* h264Data = new char[1024 * 1000];
    memset(h264Data, 0, 1024 * 1000);
    int h264Size = 0;

    while (true)
    {
        mOpencvReader.GetVideoFrame(pYuv420pFrame);
        mEncoderH264.GetH264Data(pYuv420pFrame, h264Data, h264Size);
        printf("encode h264 data:%d\n", h264Size);
        int n = 0;
    }
}

int testYuvConverter()
{
    OpencvReader mOpencvReader;
    mOpencvReader.InitOpencv("rtsp://admin:admin1234@10.0.30.31:554/Streaming/Channels/101");
    int mjpegWidth = 0;
    int mjpegHeight = 0;
    mOpencvReader.GetVideoInfo(mjpegWidth, mjpegHeight);
    char* videoData = new char[1024 * 10000];
    memset(videoData, 0, 1024 * 10000);
    int videoSize = 0;

    AVFrame* pYuv420pFrame = av_frame_alloc();
    pYuv420pFrame->format = AV_PIX_FMT_YUV420P;
    pYuv420pFrame->width = mjpegWidth;
    pYuv420pFrame->height = mjpegHeight;
    int ret = av_frame_get_buffer(pYuv420pFrame, 1);
    if (ret < 0)
    {
        printf("Failed to get Frame Buffer!\n");
        av_frame_free(&pYuv420pFrame);
        return -1;
    }

    YuvConverter mYuvConverter;
    int dstWidth = 1280;
    int dstHeight = 720;
    mYuvConverter.InitConverter(mjpegWidth, mjpegHeight, dstWidth, dstHeight);
    AVFrame* pYuv420pFrame2 = av_frame_alloc();
    pYuv420pFrame2->format = AV_PIX_FMT_YUV420P;
    pYuv420pFrame2->width = mjpegWidth;
    pYuv420pFrame2->height = mjpegHeight;
    ret = av_frame_get_buffer(pYuv420pFrame2, 1);
    if (ret < 0)
    {
        printf("Failed to get Frame Buffer!\n");
        av_frame_free(&pYuv420pFrame2);
        return -1;
    }

    EncodeH264 mEncoderH264;
    mEncoderH264.InitH264Encoder(dstWidth, dstHeight, 25);
    char* h264Data = new char[1024 * 1000];
    memset(h264Data, 0, 1024 * 1000);
    int h264Size = 0;

    while (true)
    {
        mOpencvReader.GetVideoFrame(pYuv420pFrame);
        mYuvConverter.ConvertWH(pYuv420pFrame, pYuv420pFrame2);
        mEncoderH264.GetH264Data(pYuv420pFrame2, h264Data, h264Size);
        printf("encode h264 data:%d\n", h264Size);
        int n = 0;
    }
}

int testMp4_H264()
{
    std::string mp4file = "weather.mp4";
    Mp4Reader mMp4Reader;
    mMp4Reader.InitMp4Reader(mp4file);
    int h264Width = 0;
    int h264Height = 0;
    AVCodecID nCodecId;
    mMp4Reader.GetVideoInfo(h264Width, h264Height, nCodecId);
    char* videoData = new char[1024 * 1000];
    memset(videoData, 0, 1024 * 1000);
    int videoSize = 0;

    DecodeH264 mDecodeH264;
    mDecodeH264.InitH264Decoder(h264Width, h264Height);
    AVFrame* pYuv420pFrame = av_frame_alloc();
    pYuv420pFrame->format = AV_PIX_FMT_YUV420P;
    pYuv420pFrame->width = h264Width;
    pYuv420pFrame->height = h264Height;
    int ret = av_frame_get_buffer(pYuv420pFrame, 1);
    if (ret < 0)
    {
        printf("Failed to get Frame Buffer!\n");
        av_frame_free(&pYuv420pFrame);
        return -1;
    }

    EncodeH264 mEncoderH264;
    mEncoderH264.InitH264Encoder(h264Width, h264Height, 25);
    char* h264Data = new char[1024 * 1000];
    memset(h264Data, 0, 1024 * 1000);
    int h264Size = 0;

    while (true)
    {
        mMp4Reader.GetVideoData(videoData, videoSize);
        printf("get video data:%d\n", videoSize);
        mDecodeH264.GetYuvData(videoData, videoSize, pYuv420pFrame);
        mEncoderH264.GetH264Data(pYuv420pFrame, h264Data, h264Size);
        printf("encode h264 data:%d\n", h264Size);
    }

    return 0;
}

int testMp4_AAC()
{
    std::string mp4file = "weather.mp4";
    Mp4Reader mMp4Reader;
    mMp4Reader.InitMp4Reader(mp4file);
    int nSampleRate;
    int nSampleChannels;
    mMp4Reader.GetAudioInfo(nSampleRate, nSampleChannels);

    char* audioData = new char[1024 * 1000];
    memset(audioData, 0, 1024 * 1000);
    int audioSize = 0;

    while (true)
    {
        mMp4Reader.GetAudioData(audioData, audioSize);
        printf("get audio data:%d\n", audioSize);
    }
    return 0;
}

int main()
{
    //testH264_H264();
    //testH265_H264();
    //testAAC_AAC();
    //testMjpeg_H264();
    //testPcm_AAC();
    //testCV_H264();
    //testYuvConverter();
    testMp4_H264();
    //testMp4_AAC();
    std::cout << "Hello World!\n";
}


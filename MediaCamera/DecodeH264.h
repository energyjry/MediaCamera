#pragma once
extern "C" {
#include <libavformat/avformat.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/pixdesc.h>
#include <libavutil/error.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavutil/timestamp.h>
#include <libavdevice/avdevice.h>
#include "libavutil/time.h"
};

#include <iostream>
#include <fstream>
#include <string.h>

class DecodeH264
{
private:
	int nVideoWidth;
	int nVideoHeight;
	AVCodec* pH264Codec;
	AVCodecContext* pH264CodecCtx;
	AVPacket* pH264Packet;
	AVFrame* pYuvFrame;			//视频图像格式转换：AV_PIX_FMT_YUVJ420P ---> AV_PIX_FMT_YUV420P
	SwsContext* pSwsCtx;
	FILE* mYuvFile;
	int nYuvSize;
	uint8_t* pYuvBuffer;
private:

public:
	bool InitH264Decoder(int width, int height);
	bool GetYuvData(char* h264Data, int h264Size, AVFrame* pYuv420pFrame);
};


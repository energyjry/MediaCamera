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

class EncodeH264
{
private:
	AVCodec* pH264Codec;
	AVCodecContext* pH264CodecCtx;
	AVPacket* pH264Packet;
	char* pH264Buffer;
	FILE* mh264File;
	int nPts;
private:

public:
	bool InitH264Encoder(int width, int height, int frameRate);
	bool GetH264Data(AVFrame* yuvFrame, char*& pOutData, int& iSize);
};


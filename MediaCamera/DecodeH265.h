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

class DecodeH265
{
private:
	int nVideoWidth;
	int nVideoHeight;
	AVCodec* pH265Codec;
	AVCodecContext* pH265CodecCtx;
	AVPacket* pH265Packet;
	AVFrame* pYuvFrame;
	SwsContext* pSwsCtx;
	FILE* mYuvFile;
	int nYuvSize;
	uint8_t* pYuvBuffer;
private:

public:
	bool InitH265Decoder(int width, int height);
	bool GetYuvData(char* h265Data, int h265Size, AVFrame* pYuv420pFrame);
};


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

class DecodeMjpeg
{
private:
	AVCodecContext* pMjpegDecodecCtx;
	SwsContext* pSwsCtx;
	AVPacket* pMjpegPacket;
	AVFrame* pYuvFrame;
	int nVideoWidth;
	int nVideoHeight;
	FILE* mYuvFile;
	int nYuvSize;
	uint8_t* pYuvBuffer;
private:

public:
	bool InitMjpegDecoder(int width, int height);
	bool GetYuvData(char* mjpegData, int mjpegSize, AVFrame* pYuv420pFrame);
};


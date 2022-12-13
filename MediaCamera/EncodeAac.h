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

class EncodeAac
{
private:
	AVCodec* pAacCodec;
	AVCodecContext* pAacCodecCtx;
	AVPacket* pAacPacket;
	AVFrame* pPcmFrame;
	SwrContext* pSwrCtx;	//ÒôÆµÖØ²ÉÑù£ºAV_SAMPLE_FMT_S16 --> AV_SAMPLE_FMT_FLTP

	int nSampleRate;
	int nSampleChannels;
	int pts;

	char* pAacBuffer;
	FILE* mAacFile;
	int nPts;
private:

public:
	bool InitAacEncoder(int sampleRate, int sampleChannels);
	bool GetAacData(char* pInData, int iSize, char*& pOutData, int& oSize);
};


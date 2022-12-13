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

class DecodeAac
{
private:
	AVFormatContext* pAacFormatCtx;
	AVCodec* pAacCodec;
	AVCodecContext* pAacCodecCtx;
	AVPacket* pAacPacket;
	AVFrame* pPcmFrame;
	SwrContext* pSwrCtx;	//ÒôÆµÖØ²ÉÑù£ºAV_SAMPLE_FMT_FLTP --> AV_SAMPLE_FMT_S16
	int nSampleRate;
	int nSampleChannels;
	int pts;
	int nPcmSize;
	uint8_t** pPcmBuffer;
	FILE* mPcmFile;
public:
	bool InitAacDecoder(int sampleRate, int sampleChannels);
	bool GetPcmData(char* aacData, int aacSize, char*& pOutData, int& iSize);
};


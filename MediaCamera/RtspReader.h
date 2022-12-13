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

class RtspReader
{
private:
	AVFormatContext* pFormatCtx;
	int nVideoStreamIndex;
	int nAudioStreamIndex;

	AVCodecContext* pVideoCodecCtx;
	int nVideoWidth;
	int nVideoHeight;
	int nVideoFrameRate;
	AVCodecID nVideoCodecId;

	AVCodecContext* pAudioCodecCtx;
	int nAudiomSampleRate;
	int nAudioChannels;
	AVCodecID nAudioCodecId;

	FILE* mVideoFile;
	FILE* mAudioFile;
private:
	void get_adts_header(AVCodecContext* ctx, uint8_t* adts_header, int aac_length);
public:
	bool InitRtspReader(std::string rtspUrl);
	bool GetVideoInfo(int& videoWidth, int& videoHeight, AVCodecID& codecId);
	bool GetVideoData(char* pOutData, int& iSize);

	bool GetAudioInfo(int& sampleRate, int& sampleChannels);
	bool GetAudioData(char* pOutData, int& iSize);

};


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

class UsbReader
{
private:
	AVFormatContext* pVideoFormatCtx;
	int nVideoWidth;
	int nVideoHeight;
	int nVideoFrameRate;
	AVCodecID nVideoCodecId;
	int nVideoStreamIndex;

	AVFormatContext* pAudioFormatCtx;
	int nAudiomSampleRate;
	int nAudioChannels;
	AVCodecID nAudioCodecId;
	int nAudioStreamIndex;

	FILE* mVideoFile;
	FILE* mAudioFile;
	AVPacket* mJpegPacket;
private:
	std::string AnsiToUTF8(const char* _ansi, int _ansi_len);
public:
	bool InitUsbVideo(std::string videoDevice);
	bool GetVideoInfo(int& videoWidth, int& videoHeight, AVCodecID& codecId);
	bool GetVideoData(char* pOutData, int& iSize);

	bool InitUsbAudio(std::string audioDevice);
	bool GetAudioInfo(int& sampleRate, int& sampleChannels);
	bool GetAudioData(char* pOutData, int& iSize);
};


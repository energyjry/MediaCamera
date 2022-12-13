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

class YuvConverter
{
private:
	int mSrcWidth;
	int mSrcHeight;
	int mDstWidth;
	int mDstHeight;
private:
	SwsContext* pSwsCtx;
public:
	bool InitConverter(int sWidth, int sHeight, int dWidth, int dHeight);
	bool ConvertWH(AVFrame* sYuv420pFrame, AVFrame* dYuv420pFrame);

};


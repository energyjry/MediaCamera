#include "YuvConverter.h"

bool YuvConverter::InitConverter(int sWidth, int sHeight, int dWidth, int dHeight)
{
	mSrcWidth = sWidth;
	mSrcHeight = sHeight;
	mDstWidth = dWidth;
	mDstHeight = dHeight;

	pSwsCtx = sws_getContext(mSrcWidth, mSrcHeight, AV_PIX_FMT_YUV420P, mDstWidth, mDstHeight, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	if (!pSwsCtx)
	{
		printf("sws_getContext failed\n");
		return false;
	}
	return true;
}

bool YuvConverter::ConvertWH(AVFrame* sYuv420pFrame, AVFrame* dYuv420pFrame)
{
	sws_scale(pSwsCtx, sYuv420pFrame->data, sYuv420pFrame->linesize, 0, mSrcHeight, dYuv420pFrame->data, dYuv420pFrame->linesize);
	return true;
}

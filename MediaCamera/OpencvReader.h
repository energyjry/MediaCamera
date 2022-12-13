#pragma once
#include <iostream>
#include <fstream>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
#include <libavutil/error.h>
}

#include <opencv2/core/core.hpp> 
#include <opencv2/highgui.hpp> 
#include <opencv2/highgui/highgui_c.h> 
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

class OpencvReader
{
private:
	cv::VideoCapture m_capture;
	int nVideoWidth;
	int nVideoHeight;
	int nVideoFrameRate;

	char* pTempData;;
private:

public:
	bool InitOpencv(std::string path);
	bool GetVideoInfo(int& videoWidth, int& videoHeight);
	bool GetVideoData(char* pOutData, int& iSize);
	bool GetVideoFrame(AVFrame* pYuv420pFrame);
};


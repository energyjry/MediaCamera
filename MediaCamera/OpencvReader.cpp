#include "OpencvReader.h"

bool OpencvReader::InitOpencv(std::string path)
{
    if (!m_capture.open(path.c_str()))
    {
        return false;
    }
    nVideoWidth = (int)m_capture.get(cv::CAP_PROP_FRAME_WIDTH);
    nVideoHeight = (int)m_capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    nVideoFrameRate = (int)m_capture.get(cv::CAP_PROP_FPS);

    pTempData = new char[1024 * 10000];
    memset(pTempData, 0, 1024 * 10000);
	return true;
}

bool OpencvReader::GetVideoInfo(int& videoWidth, int& videoHeight)
{
    if (m_capture.isOpened())
    {
        videoWidth = nVideoWidth;
        videoHeight = nVideoHeight;
        return true;
    }
    else
    {
        return false;
    }
}

bool OpencvReader::GetVideoData(char* pOutData, int& iSize)
{
    cv::Mat frame;
    cv::Mat yuvFrame;
    while (m_capture.isOpened())
    {
        m_capture >> frame;
        if (frame.empty())
            continue;
        // 测试一些图像处理
        cv::Scalar color_blue(225, 47, 0);
        cv::putText(frame, "hello world", cv::Point(nVideoWidth / 2, nVideoHeight/2), cv::FONT_HERSHEY_SIMPLEX, 0.8, color_blue, 2);
        cv::circle(frame, cv::Point(nVideoWidth / 2, nVideoHeight / 2), 100, color_blue, 2);
        cv::cvtColor(frame, yuvFrame, CV_BGR2YUV_I420);
        iSize = nVideoWidth * nVideoHeight * 3 / 2;
        cv::imshow("111", frame);
        memcpy(pOutData, yuvFrame.data, iSize);
        cv::waitKey(1);
        return true;
    }
    return false;
}

bool OpencvReader::GetVideoFrame(AVFrame* pYuv420pFrame)
{
    int videoSize = 0;
    if (GetVideoData(pTempData, videoSize))
    {
        av_image_fill_arrays(pYuv420pFrame->data,
            pYuv420pFrame->linesize,
            (const uint8_t*)pTempData,
            AV_PIX_FMT_YUV420P,
            pYuv420pFrame->width,
            pYuv420pFrame->height,
            1);
        pYuv420pFrame->data[0] = (uint8_t*)pTempData;			//Y值
        pYuv420pFrame->data[1] = (uint8_t*)pTempData + pYuv420pFrame->width * pYuv420pFrame->height;	//U值
        pYuv420pFrame->data[2] = (uint8_t*)pTempData + pYuv420pFrame->width * pYuv420pFrame->height * 5 / 4;	//V值
        return true;
    }
    else {
        return false;
    }
}

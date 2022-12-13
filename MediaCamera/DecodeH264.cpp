#include "DecodeH264.h"

bool DecodeH264::InitH264Decoder(int width, int height)
{
	nVideoWidth = width;
	nVideoHeight = height;
	//m_YuvBuf = new char[width * height * 1.5];
	//codec = avcodec_find_encoder_by_name("hevc_cuvid");
	pH264Codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!pH264Codec) {
		printf("Codec not found AV_CODEC_ID_H264!\n");
		return false;
	}
	pH264CodecCtx = avcodec_alloc_context3(pH264Codec);
	if (!pH264CodecCtx) {
		printf("avcodec_alloc_context3 error!\n");
		return false;
	}
	if (avcodec_open2(pH264CodecCtx, pH264Codec, NULL) < 0) {
		printf("Could not open H264 codec!\n");
		return false;
	}
	pH264Packet = av_packet_alloc();
	if (!pH264Packet)
	{
		printf("Could not open H264 codec!\n");
		return false;
	}
	pYuvFrame = av_frame_alloc();
	if (!pYuvFrame)
	{
		printf("av_frame_alloc failed!\n");
		return false;
	}
	pSwsCtx = sws_getContext(nVideoWidth, nVideoHeight, AV_PIX_FMT_YUVJ420P, nVideoWidth, nVideoHeight, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	if (!pSwsCtx)
	{
		printf("sws_getContext failed\n");
		return false;
	}

	nYuvSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, nVideoWidth, nVideoHeight, 1);
	pYuvBuffer = (uint8_t*)av_malloc(nYuvSize);

	mYuvFile = fopen("rtsp.yuv", "wb");
	if (mYuvFile == nullptr) {
		printf("open %s failed!\n", "rtsp.yuv");
		return false;
	}
    return true;
}

bool DecodeH264::GetYuvData(char* h264Data, int h264Size, AVFrame* pYuv420pFrame)
{
	int ret = 0;
	pH264Packet->size = h264Size;
	pH264Packet->data = (uint8_t*)av_malloc(pH264Packet->size);
	memcpy(pH264Packet->data, h264Data, h264Size);
	ret = av_packet_from_data(pH264Packet, pH264Packet->data, pH264Packet->size);
	if (ret < 0)
	{
		printf("av_packet_from_data error!\n");
		av_free(pH264Packet->data);
		return false;
	}
	ret = avcodec_send_packet(pH264CodecCtx, pH264Packet);
	if (ret < 0)
	{
		printf("avcodec_send_packet error!\n");
		av_free(pH264Packet->data);
		return false;
	}
	ret = avcodec_receive_frame(pH264CodecCtx, pYuvFrame);
	if (ret < 0)
	{
		printf("avcodec_receive_frame error!\n");
		return false;
	}
	
	if (pYuvFrame->format == AV_PIX_FMT_YUVJ420P)
	{
		av_image_fill_arrays(pYuv420pFrame->data, pYuv420pFrame->linesize, pYuvBuffer, AV_PIX_FMT_YUV420P, nVideoWidth, nVideoHeight, 1);
		sws_scale(pSwsCtx, pYuvFrame->data, pYuvFrame->linesize, 0, nVideoHeight, pYuv420pFrame->data, pYuv420pFrame->linesize);
		fwrite((char*)pYuvBuffer, 1, nYuvSize, mYuvFile);
		//av_free(sw_yuvbuf);
	}
	av_packet_unref(pH264Packet);
	return true;
}

#include "EncodeH264.h"

bool EncodeH264::InitH264Encoder(int width, int height, int frameRate)
{
	//pH264Codec = avcodec_find_encoder_by_name("libx264");
	pH264Codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!pH264Codec)
	{
		printf("H264Encoder:Codec not found libx264 encoder!\n");
		return false;
	}
	// 编码器Context设置参数
	pH264CodecCtx = avcodec_alloc_context3(pH264Codec);
	pH264CodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pH264CodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pH264CodecCtx->width = width;
	pH264CodecCtx->height = height;
	pH264CodecCtx->time_base.num = 1;
	pH264CodecCtx->time_base.den = frameRate;
	pH264CodecCtx->bit_rate = 900000;
	//调整视频编码质量
	//极优_最接近原图(转换前后视频大小1:4)
	/*c->qmin = 10;
	c->qmax = 20;*/
	//优(转换前后视频大小1:2)
	/*c->qmin = 10;
	c->qmax = 25;*/
	//良(转换前后视频大小1:0.8)
	pH264CodecCtx->qmin = 10;
	pH264CodecCtx->qmax = 30;
	//一般(转换前后视频大小1:0.5)
	/*c->qmin = 10;
	c->qmax = 35;*/
	// 打开编码器
	if (avcodec_open2(pH264CodecCtx, pH264Codec, NULL) < 0)
	{
		printf("H264Encoder:Could not open H264 codec!\n");
		return false;
	}

	pH264Packet = av_packet_alloc();
	if (NULL == pH264Packet)
	{
		printf("H264Encoder:Could not alloc av_packet!\n");
		return false;
	}
	pH264Buffer = new char[1024 * 1000];

	mh264File = fopen("usb_out.h264", "wb");
	if (mh264File == nullptr) {
		printf("open %s failed!\n", "usb_out.h264");
		return false;
	}

	nPts = 0;
    return true;
}

bool EncodeH264::GetH264Data(AVFrame* yuvFrame, char*& pOutData, int& iSize)
{
	yuvFrame->pts = nPts++;
	int ret = avcodec_send_frame(pH264CodecCtx, yuvFrame);
	if (ret < 0)
	{
		printf("H264Encoder:Error sending a frame for encoding!\n");
		return false;
	}
	while (ret >= 0)
	{
		ret = avcodec_receive_packet(pH264CodecCtx, pH264Packet);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
			return false;
		}
		else if (ret < 0)
		{
			printf("H264Encoder:Error during encoding!\n");
			return false;
		}
		memcpy(pH264Buffer, pH264Packet->data, pH264Packet->size);
		fwrite((char*)pH264Packet->data, 1, pH264Packet->size, mh264File);
		pOutData = pH264Buffer;
		iSize = pH264Packet->size;
		
		av_packet_unref(pH264Packet);
		return true;
	}
	return true;
}

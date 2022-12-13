#include "Mp4Reader.h"

void Mp4Reader::get_adts_header(AVCodecContext* ctx, uint8_t* adts_header, int aac_length)
{
	uint8_t freq_idx = 0;    //0: 96000 Hz  3: 48000 Hz 4: 44100 Hz
	switch (ctx->sample_rate) {
	case 96000: freq_idx = 0; break;
	case 88200: freq_idx = 1; break;
	case 64000: freq_idx = 2; break;
	case 48000: freq_idx = 3; break;
	case 44100: freq_idx = 4; break;
	case 32000: freq_idx = 5; break;
	case 24000: freq_idx = 6; break;
	case 22050: freq_idx = 7; break;
	case 16000: freq_idx = 8; break;
	case 12000: freq_idx = 9; break;
	case 11025: freq_idx = 10; break;
	case 8000: freq_idx = 11; break;
	case 7350: freq_idx = 12; break;
	default: freq_idx = 4; break;
	}
	uint8_t chanCfg = ctx->channels;
	uint32_t frame_length = aac_length + 7;
	adts_header[0] = 0xFF;
	adts_header[1] = 0xF1;
	adts_header[2] = ((ctx->profile) << 6) + (freq_idx << 2) + (chanCfg >> 2);
	adts_header[3] = (((chanCfg & 3) << 6) + (frame_length >> 11));
	adts_header[4] = ((frame_length & 0x7FF) >> 3);
	adts_header[5] = (((frame_length & 7) << 5) + 0x1F);
	adts_header[6] = 0xFC;
}

bool Mp4Reader::InitMp4Reader(std::string filename)
{
	//封装格式上下文。统领全局的结构体，保存了视频文件封装格式的相关信息
	pFormatCtx = avformat_alloc_context();

	if (avformat_open_input(&pFormatCtx, filename.c_str(), nullptr, nullptr) != 0)
	{
		printf("Can not open video:%s", filename.c_str());
		return false;
	}
	if (avformat_find_stream_info(pFormatCtx, nullptr) < 0)
	{
		printf("Can not find video stream info");
		return false;
	}
	nVideoStreamIndex = -1;
	nAudioStreamIndex = -1;

	printf("stream nums:%d\n", pFormatCtx->nb_streams);
	for (int i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			nVideoStreamIndex = i;
			printf("video_stream_idx=%d\n", nVideoStreamIndex);
		}
		else if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			nAudioStreamIndex = i;
			printf("audio_stream_idx=%d\n", nAudioStreamIndex);
		}
	}
	if (nVideoStreamIndex != -1)
	{
		pVideoCodecCtx = avcodec_alloc_context3(nullptr);
		avcodec_parameters_to_context(pVideoCodecCtx, pFormatCtx->streams[nVideoStreamIndex]->codecpar);
		nVideoWidth = pFormatCtx->streams[nVideoStreamIndex]->codecpar->width;
		nVideoHeight = pFormatCtx->streams[nVideoStreamIndex]->codecpar->height;
		nVideoCodecId = pFormatCtx->streams[nVideoStreamIndex]->codecpar->codec_id;
		nVideoFrameRate = pFormatCtx->streams[nVideoStreamIndex]->avg_frame_rate.num / pFormatCtx->streams[nVideoStreamIndex]->avg_frame_rate.den;
		printf("video width=%d, video height=%d, codecId:%d\n", nVideoWidth, nVideoHeight, nVideoCodecId);
		AVCodec* pVideoCodec = avcodec_find_decoder(pVideoCodecCtx->codec_id);
		if (pVideoCodec == nullptr)
		{
			printf("can not find video decoder\n");
			return false;
		}
		if (avcodec_open2(pVideoCodecCtx, pVideoCodec, nullptr) < 0)
		{
			printf("Video decoder can not open\n");
			return false;
		}
		if (nVideoCodecId == AV_CODEC_ID_H264)
		{
			mVideoFile = fopen("mp4.h264", "wb");
		}
		else if (nVideoCodecId == AV_CODEC_ID_H265)
		{
			mVideoFile = fopen("mp4.h265", "wb");
		}
		if (mVideoFile == nullptr) {
			printf("open %s failed!\n", "rtsp.h264");
			return false;
		}
	}
	else
	{
		printf("Can not find video stream");
	}

	const AVBitStreamFilter* bsfilter = av_bsf_get_by_name("h264_mp4toannexb");
	av_bsf_alloc(bsfilter, &bsf_ctx); //AVBSFContext;
	avcodec_parameters_copy(bsf_ctx->par_in, pFormatCtx->streams[nVideoStreamIndex]->codecpar);
	av_bsf_init(bsf_ctx);

	if (nAudioStreamIndex != -1)
	{
		pAudioCodecCtx = avcodec_alloc_context3(nullptr);
		avcodec_parameters_to_context(pAudioCodecCtx, pFormatCtx->streams[nAudioStreamIndex]->codecpar);
		nAudioCodecId = pFormatCtx->streams[nAudioStreamIndex]->codecpar->codec_id;
		nAudiomSampleRate = pFormatCtx->streams[nAudioStreamIndex]->codecpar->sample_rate;
		nAudioChannels = pFormatCtx->streams[nAudioStreamIndex]->codecpar->channels;
		//nAudioSampleFmt = pFormatCtx->streams[nAudioStreamIndex]->codecpar->format;
		AVCodec* pAudioCodec = avcodec_find_decoder(pAudioCodecCtx->codec_id);
		if (pAudioCodec == nullptr)
		{
			printf("can not find video decoder\n");
			return false;
		}
		if (avcodec_open2(pAudioCodecCtx, pAudioCodec, nullptr) < 0)
		{
			printf("Video decoder can not open\n");
			return false;
		}
		if (nAudioCodecId == AV_CODEC_ID_AAC)
		{
			mAudioFile = fopen("mp4.aac", "wb");
		}
	}
    return true;
}

bool Mp4Reader::GetVideoInfo(int& videoWidth, int& videoHeight, AVCodecID& codecId)
{
	if (nVideoStreamIndex != -1)
	{
		videoWidth = nVideoWidth;
		videoHeight = nVideoHeight;
		codecId = nVideoCodecId;
		return true;
	}
	else
	{
		return false;
	}
}

bool Mp4Reader::GetVideoData(char* pOutData, int& iSize)
{
	while (true)
	{
		AVPacket* stream_packet = av_packet_alloc();
		stream_packet->flags = 0;
		if (av_read_frame(pFormatCtx, stream_packet) >= 0)
		{
			if (nVideoStreamIndex != -1 && stream_packet->stream_index == nVideoStreamIndex)
			{
				int input_size = stream_packet->size;
				if (av_bsf_send_packet(bsf_ctx, stream_packet) != 0) // bitstreamfilter内部去维护内存空间
				{
					av_packet_unref(stream_packet);   // 你不用了就把资源释放掉
					continue;       // 继续送
				}
				av_packet_unref(stream_packet);
				while (av_bsf_receive_packet(bsf_ctx, stream_packet) == 0)
				{
					memcpy(pOutData, stream_packet->data, stream_packet->size);
					iSize = stream_packet->size;
					fwrite(stream_packet->data, 1, stream_packet->size, mVideoFile);
					av_packet_unref(stream_packet);
					av_packet_free(&stream_packet);
					return true;
				}
				return true;
			}
		}
		else
		{
			av_usleep(1000);
		}
	}
	return true;
}

bool Mp4Reader::GetAudioInfo(int& sampleRate, int& sampleChannels)
{
	if (nAudioStreamIndex != -1)
	{
		sampleRate = nAudiomSampleRate;
		sampleChannels = nAudioChannels;
		return true;
	}
	else
	{
		return false;
	}
}

bool Mp4Reader::GetAudioData(char* pOutData, int& iSize)
{
	while (true)
	{
		AVPacket* stream_packet = av_packet_alloc();
		stream_packet->flags = 0;
		if (av_read_frame(pFormatCtx, stream_packet) >= 0)
		{
			if (nAudioStreamIndex != -1 && stream_packet->stream_index == nAudioStreamIndex)
			{
				uint8_t aac_header[7];
				get_adts_header(pAudioCodecCtx, aac_header, stream_packet->size);
				size_t len = 0;
				len = fwrite(aac_header, 1, 7, mAudioFile);
				if (len != 7) {
					fprintf(stderr, "fwrite aac_header failed\n");
					return false;
				}
				fwrite((char*)stream_packet->data, 1, stream_packet->size, mAudioFile);
				memcpy(pOutData, stream_packet->data, stream_packet->size);
				iSize = stream_packet->size;
				av_packet_free(&stream_packet);
				return true;
			}
		}
		else
		{
			av_usleep(1000 * 10);
		}
	}
	return false;
}



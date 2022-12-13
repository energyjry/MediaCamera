#include "DecodeAac.h"

bool DecodeAac::InitAacDecoder(int sampleRate, int sampleChannels)
{
	nSampleRate = sampleRate;
	nSampleChannels = sampleChannels;
	pAacCodec = avcodec_find_decoder(AV_CODEC_ID_AAC);
	if (pAacCodec == NULL)
	{
		printf("avcodec_find_decoder failed \n");
		return false;
	}

	pAacCodecCtx = avcodec_alloc_context3(pAacCodec);
	if (pAacCodecCtx == NULL)
	{
		printf("avcodec_alloc_context3 failed \n");
		return false;
	}
	pAacCodecCtx->codec_id = AV_CODEC_ID_AAC;
	pAacCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
	pAacCodecCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;//AV_SAMPLE_FMT_FLTP
	pAacCodecCtx->sample_rate = nSampleRate;
	pAacCodecCtx->frame_size = 1024;
	
	//pAacCodecCtx->channels = av_get_channel_layout_nb_channels(pAacCodecCtx->channel_layout);
	pAacCodecCtx->channels = sampleChannels;
	pAacCodecCtx->channel_layout = av_get_default_channel_layout(pAacCodecCtx->channels);
	
	pAacCodecCtx->bit_rate = 128000;


	if (avcodec_open2(pAacCodecCtx, pAacCodec, NULL) < 0) {
		printf("avcodec_open2 failed!\n");
		return false;
	}

	pAacPacket = av_packet_alloc();
	if (pAacPacket == NULL)
	{
		printf("av_packet_alloc failed!\n");
		return false;
	}
	av_init_packet(pAacPacket);

	pPcmFrame = av_frame_alloc();
	if (pPcmFrame == NULL)
	{
		printf("av_frame_alloc failed!\n");
		return false;
	}
	pPcmFrame->nb_samples = pAacCodecCtx->frame_size;
	pPcmFrame->format = AV_SAMPLE_FMT_S16;
	pPcmFrame->channels = pAacCodecCtx->channels;

	pSwrCtx = swr_alloc();
	if (pSwrCtx == NULL) {
		printf("swr_alloc failed!\n");
		return false;
	}
	swr_alloc_set_opts(
		pSwrCtx,
		pAacCodecCtx->channel_layout,
		AV_SAMPLE_FMT_S16,
		pAacCodecCtx->sample_rate,
		pAacCodecCtx->channel_layout,
		pAacCodecCtx->sample_fmt,
		pAacCodecCtx->sample_rate,
		0, nullptr
	);
	int ret = swr_init(pSwrCtx);
	if (ret < 0)
	{
		printf("swr_init failed!\n");
		return false;
	}

	

	nPcmSize = av_samples_get_buffer_size(NULL, pPcmFrame->channels, pPcmFrame->nb_samples, AV_SAMPLE_FMT_S16, 0);
	pPcmBuffer = (uint8_t**)av_calloc(1, sizeof(*pPcmBuffer));
	
	//uint8_t** data = (uint8_t**)av_calloc(1, sizeof(*data));
	int alloc_size = av_samples_alloc(pPcmBuffer,NULL, pPcmFrame->channels, pPcmFrame->nb_samples, AV_SAMPLE_FMT_S16,0);

	mPcmFile = fopen("rtsp.pcm", "wb");
	if (mPcmFile == nullptr) {
		printf("open %s failed!\n", "rtsp.pcm");
		return false;
	}

	pts = 0;
	return true;
}

bool DecodeAac::GetPcmData(char* aacData, int aacSize, char*& pOutData, int& iSize)
{
	pAacPacket->size = aacSize;
	pAacPacket->data = (uint8_t*)av_malloc(pAacPacket->size);
	memcpy(pAacPacket->data, aacData, aacSize);
	int ret = av_packet_from_data(pAacPacket, pAacPacket->data, pAacPacket->size);
	pAacPacket->pts = pts++;
	if (ret < 0)
	{
		printf("av_packet_from_data error!\n");
		av_free(pAacPacket->data);
		return false;
	}
	ret = avcodec_send_packet(pAacCodecCtx, pAacPacket);
	if (ret < 0)
	{
		printf("avcodec_send_packet error!\n");
		av_free(pAacPacket->data);
		return false;
	}
	while (true)
	{
		ret = avcodec_receive_frame(pAacCodecCtx, pPcmFrame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) 
		{
			break;
		}
		if (ret < 0)
		{
			printf("avcodec_receive_frame error!\n");
			return false;
		}
		if (pPcmFrame->format == AV_SAMPLE_FMT_FLTP)
		{
			swr_convert(pSwrCtx, pPcmBuffer, nPcmSize, (const uint8_t**)pPcmFrame->data, pPcmFrame->nb_samples);
			fwrite(pPcmBuffer[0], 1, nPcmSize, mPcmFile);
			pOutData = (char*)pPcmBuffer[0];
			iSize = nPcmSize;
		}
	}
	av_packet_unref(pAacPacket);
	return true;
}

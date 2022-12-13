#include "EncodeAac.h"

bool EncodeAac::InitAacEncoder(int sampleRate, int sampleChannels)
{
    nSampleRate = sampleRate;
    nSampleChannels = sampleChannels;

    pAacCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (pAacCodec == NULL)
    {
        printf("can not find encoder\n");
        return false;
    }
    pAacCodecCtx = avcodec_alloc_context3(pAacCodec);
    if (pAacCodecCtx == NULL)
    {
        printf("could not allocate AVCodecContext\n");
        return false;
    }

    pAacCodecCtx->codec_id = AV_CODEC_ID_AAC;
    pAacCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    pAacCodecCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;
    pAacCodecCtx->sample_rate = nSampleRate;
    pAacCodecCtx->channels = nSampleChannels;
    pAacCodecCtx->channel_layout = av_get_default_channel_layout(pAacCodecCtx->channels);
   
    pAacCodecCtx->bit_rate = 128000;

    if (avcodec_open2(pAacCodecCtx, pAacCodec, NULL) < 0)
    {
        printf("can not open encoder\n");
        return false;
    }
    
    pPcmFrame = av_frame_alloc();
    if (!pPcmFrame)
    {
        printf("can not alloc frame\n");
        return -1;
    }
    pPcmFrame->nb_samples = pAacCodecCtx->frame_size;
    pPcmFrame->format = pAacCodecCtx->sample_fmt;
    pPcmFrame->channels = pAacCodecCtx->channels;

    pSwrCtx = swr_alloc();
    swr_alloc_set_opts(pSwrCtx,
        pAacCodecCtx->channel_layout,
        AV_SAMPLE_FMT_FLTP,
        pAacCodecCtx->sample_rate,
        pAacCodecCtx->channel_layout,
        pAacCodecCtx->sample_fmt,
        pAacCodecCtx->sample_rate, 
        0, NULL);


    swr_init(pSwrCtx);

    return true;
}

bool EncodeAac::GetAacData(char* pInData, int iSize, char*& pOutData, int& oSize)
{
    return true;
}

#include "UsbReader.h"
#include <Windows.h>

std::string UsbReader::AnsiToUTF8(const char* _ansi, int _ansi_len)
{
    std::string str_utf8("");
    wchar_t* pUnicode = NULL;
    BYTE* pUtfData = NULL;
    do
    {
        int unicodeNeed = MultiByteToWideChar(CP_ACP, 0, _ansi, _ansi_len, NULL, 0);
        pUnicode = new wchar_t[unicodeNeed + 1];
        memset(pUnicode, 0, (unicodeNeed + 1) * sizeof(wchar_t));
        int unicodeDone = MultiByteToWideChar(CP_ACP, 0, _ansi, _ansi_len, (LPWSTR)pUnicode, unicodeNeed);

        if (unicodeDone != unicodeNeed)
        {
            break;
        }
        int utfNeed = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)pUnicode, unicodeDone, (char*)pUtfData, 0, NULL, NULL);
        pUtfData = new BYTE[utfNeed + 1];
        memset(pUtfData, 0, utfNeed + 1);
        int utfDone = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)pUnicode, unicodeDone, (char*)pUtfData, utfNeed, NULL, NULL);

        if (utfNeed != utfDone)
        {
            break;
        }
        str_utf8.assign((char*)pUtfData);
    } while (false);

    if (pUnicode)
    {
        delete[] pUnicode;
    }
    if (pUtfData)
    {
        delete[] pUtfData;
    }

    return str_utf8;
}

bool UsbReader::InitUsbVideo(std::string videoDevice)
{
    int ret = 0;
    //1.初始化视频采集
    avdevice_register_all();
    pVideoFormatCtx = avformat_alloc_context();
    AVInputFormat* ifmt = av_find_input_format("dshow");
    if (ifmt == nullptr)
    {
        printf("输入格式错误.\n");
        return false;
    }
    AVDictionary* options = nullptr;
    av_dict_set(&options, "buffer_size", "10240000", 0);
    av_dict_set(&options, "max_delay", "500000", 0);
    av_dict_set(&options, "stimeout", "3000000", 0);
    std::string deviceUrl = "video=" + videoDevice;
    std::string device_url_utf8 = AnsiToUTF8(deviceUrl.c_str(), deviceUrl.length());
    if (avformat_open_input(&pVideoFormatCtx, device_url_utf8.c_str(), ifmt, &options) != 0) //
    {
        printf("无法打开输入流.\n");
        return false;
    }
    if (avformat_find_stream_info(pVideoFormatCtx, NULL) < 0) {
        printf("avformat_find_stream_info failed!\n");
        return false;
    }
    nVideoStreamIndex = av_find_best_stream(pVideoFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (nVideoStreamIndex < 0) {
        printf("av_find_best_stream failed! (audioIdx %d)\n", nVideoStreamIndex);
        return false;
    }

    AVCodecParameters* aCodecParam = pVideoFormatCtx->streams[nVideoStreamIndex]->codecpar;
    if (NULL == aCodecParam) {
        printf("%s failed (aCodecPar is null)\n", __func__);
        return false;
    }
    nVideoWidth = aCodecParam->width;
    nVideoHeight = aCodecParam->height;
    nVideoCodecId = aCodecParam->codec_id;
    nVideoFrameRate = int(pVideoFormatCtx->streams[nVideoStreamIndex]->avg_frame_rate.num / pVideoFormatCtx->streams[nVideoStreamIndex]->avg_frame_rate.den);
    mVideoFile = fopen("usb.mjpeg", "wb");
    if (mVideoFile == nullptr) {
        printf("open %s failed!\n", "usb.mjpeg");
        return false;
    }
    return true;
}

bool UsbReader::GetVideoInfo(int& videoWidth, int& videoHeight, AVCodecID& codecId)
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

bool UsbReader::GetVideoData(char* pOutData, int& iSize)
{
    while (true)
    {
        AVPacket* stream_packet = av_packet_alloc();
        stream_packet->flags = 0;
        if (av_read_frame(pVideoFormatCtx, stream_packet) >= 0)
        {
            if (nVideoStreamIndex != -1 && stream_packet->stream_index == nVideoStreamIndex)
            {
                fwrite((char*)stream_packet->data, 1, stream_packet->size, mVideoFile);
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
    return true;
}

bool UsbReader::InitUsbAudio(std::string audioDevice)
{
    avdevice_register_all();
    pAudioFormatCtx = avformat_alloc_context();
    AVInputFormat* ifmt = av_find_input_format("dshow"); //windows下dshow
    //AVInputFormat *ifmt=av_find_input_format("video4linux2");  //linux
    if (ifmt == NULL)
    {
        printf("输入格式错误.\n");
        return false;
    }
    std::string device_url = "audio=" + audioDevice;
    //std::string device_url = "audio=麦克风 (USB Microphone)";
    std::string device_url_utf8 = AnsiToUTF8(device_url.c_str(), device_url.length());

    if (avformat_open_input(&pAudioFormatCtx, device_url_utf8.c_str(), ifmt, nullptr) != 0) //
    {
        printf("无法打开输入流.\n");
        return false;
    }
    if (avformat_find_stream_info(pAudioFormatCtx, NULL) < 0) {
        printf("avformat_find_stream_info failed!\n");
        return false;
    }
    nAudioStreamIndex = av_find_best_stream(pAudioFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (nAudioStreamIndex < 0) {
        printf("av_find_best_stream failed! (audioIdx %d)\n", nAudioStreamIndex);
        return false;
    }
    AVStream* aStream = pAudioFormatCtx->streams[nAudioStreamIndex];
    AVCodecParameters* aCodecPar = aStream->codecpar;
    if (NULL == aCodecPar) {
        printf("%s failed (aCodecPar is null)\n", __func__);
        return false;
    }
    AVCodecID mAudioCodec = aCodecPar->codec_id;
    if ((mAudioCodec >= AV_CODEC_ID_FIRST_AUDIO) && (mAudioCodec <= AV_CODEC_ID_PCM_F24LE)) {
        printf("audio codec is PCM\n");
    }
    else if (AV_CODEC_ID_MP3 == mAudioCodec) {
        printf("audio codec is MP3\n");
    }
    else if (AV_CODEC_ID_AAC == mAudioCodec) {
        printf("audio codec is AAC\n");
    }
    else {
        printf("audio codec is 0x%x\n", mAudioCodec);
    }
    printf("audio sample_rate %d\n", aCodecPar->sample_rate);
    nAudioCodecId = pAudioFormatCtx->streams[nAudioStreamIndex]->codecpar->codec_id;
    nAudiomSampleRate = pAudioFormatCtx->streams[nAudioStreamIndex]->codecpar->sample_rate;
    nAudioChannels = pAudioFormatCtx->streams[nAudioStreamIndex]->codecpar->channels;
   
    mAudioFile = fopen("usb.pcm", "wb");
    if (mAudioFile == nullptr) {
        printf("open %s failed!\n", "usb.pcm");
        return false;
    }

    return true;
}

bool UsbReader::GetAudioInfo(int& sampleRate, int& sampleChannels)
{
    if (nAudioStreamIndex >= 0)
    {
        sampleRate = nAudiomSampleRate;
        sampleChannels = nAudioChannels;
        return true;
    }
    return false;
}

bool UsbReader::GetAudioData(char* pOutData, int& iSize)
{
    while (true)
    {
        AVPacket* stream_packet = av_packet_alloc();
        stream_packet->flags = 0;
        if (av_read_frame(pAudioFormatCtx, stream_packet) >= 0)
        {
            //printf("read pcm, size=%d;", mPcmPacket->size);
            memcpy(pOutData, stream_packet->data, stream_packet->size);
            iSize = stream_packet->size;
            fwrite((char*)stream_packet->data, 1, stream_packet->size, mAudioFile);
            return true;
        }
        else
        {
            av_usleep(1000); // 10ms
        }
    }
    return true;
}

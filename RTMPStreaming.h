#pragma once  

#include <string>
#include <stdio.h>

#include "rtmp.h"  
#include "rtmp_sys.h"  
#include "amf.h"
#include "log.h"


#define FILEBUFSIZE (1024 * 1024 * 20)       //  10M  
  
/*
enum {
  NAL_SLICE = 1, NAL_DPA = 2, NAL_DPB = 3, NAL_DPC = 4,
  NAL_IDR_SLICE = 5, NAL_SEI = 6, NAL_SPS = 7, NAL_PPS = 8,
  NAL_AUD = 9, NAL_END_SEQUENCE = 10, NAL_END_STREAM = 11, NAL_FILLER_DATA = 12,
  NAL_SPS_EXT = 13, NAL_AUXILIARY_SLICE = 19, NAL_FF_IGNORE = 0xff0f001
}
*/

// NALU單元  
typedef struct _NaluUnit  
{  
    int type;  
    int size;  
    unsigned char *data;  

}NaluUnit;  
  
typedef struct _RTMPMetadata  
{  
    // video, must be h264 type  
    unsigned int    nWidth;  
    unsigned int    nHeight;  
    unsigned int    nFrameRate;     // fps  
    unsigned int    nVideoDataRate; // bps  
    unsigned int    nSpsLen;  
    unsigned char   Sps[1024];  
    unsigned int    nPpsLen;  
    unsigned char   Pps[68000];  
  
    // audio, must be aac type  
    bool            bHasAudio;  
    unsigned int    nAudioSampleRate;  
    unsigned int    nAudioSampleSize;  
    unsigned int    nAudioChannels;  
    char            pAudioSpecCfg;  
    unsigned int    nAudioSpecCfgLen;  
  
} RTMPMetadata,*LPRTMPMetadata;  
  
  
class RTMPStreaming  
{  
public:  
    RTMPStreaming(void);  
    ~RTMPStreaming(void);  
public:  
    // 連接到RTMP Server  
    bool Connect(const char* url);  
 
    void Close();  
    // 發送MetaData  
    bool SendMetadata(LPRTMPMetadata lpMetaData);  
 
    bool SendH264Packet(unsigned char *data,unsigned int size,bool bIsKeyFrame,unsigned int nTimeStamp);  
 
    bool SendH264File(const char *pFileName);  

	bool h264_decode_sps(BYTE * buf,unsigned int nLen,int &width,int &height);

private:  
  
    bool ReadOneNalu(NaluUnit &nalu, unsigned char* buf, int *bufSize);
 
    int SendPacket(unsigned int nPacketType,unsigned char *data,unsigned int size,unsigned int nTimestamp);  

 
private:  
    RTMP* m_pRtmp;  
    unsigned char* m_pFileBuf;  
    unsigned int  m_nFileBufSize;  
    unsigned int  m_nCurPos;  
};  
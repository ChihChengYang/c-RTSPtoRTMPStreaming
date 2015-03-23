#pragma once

#include "parameters.h"
 
    #include "GlobalApp.h"
    #include "RtspClient.h"
 
    #include "AVS_AvDecoder.h"
    #include "FrameBuffer.h"
    #include "CommonDefine.h"

    typedef struct _VideoContext{
        void*	pRtspServerContext;
        void*	pLprContext;
    }VideoContext;
 
 
class RTSPStreaming
{
public:
 
	RTSPStreaming(void);
	~RTSPStreaming(void);
 
	void startStreaming();
	
	void RTSPConnect( const char* ip , const char* user , const char* pws, int ch);

    RtspClient LprRtspClient;
    VideoContext	videocontext;

    static void LiveVideoDecodedData(void *pContext, int nChannel, unsigned char *pRGBData[3],
                unsigned char *pYUVData, unsigned int nWidth, unsigned int nHeight,
                unsigned int nPitch, unsigned int	nFrameType, time_t tFrameTime, unsigned int nFrametimeMSec);

    AVDHANDLE   m_hDecoder[MAX_LIVEVIEWER_NUM];

	void streamingData(unsigned char *data, unsigned int size);

//---------------------------- 
	typedef void (*streamDataCBFun)( unsigned char *data, unsigned int size );
	streamDataCBFun pStreamDataCBFunCBFun;
	void SetStreamDataCallBack( streamDataCBFun fun ) { pStreamDataCBFunCBFun = fun; } 
//----------------------------

};

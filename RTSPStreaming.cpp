 
#include "RTSPStreaming.h"
 
#include <iostream>
using namespace std;

RTSPStreaming::RTSPStreaming(void)
{
 
}

RTSPStreaming::~RTSPStreaming(void)
{
}

void RTSPStreaming::streamingData(unsigned char *data, unsigned int size)
{
	(*pStreamDataCBFunCBFun)( data , size );//callback function

}

 
void RecvVideoStreamData(void	*pContext, MEDIAINFO	*pMediaInfo)
{
    VSPARAM				*pVsParam		=	(VSPARAM*)pContext;
    VideoContext		*pvideocontext	=	(VideoContext*)pVsParam->pContext;
    //RtspServer			*pRtspserver	=	(RtspServer*)pvideocontext->pRtspServerContext;
    RTSPStreaming				*pLpr			=	(RTSPStreaming*)pvideocontext->pLprContext;

    int nCh;
    if (ISVIDEOTYPE(pMediaInfo->nMediaType))
    {
        nCh = pVsParam->nChannel;
        //if(pLpr->m_bLprServiceStart[nCh] ==  true)
        {
 
#ifdef DECODER
            if (pLpr->m_hDecoder[nCh] == 0)
            {
                pLpr->m_hDecoder[nCh] = AVS_OpenAVDecoder(nCh, pMediaInfo->nMediaType);

                AVS_DecodeRegistCB(pLpr->m_hDecoder[nCh], &RTSPStreaming::LiveVideoDecodedData);

                printf( "AVS_DecodeRegistCB\n" );
            }
            AVS_DecodeVideo(pLpr->m_hDecoder[nCh], pMediaInfo->pData, pMediaInfo->nDataSize, pLpr, 0, 0);
            //pRtspserver->PutVideoStream(nCh , (char*)pMediaInfo->pData , pMediaInfo->nDataSize, pMediaInfo->nMediaType);
#endif

//======================================================
 		pLpr->streamingData( pMediaInfo->pData, pMediaInfo->nDataSize);
//======================================================
		}

    }
 
}
 
void RTSPStreaming::LiveVideoDecodedData(void *pContext, \
                                  int nChannel, \
                                  unsigned char *pRGBData[3], \
                                  unsigned char *pYUVData, \
                                  unsigned int nWidth, \
                                  unsigned int nHeight, \
                                  unsigned int nPitch, \
                                  unsigned int	nFrameType, \
                                  time_t tFrameTime, \
                                  unsigned int nFrametimeMSec)
{
 
#ifdef MAXCPCSDK
 

      std::vector<pPEOPLE_COUNT_RECT>  vectDetectObject1 =
              pcDetectPeople(track_handler1, pRGBData[0], nWidth-8, nHeight-8, &channel_in, &channel_out);

      // loop to get result (channel 1)
      for (int i=0; i<vectDetectObject1.size(); i++) {
 
          pPEOPLE_COUNT_RECT pDetect_Object = vectDetectObject1.at(i);
 
      }

 
#endif
 
#if 0
    int	s32ReadSize = nWidth*nHeight*3/2;

    static FILE	*outFile;
    static int nIdx = 0;
    if (nIdx == 0)
    {
        char	filename[256];
     nIdx++;
        outFile  = fopen("G:\\test.yuv" , "wb");
    }
    if (outFile )
    {

        fwrite(pYUVData, 1, s32ReadSize,outFile);
        fflush(outFile);
        //fclose(outFile[RtpParam->nChannel]);
        //outFile[RtpParam->nChannel] = 0;
    }
#endif


}

void RTSPStreaming::startStreaming()
{
	    
	memset(m_hDecoder,0,MAX_LIVEVIEWER_NUM * sizeof(AVDHANDLE));
	 

    videocontext.pRtspServerContext	= NULL;
    videocontext.pLprContext		= this;
    LprRtspClient.RegRecvVideoStreamCB(RecvVideoStreamData,&videocontext);

    theApp.m_LPRChSet[0].nEnable = 1;
    theApp.m_LPRChSet[0].nPort = 554;


    sprintf(theApp.m_LPRChSet[0].szUsername,"%s","Admin");
    sprintf(theApp.m_LPRChSet[0].szPwd,"%s","1234");
    sprintf(theApp.m_LPRChSet[0].szUrl,"%s","10.62.8.179/h264");
 
    theApp.m_DLPRServerSdk.SetIpCam(&theApp.m_LPRChSet[0],0);

}


void RTSPStreaming::RTSPConnect( const char* ip , const char* user , const char* pws, int ch)
{
	    
	memset(m_hDecoder,0,MAX_LIVEVIEWER_NUM * sizeof(AVDHANDLE));
	 

    videocontext.pRtspServerContext	= NULL;
    videocontext.pLprContext		= this;
    LprRtspClient.RegRecvVideoStreamCB(RecvVideoStreamData,&videocontext);

    theApp.m_LPRChSet[ch].nEnable = 1;
    theApp.m_LPRChSet[ch].nPort = 554;


    sprintf(theApp.m_LPRChSet[ch].szUsername,"%s",user);
    sprintf(theApp.m_LPRChSet[ch].szPwd,"%s",pws);
    sprintf(theApp.m_LPRChSet[ch].szUrl,"%s",ip);
 

    theApp.m_DLPRServerSdk.SetIpCam(&theApp.m_LPRChSet[ch],ch);

}
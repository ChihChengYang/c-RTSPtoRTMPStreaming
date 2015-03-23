#pragma once  

#include <string>
#include <stdio.h>
 


#include "RTMPStreaming.h"  
#include "RTSPStreaming.h"

#define  SHMBASENAME "Global\MappingWidget_"

#define  WINDOWBASENAME "Widget_"

#define  MUTEXBASENAME "Mutex_"

typedef enum{ 
	ISALIVE_OPERATION	=		0x00000001,
	ALIVE_OPERATION		=		0x00000002,
	NOTALIVE_OPERATION	=		0x00000003
  
}RRSHMINFO_ENUM;

typedef struct _sRRShmInfo{
	RRSHMINFO_ENUM status;
}sRRShmInfo;

typedef struct _sRRShmParams{
	//TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
	TCHAR szName[110];
	HANDLE hMapFile;
	TCHAR mutexName[110];
	HANDLE hMutex;
  	int notAliveCount;
	sRRShmInfo* pRRInfoBuf;
}sRRShmParams;



	void fSetShmInfoRR(sRRShmParams *rrShmParams);

class Rtsp2Rtmp  
{  
public:  
    Rtsp2Rtmp(void);  
    ~Rtsp2Rtmp(void);  

//
	// 連接到RTMP Server  
    bool RTMPConnect(const char* url);  
    // 斷開連接  
    void RTMPClose();  
 
	void RTSPConnect( const char* ip , const char* user , const char* pws, int ch);
//
	static void streamingData(unsigned char *data, unsigned int size);

	int send264DataToRTMP(unsigned char *data, unsigned int size);

	int parserSPSPPS(unsigned char *data, unsigned int size);

	int find264NextGapPosition(int &type, int &endPos, unsigned char *data, unsigned int size);
	
	static void *sellf;  //指向自己的pointer
	static Rtsp2Rtmp* This;


	int openShmClient( sRRShmParams *rrShmParams, int id );
	
	void setShmCloseRR(sRRShmParams *rrShmParams);

private:  
 	RTSPStreaming rts;
	RTMPStreaming rtm;
	RTMPMetadata metaData;  

	bool isGetSPSPPS;
	bool isRTMPConnect;

	unsigned int start_time;
};  
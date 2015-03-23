#include "Rtsp2Rtmp.h"
 
#include <iostream>
using namespace std;

#include <windows.h>
#include <tchar.h>
#include <process.h>

Rtsp2Rtmp *Rtsp2Rtmp::This = 0;

Rtsp2Rtmp::Rtsp2Rtmp(void)
{
	Rtsp2Rtmp::This = this;
 

 	rts.SetStreamDataCallBack(&Rtsp2Rtmp::streamingData);

		   
	memset(&metaData,0,sizeof(RTMPMetadata));  

	isRTMPConnect = isGetSPSPPS = false;

	start_time = 0;
}

Rtsp2Rtmp::~Rtsp2Rtmp(void)
{
}
 
void Rtsp2Rtmp::RTSPConnect(  const char* ip , const char* user , const char* pws, int ch)
{
		rts.RTSPConnect( ip ,user , pws ,ch );
}

bool Rtsp2Rtmp::RTMPConnect(const char* url)
{
	isRTMPConnect = rtm.Connect(url);  
	return isRTMPConnect;
}
    
void Rtsp2Rtmp::RTMPClose()
{
	rtm.Close(); 
}

void Rtsp2Rtmp::streamingData(unsigned char *data, unsigned int size)
{
 
	//=====================================================
	static FILE	*outFile;
    static int nIdx = 0;  static int XXX = 0;
    if (nIdx == 0)
    {
        char	filename[256];
		 nIdx++;
        outFile  = fopen("G:\\test.264" , "wb");
    }

	if(This->isRTMPConnect && This->isGetSPSPPS){
		  This->send264DataToRTMP( data, size);
	 
		// fwrite( data, 1, size, outFile);
        // fflush(outFile);
	}

	if(XXX==0){
 	
		if(This->parserSPSPPS(data,size) == 1){
 			This->isGetSPSPPS = true;
			 
			bool bSendMetadata = false;
			if(This->isRTMPConnect){
				 
				  bSendMetadata = This->rtm.SendMetadata(&This->metaData);  
				   This->start_time = GetTickCount(); 
			}
 
			 XXX=1;
		}

	}



    if (outFile && This->isGetSPSPPS && XXX == 1 )
    {  
		XXX = 2;
     //    fwrite( data, 1, size, outFile);
       //  fflush(outFile);
 
    }
	
//======================================================
}

int Rtsp2Rtmp::send264DataToRTMP(unsigned char *data, unsigned int size)
{
	static unsigned int tick = 0; 
	
	 unsigned int timeoffset;


	int ret = 0;
	int h,endPos = 0,type;		
	unsigned char *dataTemp = data;
	unsigned int sizeTemp = size;
	
	endPos = 0;
	h =find264NextGapPosition(type, endPos, dataTemp, sizeTemp);
//----------------------------------------------------	
//remove sps pps
	if(type == 0x07){
		dataTemp =  data+endPos;
		sizeTemp =  size-endPos;

		h =find264NextGapPosition(type, endPos, dataTemp, sizeTemp);
		if( type==0x08 )
		{
			dataTemp =  dataTemp+endPos;
			sizeTemp =  sizeTemp-endPos;
			 
			h =find264NextGapPosition(type, endPos, dataTemp, sizeTemp);

			// 	cout << " SP " ;
		}

	}
//----------------------------------------------------	
	if(h>0){
		
		bool bKeyframe  = (type == 0x05) ? TRUE : FALSE;

		     
		timeoffset = GetTickCount() - start_time; 

		bool bK = rtm.SendH264Packet(&dataTemp[h], (sizeTemp-h), bKeyframe, timeoffset);  
		//bool bK = rtm.SendH264Packet(data, size, bKeyframe,tick);  
		//msleep(30);  
		
		 //tick += 30;
 
	}
	return ret;
}

int Rtsp2Rtmp::parserSPSPPS(unsigned char *data, unsigned int size)
{
	int ret = 0;
	int h,endPos,type;		
	unsigned char *dataTemp = data;
	unsigned int sizeTemp = size;
 
	h = find264NextGapPosition(type, endPos, dataTemp, sizeTemp);
	if( type==0x07 )
	{
 
//-------------------							
			metaData.nSpsLen = (endPos-h);  
			memcpy(metaData.Sps, &dataTemp[h], metaData.nSpsLen );  
 
			int width = 0,height = 0;  
			rtm.h264_decode_sps(metaData.Sps,metaData.nSpsLen,width,height);  
	 
			metaData.nWidth = width;  
			metaData.nHeight = height;  
			metaData.nFrameRate = 30;  
 
//-------------------
//============================================================
		dataTemp =  data+endPos;
		sizeTemp =  size-endPos;

		h =find264NextGapPosition(type, endPos, dataTemp, sizeTemp);
		if( type==0x08 )
		{
 		    metaData.nPpsLen = (endPos-h);   
			memcpy(metaData.Pps, &dataTemp[h], metaData.nPpsLen ); 
 
//-------------------
				ret = 1;
//-------------------
		}
//============================================================

	}

	return ret;
}

int Rtsp2Rtmp::find264NextGapPosition(int &type, int &endPos, unsigned char *data, unsigned int size)
{
	int p;
	endPos = -1;
	for(int i = 0 ; i <size ;++i ){
		p=i;  
		if( (data[p] == 0x00 &&  
            data[p+1] == 0x00 &&  
            data[p+2] == 0x00 &&  
            data[p+3] == 0x01  )){  

			type = data[p+4]&0x1f; 
//-------------------------------		
			if(type == 0x07 || type == 0x08)
			{
				int pos = p+4;
				for(int j = pos ; j <size ;++j ){
			
					if( (data[j] == 0x00 &&  
						data[j+1] == 0x00 &&  
						data[j+2] == 0x00 &&  
						data[j+3] == 0x01  ))
					{
						endPos = j;
					}
					if( (data[j] == 0x00 &&  
						data[j+1] == 0x00 &&  
						data[j+2] == 0x01 ))
					{
						endPos = j;
					}
					if(endPos>0){
						break;
					}
				}
			}
//-------------------------------
			return 4;
 		}
		if( (data[p] == 0x00 &&  
            data[p+1] == 0x00 &&  
            data[p+2] == 0x01 ))
		{  
			type = data[p+3]&0x1f;  
//-------------------------------	
			if(type == 0x07 || type == 0x08)
			{
				int pos = p+3;
				for(int j = pos ; j <size ;++j ){
			
					if( (data[j] == 0x00 &&  
						data[j+1] == 0x00 &&  
						data[j+2] == 0x00 &&  
						data[j+3] == 0x01  ))
					{
						endPos = j;
					}
					if( (data[j] == 0x00 &&  
						data[j+1] == 0x00 &&  
						data[j+2] == 0x01 ))
					{
						endPos = j;
					}
					if(endPos>0){
						break;
					}
				}
			}
//-------------------------------
 			return 3;
 		}
	}
	return -1;

}

unsigned __stdcall setRRShmParamsThreadFun( void* pArguments )
{
	sRRShmParams *pRRShmParams = ((sRRShmParams *)pArguments);
	
	while( true )
	{
 
		  fSetShmInfoRR(pRRShmParams);

		Sleep(30);
	}
		return 0;
}

int Rtsp2Rtmp::openShmClient( sRRShmParams *rrShmParams, int id )
{
	char Outbuffer1[10];	char Outbuffer2[110]; 	char Outbuffer3[110];
	itoa( id , Outbuffer1 , 10);
	lstrcpy(Outbuffer2,TEXT(SHMBASENAME));
	lstrcat(Outbuffer2,Outbuffer1);
	_tcscpy(rrShmParams->szName,Outbuffer2);

	lstrcpy(Outbuffer3,TEXT(MUTEXBASENAME));
	lstrcat(Outbuffer3,Outbuffer1);
	_tcscpy(rrShmParams->mutexName,Outbuffer3);

	rrShmParams->hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		rrShmParams->szName);               // name of mapping object

	if (rrShmParams->hMapFile == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}

	rrShmParams->pRRInfoBuf = (sRRShmInfo*) MapViewOfFile(rrShmParams->hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		sizeof(sRRShmInfo));

	if (rrShmParams->pRRInfoBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());
		CloseHandle(rrShmParams->hMapFile);
		return 1;
	}
	rrShmParams->hMutex  = OpenMutex(MUTEX_ALL_ACCESS, FALSE, rrShmParams->mutexName);

//==============================================
		//-------------------------
		HANDLE hThread;
		unsigned threadID;
		// Create the second thread.
		hThread = (HANDLE)_beginthreadex( NULL, 0, &setRRShmParamsThreadFun, (void*)rrShmParams  , 0, &threadID );
		//WaitForSingleObject( hThread, INFINITE );
		// Destroy the thread object.
		CloseHandle( hThread );
	//-------------------------
//==============================================
	return 0;
}

void Rtsp2Rtmp::setShmCloseRR(sRRShmParams *rrShmParams)
{
	WaitForSingleObject(rrShmParams->hMutex, 100000);
	rrShmParams->pRRInfoBuf->status = NOTALIVE_OPERATION;
	ReleaseMutex(rrShmParams->hMutex);
 
}

void fSetShmInfoRR(sRRShmParams *rrShmParams)
{
	if(rrShmParams->hMapFile)
	{
		WaitForSingleObject(rrShmParams->hMutex, 100000);
 		rrShmParams->pRRInfoBuf->status = ALIVE_OPERATION;
		ReleaseMutex(rrShmParams->hMutex);
 
	}
}
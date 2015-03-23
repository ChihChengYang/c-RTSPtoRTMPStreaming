 

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "Rtsp2Rtmp.h"

static sRRShmParams gRRShmParams;
 
int main(int argc, char *argv[]) 
{

#if 0
	RTMPStreaming rtmpSender;      
     bool bRet = rtmpSender.Connect("rtmp://127.0.0.1/live_0/livestream");    
 
	if(bRet){
		printf("Connect OK..............\n");
  	    rtmpSender.SendH264File("G:\\test.264");   
		printf("SendH264File OK..............\n");

	}
	getchar();
    rtmpSender.Close();  

#endif
#if 0
	Rtsp2Rtmp r2r;
 	r2r.RTMPConnect("rtmp://127.0.0.1:1935/live_0/livestream");   
 	getchar();
//	r2r.RTMPClose();
#endif

#if 1
		
	char szCamIP[256], szCamUsr[256], szCamPwd[256];
	char szId[10],szRMTPAppName[256],szRMTPAddress[256];
	
	int id;
	int camIPCheck, camUsrCheck, camPwdCheck, idCheck, rmtpAppNameCheck;
	rmtpAppNameCheck=idCheck=camIPCheck=camUsrCheck=camPwdCheck=0;

	strcpy(szCamIP,"");
	strcpy(szCamUsr, "");
	strcpy(szCamPwd, "");

	printf("argc %d \n",argc);
	int i = 0,itmp,argctmp;
	argctmp = (argc-1);

	while(i < argctmp)
	{
		i++;	
		char *arg = argv[i];
 
		printf("argv %s \n",argv[i]);

		switch(arg[0]) {
		   case '-':
            // 處理參數，設定執行選項，例如-o、-p、-r等等
			   switch(arg[1]) {
				case 'a': //ip
					itmp = i++;	
					if(itmp<argctmp){
						strcpy(szCamIP, argv[i]);
						camIPCheck=1;
					}
				break;
				case 'u': //user
					itmp = i++;	
					if(itmp<argctmp){
						strcpy(szCamUsr, argv[i]);
						camUsrCheck=1;
					}
				break;
				case 'p': //pwd
					itmp = i++;	
					if(itmp<argctmp){
						strcpy(szCamPwd, argv[i]);
						camPwdCheck=1;
					}
				break;
				case 'i': //id
					itmp = i++;	
					if(itmp<argctmp){
						strcpy(szId, argv[i]);
						id = atoi(szId);
						idCheck=1;
					}
				break;
				case 'r': // RMTP App Name
					itmp = i++;	
					if(itmp<argctmp){
						strcpy(szRMTPAppName, argv[i]);
						rmtpAppNameCheck=1;
					}
				break;
				default:
				break;
			}
			default:
			// 執行對應功能
				break;
		}
			printf("i %d \n",i);
	}
	 

	Rtsp2Rtmp r2r;
	if(camIPCheck){
		printf("szCamIP %s,szCamUsr %s,szCamPwd %s\n",szCamIP,szCamUsr,szCamPwd);
	

		if(id>=0)
		{
			r2r.openShmClient( &gRRShmParams, id );
					 
			printf("openShmClient %d \n",gRRShmParams.pRRInfoBuf->status);

			r2r.RTSPConnect(szCamIP,szCamUsr,szCamPwd, id); 

			strcpy(szRMTPAddress,"rtmp://127.0.0.1:1935/");
			strcat(szRMTPAddress,szRMTPAppName);
			strcat(szRMTPAddress,"/livestream");
			r2r.RTMPConnect(szRMTPAddress);


			printf("Connect OK.....%d......%s...\n",id,szRMTPAddress);
		}
	}

	getchar();

#endif

	return 0;
}

 
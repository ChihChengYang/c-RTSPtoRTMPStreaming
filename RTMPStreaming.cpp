

#include "RTMPStreaming.h"  
 
#include <math.h>  

#ifdef WIN32    
#include <windows.h>  
#endif  
  
#ifdef WIN32  
#pragma comment(lib,"WS2_32.lib")  
#pragma comment(lib,"winmm.lib")  
#endif  
 
enum  
{  
    FLV_CODECID_H264 = 7,  
};
 
//================================================================
 
UINT Ue(BYTE *pBuff, UINT nLen, UINT &nStartBit)  
{  
    //璸衡0bit计  
    UINT nZeroNum = 0;  
    while (nStartBit < nLen * 8)  
    {  
        if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8))) //&:籔%緇  
        {  
            break;  
        }  
        nZeroNum++;  
        nStartBit++;  
    }  
    nStartBit ++;  
  
  
    //璸衡挡狦  
    DWORD dwRet = 0;  
    for (UINT i=0; i<nZeroNum; i++)  
    {  
        dwRet <<= 1;  
        if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8)))  
        {  
            dwRet += 1;  
        }  
        nStartBit++;  
    }  
    return (1 << nZeroNum) - 1 + dwRet;  
}  
  
  
int Se(BYTE *pBuff, UINT nLen, UINT &nStartBit)  
{  
    int UeVal=Ue(pBuff,nLen,nStartBit);  
    double k=UeVal;  
    int nValue=ceil(k/2);//ceilㄧ计ceilㄧ计ノ琌―ぃ倒﹚龟计程俱计ceil(2)=ceil(1.2)=cei(1.5)=2.00  
    if (UeVal % 2==0)  
        nValue=-nValue;  
    return nValue;  
}  
  
  
DWORD u(UINT BitCount,BYTE * buf,UINT &nStartBit)  
{  
    DWORD dwRet = 0;  
    for (UINT i=0; i<BitCount; i++)  
    {  
        dwRet <<= 1;  
        if (buf[nStartBit / 8] & (0x80 >> (nStartBit % 8)))  
        {  
            dwRet += 1;  
        }  
        nStartBit++;  
    }  
    return dwRet;  
}  
  
  
bool RTMPStreaming::h264_decode_sps(BYTE * buf,unsigned int nLen,int &width,int &height)  
{  
    UINT StartBit=0;   
    int forbidden_zero_bit=u(1,buf,StartBit);  
    int nal_ref_idc=u(2,buf,StartBit);  
    int nal_unit_type=u(5,buf,StartBit);  
    if(nal_unit_type==7)  
    {  
        int profile_idc=u(8,buf,StartBit);  
        int constraint_set0_flag=u(1,buf,StartBit);//(buf[1] & 0x80)>>7;  
        int constraint_set1_flag=u(1,buf,StartBit);//(buf[1] & 0x40)>>6;  
        int constraint_set2_flag=u(1,buf,StartBit);//(buf[1] & 0x20)>>5;  
        int constraint_set3_flag=u(1,buf,StartBit);//(buf[1] & 0x10)>>4;  
        int reserved_zero_4bits=u(4,buf,StartBit);  
        int level_idc=u(8,buf,StartBit);  
  
        int seq_parameter_set_id=Ue(buf,nLen,StartBit);  
  
        if( profile_idc == 100 || profile_idc == 110 ||  
            profile_idc == 122 || profile_idc == 144 )  
        {  
            int chroma_format_idc=Ue(buf,nLen,StartBit);  
            if( chroma_format_idc == 3 )  
                int residual_colour_transform_flag=u(1,buf,StartBit);  
            int bit_depth_luma_minus8=Ue(buf,nLen,StartBit);  
            int bit_depth_chroma_minus8=Ue(buf,nLen,StartBit);  
            int qpprime_y_zero_transform_bypass_flag=u(1,buf,StartBit);  
            int seq_scaling_matrix_present_flag=u(1,buf,StartBit);  
  
            int seq_scaling_list_present_flag[8];  
            if( seq_scaling_matrix_present_flag )  
            {  
                for( int i = 0; i < 8; i++ ) {  
                    seq_scaling_list_present_flag[i]=u(1,buf,StartBit);  
                }  
            }  
        }  
        int log2_max_frame_num_minus4=Ue(buf,nLen,StartBit);  
        int pic_order_cnt_type=Ue(buf,nLen,StartBit);  
        if( pic_order_cnt_type == 0 )  
            int log2_max_pic_order_cnt_lsb_minus4=Ue(buf,nLen,StartBit);  
        else if( pic_order_cnt_type == 1 )  
        {  
            int delta_pic_order_always_zero_flag=u(1,buf,StartBit);  
            int offset_for_non_ref_pic=Se(buf,nLen,StartBit);  
            int offset_for_top_to_bottom_field=Se(buf,nLen,StartBit);  
            int num_ref_frames_in_pic_order_cnt_cycle=Ue(buf,nLen,StartBit);  
  
            int *offset_for_ref_frame=new int[num_ref_frames_in_pic_order_cnt_cycle];  
            for( int i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )  
                offset_for_ref_frame[i]=Se(buf,nLen,StartBit);  
            delete [] offset_for_ref_frame;  
        }  
        int num_ref_frames=Ue(buf,nLen,StartBit);  
        int gaps_in_frame_num_value_allowed_flag=u(1,buf,StartBit);  
        int pic_width_in_mbs_minus1=Ue(buf,nLen,StartBit);  
        int pic_height_in_map_units_minus1=Ue(buf,nLen,StartBit);  
  
        width=(pic_width_in_mbs_minus1+1)*16;  
        height=(pic_height_in_map_units_minus1+1)*16;  
  
        return true;  
    }  
    else  
        return false;  
}  

//================================================================


int InitSockets()    
{    
#ifdef WIN32    
    WORD version;    
    WSADATA wsaData;    
    version = MAKEWORD(1, 1);    
    return (WSAStartup(version, &wsaData) == 0);    
#else    
    return TRUE;    
#endif    
}    
  
inline void CleanupSockets()    
{    
#ifdef WIN32    
    WSACleanup();    
#endif    
}    
  
char * put_byte( char *output, uint8_t nVal )    
{    
    output[0] = nVal;    
    return output+1;    
}    
char * put_be16(char *output, uint16_t nVal )    
{    
    output[1] = nVal & 0xff;    
    output[0] = nVal >> 8;    
    return output+2;    
}    
char * put_be24(char *output,uint32_t nVal )    
{    
    output[2] = nVal & 0xff;    
    output[1] = nVal >> 8;    
    output[0] = nVal >> 16;    
    return output+3;    
}    
char * put_be32(char *output, uint32_t nVal )    
{    
    output[3] = nVal & 0xff;    
    output[2] = nVal >> 8;    
    output[1] = nVal >> 16;    
    output[0] = nVal >> 24;    
    return output+4;    
}    
char *  put_be64( char *output, uint64_t nVal )    
{    
    output=put_be32( output, nVal >> 32 );    
    output=put_be32( output, nVal );    
    return output;    
}    
char * put_amf_string( char *c, const char *str )    
{    
    uint16_t len = strlen( str );    
    c=put_be16( c, len );    
    memcpy(c,str,len);    
    return c+len;    
}    
char * put_amf_double( char *c, double d )    
{    
    *c++ = AMF_NUMBER;  /* type: Number */    
    {    
        unsigned char *ci, *co;    
        ci = (unsigned char *)&d;    
        co = (unsigned char *)c;    
        co[0] = ci[7];    
        co[1] = ci[6];    
        co[2] = ci[5];    
        co[3] = ci[4];    
        co[4] = ci[3];    
        co[5] = ci[2];    
        co[6] = ci[1];    
        co[7] = ci[0];    
    }    
    return c+8;    
}  
  
RTMPStreaming::RTMPStreaming(void):  
m_pRtmp(NULL),  
m_nFileBufSize(0),  
m_nCurPos(0)  
{  
    m_pFileBuf = new unsigned char[FILEBUFSIZE];  
    memset(m_pFileBuf,0,FILEBUFSIZE);  
    InitSockets();  
    m_pRtmp = RTMP_Alloc();    
    RTMP_Init(m_pRtmp);    
}  
  
RTMPStreaming::~RTMPStreaming(void)  
{  
    Close();  
    WSACleanup();    
    delete[] m_pFileBuf;  
}  
  
bool RTMPStreaming::Connect(const char* url)  
{  
    if(RTMP_SetupURL(m_pRtmp, (char*)url)<0)  
    {  
        return FALSE;  
    }  
    RTMP_EnableWrite(m_pRtmp);  
    if(RTMP_Connect(m_pRtmp, NULL)<0)  
    {  
        return FALSE;  
    }  
    if(RTMP_ConnectStream(m_pRtmp,0)<0)  
    {  
        return FALSE;  
    }  
    return TRUE;  
}  
  
void RTMPStreaming::Close()  
{  
    if(m_pRtmp)  
    {  
        RTMP_Close(m_pRtmp);  
        RTMP_Free(m_pRtmp);  
        m_pRtmp = NULL;  
    }  
}  
 
int RTMPStreaming::SendPacket(unsigned int nPacketType,unsigned char *data,unsigned int size,unsigned int nTimestamp)  
{  
    if(m_pRtmp == NULL)  
    {  
        return FALSE;  
    }  
  
    RTMPPacket packet;  
    RTMPPacket_Reset(&packet);  
    RTMPPacket_Alloc(&packet,size);  
  
    packet.m_packetType = nPacketType;  
    packet.m_nChannel = 0x04;    
    packet.m_headerType = RTMP_PACKET_SIZE_LARGE;//RTMP_PACKET_SIZE_MEDIUM
    packet.m_nTimeStamp = nTimestamp;    //0
    packet.m_nInfoField2 = m_pRtmp->m_stream_id;  
	//packet->m_hasAbsTimestamp = 0;
    packet.m_nBodySize = size;  
    memcpy(packet.m_body,data,size);  
  
    int nRet = RTMP_SendPacket(m_pRtmp,&packet,0);  
  
    RTMPPacket_Free(&packet);  
  
    return nRet;  
}  
  
bool RTMPStreaming::SendMetadata(LPRTMPMetadata lpMetaData)  
{  
    if(lpMetaData == NULL)  
    {  
        return false;  
    }  
    char body[1024] = {0};;  
      
    char * p = (char *)body;    
    p = put_byte(p, AMF_STRING );  
    p = put_amf_string(p , "@setDataFrame" );  
  
    p = put_byte( p, AMF_STRING );  
    p = put_amf_string( p, "onMetaData" );  
  
    p = put_byte(p, AMF_OBJECT );    
    p = put_amf_string( p, "copyright" );    
    p = put_byte(p, AMF_STRING );    
    p = put_amf_string( p, "firehood" );    
  
    p =put_amf_string( p, "width");  
    p =put_amf_double( p, lpMetaData->nWidth);  
  
    p =put_amf_string( p, "height");  
    p =put_amf_double( p, lpMetaData->nHeight);  
  
    p =put_amf_string( p, "framerate" );  
    p =put_amf_double( p, lpMetaData->nFrameRate);   
  
    p =put_amf_string( p, "videocodecid" );  
    p =put_amf_double( p, FLV_CODECID_H264 );  
  
    p =put_amf_string( p, "" );  
    p =put_byte( p, AMF_OBJECT_END  );  
  
    int index = p-body;  
  
    SendPacket(RTMP_PACKET_TYPE_INFO,(unsigned char*)body,p-body,0);  
  
    int i = 0;  
    body[i++] = 0x17; // 1:keyframe  7:AVC  
    body[i++] = 0x00; // AVC sequence header  
  
    body[i++] = 0x00;  
    body[i++] = 0x00;  
    body[i++] = 0x00; // fill in 0;  
  
    // AVCDecoderConfigurationRecord.  
    body[i++] = 0x01; // configurationVersion  
    body[i++] = lpMetaData->Sps[1]; // AVCProfileIndication  
    body[i++] = lpMetaData->Sps[2]; // profile_compatibility  
    body[i++] = lpMetaData->Sps[3]; // AVCLevelIndication   
    body[i++] = 0xff; // lengthSizeMinusOne    
  
    // sps nums  
    body[i++] = 0xE1; //&0x1f  
    // sps data length  
    body[i++] = (lpMetaData->nSpsLen>>8) & 0xff;  
    body[i++] = lpMetaData->nSpsLen&0xff;  
    // sps data  
    memcpy(&body[i],lpMetaData->Sps,lpMetaData->nSpsLen);  
    i= i+lpMetaData->nSpsLen;  
  
    // pps nums  
    body[i++] = 0x01; //&0x1f  
    // pps data length   
    body[i++] = (lpMetaData->nPpsLen>>8)  & 0xff;  
    body[i++] = lpMetaData->nPpsLen&0xff;  
    // pps data  
    memcpy(&body[i],lpMetaData->Pps,lpMetaData->nPpsLen);  
    i= i+lpMetaData->nPpsLen;  
  
    return SendPacket(RTMP_PACKET_TYPE_VIDEO,(unsigned char*)body,i,0);  
  
}  
  
bool RTMPStreaming::SendH264Packet(unsigned char *data,unsigned int size,bool bIsKeyFrame,unsigned int nTimeStamp)  
{  
    if(data == NULL && size<11)  
    {  
        return false;  
    }  
  
    unsigned char *body = new unsigned char[size+9];  
  
    int i = 0;  
    if(bIsKeyFrame)  
    {  
        body[i++] = 0x17;// 1:Iframe  7:AVC  
    }  
    else  
    {  
        body[i++] = 0x27;// 2:Pframe  7:AVC  
    }  
    body[i++] = 0x01;// AVC NALU  
    body[i++] = 0x00;  
    body[i++] = 0x00;  
    body[i++] = 0x00;  
  
    // NALU size  
    body[i++] = (size>>24) & 0xff;  
    body[i++] = (size>>16) & 0xff;  
    body[i++] = (size>>8) & 0xff;  
    body[i++] = size&0xff;
  
    // NALU data  
    memcpy(&body[i],data,size);  
  
    bool bRet = SendPacket(RTMP_PACKET_TYPE_VIDEO,body,i+size,nTimeStamp);  
  
    delete[] body;  
  
    return bRet;  
}  
   
 
 
bool RTMPStreaming::SendH264File(const char *pFileName)  
{  
    if(pFileName == NULL)  
    {  
        return FALSE;  
    }  
    FILE *fp = fopen(pFileName, "rb");    
    if(!fp)    
    {    
        printf("ERROR:open file %s failed!",pFileName);  
    } 
 //------------------------------------------
    fseek(fp, 0, SEEK_SET);  
    m_nFileBufSize = fread(m_pFileBuf, sizeof(unsigned char), FILEBUFSIZE, fp);  
    if(m_nFileBufSize >= FILEBUFSIZE)  
    {  
        printf("warning : File size is larger than BUFSIZE\n");  
    }  
	fclose(fp); 
 
	
//------------------------------------------

	RTMPMetadata metaData;  
    memset(&metaData,0,sizeof(RTMPMetadata));  

	unsigned int tick = 0; 
	bool bKeyframe;

	NaluUnit naluUnit;  
	while(ReadOneNalu(naluUnit,NULL,NULL))  
    {  
 //------------------------
	   if (naluUnit.type == 0x07) { //NAL_SPS
			metaData.nSpsLen = naluUnit.size;  
			memcpy(metaData.Sps,naluUnit.data,naluUnit.size);  
 
			int width = 0,height = 0;  
			h264_decode_sps(metaData.Sps,metaData.nSpsLen,width,height);  
			metaData.nWidth = width;  
			metaData.nHeight = height;  
			metaData.nFrameRate = 25;  

			printf("SPS %d.(%d,%d)\n",naluUnit.size,metaData.nWidth,metaData.nHeight);

	  } else if (naluUnit.type == 0x08) { //NAL_PPS
 
            metaData.nPpsLen = naluUnit.size;  
			memcpy(metaData.Pps,naluUnit.data,naluUnit.size); 
			printf("PPS %d.\n",naluUnit.size);

			 // 祇癳MetaData  
			 bool bSendMetadata = SendMetadata(&metaData);  
       // send_video_sps_pps();    
 
	  } else {

		bKeyframe  = (naluUnit.type == 0x05) ? TRUE : FALSE;
 
 
        SendH264Packet(naluUnit.data,naluUnit.size,bKeyframe,tick);  
		
		msleep(10);  
		tick += 10;  
 
	 }
//------------------------
		// FindIDRHeader(naluUnit.data, naluUnit.size);
				 printf("ReadOneNaluFromBuf %x,%d\n",naluUnit.type, m_nCurPos);
    }

	printf("fread %ld.\n",m_nFileBufSize);
//------------------------------------------
 

    return TRUE;  
}  
   
 
bool RTMPStreaming::ReadOneNalu(NaluUnit &nalu, unsigned char* buf, int *bufSize)  
{   
    int i = m_nCurPos;  
	int endPOS = 0;
	int pos;

    while(i<m_nFileBufSize)  
	//while(i<500)  
    {  
//----------------------------------------------
        if( (m_pFileBuf[i] == 0x00 &&  
            m_pFileBuf[i+1] == 0x00 &&  
            m_pFileBuf[i+2] == 0x00 &&  
            m_pFileBuf[i+3] == 0x01  )
 		  )  
        {  
 			i+=4;
			//printf("ReadOneNaluFromBuf %x \n",m_pFileBuf[i]);

            pos = i;  
            while (pos<m_nFileBufSize)  
            {  
                if(m_pFileBuf[pos] == 0x00 &&  
                    m_pFileBuf[pos+1] == 0x00 &&  
                    m_pFileBuf[pos+2] == 0x00 &&  
                    m_pFileBuf[pos+3] == 0x01  
                    )  
                {  
					pos+=4;
					endPOS = 4;
                    break;  
                }

			    if(m_pFileBuf[pos] == 0x00 &&  
                    m_pFileBuf[pos+1] == 0x00 &&  
                    m_pFileBuf[pos+2] == 0x01  
                    )  
                {  
					pos+=3; 
					endPOS = 3;
                    break;  
                }  
				pos+=1;
            }  

        }  
//---------------------------------------------- 
		 if( (m_pFileBuf[i] == 0x00 &&  
            m_pFileBuf[i+1] == 0x00 &&  
            m_pFileBuf[i+2] == 0x01  
           )
 		  )  
        {  
 			i+=3;
			//printf("ReadOneNaluFromBuf %x \n",m_pFileBuf[i]);

            pos = i;  
            while (pos<m_nFileBufSize)  
            {  
                if(m_pFileBuf[pos] == 0x00 &&  
                    m_pFileBuf[pos+1] == 0x00 &&  
                    m_pFileBuf[pos+2] == 0x00 &&  
                    m_pFileBuf[pos+3] == 0x01  
                    )  
                {  
					pos+=4;
					endPOS = 4;
                    break;  
                }

			    if(m_pFileBuf[pos] == 0x00 &&  
                    m_pFileBuf[pos+1] == 0x00 &&  
                    m_pFileBuf[pos+2] == 0x01  
                    )  
                {  
					pos+=3; 
					endPOS = 3;
                    break;  
                }  
				pos+=1;
            }  

        }  
//----------------------------------------------
		if( endPOS > 0)
		{
		    if(pos == m_nFileBufSize)  
            {  
                nalu.size = pos-i;    
            }  
            else  
            {  
                nalu.size = (pos-endPOS)-i;  
            }  
            nalu.type = m_pFileBuf[i]&0x1f;  
            nalu.data = &m_pFileBuf[i];  
  
            m_nCurPos = pos-endPOS;  
            return TRUE;  
		}
//----------------------------------------------
		i+=1;
    }  

    return FALSE;  
}  



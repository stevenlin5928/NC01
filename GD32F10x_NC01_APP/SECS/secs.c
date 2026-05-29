#include "string.h"
#include "gd32f10x.h"
#include "secs.h"
#include "w5500_conf.h"
extern CONFIG_MSG  ConfigMsg;

extern unsigned char SECS_S5F1_DATA_RESP[28];

int8_t SECS_S18F9_ENABLE=-1;
int32_t SECS_Code=0;

unsigned char SECS_S1F2_HEADER_RESP[16] = {0x00,0x00,0x00,0x37, 0x00,0x00,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //length byte: 0x00,0x00,0x00,0x37

//202405 
unsigned char SECS_S1F1_DATA_RESP[45] = {0x01,0x02,0x41,0x06,'L','F','R','0','1','M',0x41,13,'2','0','2','6','0','5',0x20,0x20,0x31,0x20,0x30,0x20,0x31};

unsigned char SECS_S18F10_HEADER_RESP[14] = {0x00,0x00,0x00,0x3b,0x00,0x00,0x12,0x0a,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char SECS_S18F10_DATA_RESP[12] = {0x01,0x04,0x41,0x02,0x30,0x31,0x41,0x02,0x4E,0x4F,0x41,0x10};
unsigned char SECS_S18F10_DATA2_RESP[21] =	{0x01,0x04,0x41,0x02,0x4E,0x45,0x41,0x01,0x30,0x41,0x04,0x49,0x44,0x4C,0x45,0x41,0x04,0x49,0x44,0x4C,0x45};

unsigned char SECS_S18F16_HEADER_RESP[14] = {0x00,0x00,0x00,15, 0x00,0x00,0x12,0x10,0x00,0x00,0x00,0x00,0x00,0x00}; //length byte: 0x00,0x00,0x00,0x37
unsigned char SECS_S18F16_DATA_RESP[5] = {
		0x01,0x01,//L[1]
				0x21,0x01,0x00//B[1]
};
//S2F17 Date and Time Data(DTD) H<->E
const uint8_t SECS_S2F17_HEADER_RESP[14] = {0x00,0x00,0x00,0x39,0x00,0x00,0x2,0x11,0x00,0x00,0x00,0x00,0x00,0x23};
const uint8_t SECS_S2F18_HEADER_RESP[14] = {0x00,0x00,0x00,0x39,0x00,0x00,0x2,0x12,0x00,0x00,0x00,0x00,0x00,0x23};
const uint8_t SECS_S2F22_HEADER_RESP[14] = {0x00,0x00,0x00,0x39,0x00,0x00,0x2,0x16,0x00,0x00,0x00,0x00,0x00,0x23};
const uint8_t SECS_S5F1_HEADER_RESP[14] = {0x00,0x00,0x00,38, 0x00,0x00,0x05,0x01,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char SECS_S18F71_HEADER_RESP[14] = {0x00,0x00,0x00,0x39,0x00,0x00,0x12,0x47,0x00,0x00,0x00,0x00,0x00,0x23};
unsigned char SECS_S18F75_HEADER_RESP[14] = {0x00,0x00,0x00,0x39,0x00,0x00,0x12,0x4b,0x00,0x00,0x00,0x00,0x00,0x23};

const uint8_t SECS_S18F81_HEADER_RESP[14]= {0x00,0x00,0x00,0x39,0x00,0x00,0x12,0x51,0x00,0x00,0x00,0x00,0x00,0x23};
unsigned char  SECS_S18F7X_Sn = 1;
extern unsigned char SECS_S18F10_HEADER_RESP[14]; 
extern void send_secs_socket(uint8_t *rdata,uint16_t len);
extern void _Debug2(int type, char *mess, ...);
extern int checkSecsLinkOk(void);
void report_S2F17(uint32_t session,uint8_t *datetime,uint8_t len)
{
	uint8_t hsmsbuf[128]={0};
	uint16_t hsmsLen=0;
	if(checkSecsLinkOk()<0)
	{
		return;
	}
	memcpy(hsmsbuf, SECS_S2F17_HEADER_RESP, 14);
	hsmsbuf[10]=(session>>24)&0xff;
	hsmsbuf[11]=(session>>16)&0xff;
	hsmsbuf[12]=(session>>8)&0xff;
	hsmsbuf[13]=(session>>0)&0xff;
	//
  //L[1]  0x01,0x01
	hsmsbuf[14]=0x01;
	hsmsbuf[15]=0x01;
	
	hsmsbuf[16]=0x41;
	hsmsbuf[17]=14;
	memcpy(&hsmsbuf[18],datetime,len);

	hsmsLen=18+len;

	hsmsbuf[2] = ((hsmsLen-4)>>8)&0xff; // package length - 4bytes header
  hsmsbuf[3] = (hsmsLen-4)&0xff;
	printf("S2F18 Data\r\n");
  for(int  i=0;i<hsmsLen;i++)
	{
		printf(" %02x",hsmsbuf[i]);
	}
	printf("\r\n");
	if(ConfigMsg.debug>0)
		_Debug2(0, "report_S2F22 , packet_len[%d]  \n",hsmsLen);
											
	// send to pc
	send_secs_socket(hsmsbuf,hsmsLen);
	return;
}

void report_S2F18(uint32_t session,uint8_t *datetime,uint8_t len)
{
	uint8_t hsmsbuf[128]={0};
	uint16_t hsmsLen=0;
	if(checkSecsLinkOk()<0)
	{
		return;
	}
	memcpy(hsmsbuf, SECS_S2F18_HEADER_RESP, 14);
	hsmsbuf[10]=(session>>24)&0xff;
	hsmsbuf[11]=(session>>16)&0xff;
	hsmsbuf[12]=(session>>8)&0xff;
	hsmsbuf[13]=(session>>0)&0xff;
	//
  //L[1]  0x01,0x01
	hsmsbuf[14]=0x01;
	hsmsbuf[15]=0x01;
	
	hsmsbuf[16]=0x41;
	hsmsbuf[17]=14;
	memcpy(&hsmsbuf[18],datetime,len);
	hsmsLen=18+len;

	hsmsbuf[2] = ((hsmsLen-4)>>8)&0xff; // package length - 4bytes header
  hsmsbuf[3] = (hsmsLen-4)&0xff;
	printf("S2F18 Data\r\n");
  for(int  i=0;i<hsmsLen;i++)
	{
		printf(" %02x",hsmsbuf[i]);
	}
	printf("\r\n");
	if(ConfigMsg.debug>0)
		_Debug2(0, "report_S2F22 , packet_len[%d]  \n",hsmsLen);
											
		// send to pc
	send_secs_socket(hsmsbuf,hsmsLen);
	return;
}


void report_S5F1(uint8_t alarmId,uint8_t deviceId,uint8_t portId)
{
	if(checkSecsLinkOk()<0)
	{
		return;
	}
	uint8_t hsmsbuf[128]={0};
	uint16_t hsmslen=0;
	memcpy(hsmsbuf, SECS_S5F1_HEADER_RESP, 14); 

	//L[4]  0x01,0x04
	hsmsbuf[14]=0x01;
	hsmsbuf[15]=0x04;
	//A[2]	alarmID
	hsmsbuf[16]=0x41;
	hsmsbuf[17]=0x02;
	hsmsbuf[18]=0x30+((alarmId/10)&10);
	hsmsbuf[19]=0x30+(alarmId%10);
	//device id
	hsmsbuf[20]=0x41;
	hsmsbuf[21]=0x02;
	hsmsbuf[22]=0x30+(alarmId%10);
	//port id
	hsmsbuf[23]=0x41;
	hsmsbuf[24]=0x02;
	hsmsbuf[25]=0x30+(portId%10);
	//Alarm Text
	hsmsbuf[26]=0x41;
	hsmsbuf[27]=0x02;
	memcpy(&hsmsbuf[28],(uint8_t *)"RFID READ FAIL",strlen("RFID READ FAIL"));
	
	hsmslen=28+strlen("RFID READ FAIL");			
	//
	hsmsbuf[2]=((hsmslen-4)>>8)&0xff;		
	hsmsbuf[3]=(hsmslen-4)&0xff;
		_Debug2(0, "S5F1-->Send to EAP, --%d,hemslen:%d CM", __LINE__,hsmslen);
			
	send_secs_socket(hsmsbuf,hsmslen);
	return ;
}

void report_S18F10(uint8_t targetid,uint8_t *lfData,uint8_t lflen,uint32_t session)
{
	uint8_t hsmsbuf[128]={0};
	uint16_t hsmsLen=0;
	uint16_t offset=0;
	memcpy(hsmsbuf, SECS_S18F10_HEADER_RESP, 14);
	hsmsbuf[10]=(session>>24)&0xff;
	hsmsbuf[11]=(session>>16)&0xff;
	hsmsbuf[12]=(session>>8)&0xff;
	hsmsbuf[13]=(session>>0)&0xff;
	//
  //L[4]  0x01,0x04
	hsmsbuf[14]=0x01;
	hsmsbuf[15]=0x04;
	//A[2]
	hsmsbuf[16]=0x41;
	hsmsbuf[17]=0x02;
	hsmsbuf[18]=0x30+(targetid/10)%10;
	hsmsbuf[19]=0x30+(targetid%10);
		//A[2]
	hsmsbuf[20]=0x41;
	hsmsbuf[21]=0x02;
	hsmsbuf[22]='N';
	hsmsbuf[23]='O';
	//A[1] ceid
	hsmsbuf[24]=0x41;
	offset=25;
	if(lflen<0)
		lflen=0;
	if((lflen==0)||(lfData==NULL))
	{
		hsmsbuf[offset]=5;
		offset++;
		hsmsbuf[offset]='E';
		offset++;
		hsmsbuf[offset]='M';
		offset++;
		hsmsbuf[offset]='P';
		offset++;
		hsmsbuf[offset]='T';
		offset++;
		hsmsbuf[offset]='Y';
		offset++;
	}
	else{
		hsmsbuf[offset]=lflen;
		offset++;
		memcpy(&hsmsbuf[offset],lfData,lflen);
		offset+=lflen;		
	}

	//L[4]
	hsmsbuf[offset]=0x01;
	offset++;
	hsmsbuf[offset]=0x04;
	offset++;
	//A[2]
	hsmsbuf[offset]=0x41;
	offset++;
	hsmsbuf[offset]=0x2;
	offset++;
	hsmsbuf[offset]='N';
	offset++;
	hsmsbuf[offset]='E';
	offset++;
	//A[1]
	hsmsbuf[offset]=0x41;
	offset++;
	hsmsbuf[offset]=0x1;
	offset++;
	hsmsbuf[offset]='0';
	offset++;
	//A[4]
	hsmsbuf[offset]=0x41;
	offset++;
	hsmsbuf[offset]=0x4;
	offset++;
	hsmsbuf[offset]='I';
	offset++;
	hsmsbuf[offset]='D';
	offset++;
	hsmsbuf[offset]='L';
	offset++;
	hsmsbuf[offset]='E';
	offset++;
	//A[4]
	hsmsbuf[offset]=0x41;
	offset++;
	hsmsbuf[offset]=0x4;
	offset++;
	hsmsbuf[offset]='I';
	offset++;
	hsmsbuf[offset]='D';
	offset++;
	hsmsbuf[offset]='L';
	offset++;
	hsmsbuf[offset]='E';
	offset++;
	
	hsmsLen=offset;
	
	hsmsbuf[2] = ((hsmsLen-4)>>8)&0xff; // package length - 4bytes header
  hsmsbuf[3] = (hsmsLen-4)&0xff;
	printf("S18F10 Data\r\n");
  for(int  i=0;i<hsmsLen;i++)
	{
		printf(" %02x",hsmsbuf[i]);
	}
	printf("\r\n");
	if(ConfigMsg.debug>0)
		_Debug2(0, "report_S18F10 , packet_len[%d]  \n",hsmsLen);
											
	// send to pc
	send_secs_socket(hsmsbuf,hsmsLen);
	return;
}

void report_S2F22(uint32_t session)
{
	uint8_t hsmsbuf[128]={0};
	uint16_t hsmsLen=0;

	memcpy(hsmsbuf, SECS_S2F22_HEADER_RESP, 14);
	hsmsbuf[10]=(session>>24)&0xff;
	hsmsbuf[11]=(session>>16)&0xff;
	hsmsbuf[12]=(session>>8)&0xff;
	hsmsbuf[13]=(session>>0)&0xff;
	//
  //B[1]  0x01,0x04
	hsmsbuf[14]=0x41;
	hsmsbuf[15]=0x01;
	hsmsbuf[16]=0x00;
	hsmsLen=17;
	hsmsbuf[2] = ((hsmsLen-4)>>8)&0xff; // package length - 4bytes header
  hsmsbuf[3] = (hsmsLen-4)&0xff;
	printf("S2F22 Data\r\n");
  for(int  i=0;i<hsmsLen;i++)
	{
		printf(" %02x",hsmsbuf[i]);
	}
	printf("\r\n");
	if(ConfigMsg.debug>0)
		_Debug2(0, "report_S2F22 , packet_len[%d]  \n",hsmsLen);
											
		// send to pc
		//Write_SOCK_Data_Buffer(CONN_SOCKET_NO, USER_BANK_BUFFER, S18F71_LENGTH); //S18F71's length
		send_secs_socket(hsmsbuf,hsmsLen);
	return;
}



//void report_S18F10(uint8_t targetid,uint8_t *lfData,uint8_t lflen,uint32_t session)
//{
//	uint8_t hsmsbuf[128]={0};
//	uint16_t hsmsLen=0;
//	uint16_t offset=0;
//	memcpy(hsmsbuf, SECS_S18F10_HEADER_RESP, 14);
//	hsmsbuf[10]=(session>>24)&0xff;
//	hsmsbuf[11]=(session>>16)&0xff;
//	hsmsbuf[12]=(session>>8)&0xff;
//	hsmsbuf[13]=(session>>0)&0xff;
//	//
//  //L[4]  0x01,0x04
//	hsmsbuf[14]=0x01;
//	hsmsbuf[15]=0x04;
//	//A[2]
//	hsmsbuf[16]=0x41;
//	hsmsbuf[17]=0x02;
//	hsmsbuf[18]=0x30+(targetid/10)%10;
//	hsmsbuf[19]=0x30+(targetid%10);
//		//A[2]
//	hsmsbuf[20]=0x41;
//	hsmsbuf[21]=0x02;
//	hsmsbuf[22]='N';
//	hsmsbuf[23]='O';
//	//A[1] ceid
//	hsmsbuf[24]=0x41;
//	offset=25;
//	if((lflen==0)||(lfData==NULL))
//	{
//		hsmsbuf[offset]=5;
//		offset++;
//		hsmsbuf[offset]='E';
//		offset++;
//		hsmsbuf[offset]='M';
//		offset++;
//		hsmsbuf[offset]='P';
//		offset++;
//		hsmsbuf[offset]='T';
//		offset++;
//		hsmsbuf[offset]='Y';
//		offset++;
//	}
//	else{
//		hsmsbuf[offset]=lflen;
//		offset++;
//		memcpy(&hsmsbuf[offset],lfData,lflen);
//		offset+=lflen;		
//	}

//	//L[4]
//	hsmsbuf[offset]=0x01;
//	offset++;
//	hsmsbuf[offset]=0x04;
//	offset++;
//	//A[2]
//	hsmsbuf[offset]=0x41;
//	offset++;
//	hsmsbuf[offset]=0x2;
//	offset++;
//	hsmsbuf[offset]='N';
//	offset++;
//	hsmsbuf[offset]='E';
//	offset++;
//	//A[1]
//	hsmsbuf[offset]=0x41;
//	offset++;
//	hsmsbuf[offset]=0x1;
//	offset++;
//	hsmsbuf[offset]='0';
//	offset++;
//	//A[4]
//	hsmsbuf[offset]=0x41;
//	offset++;
//	hsmsbuf[offset]=0x4;
//	offset++;
//	hsmsbuf[offset]='I';
//	offset++;
//	hsmsbuf[offset]='D';
//	offset++;
//	hsmsbuf[offset]='L';
//	offset++;
//	hsmsbuf[offset]='E';
//	offset++;
//	//A[4]
//	hsmsbuf[offset]=0x41;
//	offset++;
//	hsmsbuf[offset]=0x4;
//	offset++;
//	hsmsbuf[offset]='I';
//	offset++;
//	hsmsbuf[offset]='D';
//	offset++;
//	hsmsbuf[offset]='L';
//	offset++;
//	hsmsbuf[offset]='E';
//	offset++;
//	
//	hsmsLen=offset;
//	
//	hsmsbuf[2] = ((hsmsLen-4)>>8)&0xff; // package length - 4bytes header
//  hsmsbuf[3] = (hsmsLen-4)&0xff;
//	printf("S18F10 Data\r\n");
//  for(int  i=0;i<hsmsLen;i++)
//	{
//		printf(" %02x",hsmsbuf[i]);
//	}
//	printf("\r\n");
//	if(ConfigMsg.debug>0)
//		_Debug2(0, "report_S18F10 , packet_len[%d]  \n",hsmsLen);
//											
//		// send to pc
//		//Write_SOCK_Data_Buffer(CONN_SOCKET_NO, USER_BANK_BUFFER, S18F71_LENGTH); //S18F71's length
//		send_secs_socket(hsmsbuf,hsmsLen);
//	return;
//}


uint8_t getSerialNo()
{
	SECS_S18F7X_Sn++;
	SECS_S18F7X_Sn = SECS_S18F7X_Sn%255;
	return SECS_S18F7X_Sn;
}
#if  1
void report_S18F71_LF_4(uint8_t ceid,int ant,uint8_t *buf,int len)
{
	/*
	L[4]
	01 04 
	A[2]
	41 02 30 33 
	A[2]
	41 02 4E 4F 
	A[1]
	41 01 31 
	L[2]
	01 02 
	41 0C 41 75 74 6F 52 65 61 64 44 61 74 61 
	41 10 4A 50 72 6F 6D 6F 73 5F 5F 5F 31 30 00 00 00 00
	
	*/
	uint8_t hsmsbuf[128]={0};
	uint16_t hsmsLen=0;
	uint16_t offset=0;
	
	memcpy(hsmsbuf, SECS_S18F71_HEADER_RESP, 14);
	hsmsbuf[5] = 0x30;
	hsmsbuf[13] = getSerialNo();

  //L[4]  0x01,0x04
	hsmsbuf[14]=0x01;
	hsmsbuf[15]=0x04;
	//A[2]
	hsmsbuf[16]=0x41;
	hsmsbuf[17]=0x02;
	hsmsbuf[18]=0x30+(ant/10)%10;
	hsmsbuf[19]=0x30+(ant%10);
		//A[2]
	hsmsbuf[20]=0x41;
	hsmsbuf[21]=0x02;
	if(len<0)
		len=0;
	if(len==0)
	{
	 hsmsbuf[22]='N';
	 hsmsbuf[23]='O';
	}
	else
	{
		hsmsbuf[22]='T';
		hsmsbuf[23]='E';
	}
	//A[1] ceid
	hsmsbuf[24]=0x41;
	hsmsbuf[25]=0x01;
	hsmsbuf[26]=0x30+(ceid%10);
		//L[2]
	hsmsbuf[27]=0x01;
	hsmsbuf[28]=0x02;
		//A[12]
	hsmsbuf[29]=0x41;
	hsmsbuf[30]=strlen("AutoReadData");
	memcpy(&hsmsbuf[31],"AutoReadData",strlen("AutoReadData"));
	offset=31+strlen("AutoReadData");
	//A[8]
  hsmsbuf[offset]=0x41;
	offset++;
	if(len!=0){
		hsmsbuf[offset]=len;
		offset++;
		memcpy(&hsmsbuf[offset],buf,len);
		offset+=len;
	}
	else
	{
		hsmsbuf[offset]=16;
		offset++;
		memcpy(&hsmsbuf[offset],buf,16);
		offset+=16;
		
	}
	

	hsmsLen=offset;
	
	hsmsbuf[2] = ((hsmsLen-4)>>8)&0xff; // package length - 4bytes header
  hsmsbuf[3] = (hsmsLen-4)&0xff;
	
	
//	printf("S18F71 Data\r\n");
//  for(int  i=0;i<hsmsLen;i++)
//	{
//		printf(" %02x",hsmsbuf[i]);
//	}
//	printf("\r\n");
	if(ConfigMsg.debug>0)
		_Debug2(0, "report_S18F71 , packet_len[%d]  \n",hsmsLen);
											
		// send to pc
		//Write_SOCK_Data_Buffer(CONN_SOCKET_NO, USER_BANK_BUFFER, S18F71_LENGTH); //S18F71's length
	send_secs_socket(hsmsbuf,hsmsLen);
	return;
}

#else
void report_S18F71(uint8_t targetid,uint8_t ceid,uint8_t *IdData,uint8_t idlen,uint8_t *barData,uint8_t barlen)
{
	uint8_t hsmsbuf[128]={0};
	uint16_t hsmsLen=0;
	uint16_t offset=0;
	memcpy(hsmsbuf, SECS_S18F71_HEADER_RESP, 14);
	//
		//
  //L[4]  0x01,0x04
	hsmsbuf[14]=0x01;
	hsmsbuf[15]=0x04;
	//A[2]
	hsmsbuf[16]=0x41;
	hsmsbuf[17]=0x02;
	hsmsbuf[18]=0x30+(targetid/10)%10;
	hsmsbuf[19]=0x30+(targetid%10);
		//A[2]
	hsmsbuf[20]=0x41;
	hsmsbuf[21]=0x02;
	hsmsbuf[22]='N';
	hsmsbuf[23]='O';
	//A[1] ceid
	hsmsbuf[24]=0x41;
	hsmsbuf[25]=0x01;
	hsmsbuf[26]=0x30+(ceid%10);
  //
	hsmsbuf[18]=0x30+(targetid/10)%10;
	hsmsbuf[19]=0x30+(targetid%10);
	//ceid
	hsmsbuf[26]=0x30+(ceid%10);
	//
	//hsmsLen=14+sizeof(SECS_S18F71_DATA_RESP);
	
	hsmsbuf[29]=0x41;
	hsmsbuf[30]=strlen("CassetteIDData");
	memcpy(&hsmsbuf[31],"CassetteIDData",strlen("CassetteIDData"));
	offset=31+strlen("CassetteIDData");
	//
  hsmsbuf[offset]=0x41;
	offset++;
	hsmsbuf[offset]=idlen;
	offset++;
	memcpy(&hsmsbuf[offset],IdData,idlen);
	offset+=idlen;
	//BarCodeData
	hsmsbuf[offset]=0x41;
	offset++;
	hsmsbuf[offset]=strlen("BarCodeData");
	offset++;
	memcpy(&hsmsbuf[offset],"BarCodeData",strlen("BarCodeData"));
	offset+=strlen("BarCodeData");
	
	hsmsbuf[offset]=0x41;
	offset++;
	hsmsbuf[offset]=barlen;
	offset++;
	memcpy(&hsmsbuf[offset],barData,barlen);
	offset+=barlen;
	
	hsmsLen=offset;
	
	hsmsbuf[2] = ((hsmsLen-4)>>8)&0xff; // package length - 4bytes header
  hsmsbuf[3] = (hsmsLen-4)&0xff;
//	printf("S18F71 Data\r\n");
//  for(int  i=0;i<hsmsLen;i++)
//	{
//		printf(" %02x",hsmsbuf[i]);
//	}
//	printf("\r\n");
	if(ConfigMsg.debug>0)
		_Debug2(0, "report_S18F71 , packet_len[%d]  \n",hsmsLen);
											
		// send to pc
		//Write_SOCK_Data_Buffer(CONN_SOCKET_NO, USER_BANK_BUFFER, S18F71_LENGTH); //S18F71's length
		send_secs_socket(hsmsbuf,hsmsLen);
	return;
}
#endif/**/
void report_S18F75(uint8_t ceid,int ant,uint8_t *buf,int len)
{
	/*01 04
	41 02 30 33 
	41 02 4E 4F 
	41 01 31 
	01 02 
	41 0C 41 75 74 6F 52 65 61 64 44 61 74 61 
	41 10 4A 50 72 6F 6D 6F 73 5F 5F 5F 31 30 00 00 00 00
	*/
	uint8_t hsmsbuf[128]={0};
	uint16_t hsmsLen=0;
	uint16_t offset=0;

	memcpy(hsmsbuf, SECS_S18F75_HEADER_RESP, 14);
	hsmsbuf[5] = 0x30;
	hsmsbuf[13] = getSerialNo();
	//L[4]  0x01,0x04
	hsmsbuf[14]=0x01;
	hsmsbuf[15]=0x04;
	//A[2]
	hsmsbuf[16]=0x41;
	hsmsbuf[17]=0x02;
	hsmsbuf[18]=0x30+(ant/10)%10;
	hsmsbuf[19]=0x30+(ant%10);
		//A[2]
	hsmsbuf[20]=0x41;
	hsmsbuf[21]=0x02;
	if(len<0)
		len=0;
	if(len==0)
	{
	 hsmsbuf[22]='N';
	 hsmsbuf[23]='O';
	}
	else
	{
		hsmsbuf[22]='T';
		hsmsbuf[23]='E';
	}
	//A[1] ceid
	hsmsbuf[24]=0x41;
	hsmsbuf[25]=0x01;
	hsmsbuf[26]=0x30+(ceid%10);
	//L[2]
	hsmsbuf[27]=0x01;
	hsmsbuf[28]=0x02;
	//A[12]
	hsmsbuf[29]=0x41;
	hsmsbuf[30]=strlen("AutoReadData");
	memcpy(&hsmsbuf[31],"AutoReadData",strlen("AutoReadData"));
	offset=31+strlen("AutoReadData");
	//A[8]
  hsmsbuf[offset]=0x41;
	offset++;
	
	if(len!=0){
		hsmsbuf[offset]=len;
		offset++;
		memcpy(&hsmsbuf[offset],buf,len);
		offset+=len;
	}
	else
	{
		hsmsbuf[offset]=16;
		offset++;
		memcpy(&hsmsbuf[offset],buf,16);
		offset+=16;
		
	}

	hsmsLen=offset;
	
	hsmsbuf[2] = ((hsmsLen-4)>>8)&0xff; // package length - 4bytes header
  hsmsbuf[3] = (hsmsLen-4)&0xff;
			
//			if(ConfigMsg.debug>0)
//				_Debug2(0, "%s , packet_len[%d] SECS_S18F71_TAG1_LENGTH: %d \n",__func__, S18F71_LENGTH, SECS_S18F71_TAG1_LENGTH);
				
	send_secs_socket(hsmsbuf,hsmsLen);
	return;
}

void report_S18F81(uint8_t targetid,uint8_t ceid,uint8_t *trackInData,uint8_t trackinlen,uint8_t *trackOutData,uint8_t trackoutlen,uint8_t *barData,uint8_t barlen)
{
	uint8_t hsmsbuf[128]={0};
	uint16_t hsmsLen=0;
	uint16_t offset=0;
	memcpy(hsmsbuf, SECS_S18F81_HEADER_RESP, 14);
	//
  //L[4]  0x01,0x04
	hsmsbuf[14]=0x01;
	hsmsbuf[15]=0x04;
	//A[2]
	hsmsbuf[16]=0x41;
	hsmsbuf[17]=0x02;
	hsmsbuf[18]=0x30+(targetid/10)%10;
	hsmsbuf[19]=0x30+(targetid%10);
		//A[2]
	hsmsbuf[20]=0x41;
	hsmsbuf[21]=0x02;
	hsmsbuf[22]='N';
	hsmsbuf[23]='O';
	//A[1] ceid
	hsmsbuf[24]=0x41;
	hsmsbuf[25]=0x01;
	hsmsbuf[26]=0x30+(ceid%10);
	//L[6]
	hsmsbuf[27]=0x01;
	hsmsbuf[28]=0x06;
	
	hsmsbuf[29]=0x41;
	hsmsbuf[30]=strlen("TrackInData");
	memcpy(&hsmsbuf[31],"TrackInData",strlen("TrackInData"));
	offset=31+strlen("TrackInData");
	//A[16]
  hsmsbuf[offset]=0x41;
	offset++;
	hsmsbuf[offset]=trackinlen;
	offset++;
	memcpy(&hsmsbuf[offset],trackInData,trackinlen);
	offset+=trackinlen;
	
	//A[12] TrackOutData
	hsmsbuf[offset]=0x41;
	offset++;
	hsmsbuf[offset]=strlen("TrackOutData");
	offset++;
	memcpy(&hsmsbuf[offset],"TrackOutData",strlen("TrackOutData"));
	offset+=strlen("TrackOutData");
	//A[16]
  hsmsbuf[offset]=0x41;
	offset++;
	hsmsbuf[offset]=trackoutlen;
	offset++;
	memcpy(&hsmsbuf[offset],trackOutData,trackoutlen);
	offset+=trackoutlen;
	
	//A[12] BarCodeData
	hsmsbuf[offset]=0x41;
	offset++;
	hsmsbuf[offset]=strlen("BarCodeData");
	offset++;
	memcpy(&hsmsbuf[offset],"BarCodeData",strlen("BarCodeData"));
	offset+=strlen("BarCodeData");
	//A[]
	hsmsbuf[offset]=0x41;
	offset++;
	hsmsbuf[offset]=barlen;
	offset++;
	memcpy(&hsmsbuf[offset],barData,barlen);
	offset+=barlen;
	
	hsmsLen=offset;
	
	hsmsbuf[2] = ((hsmsLen-4)>>8)&0xff; // package length - 4bytes header
  hsmsbuf[3] = (hsmsLen-4)&0xff;
	printf("S18F81 Data\r\n");
  for(int  i=0;i<hsmsLen;i++)
	{
		printf(" %02x",hsmsbuf[i]);
	}
	printf("\r\n");
	if(ConfigMsg.debug>0)
		_Debug2(0, "report_S18F81 , packet_len[%d]  \n",hsmsLen);
											
		// send to pc
		//Write_SOCK_Data_Buffer(CONN_SOCKET_NO, USER_BANK_BUFFER, S18F71_LENGTH); //S18F71's length
		send_secs_socket(hsmsbuf,hsmsLen);
	return;
}


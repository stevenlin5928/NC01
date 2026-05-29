#ifndef _SECS_H_
#define _SECS_H_
#include "gd32f10x.h"



//unsigned char SECS_S1F2_HEADER_RESP[16] = {0x00,0x00,0x00,0x37, 0x00,0x00,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //length byte: 0x00,0x00,0x00,0x37

////202405 
//unsigned char SECS_S1F1_DATA_RESP[45] = {0x01,0x02,0x41,0x06,'B','R','9','1','0','0',
//											0x41,13,'2','0','2','4','1','0',0x20,0x20,0x31,0x20,0x30,0x20,0x31};

//unsigned char SECS_S18F10_HEADER_RESP[14] = {0x00,0x00,0x00,0x3b,0x00,0x00,0x12,0x0a,0x00,0x00,0x00,0x00,0x00,0x00};
//unsigned char SECS_S18F10_DATA_RESP[12] = {0x01,0x04,0x41,0x02,0x30,0x31,0x41,0x02,0x4E,0x4F,0x41,0x10};
//unsigned char SECS_S18F10_DATA2_RESP[21] =	{0x01,0x04,0x41,0x02,0x4E,0x45,0x41,0x01,0x30,0x41,0x04,0x49,0x44,0x4C,0x45,0x41,0x04,0x49,0x44,0x4C,0x45};

//unsigned char SECS_S18F16_HEADER_RESP[14] = {0x00,0x00,0x00,15, 0x00,0x00,0x12,0x10,0x00,0x00,0x00,0x00,0x00,0x00}; //length byte: 0x00,0x00,0x00,0x37
//unsigned char SECS_S18F16_DATA_RESP[] = {
//		0x01,0x01,//L[1]
//				0x21,0x01,0x00//B[1]
//};
											


typedef struct _DeviceId{
        uint16_t rbit:1;
        uint16_t deviceId_high:7;
        uint16_t deviceId_low:8;
}DeviceId_t;

typedef struct _MessageId{
        uint16_t wbit:1;
        uint16_t MessageId_high:7;
        uint16_t MessageId_low:8;
}MessageId_t;

typedef struct _BlockId{
        uint16_t ebit:1;
        uint16_t MessageId_high:7;
        uint16_t MessageId_low:8;
}BlockId_t;

typedef struct _secs_header{
        DeviceId_t  deviceid;
        MessageId_t messageid;
        BlockId_t blockid;
        char system[4];
}secs_1_header;

typedef struct _hsms_header{
        uint16_t session_id;
        MessageId_t message_id;
        uint8_t pType;//==1: SECS-II ENCODING;==00:HSMS
        uint8_t sType;
        uint8_t system[4];
}hsms_header;

typedef struct _Hsms{
        uint32_t len;
        hsms_header header;
        char msg[128];
}HSMS;

//alarm ID
#define ALID_Outside_RFID_Fail  0X01
#define ALID_Inside_RFID_Fail   0X02//only return status,off led,not lock machine
#define ALID_Process_Timeout    0x03//on LED,Lock machine
#define ALID_Tool_issue         0x04//on LED,Lock machine
#define Divice_ID            0
//Port ID
#define PortId_Inside_RFID   0
#define PortId_Outside_RFID  1

//S18F71
#define TARGETID_Inside   0X0  //Inside RFID Reader?Barcode
#define TARGETID_Outside  0X0 //Outside RFID Reader?Barcode
#define CEID_LOAD         0x00// Load
#define CEID_UNLOAD				0x01//unload

void report_S2F17(uint32_t session,uint8_t *datetime,uint8_t len);
void report_S2F18(uint32_t session,uint8_t *datetime,uint8_t len);
void report_S2F22(uint32_t session);
void report_S5F1(uint8_t alarmId,uint8_t deviceId,uint8_t portId);
void report_S18F10(uint8_t targetid,uint8_t *lfData,uint8_t lflen,uint32_t session);
void report_S18F71(uint8_t targetid,uint8_t ceid,uint8_t *IdData,uint8_t idlen,uint8_t *barData,uint8_t barlen);
void report_S18F71_LF_4(uint8_t ceid,int ant,uint8_t *buf,int len);
void report_S18F75(uint8_t ceid,int ant,uint8_t *buf,int len);
void report_S18F81(uint8_t targetid,uint8_t ceid,uint8_t *trackInData,uint8_t trackinlen,uint8_t *trackOutData,uint8_t trackoutlen,uint8_t *barData,uint8_t barlen);
#endif/**/

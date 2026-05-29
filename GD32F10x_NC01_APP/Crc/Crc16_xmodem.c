/*
* Calculate the crc16-xmodem
*/
#include <stdio.h>
#include "Crc16_xmodem.h"

static unsigned short Crc16CcittxmodemTable[256]=
{
	0,	4129,	8258,	12387,	16516,	20645,	24774,	28903,
	33032,	37161,	41290,	45419,	49548,	53677,	57806,	61935,
	4657,	528,	12915,	8786,	21173,	17044,	29431,	25302,
	37689,	33560,	45947,	41818,	54205,	50076,	62463,	58334,
	9314,	13379,	1056,	5121,	25830,	29895,	17572,	21637,
	42346,	46411,	34088,	38153,	58862,	62927,	50604,	54669,
	13907,	9842,	5649,	1584,	30423,	26358,	22165,	18100,
	46939,	42874,	38681,	34616,	63455,	59390,	55197,	51132,
	18628,	22757,	26758,	30887,	2112,	6241,	10242,	14371,
	51660,	55789,	59790,	63919,	35144,	39273,	43274,	47403,
	23285,	19156,	31415,	27286,	6769,	2640,	14899,	10770,
	56317,	52188,	64447,	60318,	39801,	35672,	47931,	43802,
	27814,	31879,	19684,	23749,	11298,	15363,	3168,	7233,
	60846,	64911,	52716,	56781,	44330,	48395,	36200,	40265,
	32407,	28342,	24277,	20212,	15891,	11826,	7761,	3696,
	65439,	61374,	57309,	53244,	48923,	44858,	40793,	36728,
	37256,	33193,	45514,	41451,	53516,	49453,	61774,	57711,
	4224,	161,	12482,	8419,	20484,	16421,	28742,	24679,
	33721,	37784,	41979,	46042,	49981,	54044,	58239,	62302,
	689,	4752,	8947,	13010,	16949,	21012,	25207,	29270,
	46570,	42443,	38312,	34185,	62830,	58703,	54572,	50445,
	13538,	9411,	5280,	1153,	29798,	25671,	21540,	17413,
	42971,	47098,	34713,	38840,	59231,	63358,	50973,	55100,
	9939,	14066,	1681,	5808,	26199,	30326,	17941,	22068,
	55628,	51565,	63758,	59695,	39368,	35305,	47498,	43435,
	22596,	18533,	30726,	26663,	6336,	2273,	14466,	10403,
	52093,	56156,	60223,	64286,	35833,	39896,	43963,	48026,
	19061,	23124,	27191,	31254,	2801,	6864,	10931,	14994,
	64814,	60687,	56684,	52557,	48554,	44427,	40424,	36297,
	31782,	27655,	23652,	19525,	15522,	11395,	7392,	3265,
	61215,	65342,	53085,	57212,	44955,	49082,	36825,	40952,
	28183,	32310,	20053,	24180,	11923,	16050,	3793,	7920
};

/*
* *描述：计算crc
* 函数名：Cal_Crc16_xmodem
* 参数：
*		buf:数据指针
*		len:数据长度
* 返回：
*		16位CRC校验码
*/
unsigned short Cal_Crc16_xmodem(unsigned char *buf, int len)
{
    unsigned short uiCRC = 0;
	int i=0;

	for (i = 0; i < len; i++)
	{
		unsigned char b = (unsigned char)(uiCRC >> 8);
		uiCRC <<= 8;
		uiCRC ^= Crc16CcittxmodemTable[(b ^ buf[i]) & 0xFF];
	}

    return (unsigned short)uiCRC;
}

/*
* *描述：检验crc
* 函数名：CheckCrc
* 参数：
*		buf:数据指针
*		len:数据长度
* 返回：
*		0:crc正常
*		-1:CRC异常
*/
int CheckCrc(unsigned char* buf, int len)
{
	unsigned short uiCRC = (buf[len-2]<<8)|buf[len-1];
	unsigned short crc = Cal_Crc16_xmodem(buf, len - 2);
		//printf("len:%d,crc:0x%x\n", len, crc);
	if (uiCRC ^ crc)
		return -1;
	else
		return 0;
//	return (unsigned short)uiCRC;
}

unsigned short generate_crc16(unsigned  char *buf,unsigned char len)
{
	unsigned short uiCRC = 0;
	int i=0;

	for (i = 0; i < len; i++)
	{
		unsigned char b = (unsigned char)(uiCRC >> 8);
		uiCRC <<= 8;
		uiCRC ^= Crc16CcittxmodemTable[(b ^ buf[i]) & 0xFF];
	}

    return (unsigned short)uiCRC;
}

unsigned char GetXOR(char Cmd[],int len);
int GetATCommandCRC(char byteArray[],int postfixOffset,int len);
char CheckATCommandCRC(char data [],int len);
int GetCRC(char *command, int length);

#if 0
int main()
{
	unsigned char data_cmd1[] = {
		0x5A,0x00,0x01,0x01,0x00,0x00,0x00//DCE5
	};
	unsigned char data_cmd2[] = {
		0x5A,0x00,0x01,0x01,0x08,0x00,0x09,0x00,0x1F,0xE0,0xC0,0xA8,0x01,0x02,0x1F,0xE0//9CFF
	};
	unsigned short crc1 = Cal_Crc16_xmodem(data_cmd1, sizeof(data_cmd1));
	printf("crc1:should be :DCE5,0x%04X,\n", crc1);
	unsigned short  crc2 = Cal_Crc16_xmodem(data_cmd2, sizeof(data_cmd2));
	printf("crc2:should be :9CFF,0x%04X\n", crc2);

	unsigned char data_cmd3[] = { 0x5A,0x00,0x01,0x01,0x15,0x00,0x03,0x01,0x01,0x02 };//19A8};
	unsigned short crc3 = Cal_Crc16_xmodem(data_cmd3, sizeof(data_cmd3));
	printf("crc3:should be :19A8,0x%04X,\n", crc3);

	unsigned char data_cmd4[] = { 0x5A,0x00,0x01,0x01,0x15,0x00,0x01,0x00 };// 7B22
	unsigned short crc4 = Cal_Crc16_xmodem(data_cmd4, sizeof(data_cmd4));
	printf("crc4:should be :7B22,0x%04X,\n", crc4);

	unsigned char data_cmd5[] = { 0x5A,0x00,0x01,0x01,0x16,0x00,0x00 };// 2D26 };
	unsigned short crc5 = Cal_Crc16_xmodem(data_cmd5, sizeof(data_cmd5));
	printf("crc5:should be :2D26,0x%04X,\n", crc5);

	//5A0001011600020102490D
	unsigned char data_cmd6[] = { 0x5A,0x00,0x01,0x01,0x16,0x00,0x02,0x01,0x02 };// 490D 
	unsigned short crc6 = Cal_Crc16_xmodem(data_cmd6, sizeof(data_cmd6));
	printf("crc6:should be :490D,0x%04X,\n", crc6);
	//5A0001013F001301000000000212340002FFFF01000400010101FCDB
	unsigned char data_cmd7[] = { 0x5A,0x00,0x01,0x01,0x3F,0x00,0x13,0x01,0x00,0x00,0x00,0x00,0x02,0x12,0x34,0x00,0x02,0xFF,0xFF,0x01,0x00,0x04,0x00,0x01,0x01,0x01 };//
	unsigned short crc7 = Cal_Crc16_xmodem(data_cmd7, sizeof(data_cmd7));
	printf("crc7:should be :FCDB,0x%04X,\n", crc7);
	//5A0001013F00010024C7
	unsigned char data_cmd8[] = { 0x5A,0x00,0x01,0x01,0x3F,0x00,0x01,0x00 };//
	unsigned short crc8 = Cal_Crc16_xmodem(data_cmd8, sizeof(data_cmd8));
	printf("crc8:should be :24C7,0x%04X,\n", crc8);
	//5A00010140001301000000000212340002FFFF01000400010101B1E0
	unsigned char data_cmd9[] = { 0x5A,0x00,0x01,0x01,0x40,0x00,0x13,0x01,0x00,0x00,0x00,0x00,0x02,0x12,0x34,0x00,0x02,0xFF,0xFF,0x01,0x00,0x04,0x00,0x01,0x01,0x01 };// B1E0 
	unsigned short crc9 = Cal_Crc16_xmodem(data_cmd9, sizeof(data_cmd9));
	printf("crc9:should be :B1E0,0x%04X,\n", crc9);
}
#endif



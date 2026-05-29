


#ifndef __CRC16_XMODEM_H__
#define __CRC16_XMODEM_H__

/*
* *描述：计算crc
* 函数名：Cal_Crc16_xmodem
* 参数：
*		buf:数据指针
*		len:数据长度
* 返回：
*		16位CRC校验码
*/
unsigned short Cal_Crc16_xmodem(unsigned char* buf, int len);

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
int CheckCrc(unsigned char* buf, int len);

/*XOR checksum*/
unsigned char GetXOR(char Cmd[],int len);
int GetATCommandCRC(char byteArray[],int postfixOffset,int len);
char CheckATCommandCRC(char data [],int len);
int GetCRC(char *command, int length);

#endif



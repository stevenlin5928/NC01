#ifndef __CRC_H__
#define __CRC_H__

unsigned char GetXOR_AT(char Cmd[],int len);
int GetATCommandCRC_AT(char byteArray[],int postfixOffset,int len);
int CheckATCommandCRC_AT(char data [],int len);
int GetCRC_AT(char *command, int length);

#endif/***/

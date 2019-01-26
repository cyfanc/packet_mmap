#ifndef __PACKRT_CAPTURE_H__
#define __PACKRT_CAPTURE_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <time.h>
#include <sys/time.h>
#include "print_hex.h"

//抓包socket
int g_nSocketFd = -1;

//mmap首地址
char* g_pMmapAddr = NULL;

//环形缓冲区大小变量
struct tpacket_req g_req;

//设置网卡为混杂模式
int setPromisc(const char *szIfName, int sfd, int iFlags);

//创建socket并在内核中mmap一块环形缓冲区接收数据报文
int initSocket(const char* szIfName);

//从缓冲区提取数据报文
void run();

//分析数据包
int PsrPacket(unsigned char* pData, unsigned int offset, unsigned int len);

//提取以太网层数据
int PsrEth(unsigned char* pData, unsigned int offset, unsigned int len);

//提取ip层数据
int PsrIp(unsigned char* pData, unsigned int offset, unsigned int len);

//提取tcp层数据
int PsrUdp(unsigned char* pData, unsigned int offset, unsigned int len);
int PsrTcp(unsigned char* pData, unsigned int offset, unsigned int len);

//提取应用层数据
int PsrApp(unsigned char* pData, unsigned int offset, unsigned int len);

#endif

#include "packet_capture.h"

int offset = 0;

int setPromisc(const char *szIfName, int sfd, int iFlags)
{
	ifreq stIfr;

	strcpy(stIfr.ifr_name,szIfName);
	
	if(ioctl(sfd,SIOCGIFFLAGS,&stIfr) < 0)
		return -1;

	stIfr.ifr_flags = (iFlags) ? (stIfr.ifr_flags | IFF_PROMISC) : (stIfr.ifr_flags & ~IFF_PROMISC);

	if(ioctl(sfd,SIOCSIFFLAGS,&stIfr) < 0)
		return -1;

	return 0;
}

int initSocket(const char* szIfName)
{
    	ifreq stIf;

    	sockaddr_ll stLocal = {0};

    	if ((g_nSocketFd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
		goto ERR;

	g_req.tp_block_size = getpagesize();
	g_req.tp_block_nr = 1000;
	g_req.tp_frame_size = getpagesize() / 2;
	g_req.tp_frame_nr = (g_req.tp_block_size / g_req.tp_frame_size * g_req.tp_block_nr);
	
	if (setsockopt(g_nSocketFd, SOL_PACKET, PACKET_RX_RING, &g_req, sizeof(g_req)) < 0)
		goto ERR;
	
	g_pMmapAddr = (char *)mmap(0, g_req.tp_block_size * g_req.tp_block_nr, PROT_READ|PROT_WRITE, MAP_SHARED, g_nSocketFd, 0);
    	if (MAP_FAILED == g_pMmapAddr)
		goto ERR;

	//设置网卡为混杂模式
    	if(setPromisc(szIfName, g_nSocketFd, 1) < 0)
		goto ERR;
    
    	/* 获取物理网卡接口索引 */
    	strcpy(stIf.ifr_name,szIfName);
    	if (ioctl(g_nSocketFd, SIOCGIFINDEX, &stIf) < 0)
		goto ERR;

    	/* 绑定物理网卡 */
    	stLocal.sll_family = PF_PACKET;
    	stLocal.sll_ifindex = stIf.ifr_ifindex;
    	stLocal.sll_protocol = htons(ETH_P_IP);
	
	if (bind(g_nSocketFd, (struct sockaddr *)&stLocal, sizeof(stLocal)) < 0)
		goto ERR;

    	return 0;

ERR:
	perror("");
	close(g_nSocketFd);
	return -1;
}

int PsrApp(unsigned char* pData, unsigned int offset, unsigned int len)
{
	show_hex(pData+offset, len - offset);
	return 0;
}

int PsrTcp(unsigned char* pData, unsigned int offset, unsigned int len)
{
	tcphdr* p = (tcphdr*)(pData + offset);

	PRINTF_GREEN("src port    : %d", ntohs(p->source));
	PRINTF_YELLOW("des port    : %d", ntohs(p->dest));
	PRINTF_GREY("seq         : %d", p->seq);
	PRINTF_GREY("ack         : %d", p->ack_seq);

	offset += p->doff * 4;
	return PsrApp(pData, offset, len);;
}

int PsrUdp(unsigned char* pData, unsigned int offset, unsigned int len)
{
	udphdr* p = (udphdr *)(pData + offset);
	PRINTF_GREEN("src port    : %d", ntohs(p->source));
        PRINTF_YELLOW("des port    : %d", ntohs(p->dest));

	offset += sizeof(udphdr);
	return PsrApp(pData, offset, len);
}

int PsrIp(unsigned char* pData, unsigned int offset, unsigned int len)
{
	int nRet = -1;
	struct in_addr s;
	iphdr* p = (iphdr*)(pData + offset);
	
	PRINTF_GREY("tos         : %02x",p->tos);
	PRINTF_GREY("total length: %d(0x%02x)",ntohs(p->tot_len),ntohs(p->tot_len));
	PRINTF_GREY("id          : %d(0x%02x)",ntohs(p->id),ntohs(p->id));
	PRINTF_GREY("segment flag: %d(0x%02x)",ntohs(p->frag_off),ntohs(p->frag_off));
	PRINTF_GREY("ttl         : %02x",p->ttl);
	PRINTF_GREY("protocol    : %02x",p->protocol);
	PRINTF_GREY("check       : %d(0x%02x)",ntohs(p->check), ntohs(p->check));
	
	s.s_addr = p->saddr;
	PRINTF_GREEN("src ip      : %s",inet_ntoa(s));
	
	s.s_addr = p->daddr;
	PRINTF_YELLOW("des ip      : %s",inet_ntoa(s));

	switch(p->protocol)
	{
		case 0x06:
			PRINTF_GREY("protocol    : TCP");
			offset += p->ihl * 4;
			nRet = PsrTcp(pData, offset, len);	
			break;	
		case 0x11:	
			PRINTF_GREY("protocol    : UDP");
			offset += p->ihl * 4;
			nRet = PsrUdp(pData, offset, len);	
			break;
		case 0x01:
			PRINTF_GREY("protocol    : ICMP");		
			break;
	}	
	return nRet;
}

int PsrEth(unsigned char* pData, unsigned int offset, unsigned int len)
{
	int nRet = -1;
	ethhdr* p = (ethhdr*)(pData + offset);
	PRINTF_GREEN("src mac     : %02x:%02x:%02x:%02x:%02x:%02x",p->h_source[0], p->h_source[1], p->h_source[2], p->h_source[3], p->h_source[4], p->h_source[5]);
	PRINTF_YELLOW("des mac     : %02x:%02x:%02x:%02x:%02x:%02x",p->h_dest[0],p->h_dest[1],p->h_dest[2],p->h_dest[3],p->h_dest[4],p->h_dest[5]);
	switch (htons(p->h_proto))
	{
		case 0x0806:
			//PRINTF_GREY("protocol    : ARP");	
			break;
		case 0x8035:
			//PRINTF_GREY("protocol    : RARP");	
			break;
		case 0x0800:
			//PRINTF_GREY("protocol    : IPV4");
			offset = ETH_HLEN;
			nRet = PsrIp(pData, offset, len);
			break;
		case 0x86DD:
			//PRINTF_GREY("protocol    : IPV6");	
			break;
		case 0x8100:
			//PRINTF_GREY("protocol    : VLAN");	
			break;
	}
	return	nRet;
}

int PsrPacket(unsigned char* pData, unsigned int offset, unsigned int len)
{
	return PsrEth(pData, offset, len);
}

void run()
{
	while(1)
	{
		for (unsigned int nIndex = 0; nIndex < g_req.tp_frame_nr; ++nIndex)
		{		
			tpacket_hdr *pHead = (tpacket_hdr *)(g_pMmapAddr + nIndex * g_req.tp_frame_size);
			
			while (TP_STATUS_KERNEL == pHead->tp_status)	usleep(1000);

			unsigned char *pPacket = (unsigned char *)pHead + pHead->tp_mac;
			
			if(pHead->tp_snaplen != 60)
			{
				PsrPacket(pPacket, 0 ,pHead->tp_snaplen);
				show_hex_format(pPacket, pHead->tp_snaplen);
			}

			pHead->tp_status = TP_STATUS_KERNEL;
		}
	}
	return ;
}

int CheckArgs(int argc, char** argv)
{
	if(argc < 2){
		PRINTF_RED("please input %s eth0",argv[0]);
		return -1;
	}

	return 0;
}

int main(int argc, char** argv)
{
	if(CheckArgs(argc, argv))
		return -1;

	if(initSocket(argv[1]))
		return -1;

	run();

	return 0;
}

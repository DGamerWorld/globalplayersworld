#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
typedef int SOCKET;
//#pragma region define win32 const variable in linux
#define INVALID_SOCKET  -1
#define SOCKET_ERROR	-1
//#pragma endregion
#endif

SOCKET	GSocketUDPSocket			= INVALID_SOCKET;
sockaddr_in GSocketUDPAddr, GUDPServer;
typedef struct
{
	char wallet48[48+4];
	sockaddr_in addr;
} CIPPORT;
CIPPORT* GIPPortLists = 0;
uint32   GIPPortMaxs = 0, GIPPortCount = 0;
char GMyWalletChar[256] = {0};

void CDNAddIPPort(const char wallet[48], uint32 IP, uint16 Port)
{
	if (CharEqualNoCase(wallet, GMyWalletChar) == true)	// 自己不加
	{
		return;
	}

	for (uint32 i=0; i<GIPPortCount; ++i)
	{
		if (CharEqualNoCase(GIPPortLists[i].wallet48, wallet) == true)
		{
			GIPPortLists[i].addr.sin_addr.s_addr = IP; GIPPortLists[i].addr.sin_port = Port;
			return;
		}
	}
	if (GIPPortCount >= GIPPortMaxs)
	{
		CIPPORT *p = new CIPPORT[GIPPortMaxs + 256];
		if (p == 0) return;
		if (GIPPortLists)
		{
			memcpy(p, GIPPortLists, sizeof(CIPPORT) * GIPPortCount);
			delete[] GIPPortLists;
		}
		GIPPortLists = p;
		GIPPortMaxs += 256;
	}
	CIPPORT *p = &GIPPortLists[GIPPortCount]; ++GIPPortCount;
	memset(&p->addr, 0, sizeof(sockaddr_in));
	p->addr.sin_family = AF_INET;
	p->addr.sin_addr.s_addr = IP; p->addr.sin_port = Port;
	CharCopy(p->wallet48, 48, wallet);
}
bool CDNInit()
{
	do
	{
		memset(&GSocketUDPAddr, 0, sizeof(sockaddr_in));
		GSocketUDPAddr.sin_family = AF_INET;
		GSocketUDPAddr.sin_addr.s_addr = inet_addr("192.168.0.90"); GSocketUDPAddr.sin_port = htons(10000);

		GSocketUDPSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (GSocketUDPSocket == INVALID_SOCKET) break;

		memset(&GUDPServer, 0, sizeof(sockaddr_in));
		GUDPServer.sin_family = AF_INET;
		GUDPServer.sin_addr.s_addr = INADDR_ANY; GUDPServer.sin_port = htons(10000);
		if (bind(GSocketUDPSocket, (struct sockaddr*)&GUDPServer, sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			char Msg[256]; sprintf(Msg, "UDP bind (error %d)", GetLastError());
			MessageBox(0, Msg, "", 0);
			break;
		}

		// 设置端口复用
		int nOne = 1;
	//	setsockopt(GSocketUDPSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&nOne, sizeof(int));
	//	setsockopt(GSocketUDPSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&nOne, sizeof(int));
		// 设置SOCKET为KEEPALIVE保持连接
	//	int optval = 1;
	//	if (setsockopt(GSocketUDPSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&optval, sizeof(int))) break;
		// 设置为非阻塞方式
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		unsigned long nMode = 1;
		if (ioctlsocket(GSocketUDPSocket, FIONBIO, &nMode) == SOCKET_ERROR) break;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		fcntl(GSocketUDPSocket, F_SETFL, O_NONBLOCK);
#else
		int set = 1;
		setsockopt(GSocketUDPSocket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));	// 如果ios连不上网就会发此信息中断app,这里屏蔽掉
		fcntl(GSocketUDPSocket, F_SETFL, O_NONBLOCK);
	/*	int flags = fcntl(GSocketUDPSocket, F_GETFL, 0);
		if (flags < 0) {break;}
		flags |= O_NONBLOCK;
		if (fcntl(GSocketUDPSocket, F_SETFL, flags) < 0) {break;}*/
#endif

		return true;
	}while (0);

	SocketClose(GSocketUDPSocket);
	return false;
}
void CDNProcess()
{
	if (GMyWalletChar[0] == 0) return;
	if (rand()&7) return;

	char buf[1024]; int sln = sprintf(buf, "A:%s", GMyWalletChar);
	sendto(GSocketUDPSocket, buf, sln, 0, (struct sockaddr*)&GSocketUDPAddr, sizeof(sockaddr_in));
	while (true)
	{
		int len = sizeof(sockaddr_in); sockaddr_in adr;
		int rcv = recvfrom(GSocketUDPSocket, buf, 1024, 0, (struct sockaddr*)&adr, &len);
		if ((rcv > 2) && (buf[1] = ':'))	// %s:
		{
			if (buf[0] == 'B')	// B:%s,%u,%u
			{
				int dh1 = 0, dh2 = 0;
				for (int i=2; i<rcv; ++i)
				{
					if (buf[i] == ',')
					{
						dh1 = i;
						for (int j=i+1; j<rcv; ++j)
						{
							if (buf[j] == ',')
							{
								dh2 = j;
								break;
							}
						}
						break;
					}
				}
				buf[dh1] = 0; buf[dh2] = 0;
				CDNAddIPPort(&buf[2], atoi(&buf[dh1+1]), atoi(&buf[dh2+1]));
			}else if (buf[0] == 'C') {	// C:
				int ii = 0;
			}
		}else if (rcv <= 0) {
			break;
		}
	}

	for (uint32 i=0; i<GIPPortCount; ++i)
	{
		int sln = sprintf(buf, "C:%s", GIPPortLists[i].wallet48);
		int sss = sendto(GSocketUDPSocket, buf, sln, 0, (struct sockaddr*)&GIPPortLists[i].addr, sizeof(sockaddr_in));
		if (sss > 0)
		{
			int ii = 0;
		}
	}
}

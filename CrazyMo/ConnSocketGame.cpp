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

SOCKET	GSocketGameSocket						= INVALID_SOCKET;	// Socket
uint32	GSocketGameHeartMillisecond				= 0xffffffff;		// ������ȥ�غ���
uint32	GSocketGameReconnectTimeMs				= 0;				// ����ʱ�����
uint32	GSocketGameAddr							= INADDR_NONE;		// ��Ϸ��IP
uint16	GSocketGamePort							= 0;				// �˿�
//+���ͻ���
uint16	GSocketGameSendBufferCnt				= 0;				// �������ݴ�С  �߳���
char	GSocketGameSendBuffer4KB[4096+4];							// ���ͻ�����    �߳���
char	GSocketGameSendMemory1KB[256][1024];						// �����ݴ��ڴ�  �������̹߳���
uint16	GSocketGameSendMemoryCnt[256];
uint8	GSocketGameSendLoadIndex				= 0;				// ���Ͷ�ȡ����
uint8	GSocketGameSendSaveIndex				= 0;				// ���ͱ�������
//-���ͻ���
//+���ջ���
uint8	GSocketGameRecvLoadIndex				= 0;				// ���ն�ȡ����
uint8	GSocketGameRecvSaveIndex				= 0;				// ���ձ�������
uint16	GSocketGameRecvMemoryCnt[256];								// �������ݴ�С
char	GSocketGameRecvMemory1KB[256][1024];						// ���ջ�����
char	GSocketGameRecvBufferAll[256*1024+1024];
uint32	GSocketGameRecvBufferCnt				= 0;
//-���ջ���
bool	GSocketGameConnected					= false;
bool	GSocketGameLoginOk						= false;
bool	GSocketGameCanReconnect					= true;

inline void SocketGameClose()
{
	if (GSocketGameSocket != INVALID_SOCKET)
	{
	//	SocketGameDisconnect(GSocketGameLoginOk);	// ����֪ͨ
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		closesocket(GSocketGameSocket);
#else
		close(GSocketGameSocket);
#endif
	}
	GSocketGameSocket = INVALID_SOCKET;

	GSocketGameConnected = false;
	GSocketGameLoginOk = false;
}

void SocketGameCreate(const char ip[], uint16 port)
{
	SocketClose(GSocketGameSocket);
	GSocketGameConnected = false;
	GSocketGameLoginOk = false;
	GSocketGameAddr = SocketAddress(ip);
	GSocketGamePort = SocketPort(port);
	GSocketGameHeartMillisecond = 0xffffffff;
}
bool SocketSendMsgToConn(const char msgBody[], uint16 msgLens)
{
	if (GSocketGameLoginOk == false) return false;	// ��½�ɹ����ܷ�
	while (msgLens > 0)
	{
		uint8 nxSendSave = GSocketGameSendSaveIndex + 1, SendLoad = GSocketGameSendLoadIndex;	// ����ȡ�����Ͳ��ü�����
		if (nxSendSave == SendLoad) return false;
		if (msgLens <= 1024)
		{
			GSocketGameSendMemoryCnt[GSocketGameSendSaveIndex] = msgLens;
			memcpy(GSocketGameSendMemory1KB[GSocketGameSendSaveIndex], msgBody, msgLens);
			msgLens = 0;
		}else {
			GSocketGameSendMemoryCnt[GSocketGameSendSaveIndex] = 1024;
			memcpy(GSocketGameSendMemory1KB[GSocketGameSendSaveIndex], msgBody, 1024);
			msgLens -= 1024;
		}
		++GSocketGameSendSaveIndex;
	}
	return true;
}
void SocketGameSendRecvThread()	// ���̲߳��ܰ���SocketSendMsgToConn,����������̻߳����GSocketGameSendSaveIndex
{
	if (GSocketGameSocket == INVALID_SOCKET)	// ��û����
	{
		if (GSocketGameAddr == INADDR_NONE) return;	// ������Ҫ��¼

		GSocketGameConnected = false;
		GSocketGameSocket = SocketConnect(GSocketGameAddr, GSocketGamePort);
		if (GSocketGameSocket == INVALID_SOCKET) return;	// ����ʧ�ܷ���
		GSocketGameConnected = true;
		//+���
		GSocketGameSendLoadIndex = 0; GSocketGameSendSaveIndex = 0;
		GSocketGameSendBufferCnt = 0;
		GSocketGameRecvLoadIndex = 0; GSocketGameRecvSaveIndex = 0;
		GSocketGameRecvBufferCnt = 0;
		for (int i=0; i<256; ++i)
		{
			GSocketGameSendMemoryCnt[i] = 0;
			GSocketGameRecvMemoryCnt[i] = 0;
		}
		GSocketGameLoginOk = false;
		GSocketGameReconnectTimeMs = 0;	// ���ӳɹ���Ҫ�������ʱ��
		GSocketGameCanReconnect = true;
		//-���
		//+��ȡ��¼��ϢȻ����
		GSocketGameSendBufferCnt = SocketGameGetLoginDataThread(GSocketGameSendBuffer4KB, (GSocketGameHeartMillisecond==0xffffffff)? false: true);	// ���ɹ�ȡ��½���� ����������GSocketGameSendBuffer4KB
		//-��ȡ��¼��ϢȻ����
		//+����������������Ϊ��ͬ��������ʱ��
		GSocketGameHeartMillisecond = 0;
		GSocketGameSendBufferCnt += sprintf(&GSocketGameSendBuffer4KB[GSocketGameSendBufferCnt], "{eType=%s}", (char*)(&SYSID_CHECK));
		//-����������
	}
	uint8 SendSave = GSocketGameSendSaveIndex;	// ����ȡ�����Ͳ��ü�����
	if (SendSave != GSocketGameSendLoadIndex)	// ������
	{
		for (int i=0; i<256; ++i)
		{
			uint16 ln = GSocketGameSendMemoryCnt[GSocketGameSendLoadIndex];
			if (GSocketGameSendBufferCnt + ln < 4096)
			{
				memcpy(&GSocketGameSendBuffer4KB[GSocketGameSendBufferCnt], GSocketGameSendMemory1KB[GSocketGameSendLoadIndex], ln);
				GSocketGameSendBufferCnt += ln;

				++GSocketGameSendLoadIndex;
				if (SendSave == GSocketGameSendLoadIndex) break;
			}else {
				break;
			}
		}
	}
	if (GSocketGameSendBufferCnt > 0)	// ����ϢҪ����
	{
		long snlen = send(GSocketGameSocket, GSocketGameSendBuffer4KB, GSocketGameSendBufferCnt, 0);
		if (snlen > 0)
		{
			if (snlen < GSocketGameSendBufferCnt)
			{
				GSocketGameSendBufferCnt -= snlen;
				memcpy(GSocketGameSendBuffer4KB, &GSocketGameSendBuffer4KB[snlen], GSocketGameSendBufferCnt);
			}else {
				GSocketGameSendBufferCnt = 0;
			}
		}else if (snlen == SOCKET_ERROR) {
			if (SocketIsError())	// ����
			{
				SocketGameClose();
			}
		}
	}
	uint8 nxRecvSave = GSocketGameRecvSaveIndex + 1, RecvLoad = GSocketGameRecvLoadIndex;	// ����ȡ�����Ͳ��ü�����
	if (nxRecvSave != RecvLoad)	// ������Ϣ����û��
	{
		while (true)
		{
			long inlen = recv(GSocketGameSocket, GSocketGameRecvMemory1KB[GSocketGameRecvSaveIndex], 1024, 0);
			if (inlen <= -1)
			{
				if (SocketIsError())	// ����
				{
					SocketGameClose();
				}
				break;
			}
			if (inlen == 0)	// �Ͽ�
			{
				SocketGameClose();
				break;
			}

			GSocketGameRecvMemoryCnt[GSocketGameRecvSaveIndex] = inlen;
			++GSocketGameRecvSaveIndex;
			nxRecvSave = GSocketGameRecvSaveIndex + 1;
			if (nxRecvSave == RecvLoad) break;	// ��Ϣ���б�������
		}
	}
}
void SocketGameRecvMsgProcess()
{
	if (GSocketGameConnected == true)	// ���ӳɹ����÷�������
	{
		GSocketGameHeartMillisecond += GTimeClientSubtMs;
		if (GSocketGameHeartMillisecond > 15 * 1000)	// 15��
		{
			GSocketGameHeartMillisecond = 0;
			ServerSendMsgByType(SYSID_CHECK, 0);	// ������
		}
	}

	uint8 RecvSave = GSocketGameRecvSaveIndex;	// ����ȡ�����Ͳ��ü�����
	if (RecvSave != GSocketGameRecvLoadIndex)	// ����Ϣ
	{
		for (int i=0; i<0xff; ++i)
		{
			memcpy(&GSocketGameRecvBufferAll[GSocketGameRecvBufferCnt], GSocketGameRecvMemory1KB[GSocketGameRecvLoadIndex], GSocketGameRecvMemoryCnt[GSocketGameRecvLoadIndex]);
			GSocketGameRecvBufferCnt += GSocketGameRecvMemoryCnt[GSocketGameRecvLoadIndex];
			++GSocketGameRecvLoadIndex;
			if (RecvSave == GSocketGameRecvLoadIndex) break;
		}
		uint32 Size = 0, Lens = JsonAnalysisMessage(GSocketGameRecvBufferAll, GSocketGameRecvBufferCnt);
		while (Lens > 0)
		{
			uint32 msgType = JsonGetCharUInt("eType");
			if (msgType == SYSID_CHECK)	// {eType=%s,tm=%u,dt=%u,zt=%u}
			{
				uint32 cuMs = JsonGetUInt("tm");
				GDateGMServerDate = JsonGetUInt("dt");
				GTimeGMServerTime = cuMs + (GSocketGameHeartMillisecond>>1);
			}else {
				SocketGameRecvMsgProcessCall(msgType, &GSocketGameRecvBufferAll[Size], Lens);
			}

			Size += Lens;
			Lens = JsonAnalysisMessage(&GSocketGameRecvBufferAll[Size], GSocketGameRecvBufferCnt - Size);
		}
		GSocketGameRecvBufferCnt -= Size;
		if (GSocketGameRecvBufferCnt > 0) memcpy(GSocketGameRecvBufferAll, &GSocketGameRecvBufferAll[Size], GSocketGameRecvBufferCnt);
	}

	CDNProcess();
}

bool ServerSendMsgByType(uint32 msgType, uint8 ConIdx)
{
	char c[256]; int len = sprintf(c, "{eType=%s}", (char*)(&msgType));
	SocketSendMsgToConn(c, len);
	return true;
}

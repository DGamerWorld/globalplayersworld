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
uint32	GSocketGameHeartMillisecond				= 0xffffffff;		// 心跳包去回毫秒
uint32	GSocketGameReconnectTimeMs				= 0;				// 重连时间毫秒
uint32	GSocketGameAddr							= INADDR_NONE;		// 游戏服IP
uint16	GSocketGamePort							= 0;				// 端口
//+发送缓冲
uint16	GSocketGameSendBufferCnt				= 0;				// 发送数据大小  线程用
char	GSocketGameSendBuffer4KB[4096+4];							// 发送缓冲区    线程用
char	GSocketGameSendMemory1KB[256][1024];						// 发送暂存内存  进程与线程共用
uint16	GSocketGameSendMemoryCnt[256];
uint8	GSocketGameSendLoadIndex				= 0;				// 发送读取索引
uint8	GSocketGameSendSaveIndex				= 0;				// 发送保存索引
//-发送缓冲
//+接收缓冲
uint8	GSocketGameRecvLoadIndex				= 0;				// 接收读取索引
uint8	GSocketGameRecvSaveIndex				= 0;				// 接收保存索引
uint16	GSocketGameRecvMemoryCnt[256];								// 接收数据大小
char	GSocketGameRecvMemory1KB[256][1024];						// 接收缓冲区
char	GSocketGameRecvBufferAll[256*1024+1024];
uint32	GSocketGameRecvBufferCnt				= 0;
//-接收缓冲
bool	GSocketGameConnected					= false;
bool	GSocketGameLoginOk						= false;
bool	GSocketGameCanReconnect					= true;

inline void SocketGameClose()
{
	if (GSocketGameSocket != INVALID_SOCKET)
	{
	//	SocketGameDisconnect(GSocketGameLoginOk);	// 断线通知
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
	if (GSocketGameLoginOk == false) return false;	// 登陆成功才能发
	while (msgLens > 0)
	{
		uint8 nxSendSave = GSocketGameSendSaveIndex + 1, SendLoad = GSocketGameSendLoadIndex;	// 必须取出来就不用加锁了
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
void SocketGameSendRecvThread()	// 此线程不能包含SocketSendMsgToConn,否则进程与线程会混乱GSocketGameSendSaveIndex
{
	if (GSocketGameSocket == INVALID_SOCKET)	// 还没创建
	{
		if (GSocketGameAddr == INADDR_NONE) return;	// 还不需要登录

		GSocketGameConnected = false;
		GSocketGameSocket = SocketConnect(GSocketGameAddr, GSocketGamePort);
		if (GSocketGameSocket == INVALID_SOCKET) return;	// 重连失败返回
		GSocketGameConnected = true;
		//+清除
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
		GSocketGameReconnectTimeMs = 0;	// 连接成功就要清除重连时间
		GSocketGameCanReconnect = true;
		//-清除
		//+获取登录信息然后发送
		GSocketGameSendBufferCnt = SocketGameGetLoginDataThread(GSocketGameSendBuffer4KB, (GSocketGameHeartMillisecond==0xffffffff)? false: true);	// 连成功取登陆数据 所以这里用GSocketGameSendBuffer4KB
		//-获取登录信息然后发送
		//+发送心跳包立即是为了同步服务器时间
		GSocketGameHeartMillisecond = 0;
		GSocketGameSendBufferCnt += sprintf(&GSocketGameSendBuffer4KB[GSocketGameSendBufferCnt], "{eType=%s}", (char*)(&SYSID_CHECK));
		//-发送心跳包
	}
	uint8 SendSave = GSocketGameSendSaveIndex;	// 必须取出来就不用加锁了
	if (SendSave != GSocketGameSendLoadIndex)	// 有数据
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
	if (GSocketGameSendBufferCnt > 0)	// 有信息要发送
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
			if (SocketIsError())	// 错误
			{
				SocketGameClose();
			}
		}
	}
	uint8 nxRecvSave = GSocketGameRecvSaveIndex + 1, RecvLoad = GSocketGameRecvLoadIndex;	// 必须取出来就不用加锁了
	if (nxRecvSave != RecvLoad)	// 接收消息队列没满
	{
		while (true)
		{
			long inlen = recv(GSocketGameSocket, GSocketGameRecvMemory1KB[GSocketGameRecvSaveIndex], 1024, 0);
			if (inlen <= -1)
			{
				if (SocketIsError())	// 错误
				{
					SocketGameClose();
				}
				break;
			}
			if (inlen == 0)	// 断开
			{
				SocketGameClose();
				break;
			}

			GSocketGameRecvMemoryCnt[GSocketGameRecvSaveIndex] = inlen;
			++GSocketGameRecvSaveIndex;
			nxRecvSave = GSocketGameRecvSaveIndex + 1;
			if (nxRecvSave == RecvLoad) break;	// 消息队列保存已满
		}
	}
}
void SocketGameRecvMsgProcess()
{
	if (GSocketGameConnected == true)	// 连接成功才用发心跳包
	{
		GSocketGameHeartMillisecond += GTimeClientSubtMs;
		if (GSocketGameHeartMillisecond > 15 * 1000)	// 15秒
		{
			GSocketGameHeartMillisecond = 0;
			ServerSendMsgByType(SYSID_CHECK, 0);	// 心跳包
		}
	}

	uint8 RecvSave = GSocketGameRecvSaveIndex;	// 必须取出来就不用加锁了
	if (RecvSave != GSocketGameRecvLoadIndex)	// 有信息
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

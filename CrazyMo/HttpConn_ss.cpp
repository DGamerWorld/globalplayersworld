#include "global.h"

#define BUFFER_HTTP_CONN_64K				(0xffff+1)		// 0xffff + 1
SOCKET	GHttpConnSocket[0xff+1] = {INVALID_SOCKET};			// socket
uint32	GHttpConnAddrs[0xff+1];								// IP
uint16	GHttpConnPorts[0xff+1];								// 地址
#define HTTP_CONN_ORDER_IDEL				0x00			// 空闲
#define HTTP_CONN_ORDER_DODO				0x10			// 执行命令
#define HTTP_CONN_ORDER_DOWN				0x20			// 下载文件
#define HTTP_CONN_ORDER_POST				0x40			// 上传文件
#define HTTP_CONN_ORDER_SEND				0x01			// 发送信息
#define HTTP_CONN_ORDER_RECV				0x02			// 接受信息
#define HTTP_CONN_ORDER_OVER				0x80			// 完成
uint8	GHttpConnOrder[0xff+1];								// 命令
#define HTTP_CONN_STATE_INIT				0x00			// 初始化
#define HTTP_CONN_STATE_SEND				0x01			// 发送错误
#define HTTP_CONN_STATE_RECV				0x02			// 接受错误
#define HTTP_CONN_STATE_RVKJ				0x04			// 接受空间不足
#define HTTP_CONN_STATE_FILE				0x08			// 保存的文件打开失败
#define HTTP_CONN_STATE_CONN				0x10			// 连接服务器
#define HTTP_CONN_STATE_FAIL				0x40			// 失败
#define HTTP_CONN_STATE_SUCC				0x80			// 成功
uint8	GHttpConnState[0xff+1];								// 状态
uint8	GHttpConnCount = 0;									// 
#define HttpConnSetOver(idx, sta)	{GHttpConnOrder[idx] = HTTP_CONN_ORDER_OVER; GHttpConnState[idx] = sta;}
#define HTTP_CONN_THREAD_FAIL				0x00			// 失败
#define HTTP_CONN_THREAD_CONN				0x01			// 需要继续
#define HTTP_CONN_THREAD_SUCC				0xff			// 成功

char	GHttpConnSendRecv64K[256][BUFFER_HTTP_CONN_64K] = {{0}};
uint32	GHttpConnSendCount[256] = {0}, GHttpConnRecvCount[256] = {0};
uint32	GHttpConnRetryTimes[256];
void HttpConnDoDoThread(uint8 Idx)
{
	if (GHttpConnSocket[Idx] == INVALID_SOCKET)	// 还没连接服务器
	{
		GHttpConnSocket[Idx] = SocketConnect(GHttpConnAddrs[Idx], GHttpConnPorts[Idx]);	// 连接超级节点服
		if (GHttpConnSocket[Idx] == INVALID_SOCKET)	// 连接失败
		{
			if (++GHttpConnRetryTimes[Idx] > 40*30/*30秒*/) {HttpConnSetOver(Idx, HTTP_CONN_STATE_CONN);}	// 超30秒还失败则认为无效服务器
			return;
		}
		GHttpConnRetryTimes[Idx] = 0;	// 连接成功则重试次数清0
	}
	//+send
	if (GHttpConnOrder[Idx] & HTTP_CONN_ORDER_SEND)	// 发送数据阶段
	{
		char *http = GHttpConnSendRecv64K[Idx]; int hlen = strlen(http);
		int snln = send(GHttpConnSocket[Idx], &http[GHttpConnSendCount[Idx]], hlen - GHttpConnSendCount[Idx], 0);
		if (snln > 0)	// 发送了数据
		{
			GHttpConnSendCount[Idx] += snln;
			if (GHttpConnSendCount[Idx] >= hlen)	// 发送完毕
			{
				GHttpConnOrder[Idx] = HTTP_CONN_ORDER_DODO | HTTP_CONN_ORDER_RECV;	// 标记为接受数据阶段
				GHttpConnRetryTimes[Idx] = 0;
			}else {
				return;	// 没发送完就等下次发
			}
		}else if (
			(snln == 0) ||	// 断开
			((snln == SOCKET_ERROR) && SocketIsError()) ||	// 错误
			(++GHttpConnRetryTimes[Idx] > 40*30/*30秒*/)	// 超30秒
			)
		{
			SocketClose(GHttpConnSocket[Idx]);
			HttpConnSetOver(Idx, HTTP_CONN_STATE_SEND);
			return;
		}else {
			return;	// 网络有错误等下次发
		}
	}
	//-send
	//+recv
	if (GHttpConnOrder[Idx] & HTTP_CONN_ORDER_RECV)	// 接受数据阶段
	{
		int State = 0, Leng = 0, FBgn = 0, FEnd = 0; bool Head = true; char *http = GHttpConnSendRecv64K[Idx];
		int rcln = recv(GHttpConnSocket[Idx], &http[GHttpConnRecvCount[Idx]], BUFFER_HTTP_CONN_64K - GHttpConnRecvCount[Idx], 0);
		if (rcln > 0)
		{
			GHttpConnRecvCount[Idx] += rcln;	// 总长度
			if (Head == true)
			{
				int ret = HttpGetHead(State, Leng, FBgn, FEnd, http, GHttpConnRecvCount[Idx]);
				if ((ret > 0) && (ret <= GHttpConnRecvCount[Idx]))	{rcln = ret; Head = false;}
				else												{rcln = GHttpConnRecvCount[Idx];}
			}else {
				rcln = 0;
			}
			if (GHttpConnRecvCount[Idx] >= BUFFER_HTTP_CONN_64K)	// 接受缓冲大小被限制为64K
			{
				SocketClose(GHttpConnSocket[Idx]);
				HttpConnSetOver(Idx, HTTP_CONN_STATE_RECV);
			}else if (GHttpConnRecvCount[Idx] - rcln >= Leng) {	// 信息完整
				memcpy(http, &http[rcln], Leng); http[Leng] = 0;
				SocketClose(GHttpConnSocket[Idx]);
				HttpConnSetOver(Idx, (State==200)? HTTP_CONN_STATE_SUCC: HTTP_CONN_STATE_FAIL);
			}else {
				return;	// 没接收完就等下次收
			}
		}else if (
			(rcln == 0) ||	// 断开
			((rcln == SOCKET_ERROR) && SocketIsError()) ||	// 错误
			(++GHttpConnRetryTimes[Idx] > 40*60/*60秒*/)	// 超60秒
			)
		{
			SocketClose(GHttpConnSocket[Idx]);
			HttpConnSetOver(Idx, HTTP_CONN_STATE_RECV);
		}else {
			return;	// 网络有错误等下次收
		}
	}
	//-recv
}
char	GHttpDownWallet4K[256][4096] = {{0}};
uint8	GHttpDownWalLoad = 0, GHttpDownWalSave = 0;
uint8 HttpConnDownThread(uint8 Idx)
{
	if (GHttpConnSocket[Idx] == INVALID_SOCKET)	// 还没连接服务器
	{
		GHttpConnSocket[Idx] = SocketConnect(GHttpConnAddrs[Idx], GHttpConnPorts[Idx]);	// 连接超级节点服
		if (GHttpConnSocket[Idx] == INVALID_SOCKET)	// 连接失败
		{
			if (++GHttpConnRetryTimes[Idx] > 40*30/*30秒*/) {HttpConnSetOver(Idx, HTTP_CONN_STATE_CONN);}	// 超30秒还失败则认为无效服务器
			return HTTP_CONN_THREAD_FAIL;	// 失败
		}
		GHttpConnRetryTimes[Idx] = 0;	// 连接成功则重试次数清0
	}
	//+send
	if (GHttpConnOrder[Idx] & HTTP_CONN_ORDER_SEND)	// 发送数据阶段
	{
		char *http = GHttpConnSendRecv64K[Idx]; int hlen = strlen(http);
		int snln = send(GHttpConnSocket[Idx], &http[GHttpConnSendCount[Idx]], hlen - GHttpConnSendCount[Idx], 0);
		if (snln > 0)	// 发送了数据
		{
			GHttpConnSendCount[Idx] += snln;
			if (GHttpConnSendCount[Idx] >= hlen)	// 发送完毕
			{
				GHttpConnOrder[Idx] = HTTP_CONN_ORDER_DOWN | HTTP_CONN_ORDER_RECV;	// 标记为接受数据阶段
				GHttpConnRetryTimes[Idx] = 0;
			}else {
				return HTTP_CONN_THREAD_CONN;	// 没发送完就等下次发
			}
		}else if (
			(snln == 0) ||	// 断开
			((snln == SOCKET_ERROR) && SocketIsError()) ||	// 错误
			(++GHttpConnRetryTimes[Idx] > 40*30/*30秒*/)	// 超30秒
			)
		{
			SocketClose(GHttpConnSocket[Idx]);
			HttpConnSetOver(Idx, HTTP_CONN_STATE_SEND);
			return HTTP_CONN_THREAD_FAIL;
		}else {
			return HTTP_CONN_THREAD_CONN;	// 网络有错误等下次发
		}
	}
	//-send
	//+recv
	if (GHttpConnOrder[Idx] & HTTP_CONN_ORDER_RECV)	// 接受数据阶段
	{
		int State = 0, Leng = 0, FBgn = 0, FEnd = 0; char *http = GHttpConnSendRecv64K[Idx];
		int rcln = recv(GHttpConnSocket[Idx], &http[GHttpConnRecvCount[Idx]], BUFFER_HTTP_CONN_64K - GHttpConnRecvCount[Idx], 0);
		if (rcln > 0)
		{
			GHttpConnRecvCount[Idx] += rcln;	// 总长度
			int ret = HttpGetHead(State, Leng, FBgn, FEnd, http, GHttpConnRecvCount[Idx]);
			if ((ret > 0) && (ret <= GHttpConnRecvCount[Idx]))	{rcln = ret;}
			else												{rcln = GHttpConnRecvCount[Idx];}
			if (GHttpConnRecvCount[Idx] >= BUFFER_HTTP_CONN_64K)	// 接受缓冲大小被限制为64K
			{
				SocketClose(GHttpConnSocket[Idx]);
				HttpConnSetOver(Idx, HTTP_CONN_STATE_RECV);
				return HTTP_CONN_THREAD_FAIL;
			}else if (GHttpConnRecvCount[Idx] - rcln >= Leng) {	// 信息完整
				JsonHttpAnalysisMessage(&http[rcln], Leng);
				char bci[256]; JsonHttpGetChar(bci, 255, "bci");
				char wal[256]; JsonHttpGetChar(wal, 255, "wallet");
				uint32 block = JsonHttpGetUInt("block");
				DBWalletBlockSetByHttp(bci, wal, block);
				HttpConnDownWallet(bci, wal);
				SocketClose(GHttpConnSocket[Idx]);
				return HTTP_CONN_THREAD_SUCC;
			}else {
				return HTTP_CONN_THREAD_CONN;	// 没接收完就等下次收
			}
		}else if (
			(rcln == 0) ||	// 断开
			((rcln == SOCKET_ERROR) && SocketIsError()) ||	// 错误
			(++GHttpConnRetryTimes[Idx] > 40*60/*60秒*/)	// 超60秒
			)
		{
			SocketClose(GHttpConnSocket[Idx]);
			HttpConnSetOver(Idx, HTTP_CONN_STATE_RECV);
			return HTTP_CONN_THREAD_FAIL;
		}else {
			return HTTP_CONN_THREAD_CONN;	// 网络有错误等下次收
		}
	}
	//-recv
	return HTTP_CONN_THREAD_FAIL;
}
void HttpConnPostThread(uint8 Idx)
{
	SocketClose(GHttpConnSocket[Idx]);
	GHttpConnSocket[Idx] = SocketConnect(GHttpConnAddrs[Idx], GHttpConnPorts[Idx]);				// 连接到超级节点服
	if (GHttpConnSocket[Idx] == INVALID_SOCKET) {GHttpConnState[Idx] = HTTP_CONN_STATE_FAIL; return;}	// 连接失败
	//+send
/*	char fn[512]; sprintf(fn, "%s%s", GAppPath, GHttpConnSendBody64K);
	FILE *fp = fopen(fn, "rb");
	if (fp == 0) {GHttpConnState[Idx] = HTTP_CONN_STATE_FILE; return;}
	fseek(fp, 0, SEEK_END);
	int SendCnt = ftell(fp), SendPos = 0; char Send4KB[BUFFER_HTTP_CONN_4KB]; sockaddr_in ConnIP; ConnIP.sin_addr.s_addr = GHttpConnAddrs[Idx];
	int SLen = sprintf(Send4KB, "POST %s?eType=%s HTTP/1.1\r\nHost: %s\r\n\r\n", GHttpConnSendBody64K, (char*)(&HTTPID_UPLOAD), inet_ntoa(ConnIP.sin_addr));
	if (SLen + SendCnt <= BUFFER_HTTP_CONN_4KB)
	{
		fseek(fp, 0, SEEK_SET);
		fread(&Send4KB[SLen], 1, SendCnt, fp);
		SLen += SendCnt;
		SendPos = SendCnt;
	}else {
		fseek(fp, 0, SEEK_SET);
		fread(&Send4KB[SLen], 1, BUFFER_HTTP_CONN_4KB - SLen, fp);
		SLen = BUFFER_HTTP_CONN_4KB;
		SendPos = BUFFER_HTTP_CONN_4KB - SLen;
	}
	while (true)
	{
		int snln = send(GHttpConnSocket[Idx], Send4KB, SLen, 0);
		if (snln > 0)
		{
			if (snln < SLen) {SLen -= snln; memcpy(Send4KB, &Send4KB[snln], SLen);}	// 没发送完的数据前移
			else {SLen = 0;}
		}else if (snln == SOCKET_ERROR) {
			if (SocketIsError()) {SocketClose(GHttpConnSocket[Idx]); GHttpConnState[Idx] = HTTP_CONN_STATE_SEND; break;}	// 错误
		}
		if (SendPos >= SendCnt)	// 文件已读完
		{
			if (SLen == 0) {GHttpConnState[Idx] = HTTP_CONN_STATE_SUCC; break;}	// 而且数据也发送完毕就中断否则继续发送剩余数据
		}else if (SLen + SendCnt > BUFFER_HTTP_CONN_4KB + SendPos) {	// SLen + SendCnt - SendPos > BUFFER_HTTP_CONN_4KB
			fseek(fp, SendPos, SEEK_SET);
			fread(&Send4KB[SLen], 1, BUFFER_HTTP_CONN_4KB - SLen, fp);
			SLen = BUFFER_HTTP_CONN_4KB;
			SendPos += BUFFER_HTTP_CONN_4KB - SLen;
		}else {	// SLen + SendCnt - SendPos <= BUFFER_HTTP_CONN_4KB
			fseek(fp, SendPos, SEEK_SET);
			fread(&Send4KB[SLen], 1, SendCnt - SendPos, fp);
			SLen += SendCnt - SendPos;
			SendPos += SendCnt - SendPos;
		}
	}
	//-send
	fclose(fp);*/
}
void HttpConnSendMsgToSuper(const char url[], uint32 msgType, const char Http[])	// 发消息给超级节点服
{
	sockaddr_in ConnIP;
	for (int i=0; i<GHttpConnCount; ++i)
	{
		ConnIP.sin_addr.s_addr = GHttpConnAddrs[i];
		GHttpConnSendCount[i] = (uint16)sprintf(GHttpConnSendRecv64K[i], "GET %s?eType=%s&%s HTTP/1.1\r\nHost: %s\r\n\r\n", url, (char*)(&msgType), Http, inet_ntoa(ConnIP.sin_addr));
		GHttpConnState[i] = HTTP_CONN_STATE_INIT;
		GHttpConnOrder[i] = HTTP_CONN_ORDER_DODO | HTTP_CONN_ORDER_SEND;
		GHttpConnSendCount[i] = 0; GHttpConnRecvCount[i] = 0;
		GHttpConnRetryTimes[i] = 0;
		SocketClose(GHttpConnSocket[i]);
	}
}

typedef struct
{
	uint32 Block;										// 下载起始块
	char BCI8[8+4];
	char Wallet48[48+4];								// 钱包
} CWALLETDOWN, * WalletDownPtr;
WalletDownPtr GWalletDownLists = 0;
uint32 GWalletDownCount = 0, GWalletDownMaxCn = 0;
void WalletDownRelease()
{
	GWalletDownCount = 0; GWalletDownMaxCn = 0;
	if (GWalletDownLists) {delete[] GWalletDownLists; GWalletDownLists = 0;}
}
void WalletDownAdda(const char bci[8], const char wallet[48], uint32 bkBgn, uint32 bkEnd)
{
	for (uint32 bk=bkBgn; bk<bkEnd; ++bk)
	{
		for (uint32 i=0; i<GWalletDownCount; ++i)
		{
			if ((CharEqualNoCase(GWalletDownLists[i].BCI16, bci) == true) &&
				(CharEqualNoCase(GWalletDownLists[i].Wallet48, wallet) == true) &&
				(GWalletDownLists[i].Block == bk))
			{
				return;
			}
		}
		if (GWalletDownCount >= GWalletDownMaxCn)	// 空间不够重新申请
		{
			WalletDownPtr p = new CWALLETDOWN[GWalletDownMaxCn + 16];
			if (p == 0) return;	// 内存不够直接返回
			if (GWalletDownLists)
			{
				memcpy(p, GWalletDownLists, sizeof(CWALLETDOWN) * GWalletDownCount);	// 原来的拷贝到新内存里
				delete[] GWalletDownLists;	// 释放原来的内存
			}
			GWalletDownLists = p;	// 指向新内存
			GWalletDownMaxCn += 16;	// 最大值更正
		}
		WalletDownPtr p = &GWalletDownLists[GWalletDownCount]; ++GWalletDownCount;	// 指向当前位置
		CharCopyZero(p->BCI16, 16 + 1, bci);
		CharCopyZero(p->Wallet48, 48 + 1, wallet);
		p->Block = bk? bk: 1;
	}
}
bool HttpConnDownWalletSupr()
{
	if (GWalletDownCount == 0) return false;
	int i; sockaddr_in ConnIP;
	for (i=0; i<GHttpConnCount; ++i)
	{
		if ((GHttpConnOrder[i] != HTTP_CONN_ORDER_IDEL) && (GHttpConnOrder[i] != HTTP_CONN_ORDER_OVER)) return false;	// 
	}
	--GWalletDownCount;
	WalletDownPtr p = &GWalletDownLists[GWalletDownCount];
	char Sha2[256]; DBWalletBlockSHA2_256(p->BCI16, p->Wallet48, p->Block, Sha2);
	for (i=0; i<GHttpConnCount; ++i)
	{
		ConnIP.sin_addr.s_addr = GHttpConnAddrs[i];
		GHttpConnSendCount[i] = (uint16)sprintf(GHttpConnSendRecv64K[i], "GET ?eType=%s&bci=%s&InWal=%s&sha=%s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%u-\r\n\r\n",
			(char*)(&HTTPID_GETWALLET), p->BCI16, p->Wallet48, Sha2, inet_ntoa(ConnIP.sin_addr), p->Block*DB_BLOCK_SIZE512B);
		GHttpConnState[i] = HTTP_CONN_STATE_INIT;
		GHttpConnOrder[i] = HTTP_CONN_ORDER_DOWN | HTTP_CONN_ORDER_SEND;
		GHttpConnSendCount[i] = 0; GHttpConnRecvCount[i] = 0;
		GHttpConnRetryTimes[i] = 0;
		SocketClose(GHttpConnSocket[i]);
	}
	return true;
}
void HttpConnPostFile2Super(const char fn[])
{
	for (int i=0; i<0xff; ++i)
	{
		strcpy(GHttpConnSendRecv64K[i], fn);
		GHttpConnState[i] = HTTP_CONN_STATE_INIT;
		GHttpConnOrder[i] = HTTP_CONN_ORDER_POST | HTTP_CONN_ORDER_SEND;
	}
}
byte HttpConnRecvSuperIsSuc()
{
	byte State = 0x00;
	for (int i=0; i<GHttpConnCount; ++i)
	{
		if (GHttpConnOrder[i] != HTTP_CONN_ORDER_OVER) return ERROR_FAIL;
		State |= GHttpConnState[i];
	}
	return (State&HTTP_CONN_STATE_FAIL)? ID_BLOCK_SUCCESS: ID_SUCCESS;
}
byte HttpConnRecvSuperIsSuc(char Body[])
{
	byte State = 0x00;
	for (int i=0; i<GHttpConnCount; ++i)
	{
		if (GHttpConnOrder[i] != HTTP_CONN_ORDER_OVER) return ERROR_FAIL;
		State |= GHttpConnState[i];
	}
	memcpy(Body, GHttpConnSendRecv64K[0], 1024);
	return (State&HTTP_CONN_STATE_SUCC)? ID_SUCCESS: ID_BLOCK_SUCCESS;
}
void HttpConnMainThread()
{
	uint8 Order, Erro;
	for (int Idx=0; Idx<GHttpConnCount; ++Idx)
	{
		Order = GHttpConnOrder[Idx];
		if (Order == HTTP_CONN_ORDER_IDEL) continue;
		if (Order & HTTP_CONN_ORDER_DODO)	// 执行命令
		{
			HttpConnDoDoThread(Idx);
		}else if (Order & HTTP_CONN_ORDER_DOWN) {	// 下载文件
			Erro = HttpConnDownThread(Idx);
			if (Erro == HTTP_CONN_THREAD_FAIL)	// 失败
			{
				continue;
			}else if (Erro == HTTP_CONN_THREAD_CONN) {	// 需要继续
				break;
			}else if (Erro == HTTP_CONN_THREAD_SUCC) {
				for (int i=0; i<=0xff; ++i)
				{
					SocketClose(GHttpConnSocket[i]);
					GHttpConnOrder[i] = HTTP_CONN_ORDER_IDEL;
				}
				break;
			}
		}else if (Order & HTTP_CONN_ORDER_POST) {	// 上传文件
			HttpConnPostThread(Idx);
		}
	}
}
void HttpConnInit()
{
	for (int i=0; i<=0xff; ++i)
	{
		GHttpConnSocket[i] = INVALID_SOCKET;
		GHttpConnOrder[i] = HTTP_CONN_ORDER_IDEL;
	//	GHttpConnState[i] = ;
	}
}
bool HttpConnAddSuper(const char Addr[], uint16 Port)
{
	GHttpConnAddrs[GHttpConnCount] = SocketAddress(Addr);
	GHttpConnPorts[GHttpConnCount] = SocketPort(Port);
	++GHttpConnCount;
	return true;
}

void HttpConnGetInfo(char wallet[48], char ip[32], uint8& user, uint8 idx)
{
	wallet[0] = 0;
//	CharCopyZero(wallet, 48, GIOCPHandle[idx].Wallet48);
	if (GHttpConnAddrs[idx] == 0) {ip[0] = 0;}
	else {sockaddr_in ConnIP; ConnIP.sin_addr.s_addr = GHttpConnAddrs[idx]; strcpy(ip, inet_ntoa(ConnIP.sin_addr));}//, ntohs(Port));
//	user = GIOCPHandle[idx].UserType;
}

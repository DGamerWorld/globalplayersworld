#include "global.h"

#define BUFFER_HTTP_CONN_64K				(0xffff+1)		// 0xffff + 1
SOCKET	GHttpConnSocket[0xff+1] = {INVALID_SOCKET};			// socket
uint32	GHttpConnAddrs[0xff+1];								// IP
uint16	GHttpConnPorts[0xff+1];								// ��ַ
#define HTTP_CONN_ORDER_IDEL				0x00			// ����
#define HTTP_CONN_ORDER_DODO				0x10			// ִ������
#define HTTP_CONN_ORDER_DOWN				0x20			// �����ļ�
#define HTTP_CONN_ORDER_POST				0x40			// �ϴ��ļ�
#define HTTP_CONN_ORDER_SEND				0x01			// ������Ϣ
#define HTTP_CONN_ORDER_RECV				0x02			// ������Ϣ
#define HTTP_CONN_ORDER_OVER				0x80			// ���
uint8	GHttpConnOrder[0xff+1];								// ����
#define HTTP_CONN_STATE_INIT				0x00			// ��ʼ��
#define HTTP_CONN_STATE_SEND				0x01			// ���ʹ���
#define HTTP_CONN_STATE_RECV				0x02			// ���ܴ���
#define HTTP_CONN_STATE_RVKJ				0x04			// ���ܿռ䲻��
#define HTTP_CONN_STATE_FILE				0x08			// ������ļ���ʧ��
#define HTTP_CONN_STATE_CONN				0x10			// ���ӷ�����
#define HTTP_CONN_STATE_FAIL				0x40			// ʧ��
#define HTTP_CONN_STATE_SUCC				0x80			// �ɹ�
uint8	GHttpConnState[0xff+1];								// ״̬
uint8	GHttpConnCount = 0;									// 
#define HttpConnSetOver(idx, sta)	{GHttpConnOrder[idx] = HTTP_CONN_ORDER_OVER; GHttpConnState[idx] = sta;}
#define HTTP_CONN_THREAD_FAIL				0x00			// ʧ��
#define HTTP_CONN_THREAD_CONN				0x01			// ��Ҫ����
#define HTTP_CONN_THREAD_SUCC				0xff			// �ɹ�

char	GHttpConnSendRecv64K[256][BUFFER_HTTP_CONN_64K] = {{0}};
uint32	GHttpConnSendCount[256] = {0}, GHttpConnRecvCount[256] = {0};
uint32	GHttpConnRetryTimes[256];
void HttpConnDoDoThread(uint8 Idx)
{
	if (GHttpConnSocket[Idx] == INVALID_SOCKET)	// ��û���ӷ�����
	{
		GHttpConnSocket[Idx] = SocketConnect(GHttpConnAddrs[Idx], GHttpConnPorts[Idx]);	// ���ӳ����ڵ��
		if (GHttpConnSocket[Idx] == INVALID_SOCKET)	// ����ʧ��
		{
			if (++GHttpConnRetryTimes[Idx] > 40*30/*30��*/) {HttpConnSetOver(Idx, HTTP_CONN_STATE_CONN);}	// ��30�뻹ʧ������Ϊ��Ч������
			return;
		}
		GHttpConnRetryTimes[Idx] = 0;	// ���ӳɹ������Դ�����0
	}
	//+send
	if (GHttpConnOrder[Idx] & HTTP_CONN_ORDER_SEND)	// �������ݽ׶�
	{
		char *http = GHttpConnSendRecv64K[Idx]; int hlen = strlen(http);
		int snln = send(GHttpConnSocket[Idx], &http[GHttpConnSendCount[Idx]], hlen - GHttpConnSendCount[Idx], 0);
		if (snln > 0)	// ����������
		{
			GHttpConnSendCount[Idx] += snln;
			if (GHttpConnSendCount[Idx] >= hlen)	// �������
			{
				GHttpConnOrder[Idx] = HTTP_CONN_ORDER_DODO | HTTP_CONN_ORDER_RECV;	// ���Ϊ�������ݽ׶�
				GHttpConnRetryTimes[Idx] = 0;
			}else {
				return;	// û������͵��´η�
			}
		}else if (
			(snln == 0) ||	// �Ͽ�
			((snln == SOCKET_ERROR) && SocketIsError()) ||	// ����
			(++GHttpConnRetryTimes[Idx] > 40*30/*30��*/)	// ��30��
			)
		{
			SocketClose(GHttpConnSocket[Idx]);
			HttpConnSetOver(Idx, HTTP_CONN_STATE_SEND);
			return;
		}else {
			return;	// �����д�����´η�
		}
	}
	//-send
	//+recv
	if (GHttpConnOrder[Idx] & HTTP_CONN_ORDER_RECV)	// �������ݽ׶�
	{
		int State = 0, Leng = 0, FBgn = 0, FEnd = 0; bool Head = true; char *http = GHttpConnSendRecv64K[Idx];
		int rcln = recv(GHttpConnSocket[Idx], &http[GHttpConnRecvCount[Idx]], BUFFER_HTTP_CONN_64K - GHttpConnRecvCount[Idx], 0);
		if (rcln > 0)
		{
			GHttpConnRecvCount[Idx] += rcln;	// �ܳ���
			if (Head == true)
			{
				int ret = HttpGetHead(State, Leng, FBgn, FEnd, http, GHttpConnRecvCount[Idx]);
				if ((ret > 0) && (ret <= GHttpConnRecvCount[Idx]))	{rcln = ret; Head = false;}
				else												{rcln = GHttpConnRecvCount[Idx];}
			}else {
				rcln = 0;
			}
			if (GHttpConnRecvCount[Idx] >= BUFFER_HTTP_CONN_64K)	// ���ܻ����С������Ϊ64K
			{
				SocketClose(GHttpConnSocket[Idx]);
				HttpConnSetOver(Idx, HTTP_CONN_STATE_RECV);
			}else if (GHttpConnRecvCount[Idx] - rcln >= Leng) {	// ��Ϣ����
				memcpy(http, &http[rcln], Leng); http[Leng] = 0;
				SocketClose(GHttpConnSocket[Idx]);
				HttpConnSetOver(Idx, (State==200)? HTTP_CONN_STATE_SUCC: HTTP_CONN_STATE_FAIL);
			}else {
				return;	// û������͵��´���
			}
		}else if (
			(rcln == 0) ||	// �Ͽ�
			((rcln == SOCKET_ERROR) && SocketIsError()) ||	// ����
			(++GHttpConnRetryTimes[Idx] > 40*60/*60��*/)	// ��60��
			)
		{
			SocketClose(GHttpConnSocket[Idx]);
			HttpConnSetOver(Idx, HTTP_CONN_STATE_RECV);
		}else {
			return;	// �����д�����´���
		}
	}
	//-recv
}
char	GHttpDownWallet4K[256][4096] = {{0}};
uint8	GHttpDownWalLoad = 0, GHttpDownWalSave = 0;
uint8 HttpConnDownThread(uint8 Idx)
{
	if (GHttpConnSocket[Idx] == INVALID_SOCKET)	// ��û���ӷ�����
	{
		GHttpConnSocket[Idx] = SocketConnect(GHttpConnAddrs[Idx], GHttpConnPorts[Idx]);	// ���ӳ����ڵ��
		if (GHttpConnSocket[Idx] == INVALID_SOCKET)	// ����ʧ��
		{
			if (++GHttpConnRetryTimes[Idx] > 40*30/*30��*/) {HttpConnSetOver(Idx, HTTP_CONN_STATE_CONN);}	// ��30�뻹ʧ������Ϊ��Ч������
			return HTTP_CONN_THREAD_FAIL;	// ʧ��
		}
		GHttpConnRetryTimes[Idx] = 0;	// ���ӳɹ������Դ�����0
	}
	//+send
	if (GHttpConnOrder[Idx] & HTTP_CONN_ORDER_SEND)	// �������ݽ׶�
	{
		char *http = GHttpConnSendRecv64K[Idx]; int hlen = strlen(http);
		int snln = send(GHttpConnSocket[Idx], &http[GHttpConnSendCount[Idx]], hlen - GHttpConnSendCount[Idx], 0);
		if (snln > 0)	// ����������
		{
			GHttpConnSendCount[Idx] += snln;
			if (GHttpConnSendCount[Idx] >= hlen)	// �������
			{
				GHttpConnOrder[Idx] = HTTP_CONN_ORDER_DOWN | HTTP_CONN_ORDER_RECV;	// ���Ϊ�������ݽ׶�
				GHttpConnRetryTimes[Idx] = 0;
			}else {
				return HTTP_CONN_THREAD_CONN;	// û������͵��´η�
			}
		}else if (
			(snln == 0) ||	// �Ͽ�
			((snln == SOCKET_ERROR) && SocketIsError()) ||	// ����
			(++GHttpConnRetryTimes[Idx] > 40*30/*30��*/)	// ��30��
			)
		{
			SocketClose(GHttpConnSocket[Idx]);
			HttpConnSetOver(Idx, HTTP_CONN_STATE_SEND);
			return HTTP_CONN_THREAD_FAIL;
		}else {
			return HTTP_CONN_THREAD_CONN;	// �����д�����´η�
		}
	}
	//-send
	//+recv
	if (GHttpConnOrder[Idx] & HTTP_CONN_ORDER_RECV)	// �������ݽ׶�
	{
		int State = 0, Leng = 0, FBgn = 0, FEnd = 0; char *http = GHttpConnSendRecv64K[Idx];
		int rcln = recv(GHttpConnSocket[Idx], &http[GHttpConnRecvCount[Idx]], BUFFER_HTTP_CONN_64K - GHttpConnRecvCount[Idx], 0);
		if (rcln > 0)
		{
			GHttpConnRecvCount[Idx] += rcln;	// �ܳ���
			int ret = HttpGetHead(State, Leng, FBgn, FEnd, http, GHttpConnRecvCount[Idx]);
			if ((ret > 0) && (ret <= GHttpConnRecvCount[Idx]))	{rcln = ret;}
			else												{rcln = GHttpConnRecvCount[Idx];}
			if (GHttpConnRecvCount[Idx] >= BUFFER_HTTP_CONN_64K)	// ���ܻ����С������Ϊ64K
			{
				SocketClose(GHttpConnSocket[Idx]);
				HttpConnSetOver(Idx, HTTP_CONN_STATE_RECV);
				return HTTP_CONN_THREAD_FAIL;
			}else if (GHttpConnRecvCount[Idx] - rcln >= Leng) {	// ��Ϣ����
				JsonHttpAnalysisMessage(&http[rcln], Leng);
				char bci[256]; JsonHttpGetChar(bci, 255, "bci");
				char wal[256]; JsonHttpGetChar(wal, 255, "wallet");
				uint32 block = JsonHttpGetUInt("block");
				DBWalletBlockSetByHttp(bci, wal, block);
				HttpConnDownWallet(bci, wal);
				SocketClose(GHttpConnSocket[Idx]);
				return HTTP_CONN_THREAD_SUCC;
			}else {
				return HTTP_CONN_THREAD_CONN;	// û������͵��´���
			}
		}else if (
			(rcln == 0) ||	// �Ͽ�
			((rcln == SOCKET_ERROR) && SocketIsError()) ||	// ����
			(++GHttpConnRetryTimes[Idx] > 40*60/*60��*/)	// ��60��
			)
		{
			SocketClose(GHttpConnSocket[Idx]);
			HttpConnSetOver(Idx, HTTP_CONN_STATE_RECV);
			return HTTP_CONN_THREAD_FAIL;
		}else {
			return HTTP_CONN_THREAD_CONN;	// �����д�����´���
		}
	}
	//-recv
	return HTTP_CONN_THREAD_FAIL;
}
void HttpConnPostThread(uint8 Idx)
{
	SocketClose(GHttpConnSocket[Idx]);
	GHttpConnSocket[Idx] = SocketConnect(GHttpConnAddrs[Idx], GHttpConnPorts[Idx]);				// ���ӵ������ڵ��
	if (GHttpConnSocket[Idx] == INVALID_SOCKET) {GHttpConnState[Idx] = HTTP_CONN_STATE_FAIL; return;}	// ����ʧ��
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
			if (snln < SLen) {SLen -= snln; memcpy(Send4KB, &Send4KB[snln], SLen);}	// û�����������ǰ��
			else {SLen = 0;}
		}else if (snln == SOCKET_ERROR) {
			if (SocketIsError()) {SocketClose(GHttpConnSocket[Idx]); GHttpConnState[Idx] = HTTP_CONN_STATE_SEND; break;}	// ����
		}
		if (SendPos >= SendCnt)	// �ļ��Ѷ���
		{
			if (SLen == 0) {GHttpConnState[Idx] = HTTP_CONN_STATE_SUCC; break;}	// ��������Ҳ������Ͼ��жϷ����������ʣ������
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
void HttpConnSendMsgToSuper(const char url[], uint32 msgType, const char Http[])	// ����Ϣ�������ڵ��
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
	uint32 Block;										// ������ʼ��
	char BCI8[8+4];
	char Wallet48[48+4];								// Ǯ��
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
		if (GWalletDownCount >= GWalletDownMaxCn)	// �ռ䲻����������
		{
			WalletDownPtr p = new CWALLETDOWN[GWalletDownMaxCn + 16];
			if (p == 0) return;	// �ڴ治��ֱ�ӷ���
			if (GWalletDownLists)
			{
				memcpy(p, GWalletDownLists, sizeof(CWALLETDOWN) * GWalletDownCount);	// ԭ���Ŀ��������ڴ���
				delete[] GWalletDownLists;	// �ͷ�ԭ�����ڴ�
			}
			GWalletDownLists = p;	// ָ�����ڴ�
			GWalletDownMaxCn += 16;	// ���ֵ����
		}
		WalletDownPtr p = &GWalletDownLists[GWalletDownCount]; ++GWalletDownCount;	// ָ��ǰλ��
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
		if (Order & HTTP_CONN_ORDER_DODO)	// ִ������
		{
			HttpConnDoDoThread(Idx);
		}else if (Order & HTTP_CONN_ORDER_DOWN) {	// �����ļ�
			Erro = HttpConnDownThread(Idx);
			if (Erro == HTTP_CONN_THREAD_FAIL)	// ʧ��
			{
				continue;
			}else if (Erro == HTTP_CONN_THREAD_CONN) {	// ��Ҫ����
				break;
			}else if (Erro == HTTP_CONN_THREAD_SUCC) {
				for (int i=0; i<=0xff; ++i)
				{
					SocketClose(GHttpConnSocket[i]);
					GHttpConnOrder[i] = HTTP_CONN_ORDER_IDEL;
				}
				break;
			}
		}else if (Order & HTTP_CONN_ORDER_POST) {	// �ϴ��ļ�
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

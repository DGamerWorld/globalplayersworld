#include "Global.h"

extern bool GSocketGameLoginOk;
extern char GJson64K[0xffff+1];
Texture2D GTextureChatTalk;
Texture2D GUIDownload;
uint8 GSuprSize = 0;

bool GMyApplicationRunFirst = true;
bool GChatTalking = false;

void GameAppInitOnce()
{
	if (GMyApplicationRunFirst == false) return;	// 为了Android每次切换都要Created设计的
	GMyApplicationRunFirst = false;

	uint32 Year, Month, Day, Hour, Minute, Second; char lg[256];
	DateGotoNowSecond(Year, Month, Day, Hour, Minute, Second);
	sprintf(lg, "\r\n  {GameAppInitOnce begin Year=%u,Month=%u,Day=%u,Hour=%u,Minute=%u}", Year, Month, Day, Hour, Minute);
	_log(lg);

	AnsicToUnicodeInit();	// 初始字库
	FileUtilsInit();		// 初始文件目录
	RAND_init();
	ECC_Start();
	SocketInit();
	UIInit(VIDEOMODE_COMMON_WIDE, VIDEOMODE_COMMON_HIGH);
	WalletRead();
	CDNInit();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#else
	FileUtilsCreateDirectory((char*)(&BCI_ADMIN_MAIN));
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

	//+message
	char c[2048], s[200]; int i; uint32 fileSize;
	for (i=0; i<=0xff; ++i)
	{
		GMessage63[i][0] = 0; GMessage63[i][63] = 0;
	}
	char *fileData = (char*)FileUtilsGetDataFileMain("message.xml", fileSize);
	if (fileData)
	{
		char* xml = SAFE_NEW_ARRAY(char, fileSize + 1);
		int xpos = 0, xlen = XmlGetBlockNodeByKey(xml, xpos, "message", fileData, fileSize);	// message
		int ypos = 0, ylen = XmlGetBlockNodeByKey(c, ypos, "msg", &xml[ypos], xlen - ypos);	// msg
		while (ylen >= 0)
		{
			uint8 id = XmlGetIntFromBodyByKey(c, ylen, "id", 0);
			XmlGetStringFromBodyByKey(s, c, ylen, "text", "");
			memcpy(GMessage63[id], s, 63);

			ylen = XmlGetBlockNodeByKey(c, ypos, "msg", &xml[ypos], xlen - ypos);	// msg
		}
		SAFE_DELETE_ARRAY(xml);
	}
	//-message

	GUITextureSystem.setTextureName("sys.png");
	_log("  {GameAppInitOnce end}\r\n");
}
void GameAppDeleteDeviceObjects()
{
	GTextureChatTalk.releaseGLTexture();
}
void GameApplicationRelease()
{
	WalletDownRelease();
	for (int i=0; i<GWalletCount; ++i)
	{
		SAFE_DELETE_ARRAY(GWalletLists[i].BCILists);
	}
	SAFE_DELETE_ARRAY(GWalletLists);
	SocketRTMPRelease();
}
void GameAppUpdateBySecond()	// 一秒通知一次
{
}
void GameAppUpdateByMillisecond(uint32 tmSubMs)	// 毫秒通知
{
	GUITextureSystem.recreateTexture(false);

//	GGameStateTime += tmSubMs;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)	// 检测xml是否正确
	char txt[256], tmp[200];
	strcpy(txt, "XMBC - ");// UnicodeToAnsic(tmp, 200, GBaseMyPlyinfo->uniName8); strcat(txt, tmp);
//	swprintf(wTxt, L"千年·不灭 [%s][%u] - %s", GBaseServerName, GBaseGameGroupID, GBaseMyPlyinfo->ChnName);
extern HWND GMainHwnd;
	SetWindowTextA(GMainHwnd, txt);	// 这种微软的窗口操作不能放在线程里面,否则要引起假死.还是那句话,微软的东西很多都是垃圾
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
}
void GameAppRender(uint32 tmSubMs)
{
}
void GameAppMoveEnd(uint32 tmSubMs)
{
	switch (GGameStatus)
	{
	case EGAMESTATUS::eGSGameRun:
		{
		}
		break;
	case EGAMESTATUS::eGSGameInit:
		{
			GGameStatus = EGAMESTATUS::eGSGameRun;

			for (int i=0; i<GWalletLists[0].BCICount; ++i)
			{
				if (GWalletLists[i].WalPub63[0] == 0) continue;
				WalletDownAdd(GWalletLists[0].BCILists[i], GWalletLists[0].Wallet48, 1, 0);
			}
			strcpy(GIpIpNode, "202.69.69.180"); GPortNode = 443;
		//	strcpy(GIpIpNode, "192.168.0.82"); GPortNode = 1935;
			SocketRTMPCreate(GIpIpNode, GPortNode);
		//	SocketGameCreate(GIpIpNode, GPortNode);

			UICloseAll();
			UIShowWinID(WINDOW_MAIN);
			WinUpdateMain(true);
		}
		break;
	case EGAMESTATUS::eGSUIInit:
		{
			GUIDownload.releaseGLTexture();
			GTextureChatTalk.setTextureName("ui/chattalk.png");
			if (UILoadByXml("ui/ui.xml") == false) MessageBoxA(0, "Open ui/ui.xml fail.", "Info", MB_OK);
			UIWindowUpdateByAlign(VIDEOMODE_COMMON_WIDE, VIDEOMODE_COMMON_HIGH);

			if (GWalletCount > 0)
			{
				GGameStatus = EGAMESTATUS::eGSGameInit;
			}else {
				GGameStatus = EGAMESTATUS::eGSEmptyRun;
				UICloseAll();
				UIShowWinID(WINDOW_CREATE);
			}
		}
		break;
	case EGAMESTATUS::eGSCreateRun:
		if (GSocketGameLoginOk)
		{
			GGameStatus = EGAMESTATUS::eGSGameRun;
			CWALLET *p = &GWalletLists[GWalletCount - 1];
			int Jlen = sprintf(GJson64K, "{eType=%s,inwal=%s,pub=%s}", (char*)(&SVRID_WALCRE_MUTEX_LOCK), p->Wallet48, p->WalPub63);
			SocketSendMsgToConn(GJson64K, Jlen);
		}
		break;
	case EGAMESTATUS::eGSCreateInit:
		{
			GGameStatus = EGAMESTATUS::eGSCreateRun;
			if (GSocketGameLoginOk == false) SocketGameCreate(GIpIpNode, GPortNode);	// 还没登陆就要设定ip
		}
		break;
	case EGAMESTATUS::eGSEmptyRun:
		{
		}
		break;
	}
	//+钱包下载
	WalletDownMain();
	//-钱包下载
}
void GameAppMouseButtonLeftClick(uint8 id, float ptx, float pty)	// 应用程序响应鼠标按下
{
}
void GameUIRenderAfter(uint32 tmSubMs)
{
	//+语音
/*	if (GOpenGLMouseLeftButtonDown[0])	// 鼠标按下
	{
		if (GChatTalking)
		{
			GTextureChatTalk.drawTexture(VIDEOMODE_HALF_WIDE - GTextureChatTalk._texWide/2, VIDEOMODE_HALF_HIGH - GTextureChatTalk._texHigh/2);
		}
	}else if (GChatTalking) {	// 鼠标抬起在语音
		GChatTalking = false;
		RecorderStop();
		LPUIFlash pFsh = (LPUIFlash)UIGetControl(WINDOW_MAIN, MAIN_FSH_CHATTYPE);
		switch (pFsh->CurrFrame)
		{
		case 0: GMyTalkUpUpType[GMyBaseInfo.TalkID] = ISLAND_CHAT_FREE;  break;
		case 1: GMyTalkUpUpType[GMyBaseInfo.TalkID] = ISLAND_CHAT_WORLD; break;
		case 2: GMyTalkUpUpType[GMyBaseInfo.TalkID] = ISLAND_CHAT_TEAM;  break;
		case 3: GMyTalkUpUpType[GMyBaseInfo.TalkID] = ISLAND_CHAT_ALLY;  break;
		}
		++GMyBaseInfo.TalkID;
		SocketSendMsgToConn(ISLAND_CHAT_TALK, 0, 0);
	}*/
	//-语音
}

uint16 SocketGameGetLoginDataThread(char Json[], bool IsRec)	// 获取登录信息
{
	if ((GWalletCount > 0) && (GWalletLists[0].WalPub63[0]))	// 开始有钱包就用原来的登陆
	{
		return sprintf(Json, "{eType=%s,package=BLOCKCHAIN,version=%u,author=chunguiMoo,company=HXM,wallet=%s,pub=%s}", (char*)(&SYSID_SERVERLOGIN), BLOCKCHAIN_VERSION, GWalletLists[0].Wallet48, GWalletLists[0].WalPub63);
	}else if (GWalletCount > 0) {	// 如果一开始没钱包,这里创建了就用最新钱包登陆
		return sprintf(Json, "{eType=%s,package=BLOCKCHAIN,version=%u,author=chunguiMoo,company=HXM,wallet=%s,pub=%s}", (char*)(&SYSID_SERVERLOGIN), BLOCKCHAIN_VERSION, GWalletLists[0].Wallet48, GWalletLists[0].WalPub63);
	}else {
		return 0;
	}
}

void VideoComplete()
{
}

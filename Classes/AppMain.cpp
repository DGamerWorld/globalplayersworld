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
	if (GMyApplicationRunFirst == false) return;	// Ϊ��Androidÿ���л���ҪCreated��Ƶ�
	GMyApplicationRunFirst = false;

	uint32 Year, Month, Day, Hour, Minute, Second; char lg[256];
	DateGotoNowSecond(Year, Month, Day, Hour, Minute, Second);
	sprintf(lg, "\r\n  {GameAppInitOnce begin Year=%u,Month=%u,Day=%u,Hour=%u,Minute=%u}", Year, Month, Day, Hour, Minute);
	_log(lg);

	AnsicToUnicodeInit();	// ��ʼ�ֿ�
	FileUtilsInit();		// ��ʼ�ļ�Ŀ¼
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
void GameAppUpdateBySecond()	// һ��֪ͨһ��
{
}
void GameAppUpdateByMillisecond(uint32 tmSubMs)	// ����֪ͨ
{
	GUITextureSystem.recreateTexture(false);

//	GGameStateTime += tmSubMs;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)	// ���xml�Ƿ���ȷ
	char txt[256], tmp[200];
	strcpy(txt, "XMBC - ");// UnicodeToAnsic(tmp, 200, GBaseMyPlyinfo->uniName8); strcat(txt, tmp);
//	swprintf(wTxt, L"ǧ�ꡤ���� [%s][%u] - %s", GBaseServerName, GBaseGameGroupID, GBaseMyPlyinfo->ChnName);
extern HWND GMainHwnd;
	SetWindowTextA(GMainHwnd, txt);	// ����΢��Ĵ��ڲ������ܷ����߳�����,����Ҫ�������.�����Ǿ仰,΢��Ķ����ܶ඼������
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
			if (GSocketGameLoginOk == false) SocketGameCreate(GIpIpNode, GPortNode);	// ��û��½��Ҫ�趨ip
		}
		break;
	case EGAMESTATUS::eGSEmptyRun:
		{
		}
		break;
	}
	//+Ǯ������
	WalletDownMain();
	//-Ǯ������
}
void GameAppMouseButtonLeftClick(uint8 id, float ptx, float pty)	// Ӧ�ó�����Ӧ��갴��
{
}
void GameUIRenderAfter(uint32 tmSubMs)
{
	//+����
/*	if (GOpenGLMouseLeftButtonDown[0])	// ��갴��
	{
		if (GChatTalking)
		{
			GTextureChatTalk.drawTexture(VIDEOMODE_HALF_WIDE - GTextureChatTalk._texWide/2, VIDEOMODE_HALF_HIGH - GTextureChatTalk._texHigh/2);
		}
	}else if (GChatTalking) {	// ���̧��������
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
	//-����
}

uint16 SocketGameGetLoginDataThread(char Json[], bool IsRec)	// ��ȡ��¼��Ϣ
{
	if ((GWalletCount > 0) && (GWalletLists[0].WalPub63[0]))	// ��ʼ��Ǯ������ԭ���ĵ�½
	{
		return sprintf(Json, "{eType=%s,package=BLOCKCHAIN,version=%u,author=chunguiMoo,company=HXM,wallet=%s,pub=%s}", (char*)(&SYSID_SERVERLOGIN), BLOCKCHAIN_VERSION, GWalletLists[0].Wallet48, GWalletLists[0].WalPub63);
	}else if (GWalletCount > 0) {	// ���һ��ʼûǮ��,���ﴴ���˾�������Ǯ����½
		return sprintf(Json, "{eType=%s,package=BLOCKCHAIN,version=%u,author=chunguiMoo,company=HXM,wallet=%s,pub=%s}", (char*)(&SYSID_SERVERLOGIN), BLOCKCHAIN_VERSION, GWalletLists[0].Wallet48, GWalletLists[0].WalPub63);
	}else {
		return 0;
	}
}

void VideoComplete()
{
}

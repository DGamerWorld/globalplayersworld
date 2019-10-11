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
	if (GMyApplicationRunFirst == false) return;	
	GMyApplicationRunFirst = false;

	uint32 Year, Month, Day, Hour, Minute, Second; char lg[256];
	DateGotoNowSecond(Year, Month, Day, Hour, Minute, Second);
	sprintf(lg, "\r\n  {GameAppInitOnce begin Year=%u,Month=%u,Day=%u,Hour=%u,Minute=%u}", Year, Month, Day, Hour, Minute);
	_log(lg);

	AnsicToUnicodeInit();	
	FileUtilsInit();		
	RAND_init();
	ECC_Start();
	SocketInit();
	UIInit(VIDEOMODE_COMMON_WIDE, VIDEOMODE_COMMON_HIGH);
	WalletRead();
	CDNInit();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#else
	FileUtilsCreateDirectory((char*)(&BCI_ADMIN_MAIN));
#endif 

	
	char c[2048], s[200]; int i; uint32 fileSize;
	for (i=0; i<=0xff; ++i)
	{
		GMessage63[i][0] = 0; GMessage63[i][63] = 0;
	}
	char *fileData = (char*)FileUtilsGetDataFileMain("message.xml", fileSize);
	if (fileData)
	{
		char* xml = SAFE_NEW_ARRAY(char, fileSize + 1);
		int xpos = 0, xlen = XmlGetBlockNodeByKey(xml, xpos, "message", fileData, fileSize);	
		int ypos = 0, ylen = XmlGetBlockNodeByKey(c, ypos, "msg", &xml[ypos], xlen - ypos);	
		while (ylen >= 0)
		{
			uint8 id = XmlGetIntFromBodyByKey(c, ylen, "id", 0);
			XmlGetStringFromBodyByKey(s, c, ylen, "text", "");
			memcpy(GMessage63[id], s, 63);

			ylen = XmlGetBlockNodeByKey(c, ypos, "msg", &xml[ypos], xlen - ypos);	
		}
		SAFE_DELETE_ARRAY(xml);
	}
	

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
void GameAppUpdateBySecond()	
{
}
void GameAppUpdateByMillisecond(uint32 tmSubMs)	
{
	GUITextureSystem.recreateTexture(false);


#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)	
	char txt[256], tmp[200];
	strcpy(txt, "XMBC - ");

extern HWND GMainHwnd;
	SetWindowTextA(GMainHwnd, txt);	
#endif 
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
		
			SocketRTMPCreate(GIpIpNode, GPortNode);
		

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
			if (GSocketGameLoginOk == false) SocketGameCreate(GIpIpNode, GPortNode);	
		}
		break;
	case EGAMESTATUS::eGSEmptyRun:
		{
		}
		break;
	}
	
	WalletDownMain();
	
}
void GameAppMouseButtonLeftClick(uint8 id, float ptx, float pty)	
{
}
void GameUIRenderAfter(uint32 tmSubMs)
{
	
}

uint16 SocketGameGetLoginDataThread(char Json[], bool IsRec)	
{
	if ((GWalletCount > 0) && (GWalletLists[0].WalPub63[0]))	
	{
		return sprintf(Json, "{eType=%s,package=BLOCKCHAIN,version=%u,author=chunguiMoo,company=HXM,wallet=%s,pub=%s}", (char*)(&SYSID_SERVERLOGIN), BLOCKCHAIN_VERSION, GWalletLists[0].Wallet48, GWalletLists[0].WalPub63);
	}else if (GWalletCount > 0) {	
		return sprintf(Json, "{eType=%s,package=BLOCKCHAIN,version=%u,author=chunguiMoo,company=HXM,wallet=%s,pub=%s}", (char*)(&SYSID_SERVERLOGIN), BLOCKCHAIN_VERSION, GWalletLists[0].Wallet48, GWalletLists[0].WalPub63);
	}else {
		return 0;
	}
}

void VideoComplete()
{
}

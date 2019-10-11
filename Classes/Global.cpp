#include "Global.h"

//+
uint64	GMyBCI = 0;

uint64	GMyToValu = 0;										// 
char	GMyToWaltChar[256] = {0};							// 
uint8	GMyToWaltByte[256] = {0};							// 
char	GMyThridWallet[256] = {0};
uint32	GMyThridID = 0;

CWALLET*	GWalletLists = new CWALLET[1];
uint32		GWalletCount = 0;
//-
//+IP
char   GIpIpNode[256] = {0};
char   GIpIpHttp[256] = {0};
uint16 GPortNode = 0, GPortHttp = 0;
//-IP
EGAMESTATUS::EGameStatus GGameStatus = EGAMESTATUS::eGSUIInit;
//+message
char GMessage63[256][63+1];
//-message

void TipsGoodsRenderCall(uint16 GdsID, short GdsX, short GdsY)
{
}
void TipsSkillRenderCall(uint16 SkiID, short SkiX, short SkiY)
{
}

//+
bool WalletRead()
{
	uint32 fileSize;
	char* fileData = (char*)FileUtilsGetDataFileMain("BlockChain.conf", fileSize);
	if (fileData == 0) return false;
	// \r\n=>\n
	fileSize = CharRNToN(fileData, fileSize);
	int bgn = 0, cur = 0; char outs[256];
	for ( ; cur<fileSize; ++cur)
	{
		if (fileData[cur] != '\n') continue;

		if (CharInKeys(outs, &fileData[bgn], cur-bgn, "node", 4))
		{
			GPortNode = BLOCKCHAIN_NODE_PORT;
			for (int j=0; j<256; ++j)
			{
				if (outs[j] == ':') {outs[j] = 0; GPortNode = atoi(&outs[j+1]); break;}
			}
			strcpy(GIpIpNode, outs);
		}else if (CharInKeys(outs, &fileData[bgn], cur-bgn, "thirdID", 7)) {
			GMyThridID = atoi(outs);
		}else if (CharInKeys(outs, &fileData[bgn], cur-bgn, "thirdWallet", 11)) {
			strcpy(GMyThridWallet, outs);
		}
		bgn = cur + 1;
	}

	GWalletCount = 0;
	fileData = (char*)FileUtilsGetDataFileMain("wallet.txt", fileSize);
	if (fileData)
	{
		char *xml = SAFE_NEW_ARRAY(char, fileSize + 1), c[1024], s[256]; uint8 b[256];
		int xpos = 0, xlen = XmlGetBlockNodeByKey(xml, xpos, "wallet", fileData, fileSize);	// wallet
		int ypos = 0, ylen = XmlGetBlockNodeByKey(c, ypos, "wal", &xml[ypos], xlen - ypos);	// wal
		while (ylen >= 0)
		{
		//	uint8 id = XmlGetIntFromBodyByKey(c, ylen, "id", 0);
			CWALLET *p = CWALLET::Add();
			p->WalType = XmlGetIntFromBodyByKey(c, ylen, "type", 0);
			XmlGetStringFromBodyByKey(s, c, ylen, "name", "");
			Base58Decode(b, 255, s);
			ByteCopy((uint8*)p->Name36, b, 36); p->Name36[36] = 0;
			XmlGetStringFromBodyByKey(p->Wallet48, c, ylen, "adr", "");
			XmlGetStringFromBodyByKey(p->WalSecSig, c, ylen, "sig", "");
			XmlGetStringFromBodyByKey(p->WalPub63, c, ylen, "pub", "");
			XmlGetStringFromBodyByKey(p->WalPlan, c, ylen, "pln", "");
			XmlGetStringFromBodyByKey(s, c, ylen, "key", "");
			Base58Decode(b, 255, s);
			for (int i=0; i<32; ++i) {p->WalSecPsw[i] = b[i];}

			p->BCICount = 0; p->BCILists = new uint64[1];
			int zpos = 0, zlen = XmlGetBlockNodeByKey(s, zpos, "bci", &c[zpos], ylen - zpos);	// bci
			while (zlen >= 0)
			{
				uint64 *n = p->BCIAdd(); char a[256];
				XmlGetStringFromBodyByKey(a, s, zlen, "name", "");
				*n = CharCharUInt64(a);

				zlen = XmlGetBlockNodeByKey(s, zpos, "bci", &c[zpos], ylen - zpos);	// bci
			}

			ylen = XmlGetBlockNodeByKey(c, ypos, "wal", &xml[ypos], xlen - ypos);	// wal
		}
		SAFE_DELETE_ARRAY(xml);
	}

	return true;
}
void WalletWrite()
{
	strcpy(GJson64K, "<wallet>\r\n"); int ps = strlen(GJson64K); CWALLET *p = GWalletLists; char c[256], s[256];
	for (int i=0; i<GWalletCount; ++i, ++p)
	{
		if (p->Created == true) continue;

		Base58Encode(s, p->WalSecPsw, 32);
		Base58Encode(c, (uint8*)p->Name36, 32);
		ps += sprintf(&GJson64K[ps], " <wal name=\"%s\" type=\"%u\" adr=\"%s\" key=\"%s\" pub=\"%s\" sig=\"%s\" pln=\"%s\">\r\n", c, p->WalType, p->Wallet48, s, p->WalPub63, p->WalSecSig, p->WalPlan);
		for (int j=0; j<p->BCICount; ++j)
		{
			ps += sprintf(&GJson64K[ps], "  <bci name=\"%s\"/>\r\n", (char*)(&p->BCILists[j]));
		}
		ps += sprintf(&GJson64K[ps], " </wal>\r\n");
	}
	strcat(GJson64K, "</wallet>");
	FileUtilsSaveTextToFile("wallet.txt", GJson64K);
}
//-

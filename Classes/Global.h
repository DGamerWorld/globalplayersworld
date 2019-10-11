#ifndef _CRAZYMO_GLOBAL_H_
#define _CRAZYMO_GLOBAL_H_

#include "../CrazyMo/cocos2d.h"
#include "../CrazyMo/global.h"
#include "UI.h"

#define COIN_ZERO_SIZE							1000000

//+
extern uint64	GMyBCI;

extern uint64	GMyToValu;									// 
extern char		GMyToWaltChar[256];							// 
extern uint8	GMyToWaltByte[256];							// 
extern char		GMyThridWallet[256];
extern uint32	GMyThridID;
bool WalletRead();
void WalletWrite();
//-
//+IP
extern char   GIpIpNode[256];
extern char   GIpIpHttp[256];
extern uint16 GPortNode, GPortHttp;
//-IP

class EGAMESTATUS
{
public:
enum EGameStatus
{
	eGSUIInit,

	eGSEmptyRun,

	eGSCreateInit,
	eGSCreateRun,

	eGSGameInit,
	eGSGameRun,
};
};
extern EGAMESTATUS::EGameStatus GGameStatus;

//+message
extern char GMessage63[256][63+1];
//-message
//+
void WalletDownRelease();
void WalletDownMain();
void WalletDownAdd(uint64 bci, const char wallet[48], uint32 bkBgn, uint32 bkEnd);
void WalletDownDel(uint64 bci, const char wallet[48]);
void WalletDownSetMaxBlock(uint64 bci, const char wallet[48], uint32 mxbk);
void WalletDownSetCurBlock(uint64 bci, const char wallet[48], uint32 cubk);
uint8 WalletDownGet(uint64 bci, const char wallet[48]);
//-
//+Key Store
void KeyStoreSet(UIListPtr lst, const uint8 SecretKey[32], bool Paste);
void KeyStoreGet(uint8 SecretKey[32], const char Keys[]);
bool KeyStoreOkk(const char Keys[48], const uint8 SecretKey[32]);
bool KeyStoreOkk(const char Keys[48]);
//-Key Store

class CWALLET;
extern CWALLET*	GWalletLists;
extern uint32	GWalletCount;
class CWALLET
{
public:
	CWALLET()
	{
		ByteZero(WalSecret, 32);
		Name36[0] = 0; Name36[36] = 0;
		Wallet48[0] = 0; Wallet48[48] = 0;
		ByteZero(WalSecPsw, 32);
		WalPub63[0] = 0; WalPub63[63] = 0;
		WalSecSig[0] = 0; WalSecSig[255] = 0;
		WalPlan[0] = 0; WalPlan[255] = 0;
		WalType = 0;
		Created = false;

		BCILists = 0;
		BCICount = 0;
	}
	inline static CWALLET* Add()
	{
		CWALLET *p = new CWALLET[GWalletCount + 2];
		if (p)
		{
			if (GWalletLists)
			{
				memcpy(p, GWalletLists, sizeof(CWALLET) * GWalletCount);
				delete[] GWalletLists;
			}
			GWalletLists = p;
			p = &GWalletLists[GWalletCount]; ++GWalletCount;
			return p;
		}
		return GWalletLists;
	}

	uint8	WalSecret[32];									// 
	char	Name36[36+4];									// 
	char	Wallet48[48+4];									// 
	uint8	WalSecPsw[32];									// 
	char	WalPub63[63+1];									// 
	char	WalSecSig[256];									// 
	char	WalPlan[256];									// 
	uint8	WalType;										//  A or B
	bool	Created;

	uint64*	BCILists;
	uint32	BCICount;
	inline uint64* BCIAdd()
	{
		uint64 *p = new uint64[BCICount + 2];
		if (p)
		{
			if (BCILists)
			{
				memcpy(p, BCILists, sizeof(uint64) * BCICount);
				delete[] BCILists;
			}
			BCILists = p;
			p = &BCILists[BCICount]; ++BCICount;
			return p;
		}
		return BCILists;
	}
};

#endif // _CRAZYMO_GLOBAL_H_

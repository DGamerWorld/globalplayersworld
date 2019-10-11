#ifndef _CRAZYMO_UI_H_
#define _CRAZYMO_UI_H_

#define PointInRect(ptx, pty, l, t, r, b)	((ptx>=l) && (ptx<=r) && (pty>=t) && (pty<=b))

#define CHAT_FONT_SIZE						12
#define MAX_SKILL_GDS_MAX					8


#define WINDOW_MAIN							0		
#define MAIN_CTR_BCI						0		
#define MAIN_LAB_NAME						2		
#define MAIN_LAB_WALLET						3		
#define MAIN_LAB_MONEY						4		
#define MAIN_BTN_ZICAN						5		
#define MAIN_BTN_ITSME						6		
#define MAIN_BTN_SEL						7		
#define MAIN_BTN_ADD						8		


#define BCI_BTN_SIGNED						0xee	
#define BCI_BTN_CANCEL						0xdd	
#define WINDOW_BCI							1		
#define BCI_BTN_RETURN						2		
#define BCI_LAB_BCI							3		
#define BCI_LAB_COIN						4		
#define BCI_LAB_MONEY						5		
#define BCI_CTR_VFY							6		
#define BCI_PIC_INN							7
#define BCI_PIC_TOO							8
#define BCI_PIC_MINE						9
#define BCI_BTN_SEND						10		
#define BCI_BTN_RECV						11		
#define BCI_BTN_MORE						12		
#define BCI_PIC_QUR							13
#define BCI_PIC_QKN							14
#define BCI_PIC_QOK							15

#define WINDOW_SEND							2
#define SEND_BTN_RETURN						0		
#define SEND_BTN_XXXX						1		
#define SEND_LAB_TITLE						2		
#define SEND_PRO_WALLNE						4		
#define SEND_EDT_WALLET						5
#define SEND_EDT_MONEY						7
#define SEND_EDT_PLAN						9
#define SEND_BTN_NEXT						11		
#define SEND_BTN_PASTE						12		
#define SEND_LAB_MSG						13

#define WINDOW_RECV							3

#define RECV_LAB_NAME						3		
#define RECV_LAB_WALLET						4		
#define RECV_BTN_COPY						5		

#define WINDOW_TRADE						4
#define TRADE_BTN_RETURN					3		
#define TRADE_LAB_MONEY						4		
#define TRADE_LAB_BCI						5		
#define TRADE_LAB_INWAL						8
#define TRADE_LAB_TOWAL						10
#define TRADE_LAB_PLAN						12
#define TRADE_LAB_TXID						14
#define TRADE_LAB_BLOCK						16
#define TRADE_LAB_TIME						18
#define TRADE_BTN_URL						20

#define WINDOW_ITSME						5
#define ITSME_BTN_WALLET					3		
#define ITSME_BTN_TRADE						4		
#define ITSME_BTN_ZICAN						17

#define WINDOW_SEL							10
#define SEL_LAB_NAME						3		
#define SEL_BTN_SAOSAO						4		
#define SEL_BTN_CREATE						6		
#define SEL_BTN_EXWALT						8		

#define WINDOW_CREATE						11		
#define CREATE_BTN_RETURN					4		
#define CREATE_EDT_WALLET					5		
#define CREATE_EDT_PSWD						6		
#define CREATE_EDT_PSDQR					7		
#define CREATE_EDT_PSDTS					8		
#define CREATE_CHK_PLAN						9		
#define CREATE_BTN_CREATE					10		
#define CREATE_BTN_DAORW					11		
#define CREATE_LAB_MSG						12

#define WINDOW_PSWD							12		
#define PSWD_PIC_PLAN						3		
#define PSWD_EDT_PSWD						4		
#define PSWD_BTN_OK							5		
#define PSWD_LAB_MSG						7		

#define WINDOW_BCIADD						13
#define BCIADD_BTN_RETURN					2		
#define BCIADD_BTN_SEARCH					3		
#define BCIADD_CTR_BCI						4		
#define BCIADD_CHK_SEL						5

#define WINDOW_DAORW						14		
#define DAORW_BTN_XXXX						4		
#define DAORW_BTN_REMOK						6		
#define DAORW_BTN_SECOK						7		
#define DAORW_BTN_REMNO						8		
#define DAORW_BTN_SECNO						9		
#define DAORW_EDT_SECA						10		
#define DAORW_EDT_SECB						11		
#define DAORW_BTN_PASTEA					12		
#define DAORW_BTN_PASTEB					13		
#define DAORW_EDT_WALLET					14		
#define DAORW_EDT_PSWD						16		
#define DAORW_EDT_PSW						18		
#define DAORW_EDT_PLAN						20		
#define DAORW_CHK_PLAN						22		
#define DAORW_CHK_ZUFU						23		
#define DAORW_BTN_START						24		
#define DAORW_BTN_REMXX						25		
#define DAORW_BTN_SECXX						26		
#define DAORW_LAB_MSG						27		
#define DAORW_LAB_KEYS						28		

#define WINDOW_GANLI						15		
#define GANLI_BTN_RETURN					2		
#define GANLI_CTR_WALLET					3
#define GANLI_BTN_CREATE					4
#define GANLI_BTN_DAORU						5
#define GANLI_PIC_HEAD						6		

#define WINDOW_EXWAL						16		
#define EXWAL_BTN_RETURN					2
#define EXWAL_CTR_WALLET					3
#define EXWAL_PIC_SELECT					4

#define WINDOW_TXINF						17		
#define TXINF_LAB_NAME						1
#define TXINF_BTN_RETURN					2
#define TXINF_BTN_MORE						3
#define TXINF_CTR_TRADE						4
#define TXINF_PIC_INN						5
#define TXINF_PIC_TOO						6
#define TXINF_PIC_ERR						7

#define WINDOW_MDPSW						18		
#define MDPSW_BTN_RETURN					2		
#define MDPSW_BTN_SAVE						3		
#define MDPSW_EDT_PSW_CUR					5
#define MDPSW_EDT_PSW_NEW					7
#define MDPSW_EDT_PSW_QRM					9
#define MDPSW_BTN_DAORU						12		
#define MDPSW_LAB_MSG						13

#define WINDOW_WALMX						19		
#define WALMX_BTN_RETURN					2
#define WALMX_BTN_SAVE						3
#define WALMX_BTN_PSW_XG					7
#define WALMX_BTN_SEC_DC					8
#define WALMX_BTN_KEY_DC					9
#define WALMX_BTN_KEY_SAV					10
#define WALMX_BTN_DEL_WAL					11
#define WALMX_LAB_NAME						12
#define WALMX_LAB_WALLET					13
#define WALMX_EDT_NAME						14
#define WALMX_LAB_PSW_TS					15

#define WINDOW_KEYSV						20
#define KEYSV_BTN_OK						1

#define WINDOW_SECDC						21
#define SECDC_BTN_COPY						2
#define SECDC_LAB_SECRET					3

#define WINDOW_WALOK						22
#define WALOK_BTN_SAVE						3		
#define WALOK_BTN_STUDY						4		

#define WINDOW_KEYZU						23		
#define KEYZU_BTN_NEXT						7		
#define KEYZU_LST_KEYZU						8		
#define KEYZU_LST_KEYFU						9		

#define WINDOW_KEYYZZ						24		
#define KEYS_BTN_OKOK						6		
#define KEYS_LAB_KEYS						7		
#define KEYS_LAB_MSG						8		
#define KEYS_BTN_A							9		
#define KEYS_BTN_Z							34		
#define KEYS_BTN_BACK						35		
#define KEYS_BTN_SPACE						36		
#define KEYS_BTN_RETURN						37		
#define KEYS_BTN_PASTE						38		
#define WINDOW_KEYYZF						25		
#define WINDOW_KEYSRZ						26		
#define WINDOW_KEYSRF						27		

#define WINDOW_BCI_CREATE					30
#define BCI_CREATE_LAB_BCI					1
#define BCI_CREATE_LAB_ICO					2
#define BCI_CREATE_FSH_ZFA					3
#define BCI_CREATE_BTN_OK					5

#define WINDOW_BCICOK						31


void WinUpdateMain(bool IsPro);
void WinUpdateBCI();
void WinUpdateBCIAdd(uint32 Indx);
void WinUpdateSend(const char txMsg[]);
void WinUpdateRecv();
void WinUpdateTrade();
void WinUpdateItsMe();
void WinUpdateGanLi();
void WinUpdateExWal();
void WinUpdateTxInf();
void WinUpdateWalMx(uint32 wal=MAX_DWORD_SIZE);
void WinUpdatePswSr(uint8 winID, uint32 wal);
void WinUpdateDaoRW(bool IsKeyStore);
void WinUpdateBCICreate(uint64 bci, const char wallet[], const char ico[], const char zfa[], const char plan[], const char lck[]);

void WinRun(uint8 winID, uint32 wal);

#endif 

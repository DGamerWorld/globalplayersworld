#ifndef _CRAZYMO_UI_H_
#define _CRAZYMO_UI_H_

#define PointInRect(ptx, pty, l, t, r, b)	((ptx>=l) && (ptx<=r) && (pty>=t) && (pty<=b))

#define CHAT_FONT_SIZE						12
#define MAX_SKILL_GDS_MAX					8

//+主窗口
#define WINDOW_MAIN							0		// 主窗口
#define MAIN_CTR_BCI						0		// 详细
#define MAIN_LAB_NAME						2		// 名称
#define MAIN_LAB_WALLET						3		// 钱包
#define MAIN_LAB_MONEY						4		// 资产
#define MAIN_BTN_ZICAN						5		// 资产
#define MAIN_BTN_ITSME						6		// 我的
#define MAIN_BTN_SEL						7		// 选择
#define MAIN_BTN_ADD						8		// 添加
//-主窗口

#define BCI_BTN_SIGNED						0xee	// 签名
#define BCI_BTN_CANCEL						0xdd	// 取消
#define WINDOW_BCI							1		// 
#define BCI_BTN_RETURN						2		// 
#define BCI_LAB_BCI							3		// 
#define BCI_LAB_COIN						4		// 
#define BCI_LAB_MONEY						5		// 
#define BCI_CTR_VFY							6		// 
#define BCI_PIC_INN							7
#define BCI_PIC_TOO							8
#define BCI_PIC_MINE						9
#define BCI_BTN_SEND						10		// 
#define BCI_BTN_RECV						11		// 
#define BCI_BTN_MORE						12		// 
#define BCI_PIC_QUR							13
#define BCI_PIC_QKN							14
#define BCI_PIC_QOK							15

#define WINDOW_SEND							2
#define SEND_BTN_RETURN						0		// 
#define SEND_BTN_XXXX						1		// 
#define SEND_LAB_TITLE						2		// 
#define SEND_PRO_WALLNE						4		// 收线
#define SEND_EDT_WALLET						5
#define SEND_EDT_MONEY						7
#define SEND_EDT_PLAN						9
#define SEND_BTN_NEXT						11		// 
#define SEND_BTN_PASTE						12		// 
#define SEND_LAB_MSG						13

#define WINDOW_RECV							3
//#define RECV_BTN_RETURN						2		// 
#define RECV_LAB_NAME						3		// 名称
#define RECV_LAB_WALLET						4		// 钱包
#define RECV_BTN_COPY						5		// 复制

#define WINDOW_TRADE						4
#define TRADE_BTN_RETURN					3		// 返回
#define TRADE_LAB_MONEY						4		// 
#define TRADE_LAB_BCI						5		// 
#define TRADE_LAB_INWAL						8
#define TRADE_LAB_TOWAL						10
#define TRADE_LAB_PLAN						12
#define TRADE_LAB_TXID						14
#define TRADE_LAB_BLOCK						16
#define TRADE_LAB_TIME						18
#define TRADE_BTN_URL						20

#define WINDOW_ITSME						5
#define ITSME_BTN_WALLET					3		// 管理钱包
#define ITSME_BTN_TRADE						4		// 交易记录
#define ITSME_BTN_ZICAN						17

#define WINDOW_SEL							10
#define SEL_LAB_NAME						3		// 名称
#define SEL_BTN_SAOSAO						4		// 扫扫
#define SEL_BTN_CREATE						6		// 创建
#define SEL_BTN_EXWALT						8		// 切换

#define WINDOW_CREATE						11		// 创建
#define CREATE_BTN_RETURN					4		// 返回
#define CREATE_EDT_WALLET					5		// 包名
#define CREATE_EDT_PSWD						6		// 密码
#define CREATE_EDT_PSDQR					7		// 重密
#define CREATE_EDT_PSDTS					8		// 密提
#define CREATE_CHK_PLAN						9		// 条款
#define CREATE_BTN_CREATE					10		// 创建
#define CREATE_BTN_DAORW					11		// 导入钱包
#define CREATE_LAB_MSG						12

#define WINDOW_PSWD							12		// 输密
#define PSWD_PIC_PLAN						3		// 提示
#define PSWD_EDT_PSWD						4		// 密码
#define PSWD_BTN_OK							5		// 确定
#define PSWD_LAB_MSG						7		// 

#define WINDOW_BCIADD						13
#define BCIADD_BTN_RETURN					2		// 关闭
#define BCIADD_BTN_SEARCH					3		// 搜索
#define BCIADD_CTR_BCI						4		// 
#define BCIADD_CHK_SEL						5

#define WINDOW_DAORW						14		// 导入钱包
#define DAORW_BTN_XXXX						4		// 搜索
#define DAORW_BTN_REMOK						6		// 助记
#define DAORW_BTN_SECOK						7		// 私钥
#define DAORW_BTN_REMNO						8		// 助记
#define DAORW_BTN_SECNO						9		// 私钥
#define DAORW_EDT_SECA						10		// 主私钥
#define DAORW_EDT_SECB						11		// 副私钥
#define DAORW_BTN_PASTEA					12		// 粘贴
#define DAORW_BTN_PASTEB					13		// 粘贴
#define DAORW_EDT_WALLET					14		// 包名
#define DAORW_EDT_PSWD						16		// 密码
#define DAORW_EDT_PSW						18		// 重密
#define DAORW_EDT_PLAN						20		// 密码提示
#define DAORW_CHK_PLAN						22		// 条款
#define DAORW_CHK_ZUFU						23		// 主副私钥
#define DAORW_BTN_START						24		// 开始
#define DAORW_BTN_REMXX						25		// 助记
#define DAORW_BTN_SECXX						26		// 私钥
#define DAORW_LAB_MSG						27		// 
#define DAORW_LAB_KEYS						28		// 

#define WINDOW_GANLI						15		// 管理钱包
#define GANLI_BTN_RETURN					2		// 返回
#define GANLI_CTR_WALLET					3
#define GANLI_BTN_CREATE					4
#define GANLI_BTN_DAORU						5
#define GANLI_PIC_HEAD						6		// 头框

#define WINDOW_EXWAL						16		// 切换帐号
#define EXWAL_BTN_RETURN					2
#define EXWAL_CTR_WALLET					3
#define EXWAL_PIC_SELECT					4

#define WINDOW_TXINF						17		// 交易明细
#define TXINF_LAB_NAME						1
#define TXINF_BTN_RETURN					2
#define TXINF_BTN_MORE						3
#define TXINF_CTR_TRADE						4
#define TXINF_PIC_INN						5
#define TXINF_PIC_TOO						6
#define TXINF_PIC_ERR						7

#define WINDOW_MDPSW						18		// 更改密码
#define MDPSW_BTN_RETURN					2		// 关闭
#define MDPSW_BTN_SAVE						3		// 完成
#define MDPSW_EDT_PSW_CUR					5
#define MDPSW_EDT_PSW_NEW					7
#define MDPSW_EDT_PSW_QRM					9
#define MDPSW_BTN_DAORU						12		// 导入
#define MDPSW_LAB_MSG						13

#define WINDOW_WALMX						19		// 钱包
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
#define WALOK_BTN_SAVE						3		// 备份
#define WALOK_BTN_STUDY						4		// 教程

#define WINDOW_KEYZU						23		// 助记
#define KEYZU_BTN_NEXT						7		// 下步
#define KEYZU_LST_KEYZU						8		// 助主
#define KEYZU_LST_KEYFU						9		// 助副

#define WINDOW_KEYYZZ						24		// 助验主
#define KEYS_BTN_OKOK						6		// 确认
#define KEYS_LAB_KEYS						7		// 单词
#define KEYS_LAB_MSG						8		// 信息
#define KEYS_BTN_A							9		// A
#define KEYS_BTN_Z							34		// Z
#define KEYS_BTN_BACK						35		// <-
#define KEYS_BTN_SPACE						36		// 空格
#define KEYS_BTN_RETURN						37		// 回车
#define KEYS_BTN_PASTE						38		// 粘贴
#define WINDOW_KEYYZF						25		// 助验副
#define WINDOW_KEYSRZ						26		// 助入主
#define WINDOW_KEYSRF						27		// 助入副

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

#endif // _CRAZYMO_UI_H_

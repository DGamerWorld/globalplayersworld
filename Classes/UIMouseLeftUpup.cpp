#include "Global.h"

// dwType dwValue 
// dwEvernd0 dwEvernd1 dwEvernd2 dwEvernd3	
void UIMouseButtonLeftUpupCall(uint8 winID, uint8 ctrID, uint32 mainType, uint32 mainValue,
							   uint32 cellType, uint32 cellValue, const uint32 dwExtend8[8], uint32 selIdx)	// 
{
	switch (winID)
	{
	case WINDOW_MAIN:
		if (ctrID == MAIN_CTR_BCI)
		{
			if (selIdx >= GWalletLists[0].BCICount) break;
			GMyBCI = GWalletLists[0].BCILists[selIdx];
			UICloseAll();
			UIShowWinID(WINDOW_BCI);
			WinUpdateBCI();
		}else if (ctrID == MAIN_BTN_SEL) {	// 
			UISlideShowWinID(WINDOW_SEL, -366, 0, 0xff);
			UIWindowPtr wnRecv = UIGetWindow(WINDOW_SEL);
			UILabelPtr lab = wnRecv->GetLabel(SEL_LAB_NAME);
			lab->SetTexts(GWalletLists[0].Name36);
		}else if (ctrID == MAIN_BTN_ADD) {	// 
			int Jlen = sprintf(GJson64K, "{eType=%s,idx=%u}", (char*)(&SVRID_BCI_GET), 0);
			SocketSendMsgToConn(GJson64K, Jlen);
			UICloseAll();
			UIShowWinID(WINDOW_BCIADD);
			WinUpdateBCIAdd(0);
		}else if (ctrID == MAIN_BTN_ITSME) {	// 
			UICloseAll();
			UIShowWinID(WINDOW_ITSME);
			WinUpdateItsMe();
		}else if (ctrID == MAIN_LAB_WALLET) {
		//	UICloseAll();
			UIShowWinID(WINDOW_RECV);
			WinUpdateRecv();
		}
		break;
	case WINDOW_BCI:
		if (ctrID == BCI_BTN_RETURN)
		{
			UICloseAll();
			UIShowWinID(WINDOW_MAIN);
			WinUpdateMain(false);
		}else if (ctrID == BCI_BTN_SEND) {
			UICloseAll();
			UIShowWinID(WINDOW_SEND);
			WinUpdateSend("");
		}else if (ctrID == BCI_BTN_RECV) {
		//	UICloseAll();
			UIShowWinID(WINDOW_RECV);
			WinUpdateRecv();
		}else if (ctrID == BCI_BTN_SIGNED) {	// 
			if (cellType)
			{
				if (GWalletLists[0].WalType != 0) {MessageBoxA(0, GMessage63[3], "Info", MB_OK); break;}
				if (ByteIsZero(GWalletLists[0].WalSecret, 32) == true) {WinUpdatePswSr(WINDOW_BCI, 0); break;}
			//	WinUpdateBCIMsg("...");
				uint8 wal[256]; WalletToByte(wal, GWalletLists[0].Wallet48);
				uint8 Erro = DBWalletSignTooCreated(GMyBCI, wal, cellValue, GWalletLists[0].WalSecret);
				if (Erro == ID_SUCCESS)	// 
				{
				}else {
					MessageBoxA(0, GMessage63[Erro], "Info", MB_OK);
				}
			}else {
				if (GWalletLists[0].WalType != 1) {MessageBoxA(0, GMessage63[4], "Info", MB_OK); break;}
				if (ByteIsZero(GWalletLists[0].WalSecret, 32) == true) {WinUpdatePswSr(WINDOW_BCI, 0); break;}
			//	WinUpdateBCIMsg("...");
				uint8 wal[256]; WalletToByte(wal, GWalletLists[0].Wallet48);
				uint8 Erro = DBWalletSignInnCreated(GMyBCI, wal, cellValue, GWalletLists[0].WalSecret);
				if (Erro == ID_SUCCESS)	// 
				{
				}else {
					MessageBoxA(0, GMessage63[Erro], "Info", MB_OK);
				}
			}
		}else if (ctrID == BCI_BTN_CANCEL) {	// 
		//	GGameState.Trade = EGAMETRADE::eGTCancellMutexInit; GGameState.Time = 0;// WinUpdateBCIMsg("...");
		}
		break;
	case WINDOW_SEND:
		if (ctrID == SEND_BTN_RETURN)
		{
			UICloseAll();
			UIShowWinID(WINDOW_BCI);
			WinUpdateBCI();
		}else if (ctrID == SEND_BTN_NEXT) {
			UIWindowPtr wnSend = UIGetWindow(WINDOW_SEND);
			UIEditPtr jbs = wnSend->GetEditBox(SEND_EDT_MONEY);
			UIEditPtr wal = wnSend->GetEditBox(SEND_EDT_WALLET);
			wal->GetTexts(GMyToWaltChar, 255);
			WalletToByte(GMyToWaltByte, GMyToWaltChar);
			char toVal[256]; jbs->GetTexts(toVal, 255);
			uint32 e; GMyToValu = CharToUInt64(toVal, e);
			GMyToValu *= COIN_ZERO_SIZE; GMyToValu += e;
			if (GMyToWaltChar[0] == 0)
			{
				WinUpdateSend("");
				uint8 utf8[256]; PasteBoardGet(utf8, 255);
				if ((utf8[0] != 0) && (Base58Verify(utf8, 255) == true))
				{
					UIBtnPtr btn = wnSend->GetButton(SEND_BTN_PASTE);
					btn->Visibled = true;
				}
				UISetEditFocusIn(WINDOW_SEND, SEND_EDT_WALLET);
				break;
			}
			if (GMyToValu == 0)
			{
				WinUpdateSend("");
				UISetEditFocusIn(WINDOW_SEND, SEND_EDT_MONEY);
				break;
			}
			if (ByteIsZero(GWalletLists[0].WalSecret, 32) == true)
			{
				WinUpdatePswSr(WINDOW_SEND, 0);
				break;
			}

			WinUpdateSend("...");
			uint8 inw[256]; WalletToByte(inw, GWalletLists[0].Wallet48);
			uint8 Erro = DBWalletTradingCreated(GMyBCI, inw, GMyToWaltByte, GMyToValu, GWalletLists[0].WalSecret, GMyThridID, GMyThridWallet);
			if (Erro == ID_SUCCESS)	// 
			{
			}else {
				WinUpdateSend(GMessage63[Erro]);
			}
		}else if (ctrID == SEND_BTN_PASTE) {
			UIWindowPtr wnSend = UIGetWindow(WINDOW_SEND);
			UIBtnPtr btn = wnSend->GetButton(SEND_BTN_PASTE);
			btn->Visibled = false;
			uint8 utf8[256]; PasteBoardGet(utf8, 255);
			Base58Verify(utf8, 255);
			UIEditPtr wal = wnSend->GetEditBox(SEND_EDT_WALLET);
			wal->SetTextAsUTF8(utf8);
			wal->SetCursorPosToEnd();
			wal->GetTexts(GMyToWaltChar, 255);
			WalletDownAdd(GMyBCI, GMyToWaltChar, 1, 0);
		}else if (ctrID == SEND_EDT_WALLET) {
			uint8 utf8[256]; PasteBoardGet(utf8, 255);
			if ((utf8[0] != 0) && (Base58Verify(utf8, 255) == true))
			{
				UIWindowPtr wnSend = UIGetWindow(WINDOW_SEND);
				UIBtnPtr btn = wnSend->GetButton(SEND_BTN_PASTE);
				btn->Visibled = true;
			}
		}
		break;
	case WINDOW_RECV:
		if (ctrID == RECV_BTN_COPY)
		{
			UIWindowPtr wnRecv = UIGetWindow(WINDOW_RECV);
			UILabelPtr lab = wnRecv->GetLabel(RECV_LAB_WALLET);
			uint16* unic = lab->GetText();
			uint8 utf8[256]; UnicodeToUTF8(utf8, 255, unic, 255);
			PasteBoardSet(utf8);
		}
		break;
	case WINDOW_ITSME:	// 
		if (ctrID == ITSME_BTN_ZICAN)
		{
			UICloseAll();
			UIShowWinID(WINDOW_MAIN);
			WinUpdateMain(false);
		}else if (ctrID == ITSME_BTN_WALLET) {	// 
			UICloseAll();
			UIShowWinID(WINDOW_GANLI);
			WinUpdateGanLi();
		}else if (ctrID == ITSME_BTN_TRADE) {	// 
			UICloseAll();
			UIShowWinID(WINDOW_TXINF);
			WinUpdateTxInf();
		}
		break;
	case WINDOW_TXINF:
		if (ctrID == TXINF_BTN_RETURN)
		{
			UICloseAll();
			UIShowWinID(WINDOW_ITSME);
			WinUpdateItsMe();
		}else if (ctrID == TXINF_BTN_MORE) {
		}
		break;
	case WINDOW_GANLI:	// 
		if (ctrID == GANLI_BTN_RETURN)	// 
		{
			UICloseAll();
			UIShowWinID(WINDOW_ITSME);
			WinUpdateItsMe();
		}else if (ctrID == GANLI_BTN_CREATE) {
			UICloseAll();
			UIShowWinID(WINDOW_CREATE);
		}else if (ctrID == GANLI_BTN_DAORU) {
		//	UICloseAll();
			WinUpdateDaoRW(true);
		}else if ((ctrID == GANLI_CTR_WALLET) && (selIdx < GWalletCount)) {
			UICloseAll();
			UIShowWinID(WINDOW_WALMX);
			WinUpdateWalMx(selIdx);
		}
		break;
	case WINDOW_EXWAL:	// 
		if ((ctrID == EXWAL_CTR_WALLET) && (selIdx < GWalletCount))
		{
			CWALLET wal = GWalletLists[0];
			GWalletLists[0] = GWalletLists[selIdx];
			GWalletLists[selIdx] = wal;
			WalletWrite();
			GGameStatus = EGAMESTATUS::eGSGameInit;
		}else if (ctrID == EXWAL_BTN_RETURN) {
			UICloseAll();
			UIShowWinID(WINDOW_MAIN);
			WinUpdateMain(false);
		}
		break;
	case WINDOW_WALMX:	// 
		if (ctrID == WALMX_BTN_RETURN)
		{
			UICloseAll();
			UIShowWinID(WINDOW_GANLI);
			WinUpdateGanLi();
		}else if (ctrID == WALMX_BTN_SAVE) {
		}else if (ctrID == WALMX_BTN_PSW_XG) {
			UICloseAll();
			UIShowWinID(WINDOW_MDPSW);
			UIWindowPtr wnWmx = UIGetWindow(WINDOW_WALMX);
			UIWindowPtr wnPsw = UIGetWindow(WINDOW_MDPSW);
			wnPsw->dwValue = wnWmx->dwValue;
		}else if (ctrID == WALMX_BTN_SEC_DC) {
			UIWindowPtr wnWmx = UIGetWindow(WINDOW_WALMX);
			CWALLET *p = &GWalletLists[wnWmx->dwValue];
			if (ByteIsZero(p->WalSecret, 32) == true)
			{
				WinUpdatePswSr(WINDOW_WALMX, wnWmx->dwValue);
				break;
			}
			WinRun(WINDOW_WALMX, wnWmx->dwValue);
		}else if (ctrID == WALMX_BTN_KEY_DC) {
		}else if (ctrID == WALMX_BTN_KEY_SAV) {
			UIShowWinID(WINDOW_KEYSV);
		}else if (ctrID == WALMX_BTN_DEL_WAL) {
			UIWindowPtr wnWmx = UIGetWindow(WINDOW_WALMX);
			GWalletLists[wnWmx->dwValue].Created = true;
			WalletWrite();
			UICloseAll();
			UIShowWinID(WINDOW_GANLI);
			WinUpdateGanLi();
		}
		break;
	case WINDOW_MDPSW:	// 
		if (ctrID == MDPSW_BTN_RETURN)	// 
		{
			UICloseAll();
			UIShowWinID(WINDOW_WALMX);
			WinUpdateWalMx();
		}else if (ctrID == MDPSW_BTN_SAVE) {	// 
			UIWindowPtr wnPsw = UIGetWindow(WINDOW_MDPSW);
			UIEditPtr edt = wnPsw->GetEditBox(MDPSW_EDT_PSW_CUR);	// 
			char old[256]; edt->GetTexts(old, 255);
#define PSWD_MSG(txMsg) \
{ \
	UILabelPtr lab = wnPsw->GetLabel(MDPSW_LAB_MSG); \
	lab->SetTexts(txMsg); \
	lab->Visibled = (txMsg[0])? true: false; \
	lab->dwValue = GTimeClientCurrMs; \
	lab->SetFontColor(0xff000000); \
}
			CWALLET *p = &GWalletLists[wnPsw->dwValue];
			uint8 sha[64], sec[256]; SHA3_256(sha, old, strlen(old)); int i;
			for (i=0; i<32; ++i)
			{
				sec[i] = sha[i] ^ p->WalSecPsw[i];
			}
			char pub[256]; uint8 pubkey[256]; secp256k1_ec_pubkey_create(pub, pubkey, sec);
			if (SignVerifyByPublicKey(p->WalSecSig, sha, pub) == false)
			{
				UISetEditFocusIn(WINDOW_MDPSW, MDPSW_EDT_PSW_CUR);
				PSWD_MSG("")
				break;
			}
			edt = wnPsw->GetEditBox(MDPSW_EDT_PSW_NEW);
			char pswd[256]; edt->GetTexts(pswd, 255);
			if ((pswd[0] == 0) || (strlen(pswd) < 6))
			{
				UISetEditFocusIn(WINDOW_MDPSW, MDPSW_EDT_PSW_NEW);
				PSWD_MSG("6")
				break;
			}
			edt = wnPsw->GetEditBox(MDPSW_EDT_PSW_QRM);
			char psw[256]; edt->GetTexts(psw, 255);
			if (CharEqual(psw, pswd) == false)
			{
				UISetEditFocusIn(WINDOW_MDPSW, MDPSW_EDT_PSW_QRM);
				PSWD_MSG("")
				break;
			}
			uint8 SHA[64]; SHA3_256(SHA, pswd, strlen(pswd));
			for (i=0; i<32; ++i)
			{
				p->WalSecret[i] = sha[i] ^ p->WalSecPsw[i];
				p->WalSecPsw[i] = SHA[i] ^ p->WalSecret[i];
			}
			secp256k1_ec_pubkey_create(p->WalPub63, pubkey, p->WalSecret);
			SignCreateBySecretKey(p->WalSecSig, SHA, p->WalSecret);
			WalletWrite();

			UICloseAll();
			UIShowWinID(WINDOW_WALMX);
			WinUpdateWalMx();
		}else if (ctrID == MDPSW_BTN_DAORU) {	// 
		}
		break;
	case WINDOW_TRADE:
		if (ctrID == TRADE_BTN_RETURN)
		{
			UICloseAll();
			UIShowWinID(WINDOW_BCI);
			WinUpdateBCI();
		}
		break;
	case WINDOW_SEL:
		if (ctrID == SEL_BTN_CREATE)	// 
		{
			UICloseAll();
			UIShowWinID(WINDOW_CREATE);
		}else if (ctrID == SEL_BTN_SAOSAO) {	// 
		}else if (ctrID == SEL_BTN_EXWALT) {	// 
			UICloseAll();
			UIShowWinID(WINDOW_EXWAL);
			WinUpdateExWal();
		}
		break;
	case WINDOW_BCIADD:
		if (ctrID == BCIADD_BTN_RETURN)	// 
		{
			GWalletLists[0].BCICount = 0; uint64 bci = 0;
			UIWindowPtr wnBci = UIGetWindow(WINDOW_BCIADD);
			UICtrlPtr ctl = wnBci->GetCtrlBox(BCIADD_CTR_BCI);
			for (int i=0; i<=0xff; ++i)
			{
				UICheckPtr chk = (UICheckPtr)ctl->GetCtrl(i, 2);
				if (chk == 0) break;
				if (chk->GetCheck() == false) continue;

				bci = BCIGetByIndex(chk->dwType);
				uint64 *p = GWalletLists[0].BCIAdd();
				if (p == 0) break;
				FileUtilsCreateDirectory((char*)(&bci));
				*p = bci;
				WalletDownAdd(bci, GWalletLists[0].Wallet48, 1, 0);
			}
			WalletWrite();

			UICloseAll();
			UIShowWinID(WINDOW_MAIN);
			WinUpdateMain(false);
		}else if (ctrID == BCIADD_BTN_SEARCH) {	// 
		}
		break;
	case WINDOW_CREATE:
		if (ctrID == CREATE_BTN_CREATE)	// 
		{
			UIWindowPtr wnCet = UIGetWindow(WINDOW_CREATE);
			UIEditPtr edt = wnCet->GetEditBox(CREATE_EDT_WALLET);	// 
			char name[256]; edt->GetTexts(name, 255);
			edt = wnCet->GetEditBox(CREATE_EDT_PSWD);
			char pswd[256]; edt->GetTexts(pswd, 255);
			edt = wnCet->GetEditBox(CREATE_EDT_PSDTS);
			char plan[256]; edt->GetTexts(plan, 255);
#define CREATE_MSG(txMsg) \
{ \
	UILabelPtr lab = wnCet->GetLabel(CREATE_LAB_MSG); \
	lab->SetTexts(txMsg); \
	lab->Visibled = (txMsg[0])? true: false; \
	lab->dwValue = GTimeClientCurrMs; \
	lab->SetFontColor(0xff000000); \
}
			if (name[0] == 0)
			{
				UISetEditFocusIn(WINDOW_CREATE, CREATE_EDT_WALLET);
				CREATE_MSG("")
				break;
			}
			if ((pswd[0] == 0) || (strlen(pswd) < 6))
			{
				UISetEditFocusIn(WINDOW_CREATE, CREATE_EDT_PSWD);
				CREATE_MSG("6")
				break;
			}
			edt = wnCet->GetEditBox(CREATE_EDT_PSDQR);
			char psw[256]; edt->GetTexts(psw, 255);
			if (CharEqual(psw, pswd) == false)
			{
				UISetEditFocusIn(WINDOW_CREATE, CREATE_EDT_PSDQR);
				CREATE_MSG("")
				break;
			}
			UICheckPtr chk = wnCet->GetCheckBox(CREATE_CHK_PLAN);
			if (chk->GetCheck() == false)
			{
				CREATE_MSG("")
				break;
			}

			uint8 sha[64]; SHA3_256(sha, pswd, strlen(pswd));
			CWALLET *a = CWALLET::Add();	// ,
			a->Created = true; a->WalType = 0;
			uint64Ptr p = a->BCIAdd();
			if (p) *p = BCI_ADMIN_MAIN;
			char wal[256], pub[256]; uint8 sec[256]; int i;
			MakeNewKey256(wal, a->WalPub63, pub, a->WalSecret, sec);
			CharCopy(a->Wallet48, 48+1, wal);
			CharCopy(a->Name36, 36, name);
			CharCopy(a->WalPlan, 255, plan);
			for (i=0; i<32; ++i)
			{
				a->WalSecPsw[i] = sha[i] ^ a->WalSecret[i];
			}
			SignCreateBySecretKey(a->WalSecSig, sha, a->WalSecret);
			CWALLET *b = CWALLET::Add();
			b->Created = true; b->WalType = 1;
			p = b->BCIAdd();
			if (p) *p = BCI_ADMIN_MAIN;
			ByteCopy(b->WalSecret, sec, 32);
			CharCopy(b->WalPub63, 63+1, pub);
			CharCopy(b->Wallet48, 48+1, wal);
			CharCopy(b->Name36, 36, name);
			CharCopy(b->WalPlan, 255, plan);
			for (i=0; i<32; ++i)
			{
				b->WalSecPsw[i] = sha[i] ^ b->WalSecret[i];
			}
			SignCreateBySecretKey(b->WalSecSig, sha, b->WalSecret);
			GGameStatus = EGAMESTATUS::eGSCreateInit;
		}else if (ctrID == CREATE_BTN_DAORW) {	// 
		//	UICloseAll();
			WinUpdateDaoRW(true);
		}else if (ctrID == CREATE_BTN_RETURN) {	// 
			UICloseAll();
			UIShowWinID(WINDOW_MAIN);
			WinUpdateMain(false);
		}
		break;
	case WINDOW_WALOK:
		if (ctrID == WALOK_BTN_SAVE)	// 
		{
		//	UICloseAll();
			UIShowWinID(WINDOW_KEYZU);
			UIWindowPtr wnWok = UIGetWindow(WINDOW_WALOK);
			UIWindowPtr wnKzu = UIGetWindow(WINDOW_KEYZU);
			wnKzu->dwValue = wnWok->dwValue;
			UIListPtr lst = wnKzu->GetListBox(KEYZU_LST_KEYZU);	// 
			KeyStoreSet(lst, GWalletLists[wnWok->dwValue].WalSecret, true);
			lst = wnKzu->GetListBox(KEYZU_LST_KEYFU);	// 
			KeyStoreSet(lst, GWalletLists[wnWok->dwValue+1].WalSecret, false);
		}else if (ctrID == WALOK_BTN_STUDY) {	// 
		}
		break;
	case WINDOW_KEYZU:	// 
		if (ctrID == KEYZU_BTN_NEXT)	// 
		{
		//	UICloseAll();
			UIShowWinID(WINDOW_KEYYZZ);
			UIWindowPtr wnKnx = UIGetWindow(WINDOW_KEYZU);
			UIWindowPtr wnKyz = UIGetWindow(WINDOW_KEYYZZ);
			wnKyz->dwValue = wnKnx->dwValue;
			UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_KEYS);
			lab->SetTexts("");
			UIWindowPtr wnKyf = UIGetWindow(WINDOW_KEYYZF);
			wnKyf->dwValue = wnKnx->dwValue+1;
		}
		break;
	case WINDOW_KEYYZF:
	case WINDOW_KEYYZZ:
	case WINDOW_KEYSRF:
	case WINDOW_KEYSRZ:
		if (ctrID == KEYS_BTN_OKOK)	// 
		{
			if (winID == WINDOW_KEYYZZ)
			{
				UIWindowPtr wnKyz = UIGetWindow(winID);
				UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_KEYS);
				char txt[256]; lab->GetText(txt, 255); strcat(txt, " ");
				if (KeyStoreOkk(txt, GWalletLists[wnKyz->dwValue].WalSecret) == false)
				{
					UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_MSG);
					lab->SetTexts("");
					lab->Visibled = true;
					lab->dwValue = GTimeClientCurrMs;
					lab->SetFontColor(0xff000000);
					break;
				}
				UICloseWinID(WINDOW_KEYYZZ);
				UIShowWinID(WINDOW_KEYYZF);
				UIWindowPtr wnKyf = UIGetWindow(WINDOW_KEYYZF);
				lab = wnKyf->GetLabel(KEYS_LAB_KEYS);
				lab->SetTexts("");
			}else if (winID == WINDOW_KEYYZF) {
				UIWindowPtr wnKyz = UIGetWindow(winID);
				UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_KEYS);
				char txt[256]; lab->GetText(txt, 255); strcat(txt, " ");
				if (KeyStoreOkk(txt, GWalletLists[wnKyz->dwValue].WalSecret) == false)
				{
					UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_MSG);
					lab->SetTexts("");
					lab->Visibled = true;
					lab->dwValue = GTimeClientCurrMs;
					lab->SetFontColor(0xff000000);
					break;
				}
				UICloseAll();
				UIShowWinID(WINDOW_MAIN);
				WinUpdateMain(false);
			}else if (winID == WINDOW_KEYSRZ) {
				UIWindowPtr wnKyz = UIGetWindow(winID);
				UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_KEYS);
				char txt[256]; lab->GetText(txt, 255); strcat(txt, " ");
				if (KeyStoreOkk(txt) == false)
				{
					UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_MSG);
					lab->SetTexts("");
					lab->Visibled = true;
					lab->dwValue = GTimeClientCurrMs;
					lab->SetFontColor(0xff000000);
					break;
				}
				UICloseWinID(WINDOW_KEYSRZ);
				UIShowWinID(WINDOW_KEYSRF);
				UIWindowPtr wnKsf = UIGetWindow(WINDOW_KEYSRF);
				lab = wnKsf->GetLabel(KEYS_LAB_KEYS);
				lab->SetTexts("");
			}else {
				UIWindowPtr wnKyz = UIGetWindow(winID);
				UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_KEYS);
				char txt[256]; lab->GetText(txt, 255); strcat(txt, " ");
				if (KeyStoreOkk(txt) == false)
				{
					UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_MSG);
					lab->SetTexts("");
					lab->Visibled = true;
					lab->dwValue = GTimeClientCurrMs;
					lab->SetFontColor(0xff000000);
					break;
				}
				UIWindowPtr wnKsz = UIGetWindow(WINDOW_KEYSRZ);
				lab = wnKsz->GetLabel(KEYS_LAB_KEYS);
				lab->GetText(txt, 255);
				uint8 secA[256]; KeyStoreGet(secA, txt);
				UIWindowPtr wnKsf = UIGetWindow(WINDOW_KEYSRF);
				lab = wnKsf->GetLabel(KEYS_LAB_KEYS);
				lab->GetText(txt, 255);
				uint8 secB[256]; KeyStoreGet(secB, txt);

				SecretToWallet(txt, secA, secB);
				UIWindowPtr wnDrw = UIGetWindow(WINDOW_DAORW);	// 
				lab = wnDrw->GetLabel(DAORW_LAB_KEYS);
				lab->SetTexts(txt);
				UICloseWinID(WINDOW_KEYSRF);
			}
		}else if ((KEYS_BTN_A <= ctrID) && (ctrID <= KEYS_BTN_Z)) {
			UIWindowPtr wnKyz = UIGetWindow(winID);
			UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_KEYS);
			char txt[256]; lab->GetText(txt, 255);
			int n = strlen(txt); if (n >= 255) break;
			txt[n] = 'a' + ctrID - KEYS_BTN_A;
			txt[n+1] = 0;
			lab->SetTexts(txt);
		}else if (ctrID == KEYS_BTN_BACK) {	// <-
			UIWindowPtr wnKyz = UIGetWindow(winID);
			UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_KEYS);
			char txt[256]; lab->GetText(txt, 255);
			int n = strlen(txt); if (n <= 0) break;
			txt[n-1] = 0;
			lab->SetTexts(txt);
		}else if (ctrID == KEYS_BTN_SPACE) {	// 
			UIWindowPtr wnKyz = UIGetWindow(winID);
			UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_KEYS);
			char txt[256]; lab->GetText(txt, 255);
			int n = strlen(txt); if (n >= 255) break;
			txt[n] = ' ';
			txt[n+1] = 0;
			lab->SetTexts(txt);
		}else if (ctrID == KEYS_BTN_RETURN) {	// 
			UIWindowPtr wnKyz = UIGetWindow(winID);
			UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_KEYS);
			char txt[256]; lab->GetText(txt, 255);
			int n = strlen(txt); if (n >= 255) break;
			txt[n] = '\n';
			txt[n+1] = 0;
			lab->SetTexts(txt);
		}else if (ctrID == KEYS_BTN_PASTE) {	// 
			UIWindowPtr wnKyz = UIGetWindow(winID);
			UILabelPtr lab = wnKyz->GetLabel(KEYS_LAB_KEYS);
			char txt[256] = {0};
			FILE * fp;
			if		(winID == WINDOW_KEYYZZ)	fp = fopen("a.txt", "rb");
			else if (winID == WINDOW_KEYYZF)	fp = fopen("b.txt", "rb");
			else if (winID == WINDOW_KEYSRZ)	fp = fopen("a.txt", "rb");
			else 								fp = fopen("b.txt", "rb");
			if (fp)
			{
				fread(txt, 1, 255, fp);
				fclose(fp);
			}
			lab->SetTexts(txt);
		}
		break;
	case WINDOW_DAORW:
		if (ctrID == DAORW_BTN_REMNO)	// 
		{
			WinUpdateDaoRW(true);
		}else if (ctrID == DAORW_BTN_SECNO) {	// 
			WinUpdateDaoRW(false);
		}else if (ctrID == DAORW_BTN_START) {	// 
			UIWindowPtr wnDrw = UIGetWindow(WINDOW_DAORW);	// 
#define DAORW_MSG(txMsg) \
{ \
	UILabelPtr lab = wnDrw->GetLabel(DAORW_LAB_MSG); \
	lab->SetTexts(txMsg); \
	lab->Visibled = (txMsg[0])? true: false; \
	lab->dwValue = GTimeClientCurrMs; \
	lab->SetFontColor(0xff000000); \
}
			UIEditPtr edt = wnDrw->GetEditBox(DAORW_EDT_WALLET);
			char nam[256]; edt->GetTexts(nam, 255);
			if (nam[0] == 0)
			{
				UISetEditFocusIn(WINDOW_DAORW, DAORW_EDT_WALLET);
				DAORW_MSG("");
				break;
			}
			edt = wnDrw->GetEditBox(DAORW_EDT_PSWD);
			char pswd[256]; edt->GetTexts(pswd, 255);
			if ((pswd[0] == 0) || (strlen(pswd) < 6))
			{
				UISetEditFocusIn(WINDOW_DAORW, DAORW_EDT_PSWD);
				DAORW_MSG("6");
				break;
			}
			edt = wnDrw->GetEditBox(DAORW_EDT_PSW);
			char psw[256]; edt->GetTexts(psw, 255);
			if (psw[0] == 0)
			{
				UISetEditFocusIn(WINDOW_DAORW, DAORW_EDT_PSW);
				DAORW_MSG("");
				break;
			}
			if (CharEqual(pswd, psw) == false)
			{
				UISetEditFocusIn(WINDOW_DAORW, DAORW_EDT_PSW);
				DAORW_MSG("");
				break;
			}
			UICheckPtr chk = wnDrw->GetCheckBox(DAORW_CHK_PLAN);
			if (chk->GetCheck() == false)
			{
				DAORW_MSG("")
				break;
			}
			edt = wnDrw->GetEditBox(DAORW_EDT_PLAN);
			char plan[256]; edt->GetTexts(plan, 255);

			UIBtnPtr btn = wnDrw->GetButton(DAORW_BTN_SECOK);
			if (btn->Visibled == true)
			{
				edt = wnDrw->GetEditBox(DAORW_EDT_SECA);
				char txt[256]; edt->GetTexts(txt, 255);
				if (txt[0] == 0)
				{
					UISetEditFocusIn(WINDOW_DAORW, DAORW_EDT_SECA);
					DAORW_MSG("");
					break;
				}
				uint8 secA[256]; Base58Decode(secA, 255, txt);
				edt = wnDrw->GetEditBox(DAORW_EDT_SECB);
				edt->GetTexts(txt, 255);
				if (txt[0] == 0)
				{
					UISetEditFocusIn(WINDOW_DAORW, DAORW_EDT_SECB);
					DAORW_MSG("");
					break;
				}
				uint8 secB[256]; Base58Decode(secB, 255, txt);
				CWALLET *a = CWALLET::Add();
				a->Created = false;
				UICheckPtr chk = wnDrw->GetCheckBox(DAORW_CHK_ZUFU);
				if (chk->GetCheck())	// 
				{
					a->WalType = 0;
					ByteCopy(a->WalSecret, secA, 32);
				}else {
					a->WalType = 1;
					ByteCopy(a->WalSecret, secB, 32);
				}
				uint64Ptr p = a->BCIAdd();
				if (p) *p = BCI_ADMIN_MAIN;
				CharCopy(a->Name36, 36, nam);
				CharCopy(a->WalPlan, 255, plan);
				uint8 pubkey[256]; secp256k1_ec_pubkey_create(a->WalPub63, pubkey, a->WalSecret);
				SecretToWallet(a->Wallet48, secA, secB);
				uint8 sha[64]; SHA3_256(sha, pswd, strlen(pswd));
				for (int i=0; i<32; ++i)
				{
					a->WalSecPsw[i] = sha[i] ^ a->WalSecret[i];
				}
				SignCreateBySecretKey(a->WalSecSig, sha, a->WalSecret);
				WalletWrite();
			}else {
				UILabelPtr lab = wnDrw->GetLabel(DAORW_LAB_KEYS);
				char wal[256]; lab->GetText(wal, 255);
				if (wal[0] == 0)
				{
					DAORW_MSG("");
					break;
				}

				CWALLET *a = CWALLET::Add();
				a->Created = false;
				CharCopy(a->Name36, 36, nam);
				CharCopy(a->WalPlan, 255, plan);
				CharCopy(a->Wallet48, 48+1, wal);
				UICheckPtr chk = wnDrw->GetCheckBox(DAORW_CHK_ZUFU);
				if (chk->GetCheck())	// 
				{
					a->WalType = 0;
					UIWindowPtr wnKsz = UIGetWindow(WINDOW_KEYSRZ);
					lab = wnKsz->GetLabel(KEYS_LAB_KEYS);
					lab->GetText(wal, 255);
					KeyStoreGet(a->WalSecret, wal);
				}else {
					a->WalType = 1;
					UIWindowPtr wnKsf = UIGetWindow(WINDOW_KEYSRF);
					lab = wnKsf->GetLabel(KEYS_LAB_KEYS);
					lab->GetText(wal, 255);
					KeyStoreGet(a->WalSecret, wal);
				}
				uint64Ptr p = a->BCIAdd();
				if (p) *p = BCI_ADMIN_MAIN;

				uint8 pubkey[256]; secp256k1_ec_pubkey_create(a->WalPub63, pubkey, a->WalSecret);
				uint8 sha[64]; SHA3_256(sha, pswd, strlen(pswd));
				for (int i=0; i<32; ++i)
				{
					a->WalSecPsw[i] = sha[i] ^ a->WalSecret[i];
				}
				SignCreateBySecretKey(a->WalSecSig, sha, a->WalSecret);
				WalletWrite();
			}
			if (GGameStatus == EGAMESTATUS::eGSEmptyRun)
			{
				GGameStatus = EGAMESTATUS::eGSGameInit;
			}else {
				WinUpdateGanLi();
				UICloseWinID(WINDOW_DAORW);
			}
		}else if (ctrID == DAORW_BTN_REMXX) {	// 
		}else if (ctrID == DAORW_BTN_SECXX) {	// 
		}else if (ctrID == DAORW_BTN_PASTEA) {	// 
			UIWindowPtr wnDrw = UIGetWindow(WINDOW_DAORW);	// 
			UIBtnPtr btn = wnDrw->GetButton(DAORW_BTN_PASTEA);
			btn->Visibled = false;
			uint8 utf8[256]; PasteBoardGet(utf8, 255);
			Base58Verify(utf8, 255);
			UIEditPtr edt = wnDrw->GetEditBox(DAORW_EDT_SECA);
			edt->SetTextAsUTF8(utf8);
			edt->SetCursorPosToEnd();
		}else if (ctrID == DAORW_BTN_PASTEB) {	// 
			UIWindowPtr wnDrw = UIGetWindow(WINDOW_DAORW);	// 
			UIBtnPtr btn = wnDrw->GetButton(DAORW_BTN_PASTEB);
			btn->Visibled = false;
			uint8 utf8[256]; PasteBoardGet(utf8, 255);
			Base58Verify(utf8, 255);
			UIEditPtr edt = wnDrw->GetEditBox(DAORW_EDT_SECB);
			edt->SetTextAsUTF8(utf8);
			edt->SetCursorPosToEnd();
		}else if (ctrID == DAORW_EDT_SECA) {
			UIWindowPtr wnDrw = UIGetWindow(WINDOW_DAORW);	// 
			UIBtnPtr btn = wnDrw->GetButton(DAORW_BTN_SECOK);
			if (btn->Visibled == true)
			{
				uint8 utf8[256]; PasteBoardGet(utf8, 255);
				if ((utf8[0] != 0) && (Base58Verify(utf8, 255) == true))
				{
					btn = wnDrw->GetButton(DAORW_BTN_PASTEA);
					btn->Visibled = true;
				}
			}
		}else if (ctrID == DAORW_EDT_SECB) {
			UIWindowPtr wnDrw = UIGetWindow(WINDOW_DAORW);	// 
			UIBtnPtr btn = wnDrw->GetButton(DAORW_BTN_SECOK);
			if (btn->Visibled == true)
			{
				uint8 utf8[256]; PasteBoardGet(utf8, 255);
				if ((utf8[0] != 0) && (Base58Verify(utf8, 255) == true))
				{
					btn = wnDrw->GetButton(DAORW_BTN_PASTEB);
					btn->Visibled = true;
				}
			}
		}else if (ctrID == DAORW_LAB_KEYS) {
		//	UICloseAll();
			UIShowWinID(WINDOW_KEYSRZ);
			UIWindowPtr wnKsr = UIGetWindow(WINDOW_KEYSRZ);
			UILabelPtr lab = wnKsr->GetLabel(KEYS_LAB_KEYS);
			lab->SetTexts("");
		}
		break;
	case WINDOW_SECDC:
		if (ctrID == SECDC_BTN_COPY)
		{
			UIWindowPtr wnSec = UIGetWindow(WINDOW_SECDC);
			UILabelPtr lab = wnSec->GetLabel(SECDC_LAB_SECRET);
			uint16* unic = lab->GetText();
			uint8 utf8[256]; UnicodeToUTF8(utf8, 255, unic, 255);
			PasteBoardSet(utf8);
		}
		break;
	case WINDOW_PSWD:	// 
		if (ctrID == PSWD_BTN_OK)
		{
			UIWindowPtr wnPsw = UIGetWindow(WINDOW_PSWD);
			CWALLET *p = &GWalletLists[wnPsw->dwValue];
			UIEditPtr edt = wnPsw->GetEditBox(PSWD_EDT_PSWD);	// 
			char psw[256]; edt->GetTexts(psw, 255);
			uint8 sha[64], sec[256]; SHA3_256(sha, psw, strlen(psw)); int i;
			for (i=0; i<32; ++i)
			{
				sec[i] = sha[i] ^ p->WalSecPsw[i];
			}
			char pub[256]; uint8 pubkey[256]; secp256k1_ec_pubkey_create(pub, pubkey, sec);
			if (SignVerifyByPublicKey(p->WalSecSig, sha, pub) == true)
			{
				ByteCopy(p->WalSecret, sec, 32);
				CharCopy(p->WalPub63, 63, pub);
				WalletWrite();
				UICloseWinID(WINDOW_PSWD);
				WinRun(wnPsw->dwType, wnPsw->dwValue);
			}else {
				UILabelPtr lab = wnPsw->GetLabel(PSWD_LAB_MSG);
				lab->SetTexts("");
				lab->Visibled = true;
				lab->dwValue = GTimeClientCurrMs;
				lab->SetFontColor(0xff000000);
			}
		}
		break;
	case WINDOW_BCI_CREATE:
		if (ctrID == BCI_CREATE_BTN_OK)
		{
			if (ByteIsZero(GWalletLists[0].WalSecret, 32) == true)
			{
				WinUpdatePswSr(WINDOW_BCI_CREATE, 0);
				break;
			}
			WinRun(WINDOW_BCI_CREATE, 0);
		}
		break;
	}
}
void UIMouseButtonLeftUpupPlaySoundCall(uint8 winID, uint8 ctrID)
{
}

# Functions: Support code (Misc, StrTok, WEBCTRL, GameMonitor, other)

Source: `sources\client-2.20\` (librehelbreath ClientSources c01544d). 49 function definitions (including `#if` duplicate bodies). Generated from a col-0 signature scan, then reviewed by hand.

Column key: **Where** file:start-end (verified). **Purpose** from name pattern and the nearest source comment (inferred unless the row ends in [V], meaning re-checked against the function body by the 2026-10-25 review, or the overview says verified; comments are UTF-8 Korean in source, quoted as-is). **Callers/Callees** matched by bare name across all files (verified as text matches; same-named methods in different classes are merged). **Msgs** MSGID/DEF_NOTIFY/etc. names in the body (see `..\..\shared\messages\`). **Dlg** dialog ids enabled/disabled (ids in `..\data-structures.md`). **Files** path literals in the body.

## Overview

- `CMisc`: direction maths (`cCalcDirection`, `cGetNextMoveDir`, `GetPoint`), name/string validation (five `#if` bodies of `bCheckValidName`), XOR file conversion (`_iConvertFileXor`, used for badword.txt), checksum. `CStrTok`: tokenizer. `CWebContainer`/`CWebCtrl`: embedded IE control for HTML notices (`DEF_HTMLCOMMOM`). `CGameMonitor`: bad-word list.
- Game.cpp functions here did not match a subsystem rule; each is listed with its own purpose.

## Function table

| Function | Where | Lines | Purpose | Callers | Callees | Msgs | Dlg | Files |
|---|---|---|---|---|---|---|---|---|
| `CGameMonitor::CGameMonitor` | GameMonitor.cpp:11-17 | 7 | comment: Construction/Destruction | - | - | - | - | - |
| `CGameMonitor::~CGameMonitor` | GameMonitor.cpp:19-25 | 7 | (name only) | - | - | - | - | - |
| `CGameMonitor::iReadBadWordFileList` | GameMonitor.cpp:27-64 | 38 | load (iRead...) and match (bCheck...) the badword list [V] | bInit | CMsg, CStrTok, pGet | - | - | - |
| `CGameMonitor::bCheckBadWord` | GameMonitor.cpp:66-83 | 18 | load (iRead...) and match (bCheck...) the badword list [V] | _bCheckBadWords | - | - | - | - |
| `CMisc::CMisc` | Misc.cpp:12-15 | 4 | comment: Construction/Destruction | - | - | - | - | - |
| `CMisc::~CMisc` | Misc.cpp:17-20 | 4 | (name only) | - | - | - | - | - |
| `CMisc::cGetNextMoveDir` | Misc.cpp:22-46 | 25 | direction code 1-8 (1 north, clockwise) from source toward destination, 0 if equal [V] | CommandProcessor, GetPlayerTurn, _DrawThunderEffect, bEffectFrameCounter, iObjectFrameCounter | - | - | - | - |
| `CMisc::GetPoint` | Misc.cpp:49-114 | 66 | Bresenham stepping iCount steps carrying the error term [V] | DrawEffects, _DrawThunderEffect, bEffectFrameCounter | - | - | - | - |
| `CMisc::GetDirPoint` | Misc.cpp:117-129 | 13 | (name only); *pY-- / *pX++ step the pointer (Misc.cpp:120-127); no caller ([CLIENT-STATE-009](../../issues/client-state.md#client-state-009-dead-and-faulty-helper-functions-in-cmisc)) [V] | - | - | - | - | - |
| `CMisc::bCheckValidString` | Misc.cpp:131-139 | 9 | accepts a string only if it has no spaces [V] | UpdateScreen_OnChangePassword, UpdateScreen_OnInputKeyCode, UpdateScreen_OnSelectCharacter, _Draw_OnLogin | - | - | - | - |
| `CMisc::bCheckIMEString` | Misc.cpp:141-149 | 9 | accepts a string only if it has no high-bit bytes [V] | ChatMsgHandler, DrawChatMsgBox, DrawDialogBox_Magic, DrawDialogBox_MagicShop, DrawNpcName, UpdateScreen_OnGame | - | - | - | - |
| `CMisc::ReplaceString` | Misc.cpp:151-158 | 8 | char substitution [V] | DrawDialogBox_Magic, DrawDialogBox_MagicShop, DrawDialogBox_Skill, InitPlayerCharacteristics, NotifyMsg_GuildDisbanded, UpdateScreen_OnCreateNewAccount, bSendCommand | - | - | - | - |
| `CMisc::cCalcDirection` | Misc.cpp:160-212 | 53 | same as cGetNextMoveDir using slope thresholds 1/3 and 3; returns 1 for equal points (cGetNextMoveDir returns 0); the fifth test (203-207) is reachable only at dTmp3 == -0.3333f [V] | DrawObjects, bAddNewEffect | - | - | - | - |
| `CMisc::ColorTransfer` | Misc.cpp:214-251 | 38 | COLORREF to per-channel 565/555 values [V] | DrawVersion, PutString_SprNum, _DrawThunderEffect, bInit | - | - | - | - |
| `CMisc::bEncode` | Misc.cpp:254-263 | 10 | (name only); no caller ([CLIENT-STATE-009](../../issues/client-state.md#client-state-009-dead-and-faulty-helper-functions-in-cmisc)) [V] | - | - | - | - | - |
| `CMisc::bDecode` | Misc.cpp:266-275 | 10 | (name only); no caller ([CLIENT-STATE-009](../../issues/client-state.md#client-state-009-dead-and-faulty-helper-functions-in-cmisc)) [V] | - | - | - | - | - |
| `CMisc::bCheckValidName` | Misc.cpp:278-306 | 29 | input-name validation, Korea (DEF_LANGUAGE 3) body; reject special characters [V] | UpdateScreen_OnChangePassword, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, UpdateScreen_OnInputKeyCode, UpdateScreen_OnLogin, _Draw_OnLogin, _bDraw_OnCreateNewCharacter | - | - | - | - |
| `CMisc::bCheckValidName` | Misc.cpp:308-351 | 44 | input-name validation, Taiwan (1) body; reject special characters [V] | UpdateScreen_OnChangePassword, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, UpdateScreen_OnInputKeyCode, UpdateScreen_OnLogin, _Draw_OnLogin, _bDraw_OnCreateNewCharacter | - | - | - | - |
| `CMisc::bCheckValidName` | Misc.cpp:353-384 | 32 | input-name validation, China (2) body; reject special characters [V] | UpdateScreen_OnChangePassword, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, UpdateScreen_OnInputKeyCode, UpdateScreen_OnLogin, _Draw_OnLogin, _bDraw_OnCreateNewCharacter | - | - | - | - |
| `CMisc::bCheckValidName` | Misc.cpp:386-416 | 31 | input-name validation, English (4, active): rejects non-ASCII and , = space \n \t . \ / : * ? < > / "; stricter set only with DEF_FUCK_USA body; reject special characters [V] | UpdateScreen_OnChangePassword, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, UpdateScreen_OnInputKeyCode, UpdateScreen_OnLogin, _Draw_OnLogin, _bDraw_OnCreateNewCharacter | - | - | - | - |
| `CMisc::bCheckValidName` | Misc.cpp:418-431 | 14 | input-name validation, Japan (5) body; reject special characters [V] | UpdateScreen_OnChangePassword, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, UpdateScreen_OnInputKeyCode, UpdateScreen_OnLogin, _Draw_OnLogin, _bDraw_OnCreateNewCharacter | - | - | - | - |
| `CMisc::_iGetFileCheckSum` | Misc.cpp:434-476 | 43 | sum of 3 sampled bytes of a name-obfuscated (+1 per char) file, unused ([CLIENT-STATE-009](../../issues/client-state.md#client-state-009-dead-and-faulty-helper-functions-in-cmisc)) [V] | - | - | - | - | - |
| `CMisc::_iConvertFileXor` | Misc.cpp:478-543 | 66 | file = 10-byte header (decimal body size, each byte XOR 20) + body XOR key (35); returns FALSE if size mismatches. Opened with mode rt (text mode: CRLF folding and Ctrl-Z end-of-file can corrupt XORed bytes); on mismatch the destination FILE is never closed. Missing Curse.txt/badword.txt makes bInit return FALSE and Initialize post a quit (Wmain.cpp:851-853) [V] | bInit | - | - | - | - |
| `CMisc::iGetTextLengthLoc` | Misc.cpp:545-561 | 17 | smallest prefix length whose GDI width exceeds a limit [V] | ChatMsgHandler | - | - | - | - |
| `CMisc::bIsValidSSN` | Misc.cpp:564-621 | 58 | length/digit (SSN) and '@'+'.' (email) checks, not used in English paths [V] | UpdateScreen_OnChangePassword, UpdateScreen_OnCreateNewAccount | - | - | - | - |
| `CMisc::bIsValidSSN` | Misc.cpp:624-654 | 31 | length/digit (SSN) and '@'+'.' (email) checks, not used in English paths [V] | UpdateScreen_OnChangePassword, UpdateScreen_OnCreateNewAccount | - | - | - | - |
| `CMisc::bIsValidEmail` | Misc.cpp:657-677 | 21 | length/digit (SSN) and '@'+'.' (email) checks, not used in English paths [V] | UpdateScreen_OnCreateNewAccount | - | - | - | - |
| `CStrTok::CStrTok` | StrTok.cpp:11-18 | 8 | comment: Construction/Destruction | OnKeyUp, _LoadAgreementTextContents, _LoadGameMsgTextContents, _LoadTextDlgContents, __bDecodeBuildItemContents, __bDecodeContentsAndBuildItemForSaleList, _iLoadTextDlgContents2, bCheckLocalChatCommand, +4 | - | - | - | - |
| `CStrTok::~CStrTok` | StrTok.cpp:20-23 | 4 | (name only) | - | - | - | - | - |
| `CStrTok::pGet` | StrTok.cpp:25-67 | 43 | next token by separator set [V] | OnKeyUp, _LoadAgreementTextContents, _LoadGameMsgTextContents, _LoadTextDlgContents, __bDecodeBuildItemContents, __bDecodeContentsAndBuildItemForSaleList, _iLoadTextDlgContents2, bCheckLocalChatCommand, +4 | _bIsSeperator | - | - | - |
| `CStrTok::_bIsSeperator` | StrTok.cpp:69-81 | 13 | (name only) | pGet | - | - | - | - |
| `CWebContainer::CWebContainer` | WEBCTRL.CPP:6-17 | 12 | (name only) | WebCtrlProc | - | - | - | - |
| `CWebContainer::~CWebContainer` | WEBCTRL.CPP:19-24 | 6 | (name only) | - | - | - | - | - |
| `CWebContainer::add` | WEBCTRL.CPP:26-54 | 29 | embedded IE ActiveX host (add/remove/setLocation/Navigate); only used with DEF_HTMLCOMMOM [V] | WebCtrlProc | ConnectEvents, setFocus, setVisible | - | - | - |
| `CWebContainer::remove` | WEBCTRL.CPP:62-85 | 24 | embedded IE ActiveX host (add/remove/setLocation/Navigate); only used with DEF_HTMLCOMMOM [V] | WebCtrlProc | DisconnectEvents | - | - | - |
| `CWebContainer::setLocation` | WEBCTRL.CPP:93-107 | 15 | embedded IE ActiveX host (add/remove/setLocation/Navigate); only used with DEF_HTMLCOMMOM [V] | WebCtrlProc | - | - | - | - |
| `CWebContainer::setVisible` | WEBCTRL.CPP:115-130 | 16 | (name only) | add | - | - | - | - |
| `CWebContainer::setFocus` | WEBCTRL.CPP:138-150 | 13 | (name only) | add | - | - | - | - |
| `CWebContainer::ConnectEvents` | WEBCTRL.CPP:152-159 | 8 | (name only) | add | GetConnectionPoint | - | - | - |
| `CWebContainer::DisconnectEvents` | WEBCTRL.CPP:161-170 | 10 | (name only) | remove | GetConnectionPoint | - | - | - |
| `CWebContainer::GetConnectionPoint` | WEBCTRL.CPP:172-185 | 14 | (name only) | ConnectEvents, DisconnectEvents | - | - | - | - |
| `CWebContainer::getWeb` | WEBCTRL.CPP:187-190 | 4 | (name only) | WebCtrlProc | - | - | - | - |
| `CWebCtrl::CWebCtrl` | WEBCTRL.CPP:194-199 | 6 | comment: WebCtrl | WebCtrlProc | - | - | - | - |
| `WebCtrlProc` | WEBCTRL.CPP:201-224 | 24 | (name only) | - | CWebCtrl | - | - | - |
| `CWebCtrl::Navigate` | WEBCTRL.CPP:226-266 | 41 | embedded IE ActiveX host (add/remove/setLocation/Navigate); only used with DEF_HTMLCOMMOM [V] | WebCtrlProc | - | - | - | - |
| `CWebCtrl::WebCtrlProc` | WEBCTRL.CPP:268-322 | 55 | (name only) | - | CWebContainer, Navigate, add, getWeb, remove, setLocation | - | - | - |
| `InitWebCtrl` | WEBCTRL.CPP:324-346 | 23 | (name only) | CreateWebControl, InitInstance | - | - | - | - |
| `UninitWebCtrl` | WEBCTRL.CPP:348-360 | 13 | (name only) | CreateWebControl | - | - | - | - |
| `CreateWebControl` | WEBCTRL.CPP:364-383 | 20 | (name only) | InitInstance | InitWebCtrl, UninitWebCtrl | - | - | - |

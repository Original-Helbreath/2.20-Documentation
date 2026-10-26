# Functions: Startup, main loop and shutdown

Source: `sources\client-2.20\` (librehelbreath ClientSources c01544d). 29 function definitions (including `#if` duplicate bodies). Generated from a col-0 signature scan, then reviewed by hand.

Column key: **Where** file:start-end (verified). **Purpose** from name pattern and the nearest source comment (inferred unless the row ends in [V], meaning re-checked against the function body by the 2026-10-25 review, or the overview says verified; comments are UTF-8 Korean in source, quoted as-is). **Callers/Callees** matched by bare name across all files (verified as text matches; same-named methods in different classes are merged). **Msgs** MSGID/DEF_NOTIFY/etc. names in the body (see `..\..\shared\messages\`). **Dlg** dialog ids enabled/disabled (ids in `..\data-structures.md`). **Files** path literals in the body.

## Overview

- **Entry** (verified): `WinMain` (Wmain.cpp:236) loads `search.dll` through an obfuscated name (`"rd`qbg-ckk"`, each byte +1) and calls `__FindHackingDll__("CRCCHECK")` (a failed check returns before any window exists), registers window class `Client-I%d` (title `Helbreath Crusade`) and runs `Initialize` (bInit), and only then, under `#ifndef _DEBUG`, takes the single-instance mutex `0543kjg3j31%` (Wmain.cpp:275-294), then runs `EventLoop`.
- **Initialize** (Wmain.cpp:834, verified): `WSAStartup`, `CGame::bInit`, a 1000 ms periodic multimedia timer (`timeSetEvent` in `_StartTimer`, Wmain.cpp:413, 857; `_TimerFunc` posts `WM_USER_TIMERSIGNAL`, WndProc calls `CGame::OnTimer`), and the blend/add lookup tables (see `..\rendering\blitters.md`).
- **CGame::bInit** (Game.cpp:811, verified) order: command line, `bCheckImportantFile`, `_bDecodeBuildItemContents`, `login.cfg`, `ItemName.cfg`, `magiccfg.txt`, `Skillcfg.txt`, DirectDraw and DirectInput init, `New-Dialog.pak` and `interface2.pak`, `CMapData`, dye/colour tables, `badword.txt` (XOR 35 into a temp `.tmp`, deleted after), `Curse.txt` (DEF_LANGUAGE > 2), `GameMsgList`. Details per file in `config-loading.md`.
- **Main loop** (verified): `EventLoop` pumps messages with `PeekMessage`, and on idle calls `CGame::UpdateScreen`, which dispatches on `m_cGameMode` (Game.cpp:1106; table in `game-modes.md`). Frame pacing is in `..\rendering\frame-pipeline.md`.
- **OnTimer** (verified): every 8 s sends `MSGID_COMMAND_CHECKCONNECTION` and sweeps sprites unused for 60 s; every 2 s it counts the lag (`m_cCommandCount >= 6` -> `++m_iNetLagCount`) and goes to `ONCONNECTIONLOST` when the counter reaches 7 (Game.cpp:4272-4301); every 5 s it also runs the level-up/crusade checks, and has a billing branch (Game.cpp:4281-4285, 4304-4346) that is inert in English because `G_bIsCalcSocketConnected` starts TRUE (Wmain.cpp:63).
- **ThreadProc** (Wmain.cpp:429-785) only exists for DEF_LANGUAGE == 3: Korean ISP billing sockets. Inert in the English build.
- **Settings** (verified): `ReadSettings`/`WriteSettings` use `HKCU\Software\Siementech\Helbreath\Settings`, values `Magic` and `ShortCut0`..`ShortCut4`.
- **Shutdown**: `CGame::Quit` releases sockets, sprites and DirectX; `GoHomepage` opens the publisher site on exit for some language builds: `OnDestroy` calls it only under `#if DEF_LANGUAGE < 3` (Taiwan, China; Wmain.cpp:396-398, verified), never in the English build.

## Function table

| Function | Where | Lines | Purpose | Callers | Callees | Msgs | Dlg | Files |
|---|---|---|---|---|---|---|---|---|
| `CGame::ReadSettings` | Game.cpp:320-381 | 62 | reads Settings; comment: Construction/Destruction | CGame | - | - | - | - |
| `CGame::WriteSettings` | Game.cpp:383-438 | 56 | (name only) | Quit | - | - | - | - |
| `CGame::CGame` | Game.cpp:440-801 | 362 | comment: 2002. 10. 29 정진광 아이템 버리기.. | - | ReadSettings | DEF_OBJECTSTOP | - | - |
| `CGame::~CGame` | Game.cpp:804-808 | 5 | (name only) | - | - | - | - | - |
| `CGame::bInit` | Game.cpp:811-1020 | 210 | comment: 클라이언트 초기화 | Initialize, WndProc | CMapData, CSprite, CStrTok, ColorTransfer, Create, LoadCurse, _LoadGameMsgTextContents, _bDecodeBuildItemContents, _iConvertFileXor, bCheckImportantFile, +6 | - | - | BADWORD.TXT file contains wrong infomation., CURSE.TXT file contains wrong infomation., ItemName.cfg file contains wrong infomation., MAGICCFG.TXT file contains wrong infomation., +9 |
| `CGame::Quit` | Game.cpp:1023-1103 | 81 | comment: 종료 처리 | OnDestroy | ChangeGameMode, WriteSettings | - | - | - |
| `CGame::OnTimer` | Game.cpp:4266-4348 | 83 | timer tick handler: keep-alive and sprite sweep every 8 s, lag counter every 2 s, level-up/crusade checks every 5 s, billing branch (see Overview) [V] | WndProc | ChangeGameMode, EnableDialogBox, PlaySound, ReleaseTimeoverChatMsg, ReleaseUnusedSprites, _iCheckLUS, bSendCommand, bStop, iSendMsgBlockingMode | DEF_MSGTYPE_CONFIRM, MSGID_COMMAND_CHECKCONNECTION | 12, 33 | - |
| `CGame::InitGameSettings` | Game.cpp:4901-5148 | 248 | comment: 초기화 해주어야 할 수치를 설정한다. | UpdateScreen_OnSelectCharacter | EnableDialogBox | DEF_OBJECTSTOP | 9 | - |
| `CGame::bInitMagicCfgList` | Game.cpp:8326-8498 | 173 | comment: 마법 번호 | bInit | CStrTok, _bGetIsStringIsNumber, pGet | - | - | magiccfg.txt |
| `CGame::bCheckImportantFile` | Game.cpp:8500-8519 | 20 | checks only that contents\badword.txt exists [V] | WndProc, bInit | - | - | - | CONTENTS\\badword.txt |
| `CGame::bInitSkillCfgList` | Game.cpp:8521-8637 | 117 | comment: 기술 번호 | bInit | CStrTok, _bGetIsStringIsNumber, pGet | - | - | Skillcfg.txt |
| `CGame::ClearContents_OnCreateNewAccount` | Game.cpp:15499-15509 | 11 | comment: v1.4334 | UpdateScreen_OnAgreement | - | - | - | - |
| `CGame::ReleaseTimeoverChatMsg` | Game.cpp:15804-15839 | 36 | (name only) | OnTimer | - | - | - | - |
| `CGame::ClearContents_OnSelectCharacter` | Game.cpp:21533-21536 | 4 | (name only) | LogResponseHandler, UpdateScreen_OnMainMenu | - | - | - | - |
| `CGame::GoHomepage` | Game.cpp:47865-47935 | 71 | comment: 인터네셔널.. | OnDestroy, UpdateScreen_OnMainMenu | GetRegKey | - | - | .htm |
| `WndProc` | Wmain.cpp:72-234 | 163 | main window procedure: routes window, key, timer and socket messages to the CGame handlers; billing-socket-closed case handled inside | - | ChangeGameMode, GetText, OnDestroy, OnGameSocketEvent, OnKeyDown, OnKeyUp, OnLogSocketEvent, OnSysKeyDown, OnSysKeyUp, OnTimer, +5 | - | - | - |
| `WinMain` | Wmain.cpp:236-304 | 69 | comment: v1.432 | - | EventLoop, InitApplication, InitInstance, Initialize | - | - | - |
| `InitApplication` | Wmain.cpp:306-322 | 17 | (name only) | WinMain | - | - | - | - |
| `InitInstance` | Wmain.cpp:324-371 | 48 | comment: #ifdef DEF_FULL_SCREEN | WinMain | CreateWebControl, InitWebCtrl | - | - | - |
| `EventLoop` | Wmain.cpp:373-386 | 14 | PeekMessage pump with idle UpdateScreen [V] | WinMain | UpdateScreen, UpdateScreen_OnLoading | - | - | - |
| `OnDestroy` | Wmain.cpp:388-400 | 13 | event handler Destroy; comment: 대만과 중국만 종료시 홈페이지로 가장.. | WndProc | GoHomepage, Quit, _StopTimer | - | - | - |
| `_TimerFunc` | Wmain.cpp:402-405 | 4 | 1 s multimedia timer (timeSetEvent) posting WM_USER_TIMERSIGNAL [V] | - | - | - | - | - |
| `_StartTimer` | Wmain.cpp:407-414 | 8 | 1 s multimedia timer (timeSetEvent) posting WM_USER_TIMERSIGNAL [V] | Initialize | - | - | - | - |
| `_StopTimer` | Wmain.cpp:416-426 | 11 | 1 s multimedia timer (timeSetEvent) posting WM_USER_TIMERSIGNAL [V] | OnDestroy | - | - | - | - |
| `ThreadProc` | Wmain.cpp:429-785 | 357 | Korean billing connect thread (DEF_LANGUAGE==3 only) [V] | - | CPSocketConnet, XSocket, bBlockConnect, bInitBufferSize, iSendMsg, iSendMsgBlockingMode | - | - | - |
| `CPSocketConnet` | Wmain.cpp:788-831 | 44 | comment: v2.03 과금 | ThreadProc | XSocket, bBlockConnect, bInitBufferSize | - | - | - |
| `Initialize` | Wmain.cpp:834-883 | 50 | WSAStartup, bInit, blend/add tables, start timer [V] | WinMain | _StartTimer, bInit | - | - | - |
| `GetRegKey` | Wmain.cpp:885-899 | 15 | (name only) | GoHomepage | - | - | - | - |
| `GoHomepage` | Wmain.cpp:901-945 | 45 | comment: If it failed, get the .htm regkey and lookup the program | OnDestroy, UpdateScreen_OnMainMenu | GetRegKey | - | - | .htm |

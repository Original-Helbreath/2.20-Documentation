# Window UI and logging

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
`cfg:` means data loaded from that config, not a literal file open in the body [V, review G1].
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 20

## CGame::DisplayInfo - SRV Game.cpp:888-973

Paints the status block (888-973): a status line (TAIWAN variant at 895), mode banners per switch (903-957; shown in this build: CHINESE and DB GAME SERVER) and one row per map: "Map(%s) Object(%d) P N A E M" at y = 400 + 15*row (961-972). Reads no config file.

- sig: `void CGame::DisplayInfo(HDC hdc)`
- callers: OnPaint
- cfg: none (prints members only)
- switches: DEF_BALLEVENT, DEF_BEGINNEREVENTMODE, DEF_CHINESE, DEF_DBGAMESERVER, DEF_GUILDWARMODE, DEF_INTERSERVER, DEF_ITEMDROPRATINGUP, DEF_TAIWAN, DEF_TESTSERVER
- Korean comments (UTF-8, first 1, untranslated quote): NT에서 에러남! 주의

## CGame::OnKeyDown - SRV Game.cpp:32869-32882

F1/F4/F12 flags (sets m_bF1pressed and similar).

- sig: `void CGame::OnKeyDown(WPARAM wParam, LPARAM lParam)`
- callers: WndProc

## CGame::OnKeyUp - SRV Game.cpp:32884-32973

Hotkeys (32884-32973); most need F1 held: F1+F6 and F1+F7 send shutdown announcements 1 and 2; F1+F9 LocalStartCrusadeMode; F1+F11 ManualEndCrusadeMode(0); F1+Home GlobalStartCrusadeMode; Insert and Delete launch the meteor via _GrandMagicLaunchMsgSend plus MeteorStrikeMsgHandler (no F1 needed); F2 and End are dead. This is the CGame member that WndProc calls (Wmain.cpp:80) ([GSO-045](../../issues/game-server-other.md#gso-045-unused-logging-and-hotkey-code)).

- sig: `void CGame::OnKeyUp(WPARAM wParam, LPARAM lParam)`
- callers: WndProc
- callees: GlobalStartCrusadeMode, LocalStartCrusadeMode, ManualEndCrusadeMode, MeteorStrikeMsgHandler, PutLogList, SendNotifyMsg, _GrandMagicLaunchMsgSend
- notifies: DEF_NOTIFY_SERVERSHUTDOWN
- Korean comments (UTF-8, first 2, untranslated quote): 테스트 / 게임 서버 셧다운이 시작되면 더이상의 클라이언트 접속이 거부된다.

## CGame::bOnClose - SRV Game.cpp:38183-38191

WM_CLOSE confirmation: Yes/No message box titled with m_cServerName unless the server already shut down (38183-38191). Reads no config file.

- sig: `BOOL CGame::bOnClose()`
- callers: WndProc
- cfg: none

## CGame::_bItemLog - SRV Game.cpp:42474-42583 (inactive: DEF_TAIWANLOG)

Item log (Taiwan layout; inactive). Overload (iGiveH, iRecvH, pItem, bForceItemLog): the handle form. Per-overload caller lists are given on the active twin below.

- sig: `BOOL CGame::_bItemLog(int iAction,int iGiveH, int iRecvH, class CItem * pItem,BOOL bForceItemLog)`
- callers: CGame::AdminOrder_CloseConn, CGame::AdminOrder_CreateItem, CGame::AdminOrder_SummonPlayer, CGame::BuildItemHandler, CGame::CheckSpecialEventThirdYear, CGame::ConfirmExchangeItem, CGame::DeleteNpc, CGame::DismissGuildApproveHandler, CGame::DropItemHandler, CGame::DynamicObjectEffectProcessor, CGame::FightzoneReserveHandler, CGame::GSM_RequestShutupPlayer, CGame::GetFightzoneTicketHandler, CGame::GetHeroMantleHandler, CGame::GiveItemHandler, CGame::ItemDepleteHandler, CGame::JoinGuildApproveHandler, CGame::NpcDeadItemGenerator, CGame::NpcDeadItemGeneratorWithItemEvent, CGame::PlayerMagicHandler, CGame::ReqRepairItemCofirmHandler, CGame::ReqSellItemConfirmHandler, CGame::RequestCivilRightHandler, CGame::RequestDisbandGuildHandler, CGame::RequestItemUpgradeHandler ...
- callees: bSendMsgToLS
- messages: MSGID_GAMEITEMLOG
- Korean comments (UTF-8, first 2, untranslated quote): !!주의 한국에 적용할때  New Item 이 생길때는  iGive가 널일수 있다. / 복사된 아이템 저장 요청이다.

## CGame::_bItemLog - SRV Game.cpp:42586-42698 (inactive: DEF_TAIWANLOG)

Item log by name (Taiwan; inactive). Overload (iClientH, char* name, pItem): the name form. Per-overload caller lists are given on the active twin below.

- sig: `BOOL CGame::_bItemLog(int iAction,int iClientH , char * cName, class CItem * pItem)`
- callers: CGame::AdminOrder_CloseConn, CGame::AdminOrder_CreateItem, CGame::AdminOrder_SummonPlayer, CGame::BuildItemHandler, CGame::CheckSpecialEventThirdYear, CGame::ConfirmExchangeItem, CGame::DeleteNpc, CGame::DismissGuildApproveHandler, CGame::DropItemHandler, CGame::DynamicObjectEffectProcessor, CGame::FightzoneReserveHandler, CGame::GSM_RequestShutupPlayer, CGame::GetFightzoneTicketHandler, CGame::GetHeroMantleHandler, CGame::GiveItemHandler, CGame::ItemDepleteHandler, CGame::JoinGuildApproveHandler, CGame::NpcDeadItemGenerator, CGame::NpcDeadItemGeneratorWithItemEvent, CGame::PlayerMagicHandler, CGame::ReqRepairItemCofirmHandler, CGame::ReqSellItemConfirmHandler, CGame::RequestCivilRightHandler, CGame::RequestDisbandGuildHandler, CGame::RequestItemUpgradeHandler ...
- callees: bSendMsgToLS
- messages: MSGID_GAMEITEMLOG
- Korean comments (UTF-8, first 1, untranslated quote): 로그 남긴다.

## CGame::_bItemLog - SRV Game.cpp:42704-42816

Item log (Korean layout, active) -> MSGID_GAMEITEMLOG. Handle form (iGiveH, iRecvH, pItem). Unless bForceItemLog it returns FALSE for non-good items (42712) and always for DEF_ITEMLOG_RETRIEVE (42713); 13 actions are handled; the guard `m_pClientList[iGiveH]->m_cCharName == NULL` (42708) tests an array and is never true, and it dereferences a possibly NULL client ([GSO-043](../../issues/game-server-other.md#gso-043-_bitemlog-null-guard-tests-an-array-and-the-function-can-dereference-null)).
- sig: `BOOL CGame::_bItemLog(int iAction,int iGiveH, int iRecvH, class CItem * pItem,BOOL bForceItemLog)`
- callers (handle form only [V]; call sites compiled in this build): CheckSpecialEventThirdYear, ConfirmExchangeItem, DropItemHandler, GetFightzoneTicketHandler, GetHeroMantleHandler, GiveItemHandler, ItemDepleteHandler, PlayerMagicHandler, ReqSellItemConfirmHandler, RequestItemUpgradeHandler, RequestRetrieveItemHandler, _bDecodePlayerDatafileContents (DB decoder 6099, 6239), bCheckIsItemUpgradeSuccess, iClientMotion_GetItem_Handler. Handle-form calls inside inactive blocks (DEF_TAIWANLOG or the file-mode decoder): AdminOrder_CreateItem, BuildItemHandler, RequestCivilRightHandler, RequestPurchaseItemHandler, bSetItemToBankItem (both overloads), file-mode _bDecodePlayerDatafileContents.
- merged name-scan list for reference (all four overloads, cut at 25 of about 39 distinct callers): CGame::AdminOrder_CloseConn, CGame::AdminOrder_CreateItem, CGame::AdminOrder_SummonPlayer, CGame::BuildItemHandler, CGame::CheckSpecialEventThirdYear, CGame::ConfirmExchangeItem, CGame::DeleteNpc, CGame::DismissGuildApproveHandler, CGame::DropItemHandler, CGame::DynamicObjectEffectProcessor, CGame::FightzoneReserveHandler, CGame::GSM_RequestShutupPlayer, CGame::GetFightzoneTicketHandler, CGame::GetHeroMantleHandler, CGame::GiveItemHandler, CGame::ItemDepleteHandler, CGame::JoinGuildApproveHandler, CGame::NpcDeadItemGenerator, CGame::NpcDeadItemGeneratorWithItemEvent, CGame::PlayerMagicHandler, CGame::ReqRepairItemCofirmHandler, CGame::ReqSellItemConfirmHandler, CGame::RequestCivilRightHandler, CGame::RequestDisbandGuildHandler, CGame::RequestItemUpgradeHandler ...
- callees: _bCheckGoodItem, bSendMsgToLS, iGetPeerAddress
- messages: MSGID_GAMEITEMLOG
- Korean comments (UTF-8, first 3, untranslated quote): !!주의 한국에 적용할때  New Item 이 생길때는  iGiveH가 널일수 있다. / v2.17 2002-7-31 새크리파이스 버그 여부를 판별하기 위해 추적 로그 추가 / 복사된 아이템 저장 요청이다.

## CGame::_bItemLog - SRV Game.cpp:42819-42870

Item log by name (active). Name form (iClientH, char* name, pItem). Returns FALSE when pItem is NULL (42821) or not a good item (42822); only NEWGENDROP, SKILLLEARN, MAGICLEARN, SUMMONMONSTER, POISONED and REPAIR are handled. The only callers compiled in this build are DeleteNpc (30341), NpcDeadItemGenerator (45620) and NpcDeadItemGeneratorWithItemEvent (45740), all NEWGENDROP; every other by-name call site passes a NULL item and sits in an inactive DEF_TAIWANLOG block, so in practice only NEWGENDROP can log ([GSO-044](../../issues/game-server-other.md#gso-044-most-item-log-call-sites-are-inactive)).
- sig: `BOOL CGame::_bItemLog(int iAction,int iClientH , char * cName, class CItem * pItem)`
- callers (name form only [V]): DeleteNpc, NpcDeadItemGenerator, NpcDeadItemGeneratorWithItemEvent. Name-form calls in inactive DEF_TAIWANLOG blocks: AdminOrder_CloseConn, AdminOrder_SummonPlayer, DismissGuildApproveHandler, DynamicObjectEffectProcessor, FightzoneReserveHandler, GSM_RequestShutupPlayer, GiveItemHandler, JoinGuildApproveHandler, PlayerMagicHandler, ReqRepairItemCofirmHandler, RequestDisbandGuildHandler, RequestStudyMagicHandler, RequestSummonWarUnitHandler, ResponseCreateNewGuildHandler, ServerStockMsgHandler, ShutUpPlayer, TrainSkillResponse, UserCommand_BanGuildsman, iCalculateAttackEffect.
- merged name-scan list for reference (all four overloads): CGame::AdminOrder_CloseConn, CGame::AdminOrder_CreateItem, CGame::AdminOrder_SummonPlayer, CGame::BuildItemHandler, CGame::CheckSpecialEventThirdYear, CGame::ConfirmExchangeItem, CGame::DeleteNpc, CGame::DismissGuildApproveHandler, CGame::DropItemHandler, CGame::DynamicObjectEffectProcessor, CGame::FightzoneReserveHandler, CGame::GSM_RequestShutupPlayer, CGame::GetFightzoneTicketHandler, CGame::GetHeroMantleHandler, CGame::GiveItemHandler, CGame::ItemDepleteHandler, CGame::JoinGuildApproveHandler, CGame::NpcDeadItemGenerator, CGame::NpcDeadItemGeneratorWithItemEvent, CGame::PlayerMagicHandler, CGame::ReqRepairItemCofirmHandler, CGame::ReqSellItemConfirmHandler, CGame::RequestCivilRightHandler, CGame::RequestDisbandGuildHandler, CGame::RequestItemUpgradeHandler ...
- callees: _bCheckGoodItem, bSendMsgToLS, iGetPeerAddress
- messages: MSGID_GAMEITEMLOG
- Korean comments (UTF-8, first 1, untranslated quote): 로그 남긴다.

## CGame::_bCheckGoodItem - SRV Game.cpp:42874-43010

Whether an item is valuable enough to log: gold (id 90) only when m_dwCount > 10000 (42878-42882); a fixed id list (20, 259, 290 ... 776) always counts; default branch (43004-43008): any item with `(attr & 0xF0F0F001) != 0` and id > 30 counts as good.

- sig: `BOOL CGame::_bCheckGoodItem( class CItem * pItem )`
- callers: CGame::_bItemLog
- Korean comments (UTF-8, first 6, untranslated quote): Gold에 한해 10000원 이상만 로그에 남긴다. / 엑스 칼리버 / case 90: // Gold 추가

## CGame::_bCrusadeLog - SRV Game.cpp:43012-43047

Crusade log -> MSGID_GAMECRUSADELOG.

- sig: `BOOL CGame::_bCrusadeLog(int iAction,int iClientH,int iData, char * cName)`
- callers: CGame::CheckCrusadeResultCalculation, CGame::LocalEndCrusadeMode, CGame::LocalStartCrusadeMode, CGame::SelectCrusadeDutyHandler
- callees: bSendMsgToLS
- messages: MSGID_GAMECRUSADELOG
- Korean comments (UTF-8, first 1, untranslated quote): 로그 남긴다.

## CGame::_bPKLog - SRV Game.cpp:43050-43097

PK log -> MSGID_GAMEITEMLOG.

- sig: `BOOL CGame::_bPKLog(int iAction,int iAttackerH , int iVictumH, char * pNPC)`
- callers: CGame::ApplyCombatKilledPenalty, CGame::ApplyPKpenalty, CGame::ClientKilledHandler, CGame::EnemyKillRewardHandler, CGame::PK_KillRewardHandler
- callees: bSendMsgToLS
- messages: MSGID_GAMEITEMLOG
- Korean comments (UTF-8, first 1, untranslated quote): 로그 남긴다.

## PutLogList - SRV Wmain.cpp:287-296

Prepends a line to the on-screen log ring and flags a repaint (Wmain.cpp:287-296): shifts 49 lines down 120 bytes, writes the new line at slot 0 (max 120 chars, no bound check on the memcpy), and sets G_cMsgUpdated; the repaint happens later in UpdateScreen (298-304). OnPaint shows 20 of the 50 slots (319-322). The callers list below is cut at 25 of about 106 ([GSO-050](../../issues/game-server-other.md#gso-050-putloglist-copies-the-message-without-a-bound)).

- sig: `void PutLogList(char * cMsg)`
- callers: CGame::AdminOrder_SetForceRecallTime, CGame::AdminOrder_Summon, CGame::BuildItemHandler, CGame::CalcMeteorStrikeEffectHandler, CGame::ChatMsgHandler, CGame::CheckClientResponseTime, CGame::CheckConnectionHandler, CGame::ClientKilledHandler, CGame::CollectedManaHandler, CGame::CreateCrusadeStructures, CGame::DeleteClient, CGame::EnemyKillRewardHandler, CGame::EnergySphereProcessor, CGame::FightzoneReserveHandler, CGame::ForceDisconnectAccount, CGame::GSM_ConstructionPoint, CGame::GSM_SetGuildConstructLoc, CGame::GSM_SetGuildTeleportLoc, CGame::GiveItemHandler, CGame::InitPlayerData, CGame::JoinPartyHandler, CGame::LocalEndCrusadeMode, CGame::LocalSavePlayerData, CGame::LocalStartCrusadeMode, CGame::MeteorStrikeHandler ...

## UpdateScreen - SRV Wmain.cpp:298-304

Invalidates the window.

- sig: `void UpdateScreen()`
- callers: EventLoop

## OnPaint - SRV Wmain.cpp:307-328

Draws the log lines and DisplayInfo.

- sig: `void OnPaint()`
- callers: WndProc
- callees: DisplayInfo
- Korean comments (UTF-8, first 1, untranslated quote): 이벤트 리스트를 출력한다.

## OnKeyUp - SRV Wmain.cpp:332-334

Empty body (Wmain.cpp:332-334) and never called: the only free-function call is commented out (Wmain.cpp:96-98); WndProc calls the CGame::OnKeyUp member (Wmain.cpp:80). The name-scan caller WndProc is a merge with the member ([GSO-045](../../issues/game-server-other.md#gso-045-unused-logging-and-hotkey-code)).

- sig: `void OnKeyUp(WPARAM wParam, LPARAM lParam)`
- callers: none (see above)

## PutLogFileList - SRV Wmain.cpp:378-396

Appends to the event log file Events.log (Wmain.cpp:384).

- sig: `void PutLogFileList(char * cStr)`
- callers: CGame::BuildItemHandler, CGame::CheckClientResponseTime, CGame::CheckConnectionHandler, CGame::CheckSpecialEvent, CGame::CheckSpecialEventThirdYear, CGame::EnergySphereProcessor, CGame::FightzoneReserveHandler, CGame::GetExp, CGame::GetHeroMantleHandler, CGame::GetOccupyFlagHandler, CGame::OnSubLogSocketEvent, CGame::OnTimer, CGame::RequestInitDataHandler, CGame::_PenaltyItemDrop, CGame::_bDecodePlayerDatafileContents, CGame::bCheckEnergySphereDestination, CGame::bCheckLevelUp, CGame::bSendMsgToLS

## PutAdminLogFileList - SRV Wmain.cpp:398-416

Appends to the admin (GM) log file AdminEvents.log (Wmain.cpp:404).

- sig: `void PutAdminLogFileList(char * cStr)`
- callers: CGame::_BWM_Command_Shutup

## PutItemLogFileList - SRV Wmain.cpp:418-458

Buffers item-log lines in G_cLogBuffer (30000 bytes) and flushes at 100 entries, after 10 s, or when bIsSave is set, to `LogData\ItemEvents<yyyymmdd>.log` (Wmain.cpp:418-458). It has no live caller: the only call is commented out (Game.cpp:42490), so item logs reach the Log server through bSendMsgToLS only ([GSO-045](../../issues/game-server-other.md#gso-045-unused-logging-and-hotkey-code)).

- sig: `void PutItemLogFileList(char * cStr, BOOL bIsSave)`
- Korean comments (UTF-8, first 3, untranslated quote): v2.14 성후니 변경 대만 로그 부하를 줄이기 위해 / 로그 버퍼에 100개의 로그가 쌓여 있던지 / 혹은 로그를 쓴지 10초가 지난경우 그리고 마지막 셧다운시는 무조건 저장한다.

## PutLogEventFileList - SRV Wmain.cpp:460-478

Appends to the special event log file LogEvents.log (Wmain.cpp:466).

- sig: `void PutLogEventFileList(char * cStr)`
- callers: CGame::DeleteClient, CGame::NpcKilledHandler, CGame::RequestInitDataHandler, CGame::RequestTeleportHandler

## PutDebugMsg - SRV Wmain.cpp:480-498

Appends a timestamped line to the file DebugMsg.log (Wmain.cpp:486); it does not use OutputDebugString.

- sig: `void PutDebugMsg(char * cStr)`
- callers: CGame::NpcDeadItemGeneratorWithItemEvent

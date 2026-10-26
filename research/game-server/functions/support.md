# Support: Wmain, small classes, Misc, StrTok, XSocket, Msg

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
`cfg:` means data loaded from that config, not a literal file open in the body [V, review G1].
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 87

## CBuildItem::CBuildItem - SRV BuildItem.cpp:11-31

Constructor: clears the name, sets item id -1, skill limit 0, and the six material slots (item id, count, value, index -1), plus max value, average value, max skill and attribute to 0.

- sig: `CBuildItem::CBuildItem()`

## CBuildItem::~CBuildItem - SRV BuildItem.cpp:33-36

Destructor: empty.

- sig: `CBuildItem::~CBuildItem()`

## CClient::CClient - SRV Client.cpp:11-302

Constructor (292 lines of initialisation): creates the client XSocket (block limit DEF_CLIENTSOCKETBLOCKLIMIT, buffer DEF_MSGBUFFERSIZE) and sets every player field to its default (empty names, profile "__________", location "NONE", guild rank/GUID -1, init-complete FALSE, LU_* 0, ...).

- sig: `CClient::CClient(HWND hWnd)`
- callers: CGame::bAccept
- callees: XSocket, bInitBufferSize
- Korean comments (UTF-8, first 6, untranslated quote): v1.432 사용하지 않는다. / 아이템 장착 상태 초기화한 후 설정한다. / 아이템 리스트 초기화

## CClient::~CClient - SRV Client.cpp:304-314

Destructor: deletes the XSocket and every inventory item (the warehouse list is not freed here) ([GSO-049](../../issues/game-server-other.md#gso-049-cclient-destructor-does-not-free-the-warehouse-list)).

- sig: `CClient::~CClient()`

## CDelayEvent::CDelayEvent - SRV DelayEvent.cpp:11-14

Constructor: empty.

- sig: `CDelayEvent::CDelayEvent()`

## CDelayEvent::~CDelayEvent - SRV DelayEvent.cpp:16-19

Destructor: empty.

- sig: `CDelayEvent::~CDelayEvent()`

## CDynamicObject::CDynamicObject - SRV DynamicObject.cpp:11-27

Constructor: stores owner, owner type, type, map index, x/y, register time, last time and V1; count 0.

- sig: `CDynamicObject::CDynamicObject(short sOwner, char cOwnerType, short sType, char cMapIndex, short sX, short sY, DWORD dwRegisterTime, DWORD dwLastTime, int iV1)`
- callers: CGame::iAddDynamicObjectList

## CDynamicObject::~CDynamicObject - SRV DynamicObject.cpp:29-32

Destructor: empty.

- sig: `CDynamicObject::~CDynamicObject()`

## CFish::CFish - SRV Fish.cpp:11-24

Constructor: stores map index, x/y, type, the catch item and difficulty (minimum 1); engaging count 0.

- sig: `CFish::CFish(char cMapIndex, short sX, short sY, short sType, class CItem * pItem, int iDifficulty)`
- callers: CGame::iCreateFish

## CFish::~CFish - SRV Fish.cpp:26-29

Destructor: deletes the catch item if still set.

- sig: `CFish::~CFish()`

## CGame::_bGetIsStringIsNumber - SRV Game.cpp:9966-9973

True if every character is a digit or '-' (a '-' is accepted anywhere, and the empty string counts as a number) (Game.cpp:9969-9972) ([GSO-046](../../issues/game-server-other.md#gso-046-_bgetisstringisnumber-accepts-a-minus-sign-anywhere-and-the-empty-string)).

- sig: `BOOL CGame::_bGetIsStringIsNumber(char * pStr)`
- callers: CGame::__bReadMapInfo, CGame::_bDecodeBuildItemConfigFileContents, CGame::_bDecodeDupItemIDFileContents, CGame::_bDecodeItemConfigFileContents, CGame::_bDecodeMagicConfigFileContents, CGame::_bDecodeNpcConfigFileContents, CGame::_bDecodeNpcItemConfigFileContents, CGame::_bDecodeOccupyFlagSaveFileContents, CGame::_bDecodePlayerDatafileContents, CGame::_bDecodePortionConfigFileContents, CGame::_bDecodeQuestConfigFileContents, CGame::_bDecodeSkillConfigFileContents, CGame::_bDecodeTeleportListConfigFileContents, CGame::bReadCrusadeStructureConfigFile

## CGame::iDice - SRV Game.cpp:20208-20221

Sum of iThrow rolls of 1..iRange (0 if range <= 0) (verified 20212-20220). The callers list below is cut at 25.

- sig: `int CGame::iDice(int iThrow, int iRange)`
- callers: CGame::AdminOrder_CreateItem, CGame::ApplyCombatKilledPenalty, CGame::ApplyPKpenalty, CGame::BuildItemHandler, CGame::CalculateGuildEffect, CGame::ChatMsgHandler, CGame::CheckUniqueItemEquipment, CGame::CreateCrusadeStructures, CGame::DeleteClient, CGame::DeleteNpc, CGame::DoMeteorStrikeDamageHandler, CGame::DynamicObjectEffectProcessor, CGame::Effect_Damage_Spot, CGame::Effect_Damage_Spot_DamageMove, CGame::Effect_HpUp_Spot, CGame::Effect_SpDown_Spot, CGame::Effect_SpUp_Spot, CGame::EnemyKillRewardHandler, CGame::EnergySphereProcessor, CGame::FishGenerator, CGame::FishProcessor, CGame::GetMapInitialPoint, CGame::InitPlayerData, CGame::MeteorStrikeHandler, CGame::MineralGenerator ...

## CGuildsMan::CGuildsMan - SRV GuildsMan.cpp:11-14

Constructor: clears the name.

- sig: `CGuildsMan::CGuildsMan()`

## CGuildsMan::~CGuildsMan - SRV GuildsMan.cpp:16-19

Destructor: empty.

- sig: `CGuildsMan::~CGuildsMan()`

## CItem::CItem - SRV Item.cpp:11-46

Constructor: clears the name and sets the item fields (sprite, effect values, etc.) to defaults.

- sig: `CItem::CItem()`

## CItem::~CItem - SRV Item.cpp:48-51

Destructor: empty.

- sig: `CItem::~CItem()`

## CMagic::CMagic - SRV Magic.cpp:11-15

Constructor: clears the name, attribute 0.

- sig: `CMagic::CMagic()`

## CMagic::~CMagic - SRV Magic.cpp:17-20

Destructor: empty.

- sig: `CMagic::~CMagic()`

## CMineral::CMineral - SRV Mineral.cpp:11-19

Constructor: stores type, map index, x/y and remaining count; difficulty 0.

- sig: `CMineral::CMineral(char cType, char cMapIndex, int sX, int sY, int iRemain)`
- callers: CGame::iCreateMineral

## CMineral::~CMineral - SRV Mineral.cpp:21-24

Destructor: empty.

- sig: `CMineral::~CMineral()`

## CMisc::CMisc - SRV Misc.cpp:11-14

Constructor: empty.

- sig: `CMisc::CMisc()`

## CMisc::~CMisc - SRV Misc.cpp:16-19

Destructor: empty.

- sig: `CMisc::~CMisc()`

## CMisc::GetMyCursorPos - SRV Misc.cpp:21-28

Reads the screen cursor position via GetCursorPos (Misc.cpp:21-28). No callers ([GSO-048](../../issues/game-server-other.md#gso-048-unused-helper-functions-in-cmisc)).

- sig: `void CMisc::GetMyCursorPos(short * pX, short * pY)`

## CMisc::cGetNextMoveDir - SRV Misc.cpp:31-55

Maps the sign of (sX-dX, sY-dY) to one of 8 directions (Misc.cpp:31-55); there is no tile check. The CGame overload (7 args) is a different function.

- sig: `char CMisc::cGetNextMoveDir(short sX, short sY, short dX, short dY)`
- callers (CMisc overload only [V]): CGame::cGetNextMoveDir (11454), CGame::NpcBehavior_Attack (11904, 11961, 12131), CGame::PlayerMagicHandler (22316). NpcBehavior_Move and NpcBehavior_Flee call only the CGame overload.

## CMisc::GetPoint - SRV Misc.cpp:58-132

Bresenham step: next point on the line from (x0,y0) to (x1,y1). Used only for NPC pathing (sole caller CGame::cGetNextMoveDir, 11452); linear spells and arrows use GetPoint2.

- sig: `void CMisc::GetPoint(int x0, int y0, int x1, int y1, int * pX, int * pY, int * pError)`
- callers: CGame::cGetNextMoveDir

## CMisc::GetPoint2 - SRV Misc.cpp:135-213

Variant of GetPoint returning the n-th point along the line.

- sig: `void CMisc::GetPoint2(int x0, int y0, int x1, int y1, int * pX, int * pY, int * pError, int iCount)`
- callers: CGame::NpcMagicHandler, CGame::PlayerMagicHandler

## CMisc::GetDirPoint - SRV Misc.cpp:216-229

Intended to offset a point one tile in a direction 1..8, but `*pY--` / `*pX++` step the pointer, not the value, so it changes nothing useful (Misc.cpp:219-226). No callers ([GSO-047](../../issues/game-server-other.md#gso-047-cmiscgetdirpoint-steps-the-pointers-instead-of-the-values)).

- sig: `void CMisc::GetDirPoint(char cDir, int * pX, int * pY)`

## CMisc::bEncode - SRV Misc.cpp:232-245

XOR string encoder; disabled by an early 'return TRUE' (no-op) ([GSO-002](../../issues/game-server-other.md#gso-002-cmiscbencode-and-bdecode-are-no-ops)).

- sig: `BOOL CMisc::bEncode(char cKey, char *pStr)`

## CMisc::bDecode - SRV Misc.cpp:248-261

XOR string decoder; disabled by an early 'return TRUE' (no-op) ([GSO-002](../../issues/game-server-other.md#gso-002-cmiscbencode-and-bdecode-are-no-ops)).

- sig: `BOOL CMisc::bDecode(char cKey, char *pStr)`
- callers: CGame::RequestInitDataHandler, CGame::RequestInitPlayerHandler

## CMisc::bCheckValidName - SRV Misc.cpp:264-296

Rejects names containing special characters.

- sig: `BOOL CMisc::bCheckValidName(char *pStr)`
- callers: CGame::_bDecodePlayerDatafileContents
- Korean comments (UTF-8, first 4, untranslated quote): 특수 문자가 들어가 있는 경우 거부 / 적합 / 적합

## CMisc::Temp - SRV Misc.cpp:299-369

One-off map-file converter: reads middleland.amd, middleland1.amd, middleland2.amd and writes middleland.amd.result (MAPSIZEX = 824 header) (Misc.cpp:299-369). Its only call is commented out (Game.cpp:523) ([GSO-048](../../issues/game-server-other.md#gso-048-unused-helper-functions-in-cmisc)).

- sig: `void CMisc::Temp()`
- Korean comments (UTF-8, first 5, untranslated quote): 기존 화일 위치 이동 / 새 파일 헤더 쓴다. / 새 파일 윗부분

## CMsg::CMsg - SRV Msg.cpp:11-15

Constructor: data pointer NULL, size 0.

- sig: `CMsg::CMsg()`

## CMsg::~CMsg - SRV Msg.cpp:17-20

Destructor: deletes the stored message copy.

- sig: `CMsg::~CMsg()`

## CMsg::bPut - SRV Msg.cpp:22-35

Stores a queued message copy (from, data, size, index, key).

- sig: `BOOL CMsg::bPut(char cFrom, char * pData, DWORD dwSize, int iIndex, char cKey)`
- callers: CGame::bPutMsgQuene, CGame::bReadNotifyMsgListFile

## CMsg::Get - SRV Msg.cpp:37-44

Returns a queued message's fields.

- sig: `void CMsg::Get(char * pFrom, char * pData, DWORD * pSize, int * pIndex, char * pKey)`
- callers: CGame::NoticeHandler, CGame::bGetMsgQuene

## CNpc::CNpc - SRV Npc.cpp:11-54

Constructor: initialises the NPC's fields to defaults.

- sig: `CNpc::CNpc(char * pName5)`
- callers: CGame::_bDecodeNpcConfigFileContents, CGame::bCreateNewNpc
- Korean comments (UTF-8, first 2, untranslated quote): v2.19 2002-12-16 농사 스킬 관련 / v2.19 2002-12-16 농사 스킬 관련

## CNpc::~CNpc - SRV Npc.cpp:56-59

Destructor: empty.

- sig: `CNpc::~CNpc()`

## CNpcItem::CNpcItem - SRV Npc.cpp:62-70

Constructor: clears the name, item id 0, first/second probability and target value 0.

- sig: `CNpcItem::CNpcItem()`
- callers: CGame::_bDecodeNpcItemConfigFileContents

## COccupyFlag::COccupyFlag - SRV OccupyFlag.cpp:11-20

Constructor: stores x/y, side, EK count and dynamic-object index.

- sig: `COccupyFlag::COccupyFlag(int dX, int dY, char cSide, int iEKNum, int iDOI)`
- callers: CMap::iRegisterOccupyFlag

## COccupyFlag::~COccupyFlag - SRV OccupyFlag.cpp:22-25

Destructor: empty.

- sig: `COccupyFlag::~COccupyFlag()`

## CPortion::CPortion - SRV Potion.cpp:11-21

Constructor: clears the name, skill limit and difficulty 0, the 12 ingredient slots -1.

- sig: `CPortion::CPortion()`

## CPortion::~CPortion - SRV Potion.cpp:23-26

Destructor: empty.

- sig: `CPortion::~CPortion()`

## CQuest::CQuest - SRV Quest.cpp:11-14

Constructor: empty.

- sig: `CQuest::CQuest()`

## CQuest::~CQuest - SRV Quest.cpp:16-19

Destructor: empty.

- sig: `CQuest::~CQuest()`

## CSkill::CSkill - SRV Skill.cpp:11-14

Constructor: clears the name.

- sig: `CSkill::CSkill()`

## CSkill::~CSkill - SRV Skill.cpp:16-19

Destructor: empty.

- sig: `CSkill::~CSkill()`

## CStrTok::CStrTok - SRV StrTok.cpp:12-19

Constructor: stores the data and separator pointers, the data length and cursor 0.

- sig: `CStrTok::CStrTok(char * pData, char * pSeps)`
- callers: CGame::AdminOrder_CallGuard, CGame::AdminOrder_CheckIP, CGame::AdminOrder_CloseConn, CGame::AdminOrder_CreateFish, CGame::AdminOrder_CreateItem, CGame::AdminOrder_GetNpcStatus, CGame::AdminOrder_Polymorph, CGame::AdminOrder_ReserveFightzone, CGame::AdminOrder_SetAttackMode, CGame::AdminOrder_SetForceRecallTime, CGame::AdminOrder_SetInvi, CGame::AdminOrder_Summon, CGame::AdminOrder_SummonPlayer, CGame::AdminOrder_Teleport, CGame::CheckAndNotifyPlayerConnection, CGame::GetPlayerProfile, CGame::RequestJoinPartyHandler, CGame::SetPlayerReputation, CGame::SetSummonMobAction, CGame::ShutUpPlayer, CGame::ToggleWhisperPlayer, CGame::UserCommand_BanGuildsman, CGame::__bReadMapInfo, CGame::_bDecodeBuildItemConfigFileContents, CGame::_bDecodeDupItemIDFileContents ...

## CStrTok::~CStrTok - SRV StrTok.cpp:21-24

Destructor: empty.

- sig: `CStrTok::~CStrTok()`

## CStrTok::pGet - SRV StrTok.cpp:26-67

Returns the next token using the separator set (cfg parsing).

- sig: `char * CStrTok::pGet()`
- callers: CGame::AdminOrder_CallGuard, CGame::AdminOrder_CheckIP, CGame::AdminOrder_CloseConn, CGame::AdminOrder_CreateFish, CGame::AdminOrder_CreateItem, CGame::AdminOrder_GetNpcStatus, CGame::AdminOrder_Polymorph, CGame::AdminOrder_ReserveFightzone, CGame::AdminOrder_SetAttackMode, CGame::AdminOrder_SetForceRecallTime, CGame::AdminOrder_SetInvi, CGame::AdminOrder_Summon, CGame::AdminOrder_SummonPlayer, CGame::AdminOrder_Teleport, CGame::CheckAndNotifyPlayerConnection, CGame::GetPlayerProfile, CGame::RequestJoinPartyHandler, CGame::SetPlayerReputation, CGame::SetSummonMobAction, CGame::ShutUpPlayer, CGame::ToggleWhisperPlayer, CGame::UserCommand_BanGuildsman, CGame::__bReadMapInfo, CGame::_bDecodeBuildItemConfigFileContents, CGame::_bDecodeDupItemIDFileContents ...
- callees: _bIsSeperator
- Korean comments (UTF-8, first 2, untranslated quote): separator가 아니면 토큰을 작성 / 다음 토큰까지 포인터를 이동시킨후 토큰을 반환한다.

## CStrTok::_bIsSeperator - SRV StrTok.cpp:69-82

Separator test: NUL, CR LF, or any character in m_pSeps (StrTok.cpp:69-82). There is no quote handling.

- sig: `BOOL CStrTok::_bIsSeperator(char cData, char cNextData)`
- callers: CStrTok::pGet

## CStrategicPoint::CStrategicPoint - SRV StrategicPoint.cpp:11-16

Constructor: x, y and side to 0.

- sig: `CStrategicPoint::CStrategicPoint()`

## CStrategicPoint::~CStrategicPoint - SRV StrategicPoint.cpp:18-21

Destructor: empty.

- sig: `CStrategicPoint::~CStrategicPoint()`

## CStructure::CStructure - SRV Structure.cpp:11-14

Constructor: empty (Structure.cpp is not built).

- sig: `CStructure::CStructure()`

## CStructure::~CStructure - SRV Structure.cpp:16-19

Destructor: empty (Structure.cpp is not built).

- sig: `CStructure::~CStructure()`

## CTeleport::CTeleport - SRV Teleport.cpp:11-25

Constructor: clears the NPC and map names, position -5, cost and level range 0, side 0 (all), hunter/neutral/criminal permissions TRUE.

- sig: `CTeleport::CTeleport()`

## CTeleport::~CTeleport - SRV Teleport.cpp:27-30

Destructor: empty.

- sig: `CTeleport::~CTeleport()`

## CTeleportLoc::CTeleportLoc - SRV TeleportLoc.cpp:11-29

Constructor: clears the destination map name; source/destination coordinates -1; V1/V2, times and summon count 0.

- sig: `CTeleportLoc::CTeleportLoc()`

## CTeleportLoc::~CTeleportLoc - SRV TeleportLoc.cpp:31-34

Destructor: empty.

- sig: `CTeleportLoc::~CTeleportLoc()`

## CTile::CTile - SRV Tile.cpp:11-39

Constructor: tile defaults (move allowed, not teleport/water/farm, no owner or dead owner, empty item stack, no dynamic object, temp move allowed, occupy and attribute 0).

- sig: `CTile::CTile()`
- Korean comments (UTF-8, first 1, untranslated quote): v2.19 2002-12-16 농사 스킬 관련

## CTile::~CTile - SRV Tile.cpp:41-46

Destructor: deletes every item on the tile.

- sig: `CTile::~CTile()`

## ThreadProc - SRV Wmain.cpp:58-66

Never started and effectively dead: the loop condition `if (G_pGame = NULL) G_pGame->OnTimer(NULL);` is an assignment (Wmain.cpp:61), and no thread is created anywhere. The OnTimer callee is a name-scan artefact ([GSO-040](../../issues/game-server-other.md#gso-040-threadproc-never-runs)).

- sig: `void ThreadProc(void *ch)`
- callees: none live

## WndProc - SRV Wmain.cpp:68-139

Window procedure: routes WM_USER socket messages (accept, client, log, sub-log, gate) and keys/timer/paint to CGame.

- sig: `LRESULT CALLBACK WndProc( HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam )`
- callees: OnAccept, OnClientSocketEvent, OnDestroy, OnGateSocketEvent, OnKeyDown, OnKeyUp, OnMainLogSocketEvent, OnPaint, OnStartGameSignal, OnSubLogSocketEvent, OnTimer, bOnClose

## WinMain - SRV Wmain.cpp:143-153

Entry point: InitApplication, InitInstance, Initialize, EventLoop.

- sig: `int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )`
- callees: EventLoop, InitApplication, InitInstance, Initialize

## InitApplication - SRV Wmain.cpp:157-173

Registers the window class (WndProc, arrow cursor, window background).

- sig: `BOOL InitApplication( HINSTANCE hInstance)`
- callers: WinMain

## InitInstance - SRV Wmain.cpp:176-214

Creates and shows the 800x600 popup window titled "Helbreath GameServer V<upper>.<lower> <build> (Executed at: month day hour)" built from Version.h (Wmain.cpp:189-202).

- sig: `BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )`
- callers: WinMain
- Korean comments (UTF-8, first 1, untranslated quote): 서버 부팅시간 기록

## EventLoop - SRV Wmain.cpp:218-234

Message pump.

- sig: `int EventLoop()`
- callers: WinMain
- callees: UpdateScreen

## Initialize - SRV Wmain.cpp:238-263

Creates CGame, calls bInit; if bInit fails it logs "(!!!) STOPPED!" and returns without a timer or listen socket (Wmain.cpp:248-251). Otherwise starts the 300 ms multimedia timer (Wmain.cpp:254), opens the listen socket on the game port. GServer.cfg is read only through bInit.

- sig: `void Initialize()`
- callers: WinMain
- callees: CGame, PutLogList, XSocket, _InitWinsock, _StartTimer, bInit, bListen
- cfg: GServer.cfg
- Korean comments (UTF-8, first 2, untranslated quote): 게임 진행용 타이머 / v2.14 성후니 변경 대만 로그 부하 줄이기 위해

## OnDestroy - SRV Wmain.cpp:267-283

Deletes the listen and log sockets, calls Quit and deletes the game, stops the timer, WSACleanup, closes pLogFile and posts quit (Wmain.cpp:267-283).

- sig: `void OnDestroy()`
- callers: WndProc
- callees: Quit, _StopTimer, _TermWinsock

## XSocket::XSocket - SRV XSocket.cpp:11-40

Constructor: no buffers, invalid socket, header-reading state (3 bytes), empty unsent-data queue, stores the window handle and block limit.

- sig: `XSocket::XSocket(HWND hWnd, int iBlockLimit)`
- callers: CClient::CClient, CGame::OnGateSocketEvent, CGame::OnSubLogSocketEvent, CGame::OnTimer, CGame::SendMsgToGateServer, CGame::bAccept, CGame::bInit, CGame::bSendMsgToLS, Initialize

## XSocket::~XSocket - SRV XSocket.cpp:42-54

Destructor: frees the buffers and unsent blocks, then closes the connection.

- sig: `XSocket::~XSocket()`
- callees: _CloseConn
- Korean comments (UTF-8, first 1, untranslated quote): 소켓을 마저 읽고 닫는다.

## XSocket::bInitBufferSize - SRV XSocket.cpp:56-70

Allocates send and receive buffers of size+8 each (XSocket.cpp:61-64). Every caller passes DEF_MSGBUFFERSIZE = 30000 (Client.h:24; Client.cpp:17, Game.cpp:801, 810, 3732, 4775, 20591, 20751, 42448). The 8192*5 value is the SO_RCVBUF/SO_SNDBUF socket option (XSocket.cpp:155, 507), not this buffer.

- sig: `BOOL XSocket::bInitBufferSize(DWORD dwBufferSize)`
- callers: CClient::CClient, CGame::OnGateSocketEvent, CGame::OnSubLogSocketEvent, CGame::OnTimer, CGame::SendMsgToGateServer, CGame::bInit, CGame::bSendMsgToLS

## XSocket::iOnSocketEvent - SRV XSocket.cpp:72-118

Translates WSAAsyncSelect FD_* events into DEF_XSOCKEVENT_* codes; drives read/send-unsent/connect/close.

- sig: `int XSocket::iOnSocketEvent(WPARAM wParam, LPARAM lParam)`
- callers: CGame::OnClientSocketEvent, CGame::OnGateSocketEvent, CGame::OnMainLogSocketEvent, CGame::OnSubLogSocketEvent
- callees: _iOnRead, _iSendUnsentData, bConnect
- Korean comments (UTF-8, first 5, untranslated quote): 리스닝 소켓의 이벤트는 처리할 수 없다. / 초기화 되지 않아서 처리할 수 없다. / 이 소켓의 접속이 실패했으므로 재접속을 시도한다.

## XSocket::bConnect - SRV XSocket.cpp:120-167

Non-blocking connect with WSAAsyncSelect registration.

- sig: `BOOL XSocket::bConnect(char * pAddr, int iPort, unsigned int uiMsg)`
- callers: CGame::OnGateSocketEvent, CGame::OnSubLogSocketEvent, CGame::OnTimer, CGame::SendMsgToGateServer, CGame::bInit, CGame::bSendMsgToLS, XSocket::iOnSocketEvent
- Korean comments (UTF-8, first 4, untranslated quote): 리스닝 소켓으로 초기화된 클래스는 이 함수를 사용할 수 없다. / 소켓을 논블록킹 모드로 / 주소를 바인드한다.

## XSocket::_iOnRead - SRV XSocket.cpp:169-254

Two-phase framed read: 3-byte header [key(1), total size WORD incl. header] then body (XSocket.cpp:169-254). It does not decrypt; decryption happens in pGetRcvDataPointer (553-559).

- sig: `int XSocket::_iOnRead()`
- callers: XSocket::iOnSocketEvent
- Korean comments (UTF-8, first 6, untranslated quote): 소켓이 닫혔다. / 헤더를 다 읽었다. / 읽어야 할 몸체 사이즈를 계산한다.

## XSocket::_iSend - SRV XSocket.cpp:258-320

send() loop; queues the remainder as unsent data on WSAEWOULDBLOCK (up to the block limit).

- sig: `int XSocket::_iSend(char * cData, int iSize, BOOL bSaveFlag)`
- callers: XSocket::iSendMsg
- callees: _iRegisterUnsentData
- Korean comments (UTF-8, first 6, untranslated quote): 만약 대기열에 데이터가 남아 있고 꼭 보내야 하는 데이터라면 / 메시지의 순서를 맞추기 위해 무조건 대기열에 저장해야 한다. / 할당할 메모리가 없다. 이경우는 복구할 방법이 없다.

## XSocket::_iSend_ForInternalUse - SRV XSocket.cpp:324-348

send() used when flushing unsent data.

- sig: `int XSocket::_iSend_ForInternalUse(char * cData, int iSize)`
- callers: XSocket::_iSendUnsentData
- Korean comments (UTF-8, first 2, untranslated quote): 소켓에 에러가 발생했다. / 블럭상태이면 더이상 보낼 수 없으므로 지금까지 보낸 데이터 사이즈만을 반환

## XSocket::_iRegisterUnsentData - SRV XSocket.cpp:352-370

Queues an unsent block in a circular list that wraps at m_iBlockLimit (XSocket.cpp:367, 392): 15 for client sockets (DEF_CLIENTSOCKETBLOCKLIMIT, Client.h:22), 300 for server sockets (Game.h:51). DEF_XSOCKBLOCKLIMIT is only the array size (XSocket.cpp:26); the wrap that used it is commented out.

- sig: `int XSocket::_iRegisterUnsentData(char * cData, int iSize)`
- callers: XSocket::_iSend
- Korean comments (UTF-8, first 4, untranslated quote): 큐가 가득차서 더이상 데이터를 대기열에 저장할 수 없다. / 메모리 할당에 실패했다. / 데이터 저장

## XSocket::_iSendUnsentData - SRV XSocket.cpp:374-411

Flushes queued blocks on FD_WRITE.

- sig: `int XSocket::_iSendUnsentData()`
- callers: XSocket::iOnSocketEvent
- callees: _iSend_ForInternalUse
- Korean comments (UTF-8, first 5, untranslated quote): 가능한 한 대기열의 데이터를 보낸다. / Head큐의 데이터를 다 보냈다.	다음 데이터를 보낸다. / 헤드 포인터 증가

## XSocket::iSendMsg - SRV XSocket.cpp:414-446

Frames a message: [key][WORD size+3][body]; if key != 0 each body byte b[i] += (i ^ key); b[i] ^= (key ^ (size - i)).

- sig: `int XSocket::iSendMsg(char * cData, DWORD dwSize, char cKey)`
- callers: CGame::AdminOrder_CreateItem, CGame::ChatMsgHandler, CGame::ChatMsgHandlerGSM, CGame::CheckConnectionHandler, CGame::CheckSpecialEventThirdYear, CGame::FightzoneReserveHandler, CGame::GetFightzoneTicketHandler, CGame::GetHeroMantleHandler, CGame::GetOccupyFlagHandler, CGame::GetRewardMoneyHandler, CGame::GiveItemHandler, CGame::InitPlayerData, CGame::PlayerMagicHandler, CGame::ReqCreatePortionHandler, CGame::ReqRepairItemCofirmHandler, CGame::ReqSellItemConfirmHandler, CGame::RequestChargedTeleportHandler, CGame::RequestCivilRightHandler, CGame::RequestCreateNewGuildHandler, CGame::RequestFullObjectData, CGame::RequestInitDataHandler, CGame::RequestNoticementHandler, CGame::RequestPurchaseItemHandler, CGame::RequestRetrieveItemHandler, CGame::RequestStudyMagicHandler ...
- callees: _iSend
- Korean comments (UTF-8, first 5, untranslated quote): 메시지 크기가 버퍼보다 크면 보낼 수 없다. / 리스닝 소켓 혹은 닫힌 소켓으로 메시지를 보낼 수는 없다. / 초기화 되지 않아서 메시지를 보낼 수 없다.

## XSocket::bListen - SRV XSocket.cpp:448-481

Creates, binds and listens a socket with async accept.

- sig: `BOOL XSocket::bListen(char * pAddr, int iPort, unsigned int uiMsg)`
- callers: Initialize
- Korean comments (UTF-8, first 2, untranslated quote): 소켓을 생성한다. / 주소를 바인드한다.

## XSocket::bAccept - SRV XSocket.cpp:483-512

Only accept()s a connection onto the passed XSocket, registers WSAAsyncSelect for FD_READ|FD_WRITE|FD_CLOSE and sets 40960-byte (8192*5) socket buffer options (XSocket.cpp:483-512). The slot search, shutdown/WLserver rejection and IP logging are in CGame::bAccept (Game.cpp:399-464).

- sig: `BOOL XSocket::bAccept(class XSocket * pXSock, unsigned int uiMsg)`
- callers: CGame::bAccept (Game.cpp:431, 460); OnAccept calls CGame::bAccept, not this
- Korean comments (UTF-8, first 3, untranslated quote): 클라이언트의 접속을 받는다 . / Accept된 소켓은 보통 소켓으로 초기화 된다. / 소켓 옵션을 조정한다.

## XSocket::_CloseConn - SRV XSocket.cpp:514-532

Closes the socket and marks it closed.

- sig: `void XSocket::_CloseConn()`
- callers: XSocket::~XSocket

## XSocket::iGetSocket - SRV XSocket.cpp:534-537

Returns the raw SOCKET.

- sig: `SOCKET XSocket::iGetSocket()`

## XSocket::pGetRcvDataPointer - SRV XSocket.cpp:539-561

Returns the decoded body pointer, size and key of the last message.

- sig: `char * XSocket::pGetRcvDataPointer(DWORD * pMsgSize, char * pKey)`
- callers: CGame::OnClientRead, CGame::OnGateRead, CGame::OnMainLogRead, CGame::OnSubLogRead
- Korean comments (UTF-8, first 2, untranslated quote): 헤더크기는 제외해서 반환한다. / v.14 : m_pSndBuffer +3 부터 dwSize까지 cKey가 0이 아니라면 암호화를 푼다.

## _InitWinsock - SRV XSocket.cpp:567-579

WSAStartup asking for Winsock 2.2 (`MAKEWORD(2, 2)`, XSocket.cpp:574). The 'Socket 1.1 not found' message is printed by Initialize (Wmain.cpp:242), not here.

- sig: `BOOL _InitWinsock()`
- callers: Initialize
- Korean comments (UTF-8, first 1, untranslated quote): 소켓의 버젼을 체크한다.

## _TermWinsock - SRV XSocket.cpp:582-585

WSACleanup.

- sig: `void _TermWinsock()`
- callers: OnDestroy

## XSocket::iGetPeerAddress - SRV XSocket.cpp:589-599

getpeername into a dotted string.

- sig: `int XSocket::iGetPeerAddress(char * pAddrString)`
- callers: CGame::_bItemLog, CGame::bAccept, CGame::bSendMsgToLS

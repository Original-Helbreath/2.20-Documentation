# Startup, config loading and the main loop

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
`cfg:` means data loaded from that config (often via the Log server), not a literal file open in the body [V, review G1].
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 13

## CGame::CGame - SRV Game.cpp:268-392

Constructor: zeroes every list pointer and stores the window handle; real init is bInit.

- sig: `CGame::CGame(HWND hWnd)`
- callers: Initialize
- cfg: none (the constructor only initialises members; no file reads or file-name literals, Game.cpp:268-392)

## CGame::~CGame - SRV Game.cpp:394-397

Destructor; empty in practice (cleanup happens in bInit/Quit).

- sig: `CGame::~CGame()`

## CGame::bInit - SRV Game.cpp:514-870

Full (re)initialisation: frees and nulls all lists, seeds construction-point table, builds the level-exp table (1..299) and skill-SSN table (0..100), reads GServer.cfg, opens main-log and gate sockets, reads notice.txt, primes the gate stock-message buffer, resets crusade state. Also used for auto-reboot.

- sig: `BOOL CGame::bInit()`
- callers: CGame::OnTimer (3799, auto reboot, ShutDownCode 3/4), Initialize (Wmain.cpp:248). `_bRegisterMap` calls the overload CMap::bInit(char*) (Game.cpp:5512), not this one.
- callees: PutLogList, XSocket, _iCalcSkillSSNpoint, bConnect, bInitBufferSize, bReadNotifyMsgListFile, bReadProgramConfigFile, iDice, iGetLevelExp
- messages: MSGID_SERVERSTOCKMSG
- types: DEF_MSGTYPE_CONFIRM
- cfg: GServer.cfg (791), notice.txt (834) only; Item/Magic/Npc/Potion/Quest/Skill/BuildItem/Crusade/TeleportList arrive later via the log server
- switches: DEF_MAXLEVEL150 (inactive; block 773-785 compiled out)
- Korean comments (UTF-8, first 6, untranslated quote): 일단 여기서 설정 / 2002-12-6  Teleport 기능 추가 / dwTime - 남은 예약 가능한 시간

## CGame::GameProcess - SRV Game.cpp:5045-5050

Per-timer-tick core: NpcProcess, MsgProcess, DelayEventProcess.

- sig: `void CGame::GameProcess()`
- callers: CGame::OnTimer
- callees: DelayEventProcess, MsgProcess, NpcProcess

## CGame::bReadProgramConfigFile - SRV Game.cpp:5202-5352

Parses GServer.cfg keyword by keyword (5202-5352): game-server-name (<=10 chars), game-server-port, log-server-address (<=15), internal-log-server-port, game-server-map (<=10, -> _bRegisterMap), gate-server-address, gate-server-port. `[CONFIG]`/`[MAPS]` tags and a `game-server-address` key are ignored; the address comes from gethostbyname (v2.04).

- sig: `BOOL CGame::bReadProgramConfigFile(char * cFn)`
- callers: CGame::bInit
- callees: CStrTok, PutLogList, _bRegisterMap, pGet
- cfg: GServer.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 게임서버의 초기화 파일을 읽을 수 없다. / 서버의 이름이 너무 길다. / v2.04 게임서버 IP 자동으로 인식

## CGame::_bRegisterMap - SRV Game.cpp:5488-5540

Creates a CMap for a map name if not already registered, calls CMap::bInit (loads mapdata/<name>.amd); records Aresden/Elvine/Middleland indices. It does not call __bReadMapInfo (that happens in OnStartGameSignal via _bReadMapInfoFiles, 20228-20230). A duplicate map returns FALSE with a critical error.

- sig: `BOOL CGame::_bRegisterMap(char * pName)`
- callers: CGame::bReadProgramConfigFile
- callees: CMap, PutLogList, bInit
- Korean comments (UTF-8, first 6, untranslated quote): 먼저 이미 등록된 맵인지 검색한다. / 이미 등록된 맵이다. / 등록되어 있지 않은 맵이므로 등록한다.

## CGame::_bReadMapInfoFiles - SRV Game.cpp:9976-9983

Thin wrapper calling __bReadMapInfo for one map index.

- sig: `BOOL CGame::_bReadMapInfoFiles(int iMapIndex)`
- callers: CGame::OnStartGameSignal
- callees: __bReadMapInfo

## CGame::OnStartGameSignal - SRV Game.cpp:20223-20247

Runs once everything is loaded: spawns map NPCs from the map info, reads Crusade.cfg structures, links strike points (_LinkStrikePointMapIndex), reads GameData\CrusadeGUID.txt and WarSchedule.cfg (bReadCrusadeScheduleConfigFile), logs 'Game server activated', then creates the gold item (`m_pGold = new CItem; _bInitItemAttr(m_pGold,"Gold")`, 20244-20245). No cfg literal is listed because the file names are call arguments (Crusade.cfg, WarSchedule.cfg).

- sig: `void CGame::OnStartGameSignal()`
- callers: WndProc
- callees: PutLogList, _LinkStrikePointMapIndex, _bInitItemAttr, _bReadMapInfoFiles, bReadCrusadeGUIDFile, bReadCrusadeScheduleConfigFile, bReadCrusadeStructureConfigFile
- Korean comments (UTF-8, first 3, untranslated quote): 맵 이벤트 화일을 읽어 각각의 맵에 할당된 NPC들을 세팅한다. / 초기 설치 건물 정보 읽는다. / 폭격 포인트 정보를 바탕으로 맵 인덱스를 링크시킨다.

## CGame::_bDecodeWLServerConfigFileContents - SRV Game.cpp:25019-25066

Decodes the WLserver contents (MSGID_WLSERVERFILECONTENTS, DEF_DEFENCEHACKING): world-server-port -> m_iWorldLogServerPort; gates start-up (sets m_bIsWLServerAvailable, MsgProcess 15240). The wsprintf at 25041 after the `break` is unreachable.

- sig: `BOOL CGame::_bDecodeWLServerConfigFileContents(char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: CStrTok, PutLogList, pGet
- cfg: WLserver cfg

## CGame::_bDecodeWorldConfigFileContents - SRV Game.cpp:25070-25141

Decodes the world config pushed by the Log server (MSGID_WORLDCONFIGCONTENTS): world-server-max-user, world-server-max-level (m_iPlayerMaxLevel), world-server-forcerecall-time.

- sig: `BOOL CGame::_bDecodeWorldConfigFileContents(char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: CStrTok, PutLogList, pGet
- cfg: World cfg
- Korean comments (UTF-8, first 1, untranslated quote): v2.17 2002-7-15 월드 서버에서 강콜시간을 수정할 수 있게 한다.

## CGame::Quit - SRV Game.cpp:27115-27181

Shutdown: stops the worker thread, frees every list and socket.

- sig: `void CGame::Quit()`
- callers: OnDestroy
- cfg: none (27115-27181 only frees lists, the DupItemID list, m_pNoticementData and m_pGold)
- Korean comments (UTF-8, first 1, untranslated quote): 쓰레드를 죽인다.

## CGame::bReadNotifyMsgListFile - SRV Game.cpp:36258-36320

Reads notice.txt (`notify_msg = ...` lines, separators "=\t\n;") into m_pNoticeMsgList[].

- sig: `BOOL CGame::bReadNotifyMsgListFile(char * cFn)`
- callers: CGame::bInit
- callees: CStrTok, PutLogList, bPut, pGet
- cfg: notice.txt

## CGame::_bDecodeNoticementFileContents - SRV Game.cpp:46031-46050

Stores the noticement text pushed by the Log server for RequestNoticementHandler.

- sig: `void CGame::_bDecodeNoticementFileContents(char *pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: PutLogList

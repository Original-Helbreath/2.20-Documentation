# game-server review, part 3 (persistence, timers, ui-logging, support, build-config, index)

Reviewer: read-only verification against `.claude/scratch/srv220/` (cp949, errors=replace). Nothing in the docs was edited.
Status words: verified / failed / unverified. Method per check is stated where it is not a plain read of the source.
Scratch scripts used: `.claude/scratch/verify3/*.py` (range, caller, callee, define scans).

## Headline checks

| Check | Result |
|---|---|
| Version.h: v2.20 source reports as v2.19 build 429 | verified. GlobalDef.h:16 defines DEF_CHINESE; DEF_INTERSERVER/DEF_USA/DEF_KOREA are all commented out (GlobalDef.h:14,18,19); Version.h:19-22 then gives "2"."19" and 429. The window title is built from it at Wmain.cpp:189 ("Helbreath GameServer V2.19 429 ..."); Game.cpp:20708 sends DEF_BUILDDATE. .dsp has no /D flags beyond WIN32, NDEBUG, _WINDOWS, so nothing else changes this. |
| 547 live function definitions | verified (measured). Two independent counts (extract.py's funcs.json and a separate col-0 header scan) give the same 547 line numbers. |
| 543 active | verified (measured). The 4 inactive: Game.cpp:6945 and 8917 (file-mode pair, `#else` of DEF_DBGAMESERVER) and 42474 and 42586 (DEF_TAIWANLOG). |
| Every `## name - SRV file:a-b` range in the 4 function docs (122 entries) | verified. Name is on line a and brace matching from line a closes on line b for all 122. |
| All 547 functions are documented exactly once across the 14 function docs; each "Functions: N" header equals its heading count | verified |
| Dead-code list in build-config.md | verified for the 7 listed functions, with one omission (see row B20) |
| "Commit 1697e41" in the index | unverified (no git metadata in the export) |

## Findings

Severity: wrong = contradicts the code; imprecise = true but misleading, vague or unsupported; missing = something present in the code the doc omits.

### build-config.md

| # | Entry | What is wrong | What the code shows | Severity |
|---|---|---|---|---|
| B1 | DEF_CHINESE row: "Turns on the six switches below" | Count is wrong. Eight switches follow. | GlobalDef.h:65-70 (CANDYEVENT, CHINESEGUILDLIMIT, NPCITEM) plus GlobalDef.h:72-78, shared with JAPAN/USA (DBGAMESERVER, LOCALNPCNAME, ENGLISHITEMNAME, LOGTIME, DEFENCEHACKING) = 8. The doc's own table has 8 "via CHINESE" rows. | wrong |
| B2 | Compiled-out table: `DEF_NO_MSGSPEEDCHECK` | Wrong macro name. | The macro is `NO_MSGSPEEDCHECK` with no DEF_ prefix: GlobalDef.h:6, tested `#ifndef NO_MSGSPEEDCHECK` at Game.cpp:1149 and 11304. Anyone grepping for the doc's name finds nothing. | wrong |
| B3 | DEF_ENGLISHITEMNAME: "Defined but not tested in any .cpp" | Misleading. It does have an effect, in headers. | englishitem.h:1 `#ifdef DEF_ENGLISHITEMNAME` and koreaitem.h:1 `#ifndef DEF_ENGLISHITEMNAME` select which DEF_ITEMNAME_* string macros exist (e.g. "Arrow", "ArenaTicket(%d)"); both are included at Game.h:47-48. With CHINESE on, the English names are used. The "[V by grep]" tag only covers .cpp files. | wrong |
| B4 | DEF_CONSULTATION: "Enables DEF_PKPENALTY, DEF_SAMESIDETOWNPROTECT and DEF_ITEMDROP (44749, 52736)" | The cited sites are CONSULTATION's own tests, not those of the three it enables. Also omits that only DEF_KOREA defines it. | Enabling: GlobalDef.h:82-87 (DEF_ALLPLAYERPROTECT stays commented, line 83). Own sites: Game.cpp:44749 (`#ifdef ... #else` 44801 ... `#endif` 44880, a drop switch) and 52736 (`#ifndef`, force-recall times 20*3 vs 20*1). Defined only at GlobalDef.h:28 inside `#ifdef DEF_KOREA`. | imprecise |
| B5 | DEF_TESTSERVER: "28 sites: exp x70/x30, skill x100, boosted drops" | The count is right (26 in Game.cpp, Game.h:186, GlobalDef.h:41) but 16 of the Game.cpp sites are `#ifndef DEF_TESTSERVER`, i.e. live code in the current build that vanishes when TESTSERVER is on. The doc reads as if all 28 add behaviour. | `#ifndef` at Game.cpp:4027 (hourly 300000-exp speed-hack check and ban), 10945, 10959, 10974, 36089, 36466, 36549, 36661, 44018, 44029, 44082, 44112, 44227, 44324, 44495, 44625 (all "admin command enabled" gates). Additive: 28715 and 28865 (skill SSN x100), 38435 (skill level =100), 44675 (drop probability -3000), 44712 (item count x10), 47313 (NPC drop table), 51748 (party exp x30), 51765 and 51771 (exp x70), 903 (banner). Also turns on DBGAMESERVER (GlobalDef.h:41-42). | imprecise |
| B6 | DEF_GUILDWARMODE: "Init/teleport (2150, 23756)" | Direction reversed for those two sites. They are `#ifndef`: the fightzone force-recall block is live now and is removed if the mode is on. Display line 933 not listed. | Game.cpp:2150 and 23756 `#ifndef DEF_GUILDWARMODE`; blocks magic at 20853 (`#ifdef`), exchange 40839, special abilities 47124; banner 933. | imprecise |
| B7 | DEF_KOREA: "Korea rules (16387)" | Vague, and the site is `#ifndef`. Omits what KOREA pulls in. | Game.cpp:16387 `#ifndef DEF_KOREA` drops the admin-level check on the appr4 0x0001 flag. GlobalDef.h:27-31 sets CONSULTATION, DEFENCEHACKING, CHINESECANDYEVENT. Version.h:14-17 gives 2.20 build 207. | imprecise |
| B8 | DEF_V219: "Hunter mode (52784, 52827, 52897)" | 52784 and 52827 are `#ifndef DEF_V219 return;` guards (inert while V219 is on). "Party exp table" has no site. | Game.cpp:52784-52786, 52827-52829 (`#ifndef`), 52897 (`#ifdef`), 51708 (party exp, `#ifdef`). | imprecise |
| B9 | DEF_CHINESEGUILDLIMIT: "level 100 and 1,000,000 gold" | Incomplete. Also deducts the gold; the charisma >= 20 rule applies in both builds. | Game.cpp:16566 (level >= 100 and charisma >= 20, else level >= 20), 16582-16587 (Gold -1000000 via SetItemCount), 16667-16673 (also rejects neutral, hunter, PK-zone, gold < 1000000). | imprecise |
| B10 | DEF_JAPAN, DEF_USA: "Region variants [I]" | Not just inference. Their effects are readable. | JAPAN's only effect is joining the DBGAMESERVER group (GlobalDef.h:72). USA: GlobalDef.h:47-51 (SAMESIDETOWNPROTECT, MAXLEVEL150) plus line 72 group; Version.h:9-12 gives 2.20 build 222. | imprecise |
| B11 | DEF_INTERSERVER: "Status display only (927)" | Also drives a define block and the version number. | GlobalDef.h:33-39 (MAXLEVEL150, DBGAMESERVER, DEFENCEHACKING, ENGLISHITEMNAME, LOCALNPCNAME); Version.h:4-7 gives 2.20 build 306. | imprecise |
| B12 | DEF_MAXLEVEL150: "levels 140-180" | Loop covers 140..179. | Game.cpp:775 `for (i = 140; i < 180; i++)`, remap by (i-140)*3 at 776-778. Also set by INTERSERVER and USA. | imprecise |
| B13 | DEF_ITEMDROPRATINGUP: "Higher drop rate" | It is a value macro that must carry a number; defined bare it would not compile. Currently commented with 1500 (USA) and 3000 (CHINESE). | Game.cpp:44678-44679 `iItemprobability -= DEF_ITEMDROPRATINGUP;`; banner 946; GlobalDef.h:50 and 69. | imprecise |
| B14 | DEF_NPCITEM: "NpcItem config drops on DeleteNpc (47193)" | The guarded function is bGetItemNameWhenDeleteNpc, not DeleteNpc. Branch is `#if defined ... #else` (47260) `#endif` (47548). | Game.cpp:47187-47193. | imprecise |
| B15 | Site lists claimed "every ... sites" (index line too) | Several sites missing. | DEF_CHINESE banner Game.cpp:921. DEF_DBGAMESERVER banner 953. DEF_LOGTIME: only the effect is given; sites 16888, 30318, 45588, 45709 are not. DEF_BEGINNERTOWNPROTECT: 31208 and 31765 missing (only 12592 given). Banner lines for BALLEVENT 909, INTERSERVER 927, GUILDWARMODE 933, BEGINNEREVENTMODE 939, ITEMDROPRATINGUP 946 missing. | missing |
| B16 | DEF_TAIWAN row | Sites 894 and 915 (banner and status line) missing. Header effects missing. DEF_TAIWANLOG has about 28 `#ifdef` sites, not described. | Game.h:160 (DEF_MAXCONSTRUCTNUM 7 vs 10), Game.h:220 (DEF_MAX_CRUSADESUMMONMOB 5 vs 500; used at Game.cpp:48837 and 48864). TAIWAN also sets DEF_TAIWANLOG, `#undef DEF_WINTER`, DBGAMESERVER, LOCALNPCNAME, ENGLISHITEMNAME, LOGTIME (GlobalDef.h:54-63). | missing |
| B17 | No transitive define chains | The GlobalDef.h region blocks are never laid out, so a reader cannot tell what flipping one region switch does. | GlobalDef.h:27-31 (KOREA), 33-39 (INTERSERVER), 41-45 (TESTSERVER -> DBGAMESERVER), 47-51 (USA), 54-63 (TAIWAN), 65-70 and 72-78 (CHINESE/JAPAN/USA). | missing |
| B18 | DEF_TESTSERVER also at Game.h:186 | Not listed. It selects between two identical DEF_PLAYERMAXLEVEL 180 definitions, so no effect. | Game.h:186-192. | missing |
| B19 | Version table | Only the CHINESE row is given. | Version.h: INTERSERVER 2.20/306, USA 2.20/222, KOREA 2.20/207, CHINESE 2.19/429, default 2.19/213. | missing |
| B20 | "Dead code" list | The older two-argument `iGetPlayerABSStatus(int iWhatH, int iRecvH)` is also commented out and not listed. A live copy with the same signature follows at 37686. | Game.cpp:37560-37684 (inside `/* */`). The 1-argument one is listed (37735). | missing |
| B21 | Dead code list: overload note | Each dead name also has a live overload or twin, which is why name-based caller scans merge. | Live: bReadProgramConfigFile 5202, bCheckTotalSkillMasteryPoints 32531, iGetPlayerABSStatus 37686, bCreateNewNpc 10043, iGetNpcRelationship 37772. | imprecise |
| B22 | "Structure.cpp ... not built" vs function totals | True, but Structure.cpp's two constructors/destructors are counted in the 547/543, and XSocket.cpp (19) and StrTok.cpp (4) are counted although the dsp links them prebuilt. Functions actually compiled from the dsp: 543 - 2 - 19 - 4 = 518 (derived, not measured by compiling). | HGserver.dsp:95-139 and 147-327 (no Structure.cpp, XSocket.cpp, StrTok.cpp); lines 351 and 355 (libs). | imprecise |

### functions/persistence.md (6 entries)

| # | Entry | What is wrong | What the code shows | Severity |
|---|---|---|---|---|
| P1 | CGame::LocalSavePlayerData | Layout and format not given; "log server unreachable" is not exactly the trigger. | Game.cpp:38755-38768 writes `Character_<month>_<day>_<hour>_<min>\AscII<first byte>\<name>.txt` and opens it "wt" (text mode) although the active record is the binary DB form (Game.cpp:6428). Caller is DeleteClient only when `bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATALOGOUT ...)` returns FALSE (Game.cpp:2800). | missing |
| P2 | _bDecodePlayerDatafileContents (both) callees `_bInitItemAttr` | Merges two overloads; only one is used. | Game.cpp:5953, 6119, 7060, 7636 call the (CItem*, char* name) version at 9857; the (CItem*, int) version at 9911 is not called from either decoder. | imprecise |
| P3 | _bDecodePlayerDatafileContents (DB) | Its `_bGetIsStringIsNumber` absence is right, but the doc does not say bCheckValidName (6315) and the file-mode call (8803) are separate sites of the merged name. Callers list is correct for both. | Game.cpp:6315, 8803. | imprecise |

Ranges, signatures, callers, notifies verified for all 6 entries. DEF_NOTIFY_FIGHTZONERESERVE and DEF_NOTIFY_SERVERCHANGE verified. ResponseSavePlayerDataReplyHandler dispatch verified at Game.cpp:15108.

### functions/timers.md (9 entries)

| # | Entry | What is wrong | What the code shows | Severity |
|---|---|---|---|---|
| T1 | CGame::OnTimer callers: "ThreadProc, WndProc" | ThreadProc never reaches OnTimer, and is never started. The caller is a name-scan artefact. | Wmain.cpp:61 `if (G_pGame = NULL) G_pGame->OnTimer(NULL);` is an assignment (also nulls G_pGame), so the branch is never taken. No `_beginthread` or `CreateThread` anywhere (grep); only an `extern` at Game.cpp:265. The one real caller is WndProc, Wmain.cpp:89. | wrong |
| T2 | OnTimer purpose | Missing the base period and several conditions. | Base tick is 300 ms (Wmain.cpp:254 `_StartTimer(300)`), so "every tick" means every 300 ms and the 600 ms MobGenerator fires on every second tick. SaveOccupyFlagData only if `m_iMiddlelandMapIndex >= 0` (3760); `srand` reseed in the same 3 min block (3769); AgingMapSectorInfo every 5th 10 s pass (3819); also InvalidateRect each 3 s (3687). Shutdown drain polls every 2 s (3791); codes 3/4 call bInit and bump m_iAutoRebootingCount, else 20-step final countdown ending in `SendMessage(WM_DESTROY)` (3797-3805, 3716-3723). | imprecise |
| T3 | OnTimer `cfg: GServer.cfg` | Not read in OnTimer itself. It only happens indirectly through bInit on auto-reboot. | Game.cpp:3799. Callee list is also cut off with "..." (31 callees; bInitBufferSize is the hidden one). | imprecise |
| T4 | CheckClientResponseTime purpose | Omits notable per-client work. | Hourly speed-hack check: more than 300000 exp gained in an hour sets a one-month PenaltyBlock and calls DeleteClient (Game.cpp:4027-4057, compiled out under TESTSERVER). Skill-message flood kick (4016-4021). Hunger tick (3880-3894) which also stretches HP/MP/SP regen (3897-3919). Hunter recall from mapped zones (4097-4110). "jail" is really the locked-map timer (3986-3992). | missing |
| T5 | DelayEventProcessor / bRegisterDelayEvent | Handled delay types not listed. | Game.cpp:33033-33128 handles CALCMETEORSTRIKEEFFECT, DOMETEORSTRIKEDAMAGE, METEORSTRIKE, USEITEM_SKILL, MAGICRELEASE; DAMAGEOBJECT is a no-op (33071). bRegisterDelayEvent returns FALSE when DEF_MAXDELAYEVENTS is full and stores dwLastTime as an absolute trigger time (33013). bRemoveFromDelayEventList removes all effects when iEffectType is NULL (33145). | imprecise |
| T6 | _StartTimer / _StopTimer | Missing the period tweak and the interval. | Wmain.cpp:355-357 `timeBeginPeriod(min)` then `timeSetEvent(dwTime,0,...)`; 369-372 `timeKillEvent` then `timeEndPeriod`. | imprecise |

Ranges, signatures, MSGID, notifies and callers/callees verified for OnTimer (aside from T1), CheckClientResponseTime (17 callees, 5 notifies), DelayEventProcess (empty stub, called at Game.cpp:5049 from GameProcess), bRegisterDelayEvent, bRemoveFromDelayEventList, _TimerFunc, _StartTimer, _StopTimer.

### functions/ui-logging.md (20 entries)

| # | Entry | What is wrong | What the code shows | Severity |
|---|---|---|---|---|
| U1 | OnKeyUp (Wmain.cpp:332-334), "Hotkeys", callers WndProc | Wrong on both. The body is empty, and nothing calls it. WndProc calls `G_pGame->OnKeyUp` (the CGame member). | Wmain.cpp:332-334 `{ }`; the only free-function call is commented out at Wmain.cpp:96-98; WndProc calls the member at Wmain.cpp:80. Name-scan merge of the free function and CGame::OnKeyUp. | wrong |
| U2 | PutDebugMsg: "Debug output (OutputDebugString-style)" | It writes a file, not the debugger. | Wmain.cpp:486 `fopen("DebugMsg.log","at")` with a `(yyyy:mm:dd:hh:mm) - ` stamp. No OutputDebugString in the source. | wrong |
| U3 | DisplayInfo `cfg: GServer.cfg, World cfg` and bOnClose `cfg: GServer.cfg` | [V] claim unsupported: neither function reads a config file. | Game.cpp:888-973 prints members only. Game.cpp:38183-38191 shows a MessageBox titled with m_cServerName. | wrong |
| U4 | _bCheckGoodItem: "gold >= 10000" | Boundary wrong, and the general rule is omitted. | Game.cpp:42878-42882 gold (id 90) logs only when `m_dwCount > 10000`. Default branch (43004-43008): any item with `(attr & 0xF0F0F001) != 0` and id > 30 counts as good; a fixed id list (20, 259, 290 ... 776) always counts. | wrong |
| U5 | CGame::_bItemLog active by-name variant (42819-42870) | Purpose line "Item log by name (active)" hides that in practice only NEWGENDROP can log. | Line 42821 returns FALSE when pItem is NULL and 42822 when not a good item; callers for CREATEGUILD, BANGUILD, JOINGUILD, POISONED, SUMMON, SPELLFIELD, MAGICLEARN, SKILLLEARN, REPAIR, SHUTUP etc. pass NULL (Game.cpp:16590, 16744, 17608, 19072, 19131, 22077, 22307, 22612, 24962, 25423, 34587, 36141, 36705, 44365, 48930, 49806), so they never log. Handled actions: NEWGENDROP, SKILLLEARN, MAGICLEARN, SUMMONMONSTER, POISONED, REPAIR only. | missing |
| U6 | CGame::_bItemLog active handle variant (42704-42816) | Missing the filter rules. | Unless bForceItemLog, it returns FALSE for non-good items (42712) and always for DEF_ITEMLOG_RETRIEVE (42713). 13 actions handled; `m_pClientList[iGiveH]->m_cCharName == NULL` (42708) tests an array, never true, and derefs a possibly NULL client. | missing |
| U7 | All four `_bItemLog` entries share one identical callers list | Overload merge. List is also cut at 25 (39 distinct callers measured). The handle form (iGiveH, iRecvH, pItem) and the name form (iClientH, char* name, pItem) have disjoint caller sets. | Name form: ResponseCreateNewGuildHandler 16590, RequestDisbandGuildHandler 16744, PlayerMagicHandler 22077/22307, RequestStudyMagicHandler 24962, TrainSkillResponse 25423, DeleteNpc 30341, NpcDeadItemGenerator 45620, AdminOrder_SummonPlayer 44365 etc. Handle form: DropItemHandler 15660, GiveItemHandler 17169, RequestPurchaseItemHandler 17090, ConfirmExchangeItem 41113 etc. | imprecise |
| U8 | CGame::OnKeyUp "Hotkeys" | Too vague; most keys need F1 held. | Game.cpp:32884-32973: F1+F6 and F1+F7 send shutdown announcements 1 and 2; F1+F9 LocalStartCrusadeMode; F1+F11 ManualEndCrusadeMode(0); F1+Home GlobalStartCrusadeMode; Insert and Delete launch the meteor via _GrandMagicLaunchMsgSend plus MeteorStrikeMsgHandler (no F1 needed); F2 and End are dead. | imprecise |
| U9 | CGame::DisplayInfo "Paints the status block" | Misses what it draws. | Status line (TAIWAN variant at 895), mode banners per switch (903-957) and one row per map: "Map(%s) Object(%d) P N A E M" at y = 400 + 15*row (961-972). Banners shown in this build: CHINESE and DB GAME SERVER. | imprecise |
| U10 | PutLogList: "Appends a line ... and repaints" | It prepends and only flags a repaint. | Wmain.cpp:287-296: shifts 49 lines down 120 bytes, new line at slot 0 (max 120 chars, no bound check on memcpy), sets G_cMsgUpdated; the repaint happens later in UpdateScreen (298-304). OnPaint shows 20 of the 50 slots (319-322). Callers list is cut at 25 of about 106. | imprecise |
| U11 | PutItemLogFileList: "Appends to the item log file" and no callers | It buffers; and it has no live caller. | Wmain.cpp:418-458: G_cLogBuffer (30000 bytes), flushed at 100 entries, after 10 s, or bIsSave, to `LogData\ItemEvents<yyyymmdd>.log`. The only call is commented out (Game.cpp:42490), so item logs go to the Log server via bSendMsgToLS only. | imprecise |
| U12 | PutLogFileList, PutAdminLogFileList, PutLogEventFileList | File names not given. | Events.log (Wmain.cpp:384), AdminEvents.log (404), LogEvents.log (466). | missing |

Ranges and signatures verified for all 20. DisplayInfo switch list (9) verified against banner sites. _bPKLog (MSGID_GAMEITEMLOG) and _bCrusadeLog (MSGID_GAMECRUSADELOG) verified, including action sets.

### functions/support.md (87 entries)

| # | Entry | What is wrong | What the code shows | Severity |
|---|---|---|---|---|
| S1 | XSocket::bAccept | Purpose and caller describe CGame::bAccept (name merge). | XSocket::bAccept (XSocket.cpp:483-512) only accept()s onto the passed XSocket, registers WSAAsyncSelect, sets 40960-byte (8192*5) socket buffers. The slot search, shutdown/WLserver rejection and IP logging are in CGame::bAccept (Game.cpp:399-464). Callers of the XSocket one are CGame::bAccept at 431 and 460; OnAccept (Wmain.cpp:339) calls CGame::bAccept. | wrong |
| S2 | XSocket::_iOnRead: "decrypts body when key != 0" | The read path does not decrypt. | Decryption is in pGetRcvDataPointer, XSocket.cpp:553-559. _iOnRead (169-254) only does the 3-byte header, size WORD at offset 1, and body read. | wrong |
| S3 | XSocket::bInitBufferSize: "client sockets 8 KB class sizes set by caller" | Unsupported number. | Every caller passes DEF_MSGBUFFERSIZE = 30000 (Client.h:24; Client.cpp:17, Game.cpp:801, 810, 3732, 4775, 20591, 20751, 42448). XSocket.cpp:61-64 allocates size+8 for both buffers. The 8192*5 value is the SO_RCVBUF/SO_SNDBUF option (XSocket.cpp:155, 507). | wrong |
| S4 | XSocket::_iRegisterUnsentData: "circular list, DEF_XSOCKBLOCKLIMIT 300" | Limit is per socket. | XSocket.cpp:367 and 392 wrap at m_iBlockLimit (ctor arg): 15 for client sockets (DEF_CLIENTSOCKETBLOCKLIMIT, Client.h:22), 300 for server sockets (Game.h:51). DEF_XSOCKBLOCKLIMIT is only the array size (XSocket.cpp:26); the wrap that used it is commented out. | wrong |
| S5 | CMisc::cGetNextMoveDir: "avoiding blocked tiles" | No tile check at all. | Misc.cpp:31-55 maps the sign of (sX-dX, sY-dY) to one of 8 directions. | wrong |
| S6 | CMisc::cGetNextMoveDir callers: NpcBehavior_Attack, _Flee, _Move, PlayerMagicHandler | Overload merge with CGame::cGetNextMoveDir (7 args). _Flee and _Move only call the CGame one; the CGame one is the real caller for the path. | `m_Misc.cGetNextMoveDir`: Game.cpp:11454 (inside CGame::cGetNextMoveDir), 11904/11961/12131 (NpcBehavior_Attack), 22316 (PlayerMagicHandler). CGame version called at 11621, 11642 (NpcBehavior_Move), 12243 (_Attack), 14826 (NpcBehavior_Flee). | wrong |
| S7 | CMisc::GetPoint: "used by linear spells/arrows" | Spells and arrows use GetPoint2. GetPoint is used only for NPC pathing. | Sole caller Game.cpp:11452 (CGame::cGetNextMoveDir); GetPoint2 at 21170, 21279, 22950. The doc's own callers line already says so. | wrong |
| S8 | CMisc::GetDirPoint: "Offsets a point one tile in a direction 1..8" | The function does nothing useful. `*pY--` decrements the pointer, not the value. | Misc.cpp:219-226. No callers. | wrong |
| S9 | CStrTok::_bIsSeperator: "text inside double quotes is kept together [I]" | There is no quote handling. | StrTok.cpp:69-82: separators are NUL, CR LF, and any char in m_pSeps. | wrong |
| S10 | ThreadProc: "Worker thread (idle loop)", callee OnTimer | Same defect as T1. Never started; the condition is an assignment. | Wmain.cpp:58-66; no thread creation exists. | wrong |
| S12 | CMisc::Temp: "CMisc member (Misc.cpp)" | Placeholder. It is a one-off map-file converter. | Misc.cpp:299-369 reads middleland.amd, middleland1.amd, middleland2.amd and writes middleland.amd.result (MAPSIZEX = 824 header). Its only call is commented out (Game.cpp:523). | imprecise |
| S13 | CMisc::GetMyCursorPos: "Window cursor position helper" | Screen coords via GetCursorPos; no callers. | Misc.cpp:21-28. | imprecise |
| S14 | About 45 entries say only "X member (X.cpp)" or "free member (Wmain.cpp)" (all constructors/destructors, InitApplication, InitInstance) | Placeholder purposes. InitInstance should note the title string with the version. CClient::CClient is 292 lines of init. | InitInstance builds "Helbreath GameServer V%s.%s %d" (Wmain.cpp:189) and an 800x600 popup window (201-202). | imprecise |
| S15 | Default constructors (CItem, CPortion, CTile, CMagic, CSkill ...) have no callers listed | Token scan misses `new class CItem;` (no parentheses). | CNpc/CFish/CMineral/CDynamicObject get callers only because they take arguments. | imprecise |
| S16 | _InitWinsock: "failure message says 'Socket 1.1 not found'" | The message is in Initialize, not in this function. The call actually asks for Winsock 2.2. | XSocket.cpp:574 `MAKEWORD(2, 2)`; message at Wmain.cpp:242. | imprecise |
| S17 | Initialize / OnDestroy | Initialize: on bInit failure it prints "(!!!) STOPPED!" and returns without timer or listen socket (Wmain.cpp:248-251). OnDestroy also deletes the listen/log sockets and the game and closes pLogFile (269-282). `cfg: GServer.cfg` is via bInit only. | | imprecise |
| S18 | CGame::_bGetIsStringIsNumber: "True if a token is all digits" | Also accepts '-' anywhere and the empty string. | Game.cpp:9969-9972. | imprecise |
| S19 | CGame::iDice: callers list cut at 25 | Fine as logic (Game.cpp:20212-20220 verified). | | imprecise |

Ranges, signatures, sources verified for all 87 entries. iSendMsg framing and cipher, pGetRcvDataPointer, bListen, _CloseConn, iGetPeerAddress, CMsg::bPut/Get, bEncode/bDecode no-op, bCheckValidName character set, GetPoint2 verified.

### ..\game-server.md (index)

| # | Entry | What is wrong | What the code shows | Severity |
|---|---|---|---|---|
| I1 | build-config line: "every #define switch, its value and sites" | Over-claims; see B15-B19. | | imprecise |
| I2 | support.md line | Does not mention that the file also holds two CGame helpers (iDice, _bGetIsStringIsNumber) and the Wmain startup/shutdown functions (WinMain, Initialize, OnDestroy, ThreadProc). | support.md headings. | imprecise |
| I3 | "export of commit 1697e41" | Cannot be checked from the export. | | unverified |

All 17 index paths exist (14 function docs, mechanics.md, build-config.md, data-structures.md), as do `servers.md`, `shared/messages/` and `shared/formats/`. "Reports itself as v2.19" verified.

## Counts

| Severity | Count | Rows |
|---|---|---|
| wrong | 18 | B1-B3, T1, U1-U4, S1-S10 (S10 repeats the T1 defect) |
| imprecise | 34 | B4-B14, B21, B22, P2, P3, T2, T3, T5, T6, U7-U11, S12-S19, I1, I2 |
| missing | 11 | B15-B20, P1, T4, U5, U6, U12 |
| unverified | 1 | I3 (commit id) |

63 issue rows in total (counted from the tables above).

## Entries checked

179 doc entries: persistence 6, timers 9, ui-logging 20, support 87, build-config 39 (13 switches in effect, 22 compiled-out rows, 3 layout bullets, 1 dead-code block), index 18 (1 header plus 17 links). Plus 3 headline claims (version, 547, 543): 182 checks in total.

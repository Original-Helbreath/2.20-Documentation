# Build switches and project layout (v2.20 HGserver)

Source: `.claude/scratch/srv220/GlobalDef.h`, `Version.h`, `HGserver.dsp`. Tags: [V] verified, [I] inferred.

## Project layout
- [V] HGserver.dsp compiles these files: Client, Game, Teleport, wmain, BuildItem, DelayEvent, DynamicObject, Fish, GuildsMan, Item, Magic, Map, Mineral, Misc, Msg, Npc, OccupyFlag, Potion, Quest, Skill, StrategicPoint, TeleportLoc, Tile. It links GameServerStrTok.lib and GameServerXsocket.lib, so StrTok.cpp and XSocket.cpp arrive prebuilt. The function totals in game-server.md (547 live, 543 active) still count Structure.cpp's 2 members, XSocket.cpp's 19 and StrTok.cpp's 4; functions actually compiled from the dsp: 543 - 2 - 19 - 4 = 518 (derived, not measured by compiling).
- [V] In the original VC6 project (HGserver.dsp) Structure.cpp, CrusadeCore.cpp and rusadeCore.cpp are not built (the .dsp lists only the XSocket.h and StrTok.h headers, no .cpp for them; the 518 count above is for that build). CrusadeCore.cpp is fully commented out ([GSO-009](../issues/game-server-other.md#gso-009-structurecpp-crusadecorecpp-and-rusadecorecpp-are-not-built-in-the-vc6-project)).
- [V] `sources/hgserver-librehelbreath/CMakeLists.txt` (line 5: `file(GLOB_RECURSE sources src/*.cpp src/*.h)`) builds every .cpp under src/, including Structure.cpp, XSocket.cpp, StrTok.cpp, CrusadeCore.cpp and rusadeCore.cpp, with no prebuilt libs linked, so the 518-function limit does not apply there (function count for that build not re-derived). Per the round-2 review it differs from the scratch tree only by two compile fixes (`Game.cpp:48960` static_cast, `Wmain.cpp:357` LPTIMECALLBACK cast) with line numbers unchanged (full diff re-run, CRLF ignored: exactly these two lines differ, no other file) [CMake contents V; diff claim V].
- [V] Version.h: DEF_CHINESE selects version "2"."19", build 429. The 2.20 export therefore reports itself as v2.19 ([GSO-003](../issues/game-server-other.md#gso-003-the-220-source-builds-and-reports-as-version-219)).

## Switches in effect

| Switch | State | Effect and sites |
|---|---|---|
| DEF_CHINESE | on | Selects the Chinese build. Turns on the eight switches below (CANDYEVENT, CHINESEGUILDLIMIT, NPCITEM directly, GlobalDef.h:65-70; DBGAMESERVER, LOCALNPCNAME, ENGLISHITEMNAME, LOGTIME, DEFENCEHACKING via the CHINESE/JAPAN/USA group, GlobalDef.h:72-78). `/who` needs admin level 3 (Game.cpp:10753). |
| DEF_CHINESECANDYEVENT | on (via CHINESE) | No effect: its only use (Dec 2002 candy drops from NPC types 55/61) is commented out (Game.cpp:44883; [GSO-010](../issues/game-server-other.md#gso-010-def_chinesecandyevent-has-no-effect)). |
| DEF_CHINESEGUILDLIMIT | on (via CHINESE) | Guild creation needs level 100 and 1,000,000 gold (Game.cpp:16565-16667); Charisma 20 is required in both builds (16566, 16669-16673), and the gold is deducted in the reply handler (16582-16587). Also rejects neutral, hunter and PK-zone players. |
| DEF_NPCITEM | on (via CHINESE) | NpcItem config drops: guards bGetItemNameWhenDeleteNpc (called from DeleteNpc); active branch 47193-47258, `#else` branch 47260-47547 is inactive (Game.cpp:47187-47193). |
| DEF_DBGAMESERVER | on (via CHINESE) | DB-mode player record. Adds m_dwCharID to Log messages (4498). Uses the DB decoder/encoder (5549); the file-mode pair at 6945/8917 is compiled out. |
| DEF_LOCALNPCNAME | on (via CHINESE) | NPC killer name logged as "NPCNPCNPC@type" (19295). |
| DEF_ENGLISHITEMNAME | on (via CHINESE) | Not tested in any .cpp [V by grep], but englishitem.h:1 (`#ifdef`) and koreaitem.h:1 (`#ifndef`) select which DEF_ITEMNAME_* string macros exist (e.g. "Arrow"); both are included at Game.h:47-48, so the English names are used. |
| DEF_LOGTIME | on (via CHINESE) | Item unique id value 3 = timeGetTime. |
| DEF_DEFENCEHACKING | on (via CHINESE) | Start-up requires the WLserver file from the Log server (411, 3699, 15180, 15236, 18655, 40760, 49005; [GSO-013](../issues/game-server-other.md#gso-013-start-up-is-gated-on-the-wlserver-file-push-under-def_defencehacking)). |
| DEF_V219 | on | Price byte in init and teleport (2051, 23657). Crusade winner 10 % shop discount (16902) with half-price floor (16936). Crusade duty (49051). Hunter mode (52784 and 52827 are `#ifndef DEF_V219 return;` guards, inert while V219 is on; 52897 is `#ifdef`). Party exp table (51708, `#ifdef`). |
| DEF_BEGINNERTOWNPROTECT | on | Neutral innocents in NONPK towns are immune (12592). |
| DEF_MAXCRUSADESTRUCTURES | 300 | Size of m_stCrusadeStructures. |
| DEF_MAXAGRICULTURE | 200 | Crop limit. |

## Switches compiled out

| Switch | Effect if turned on (sites) |
|---|---|
| DEF_WEAPONSPEEDLIMIT | Weapon speed checks at equip (16178, 16235). |
| NO_MSGSPEEDCHECK (no DEF_ prefix; GlobalDef.h:6) | Skips move/attack speed checks (tested `#ifndef` at 1149, 11304). |
| DEF_CHECKUSERNPCNUM | User-to-NPC ratio for mob spawns (29005). |
| DEF_GUILDWARMODE | 2150 and 23756 are `#ifndef`: the fightzone force-recall block is live now and is removed if the mode is on. Blocks magic 31/40/41/46/54/55/73 (20853, `#ifdef`), exchange (40839) and special abilities (47124). Banner line 933. |
| DEF_BEGINNEREVENTMODE | Display line (939); double exp on arefarm/elvfarm/arebrk/elvbrk/middled1n (51654). |
| DEF_TESTSERVER | 28 sites (26 in Game.cpp, Game.h:186, GlobalDef.h:41). Additive when on: skill SSN x100 (28715, 28865), skill level = 100 (38435), drop probability -3000 (44675), item count x10 (44712), NPC drop table (47313), party exp x30 (51748), exp x70 (51765, 51771), banner (903). 16 sites are `#ifndef DEF_TESTSERVER` and are LIVE in this build, vanishing when TESTSERVER is on: the hourly 300000-exp speed-hack check and ban (4027) and the "admin command enabled" gates (10945, 10959, 10974, 36089, 36466, 36549, 36661, 44018, 44029, 44082, 44112, 44227, 44324, 44495, 44625). Also turns on DBGAMESERVER (GlobalDef.h:41-42). Game.h:186 selects between two identical DEF_PLAYERMAXLEVEL 180 definitions (no effect). |
| DEF_KOREA | 16387 is `#ifndef DEF_KOREA`: the admin-level check on the appr4 0x0001 flag is dropped when on. GlobalDef.h:27-31 turns on CONSULTATION, DEFENCEHACKING and CHINESECANDYEVENT; Version.h:14-17 gives 2.20 build 207. |
| DEF_TAIWAN | Admin levels (11030, 36093, 43961, 46328, 46370); DEF_MAXCONSTRUCTNUM 7 vs 10 (Game.h:160); DEF_MAX_CRUSADESUMMONMOB 5 vs 500 (Game.h:220; used at 48837, 48864); banner and status line (894, 915). Also sets DEF_TAIWANLOG, `#undef DEF_WINTER`, DBGAMESERVER, LOCALNPCNAME, ENGLISHITEMNAME, LOGTIME (GlobalDef.h:54-63). |
| DEF_JAPAN, DEF_USA | JAPAN's only effect is joining the DBGAMESERVER/LOCALNPCNAME/ENGLISHITEMNAME/LOGTIME/DEFENCEHACKING group (GlobalDef.h:72). USA also sets SAMESIDETOWNPROTECT and MAXLEVEL150 (GlobalDef.h:47-51); Version.h:9-12 gives 2.20 build 222. |
| DEF_INTERSERVER | Status display (927); also defines MAXLEVEL150, DBGAMESERVER, DEFENCEHACKING, ENGLISHITEMNAME, LOCALNPCNAME (GlobalDef.h:33-39); Version.h:4-7 gives 2.20 build 306. |
| DEF_WINTER | Snow weather (37410). |
| DEF_CONSULTATION | Enables DEF_PKPENALTY, DEF_SAMESIDETOWNPROTECT and DEF_ITEMDROP (GlobalDef.h:82-87; DEF_ALLPLAYERPROTECT stays commented, line 83). Defined only inside `#ifdef DEF_KOREA` (GlobalDef.h:28). Its own sites are 44749 (`#ifdef ... #else` 44801 ... `#endif` 44880, a drop switch) and 52736 (`#ifndef`, force-recall times 20*3 vs 20*1). |
| DEF_PKPENALTY | Jail regardless of location (28029, 28043). |
| DEF_ITEMDROP | Consumable death-drop logic (28309, 28385). |
| DEF_SAMESIDETOWNPROTECT | Innocent players cannot be hit by same-side players in NONPK towns (12588, 31204, 31761). |
| DEF_ALLPLAYERPROTECT | Innocents immune in NEVERNONPK zones; outside crusade, same-side innocents immune in NONPK towns (12570, 31186, 31743, 52932). |
| DEF_BALLEVENT | Taiwan wish-ball event drops, 2002-12-20 (909, 44817). |
| DEF_ITEMDROPRATINGUP | Value macro, must carry a number (`iItemprobability -= DEF_ITEMDROPRATINGUP`, 44678-44679); lowers the drop threshold. Currently commented with 1500 (USA) and 3000 (CHINESE) (GlobalDef.h:50, 69). Banner 946. |
| DEF_MAXLEVEL150 | Exp table remap for levels 140-179 (`for (i = 140; i < 180; i++)`, remap by (i-140)*3, 775-778). Also set by INTERSERVER and USA. |
| DEF_TAIWANLOG | Taiwan item log layout. `_bItemLog` at 42474/42586 is compiled out. |
| DEF_TIAWAN (typo) | Game.cpp:36665; never defined, so that block is dead ([GSO-011](../issues/game-server-other.md#gso-011-misspelt-build-switches-leave-blocks-dead)). |
| TAIWANLOG (no DEF_) | Game.cpp:34585; never defined ([GSO-011](../issues/game-server-other.md#gso-011-misspelt-build-switches-leave-blocks-dead)). |

## Region switch chains (GlobalDef.h) and versions
- [V] KOREA (27-31) -> CONSULTATION, DEFENCEHACKING, CHINESECANDYEVENT. INTERSERVER (33-39) -> MAXLEVEL150, DBGAMESERVER, DEFENCEHACKING, ENGLISHITEMNAME, LOCALNPCNAME. TESTSERVER (41-45) -> DBGAMESERVER. USA (47-51) -> SAMESIDETOWNPROTECT, MAXLEVEL150. TAIWAN (54-63) -> TAIWANLOG, DBGAMESERVER, LOCALNPCNAME, ENGLISHITEMNAME, LOGTIME, `#undef DEF_WINTER`. CHINESE (65-70) -> CHINESECANDYEVENT, CHINESEGUILDLIMIT, NPCITEM; CHINESE/JAPAN/USA (72-78) -> DBGAMESERVER, LOCALNPCNAME, ENGLISHITEMNAME, LOGTIME, DEFENCEHACKING. CONSULTATION (82-87) -> PKPENALTY, SAMESIDETOWNPROTECT, ITEMDROP.
- [V] Version.h: INTERSERVER 2.20 build 306, USA 2.20 build 222, KOREA 2.20 build 207, CHINESE 2.19 build 429, default 2.19 build 213.
- [V] Not every site is listed in the tables above. Additional sites: banner lines in DisplayInfo (CHINESE 921, BALLEVENT 909, INTERSERVER 927, GUILDWARMODE 933, BEGINNEREVENTMODE 939, ITEMDROPRATINGUP 946, DBGAMESERVER 953); DEF_LOGTIME 16888, 30318, 45588, 45709; DEF_BEGINNERTOWNPROTECT 31208 and 31765 (besides 12592); DEF_TAIWANLOG has about 28 `#ifdef` sites (the `_bItemLog` call sites among them).

## Dead code inside /* */ (not counted as live functions)
- [V] Several functions exist only inside `/* */` blocks (not counted as live functions); each has a live overload or twin, which is why the name-based caller scans merge them: bReadProgramConfigFile 5202, bCheckTotalSkillMasteryPoints 32531, iGetPlayerABSStatus 37686, bCreateNewNpc 10043, iGetNpcRelationship 37772. The dead originals are listed in [GSO-012](../issues/game-server-other.md#gso-012-several-functions-exist-only-inside-commented-out-blocks).

## Constants
Limits and timers (DEF_MAX*, DEF_*TIME) are listed in `data-structures.md`.

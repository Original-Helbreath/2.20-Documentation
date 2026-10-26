# System: crusade

*Verdict (size review): about 20 KB, but the subsystems presuppose each other (duties feed points, points feed structures, structures feed mana, mana feeds the meteor that ends the war); readers come for the whole war loop, so it stays one file with a Contents block. Energy sphere and occupy flags are short sections inside it.*

Helbreath 2.20. Paths: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`, `G` = `sources/gateserver-librehelbreath/`.
Tags: [V] read in source, [I] inferred, [U] unverified. Function bodies (69 functions): `research/game-server/functions/crusade-quests.md`;
GSM table: `research/systems/commands/gate-relay.md`.

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flow - 7. Rules - 8. Couplings -
9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose

The crusade ("total war", 전면전) is the scheduled, world-wide war between Aresden and Elvine that gives the PvP rivalry
an objective. When it starts on every game server at once, each citizen picks a duty - soldier, engineer (constructor) or,
for guild masters only, commander [V S Game.cpp:49042-49072]. Fighting earns **war contribution** (paid out as exp after
the war) and **construction points** (spent by commanders/engineers on towers, mana collectors, detectors and war
units) [V S 12744-12752, 14595-14668, 28127-28138, 48722-48927]. Mana collectors placed next to the mana stones in
Middleland harvest mana; mana from every server is pooled through the gate into the side's Grand Magic Generator; when it
is full, a **meteor strike** hits the enemy town's strike points (buildings) [V S 47711-47781, 48137-48243, 48246-48450].
When all strike points of one town are destroyed, that side loses and the war ends everywhere; a GM can also end it as a
draw [V S 49947-49975, 50526-50560]. Players are rewarded at their next exp-stock tick after the war, even if they were
offline when it ended, because the crusade GUID is stored per character [V S 47839-47909].

## 2. Subsystems

| # | Subsystem | Intent | Owner functions |
|---|---|---|---|
| 2.1 | Schedule and start/stop | WarSchedule.cfg on one designated server, GM override, GSM_BEGIN/ENDCRUSADE to all servers, GUID file | `CrusadeWarStarter`, `Global/Local Start/End`, `ManualEndCrusadeMode`, `_CreateCrusadeGUID` |
| 2.2 | Duties | soldier / engineer / commander roles with different dialogs and powers | `SelectCrusadeDutyHandler`, client dialogs 33/36/37/38 |
| 2.3 | War points | war contribution (reward) and construction points (currency), with caps | kill handlers, `CheckCommanderConstructionPoint`, `GSM_ConstructionPoint` |
| 2.4 | Command tools | guild teleport point, construction point, map status, help call, war-unit summon | `RequestSetGuild*LocHandler`, `RequestGuildTeleportHandler`, `MapStatusHandler`, `RequestHelpHandler`, `RequestSummonWarUnitHandler` |
| 2.5 | Structures and construction | Crusade.cfg fixed structures, engineer construction kits, structure behaviours (collector, detector, GMG) | `CreateCrusadeStructures`, `__bSetConstructionKit`, `_bNpcBehavior_*`, `RemoveCrusadeStructures`, `KillCrusadeObjects` |
| 2.6 | Mana and grand magic | mana stones -> collectors -> cross-server pool -> GMG -> meteor | `_bNpcBehavior_ManaCollector`, `SendCollectedMana`, `CollectedManaHandler`, `_NpcBehavior_GrandMagicGenerator` |
| 2.7 | Meteor strike and victory | strike points in town maps, damage to buildings and players, results, end of war | `MeteorStrikeMsgHandler`, `MeteorStrikeHandler`, `DoMeteorStrikeDamageHandler`, `CalcMeteorStrikeEffectHandler`, `GrandMagicResultHandler` |
| 2.8 | Results and rewards | contribution -> exp stock, winner bonus, losing-commander head start next time | `CheckCrusadeResultCalculation`, `SelectCrusadeDutyHandler` (DEF_V219) |
| 2.9 | Middleland sync | share structure positions and guild points between servers | `SyncMiddlelandMapInfo`, `GSM_SetGuild*Loc` |
| 2.10 | Energy sphere (side event) | random Middleland ball pushed into a nation's goal; contribution +5/-10 | `EnergySphereProcessor`, `bCheckEnergySphereDestination` |
| 2.11 | Occupy flags (retired) | plant EK-bought flags to claim Middleland tiles | `GetOccupyFlagHandler`, `__bSetOccupyFlag` (dead), `SaveOccupyFlagData` (no-op) |

## 3. Functions (by subsystem, key hops only; the full list is in crusade-quests.md)

| Sub | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | `bReadCrusadeScheduleConfigFile` | game server | S Game.cpp:50691-50779 | WarSchedule.cfg: `game-server-name`, up to 10 `schedule = day hour minute` [V doc] |
| 2.1 | `CrusadeWarStarter` | game server | S Game.cpp:50669-50689 | minute match on the designated server [V] |
| 2.1 | `GlobalStartCrusadeMode` | game server | S Game.cpp:50274-50300 | day guard, GUID = `timeGetTime()`, GSM_BEGINCRUSADE, local start [V] |
| 2.1 | `LocalStartCrusadeMode` / `LocalEndCrusadeMode` | game server | S Game.cpp:47912-47988 | set mode, reset duties/points, notify, strike points, structures, GUID file, recall hunters [V] |
| 2.1 | `ManualEndCrusadeMode` | game server | S Game.cpp:50526-50560 | GM end as draw + GSM_ENDCRUSADE [V doc] |
| 2.1 | `_CreateCrusadeGUID` / `bReadCrusadeGUIDFile` | game server | S Game.cpp:50420-50523 | `GameData/CrusadeGUID.Txt` (GUID + winner) [V doc] |
| 2.1 | `ServerStockMsgHandler` GSM_BEGIN/ENDCRUSADE | game server | S Game.cpp:49488-49500 | remote start/end [V doc] |
| 2.2 | `DlgBoxClick_CrusadeJob` / `DrawDialogBox_CrusadeJob` | client | C Game.cpp:19141-19238, 45311-45407 | duty pick dialog 33 [V doc] |
| 2.2 | `SelectCrusadeDutyHandler` | game server | S Game.cpp:49042-49072 | set duty; commander needs rank 0 [V] |
| 2.2 | `NotifyMsgHandler` `DEF_NOTIFY_CRUSADE` | client | C Game.cpp:30559-30620 | start/end banners, duty dialog, Middleland map request [V] |
| 2.3 | `iCalculateAttackEffect` structure-complete branch | game server | S Game.cpp:12740-12755 | engineer finishing a structure gets contribution [V] |
| 2.3 | `NpcKilledHandler` crusade block | game server | S Game.cpp:14595-14690 | war unit kills -> points; friendly kill zeroes contribution; NPC kills credit the guild commander [V] |
| 2.3 | `ClientKilledHandler` NPC-killer block | game server | S Game.cpp:19477-19510 | player killed by a guild's war unit -> commander points [V] |
| 2.3 | PK/EK reward block | game server | S Game.cpp:28120-28201 | enemy kill in crusade -> exp, contribution x5, points level/2 [V] |
| 2.3 | `CheckCommanderConstructionPoint`, `GSM_ConstructionPoint` | game server | S Game.cpp:50302-50380 | move engineers' points to the commander across servers [V doc] |
| 2.4 | `DlgBoxClick_Commander/Constructor/Soldier`, `_RequestMapStatus` | client | C Game.cpp:19536-19960, 19240 | duty dialogs 36/37/38; map status every 10 s [V doc] |
| 2.4 | `RequestSetGuildTeleportLocHandler`, `RequestSetGuildConstructLocHandler` | game server | S Game.cpp:48453-48662 | set points, GSM to other servers [V] |
| 2.4 | `RequestGuildTeleportHandler` | game server | S Game.cpp:48664-48698 | teleport to the guild point [V] |
| 2.4 | `MapStatusHandler`, `_SendMapStatus` | game server | S Game.cpp:49074-49227 | structure list in chunks of 100 [V doc] |
| 2.4 | `RequestHelpHandler` | game server | S Game.cpp:49268-49287 | soldier position to the commander [V] |
| 2.4 | `RequestSummonWarUnitHandler` | game server | S Game.cpp:48700-48935 | spend points on units/structures near the construct point [V] |
| 2.5 | `bReadCrusadeStructureConfigFile` | game server | S Game.cpp:5354-5484 | Crusade.cfg: map, NPC type, x, y [V doc] |
| 2.5 | `CreateCrusadeStructures` | game server | S Game.cpp:48019-48134 | spawn configured structures, pick 3 mana stones [V] |
| 2.5 | `__bSetConstructionKit` | game server | S Game.cpp:47637-47708 | engineer kit -> tower etc. (duty 2, per-map cap) [V] |
| 2.5 | `_bNpcBehavior_ManaCollector`, `_bNpcBehavior_Detector`, `_NpcBehavior_GrandMagicGenerator` | game server | S Game.cpp:47711-47836, 48198-48243 | structure behaviours from `NpcBehavior_Stop` [V] |
| 2.5 | `RemoveCrusadeStructures`, `KillCrusadeObjects` | game server | S Game.cpp:49230-49248, 50781-50799 | end-of-war cleanup [V doc] |
| 2.6 | `SendCollectedMana`, `CollectedManaHandler` | game server | S Game.cpp:48137-48195 | GSM_COLLECTEDMANA out / in [V] |
| 2.7 | `MeteorStrikeMsgHandler` | game server | S Game.cpp:48246-48308 | warn players, schedule strike 5 s later [V] |
| 2.7 | `MeteorStrikeHandler` | game server | S Game.cpp:48336-48450 | pick an intact strike point, damage it [V doc] |
| 2.7 | `DoMeteorStrikeDamageHandler` | game server | S Game.cpp:49866-49917 | damage to players in the struck map [V doc] |
| 2.7 | `CalcMeteorStrikeEffectHandler` | game server | S Game.cpp:49922-50044 | count surviving strike points; end war; GSM_GRANDMAGICRESULT [V] |
| 2.7 | `MeteorStrikeComing`, `GrandMagicResult` | client | C Game.cpp:19977, 47247 | warning and result screens [V doc] |
| 2.8 | `CheckCrusadeResultCalculation` | game server | S Game.cpp:47839-47909 | reward on the exp-stock tick [V] |
| 2.8 | `CrusadeContributionResult`, `CrusadeWarResult` | client | C Game.cpp:21097, 21177 | result dialogs [V doc] |
| 2.9 | `SyncMiddlelandMapInfo` | game server | S Game.cpp:50079-50131 | GSM_MIDDLEMAPSTATUS [V doc] |
| 2.10 | `EnergySphereProcessor`, `bCheckEnergySphereDestination` | game server | S Game.cpp:46993-47110, 46846-46991 | sphere spawn and goal [V doc + V 46930-46950] |
| 2.11 | `GetOccupyFlagHandler`, `__bSetOccupyFlag`, `SaveOccupyFlagData` | game server | S Game.cpp:39978-40140, 39499-39890, 47991-48016 | see 9.1 [V doc] |

## 4. Commands and messages

- Client -> game (`MSGID_COMMAND_COMMON`): `REQUEST_SELECTCRUSADEDUTY` (iV1 duty), `SETGUILDTELEPORTLOC`,
  `SETGUILDCONSTRUCTLOC` (x, y, map), `GUILDTELEPORT`, `SUMMONWARUNIT` (x, y, type, count, mode), `REQUEST_HELP`,
  `REQUEST_MAPSTATUS` (mode, map) [V S Game.cpp:15399-15425]; `REQ_GETOCCUPYFLAG` [V S 15471]; construction kits are
  items used with `REQ_USEITEM` [V doc].
- Game -> client: `DEF_NOTIFY_CRUSADE` (mode, duty, contribution, winner) [V S 47932, 47975, 47852], `CONSTRUCTIONPOINT`,
  `MAPSTATUSNEXT/LAST`, `TCLOC`, `HELP`/`HELPFAILED`, `CANNOTCONSTRUCT` (1 no room, 2 too far from construct point,
  3 build limit or no point, 4 unit limit) [V S 48828-48907], `NOMORECRUSADESTRUCTURE`, `METEORSTRIKECOMING` (1 own town
  targeted, 2 elsewhere) [V S 48261-48265], `METEORSTRIKEHIT`, `GRANDMAGICRESULT`, `ENERGYSPHERECREATED/GOALIN`,
  `ENEMYKILLS`, `LOCKEDMAP`.
- Gate GSM: BEGINCRUSADE 0x06 (GUID), ENDCRUSADE 0x07 (winner, results), COLLECTEDMANA 0x05, GRANDMAGICLAUNCH 0x04,
  GRANDMAGICRESULT 0x03, MIDDLEMAPSTATUS 0x08, SETGUILDTELEPORTLOC 0x09, CONSTRUCTIONPOINT 0x0A, SETGUILDCONSTRUCTLOC 0x0B.
- GM slash: `/begincrusadetotalwar`, `/endcrusadetotalwar` (admin 3 + enable), `/energysphere` (admin 2).
- Log: `MSGID_GAMECRUSADELOG` via `_bCrusadeLog` (start, end, duty, exp) -> world log `EKEventsLog` [V doc].

## 5. Data

| Item | Where | Meaning |
|---|---|---|
| `m_bIsCrusadeMode`, `m_dwCrusadeGUID`, `m_iCrusadeWinnerSide`, `m_iWinnerSide`, `m_iLatestCrusadeDayOfWeek`, `m_bIsCrusadeWarStarter` | S Game.h | war state [V] |
| `m_stCrusadeWarSchedule[DEF_MAXSCHEDULE=10]` | S Game.h | day/hour/minute [V doc] |
| `m_stCrusadeStructures[300]` | S Game.h:803 | Crusade.cfg rows; type 42 = mana stone [V S 48034] |
| `m_iNpcConstructionPoint[]` | S Game.cpp:583-604 | cost per NPC type: 43 LWB 1000, 44 GHK 2000, 45 GHKABS 3000, 46 TK 2000, 47 BG 3000, 51 catapult 1500, 1-6 100 [V] |
| `m_pGuildTeleportLoc[DEF_MAXGUILDS=1000]` (`CTeleportLoc`: GUID, map, x, y, built count `m_iV2`, unit count) | S Game.h:809 | per-guild war points [V S 48835-48869] |
| `m_iCollectedMana[3]`, `m_iAresdenMana`, `m_iElvineMana` | S Game.h | per-server harvest; pooled mana [V doc] |
| `m_iAresdenMapIndex`, `m_iElvineMapIndex`, `m_iMiddlelandMapIndex` | S Game.cpp:5522-5531 | maps named `aresden`, `elvine`, `middleland` on this server [V] |
| `Map::m_stStrikePoint[DEF_MAXSTRIKEPOINTS=20]`, `m_iTotalStrikePoints` | S Map.h:48, 194 | town buildings with HP, from map config (`strike-point`) [V S 26640-26670] |
| `Map::m_stEnergySphereGoalList`, `m_iCurEnergySphereGoalPointIndex` | S Map.h | Middleland goals [V S 46930] |
| per client: `m_iCrusadeDuty` (1 soldier, 2 engineer, 3 commander), `m_dwCrusadeGUID`, `m_iWarContribution`, `m_iConstructionPoint`, `m_cConstructMapName`, `m_iConstructLocX/Y`, `m_stCrusadeStructureInfo[300]` | S Client.h:299-329 | war fields; contribution saved as `character-war-contribution` [V S 9195] |
| caps | `DEF_MAXCONSTRUCTIONPOINT` 30000, `DEF_MAXWARCONTRIBUTION` 500000, `DEF_GMGMANACONSUMEUNIT` 15, `DEF_MAXCONSTRUCTNUM` 10 (7 Taiwan), `DEF_MAX_CRUSADESUMMONMOB` 500 (5 Taiwan), `DEF_MAXCRUSADESTRUCTURES` 300 | S Game.h:161-223, GlobalDef.h:2 [V] |
| files | `Crusade.cfg`, `WarSchedule.cfg`, `GameData/CrusadeGUID.Txt`; retired `GameData/OccupyFlag.txt`, world-log `AresdenOccupyFlag.Txt`/`ElvineOccupyFlag.Txt` | [V doc] |

## 6. Flow

**F1 Start.** OnTimer -> `CrusadeWarStarter` on the server named in WarSchedule.cfg: weekday, hour, minute match
(the weekday is the system `wDayOfWeek`, Sunday = 0) [V S 50674-50687] (or GM `/begincrusadetotalwar`) -> `GlobalStartCrusadeMode`: day guard, GUID, GSM_BEGINCRUSADE to the
gate, `LocalStartCrusadeMode` [V 50281-50299] -> gate fans out [V doc gate 4] -> each server `LocalStartCrusadeMode`: mode
on, `m_iCrusadeWinnerSide` set to 0 (the shop discount keys on this field, so it lapses for the whole war, see shop.md;
`m_iWinnerSide` is the separate field that keeps the loser head start in F2), GUID file, every client duty 0 / points 0 / GUID stamped, `NOTIFY_CRUSADE` [V 47916-47933];
`RestoreStrikePoints` on every map, `CreateCrusadeStructures` (Crusade.cfg structures on maps this server hosts, 3 random
mana stones) [V 47936-47940] -> client: banner, duty dialog 33, non-commanders request the Middleland map status
[V C 30577-30600].

**F2 Duty.** Dialog 33 -> `REQUEST_SELECTCRUSADEDUTY` -> commander needs rank 0; first-time commander of the side that lost
the last war starts with 3000 construction points (DEF_V219) [V S 49046-49060] -> `NOTIFY_CRUSADE` with duty -> client
opens dialog 36/37/38.

**F3 Points.** Killing an enemy player in crusade: exp, contribution += reward x5, construction points += victim level/2
[V S 28127-28138]. Killing an enemy war unit: points/contribution by type (e.g. battle golem 1500/3000) [V 14595-14625];
killing a friendly one zeroes the killer's contribution [V 14640]. Kills by a guild's war unit credit that guild's
commander on this server, else GSM_CONSTRUCTIONPOINT finds him on another server [V 14650-14690, 19477-19510]. An engineer
completing a structure gets 500-700 contribution [V 12744-12752].

**F4 Build and summon.** Commander sets the construct point (and guild teleport point) on the Middleland map ->
`RequestSetGuild*LocHandler` stores it and sends GSM to other servers [V S 48453-48475]. Commander/engineer
`SUMMONWARUNIT`: enough points for the type [V 48722]; the handler accepts a count up to 10 but then sets it to 1, so one
unit or structure is made per request [V 48717, 48727]; not a fixed-day map [V 48724]; structures (types 36-39) need the
caller within 10 tiles of the construct point and fewer than 10 buildings for the guild [V 48815-48853]; units need the
guild entry and fewer than 500 units [V 48855-48875]; NPC created in follow or guard mode, points deducted
[V 48912-48927]. Members use `GUILDTELEPORT` to jump to the guild point [V 48676-48688].

**F5 Mana and meteor.** Mana collector next to a mana stone adds up to 3 per behaviour tick to `m_iCollectedMana[side]`
and regenerates friendly MP [V doc S 47753-47770] -> OnTimer `SendCollectedMana` -> GSM_COLLECTEDMANA -> every server adds
to `m_iAresdenMana`/`m_iElvineMana` if it hosts that town [V doc 48173-48195] -> the side's Grand Magic Generator (in the
town map) turns each pool > 15 into +1 mana stock (surplus lost); stock > NPC max mana -> `_GrandMagicLaunchMsgSend` +
`MeteorStrikeMsgHandler(side)` [V 48202-48236] -> warn every player (code 1 in the target town, 2 elsewhere), delay event
5 s [V 48251-48270] -> `MeteorStrikeHandler` damages a random intact strike point, `DoMeteorStrikeDamageHandler` hurts
players with a side in that map [V doc] -> `CalcMeteorStrikeEffectHandler`: if no strike point remains, the town's owner
loses: `LocalEndCrusadeMode(winner)` + GSM_ENDCRUSADE; else GSM_GRANDMAGICRESULT to show damage [V 49935-49994].

**F6 End and reward.** Every server `LocalEndCrusadeMode`: remove structures, kill war units, write GUID file with winner,
duties/points reset, `NOTIFY_CRUSADE` with winner, recall hunters [V S 47956-47979]. Each client's exp-stock tick runs
`CheckCrusadeResultCalculation`: if the character's GUID matches the last crusade: draw -> stock += contribution/6;
winner side -> level bonus + contribution x 1.2; loser -> level bonus + contribution/5 (the 300,000 clamp is applied
before the level bonus is added; a draw has no level bonus; [CRUSADE-005](../issues/crusade.md#crusade-005-reward-arithmetic-drift)) [V 47846, 47855-47868]; mismatch -> notify only; then
contribution, duty and GUID cleared [V 47844-47904].

## 7. Rules and invariants

| Rule | Value | Where |
|---|---|---|
| starter | only the server whose name matches WarSchedule.cfg `game-server-name` | S 50729-50761 [V doc] |
| start guard | refused when `m_iLatestCrusadeDayOfWeek` equals today (see 9.2) | S 50282-50285 [V] |
| commander | guild rank 0 | S 49046 [V] |
| loser head start | 3000 construction points to a first-time commander of last war's losing side | S 49051-49060 [V] |
| construction cap | 30000 points | S Game.h:199 [V] |
| contribution cap | 500000 while fighting; 300000 at payout | S Game.h:201; S 47846 [V] |
| enemy PK reward | contribution += exp reward x 5; points += victim level / 2 | S 28130-28135 [V] |
| friendly war-unit kill | contribution := 0 | S 14640 [V] |
| build radius | 10 tiles from the construct point (types 36-39) | S 48821-48823 [V] |
| buildings per guild | 10 (7 Taiwan) | S 48837 [V] |
| units per guild | 500 (5 Taiwan) | S 48864 [V] |
| engineer kit | duty 2 and crusade mode; no other structure-limited NPC within the 9x9 area; per-map 300 cap | S 47644-47662 [V] |
| GMG | pool > 15 -> stock +1 (pool zeroed); launch when stock > NPC `m_iMaxMana` | S 48202-48212 [V] |
| meteor delay | 5 s after warning | S 48269 [V] |
| meteor player damage | 1d(level) + level, cap 255, halved by protect 2, admins immune | doc S 49873-49884 [V doc] |
| victory | all strike points of a town map destroyed | S 49947-49960 [V] |
| payout | draw /6; win level bonus + x1.2; loss level bonus + /5 (log says /10) | S 47849-47893 [V doc + V] |
| chat secrecy | enemy side cannot hear area/guild/shout chat in crusade | chat doc [V] |
| guild ops | create/disband and secession tickets refused in crusade; joining allowed | guild doc [V] |
| quests | only assign-type 1 quests in crusade | quest doc [V] |

## 8. Couplings

Crusade reaches into:
- **guild** - `m_iGuildRank` (commander), `m_iGuildGUID` keys for `m_pGuildTeleportLoc`, commander credit, help call.
- **combat / pk-law (other cluster)** - kill handlers carry the point/contribution logic (`NpcKilledHandler` 14595-14690,
  `ClientKilledHandler` 19477, the EK reward block 28120-28201); the code applies the /4 crusade exp cut on every map during a crusade, not only crusade maps
  ([COMBAT-006](../issues/combat.md#combat-006-crusade-kill-exp-cut-applies-on-every-map); also game-server/mechanics.md 13.1 and the per-hit /3, game-server/mechanics.md:53); `GetHeroMantleHandler` / `REQ_GETREWARDMONEY` spend EKs.
- **entity / NPC (other cluster)** - structures and units are NPCs (`bCreateNewNpc`, `NpcBehavior_Stop` dispatch,
  `m_cActionLimit`, `m_iManaStock`, `m_iMaxMana`), Npc.cfg names like `AGT-Aresden` (present only in the 3.51 and 3.82 Npc.cfg copies; the 2.03 copy has no structure NPCs, see 9.8).
- **map / world (other cluster)** - strike points and energy-sphere goals in map config; `m_bIsFixedDayMode`; force recall
  (`RecallHunterPlayer`, `SetForceRecallTime`); `m_iMiddlelandMapIndex`.
- **items (other cluster)** - construction kits via `_bDepleteDestTypeItemUseEffect` -> `__bSetConstructionKit`; occupy flag item.
- **character** - `m_iExpStock` payout, level bonus; contribution (civic, quest system) changed by the energy sphere.
- **shop (this cluster)** - the crusade-winner price discount tests `m_iCrusadeWinnerSide == m_cSide`; `LocalStartCrusadeMode`
  zeroes that field, so a neutral (side 0) matches while there is no winner [V S 47918, 862, 16903].
- **chat / quest / guild (this cluster)** - read `m_bIsCrusadeMode` (see their docs).
- **gate (infrastructure)** - nine GSM records; the gate relays blindly.
- **logging (infrastructure)** - `_bCrusadeLog` -> MSGID_GAMECRUSADELOG.
- **client presentation** - dialogs 18/33/36/37/38, top messages, meteor and result screens.

Reached into by: chat, guild, quest, pk-law/exp (crusade flag); GM commands; OnTimer (`CrusadeWarStarter`,
`SendCollectedMana`, `SyncMiddlelandMapInfo`, `EnergySphereProcessor`, `SaveOccupyFlagData`).

## 9. Current behaviour vs intent

Defects are in [issues/crusade.md](../issues/crusade.md); the neutral lines are:

1. Occupy flags are retired but still wired: `GetOccupyFlagHandler` consumes up to 12 EK for a flag item, `__bSetOccupyFlag` returns FALSE before doing anything, save is a no-op and load has no callers ([CRUSADE-001](../issues/crusade.md#crusade-001-occupy-flags-are-retired-but-still-wired)).
2. `m_iLatestCrusadeDayOfWeek` is set only while it is -1 (S 50282-50285), so after the first crusade every later start on that weekday is refused until restart ([CRUSADE-002](../issues/crusade.md#crusade-002-the-once-a-day-start-guard-is-broken)).
3. The command-tool handlers (S 48453-48935) test neither duty, rank nor crusade mode, and `SelectCrusadeDutyHandler` accepts any duty value at any time ([CRUSADE-003](../issues/crusade.md#crusade-003-command-tools-have-no-server-side-role-check)).
4. `REQUEST_HELP` is never sent by the client and `NOTIFY_HELP` is ignored ([CRUSADE-004](../issues/crusade.md#crusade-004-help-call-is-unreachable-from-the-client)).
5. Contribution is capped at 300000 at payout vs 500000 while fighting; the loss log writes /10 vs /5 paid ([CRUSADE-005](../issues/crusade.md#crusade-005-reward-arithmetic-drift)).
6. Mana stone selection marks `iMSIndex[0..2]` unconditionally and writes `iMSSelected[-1]` with fewer than 3 type-42 rows (S 48040-48045) ([CRUSADE-006](../issues/crusade.md#crusade-006-mana-stone-selection-overflow)).
7. The GMG gives the same +1 stock for a pool of 100 as for 16; the surplus is lost (S 48202-48205).
8. The kit path builds Aresden's cannon tower from the literal `"CGT-Aresen"` (S 47670); the other two code paths use `"CGT-Aresden"` (S 48091, 48763); no Npc.cfg copy holds `CGT-Aresen` (2.03 has no crusade-structure NPCs at all; 3.51 names it `CGT-Aresde`; 3.82 `CGT-Aresden`, see section 10) ([CRUSADE-007](../issues/crusade.md#crusade-007-cannon-tower-kit-uses-the-literal-name-cgt-aresen)).
9. Meteor damage formulas are overwritten by `iDice(1, level) + level` ([CRUSADE-008](../issues/crusade.md#crusade-008-meteor-damage-formulas-are-overwritten)).
10. The energy sphere is crusade-adjacent but not crusade-only: it runs on Middleland any time with >= 500 world users [V doc S 47000-47008]; scoring changes civic contribution, not war contribution [V S 46940-46947].
11. The Elvine collected-mana log is guarded by the Aresden value ([CRUSADE-009](../issues/crusade.md#crusade-009-collected-mana-log-guarded-by-the-wrong-sides-value)).

## 10. Open questions

- Map strike-point definitions (map config, not in this tree) [U].
- No official 2.20 Crusade.cfg or WarSchedule.cfg exists; the facts below come from the copies named, per
  `research/shared/formats/config-files.md`.

**Shipped config, as found** (both files are read from disk by the game server, S 5354 and 50691):
- `Crusade.cfg`, 2.03 copy (byte-identical to `sources/hgserver-librehelbreath/bin/config/Crusade.cfg`, the chosen copy):
  16 `crusade-structure` rows, all NPC type 40 (energy shield generator, `ESG-*` in later Npc.cfg copies; S 14592 comment),
  8 on `aresden` and 8 on `elvine`; no type 41 (Grand Magic Generator) or 42 (mana stone) rows [V, measured by script].
  The 3.51 copy (eXtasis-GoldenBoy pack) has 21: the same 16 plus one type 41 per town and 3 type-42 rows on
  `middleland` [V]. So with the 2.03 file the mana-stone selection (S 48040) and Grand Magic Generator have no
  configured structures; they appear only in the 3.51 layout [I].
- `WarSchedule.cfg`: no 2.03 copy. The 3.51 copy (49 bytes) is `game-server-name = middleland`, `schedule = 6 22 30`: one
  slot, weekday 6 (Saturday, Sunday = 0 per S 50679), 22:30, started by the server named `middleland` [V]. It is the
  chosen copy; it is not an official 2.20 value. The 2.03 `gserver.cfg` names its server `EFREET`, so the starter flag is
  set only on a server configured with the name `middleland` [V S 50729].
- `Npc.cfg` and `CGT-Aresen`: see 9.8.
- Which server hosts the Grand Magic Generators and how `m_iMaxMana` of the GMG NPC was tuned (Npc.cfg) [U].
- Whether `m_pGuildTeleportLoc` entries are cleared at war end (not traced; `m_dwTime` suggests a timeout) [U].
- Client handling of `MAPSTATUS` chunks and the minimap icons (client cluster).

## 11. Doc discrepancies

None against the code. Additions to `crusade-quests.md`: the day-guard behaviour (9.2, [CRUSADE-002](../issues/crusade.md#crusade-002-the-once-a-day-start-guard-is-broken)) and the missing server-side role
checks in the command handlers (9.3, [CRUSADE-003](../issues/crusade.md#crusade-003-command-tools-have-no-server-side-role-check)) are not stated there.

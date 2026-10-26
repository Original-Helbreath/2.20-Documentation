# System: entity (players and NPCs as world objects, object IDs, view updates, spawning, AI, summons, taming)

*Cohesive single doc (subsystems share the object model and the broadcast path); Contents block instead of a
split. Written 2026-10-25 by the world-cluster agent.*

Paths and tags as in `map.md`: `SRV` = `sources/hgserver-librehelbreath/src/`, `CLI` = `sources/client-2.20/`,
lines as of 2026-10-25; [V] read in source (or in the reviewed function map named beside it), [I] inferred,
[U] unverified. "npc" = `research/game-server/functions/npc.md`, "mm" = `.../map-movement.md`.

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flows -
7. Rules and invariants - 8. Couplings - 9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose

Everything that stands on a tile and can act is an entity: player characters (`CClient`) and NPCs (`CNpc`:
monsters, guards, shopkeepers, crusade structures, crops, summons). The system gives each one a world id,
tells nearby clients when it appears, acts or disappears, populates maps with monsters that respawn, drives
their behaviour (wander, chase, attack, flee, call for help), and lets players own creatures (summon
scrolls/spells, crusade war units). Taming was planned and left as a stub.

## 2. Subsystems

| Subsystem | Intent |
|---|---|
| 2.1 Object identity | one numeric id per entity on the wire; players = client slot, NPCs = slot + 10000 |
| 2.2 Appear / disappear | `MSGID_EVENT_LOG` CONFIRM/REJECT to neighbours on enter, spawn, teleport, removal |
| 2.3 View snapshots | init (21x16) and strip data carry every visible owner, corpse, item and DO |
| 2.4 Full-object refresh | client asks for an object it does not know |
| 2.5 Relationship bits | upper 4 bits of status = friend/foe as seen by each receiver |
| 2.6 NPC templates | Npc.cfg and NpcItem pushed by the log server |
| 2.7 Spawning | fixed NPCs from map info; random and spot generators; mob events; GM summons |
| 2.8 NPC tick and behaviours | stop / move / attack / flee / dead state machine per NPC |
| 2.9 Targeting and assistance | nearest attackable target; one idle ally answers a call for help |
| 2.10 Movement types | waypoint, random waypoint, follow, random area, random, guard |
| 2.11 Corpse and removal | dead NPC lingers for its regen time, then drops loot and is deleted |
| 2.12 Summons and followers | summoned creatures follow a master, obey free/hold/target, expire after 5 min (Ogre type 29: 90 s) |
| 2.13 Taming | skill-driven conversion of a monster (stub) |
| 2.14 Activity sectors (NPC side) | monster activity feeds the map heat map (map.md 2.9) |

## 3. Functions

| Subsystem | Function | Program, where | Role |
|---|---|---|---|
| 2.1/2.3 | `iComposeInitMapData`, `iComposeMoveMapData` | server, SRV Game.cpp:2252, 3359 [V] | NPC ids written as `sOwner + 10000` (2394, 2494, 3494, 3593); stale handles cleared while composing (3402-3418) |
| 2.2 | `SendEventToNearClient_TypeA` (player and NPC branches) | server, SRV Game.cpp:2890-3357 [V] | NPC id +10000 at 3199 |
| 2.2 | `RequestInitDataHandler` | server, SRV Game.cpp:1611 [V] | places the player (`bGetEmptyPosition`, `SetOwner` 2028-2033), sends init data, EVENT_LOG CONFIRM (2078) |
| 2.2 | `DeleteClient` | server, SRV Game.cpp (2608 area) [V mm callers] | ClearOwner, RemoveFromTarget, EVENT_LOG REJECT |
| 2.4 | `RequestFullObjectData` | server SRV Game.cpp:30349-30490; client CLI Game.cpp:8650-8688 [V mm/map] | resend one object's full appearance as `OBJECTSTOP` |
| 2.4 | `CMapData::bSetOwner` | client, CLI MapData.cpp:892-1643 [V] | places objects; unknown or colliding id -> request full data (1221-1496) |
| 2.5 | `iGetPlayerABSStatus`, `iGetNpcRelationship(_SendEvent)` | server, SRV Game.cpp:37686, 37735, 37772, 37802, 37914 [V names] | per-receiver FOE code |
| 2.5 | `_iGetFOE` | client, CLI Game.cpp:24628 [V map] | colour names from the FOE bits |
| 2.6 | `_bDecodeNpcConfigFileContents`, `_bDecodeNpcItemConfigFileContents` | server, SRV Game.cpp:19677, 25144 [V npc] | templates into `m_pNpcConfigList[100]` |
| 2.7 | `bCreateNewNpc`, `_bInitNpcAttr` | server, SRV Game.cpp:10043-10365, 20037 [V npc + read 10160-10365] | spawn, place, stats, broadcast |
| 2.7 | `MobGenerator`, `SpecialEventHandler`, `_cGetSpecialAbility` | server, SRV Game.cpp:28988, 37990, 44129 [V npc] | top-up spawners, mob events |
| 2.7 | `AdminOrder_Summon/SummonDemon/CallGuard`, `EnergySphereProcessor` | server [V npc callers] | GM / event spawns (gm, crusade clusters) |
| 2.8 | `NpcProcess` | server, SRV Game.cpp:10434-10536 [V npc] | per-NPC timing, regen, expiry, dispatch (called from `GameProcess` 5045) |
| 2.8 | `NpcBehavior_Stop/Move/Attack/Flee/Dead` | server, SRV Game.cpp:30550, 11506, 11805, 14761, 14704 [V npc] | behaviours |
| 2.9 | `TargetSearch`, `iGetDangerValue`, `NpcRequestAssistance`, `RemoveFromTarget`, `bSetNpcAttackMode` | server, SRV Game.cpp:11665, 14722, 38068, 12261, 35934 [V npc] | target logic |
| 2.10 | `CalcNextWayPointDestination`, `cGetNextMoveDir` (7-arg), `bSetNpcFollowMode`, `ReleaseFollowMode` | server, SRV Game.cpp:29960, 11437, 9985, 24158 [V npc] | movement |
| 2.11 | `NpcKilledHandler` (combat-owned), `NpcBehavior_Dead`, `DeleteNpc` | server, SRV Game.cpp:14449, 14704, 30033 [V npc] | death, corpse, loot, slot release |
| 2.12 | `PlayerMagicHandler` case `DEF_MAGICTYPE_SUMMON` | server, SRV Game.cpp:21999-22080 [V] | summon creature for a same-side player |
| 2.12 | `SetSummonMobAction`, `iGetFollowerNumber` | server, SRV Game.cpp:39893-39976, 32975 [V] | `/free`, `/hold`, `/tgt <name>` |
| 2.13 | `iCalculateUseSkillItemEffect` -> `_TamingHandler` | server, SRV Game.cpp:33700-33799, 46085-46142 [V] | stub |
| client | `LogEventHandler`, `MotionEventHandler`, `DrawObjects`, `DrawNpcName`, `GetNpcName` | CLI Game.cpp:14610, 46077, 2278, 33409, 34412 [V map] | show entities |

## 4. Commands and messages

From by-system.md (entity row) [V per doc]: `MSGID_EVENT_LOG` (appear/disappear), `MSGID_EVENT_MOTION`
(all actions incl. `OBJECTNULLACTION` appearance refresh, `OBJECTDYING`, `OBJECTDEAD`), `MSGID_EVENT_COMMON`,
`MSGID_REQUEST_FULLOBJECTDATA`, `MSGID_DYNAMICOBJECT` (tile.md), `DEF_COMMONTYPE_TALKTONPC`,
`DEF_NOTIFY_MONSTEREVENT_POSITION`, `DEF_NOTIFY_NPCTALK` (quest). Config push `MSGID_NPCCONFIGURATIONCONTENTS`,
`MSGID_NPCITEMCONFIGCONTENTS` (log -> game). Chat-driven: `/hold`, `/free`, `/tgt <name>` (summon control,
via `ChatMsgHandler`); GM `/summon`, `/summondemon`, `/unsummonall`, `/unsummonboss`, `/attack`, `/gns`,
`/polymorph` (dead). Crusade `DEF_COMMONTYPE_SUMMONWARUNIT`.

## 5. Data

- `CNpc` (SRV Npc.h; `research/game-server/data-structures.md` CNpc): type, name (map-unique naming value
  `_<mapletter><n>`), side (1/2 town, 10 monster), HitDice/ExpDice, move type 0-6, behaviour 0-4,
  `m_cActionLimit` (0 normal, 2/3 shopkeepers, 5 stationary structures), special ability 1-8, attack
  strategy, `m_iAILevel`, follow owner index/type, `m_bIsSummoned`, `m_dwSummonedTime`,
  `m_iSummonControlMode` (0 free, 1 hold, 2 target), `m_bIsPermAttackMode`, guild GUID (war units),
  crop fields `m_iNpcCrops`, `m_iCropsSkillLV`, `m_iBuildCount`, NpcItem list.
- `CClient` entity fields: x/y/dir, type, appearance 1-4, colour, status (low 12 bits effects, high 4 FOE).
- CGame: `m_pNpcList[5000]`, `m_pClientList[2000]`, `m_iClientShortCut[2001]` (dense live list used by
  broadcasts), `m_pNpcConfigList[100]`, per-map `m_iTotalActiveObject`, `m_iMaximumObject`, naming values 0-999.
- Config: Npc.cfg and NpcItem (via log server), map info `npc`, `random-mob-generator`, `spot-mob-generator`,
  `maximum-object`, `npc-avoidrect`, `mobevent-amount` (`research/shared/formats/mapdata-txt.md`).
- Not persisted: NPCs are rebuilt from config at every start [I: no NPC save path found in persistence docs].

## 6. Flows

### 6.1 Player appears to others
`RequestInitDataHandler`: position via `bGetEmptyPosition` unless observer (1954-1957), `SetOwner`
(2028-2033), `MSGID_RESPONSE_INITDATA` with `iComposeInitMapData(x-10, y-7)` (2060), then
`SendEventToNearClient_TypeA(..., MSGID_EVENT_LOG, DEF_MSGTYPE_CONFIRM)` (2078) [V]. Receivers place the
object via `LogEventHandler` -> `bSetOwner` [V map].

### 6.2 Monster spawn
`OnTimer` -> `MobGenerator` every 600 ms (npc) -> per map, if alive objects < cap, pick a level-based
monster, `iGetEmptyNamingValue`, `bCreateNewNpc` [V npc]. `bCreateNewNpc` finds the template, checks the
day-of-week limit, places with `bGetEmptyPosition` (10160-10165), refuses in hide-gen mode if a player is
within 7 tiles (10167-10172), counts the object, registers crusade structures or crops (10345-10357) and
broadcasts `EVENT_LOG CONFIRM` (10360) [V]. Crops (type 64) return early at 10353 and skip that broadcast; `__bSetAgricultureItem` sends it itself (S :53047) [V].

### 6.3 Monster life
`NpcProcess` (npc) runs each NPC when its action time elapses: regen, summon/crop expiry, then behaviour.
Move: follow master / waypoint / random, look for a target with `TargetSearch`; Attack: melee, ranged or
magic by type, chase with `cGetNextMoveDir`, flee when danger > bravery or HP <= 2; Flee: run, 50 % chance
to call one idle same-side ally (`NpcRequestAssistance`), then return; Dead: wait regen time then
`DeleteNpc` [V npc]. Every step broadcasts `EVENT_MOTION` with the NPC id +10000 (3199) [V].

### 6.4 Death and removal
Combat calls `NpcKilledHandler` (combat cluster) -> `SetDeadOwner`, `OBJECTDYING`, behaviour DEAD [V mm
callers]. After `m_dwRegenTime`, `DeleteNpc`: clear tile, `RemoveFromTarget`, release naming value, adjust
spot-generator / crop / guild counters, maybe one drop (alchemy table, NpcItem, or gold for types 30-32),
broadcast `EVENT_LOG REJECT` (npc DeleteNpc) [V npc].

### 6.5 Summon by spell or scroll
`PlayerMagicHandler` case SUMMON (21999): not in a fight zone; the target tile must hold a player of the
caster's side (may be the caster); follower count of that player < caster's `m_cSkillMastery[4] / 20`
(22013); name `_<map><n>`; creature picked by `iV1` (scroll type 1-8: Orc ... Ogre "Orge") or, for a plain
cast, by `max(1d(skill/10), skill/20)` mapped to Slime ... Cyclops (22029-22058); `bCreateNewNpc` with
summoned flag, then follow mode on the target player [V 21999-22061; follow per npc callers]. Control: chat
`/hold`, `/free`, `/tgt <name>` -> `SetSummonMobAction` (neutrals refused, 39902) [V]. `/tgt` accepts only a player on the same map whose side is non-neutral and different from the caller's (S :39945, :39956-39957) [V]. Expiry after
`DEF_SUMMONTIME` 5 min in `NpcProcess` -> `NpcKilledHandler` (npc) [V npc]; Ogre (type 29) summons expire after 90 s (S :10519-10521) [V].

## 7. Rules and invariants

- Ids: players 1..1999, NPCs 10000 + slot (server 3199; client tests `< 10000`, CLI Game.cpp:14464) [V].
- Broadcast range +-10/+-8 (+1 for movement/dying/log events) (2927-2931, 3050-3054) [V].
- Naming values 0-999 per map (mm `iGetEmptyNamingValue`) [V mm]; NPC slots 5000, NPC templates 100.
- Spot generators: 1/3 chance per spot per tick, only while active objects < map maximum (npc) [V npc].
- Mob event flag every 300 s; demon/unicorn variant 1 in 350 (npc SpecialEventHandler) [V npc].
- NPC attack tick shortened up to 700 ms, floor 600 ms; frozen NPCs 50 % slower (npc NpcProcess) [V npc].
- GMs are never targeted by monsters; invisible targets need special ability 1 (npc TargetSearch) [V npc].
- Summon cap: followers < summon-skill / 20 (22013) [V]; not in fight zones (22003) [V]; lifetime 5 min (Ogre 90 s, S :10519-10521) [V npc].
- Experience from monsters: paid as damage is dealt (settled fact; combat cluster).

## 8. Couplings

Out: tile (SetOwner/ClearOwner/GetOwner, bGetEmptyPosition), map (map info spawns, sector counters,
naming values, `m_iMaximumObject`), combat (`iCalculateAttackEffect`, `NpcMagicHandler`, `NpcKilledHandler`),
magic (summon, invisibility handled in `RemoveFromTarget` iCode), items (`DeleteNpc` drops via
`bGetItemNameWhenDeleteNpc`, `NpcItem`), crusade (structures 36-42, war units with guild GUID,
`bAddCrusadeStructureInfo`, energy sphere), gathering (crops are NPC type 64), quest (`NpcTalkHandler`,
monster-hunt counts in kill handler), gm (AdminOrder_* spawns), infrastructure (config push from log server).
In: movement/map (`RemoveFromTarget` on teleport, disconnect), magic (`PlayerMagicHandler` summon, invisibility
cast calls `RemoveFromTarget` 22218), character (`ResurrectPlayer` -> `RemoveFromTarget` 50860), chat
(`ChatMsgHandler` -> `SetSummonMobAction`), skills (`iCalculateUseSkillItemEffect` -> `_TamingHandler`).

## 9. Current behaviour vs intent

- Taming is a stub (both branches of the level test end in `break`) and `iCalculateUseSkillItemEffect` only reaches it for a water target tile after a 1d105 roll ([ENTITY-001](../issues/entity.md#entity-001-taming-is-a-stub-and-unreachable-on-land)).
- `RemoveFromTarget` turns every guild war unit into a 5-minute summon whenever any player of that guild GUID is removed as a target ([ENTITY-002](../issues/entity.md#entity-002-removefromtarget-turns-every-guild-war-unit-into-a-5-minute-summon)).
- `bSetNpcFollowMode` / `bSetNpcAttackMode` find the NPC by the first 5 bytes of the name with an uninitialised index ([ENTITY-003](../issues/entity.md#entity-003-bsetnpcfollowmode-and-bsetnpcattackmode-use-an-uninitialised-index)).
- `SetSummonMobAction` target mode copies `dwMsgSize` bytes into `cBuff[256]` unbounded and leaves `cTargetName[11]` unzeroed ([ENTITY-004](../issues/entity.md#entity-004-setsummonmobaction-target-mode-unbounded-copy-and-unzeroed-name-buffer)).
- `bCreateNewNpc` is declared `BOOL` but returns the slot index; `__bSetAgricultureItem` relies on that (gathering.md) ([ENTITY-005](../issues/entity.md#entity-005-bcreatenewnpc-declared-bool-but-returns-the-slot-index)).
- `MobGenerator` user-count scaling is inactive and overwritten; the middleland-during-crusade `break` stops generation on every later map ([ENTITY-006](../issues/entity.md#entity-006-mobgenerator-user-count-scaling-inactive-crusade-break-stops-generation-on-later-maps)).
- `CalcNextWayPointDestination` divides by zero with one waypoint or a zero-width area ([ENTITY-007](../issues/entity.md#entity-007-calcnextwaypointdestination-divides-by-zero)).
- `bSerchMaster` (master/follower packs) has its only call commented out ([ENTITY-008](../issues/entity.md#entity-008-bserchmaster-masterfollower-packs-is-dead)).
- Stale tile handles are repaired lazily inside the compose functions (3401-3418) ([ENTITY-009](../issues/entity.md#entity-009-stale-tile-owner-handles-repaired-lazily-at-read-time)).

## 10. Open questions

- Full text of `iGetNpcRelationship` vs `_SendEvent` variants and how town guards pick sides [U, not read].
- NPC corpse regen time source (Npc.cfg field) [U].
- What `DEF_OWNERTYPE_PLAYER_INDIRECT` (3) is used for, presumably summon kill credit [U].

## 11. Doc discrepancies

None found against npc.md in the parts re-read (summon, taming entry, bCreateNewNpc tail).

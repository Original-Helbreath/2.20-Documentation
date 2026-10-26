# System: tile (tile grid, flags, occupancy, items and dynamic objects on tiles)

*Cohesive single doc; Contents block instead of a split. Written 2026-10-25 by the world-cluster agent.*

Paths and tags as in `map.md`: `SRV` = `sources/hgserver-librehelbreath/src/`, `CLI` = `sources/client-2.20/`,
lines as of 2026-10-25; [V] read in source (or in the reviewed function map named beside it), [I] inferred,
[U] unverified. "mm" = `research/game-server/functions/map-movement.md`.

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flows -
7. Rules and invariants - 8. Couplings - 9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose

The tile is the unit of the world: one character or monster per tile, a pile of dropped items, at most one
field effect (fire, poison cloud, ice storm, spike, fish shadow, ore vein, crusade flag), and fixed flags
from the map file (blocked, portal, water, farmland) plus attributes stamped at load (safe zone). Every
other world system asks the tile "can I stand here, who is here, what is lying here". The player sees it as
collision, item piles, burning ground and safe towns.

## 2. Subsystems

| Subsystem | Intent |
|---|---|
| 2.1 Static flags | walkable / portal / water / farmable from `.amd` byte 8 and sprite 19 |
| 2.2 Attributes | runtime bits: safe zone (0x4) from `no-attack-area`; bits 1/2 magic/physical protection (crusade) |
| 2.3 Occupancy | live owner (handle + class) and a separate dead owner (corpse / feigning death) per tile |
| 2.4 Item stack | up to 12 items per tile, LIFO, oldest dropped when full |
| 2.5 Dynamic objects | timed field effects registered in a global list and mirrored on the tile |
| 2.6 Temporary block | per-tile "temp move allowed" flag so minerals (and structures) block walking |
| 2.7 Occupy / territory | crusade occupy flags and a per-tile occupy tendency value |
| 2.8 Client mirror | the client's own cell window (`CMapData`) rebuilt from server snapshots and events |

## 3. Functions

| Subsystem | Function | Where | Role |
|---|---|---|---|
| 2.1 | `CMap::_bDecodeMapDataFileContents` | SRV Map.cpp:410-512 [V] | sets m_bIsMoveAllowed (0x80 clear), m_bIsTeleport (0x40), m_bIsFarmingAllowed (0x20), m_bIsWater (sprite 19) |
| 2.1 | `bGetIsTeleport`, `bGetIsWater`, `bGetIsFarm` | SRV Map.cpp:270, 581, 596 [V mm] | flag readers, 14/16/12/14 edge margin |
| 2.2 | `_SetupNoAttackArea`, `iGetAttribute` | SRV Map.cpp:684-707, 709-717 [V mm] | stamp / read attribute bits |
| 2.3 | `SetOwner`, `GetOwner`, `ClearOwner`, `SetDeadOwner`, `GetDeadOwner`, `ClearDeadOwner` | SRV Map.cpp:180-313 [V mm] | occupancy API; ClearOwner runs two independent checks: the owner is cleared if handle and class match, and the dead owner is cleared if its own handle and class match |
| 2.3 | `bGetMoveable` | SRV Map.cpp:238-254 [V] | the walk test: edge band 20, no owner, walkable, not temp-blocked; returns tile DO type |
| 2.3 | `bGetIsMoveAllowedTile` | SRV Map.cpp:256-268 [V mm] | same without the owner test (spell targeting, fire placement) |
| 2.3 | `bGetEmptyPosition` | SRV Game.cpp:11481-11504 [V] | 25-offset search for a free non-portal tile, else initial point |
| 2.4 | `bSetItem`, `pGetItem`, `iCheckItem` | SRV Map.cpp:315-383 [V mm] | item stack |
| 2.5 | `iAddDynamicObjectList` | SRV Game.cpp:28579-28640 [V] | placement rules, global slot, tile mirror, broadcast |
| 2.5 | `CheckDynamicObjectList` | SRV Game.cpp:28642-28698 [V mm] | expiry; rain shortens fire |
| 2.5 | `DynamicObjectEffectProcessor` | SRV Game.cpp:33283-33671 [V mm] | damage for poison cloud, ice storm, fire occupants |
| 2.5 | `SetDynamicObject`, `bGetDynamicObject` | SRV Map.cpp:532-560 [V mm] | tile mirror |
| 2.6 | `SetTempMoveAllowedFlag` | SRV Map.cpp:627-635 [V mm] | block/unblock (minerals) |
| 2.7 | `iRegisterOccupyFlag`, `__bSetOccupyFlag`, `_DeleteRandomOccupyFlag` | SRV Map.cpp:637; Game.cpp (crusade) [V mm] | crusade territory (crusade cluster) |
| 2.8 | `CMapData::bSetOwner`, `bSetItem`, `bSetDynamicObject`, `bSetDeadOwner`, `ShiftMapData`, `bGetIsLocateable`, `bIsTeleportLoc` | CLI MapData.cpp:892, 3385, 3587, 3422, 801, 865, 882 [V map] | client cell window |
| 2.8 | `_ReadMapData`, `DynamicObjectHandler` | CLI Game.cpp:14417, 22854 [V map] | decode server tile snapshots / DO events |

## 4. Commands and messages

- `MSGID_DYNAMICOBJECT` CONFIRM (created) / REJECT (removed) via `SendEventToNearClient_TypeB` (Game.cpp:28633, mm) [V].
- Tile snapshot inside `MSGID_RESPONSE_INITDATA`, `MSGID_RESPONSE_MOTION`/`DEF_OBJECTMOVE_CONFIRM`, `MSGID_RESPONSE_PANNING`.
- Item piles: `MSGID_EVENT_COMMON` `DEF_COMMONTYPE_ITEMDROP` / `SETITEM` (item cluster owns them).
- Notifies `DEF_NOTIFY_NEWDYNAMICOBJECT` / `DELDYNAMICOBJECT` exist but are not live (by-system.md entity row, "(x)").

## 5. Data

- `CTile` (SRV Tile.h) [V]: `m_cOwnerClass`/`m_sOwner`, `m_cDeadOwnerClass`/`m_sDeadOwner`, `m_pItem[12]`
  (`DEF_TILE_PER_ITEMS`), `m_cTotalItem`, `m_wDynamicObjectID`, `m_sDynamicObjectType`,
  `m_dwDynamicObjectRegisterTime`, `m_bIsMoveAllowed`, `m_bIsTeleport`, `m_bIsWater`, `m_bIsTempMoveAllowed`,
  `m_bIsFarmingAllowed` (v2.19), `m_iOccupyStatus` (Aresden negative, Elvine positive), `m_iOccupyFlagIndex`,
  `m_iAttribute` (bit 1 magic protection, bit 2 physical protection, bit 3 = 0x4 safe zone).
- Owner classes: 1 player, 2 NPC, 3 player-indirect (SRV Map.h:19-21) [V].
- Dynamic object types (SRV DynamicObjectID.h:2-14) [V]: 1 fire, 2 fish, 3 fish object, 4 mineral1 (ore),
  5 mineral2 (gem), 6/7 Aresden/Elvine flag, 8 ice storm, 9 spike, 10-12 poison cloud begin/loop/end, 13 fire2.
- Global list `m_pDynamicObjectList[60000]` (`DEF_MAXDYNAMICOBJECTS`, Game.h:77); the slot index is the
  wire id [V].
- Files: `.amd` byte 8 flags (`research/shared/formats/amd-map.md`).

## 6. Flows

### 6.1 Field effect placed (e.g. Fire Field spell)
Magic handler -> `iAddDynamicObjectList` (Game.cpp:28579): refuses if the tile already has a DO (28586-28587);
fire needs a move-allowed tile and its lifetime is cut by rain (status 1: x/2, 2: x/6, 3: x/12,
28590-28602); fish needs water; minerals need a moveable tile and set the temp block (28609-28614);
first free slot from 1; +1d4 s jitter on timed objects; tile mirror; `MSGID_DYNAMICOBJECT` CONFIRM to
clients within +-10/+-8 of the tile (28618-28631) [V]. Timer: `DynamicObjectEffectProcessor` damages
occupants; `CheckDynamicObjectList` expires them and broadcasts REJECT (mm) [V mm]. Client:
`DynamicObjectHandler` -> `CMapData::bSetDynamicObject` (CLI Game.cpp:22854) [V map].

### 6.2 Occupancy on a step
`iClientMotion_Move_Handler`: `bGetMoveable(dest)` -> `ClearOwner(old)` -> `SetOwner(new)` (SRV Game.cpp:1221-1237) [V];
spike under the new tile is applied in the same step (1239-1250) [V]. NPC moves do the same in
`NpcBehavior_Move/Attack/Flee` (mm SetOwner callers) [V mm].

## 7. Rules and invariants

- One live owner per tile; `bGetMoveable` refuses an occupied tile (Map.cpp:249) [V]. Dead owners do not block.
- Walkable band: x and y in [20, size-20) (Map.cpp:242) [V]; flag readers use 14/16/12/14 (mm) [V mm].
- 12 items per tile; pushing a 13th deletes the bottom one (mm bSetItem) [V mm].
- One dynamic object per tile (28586) [V].
- Spike: 2d4 on entry, not for neutral players in peace mode, admins take none (1238-1249) [V].
- Safe zone bit 0x4 covers each `no-attack-area` rect, or the whole map when the rect top is -10 (mm) [V mm].

## 8. Couplings

Out: none of note; the tile is a leaf store. In (who reads/writes tiles): movement (`bGetMoveable`,
`SetOwner`), entity (NPC placement, `bGetEmptyPosition`, `GetOwner` in `TargetSearch`, `_TamingHandler`),
combat/magic (`GetOwner`, `GetDeadOwner`, `iGetAttribute` for safe zone and protection, Effect_Damage_Spot,
meteor strike), items (`bSetItem`/`pGetItem` on drop, pickup, death drops, crafted items dropped on the
floor), gathering (water/farm flags, fish and mineral DOs), crusade (occupy flags, structures, strike
points, `m_iOccupyStatus` sent in every move confirm at 1357-1359), trade (`GetOwner` in give/exchange).

## 9. Current behaviour vs intent

- The tile index is `x + y*m_sSizeY` everywhere (Map.cpp:186-715, Game.cpp:1358, 2268, 3375, 37377, 39522...); self-consistent for square maps, aliasing and overrunning for non-square ones ([TILE-001](../issues/tile.md#tile-001-tile-index-is-x--ysizey-wrong-for-non-square-maps)).
- `bGetMoveable(..., CItem* pTopItem)` assigns to its by-value parameter, and the consuming code in the move handler is commented out (Game.cpp:1252-1307): the land-mine reagents (items 540/541) have no effect ([TILE-002](../issues/tile.md#tile-002-bgetmoveable-assigns-to-its-by-value-ptopitem-parameter-land-mine-feature-unfinished)).
- Spike damage can take HP to 0; the move handler clamps HP at 0 and the caller then runs `ClientKilledHandler(..., 1)` (Game.cpp:1309, 1045/1056) [V], so spikes kill, attributed to no attacker.
- `ClearDeadOwner` and `ClearSectorInfo` have no callers ([TILE-003](../issues/tile.md#tile-003-cleardeadowner-and-clearsectorinfo-have-no-callers)).
- The server reports `m_iOccupyStatus` in every move confirm; the client ignores it ([TILE-004](../issues/tile.md#tile-004-client-ignores-the-occupy-status-in-move-confirms)).
- `DEF_NOTIFY_NEWDYNAMICOBJECT` / `DELDYNAMICOBJECT` are never sent; `MSGID_DYNAMICOBJECT` is used instead ([TILE-005](../issues/tile.md#tile-005-newdynamicobject-and-deldynamicobject-notifies-are-never-sent)).

## 10. Open questions

- Which tile attribute bits 1/2 (magic/physical protection) are set and by what (crusade cluster) [U].
- Whether any retail `.amd` is non-square (only arebrk11 measured, 150x150) [U].

## 11. Doc discrepancies

None found against `map-movement.md` for tile functions.

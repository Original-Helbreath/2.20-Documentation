# System: movement (walk, run, turn, collision, position sync, client prediction)

*Cohesive single doc; Contents block instead of a split. Written 2026-10-25 by the world-cluster agent.*

Paths and tags as in `map.md`: `SRV` = `sources/hgserver-librehelbreath/src/`, `CLI` = `sources/client-2.20/`,
lines as of 2026-10-25; [V] read in source (or in the reviewed function map named beside it), [I] inferred,
[U] unverified. "mm" = `research/game-server/functions/map-movement.md`.

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flows -
7. Rules and invariants - 8. Couplings - 9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose

Movement is tile-to-tile in 8 directions. The client moves its own character at once (no waiting for the
server) so walking feels immediate over a 1999 dial-up link; the server is the authority, accepts or rejects
each step, and on a reject snaps the client back. Each accepted step also streams the newly visible strip of
the world, so the client's 21x16 window is always filled. Running costs stamina. The same pipeline carries
the "move-like" motions: knock-back (damage move), dash attack (attack move) and turning on the spot.
Several timing checks exist to catch speed hacks.

## 2. Subsystems

| Subsystem | Intent |
|---|---|
| 2.1 Client path step | pick the next walkable direction towards the click, with turn preference and memory of a blocked step |
| 2.2 Client prediction | apply the step locally and send it; limit unacknowledged commands to 6 |
| 2.3 Animation gate | the next command may be issued only when the current animation finishes |
| 2.4 Server validation | coordinate match, direction range, walk test, hold-person, then commit |
| 2.5 Move confirm + view strip | `OBJECTMOVE_CONFIRM` carries SP cost, occupy value, HP and the new strip |
| 2.6 Reject and resync | `OBJECTMOVE_REJECT` / `OBJECTMOTION_REJECT` restore the server's position |
| 2.7 Neighbour broadcast | other clients in range see the motion (`MSGID_EVENT_MOTION`) |
| 2.8 Stamina | run costs 1 SP per step unless "firm stamina" is active |
| 2.9 Speed-hack checks | server-clock 7-step windows, client-clock 250 ms gap, SP below -10 |
| 2.10 Turn/stop | change direction without moving |
| 2.11 Observer panning | GM observer moves the view without a body |

## 3. Functions

| Subsystem | Function | Program, where | Role |
|---|---|---|---|
| 2.1 | `CGame::cGetNextMoveDir` (7-arg overload is server NPC; this is client) | client, CLI Game.cpp:1338-1400 [V map] | tries the base dir, then one side only: +1, +2 when the player turn flag is 0, -1, -2 when it is 1; refuses portal tiles that `_bCheckMoveable` forbids |
| 2.1 | `_bCheckMoveable` | client, CLI Game.cpp:1402-1429 [V] | hunters (civilian mode) may not use the town -> middleland portals outside crusade (hard-coded coords) |
| 2.1 | `CMapData::bGetIsLocateable`, `bIsTeleportLoc` | client, CLI MapData.cpp:865, 882 [V map] | local walk test (no owner, walkable, no mineral) |
| 2.1 | `GetPlayerTurn` | client, CLI Game.cpp:17390 [V map] | simulates both turn preferences to choose the shorter |
| 2.2 | `CommandProcessor` move/run branch | client, CLI Game.cpp:36913-36970 [V] | run downgrades to walk at SP < 1 (36938); step, send, move self, lock command |
| 2.2 | `bSendCommand(MSGID_COMMAND_MOTION, ...)` | client, CLI Game.cpp:1431-2275 [V map] | packs x, y, dir, dx, dy, type, target, client time; `m_cCommandCount++` (2237) |
| 2.3 | `UpdateScreen_OnGame` -> `iObjectFrameCounter` == 2 | client, CLI Game.cpp:35407-35414 [V] | re-enables commands when the player's animation ends |
| 2.4 | `ClientMotionHandler` | server, SRV Game.cpp:976-1127 [V] | decodes and dispatches by motion type |
| 2.4 | `iClientMotion_Move_Handler` | server, SRV Game.cpp:1128-1498 [V] | validation, speed check, commit, confirm/reject |
| 2.5 | `iComposeMoveMapData` | server, SRV Game.cpp:3359-3664 [V] | only the tiles that became visible for that direction (`_tmp_iMoveLocX/Y`) |
| 2.5 | `MotionResponseHandler` | client, CLI Game.cpp:35498-35697 [V] | confirm: SP, HP, `ShiftMapData` + `_ReadMapData`; reject: restore |
| 2.6 | `SendObjectMotionRejectMsg` | server, SRV Game.cpp:33236 [V name] | for stop/attack/get-item/magic mismatches |
| 2.7 | `SendEventToNearClient_TypeA` | server, SRV Game.cpp:2890-3357 [V] | range +-10/+-8, +1 for moves; full vs short packet by +-9/+-7 |
| 2.7 | `MotionEventHandler` -> `CMapData::bSetOwner` | client, CLI Game.cpp:46077; MapData.cpp:892 [V map] | apply others' motion; unknown object -> `RequestFullObjectData` |
| 2.9 | `bCheckClientMoveFrequency` | server, SRV Game.cpp:52674-52714 [V] | client-time gap < 250 ms -> disconnect |
| 2.10 | `iClientMotion_Stop_Handler` | server, SRV Game.cpp:16475-16545 [V mm] | turn; also kicks a feign-death player standing up under someone |
| 2.11 | `iRequestPanningMapDataRequest` / `ResponsePanningHandler` | server SRV Game.cpp:46156; client CLI Game.cpp:24670 [V mm/map] | observer strip |

## 4. Commands and messages

From by-system.md (movement row) [V per doc]: client -> server `MSGID_COMMAND_MOTION` with motion type
`DEF_OBJECTSTOP`, `OBJECTMOVE`, `OBJECTRUN`, `OBJECTDAMAGEMOVE`, `OBJECTATTACKMOVE` (plus attack/magic/get-item
owned by other systems); server -> client `MSGID_RESPONSE_MOTION` with `OBJECTMOVE_CONFIRM`/`_REJECT`,
`OBJECTMOTION_CONFIRM`/`_REJECT`; broadcast `MSGID_EVENT_MOTION`; observer `MSGID_REQUEST_PANNING` /
`RESPONSE_PANNING`; `DEF_NOTIFY_DAMAGEMOVE` (combat) triggers the client's knock-back step. Slash
`/setobservermode` (GM). Wire layouts: `research/shared/messages/client-game.md`.

## 5. Data

- Client: `m_sPlayerX/Y`, `m_cPlayerDir`, `m_cCommand`, `m_sCommX/Y` (target), `m_cCommandCount`
  (unacked motions), `m_bCommandAvailable`, `m_dwCommandTime`, `m_bIsPrevMoveBlocked`, `m_iPrevMoveX/Y`,
  `m_cPlayerTurn`, `m_iNetLagCount`, view point `m_sViewPointX/Y` (pixels, (x-10)*32 / (y-7)*32).
- Server CClient: `m_sX/m_sY/m_cDir`, `m_iMoveMsgRecvCount`/`m_dwMoveLAT`, `m_iRunMsgRecvCount`/`m_dwRunLAT`,
  `m_dwMoveFreqTime`, `m_bIsMoveBlocked`, `m_bIsAttackModeChange`, `m_iSP`, `m_iTimeLeft_FirmStaminar`,
  `m_dwLastActionTime`, `m_cMagicEffectStatus[DEF_MAGICTYPE_HOLDOBJECT]`.
- Direction codes 1..8 = N, NE, E, SE, S, SW, W, NW (switch at SRV Game.cpp:1201-1210) [V].
- No config files; constants are inline.

## 6. Flows

### 6.1 Walk step (happy path)
1. Click: `CommandProcessor` sets `m_cCommand = DEF_OBJECTMOVE`/`RUN` and target; when commands are available
   and fewer than 6 are in flight (36852-36854), `cGetNextMoveDir` picks a direction (36941) [V].
2. Client sends `MSGID_COMMAND_MOTION` (36945), moves `m_sPlayerX/Y` immediately, `bSetOwner` itself in the
   new cell, sets `m_bCommandAvailable = FALSE` (36946-36962) [V]. This is the prediction.
3. Server `ClientMotionHandler` decodes (976-1022) and calls `iClientMotion_Move_Handler` (1050-1051) [V].
4. Checks: direction 1..8 (1139), alive and init-complete, start x/y equals the server's x/y, else return 2
   (silently ignored for moves) (1144); 7-step timing window (1147-1177); sector activity counters
   (1181-1193); `ClearSkillUsingStatus` (1197); `bGetMoveable(dest)`; hold-person forces failure (1218-1219) [V].
5. Commit: quest-complete check, `ClearOwner(old)`, `SetOwner(new)`, spike damage (1219-1250) [V].
6. Confirm packet: `OBJECTMOVE_CONFIRM`, new window origin (x-10, y-7), dir, SP cost byte, tile occupy status,
   HP, then `iComposeMoveMapData` strip (1311-1368) [V].
7. `ClientMotionHandler` broadcasts `EVENT_MOTION`/`OBJECTMOVE` to neighbours (1052-1055), then runs
   `ClientKilledHandler` if a spike killed the player (1056) and `bCheckClientMoveFrequency` (1058) [V].
8. Client `MotionResponseHandler`: subtract SP, update HP (with HP-change event text and logout-count cancel),
   `ShiftMapData(dir)` + `_ReadMapData` strip, `m_cCommandCount--` (CLI 35556-35608) [V].
9. When the walk animation ends, `iObjectFrameCounter` returns 2 and commands re-open (35412-35414) [V].

### 6.2 Rejected step
Server: tile blocked or held -> `m_bIsMoveBlocked = TRUE` and a 40-byte `OBJECTMOVE_REJECT` with the
player's true x/y, type, dir, appearance, colour and status (1379-1483) [V]. Client: if the object id is its
own, restore position and appearance, stop, `m_cCommandCount = 0`, cancel spell targeting, recentre the view,
remember `m_bIsPrevMoveBlocked`, play a bump sound (CLI 35610-35697) [V]. The next frequency check is
skipped once because `m_bIsMoveBlocked` resets the clock (52681-52685) [V].

### 6.3 Others moving
`SendEventToNearClient_TypeA` sends to every init-complete client on the same map within x +-11 / y +-9 for
moves (sRange 1) and +-10/+-8 otherwise (2927-2931, 3050-3054); clients already within +-9/+-7 get the
short form, edge clients the full form with name and appearance (3067-3070) [V]. The client places the
object via `bSetOwner`; if it does not know the object or the destination cell is held by another object it
asks `MSGID_REQUEST_FULLOBJECTDATA` (CLI MapData.cpp:1221-1496) [V].

## 7. Rules and invariants

- Walk: 7 steps must take at least 72*8*7 - 3000 = 1032 ms by the server clock, else disconnect (1150-1162) [V].
- Run: 7 steps at least 43*8*7 - 1500 = 908 ms (1165-1177) [V]. Both compiled unless `NO_MSGSPEEDCHECK`.
- Any two moves/runs at least 250 ms apart by the client's own timestamp, else disconnect (52698-52703) [V].
- Run costs 1 SP per step when `m_iTimeLeft_FirmStaminar == 0`; at SP <= 0 running is still accepted and SP
  goes negative; below -10 the client is disconnected (1329-1351) [V]. The client stops sending run at SP < 1 (36938) [V].
- Max 6 unacknowledged motion commands; a warning at 5; if 6 stay unanswered for 7 checks of 2 s the client
  declares the connection lost (CLI 36852-36854, 4292-4301) [V].
- Hold Person (`DEF_MAGICTYPE_HOLDOBJECT`) blocks movement server-side (1218-1219) [V].
- View window 21 x 16 tiles anchored at (x-10, y-7) (2268-2276, 1361-1367) [V].

## 8. Couplings

Out: tile (`bGetMoveable`, `SetOwner`/`ClearOwner`, spike DO), map (sector counters `m_stTempSectorInfo`),
quest (`_bCheckIsQuestCompleted` per step, 1224), skills (`ClearSkillUsingStatus` cancels fishing etc.),
character (SP, HP, firm-stamina buff), combat (`ClientKilledHandler` after spike; attack-move calls
`iClientMotion_Attack_Handler` 1070-1081), magic (hold-person status), session (`DeleteClient` on hack checks).
In: combat (`DEF_NOTIFY_DAMAGEMOVE` -> client `m_sDamageMove` turns the next command into
`OBJECTDAMAGEMOVE`, CLI 36870-36883), map (teleport tile check right after a step, CLI 35952), entity (NPC
movement shares the tile API but not this handler), pk-law/crusade (`m_bIsAttackModeChange` resets the
frequency clock, 52688-52692).

## 9. Current behaviour vs intent

- A coordinate mismatch returns 2, which the MOVE/RUN cases do not act on; only STOP/ATTACK/GETITEM/MAGIC send a reject (1033-1035); stale commands are dropped because the client already resynced ([MOVEMENT-001](../issues/movement.md#movement-001-coordinate-mismatch-on-a-move-is-returned-but-never-acted-on)).
- `bCheckClientMoveFrequency` uses the client-supplied timestamp (`dwClientTime`, 1018-1020); the 7-step server-clock window is the check that does not depend on the client ([MOVEMENT-002](../issues/movement.md#movement-002-move-frequency-check-trusts-the-client-supplied-timestamp)).
- `OBJECTDAMAGEMOVE` goes through the walk path with no frequency check and no run SP (1061-1068); it increments the walk counter, so the 7-step window still applies ([MOVEMENT-003](../issues/movement.md#movement-003-objectdamagemove-bypasses-the-frequency-check-and-run-sp-cost)).
- The run cost byte is 1 whenever firm stamina is 0 and 0 while it is active, in both the SP > 0 and the SP <= 0 branches; the branches differ only in that SP <= 0 lets SP go negative and kicks the client below -10 (1329-1353) [V]: the evident intent of the margin of 10 is a tolerance for client/server drift [I].
- Attack-rate checks live in the combat path, not here: 7 attack messages must span at least 1480 ms (80*8*7-3000, S :11304-11317) and a client-clock gap under 500 ms between attacks is rejected (S :52627); see combat.md section 10 [V].
- Client `bGetIsLocateable`/`bIsTeleportLoc` inclusive window test reads one past the cell array ([MOVEMENT-004](../issues/movement.md#movement-004-client-locateable-test-reads-one-past-the-cell-array)).
- Hunter portal blocks are hard-coded coordinates in the client (`_bCheckMoveable`) with map indices 3/11; the server equivalent is a recall after arrival (map.md sect. 7) ([MOVEMENT-005](../issues/movement.md#movement-005-hunter-portal-blocks-are-hard-coded-coordinates-in-the-client)).

## 10. Open questions

- Exact walk/run animation frame timings on the client (72 / 43 ms per frame implied by the server
  comments 1155, 1170) are in the rendering docs, not re-derived here [U].
- Whether `iClientMotion_Stop_Handler` feign-death kick (mm comment) can be triggered by others standing on a
  corpse tile [U].

## 11. Doc discrepancies

- mm says the walk window is "7 moves in LESS than ... 1032 ms" - matches. No discrepancy found.

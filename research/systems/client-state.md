# System: client state mirror, game loop and timing

*Cohesive doc (the mirror only makes sense with the message flow and the loop that drains it); read
whole. Message layouts are not repeated: `../shared/messages/client-game.md`, `client-log.md`.*

Program: client (`sources/client-2.20/`), with hops to the game server and log servers named where
the flow crosses. Tags: [V] verified in source by this pass; [Vd] verified by the linked doc's own
source review; [I] inferred; [U] unverified.

## Contents
- 1 Purpose
- 2 Subsystems
- 3 Functions
- 4 Commands and messages
- 5 Data
- 6 Flow
- 7 Rules and invariants
- 8 Couplings
- 9 Intent vs implementation
- 10 Open questions
- Doc discrepancies

## 1 Purpose

The server is authoritative; the client keeps a **partial, local copy** of what the player can see
and own, so that it can draw at frame rate and answer clicks without a round trip. The copy is:
a 40x35-cell window of the map around the player with every visible object, the player's own
stats and appearance, the inventory and bank, and assorted dialog-backed records (quest, party,
guild ops, crusade map). The design evidently favours **bandwidth and responsiveness** over strict
consistency [I]: own movement is predicted before the server agrees, most object events are sent as a
2-byte id plus direction and resolved from a local cache, and the client asks for a full record
only when the cache misses [V mechanism, see Flow]. All of it lives in one global `CGame` object
(`G_pGame`) and its `CMapData` [Vd `../client/data-structures.md`].

## 2 Subsystems

| Subsystem | Intent | Nests in |
|---|---|---|
| S1 Connection and session | two async sockets (log, game), framing/cipher, enter-game handshake, keep-alive, lag kick, server change | root |
| S2 Receive dispatch | one switch on MSGID, `MSGID_NOTIFY` fanned out to ~133 notify cases | S1 |
| S3 Map window mirror | `CMapData`: 40x35 live cells around a pivot, shifted one row/column per confirmed step, full static map for walkability | S2 |
| S4 Object mirror | place/move/remove other players, NPCs, corpses, ground items, dynamic objects, effects; object-id location cache and full-object refresh | S3 |
| S5 Own player | id, position (predicted), appearance, stats, HP/MP/SP, combat mode, command counter | S2, S4 |
| S6 Inventory and bank mirror | 50 item slots plus bag order and positions, 121 bank slots, equip flags; updated by item notifies; client tells server bag positions | S2 |
| S7 Game loop and timing | busy message loop, 1 s timer, wall-clock time stamps, per-object animation clocks | root |

## 3 Functions

Full index: `../client/functions/networking.md` (every handler and `NotifyMsg_*`),
`../client/functions/world-map.md`, `../client/functions/startup.md`.

| Sub | Function | Where | Role |
|---|---|---|---|
| S1 | `XSocket::iSendMsg` / `pGetRcvDataPointer` | XSocket.cpp:465-501 / 589-611 | 3-byte header, key-based byte cipher [Vd] |
| S1 | `CGame::OnGameSocketEvent` / `OnLogSocketEvent` | Game.cpp:1277-1327 / 14726-14771 | WSAAsyncSelect window messages -> read -> dispatch [Vd] |
| S1 | `CGame::ConnectionEstablishHandler` | Game.cpp:3030-3081 | on game connect send `MSGID_REQUEST_INITPLAYER`; on log connect send the pending log request [V 3036-3060] |
| S1 | `CGame::InitPlayerResponseHandler` | Game.cpp:3085-3109 | confirm -> `REQUEST_INITDATA`, mode WaitInitData; reject -> LogResMsg [V] |
| S1 | `CGame::bSendCommand` | Game.cpp:1431-2275 | the only packet builder; counts motion commands (`m_cCommandCount++`, 2237) [V 2237] |
| S1 | `CGame::OnTimer` | Game.cpp:4266-4348 | keep-alive 8 s, lag check 2 s, cache sweep [V 4286-4301] |
| S1 | `CGame::NotifyMsg_ServerChange` | Game.cpp:45039-45090 | drop sockets, reconnect to another game server, re-enter [Vd] |
| S2 | `CGame::GameRecvMsgHandler` | Game.cpp:2936-3027 | MSGID switch [V] |
| S2 | `CGame::NotifyMsgHandler` | Game.cpp:29946-31462 | notify switch, mostly delegating to `NotifyMsg_*` [Vd] |
| S3 | `CGame::InitDataResponseHandler` | Game.cpp:45800-46075 | reset map, dialogs, effects, chat; read id, map name, location, weather; open `.amd`; place player; read initial cells [V key lines] |
| S3 | `CMapData::OpenMapDataFile` / `Init` | MapData.cpp:718 / - | static tile layer from `mapdata\*.amd` [V start] |
| S3 | `CMapData::ShiftMapData` + `CGame::_ReadMapData` | MapData.cpp:801 / Game.cpp:14417 | scroll the window one step and fill the new edge from the confirm packet [V] |
| S3 | `CGame::ResponsePanningHandler` | Game.cpp:24670-24705 | observer panning: same shift-and-read [Vd] |
| S4 | `CGame::MotionEventHandler` | Game.cpp:46077-46361 | `MSGID_EVENT_MOTION`: full record (id < 30000) or short form (id + 30000) -> `bSetOwner`; damage/death text [V] |
| S4 | `CGame::LogEventHandler` | Game.cpp:14610-14710 | `MSGID_EVENT_LOG`: confirm = appear (STOP), reject = remove (x = -1) [V 14688-14704] |
| S4 | `CMapData::bSetOwner` | MapData.cpp:892-~1640 | move/place an owner, id cache, out-of-window removal, cache-miss refresh [V 892-960, 1205-1225, 1485-1500] |
| S4 | `CGame::RequestFullObjectData` | Game.cpp:8650-8688 | `MSGID_REQUEST_FULLOBJECTDATA` built inline [V] |
| S4 | `CMapData::bSetDeadOwner` / `bSetItem` / `bSetDynamicObject` | MapData.cpp:3422 / 3385 / 3587 | corpses, ground items, fire/fish/minerals [V starts] |
| S4 | `CGame::CommonEventHandler` / `DynamicObjectHandler` | Game.cpp:4833-4889 / 22854 | item drops, magic effects, guild-name clear / dynamic objects [Vd] |
| S4 | `CMapData::iObjectFrameCounter` | MapData.cpp:1711-3381 | advance every animation clock; returns the redraw code [V start] |
| S5 | `CGame::InitPlayerCharacteristics` | Game.cpp:5452-5592 | `MSGID_PLAYERCHARACTERCONTENTS`: stats, level, exp, guild, etc. [Vd] |
| S5 | `CGame::MotionResponseHandler` | Game.cpp:35498-35697 | confirms/rejects of own commands [V 35500-35610] |
| S5 | `NotifyMsg_HP/MP/SP/Exp/LevelUp/Killed/Hunger/...` | Game.cpp:43490-45274 | stat mirror updates with event text [Vd] |
| S5 | `CGame::InitGameSettings` | Game.cpp:4901-5148 | reset per-session state before entering [Vd] |
| S6 | `CGame::InitItemList` | Game.cpp:17689-17869 | `MSGID_PLAYERITEMLISTCONTENTS`: rebuild inventory, bank, order [V 17700-17717] |
| S6 | `NotifyMsg_ItemObtained/ItemPurchased/...EraseItem/ItemToBank/SetItemCount/...` | Game.cpp:43161-45118 | item mirror updates; obtained/purchased send `MSGID_REQUEST_SETITEMPOS` [Vd] |
| S6 | `CGame::EraseItem` / `ItemEquipHandler` | Game.cpp:31988 / 48407 | remove a slot / equip request with local checks [V starts] |
| S7 | `EventLoop`, `_StartTimer`, `WndProc` | Wmain.cpp:373-386, 407-414, 72-234 | loop, 1000 ms timer, message routing [Vd] |

## 4 Commands and messages

Owned (session/envelope/entity rows of `commands/by-system.md`, client side):
- Out: `MSGID_REQUEST_INITPLAYER`, `REQUEST_INITDATA`, `COMMAND_CHECKCONNECTION` (8 s),
  `REQUEST_FULLOBJECTDATA`, `REQUEST_SETITEMPOS`, `REQUEST_ENTERGAME` (to the log server, also on
  server change), `REQUEST_NOTICEMENT` [V/Vd].
- In: `MSGID_RESPONSE_INITPLAYER`, `RESPONSE_INITDATA`, `PLAYERCHARACTERCONTENTS`,
  `PLAYERITEMLISTCONTENTS`, `RESPONSE_MOTION` (`DEF_OBJECTMOVE_CONFIRM/REJECT`,
  `DEF_OBJECTMOTION_CONFIRM/ATTACK_CONFIRM/REJECT`), `EVENT_MOTION`, `EVENT_LOG`, `EVENT_COMMON`,
  `DYNAMICOBJECT`, `RESPONSE_PANNING`, `NOTIFY` (envelope) [V dispatch Game.cpp:2943-3025].
- Notifies that write the mirror: `DEF_NOTIFY_HP/MP/SP/EXP/LEVELUP/KILLED/HUNGER/CHARISMA`,
  `ITEMOBTAINED/ITEMPURCHASED/DROPITEMFIN_*/GIVEITEMFIN_*/ITEMDEPLETED_ERASEITEM/ITEMTOBANK/
  SETITEMCOUNT/ITEMCOLORCHANGE`, `TIMECHANGE`, `WHETHERCHANGE`, `SERVERCHANGE`, `FORCEDISCONN`
  [Vd `networking.md`]; full list `commands/notify-types.md`.

## 5 Data

- `CMapData` (MapData.h:29): `m_pData[40][35]` live cells, `m_sPivotX/Y` window origin,
  `m_tile[752][752]` static map, `m_stFrame[80][15]` animation table,
  `m_iObjectIDcacheLocX/Y[30000]` (positive = standing, negative = corpse) [Vd; sign V
  MapData.cpp:913-944].
- `CTile` live and dead owner fields, item, dynamic object, effect, chat index (Tile.h:24-71) [Vd].
- Own player: `m_sPlayerObjectID`, `m_sPlayerX/Y`, `m_cPlayerDir`, `m_sPlayerType`,
  `m_sPlayerAppr1-4`, `m_iPlayerApprColor`, `m_sPlayerStatus`, `m_iHP/m_iMP/m_iSP` (`CInt`),
  `m_bIsCombatMode`, `m_cCommand`, `m_cCommandCount`, `m_bCommandAvailable` (`CInt`) [V Game.h:702-707, Game.cpp:46242-46265].
- Items: `m_pItemList[50]`, `m_pBankList[121]` (`DEF_MAXBANKITEMS 121 // 120+1`, `CLI Game.h:81`; the server holds 120, `SRV Client.h:26`; see `persistence.md` section 5), `m_cItemOrder[]`, `m_bIsItemEquipped/Disabled[50]`
  (`CInt`), `m_sItemEquipmentStatus[15]` [Vd; V Game.h:705-706].
- Session: `m_pGSock`, `m_pLSock`, `m_iNetLagCount`, `m_cMapName`, `m_cCurLocation`, `m_cLocation`
  (home nation), `m_cMapIndex` [V].
- Time: `G_dwGlobalTime` (set each `UpdateScreen`), `m_dwCurTime`, `CMapData::m_dwFrameTime` [Vd].
- Files read: `mapdata\<map>.amd` per map change; `contents\contents1000.txt` notice cache [Vd].
  Nothing about the mirror is persisted client-side.

## 6 Flow

**Enter the world** (log -> game server):
1. Select-character screen sends `REQUEST_ENTERGAME` to the log server; the reply names the game
   server; client opens `m_pGSock` (Game.cpp:21806-22084, `LogResponseHandler` 14773) [Vd;
   world-log routing: `../servers/official-worldserver.md`].
2. Game socket connects -> `ConnectionEstablishHandler` sends `MSGID_REQUEST_INITPLAYER`
   (Game.cpp:3036-3038) [V].
3. Game server answers `RESPONSE_INITPLAYER`; confirm -> `REQUEST_INITDATA`, mode WaitInitData
   (Game.cpp:3092-3094) [V]. Character and item contents arrive as `PLAYERCHARACTERCONTENTS`
   (`InitPlayerCharacteristics`) and `PLAYERITEMLISTCONTENTS` (`InitItemList`) [V dispatch].
4. `RESPONSE_INITDATA` -> `InitDataResponseHandler`: `m_pMapData->Init()`, close location-bound
   dialogs 7,11,13,14,16,20-23, free effects and chat, read player id, map name, location, weather,
   open `mapdata\<map>.amd`, place player at `sX+14+5`, view at `(sX+9)*32`, `_ReadMapData`,
   then `ChangeGameMode(ONMAINGAME)` (Game.cpp:45812-46049) [V].

**Own step with prediction**:
1. `CommandProcessor` sends `MSGID_COMMAND_MOTION` MOVE/RUN, `m_cCommandCount++` (Game.cpp:2237),
   moves `m_sPlayerX/Y` one cell and calls `bSetOwner` for itself (36945-36960) [V].
2. Game server validates (movement system, other cluster) and replies.
3. `DEF_OBJECTMOVE_CONFIRM`: subtract stamina sent by server, overwrite `m_iHP` (with HP-change
   text and logout-countdown cancel), `ShiftMapData(dir)` + `_ReadMapData` for the new edge,
   `m_cCommandCount--` (Game.cpp:35556-35607) [V].
4. `DEF_OBJECTMOTION_REJECT`: snap to server x/y, STOP, re-place, `m_cCommandCount = 0`, cancel
   pointing mode, reset camera, force background redraw (35526-35556) [V]. `OBJECTMOVE_REJECT`
   at 35610 does the equivalent for moves [Vd].

**Another object moves** (server -> client):
1. `MSGID_EVENT_MOTION`. If id < 30000 the packet carries position, type, direction, name
   (players < 10000), appearance; otherwise id+30000 carries only direction and the damage /
   spell bytes (Game.cpp:46100-46205) [V].
2. `bSetOwner`: for a short-form event, find the object through `m_iObjectIDcacheLocX/Y`, step it
   by the direction; if the cache misses or the target cell is occupied by someone else, send
   `REQUEST_FULLOBJECTDATA` and drop the event (MapData.cpp:1215-1225, 1494-1498) [V].
   Objects stepping out of the 40x35 window are removed with their chat bubble (MapData.cpp:910-947) [V].
3. Own-player appearance updates arrive as `DEF_OBJECTNULLACTION` matched **by name**; combat mode
   is derived from `sAppr2 & 0xF000` (Game.cpp:46239-46271) [V].
4. The next frame's `iObjectFrameCounter` animates it; the painter draws it (`rendering.md`).

**Appear / disappear**: `MSGID_EVENT_LOG` confirm -> `bSetOwner(..STOP)` plus an appear effect;
reject -> `bSetOwner(id, -1, -1, ...)` removes (Game.cpp:14688-14704) [V].

**Inventory change**: server notify (e.g. `ITEMOBTAINED`) -> `NotifyMsg_ItemObtained` fills a free
slot, picks a bag position and sends `REQUEST_SETITEMPOS` so the server stores the layout
(Game.cpp:43867-44026) [Vd] -> inventory dialog redraws from `m_pItemList` (`ui.md`).

**Loop and clocks**: `EventLoop` drains window messages (input, socket, timer) and renders when
idle (Wmain.cpp:373-386); socket messages therefore mutate state between frames, never during
one [Vd; I for "never during"]. `OnTimer` runs once per second from a multimedia timer
(Wmain.cpp:407-414, 857) [Vd].

## 7 Rules and invariants

- Live window is 40x35 cells (MAPDATASIZEX/Y, MapData.h:26-27); largest map 752x752; object ids
  must be < 30000 (cache size) [Vd]. Id ranges: < 10000 player, 10000-29999 NPC, >= 30000 short
  form (id + 30000) [V Game.cpp:46100-46102, 46183].
- At most 6 unacknowledged motion commands; 7 consecutive lag checks at that level drop the
  connection (Game.cpp:4292-4301, 36854); the check's 2 s gate runs against the last-receive stamp, so under
  server silence it runs every 1 s tick (about 7 s) [V code, I effect].
- Keep-alive `MSGID_COMMAND_CHECKCONNECTION` every 8 s (Game.cpp:4272-4280) [Vd].
- Running needs SP >= 1 on the client side (Game.cpp:36934-36939) [V]; the server deducts the real
  stamina in the move confirm (35569) [V].
- Own player is not re-placed by its own motion events while running (anti speed-hack comment
  "v2.17 Tab key", Game.cpp:46264-46265) [V].
- Animation clocks per object from `m_stFrame`, frame skip up to 3 (MapData.cpp:1927-1932) [Vd].
- No bound checks on most server-supplied indices (item slot ids, dialog ids) [Vd data-structures.md].

## 8 Couplings

| Direction | Coupling point | Other system |
|---|---|---|
| in | `MSGID_RESPONSE_MOTION`, `EVENT_MOTION` produced by server movement and combat | `movement.md`, `combat.md` (other cluster) [U paths] |
| in | `RESPONSE_INITDATA`, `.amd` map files, teleport / `SERVERCHANGE` | `map.md`, `session.md` (other cluster) [U paths] |
| in | `PLAYERCHARACTERCONTENTS`, stat notifies | `character.md` (other cluster) [U path] |
| in | `PLAYERITEMLISTCONTENTS`, item notifies; out `REQUEST_SETITEMPOS` | `item.md`, `shop.md`, `trade.md` (other cluster) [U paths] |
| in | `EVENT_LOG` appear/disappear, `DYNAMICOBJECT`, full-object refresh | `entity.md` (other cluster) [U path] |
| in | log-server enter-game reply, world-log routing | account / main and world log servers (`../servers/`) |
| out | `CMapData` cells, effect list, player appearance read by the painter and animator | `rendering.md`, `audio.md` |
| out | item/quest/party/guild records shown by dialogs; notifies call `EnableDialogBox` | `ui.md` |
| in | `CommandProcessor` is the producer of motion commands and prediction | `ui.md` |

## 9 Current behaviour vs intent

- Own-player events are matched by name, not id (Game.cpp:46239) ([CLIENT-STATE-001](../issues/client-state.md#client-state-001-own-player-events-matched-by-name-not-id)).
- On a rejected move the client discards all outstanding predicted steps and snaps ([CLIENT-STATE-002](../issues/client-state.md#client-state-002-prediction-without-a-reconciliation-queue)).
- A cache-miss refresh leaves the object stale until the full record arrives ([CLIENT-STATE-003](../issues/client-state.md#client-state-003-cache-miss-refresh-drops-the-event)).
- `CMapData::bGetIsLocateable` indexes beyond the range ([CLIENT-STATE-004](../issues/client-state.md#client-state-004-bgetislocateable-indexes-out-of-range)).
- `NotifyMsg_BanGuildMan` and `_CalcSocketClosed` have no caller ([CLIENT-STATE-005](../issues/client-state.md#client-state-005-functions-with-no-caller)).
- The lag kick counts unanswered motion commands, not keep-alive replies ([CLIENT-STATE-006](../issues/client-state.md#client-state-006-lag-kick-ignores-keep-alive-replies)).
- `OnTimer` stores `m_dwCheckChatTime = m_dwTime` while comparing against `dwTime` ([CLIENT-STATE-007](../issues/client-state.md#client-state-007-ontimer-chat-check-stamp-compares-the-wrong-clock)).
- Several fields use `CInt` (memory-scanner guard) only on HP/MP/SP and command flags; the guard is probably a librehelbreath addition, not 2.20 [Vd data-structures.md, I].

## 10 Open questions

- Exact layout of the edge data `_ReadMapData` consumes: see `../shared/messages/client-game.md`
  (not re-read here) [U].
- End line of `CMapData::bSetOwner` (next definition is `bGetOwner` at MapData.cpp:1646) [U].
- Whether the server ever sends a short-form event for the player's own id while not running [U].

## Doc discrepancies

- `../client/functions/sound.md` describes `PlaySound` as panning "by distance from the player";
  `PlaySound` takes distance and pan from its callers, which measure from the **view centre**
  (MapData.cpp:1730-1747) or the screen centre in pixels (Game.cpp:6001-6008). See `audio.md`.
- None found in the networking map for the handlers re-read here.

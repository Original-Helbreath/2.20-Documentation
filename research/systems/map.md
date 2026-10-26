# System: map (maps, .amd, map info, teleports, multi-server routing)

*Cohesive single doc (one continuous system; sections cross-reference), so a Contents block instead of a
sidecar split. Written 2026-10-25 by the world-cluster agent.*

Paths: `SRV` = `sources/hgserver-librehelbreath/src/` (2.20 HGserver, byte-equal to the srv220 export the
function maps cite, so map-doc line numbers are valid here), `CLI` = `sources/client-2.20/`. Line numbers
are as of 2026-10-25. Tags: [V] read in source this pass (or in the reviewed function map named beside it),
[I] inferred, [U] unverified.

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flows -
7. Rules and invariants - 8. Couplings - 9. Current behaviour vs intent - 10. Open questions -
11. Doc discrepancies

## 1. Purpose

The world is split into named maps (towns, farms, buildings, dungeons, middleland, arenas). Each map is a
fixed tile grid plus a text "map info" file that says where monsters spawn, where portals lead, where the
safe zones are and who may stand there. For the player the system is: walk onto a portal tile and arrive on
another map, cast Recall and go home, get thrown out of a map you are too low/high level for, see day,
night and rain. For the operator it is: decide which game server hosts which maps, and let players cross
between servers without noticing more than a loading screen [I from the design of 6.3].

## 2. Subsystems

| Subsystem | Intent |
|---|---|
| 2.1 Map registry | each game server loads only the maps named in its GServer.cfg; one CMap per map |
| 2.2 Tile file (.amd) loader | shared walkability/teleport/water/farm truth for client and server (detail in `tile.md`) |
| 2.3 Map info (mapdata/<map>.txt) | per-map design data: portals, spawners, safe areas, fish/mineral points, level limits, crusade points |
| 2.4 Teleport | one handler for portal tiles, recall, forced recall, jail, GM teleport, quest/guild/paid teleports |
| 2.5 Cross-server routing | map not hosted here -> save to log server -> client reconnects via world log server |
| 2.6 Map access rules | level-limit / upper-level-limit / hunter-zone forced recall, enemy-town recall block, locked map (jail) |
| 2.7 Paid teleport list | v2.20 NPC teleport menu (TeleportList cfg) with fee into the city treasury |
| 2.8 Environment | day/night per real-time minute, weather per map, fixed-snow maps |
| 2.9 Activity sectors | 60x60 array of 20-tile cells (MAXSECTORS 60, Map.h:47; S :1185) as a heat map of player/side/monster activity, used to place mob events |
| 2.10 Location side | map name -> town side / building, which drives side-dependent rules |

Nesting: 2.1 owns 2.2 and 2.3 at startup; 2.4 is the hub, it calls 2.5 when the target is foreign and
2.6 decides whether the target is overridden (locked map) or forced ('0' recall); 2.8 and 2.9 are timer
subsystems that read per-map flags loaded by 2.3.

## 3. Functions

Server (game server unless noted). Full descriptions live in `research/game-server/functions/map-movement.md`
(cited "mm").

| Subsystem | Function | Where | Role |
|---|---|---|---|
| 2.1 | `bReadProgramConfigFile` (`game-server-map` token) | SRV Game.cpp:5202, token 5337 [V] | lists the maps this server hosts |
| 2.1 | `_bRegisterMap` | SRV Game.cpp:5488 [V] | rejects duplicates, new CMap, `bInit`, remembers middleland/aresden/elvine indices |
| 2.2 | `CMap::bInit` -> `_bDecodeMapDataFileContents` | SRV Map.cpp:392, 410-512 [V] | reads `mapdata\<name>.amd` |
| 2.3 | `OnStartGameSignal` -> `_bReadMapInfoFiles` -> `__bReadMapInfo` | SRV Game.cpp:20224-20230, 9976, 25556-27113 [V] | map info parsed only after Npc.cfg arrives (spawns need NPC templates) |
| 2.3 | `CMap::_SetupNoAttackArea` | SRV Map.cpp:684-707 [V mm] | stamps safe-zone bit 0x4 into tile attributes |
| 2.4 | `RequestTeleportHandler` | SRV Game.cpp:23250-23857 [V] | the single teleport engine (modes in 6.2) |
| 2.4 | `CMap::bSearchTeleportDest` | SRV Map.cpp:515-530 [V mm] | portal source tile -> dest map/x/y/dir |
| 2.4 | `iGetMapIndex`, `GetMapInitialPoint`, `bGetEmptyPosition` | SRV Game.cpp:37953, 42342-42367, 11481-11504 [V] | resolve the landing tile |
| 2.4 | `iComposeInitMapData` | SRV Game.cpp:2252-2569 [V] | 21x16 snapshot sent on arrival |
| 2.5 | `bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA_REPLY)` | SRV Game.cpp:23369, 23437, 23470, 23512 [V] | save before handing over |
| 2.5 | `ResponseSavePlayerDataReplyHandler` | SRV Game.cpp:36351-36366 [V] | tells the client to change server |
| 2.5 | `SendNotifyMsg` case `DEF_NOTIFY_SERVERCHANGE` | SRV Game.cpp:18646-18667 [V] | packs map name + log-server address/port |
| 2.5 | `bSendMsgToLS(MSGID_REQUEST_REGISTERGAMESERVER)` | SRV Game.cpp:4345-4382 [V] | publishes this server's map list to the log server |
| 2.6 | `OnTimer` per-client checks | SRV Game.cpp:4000-4110 [V] | force-recall timer, level limits, hunter zones -> `RequestTeleportHandler(i,"0   ")` |
| 2.6 | `SetForceRecallTime` | SRV Game.cpp:52716 [V mm] | crusade-time force recall budget |
| 2.7 | `RequestTeleportListHandler`, `RequestChargedTeleportHandler`, `_bDecodeTeleportListConfigFileContents` | SRV Game.cpp:23861, 24005, 24655 [V mm] | NPC teleport menu |
| 2.8 | `CheckDayOrNightMode`, `WhetherProcessor` | SRV Game.cpp:36049, 37387 [V mm] | day/night, rain |
| 2.9 | `UpdateMapSectorInfo`, `AgingMapSectorInfo`, `CMap::ClearTempSectorInfo` | SRV Game.cpp:47553, 47613; Map.cpp:670 [V mm] | activity heat map |
| 2.10 | `iGetMapLocationSide`, `iSetSide` | SRV Game.cpp:50563-50612, 52857 [V mm] | side of a map / of the player there |

Client (`research/client/functions/world-map.md`, networking.md):

| Function | Where | Role |
|---|---|---|
| `CommandProcessor` portal check | CLI Game.cpp:35952-35954 [V] | standing on a teleport tile with no command in flight -> request teleport |
| `RequestTeleportAndWaitData` | CLI Game.cpp:45539-45798 [V] | client-side pre-checks (beginner dungeon level 30-80, enemy buildings), sends MSGID_REQUEST_TELEPORT |
| `InitDataResponseHandler` | CLI Game.cpp:45800-46075 [V map] | loads new .amd, resets map window, weather, dialogs |
| `CMapData::OpenMapDataFile` | CLI MapData.cpp:718-761 [V map] | client copy of the .amd |
| `NotifyMsg_ServerChange` | CLI Game.cpp:45039-45090 [V map] | drops sockets, reconnects to the log server with ENTERGAME |
| `SetWhetherStatus`, `NotifyMsg_WhetherChange` | CLI Game.cpp:23258 [V map] | rain/snow visuals and sound |
| `GetOfficialMapName` | CLI Game.cpp:33743 [V map] | internal map name -> display name/index |

World log server (no source): routes `ENTERGAME` by map name to the game server that registered the map
(`research/servers/topology.md` sect. "Hypothesis 2", `research/servers/world-log-server.md`) [V per those docs].

## 4. Commands and messages

From `research/systems/commands/by-system.md` (map row) and `research/shared/messages/`:
- Client -> game: `MSGID_REQUEST_TELEPORT` (portal), `MSGID_REQUEST_TELEPORT_LIST` / `MSGID_REQUEST_CHARGED_TELEPORT`,
  `MSGID_REQUEST_INITDATA` (first entry), `MSGID_REQUEST_PANNING` (observer, see movement.md).
- Game -> client: `MSGID_RESPONSE_INITDATA`, `MSGID_RESPONSE_TELEPORT_LIST`, `MSGID_RESPONSE_CHARGED_TELEPORT`,
  notifies `DEF_NOTIFY_SERVERCHANGE`, `TIMECHANGE`, `WHETHERCHANGE`, `LOCKEDMAP`, `TOBERECALLED`,
  `FORCERECALLTIME`, `SHOWMAP`, `MONSTEREVENT_POSITION` (entity), `MSGID_DYNAMICOBJECT` (tile).
- Game <-> log: `MSGID_REQUEST_REGISTERGAMESERVER` (map list), `MSGID_REQUEST_SAVEPLAYERDATA_REPLY` /
  `MSGID_RESPONSE_SAVEPLAYERDATA_REPLY`, `MSGID_TELEPORTLISTCONTENTS` (cfg push).
- Client <-> world log: `MSGID_REQUEST_ENTERGAME` with the destination map after a server change.
- Gate: none for routing; only `GSM_REQUEST_SUMMONPLAYER` (GM summon across servers) lands in teleport.
- Slash: `/teleport`, `/tp`, `/summonplayer`, `/setforcerecalltime`, `/setobservermode` (GM cluster).

## 5. Data

- `CMap` (SRV Map.h; limits in `research/game-server/data-structures.md` CMap): name, `m_cLocationName`
  (from `map-location`), size, `m_pTile`, `m_pTeleportLoc[200]`, waypoints, spot generators, initial points
  (20), no-attack rects, fish/mineral points, level limits, type (0 normal, 1 no-penalty/no-reward),
  `m_bIsFixedDayMode`, `m_cWhetherStatus`, `m_bIsFightZone` (name prefix `fight`), sector grids 60x60.
- CGame: `m_pMapList[100]`, `m_iMiddlelandMapIndex`, `m_iAresdenMapIndex`, `m_iElvineMapIndex`.
- CClient: `m_cMapName`, `m_cMapIndex`, `m_sX/m_sY/m_cDir`, `m_cLockedMapName` + `m_iLockedMapTime` (jail),
  `m_iTimeLeft_ForceRecall`, `m_bIsOnServerChange`, `m_bIsOnWaitingProcess`, `m_cLocation` (town).
  Persisted: map name and x/y in the character file (`research/shared/formats/persistence-files.md`) [V per doc].
- Files: `GServer.cfg` `game-server-map` lines; `mapdata\<map>.amd` (`research/shared/formats/amd-map.md`);
  `mapdata\<map>.txt` (`research/shared/formats/mapdata-txt.md`, every key listed there);
  TeleportList cfg pushed by the log server.

## 6. Flows

### 6.1 Server start
GServer.cfg map lines -> `_bRegisterMap` per map (SRV Game.cpp:5304/5337 -> 5488) -> `.amd` loaded
(Map.cpp:410) -> server registers with the log server sending all map names (Game.cpp:4345-4382) -> log
pushes configs (Npc.cfg etc.) -> `OnStartGameSignal` reads every map's `.txt` (20224-20230), which also
spawns fixed NPCs through `bCreateNewNpc` [V]. Intent: map info needs NPC templates, so it is deferred [V
comment 5519].

### 6.2 Portal tile, same server
1. Client walks onto a tile whose `.amd` flag 0x40 is set; once no motion is unacknowledged it calls
   `RequestTeleportAndWaitData` (CLI Game.cpp:35952-35954) which sends `MSGID_REQUEST_TELEPORT` [V].
2. `MsgProcess` passes the raw message to `RequestTeleportHandler(iClientH, pData)` (SRV Game.cpp:15026-15029) [V].
3. Guards: init complete, alive, not already waiting (23261-23264); enemy-town recall block (23268-23281);
   cancel exchange (23284-23288); `RemoveFromTarget`, `ClearOwner`, `EVENT_LOG REJECT` to neighbours
   (23299-23306) [V].
4. `bSearchTeleportDest` on the player's tile (23318); a locked (jailed) player has the destination replaced
   by the locked map unless it is a same-side building (23323-23341) [V].
5. Portal with no explicit map: if the destination map is hosted here, set x/y/dir/map and jump to
   `RTH_NEXTSTEP` (23343-23357) [V].
6. `RTH_NEXTSTEP`: clear poison, `iSetSide`, LOCKEDMAP notify, build `MSGID_RESPONSE_INITDATA` with
   `iComposeInitMapData`, place the owner on the tile (23529 onward) [V partial; tail per mm].
7. Client `InitDataResponseHandler` loads the new `.amd` and redraws (CLI Game.cpp:45800) [V map].

### 6.3 Portal / recall to a map hosted elsewhere (server change)
1. Steps 1-4 as above; the map is not in `m_pMapList`, so the handler writes the destination into the
   client record, sends `MSGID_REQUEST_SAVEPLAYERDATA_REPLY` to the log server (not counted as a logout),
   sets `m_bIsOnServerChange` and `m_bIsOnWaitingProcess` and returns (23359-23374; same pattern for
   recall 23430-23441 and named-map modes 23461-23474, 23502-23516) [V].
2. Log server replies `MSGID_RESPONSE_SAVEPLAYERDATA_REPLY` -> `ResponseSavePlayerDataReplyHandler` finds
   the character by name and sends `DEF_NOTIFY_SERVERCHANGE` (36351-36366) [V].
3. The notify carries the destination map name, `m_cLogServerAddr` (15 bytes) and the world-log port under
   `DEF_DEFENCEHACKING`, else the internal log-server port (18646-18667) [V].
4. Client drops both sockets, connects to that address and sends `MSGID_REQUEST_ENTERGAME` with the new
   map (CLI Game.cpp:45039-45090) [V map]; the world log server answers with the game server that
   registered the map (topology.md) [V per doc]; the new server loads the character and lands it; x/y of
   -1 means "initial point" (comment 23431-23432) [V].
5. On the old server the socket closes and `DeleteClient` skips the save because `m_bIsOnServerChange` is
   set (SRV Game.cpp:2640, 2806) [V].

### 6.4 Recall and forced recall
`pData[0]` modes (23377-23519) [V]: '0' forced recall has no `break` and falls into '1'; '1' recall:
neutral -> `default`, level > 80 -> own town, else own farm (`arefarm`/`elvfarm`), locked map overrides;
lands on a random initial point (neutrals always point 0, 42361-42363); '2' and '3' teleport to a named
map and x/y (callers: jail, resurrection, GM, quests, guild teleport, crusade). Callers of '0':
force-recall timer, level-limit and hunter-zone checks in `OnTimer` (4011, 4064, 4074, 4109) [V].

## 7. Rules and invariants

- Max 100 maps per server (`DEF_MAXMAPS`), duplicate names refused (Game.cpp:5494-5501) [V].
- Map names compare on 10 bytes (`memcmp(...,10)`, 23349) [V]; client message carries 10 (18647) [V].
- Recall: level > 80 goes to town, otherwise farm (23394-23404) [V].
- Enemy-town block: a non-admin of side X standing in a map whose location is the other town cannot use
  mode '1' or '3' (23268-23281) [V].
- Level limits: every client check tick, level < `level-limit` or > `upper-level-limit` (non-zero) forces
  recall unless admin (4061-4075) [V]. Hunters (civilian mode) are recalled from middleland, 2ndmiddle,
  aresdend1, elvined1, huntzone3/4 and `middle*` outside crusade (4085-4110) [V].
- Force recall during crusade: counts down only outside crusade or inside buildings (4002-4011) [V].
- Poison cleared on every successful same-server teleport (23533) [V]; across servers poison is simply not
  transferred (comment 23524) [V].
- Day = minute < 40, night otherwise (`DEF_NIGHTTIME`, mm CheckDayOrNightMode) [V mm]; weather 1/300 per
  map per 20 s, 3-10 min, rain only unless DEF_WINTER or a fixed-snow map (mm WhetherProcessor) [V mm].

## 8. Couplings

Outgoing (map reaches into):
- entity: `__bReadMapInfo` -> `bCreateNewNpc` for fixed NPCs and spot/random generators config; `RemoveFromTarget` on teleport.
- tile: `ClearOwner`/`SetOwner`, `bGetEmptyPosition` (bGetMoveable + bGetIsTeleport).
- session/infrastructure: `bSendMsgToLS` save-reply, `m_bIsOnServerChange` read in `DeleteClient`, log-server address fields.
- trade: `_ClearExchangeStatus` on teleport (23284-23288).
- quest: `_CheckQuestEnvironment`, `_SendQuestContents` in the teleport tail (mm callee list).
- crusade: `m_cLockedMapName` / `m_iLockedMapTime`, `DEF_NOTIFY_CRUSADE`, construction point notify on arrival; `RestoreStrikePoints`.
- economy/shop: charged teleport fee into `m_stCityStatus[side].iFunds` (24110, mm).
Incoming (others reach into map): pk-law (`ApplyPKpenalty` -> jail via '2' to arejail/elvjail, 28037/28051),
character (`RequestRestartHandler` -> resurr maps 46319; `ResurrectPlayer`), magic (`PlayerMagicHandler`
recall 21993), items (`UseItemHandler` recall scrolls / tickets 30821-30877), gm (`AdminOrder_Teleport` 37194,
`AdminOrder_SummonPlayer` 44374), guild/crusade (`RequestGuildTeleportHandler` 48687, `ServerStockMsgHandler`
49620), quest (`QuestAcceptedHandler` 41989), session (`InitPlayerData` 5030-5036).

## 9. Current behaviour vs intent

- `MSGID_REQUEST_TELEPORT` is handed whole to `RequestTeleportHandler`, so the mode byte is the low byte of the message id; the portal branch is chosen by tile lookup ([MAP-001](../issues/map.md#map-001-teleport-request-mode-byte-is-the-low-byte-of-the-message-id)).
- Forced recall (case '0') has no body and falls into plain recall ([MAP-002](../issues/map.md#map-002-forced-recall-case-0-has-no-body-and-no-break)).
- The enemy-town recall block compares `m_cLocationName`; the "no recall from resurrection zone" check is commented out ([MAP-003](../issues/map.md#map-003-enemy-town-recall-block-ignores-nopenalty-resurrection-zone-check-is-dead)).
- The shop-discount price for the crusade winner is computed in the teleport tail and not used ([MAP-004](../issues/map.md#map-004-crusade-winner-shop-discount-price-computed-and-never-used)).
- The client's enemy-building pre-check is compiled only under `DEF_LANGUAGE == 3`; the server enforces the same rule by a 1-tick recall ([MAP-005](../issues/map.md#map-005-enemy-building-pre-check-is-client-side-and-compiled-only-for-def_language-3)).
- The beginner-dungeon level 30-80 gate exists only in the client; the server relies on the map's level limits checked on a timer ([MAP-006](../issues/map.md#map-006-beginner-dungeon-level-30-80-gate-exists-only-in-the-client)).
- `-1` coordinates reach `GetMapInitialPoint` because `bGetMoveable` refuses the 20-tile edge band and `bGetEmptyPosition` then falls back to the initial point (Map.cpp:242; the source carries an explicit fallback comment, S :11497) [V; I that it is intended].
- A map with no initial points leaves the landing coordinates uninitialised ([MAP-007](../issues/map.md#map-007-map-without-initial-points-leaves-uninitialised-coordinates)).
- The older `bReadProgramConfigFile` is inside a comment block; the live reader is at 5202, called from 791 ([MAP-008](../issues/map.md#map-008-old-breadprogramconfigfile-is-a-dead-commented-block)).
- `CMap::iAnalyze` is an empty stub with no callers; the sector heat map does the work ([MAP-009](../issues/map.md#map-009-cmapianalyze-is-an-empty-stub)).

## 10. Open questions

- What the world log server does when two game servers register the same map name (no source) [U].
- Whether any shipped map is non-square (tile.md sect. 9 covers the indexing consequence); sample sizes
  checked only for arebrk11 (150x150) [U].
- Whether the server-change destination x/y is validated by the receiving server beyond
  `bGetEmptyPosition` [U; receiving path is in session/infrastructure].

## 11. Doc discrepancies

- `research/game-server/functions/map-movement.md` (RequestTeleportHandler) says mode '0' is "tile teleport
  via bSearchTeleportDest". In code the portal path is taken whenever the player stands on a portal tile
  and no map name is passed, regardless of `pData[0]`; '0' is forced recall falling through to '1'
  (SRV Game.cpp:23318-23383) [V].

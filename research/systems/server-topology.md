# System: server-topology (game server <-> gate <-> Log servers: lifecycle, registration, config, shutdown)

*Size verdict (2026-10-25): one cohesive doc. Start-up, registration, liveness and shutdown are one
lifecycle that crosses all five programs; split parts would constantly cross-reference. Use the Contents block.*

Cluster: infrastructure. Citation tags `SRV`, `CLI`, `GATE`, `MLS`, `WLS` and [V]/[I]/[U] as in
`network.md` (header). Line numbers as of 2026-10-25. Port and process facts are settled in
`research/servers/topology.md` and not re-derived; gate internals in `research/gate/subsystems.md`.

## Contents
1. Purpose
2. Subsystems
3. Functions
4. Commands and messages
5. Data
6. Flow
7. Rules and invariants
8. Couplings
9. Intent vs implementation
10. Open questions
11. Doc discrepancies

## 1. Purpose

Let one Helbreath world run as several cooperating processes - many game servers splitting the maps,
one gate as the inter-server bus, one World Log server holding the world's data and routing, one Main
Log server holding accounts - and let operators bring that world up, watch it, and take it down
without losing player data. Players see the result as one continuous world, a global user count,
and an in-game countdown before maintenance. Operators see five Win32 windows with log lists and
hotkeys, plus optional monitor tools on the gate.

Design intent [I]: **the game server is a stateless, auto-restarting worker.** It reads only its own small
cfg and map files, receives all game content from the WLS at registration, refuses players until every
dependency is up, and shuts itself down (and re-initialises) when it loses the Log server or the gate
(`research/servers/topology.md` "Startup and readiness") [V doc, I intent].

Who does what (2.20):

| Program | Role in the topology | Talks to |
|---|---|---|
| Client | none beyond connecting where told | MLS, WLS (after server change), one game server |
| Game server (SRV) | hosts maps from `GServer.cfg`; worker | WLS (1 main + 10 sub sockets), gate (1 socket), clients |
| Gate (GATE) | bus: stock relay, user total, party registry, shutdown orders, monitor feed | game servers and monitors only |
| WLS | registers game servers, pushes configs, stores characters/guilds, routes by map, account state | game servers, clients (after server change), MLS (10 sockets) |
| MLS | accounts, login, character list relay, first-entry routing | clients, WLS |

## 2. Subsystems

```
server-topology
 +- 2.1 Process start-up        each program reads its cfg and opens listeners / outbound sockets
 +- 2.2 Registration            game -> WLS (map list) ; game -> gate (name, build date) ; WLS -> MLS (world, game servers)
 +- 2.3 Content distribution    WLS pushes Item/Npc/Magic/Skill/Potion/Quest/BuildItem/... into each game server
 +- 2.4 Readiness gate          game server opens for players only when all dependencies are confirmed
 +- 2.5 World activation        WLS HOME key -> WORLDSERVERACTIVATED -> MLS allows logins for that world
 +- 2.6 Liveness                gate heartbeat (3 s), 30 s silence flag, user totals, monitor feed
 +- 2.7 Link failure recovery   reconnect sub-log / gate sockets; loss of main log or gate -> auto shutdown + re-init
 +- 2.8 Shutdown orchestration  local / gate-global / admin / auto paths; notices; drain; final countdown
 +- 2.9 Operator consoles       window log lists, status line, hotkeys (game server, gate, WLS, MLS)
```

Intent per subsystem [I]:
- **2.1** every address and port is configuration, so the same binaries run in any data centre
  (`research/servers/topology.md` "Connections and ports") [V doc].
- **2.2** the WLS learns which game server hosts which map (routing); the gate learns which peers are
  game servers (relay targets) and refuses mismatched builds [V doc].
- **2.3** content files live in one place per world; game servers never read them from disk
  (`research/servers/game-log-protocol.md` 5) [V doc]. All the game server's config pushes (items, NPCs, magic,
  skills, quests, potions, crafting, teleports, NPC drops, world) come from the log server (WLS) at registration;
  see `research/shared/formats/config-files.md` and the receiving cases at `SRV Game.cpp:15201-15267` [V doc].
- **2.4** a half-configured server must not accept players [V].
- **2.5** the world opens to logins only when the operator says so [V doc].
- **2.6** operators and the gate see dead or degraded servers; players see the world population.
- **2.7** transient network faults heal; persistent faults turn into a clean drain-and-restart.
- **2.8** every exit path drains players through the save path before the process ends.
- **2.9** a single operator can run a world from keyboards and windows; manager/BWM/gate-monitor protocols exist
  (`admin.md`) but their tools are absent from the source tree.

## 3. Functions

| Sub | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | `Initialize` / `WinMain` | SRV | `Wmain.cpp:238-263`, `143-153` | create `CGame`, `bInit`, listen on `game-server-port`, start the 300 ms timer (`254`) [V doc] |
| 2.1 | `CGame::bInit` | SRV | `Game.cpp:514-870` | reset all state (`608-621`, `715-724`), read `GServer.cfg` (`791`), connect main log (`799-801`) and gate (`808-810`), read `notice.txt` [V] |
| 2.1 | `bReadProgramConfigFile` | SRV | `Game.cpp:5202-5352` | `GServer.cfg` keys; maps via `_bRegisterMap` [V doc] |
| 2.1 | `_bRegisterMap` | SRV | `Game.cpp:5488-5540` | load `mapdata\<name>.amd` per hosted map [V doc] |
| 2.1 | `Initialize`, `CGateCore::bInit`, `bReadProgramConfigFile` | GATE | `Wmain.cpp:211-241`, `GateCore.cpp:63-71, 73-179` | cfg, logging thread, listener, 3 s timer [V doc] |
| 2.1 | `InitializeWorldServer` | WLS | `0x4014e0` | cfg, 10 sockets to MLS, internal listener [V doc] |
| 2.2 | `OnMainLogSocketEvent` | SRV | `Game.cpp:4144-4191` | on connect send `REQUEST_REGISTERGAMESERVER` (`4153-4157`) [V] |
| 2.2 | `bSendMsgToLS` case REGISTER | SRV | `Game.cpp:4345-4382` | name, address (from `gethostname`), port, map list [V doc] |
| 2.2 | `OnGateSocketEvent`, `SendMsgToGateServer` case REGISTER | SRV | `Game.cpp:20547-20611`, `20668-20712` | name, address, port, maps, PID, `DEF_BUILDDATE` [V] |
| 2.2 | `RegisterGameServerHandler` | GATE | `GateCore.cpp:451-537` | build-date and duplicate-name checks [V doc] |
| 2.2 | `HandleRegisterGameServer` | WLS | `0x402510` | register maps, push configs, CONFIRM, forward to MLS [V doc] |
| 2.3 | `MsgProcess` config cases | SRV | `Game.cpp:15198-15268` | decode each pushed config; set the matching `m_bIs*Available` [V doc] |
| 2.3 | `_bDecodeWLServerConfigFileContents`, `_bDecodeWorldConfigFileContents` | SRV | `Game.cpp:25019-25066`, `25070-25141` | world port; max user / max level / force-recall [V doc] |
| 2.4 | `CGame::OnTimer` readiness block | SRV | `Game.cpp:3693-3708` | all flags + 10 sub sockets -> `WM_USER_STARTGAMESIGNAL` [V] |
| 2.4 | `OnStartGameSignal` | SRV | `Game.cpp:20223-20247` | read map info, spawn NPCs, crusade cfgs, "activated" [V doc] |
| 2.4 | `bAccept` | SRV | `Game.cpp:399-464` | refuse players until ready / during shutdown [V] |
| 2.5 | `HandleKeyUpActivate` | WLS | `0x40c550` | HOME: open client port, send `WORLDSERVERACTIVATED` [V doc] |
| 2.6 | `SendMsgToGateServer` case ALIVE | SRV | `Game.cpp:20714-20726` | every 3 s, user count; suppressed while the Log link is down [V] |
| 2.6 | `CheckGameServerActivity`, `SendTotalClientsToGameServer` | GATE | `GateCore.cpp:688-708`, `788-850` | 30 s silence flag; world total to every server [V doc] |
| 2.6 | `MsgProcess` case TOTALGAMESERVERCLIENTS | SRV | `Game.cpp:14939-14944` | store world total for `/who` and the status line [V] |
| 2.7 | `OnSubLogSocketEvent` | SRV | `Game.cpp:42393-42455` | count active, reconnect a lost sub socket [V] |
| 2.7 | `_bCheckSubLogSocketIndex` | SRV | `Game.cpp:43123-43154` | no usable sub socket -> shutdown code 3 [V] |
| 2.7 | `_CheckGateSockConnection` | SRV | `Game.cpp:43099-43121` | gate retry counter > 20 ticks -> shutdown code 4 [V] |
| 2.8 | `MsgProcess` (F1+F4, gate SHUTDOWNED, SENDSERVERSHUTDOWNMSG) | SRV | `Game.cpp:14852-14862`, `14896-14930` | local and global shutdown, notices [V] |
| 2.8 | `CGame::OnTimer` drain / reboot / final countdown | SRV | `Game.cpp:3715-3724`, `3791-3808` | 15 players per 2 s, then re-init (codes 3/4) or 20 s countdown and `WM_DESTROY` [V] |
| 2.8 | `CheckGlobalCommand`, `CheckAutoShutdownProcess`, `SendServerShutDownMsg` | GATE | `GateCore.cpp:864-909`, `1033-1106`, `911-925` | gate-driven shutdown orders [V doc] |
| 2.8 | `HandleGameServerShutdowned`, `SendServerShutdownToMLS` | WLS | `0x4033c0`, `0x40b410` | flag server state 3; forward notice to MLS [V doc] |
| 2.9 | `DisplayInfo`, `CGame::OnKeyUp` | SRV | `Game.cpp:888-973`, `32884-32973` | status line; F1+F6/F7 notices, crusade keys [V doc] |
| 2.9 | `CGateCore::OnKeyUp`, `OnPaint` | GATE | `GateCore.cpp:579-668`, `Wmain.cpp:279-315` | F2/F3 lists, F1+F4/F6/F7, monitor strip [V doc] |

## 4. Commands and messages

From `research/systems/commands/by-system.md` (session, config, monitor rows) and `research/shared/messages/`:
- Game <-> WLS registration: `MSGID_REQUEST_REGISTERGAMESERVER` (0x0512A3F4; the official WLS expects
  0x0512A3E3), `MSGID_RESPONSE_REGISTERGAMESERVER`.
- Config push (WLS -> game): `MSGID_ITEM/NPC/MAGIC/SKILL/PORTION/QUEST/BUILDITEM CONFIGURATIONCONTENTS`
  (required), `MSGID_DUPITEMIDFILECONTENTS`, `MSGID_NOTICEMENTFILECONTENTS`, `MSGID_WORLDCONFIGCONTENTS`,
  `MSGID_NPCITEMCONFIGCONTENTS`, `MSGID_WLSERVERFILECONTENTS` (required under `DEF_DEFENCEHACKING`),
  `MSGID_TELEPORTLISTCONTENTS`; occupy-flag pushes 0x17081034/35.
- Game <-> gate: `MSGID_REQUEST_REGISTERGAMESERVER` (+PID, build date), `MSGID_RESPONSE_REGISTERGAMESERVER`,
  `MSGID_GAMESERVERALIVE`, `MSGID_TOTALGAMESERVERCLIENTS`, `MSGID_SERVERSTOCKMSG`, `MSGID_PARTYOPERATION`,
  `MSGID_SENDSERVERSHUTDOWNMSG` (code 1/2), `MSGID_GAMESERVERSHUTDOWNED`.
- Game -> WLS shutdown: `MSGID_GAMESERVERSHUTDOWNED`, `MSGID_SENDSERVERSHUTDOWNMSG`.
- Gate <-> monitors: `MSGID_GAMESERVERDOWN`, `MSGID_MONITORALIVE`, `MSGID_REQUEST_GAMESERVER_SHUTDOWN`,
  `MSGID_REQUEST_ALLGAMESERVER_SHUTDOWN`, `MSGID_REQUEST_ALL_SERVER_REBOOT`, `MSGID_REQUEST_EXEC_1DOTBAT` /
  `_2DOTBAT`, `MSGID_GATEWAY_CURRENTCONN` (`research/gate/subsystems.md` 4, 6).
- WLS <-> MLS: `REGISTER_WORLDSERVER`, `REGISTER_WORLDSERVERSOCKET`, `RESPONSE_REGISTER_WORLDSERVERSOCKET`,
  `REGISTER_WORLDSERVER_GAMESERVER`, `WORLDSERVERACTIVATED`, `REQUEST_REMOVEGAMESERVER`, `SENDSERVERSHUTDOWNMSG`
  (`research/servers/official-mainserver.md` 5).
- Notifies to players: `DEF_NOTIFY_SERVERSHUTDOWN` (mode 1/2 -> client dialog 25, `CLI Game.cpp:31084-31090`).

## 5. Data

| Item | Owner | Content | Cite |
|---|---|---|---|
| `GServer.cfg` | SRV | `game-server-name`, `game-server-port`, `log-server-address`, `internal-log-server-port`, `game-server-map` (repeat), `gate-server-address`, `gate-server-port`; `game-server-address` ignored | `research/servers/topology.md`; `SRV Game.cpp:5202-5352` [V doc] |
| `GateServer.cfg` | GATE | `gate-server-address`, `gate-server-port`, `build-date`, `game-server-list` (IP allow-list, max 10) | `research/gate/subsystems.md` 9 [V doc] |
| `WLserver.cfg` | WLS | world name/address/ports, MLS address/port, `game-server-list`, `backup-drive-letter` | `research/servers/official-worldserver.md` 1 [V doc] |
| `HMLserver.cfg` | MLS | log address, public port (+1, +2), internal port | `official-mainserver.md` 1 [V doc] |
| Readiness flags | SRV | `m_bIsLogSockAvailable`, `m_bIsGateSockAvailable`, `m_bIsItem/Npc/Magic/Skill/Portion/Quest/BuildItemAvailable`, `m_bIsWLServerAvailable`, `m_iSubLogSockActiveCount`, `m_bIsGameStarted` | `SRV Game.cpp:608-621, 3693-3708` [V] |
| Shutdown state | SRV | `m_bOnExitProcess`, `m_cShutDownCode` (1 gate-global, 2 local/admin, 3 Log lost, 4 gate lost), `m_bIsServerShutdowned`, `m_iFinalShutdownCount`, `m_iAutoRebootingCount` | `SRV Game.cpp:14854, 14923, 4183, 43114, 3791-3808` [V] |
| Gate peer state | GATE | `m_bIsGameServer`, `m_sIsActivated` (1 / 0 silent / -1 build / -2 duplicate), `m_iTotalClients`, `m_bIsShutDown` | `research/gate/subsystems.md` 3 [V doc] |
| Build identity | SRV | `DEF_BUILDDATE` 429 in the `DEF_CHINESE` build (`Version.h:22`); version "2"."19" | `research/game-server/build-config.md` [V] |

## 6. Flow

**F1. World start-up (operator order is implied, not enforced)**:
1. MLS starts, listens on its internal port; HOME opens the three public client ports
   (`official-mainserver.md` 1) [V doc].
2. WLS starts: reads `WLserver.cfg`, opens 10 sockets to the MLS (first registers the world, others the
   socket), listens on its internal port (`WLS 0x4014e0`; `official-mainserver.md` 5) [V doc].
3. Gate starts: reads `GateServer.cfg`, listens (`GATE Wmain.cpp:211-241`) [V doc].
4. Each game server: `Initialize` -> `bInit` -> read `GServer.cfg`, load hosted maps, connect main log and gate
   (`SRV Game.cpp:791-810`) [V].
5. Main log connects -> `REQUEST_REGISTERGAMESERVER` (`SRV Game.cpp:4153-4157`) -> WLS registers maps, pushes 11
   configs and the occupy flags, then CONFIRM, then forwards the game server to the MLS for first-entry
   routing (`WLS 0x402510`) [V doc]. SRV `MsgProcess` decodes each config; CONFIRM sets
   `m_bIsLogSockAvailable` (`SRV Game.cpp:15126-15191, 15198-15268`) [V doc].
6. Gate connects -> REGISTER with PID and build date (`SRV Game.cpp:20556-20561, 20668-20712`) -> gate checks the
   build date and name, replies CONFIRM/REJECT (`GATE GateCore.cpp:451-537`) -> SRV sets
   `m_bIsGateSockAvailable` (`Game.cpp:14947-14955`) [V].
7. One sub-log socket per second until 10 are connected (`SRV Game.cpp:3729-3738, 42403-42410`) [V].
8. 3 s tick: when every flag holds, `WM_USER_STARTGAMESIGNAL` -> `OnStartGameSignal` spawns NPCs, reads crusade
   files, logs "Game server activated" (`SRV Game.cpp:3693-3708, 20223-20247`) [V].
9. Operator presses HOME on the WLS -> client port opens, `WORLDSERVERACTIVATED` to the MLS; logins for the
   world now succeed (`WLS 0x40c550`; `MLS 0x40e630`) [V doc].

**F2. Steady state, every 3 s**: game server -> gate `MSGID_GAMESERVERALIVE` (user count) unless the Log link
is down (`SRV Game.cpp:3684, 20714-20726`); gate refreshes alive time, forwards to monitors, sums and returns
`MSGID_TOTALGAMESERVERCLIENTS` to every game server (`GATE GateCore.cpp:435-446, 788-850`) [V; gate V doc].

**F3. Global maintenance shutdown (gate F1+F4 or monitor request)**: gate flags every server, then every
~21 s: notice 1 (x7) -> notice 2 -> `MSGID_GAMESERVERSHUTDOWNED` (~189 s total) (`research/gate/subsystems.md` 6)
[V doc]. Each notice reaches SRV `MsgProcess`, which shows `DEF_NOTIFY_SERVERSHUTDOWN` to every player and
forwards `SENDSERVERSHUTDOWNMSG` to the WLS so it stops admitting users (`SRV Game.cpp:14896-14918`, comment
`14917`) [V]. SHUTDOWNED sets code 1, `m_bOnExitProcess`, tells the WLS (`14921-14930`) [V]. Then F5.

**F4. Link loss**: main log socket error -> code 3 immediately (`SRV Game.cpp:4174-4189`); all sub sockets
unusable -> code 3 (`43140-43149`); gate socket error -> reconnect at once with a retry counter, and if not
re-registered within 20 ticks (~60 s) -> code 4 (`20581-20597`, `43099-43121`) [V]. The counter is reset to 0
on TCP connect (`OnGateSocketEvent` CONNECTIONESTABLISH, `Game.cpp:20556-20562`), not on registration CONFIRM, so
a gate that accepts the socket and then rejects registration never reaches code 4; it starts at 1 in `bInit`
(`Game.cpp:812`), so a gate that is down at first start gives code 4 after about 60 s and a `bInit()` restart loop. Then F5.

**F5. Drain and exit / re-init** (`SRV Game.cpp:3791-3808`, `3715-3724`) [V]: `bAccept` already refuses new
players (`m_bOnExitProcess`); every 2 s `_iForcePlayerDisconect(15)` deletes up to 15 clients with save; when
none remain: codes 3/4 -> `bInit()` (full re-initialisation, re-register, re-receive configs) and
`m_iAutoRebootingCount++`; other codes -> final countdown from 20, decremented per second, `WM_DESTROY` at <= 1 (about 19 s).

## 7. Rules and invariants

- A game server accepts players only after: Log CONFIRM, gate CONFIRM, the seven required configs, all 10
  sub sockets, and (`DEF_DEFENCEHACKING`) the WLserver config (`SRV Game.cpp:3693-3708, 403-416`) [V].
- Game-server address sent at registration comes from `gethostname()`, not the cfg (`research/servers/topology.md`) [V doc].
- Gate admission is by peer IP allow-list only; the registration build date must equal the cfg
  `build-date` (the shipped gate cfg says 612, the shipped game server sends 429) (`research/gate/subsystems.md` 3;
  `research/servers/game-gate-protocol.md` 8) [V doc].
- Gate silence threshold 30 s; it flags but never disconnects (`research/gate/subsystems.md` 3) [V doc].
- Gate reconnect budget ~60 s (counter > 20 at the 3 s tick, `SRV Game.cpp:43111`) [V].
- Drain speed 15 players per 2 s (`SRV Game.cpp:3791-3792`); final countdown from 20, about 19 s (`3715-3724`, `3804`) [V].
- Timed global shutdown ~189 s with nine steps (`research/gate/subsystems.md` 6) [V doc].
- WLS limits: 100 game servers, 100 maps, 9999 accepted sockets; MLS: 4 worlds (`official-worldserver.md` 3.1, 1;
  `official-mainserver.md` 5) [V doc].
- The game server never reads content cfgs from disk; only `GServer.cfg`, `notice.txt`, `Crusade.cfg`,
  `WarSchedule.cfg`, `GameData\CrusadeGUID.txt` and maps (`research/servers/topology.md`) [V doc].

## 8. Couplings

Out of server-topology:
- **session**: `bAccept` admission and the shutdown drain (`_iForcePlayerDisconect` -> `DeleteClient`) [V].
- **persistence**: drain saves through the Log link; codes 3/4 mean that link may already be gone, so the
  drain falls back to `LocalSavePlayerData` [V].
- **content systems** (items, NPCs, magic, skills, potions, quests, crafting, teleport, NPC drops; gameplay
  cluster): their config tables are filled only by the WLS push; coupling points are the `_bDecode*ConfigFileContents`
  decoders reached from `MsgProcess` (`SRV Game.cpp:15198-15268`) [V doc].
- **crusade** (gameplay cluster): `OnStartGameSignal` reads crusade structure/schedule/GUID files; the occupy-flag
  timer in `OnTimer`; global crusade start/end travel as GSM over the gate [V doc].
- **map/NPC** (gameplay cluster): `_bRegisterMap` and `OnStartGameSignal` spawn map NPCs (`_bReadMapInfoFiles`) [V doc].
- **chat / party / guild / gm** (other systems): use the gate bus that this system registers and keeps alive [V].

Into server-topology:
- **admin**: `/shutdownthisserverrightnow`, `/disconnectall`, monitor tools and the hotkeys trigger shutdown
  paths (`admin.md`) [V].
- **network**: socket events drive every link state change [V].

## 9. Current behaviour vs intent

- Gate cfg build date 612 vs game server 429, the WLS registration id, the missing `game-server-list` and the `DEF_DEFENCEHACKING` config wait stop the shipped set from starting ([TOPO-001](../issues/server-topology.md#topo-001-shipped-pieces-do-not-fit-together-as-configured)).
- `DEF_DEFENCEHACKING` is a world-server security switch (start-up depends on the pushed WLserver config), not anti-cheat ([TOPO-002](../issues/server-topology.md#topo-002-defencehacking-is-a-world-server-security-switch-not-anti-cheat)).
- A rejected game server keeps its socket and still receives stock messages and totals ([TOPO-003](../issues/server-topology.md#topo-003-rejected-gate-peers-keep-receiving-traffic)).
- After the last step the gate re-sends notice 1 every 21 s while any server has `m_bIsShutDown` ([TOPO-004](../issues/server-topology.md#topo-004-gate-shutdown-tail-re-sends-notice-1)).
- The heartbeat is withheld while the Log link is down ([TOPO-005](../issues/server-topology.md#topo-005-heartbeat-doubles-as-log-link-health)).
- `bInit` comments 50 per tick; the timer drains 15 per 2 s ([TOPO-006](../issues/server-topology.md#topo-006-comment-vs-code-on-shutdown-drain-speed)).
- `_Manager_Shutdown` returns immediately ([TOPO-007](../issues/server-topology.md#topo-007-manager-shutdown-is-dead)).
- The MLS compares the WLS shutdown notice code at the wrong offset ([TOPO-008](../issues/server-topology.md#topo-008-wls-shutdown-notice-never-accepted-by-the-mls)).
- Start order is not enforced or reported ([TOPO-009](../issues/server-topology.md#topo-009-start-order-is-implicit)).
- The world user cap is display-only ([SESS-007](../issues/session.md#sess-007-world-user-cap-parsed-never-enforced)).

## 10. Open questions

- Whether the operator tooling ("HBmaster", monitor programs) that drove `MSGID_REQUEST_*_SHUTDOWN` and the
  batch-file requests still exists; the gate only forwards those to monitors [U].
- Whether the game server's re-init after code 3/4 leaks the gold item and NPC tables (`OnStartGameSignal`
  allocates `m_pGold` again, `SRV Game.cpp:20244-20245`) [I, not traced].
- Whether a 2.20 WLS that pushed `WLserver.cfg` existed (the only binary predates 2002-12-08) [U].

## 11. Doc discrepancies

- `research/servers/topology.md` places the readiness check at `SRV Game.cpp:3693-3708`; confirmed [V].
- `research/game-server/functions/startup.md` says `bInit` "reads notice.txt" at 834; the call is
  `bReadNotifyMsgListFile("notice.txt")` near `SRV Game.cpp:834` [V]. No conflict.
- `research/gate/` line numbers are for `0b994cd`; the working tree is the clang-formatted `a2b7f1d`
  (`network.md` 11) [V].

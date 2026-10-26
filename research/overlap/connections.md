# connections between programs

Conventions: see `../overlap.md`. Every port is configuration, not protocol [V topology.md:70].
All links use the same XSocket framing (3-byte header: key, length; byte cipher when key != 0;
`../servers/wire-format.md`) - including the official MLS/WLS binaries [V official-worldserver.md 1].

## Diagram

```
                 log-server-port (2848 client cfg / 2981 pack)
  Client ------------------------------------------>  MLS  <------- WLS (10 sockets, main-log-server-port)
    |  \                                                              ^   ^
    |   \--- world-server-port 2501 (re-enter after server change) ---/   |
    |                                                                     | internal-log-server-port 3840
    | game-server-port (address from MLS/WLS ENTERGAME)                   | (1 main + 10 sub sockets)
    v                                                                     |
  GS A (maps a..) ------------------------------------------------------- +
  GS B (maps b..) ------------------------------------------------------- +
     \ gate-server-port 6502      \
      v                            v
       Gate  (GSM relay, party registry, user total, shutdown)  <--- monitor tools (same port)
```

## Link table

| # | From -> To | Port / cfg key | Message families | Lifecycle | On loss |
|---|---|---|---|---|---|
| 1 | C -> MLS | `log-server-port` (CLI LOGIN.CFG) | LOGIN, CREATENEWCHARACTER, DELETECHARACTER, ENTERGAME, CHANGEPASSWORD (`../shared/messages/client-log.md`) | short-lived; opened per request from the login screens [V CLI Game.cpp:15526-15610] | client shows an error; no state kept [I] |
| 2 | C -> WLS | `world-server-port` (2501) | ENTERGAME only (MsgType ignored) [V official-worldserver.md 4] | after `DEF_NOTIFY_SERVERCHANGE` from the GS [V SRV Game.cpp:18646-18667, CLI Game.cpp:45039-45090] | re-entry fails; account state times out after 20 s to NULL [V] |
| 3 | C -> GS | `game-server-port` | MOTION, COMMON, NOTIFY, EVENT (`../shared/messages/client-game.md`) | whole play session; address from link 1 or 2 CONFIRM [V CLI Game.cpp:15331-15360] | GS `DeleteClient` saves through link 4 [V `../systems/session.md`] |
| 4 | GS -> WLS | `log-server-address` + `internal-log-server-port` | registration, config push, player data load/save, account status, guilds, logs (`../shared/messages/game-log.md`) | opened at GS start; GS accepts no players until registered, all configs received and 10 sub sockets up [V SRV Game.cpp:3693-3708] | sub socket: reconnect; main lost -> shutdown code 3, drain saves locally [V `../systems/server-topology.md` 2.7]; WLS side removes the server and clears its accounts [V official-worldserver.md 3.5] |
| 5 | GS -> G | `gate-server-address` + `gate-server-port` | REGISTERGAMESERVER, GAMESERVERALIVE (3 s), PARTYOPERATION, SERVERSTOCKMSG (1 s flush), shutdown notices (`../shared/messages/game-gate.md`, `game-game.md`) | opened at GS start; key 0 both ways [V GATE XSocket.cpp:421-422] | GS reconnects; after ~60 s shuts down with code 4 [V SRV Game.cpp:43099-43121]; gate only logs a silent server [V] |
| 6 | WLS -> MLS | `main-log-server-address` + `main-log-server-port` | REGISTER_WORLDSERVER(+SOCKET, +_GAMESERVER), WORLDSERVERACTIVATED, CHARINFOLIST, RESPONSE_LOG relay, account status (`../shared/messages/main-world.md`) | 10 sockets at WLS start, round-robin; logins for the world allowed only after HOME key -> ACTIVATED [V official-mainserver.md 5] | WLS recreates the socket on send failure [V]; MLS side on loss [U] |
| 7 | monitor -> G | `gate-server-port` | MONITORALIVE, shutdown requests | operator tools | n/a |
| 8 | GS <-> GS | none direct; via link 5 | GSM records: chat, whisper, find player, GM summon/shut-up, crusade, guild teleport | carried in SERVERSTOCKMSG | lost silently while gate is down [I] |

## End-to-end flows that cross programs
- **First entry:** C -> MLS LOGIN; MLS -> WLS CHARINFOLIST; WLS reads character files, replies; MLS
  forwards CONFIRM unchanged; C -> MLS ENTERGAME; MLS answers with the GS that registered the map;
  C -> GS; GS -> WLS player data request [V official-mainserver.md 5; topology.md].
- **Server change:** GS saves with `REQUEST_SAVEPLAYERDATA_REPLY`; on reply GS sends
  `DEF_NOTIFY_SERVERCHANGE` with the WLS address; C -> WLS ENTERGAME with new map; WLS answers with
  the GS address; party survives via gate op 7 [V topology.md 121-133; GATE PartyManager.cpp:268-316].
  Port choice depends on a build switch: `../issues/session.md#sess-005-server-change-port-depends-on-a-build-switch`.
- **World shutdown:** operator on the gate -> SENDSERVERSHUTDOWNMSG to every GS -> GS forwards to the
  log server [V game-gate-protocol.md 6]. MLS handling: `../issues/server-topology.md#topo-008-wls-shutdown-notice-never-accepted-by-the-mls`.

## Doc discrepancies
None found between source and `../servers/topology.md` / `game-gate-protocol.md` on the points
checked here (ports, key 0, reconnect budget, readiness gate).

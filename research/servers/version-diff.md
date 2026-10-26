# Version differences relevant to the Log and Gate servers

Tags and [V]/[I]/[U] as in `topology.md`. Compared: HGserver 2.03 (`SRV203`, `1c98b2f`), HGserver
2.20 (`SRV`), Gate 2.19 (`GATE`), client 2.20 (`CLI`). Only differences that change what a Log or
Gate server must do are listed.

## Version numbers and build dates

| Program | Version string | Build date | Cite |
|---|---|---|---|
| HGserver 2.03 | "2"."03" | 831 | `SRV203 Version.h:2-5` |
| HGserver 2.20, InterServer / USA / Korea | "2"."20" | 306 / 222 / 207 | `SRV Version.h:3-17` |
| HGserver 2.20 tree, `DEF_CHINESE` (the active flag) | "2"."19" | 429 | `SRV Version.h:19-22`, `SRV GlobalDef.h:16` |
| HGserver 2.20 tree, other countries | "2"."19" | 213 | `SRV Version.h:24-28` |
| Gate | "2002-6-20" in the title | cfg `build-date` (612 in the repo cfg) | `GATE Wmain.cpp:162`, `GATE GateServer.cfg` |
| Client 2.20 | 2.19 or 2.20 by language define | - | `CLI GlobalDef.h:10-24` |

So the "2.20" game server tree as configured builds as 2.19 with build date 429. The build date only
matters to the gate (registration check). [V]

## HGserver 2.03 vs 2.20

| Area | 2.03 | 2.20 | Effect on the Log / Gate server |
|---|---|---|---|
| Wire format (XSocket) | same | same | none [V: files identical] |
| Gate registration | 39 + 11*M, no build date (`SRV203 Game.cpp:18461`) | + build date, 43 + 11*M (`SRV Game.cpp:20711`) | the 2.19 gate rejects a 2.03 server (build-date mismatch) unless patched [V code / I result] |
| Parties | not via the gate (no `MSGID_PARTYOPERATION` in 2.03 `Game.cpp`) | via the gate | gate party code unused by 2.03 [V by grep] |
| GSM records | 0x01-0x0C (`SRV203 NetMessages.h:402-404`); find-character carries one name | 0x01-0x12; find-character two names | the gate relays both [V] |
| Server-change port | `internal-log-server-port` always (`SRV203 Game.cpp:16474-16487`) | `world-server-port` from the pushed `WLserver.cfg` in `DEF_DEFENCEHACKING` builds, else internal port (`SRV Game.cpp:18646-18667`) | a 2.03 Log server takes re-entering clients on the game-server port [V] |
| Player data | text only (no `DEF_DBGAMESERVER`) | text or binary by build flag | `game-log-protocol.md` 6 [V] |
| Config pushes | item, NPC, magic, skill, potion, quest, build item, dup item ID, noticement | + world config, NPC item, WLserver file, teleport list | a 2.03 Log server can ignore the four new pushes [V by grep] |
| Log messages to the Log server | none | GM / item / crusade logs | the Log server should accept and store or ignore them [V] |
| Readiness | no WLserver condition | WLserver config required in `DEF_DEFENCEHACKING` builds (`SRV Game.cpp:3693-3708`) | push `WLserver.cfg` [V] |
| Client config | `Lserver.cfg`, `gserver.cfg` identical | same | [V] |

## Gate 2.19 vs HGserver 2.20

- The gate already expects PID + build date in registration. [V]
- The gate's `NetMessages.h` lacks GSM 0x0D-0x12 and some newer MsgIDs; irrelevant because stock
  messages pass through unparsed. [V]
- The gate defines `MSGID_REQUEST_GAMESERVER_SHUTDOWN` / `ALLGAMESERVER_SHUTDOWN` and the monitor IDs,
  which the game server never sends. [V]
- No other incompatibility found. [V for the message set compared]

## Client 2.20 vs the protocol the servers expect

- `DEF_USING_GATEWAY` is off in 2.20, so the v2.04-2.05 gateway messages are not needed. [V]
- The Log server address is hardcoded when `DEF_LANGUAGE > 3` (the 2.20 build), so a recreated Log
  server must either take that address or the client must be patched (`client-log-protocol.md` 1). [V]

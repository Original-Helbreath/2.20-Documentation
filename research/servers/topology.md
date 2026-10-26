# Topology and server roles (Helbreath v2.20)

## Sources and conventions

Paths are relative to `C:\Users\ShadowEvil\source\og-helbreath\`.

| Tag | Source | Notes |
|---|---|---|
| `SRV` | `.claude/scratch/srv220/` | HGserver 2.20, export of `sources/hgserver-librehelbreath` @ `1697e41` |
| `SRV203` | `sources/hgserver-librehelbreath` @ `1c98b2f` | HGserver 2.03, read with `git show 1c98b2f:<file>` |
| `CLI` | `sources/client-2.20/` | Client 2.20 |
| `GATE` | `sources/gateserver-librehelbreath` @ `0b994cd` | Official Gate server v2.19 (2014 import, untouched) |

- A citation is `TAG file:line`. Line numbers count `\n` from 1 in the file as stored.
- Source comments are Korean in UTF-8 (the brief said cp949; the files actually decode as UTF-8).
- **[V]** = verified, read in code. **[I]** = inferred (reasoning given). **[U]** = unknown.
- Integers are little-endian x86; see `wire-format.md`.

## Programs

| Program | Source? | Role |
|---|---|---|
| Client (`Helbreath.exe`) | yes (`CLI`) | Talks to the Log server for everything before play, then to one game server at a time. [V] |
| HGserver (game server) | yes (`SRV`) | Hosts a list of maps (`game-server-map` lines, `SRV gserver.cfg:20-28`). Many per world. Owns no persistent player data. [V] |
| Gate server | yes (`GATE`) | One per world. Message bus between game servers: broadcast relay, party registry, total user count, global shutdown, monitor feed. Never talks to the Log server or the client. [V] |
| Log server | **no** | Accounts, characters, guilds, config files, map->server routing, client login. In 2.20 split into MLS + WLS (below). [V from protocol; internals I] |
| Monitor / manager tools | no | Connect to the gate as non-game-server peers (`GATE GateCore.cpp:217-223, 710-755`), and to game servers with `MSGID_MANAGER_INIT` / `MSGID_BWM_INIT` (`SRV Game.cpp:14986-15002`). Optional. [V] |
| "Gateway" (v2.04-2.05) | no | A client-facing proxy picked by load (`MSGID_GETMINIMUMLOADGATEWAY`). Compiled out of client 2.20 (`DEF_USING_GATEWAY` undefined: `CLI Game.cpp:14786, 15353, 27747, 45074`). Not the Gate server. [V] |

### The Log server is really two programs in 2.20

- **MLS, Main Log Server** - account authority, the address in the client's login config. [I]
- **WLS, World Log Server** - one per world ("WS1" = EFREET). Holds characters, accepts the game
  servers, routes players to game servers by map. [I]

Evidence:
- `SRV WLserver.cfg:3-12`: `world-server-name = WS1`, `world-server-address`, `world-server-port = 2501`,
  `internal-world-server-port = 3840`, `main-log-server-address = 211.115.208.39`,
  `main-log-server-port = 4500`. No HGserver code opens this file; it is the WLS's own config. [V]
- The game server handles that file as `MSGID_WLSERVERFILECONTENTS` and only takes `world-server-port` from it
  (`SRV Game.cpp:15238-15241, 25019-25066`) [V, receive side]. That a WLS pushes it to every game server is [I]: the only WLS
  binary never sends 0x0FA40007 (`official-compat-220.md` section 6).
- `DEF_ENTERGAMEMSGTYPE_NEW_TOWLSBUTMLS` ("new, to WLS but MLS") `SRV NetMessages.h:315`, sent by the
  client after a server change (`CLI Game.cpp:45087`). [V]
- Comment at `SRV Game.cpp:14917`: the shutdown message "goes to the WLS; once delivered, the whole WLS
  refuses new users". [V]
- Client comment `CLI Game.cpp:3043`: "MLS opens 3 or more ports". [V]
- IP match: client `CLI LOGIN.CFG` `log-server-address = 211.115.208.39`, port 2848, equals WLS's
  `main-log-server-address 211.115.208.39`. The WLS and the gate share `211.115.208.50`
  (`SRV WLserver.cfg:7`, `SRV GateServer.cfg:3`). [V for the values; the deployment reading is I]
- `SRV Lserver.cfg:3-9` is an older single Log-server config: `log-server-port 2848` (clients),
  `internal-log-server-port 3840` (game servers), plus `gate-server-address/port` (so that Log server
  also connected to the gate, see below). Present unchanged in 2.03 (`SRV203 Lserver.cfg`). [V]

For recreation the two can be one process (see `world-log-server.md`, `recreation-plan.md`).

## Connections and ports

| From | To | Port (cfg key) | Purpose | Cite |
|---|---|---|---|---|
| Client | MLS | 2848 (`log-server-port`) | login, char list, create/delete char, password, enter game | `CLI LOGIN.CFG`, `CLI Game.cpp:15526-15610` [V] |
| Client | WLS | 2501 (`world-server-port`) | re-enter after a server change | `SRV Game.cpp:18646-18667`, `CLI Game.cpp:45039-45090` [V] |
| Client | HGserver | 2101 example (`game-server-port`) | gameplay; address/port given by the Log server | `CLI Game.cpp:15331-15360` [V] |
| HGserver | WLS / Log | 3840 (`internal-log-server-port`) | 1 main + 10 sub sockets, all to the same address:port | `SRV Game.cpp:799-801, 3729-3738`, `SRV Game.h:103` [V] |
| HGserver | Gate | 6502 (`gate-server-port`) | one socket | `SRV Game.cpp:808-810` [V] |
| WLS | MLS | 2981 = the MLS public port (`main-log-server-port` in the pack cfg; the WLS dials exactly that key); 4500 is only the 2.20 sample value | protocol: `official-mainserver.md` 5; MLS acceptance on a public socket unverified (`world-log-server.md` section 1) | `SRV WLserver.cfg:11`; `.claude/scratch/v382core/WLserver.cfg:10`, `HMLServer.cfg:3`; `wls-port.md` [V cfg and port choice / U public-socket acceptance] |
| Log server (old single Log server only; no gate key in the MLS or WLS config) | Gate | 6502 | as a "monitor" peer: receives game-server alive/down notices | `SRV Lserver.cfg:8-9` [V]; behaviour [I] from `GATE GateCore.cpp:710-755`; key sets of the official binaries: `official-mainserver.md` section 1, `official-worldserver.md` section 1 |
| Monitor tools | Gate | 6502 | status display, remote shutdown/reboot | `GATE GateCore.cpp:308-423` [V] |

Every port above is configuration, none is protocol. The 2848 / 3840 / 6502 figures are sample values from `Lserver.cfg` /
`GateServer.cfg`; the MLS pack cfg uses 2981 / 3840 (`HMLServer.cfg:3-4`) and the sample `SRV gserver.cfg:8, 11` uses the
placeholders 111 / 22. Take the real values from the deployment's cfg files.

Game server config keys actually read (2.20): `game-server-name`, `game-server-port`,
`log-server-address`, `internal-log-server-port`, `game-server-map`, `gate-server-address`,
`gate-server-port` (`SRV Game.cpp:5333-5339`). The file name is `GServer.cfg` (`SRV Game.cpp:791`).
`game-server-address` is ignored in 2.20: the address is taken from `gethostname()`
(`SRV Game.cpp:5250-5273`), so on a multi-homed or NATed host the game server registers its first
local IP. [V]

```
                 2848                  2981 (4500 in 2.20 sample)
  Client  ----------------->  MLS  <-------------------  WLS (WS1)
    |   \                                               ^  ^    ^ 2501 (client after server change)
    |    \------------------------------------------------/  |
    |                                      3840 (main + 10 sub sockets)
    | game-server-port                                   |
    v                                                    |
  HGserver A (maps a..) --------------------------------+
  HGserver B (maps b..) --------------------------------+
      \        \
       \ 6502   \ 6502
        v        v
          Gate  (relay between game servers; party registry; user total; shutdown)
```

## Startup and readiness of a game server

The game server accepts no players until all of these are true (`SRV Game.cpp:3693-3708`) [V]:
item, NPC, magic, skill, potion, quest and build-item configs received from the Log server; gate
registration confirmed; Log registration confirmed; all 10 sub-log sockets connected; and, in
`DEF_DEFENCEHACKING` builds, the WLserver config received. The game server reads no game-data
config files itself; all of them arrive from the Log server (`SRV Game.cpp:15198-15268`; the only
local reads are `GServer.cfg`, `notice.txt`, `Crusade.cfg`, `WarSchedule.cfg`,
`GameData\CrusadeGUID.txt`, map files, `SRV Game.cpp:791, 834, 20233-20239, 25601-25606`). [V]

## Verdict on the two hypotheses

**Hypothesis 1 - "Log server is the login/account server; Gate is a middleman between login and
world/game servers."**
- First half: **confirmed**. The client authenticates against the Log server
  (`CLI Game.cpp:1665-1703`), and the Log server also stores characters and serves data to the game
  servers.
- Second half: **rejected**. The gate is not between the Log server and the game servers: each game
  server connects to the Log server directly (`SRV Game.cpp:799-801`). The gate source has no outbound
  connection and no Log-server message (`GATE Wmain.cpp:234-235` is its only socket, a listener;
  `GATE GateCore.cpp:299-448` handles only game-server and monitor messages). [V]

**Hypothesis 2 - "Gate is the broker between map servers when a player crosses to a map another
HGserver hosts."**
- **Rejected for player routing.** On a teleport to a map the game server does not host, it saves
  the player to the Log server with `MSGID_REQUEST_SAVEPLAYERDATA_REPLY` (`SRV Game.cpp:23361-23373`,
  also 23430-23441, 23461-23474, 23502-23516). When the Log server replies
  `MSGID_RESPONSE_SAVEPLAYERDATA_REPLY`, the game server sends the client `DEF_NOTIFY_SERVERCHANGE`
  with the destination map and the **Log (world) server's** address and port
  (`SRV Game.cpp:36354-36372, 18646-18667`). The client then reconnects to that Log server and sends
  `MSGID_REQUEST_ENTERGAME` with the new map name (`CLI Game.cpp:45039-45090`); the Log server answers
  with the address of the game server that registered that map (`CLI Game.cpp:15331-15360`). The map
  lists reach the Log server in `MSGID_REQUEST_REGISTERGAMESERVER` (`SRV Game.cpp:4345-4382`). The gate
  also receives the map list but skips it unread (`GATE GateCore.cpp:476-481`). [V]
- **Partly right in spirit:** the gate is the only channel *between* game servers. It relays
  cross-server chat, whispers, find-player, GM summon/shut-up, crusade state and parties, and it
  keeps party membership alive across a server change (party op 7, `GATE GateCore.cpp:1269-1271`,
  `GATE PartyManager.cpp:268-316`). [V]

**Corrected model:** Log server = account + character database, config distributor, and the map ->
game-server router for both first entry and every server change. Gate = inter-game-server message
bus and party registry, with operator shutdown control. Neither hypothesis about the gate holds.

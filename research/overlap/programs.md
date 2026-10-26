# programs: responsibility, ownership, trust

Conventions: see `../overlap.md`. Topology detail: `../servers/topology.md`.

## Client (C)
Presents the world and collects input. It renders from local assets (sprite paks, `.amd` maps,
sounds) and mirrors the slice of server state the game server sends it (view snapshot, own
character, bag, bank, dialogs) [V `../systems/client-state.md`]. **Owns:** screen state, prediction
of its own movement, display copies of item/magic/skill names (`contents\ItemName.cfg`, magic/skill
cfgs, `../shared/formats/client-files.md`), `LOGIN.CFG` (log server address/port 2848) [V]. No listen
port; three outbound links in sequence. **Trusts:** the MLS for authentication and the character
list, the MLS/WLS for which game server to join, the game server for every gameplay outcome.

## Game Server (GS, HGserver)
Runs the simulation for the maps named in `GServer.cfg` (`game-server-map`) [V SRV Game.cpp:5333-5339].
**Owns:** live players, NPCs, items on the floor, tiles, combat, crusade state for its maps;
the player-record codec (`_iComposePlayerDataFileContents` / `_bDecodePlayerDatafileContents`,
`../systems/persistence.md`). **Local files:** `GServer.cfg`, `notice.txt`, `Crusade.cfg`,
`WarSchedule.cfg`, `GameData\CrusadeGUID.txt`, map `.amd` and map-info `.txt` [V SRV Game.cpp:791, 834,
20233-20239, 25601-25606]. **Port:** `game-server-port` (client listener, e.g. 2101). **Trusts:**
the WLS for all content tables (pushed at registration), for loading and storing player and guild
data, and for map routing; the gate for every message to another game server and for party
membership; the client for nothing it can recheck (but see `../issues.md` for client-only checks).

## Gate Server (G)
The inter-game-server bus of one world. It relays stock (GSM) containers byte-for-byte, keeps the
party registry (`PartyManager`), sums user counts, drives operator/monitor shutdown, and logs item
events [V `../servers/game-gate-protocol.md`; GATE GateCore.cpp:299-448]. **Owns:** party table,
registered-server table, `GateServer.cfg` (`gate-server-port` 6502, `build-date`, `game-server-list`).
Never talks to the client or a log server [V topology.md:25]. **Trusts:** registered game servers
(IP allow-list + build date is its only admission) and monitors (fixed passphrase).

## Main Log Server (MLS, MainServer.exe v3.51)
Account authority and front door. **Owns:** account files and the coarse "in game in world X" table
(10 000 entries), the world list (max 4 worlds) and the map -> game server table forwarded by each WLS
[V official-mainserver.md 1, 5, 6]. **Ports:** `internal-log-server-port` (3840, worlds),
`log-server-port` and +1, +2 (clients; 2981 in the pack cfg, 2848 in the 2.20 client) [V].
`HMLserver.cfg`. **Trusts:** WLS for character files and for the client-facing CONFIRM payload,
which it forwards unchanged.

## World Log Server (WLS, WorldServer.exe V2.15)
Per-world database and config distributor. **Owns:** character files, guild files, occupy-flag
files, content cfgs it pushes verbatim (Item/Npc/Magic/Skill/Potion/Quest/BuildItem/DupItemID,
Noticement), logs (`GMLogData`, `ItemLogData`, `EKEventsLog`), and the fine account state machine
INIT/WAIT/PLAYING/NULL [V official-worldserver.md 6, 7]. **Ports:** `internal-world-server-port`
(3840, game servers), `world-server-port` (2501, clients re-entering after a server change) [V].
**Trusts:** the MLS for account authentication (INIT status carries account, password, level); the
game server for the content of every saved record.

## What nobody owns alone
- **Identity of a session:** account + character name travel C -> MLS -> WLS -> GS with no ticket;
  each hop re-checks against its own table [I; see `../issues/session.md#sess-001-no-session-ticket-credential-checks-are-weak`].
- **Map -> server routing:** GS declares maps (to WLS and gate), WLS forwards to MLS, MLS and WLS
  both answer ENTERGAME; the gate receives the list and skips it [V GATE GateCore.cpp:476-481].
- **Liveness:** GS heartbeat to the gate is suppressed while its log link is down, so gate liveness
  doubles as log-link liveness [V; `../issues/server-topology.md#topo-005-heartbeat-doubles-as-log-link-health`].

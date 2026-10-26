# Server program configs and local state files

Files each server process opens itself at start-up (addresses, ports, schedules) and the small
state files the game server keeps. Content tables pushed by the Log server are in
`server-content-cfg.md`. Tags: [V] read in code, [I] inferred. `srv220` = `.claude/scratch/srv220/`,
`gate219` = `sources/gateserver-librehelbreath` at 0b994cd (extracted to `.claude/scratch/shared-ref/gate219/`).

## Contents
- Common syntax
- gserver.cfg (game server)
- GateServer.cfg (gate)
- Lserver.cfg (2.03-era single Log server)
- WLserver.cfg / HMLserver.cfg (official 2.20 Log servers)
- Crusade.cfg, WarSchedule.cfg, CrusadeGUID.txt
- OccupyFlag files
- notice.txt
- Logs written
- Gaps

## Common syntax

`key = value` tokens, separators `"= \t\n"` (`srv220/Game.cpp:5208`), keys compared with
`memcmp` on a fixed prefix length, so a longer key that starts with a known key also matches [V].
`[CONFIG]` is an ignored token.

## gserver.cfg (game server)

Read by `CGame::bReadProgramConfigFile("GServer.cfg")` (`srv220/Game.cpp:791`, function `:5202`;
a `DEF_DBGAMESERVER` twin is at `:5053`) [V]. Writer: hand-edited.

| Key (prefix len) | Member | Sample `srv220/gserver.cfg` |
|---|---|---|
| `game-server-name` (16) | `m_cServerName` | `EFREET` |
| `game-server-port` (16) | `m_iGameServerPort` | `2101` |
| `log-server-address` (18) | `m_cLogServerAddr` | |
| `internal-log-server-port` (24) | `m_iLogServerPort` | `111` |
| `game-server-map` (15) | adds a map to load (mode 5, `:5296-5308`; `cReadMode = 0` at 5307) | name over 10 chars aborts start-up ("must within 10 chars", `:5300`); `_bRegisterMap` failure aborts (`:5304`) |
| `gate-server-address` (19) | `m_cGateServerAddr` | |
| `gate-server-port` (16) | `m_iGateServerPort` | `22` |

Keys at `srv220/Game.cpp:5333-5339` [V]. `game-server-address` in the sample is **not** a key
(the game server address is taken from the local host, `:5264`) [V/I]. 2.20 log-server address is
the World Log server (see `research/servers/topology.md`).

## GateServer.cfg (gate)

Read by `CGateCore::bReadProgramConfigFile("GateServer.cfg")` (`gate219/GateCore.cpp:65`,
function `:73`) [V]. Keys (`:141-145`): `gate-server-port` (`m_iGateServerPort`), `build-date`
(`m_iBuildDate`, compared with the game server's build date), `game-server-list` (IP allow-list),
`gate-server-address` (second IP range, added 2003-01-17). Meaning and run notes:
`research/servers/gate-server.md`. Sample `srv220/GateServer.cfg` has only address and port.

## Lserver.cfg (2.03-era single Log server)

No reader in any source here (the Log server source is missing) [V by search]. Sample
`srv220/Lserver.cfg`: `log-server-address`, `log-server-port` (2848, client-facing),
`internal-log-server-port` (3840, game-server-facing), `gate-server-address`, `gate-server-port`
[V sample]. Port roles [I from names and `research/servers/topology.md`].

## WLserver.cfg / HMLserver.cfg (official 2.20 Log servers)

Read by the official binaries only. Keys, storage and addresses:
`research/servers/official-worldserver.md` section 1 (`ReadWLServerConfig 0x401610`) and
`official-mainserver.md` section 1 (`HMLserver.cfg`) [V there]. The WLS also pushes its config to
the game server as `MSGID_WLSERVERFILECONTENTS`; the game server reads only `world-server-port`
from it (`srv220/Game.cpp:25049`) [V]. Sample `srv220/WLserver.cfg`: `world-server-name`,
`world-server-address`, `world-server-port`, `internal-world-server-port`,
`main-log-server-address`, `main-log-server-port`, ...

## Crusade.cfg, WarSchedule.cfg, CrusadeGUID.txt

All read at crusade init, `srv220/Game.cpp:20233-20239` [V].

- **Crusade.cfg** - `bReadCrusadeStructureConfigFile` `:5354`. Record `crusade-structure = Num
  MapName Type X Y` (`:5400-5465`) [V]; sample `srv220/Crusade.cfg`.
- **WarSchedule.cfg** - `bReadCrusadeScheduleConfigFile` `:50691`. `game-server-name = <name>`
  selects the block for this server (compared with `m_cServerName`, `:50729`), then
  `schedule = Day Hour Minute` records (`:50743-50766`) [V]. No sample in `srv220/`.
- **GameData\CrusadeGUID.txt** - read by `bReadCrusadeGUIDFile` `:50460` (`CrusadeGUID`,
  `winner-side`), written by `_CreateCrusadeGUID` `:50420` as `CrusadeGUID = %d` /
  `winner-side = %d` [V]. Sample `srv220/GameData/CrusadeGUID.Txt`.

## OccupyFlag files

- Composed by `_iComposeFlagStatusContents` `srv220/Game.cpp:40307`: one line per flag
  `flag = Side X Y EKNum` [V `:40326`]. **2.20 never saves occupy flags** [V]: the send
  `case` for `MSGID_REQUEST_SAVEARESDENOCCUPYFLAGDATA` / `...ELVINE...` is inside a `/* */` block
  (`srv220/Game.cpp:4424-4468`) and every caller is commented out (`:3763-3764`, `:11024`,
  `:14865`, `:14934`); the commented call at `:4441` uses a two-argument `_iComposeFlagStatusContents`
  that no longer exists (the definition at `:40307` takes one). The load path stays live: the WLS
  pushes `MSGID_*OCCUPYFLAGSAVEFILECONTENTS` at start, decoded by
  `_bDecodeOccupyFlagSaveFileContents` `:40340` [V]. The official WLS stores them as
  `GameData\AresdenOccupyFlag.txt` / `ElvineOccupyFlag.txt`
  (`research/servers/official-worldserver.md:112,149`).
- `SaveOccupyFlagData` `:47991` begins with `return;` (`:47997`), so it never reaches its local
  `GameData\OccupyFlag.txt` write (`:48009`) [V]. Net effect: crusade occupy flags are saved
  nowhere by a 2.20 game server.

## notice.txt

`bReadNotifyMsgListFile("notice.txt")` (`srv220/Game.cpp:834`, function `:36258`), separators
`"=\t\n;"`; each `notify_msg = <text>` line adds one broadcast notice [V `:36290-36307`].

## Logs written

`Events.log`, `AdminEvents.log`, `LogEvents.log`, a dated log and `DebugMsg.log` (`srv220/Wmain.cpp:384-486`; DebugMsg at 486),
`Error.Log` (`srv220/Game.cpp:7068`) - append-only text, out of scope for parsing [V].

## Gaps

- No source reader for `Lserver.cfg` (2.03 Log server missing).
- `game-server-map` handling (mode 5) not traced to the map loader here.

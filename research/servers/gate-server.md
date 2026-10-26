# The official Gate server (v2.19)

Source: `GATE` = `sources/gateserver-librehelbreath` @ `0b994cd` (read with `git show 0b994cd:<file>`;
HEAD `a2b7f1d` only reformats it). Files: `Wmain.cpp` (window, timers, log files), `GateCore.cpp/.h`
(all logic), `PartyManager.cpp/.h`, `XSocket.*`, `NetMessages.h`, `GateServer.cfg`, VC6 and VS
project files. Tags and [V]/[I]/[U] as in `topology.md`. Wire messages are tabled in
`game-gate-protocol.md`; this doc covers behaviour, configuration and the verdict.

## Contents
1. What it is
2. Configuration
3. Who may connect
4. What it does, by timer and by message
5. Operator controls
6. Files it writes
7. Limits and quirks
8. Hypotheses checked against this source
9. Running it with the 2.20 game server

## 1. What it is

A single-window Win32 program, one per world, that listens on one TCP port and never connects
anywhere (`GATE Wmain.cpp:235` is its only socket). Peers are game servers and "monitors". It has no
database, no client-facing protocol and no Log-server protocol. [V]

## 2. Configuration

File `GateServer.cfg` in the working folder (`GATE GateCore.cpp:65`), parsed by
`bReadProgramConfigFile` (`GATE GateCore.cpp:73-179`). `key = value`, tokenised on `=`, spaces, commas, tabs
and newlines (`seps = "= ,\t\n"`, `GATE GateCore.cpp:79`); keys matched by prefix with `memcmp`. [V]

| Key | Meaning | Cite |
|---|---|---|
| `gate-server-port` | listen port (6502 in both sample files) | `GATE GateCore.cpp:107-114, 141` |
| `build-date` | must equal the build date each game server sends at registration | `GATE GateCore.cpp:117-121, 143` |
| `game-server-list` | one allowed peer IP per line; repeat the key for each (no bound is checked, [GATE-008](../issues/gate.md#gate-008-allow-list-loader-has-no-bound)) | `GATE GateCore.cpp:124-128, 144` |
| `gate-server-address` | address to listen on | `GATE GateCore.cpp:129-134, 145` |

- The repo's cfg sets address, port 6502 and `build-date = 612`, with **no** `game-server-list`
  (`git show 0b994cd:GateServer.cfg`). The copy in the HGserver tree (`SRV GateServer.cfg`) has only
  address and port. [V]
- The `gethostname()` fallback for the address never runs ([GATE-025](../issues/gate.md#gate-025-gethostname-fallback-for-the-listen-address-never-runs)), so `gate-server-address` is required in practice. [V
  code / I effect]

## 3. Who may connect

- Every accepted socket's peer IP is compared (16-byte `memcmp`) with the `game-server-list` entries;
  no match -> the connection is closed and logged to `HackAttacker.log` (`GATE GateCore.cpp:231-283`). [V]
- With no list every peer is rejected, so **both sample configs as committed accept no one**. Game
  servers, monitor tools and a Log server that connects as a monitor must all be listed. [V code / I
  that the published cfg was trimmed]
- After acceptance a peer is a game server once its `REQUEST_REGISTERGAMESERVER` is accepted; every
  other peer counts as a monitor (`m_bIsGameServer == FALSE`, `GATE GateCore.cpp:710-786`). [V]
- Capacity: 500 socket slots (`DEF_MAXCLIENTS`; 499 usable, slot 0 unused), 10 monitor status slots
  (`GATE GateCore.h:26-29`); the allow-list has no bound check (`GATE GateCore.cpp:124-128`). [V]

## 4. What it does

Every 3 s (multimedia timer started at `GATE Wmain.cpp:238`, handled in `GATE GateCore.cpp:670-686`):
1. `CheckGameServerActivity` - a game server silent for 30 s (`DEF_NORESPONSELIMIT`) is logged as
   "does not response" and reported to monitors; it is **not** disconnected (`GATE GateCore.cpp:688-708`).
2. `SendTotalClientsToGameServer` - sums every server's last reported count and sends the world total
   to each game server (`GATE GateCore.cpp:788-850`).
3. `CheckGlobalCommand` - F1+F12 held: immediate shutdown of all game servers (`GATE GateCore.cpp:864-909`).
4. `CheckAutoShutdownProcess` - the timed world shutdown (`GATE GateCore.cpp:1033-1106`).
5. `PartyManager::CheckMemberActivity` - members marked "changing server" for more than 20 s are
   removed and the removal broadcast (`GATE PartyManager.cpp:279-316`).

Every 60 s a worker thread writes each game server's user count to the daily game-server log
(`GATE Wmain.cpp:44-74`). [V]

On messages (`GATE GateCore.cpp:285-449`):
- registration with build-date and duplicate-name checks; the map list is skipped (`GATE GateCore.cpp:451-537`);
- alive / user count, forwarded to monitors;
- `SERVERSTOCKMSG` relayed unparsed to all other game servers (`GATE GateCore.cpp:1015-1031`);
- party operations (`GATE GateCore.cpp:1110-1274`, `GATE PartyManager.cpp`), memory only, party ids 1..4999 sharing one pool of
  4999 member slots, 8 members per party at most (`GATE PartyManager.h:19-20, 38-44`);
- item logs appended to `ItemEvents.log`;
- monitor messages: shutdown one / all servers (both check a fixed MsgType value 0x5A8E and a fixed
  10-byte passphrase in the source), reboot / batch-file requests forwarded to monitors (these check the MsgType only; a
  wrong MsgType deletes the sender), monitor heartbeat (`GATE GateCore.cpp:311-319, 368-377, 395-400`). [V]

When a game server's socket drops, its parties' members on that server are dropped
(`PartyManager::GameServerDown`, `GATE PartyManager.cpp:250-266`) and monitors are told. [V]

## 5. Operator controls

Keyboard on the gate window (`GATE GateCore.cpp:579-668, 852-862`) [V]:

| Keys | Effect |
|---|---|
| F2 | list registered-but-not-active servers, with reason (not activated / build date mismatch / same name) |
| F3 | list active servers |
| F1 + F4 | start the timed world shutdown (about 3 minutes, see `game-gate-protocol.md` 6) |
| F1 + F6 | send shutdown notice 1 to all servers (when F1 is not held it falls through F7 into the F12 case and clears the F12 flag, `GATE GateCore.cpp:648-666`) |
| F1 + F7 | send shutdown notice 2 |
| F1 + F12 | shut down all game servers now |

Close box: "Quit gate server?" Yes/No (`GATE Wmain.cpp:104-108`). Window layout: `ui.md`.

## 6. Files it writes

| File | Content | Cite |
|---|---|---|
| `ItemEvents.log` | item log lines from `MSGID_ITEMLOG` | `GATE Wmain.cpp:356-375` |
| `GameServerLog\GameServerLogYYYYMMDD.log` | per-minute "(name) Total user(n)" | `GATE Wmain.cpp:377-403` |
| `HackAttacker.log` | rejected peer IPs | `GATE Wmain.cpp:405-423` |

Every line starts `(YYYY:MM:DD:HH:MM) - ` (`"(%4d:%2d:%2d:%2d:%2d) - "`, so fields are space-padded,
not zero-padded). The `GameServerLog` folder is created by `_mkdir("GameServerLog")` before every write (`GATE Wmain.cpp:384`). [V]

No party, guild or account file exists. [V: the only `fopen` calls are these three]

## 7. Limits and quirks

- `MONITORALIVE` indexes `m_cMonitorStatus[10]` with the peer-supplied MsgType
  (`GATE GateCore.cpp:409-415`, [GATE-009](../issues/gate.md#gate-009-monitoralive-msgtype-is-an-unchecked-array-index)). Only allow-listed peers can send it. [V]
- Party response sizes exceed the bytes written (zero tail). [V]
- Duplicate server names are rejected; the allow-list compares IP strings exactly, so the IP the
  game server connects from must be listed as text. [V]

## 8. Hypotheses checked against this source

- **H1 (gate between login and game servers): rejected.** No code connects to or accepts a Log
  server specially; there is no Log-server MsgID in its dispatch (`GATE GateCore.cpp:299-448`); there
  is no outbound socket. A Log server that connects would be just another monitor. [V]
- **H2 (gate brokers map crossings): rejected for routing.** Registration skips the map list unread
  (`GATE GateCore.cpp:476-481`), so the gate cannot know which server hosts a map. Server changes go
  through the Log server (`topology.md`). The gate's only role in a crossing is keeping the player's
  party membership for 20 s (party op 7). [V]
- **What it actually is:** the inter-game-server bus (broadcast relay), party registry, world user
  counter and operator shutdown console. [V]

## 9. Running it with the 2.20 game server

- The 2.19 gate already parses the 2.20 registration (PID + build date). [V]
- Set `build-date` to the `DEF_BUILDDATE` of the game server build (`SRV Version.h`, included from `Game.h:37`: China 429,
  Korea 207, USA 222, InterServer 306, otherwise 213). [V]
- List every game server IP, every monitor IP and (optionally) the Log server IP under
  `game-server-list`. [V]
- GSM 0x0D-0x12 are new in 2.20 but relayed opaquely. [V]
- Recommendation: build and use this source as is (VC6/VS project included) rather than rewriting;
  fix only the cfg. A rewrite is small (about 1300 lines of logic) if a non-Windows build is wanted. [I]

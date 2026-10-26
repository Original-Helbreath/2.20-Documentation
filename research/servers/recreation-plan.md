# Recreation plan (plan only, no code)

Tags and [V]/[I]/[U] as in `topology.md`. Goal: one client logs in, makes a character, plays on
one or more HGserver 2.20 processes, changes server, logs out, and finds the character saved.

**Approach: rebuild the Log server from the gate server's code and the game server's protocol code,
using text-file storage that matches the original formats.** The gate gives the Win32 skeleton,
socket handling, timer, log list and config reader (`ui.md`, `gate-server.md`); the game server
gives every message layout from the other end (`game-log-protocol.md`) and the character file
writer/reader (`persistence.md`); the client gives the login-side layouts (`client-log-protocol.md`).

## Contents
1. Decisions
2. Gate server
3. Log server: components
4. Log server: build order with checkpoints
5. Client and game server setup
6. Effort estimate
7. Open questions to settle by testing

## 1. Decisions

| Decision | Choice | Why |
|---|---|---|
| MLS + WLS | one process | nothing outside the pair needs the split (`world-log-server.md` 5) [I] |
| Player data format | text (build HGserver without `DEF_DBGAMESERVER`; how: section 5, the exported tree's `DEF_CHINESE` switches it on) | payload = character file; no binary conversion (`persistence.md` 7) [I] |
| Storage | text files: `Account\`, `Character\`, `Guild\` with `AscII<n>` buckets | matches the original style; plaintext passwords as the original (`persistence.md`) [I] |
| Gate | reuse the official source | complete and compatible (`gate-server.md` 9) [V] |
| Platform | Win32, VC6-era code style, from the gate's `Wmain.cpp` + `XSocket` | same look and same socket semantics [I] |

## 2. Gate server

1. Build `GATE` @ `0b994cd` (project files included). [V files exist; build not tried - unverified]
2. `GateServer.cfg`: `gate-server-address` (required, `gate-server.md` 2), `gate-server-port`,
   `build-date` = the HGserver's `DEF_BUILDDATE`, one `game-server-list` line per game server IP (and
   the Log server IP if it connects as a monitor). [V]
3. (Optional) the gate creates `GameServerLog` itself (`_mkdir`, `GATE Wmain.cpp:384`). [V]
Done when: the game server log shows the gate registration confirmed and F3 on the gate lists it.

## 3. Log server: components

| Component | Source to copy or follow |
|---|---|
| Window, loop, timer, log list, log files | `GATE Wmain.cpp` (`ui.md` 5) |
| XSocket (frame, cipher, async select) | `GATE XSocket.*` or `SRV XSocket.*` (identical framing; `wire-format.md`) |
| Config reader | `GATE GateCore.cpp:73-179` style; keys from `SRV Lserver.cfg` + `world-server-port` |
| Client listener (2848) | one request per connection; layouts in `client-log-protocol.md` |
| Game-server listener (3840) | 11 sockets per game server; all replies keyed by character name (`game-log-protocol.md` 1) |
| Re-entry listener | `world-server-port` (e.g. 2501) in the pushed `WLserver.cfg`, or the 3840 port in non-`DEFENCEHACKING` builds (`world-log-server.md` 5) |
| Config push | read `Item.cfg`, `Item2.cfg`, `Item3.cfg`, `Npc.cfg`, `Magic.cfg`, `Skill.cfg`, `portion.cfg`, `Quest.cfg`, `BuildItem.cfg`, `DupItemID.cfg` from the Log folder (optionally `Noticement.txt`, `MSGID_NOTICEMENTFILECONTENTS` 0x0FA40004, `SRV Game.cpp:15250`; and `WLserver.cfg`, required only in `DEF_DEFENCEHACKING` builds, `SRV Game.cpp:3698-3701`; the official WLS sends 11 files in its own order, `official-worldserver.md` section 3.1) and send each file as one message, then the registration CONFIRM (`game-log-protocol.md` 5) |
| Map router | map name -> game server (name, IP, port) from registrations |
| Account store | `Account\AscII<n>\<name>.txt` (`persistence.md` 4) |
| Character store | blob in/out, plus a parser for list fields (`persistence.md` 3) |
| Guild store | `Guild\AscII<n>\<name>.txt` (`persistence.md` 5) |
| Session table (memory) | account -> state (offline / selecting / playing on X / changing server), character, level, IP, timestamps (`game-log-protocol.md` 7) |

## 4. Log server: build order with checkpoints

1. **Skeleton**: window, log list, config, three listeners. Check: the window opens and logs config.
2. **Game server registration + config push**. Check: HGserver logs every config received, Log
   CONFIRM, gate CONFIRM, 10 sub sockets, and opens for players (`SRV Game.cpp:3693-3708`).
3. **Client login**: decrypt, `REQUEST_LOGIN`, account file check, CONFIRM with an empty list
   (`client-log-protocol.md` 3-5). Check: the client reaches the character screen.
4. **Account creation** (`CREATENEWACCOUNT`), or create account files by hand at first.
5. **Create / delete character**: validation as in `client-log-protocol.md` 6-7; write the initial
   character file (`persistence.md` 6). Check: the slot appears with the right figure.
6. **Enter game**: `REQUEST_ENTERGAME` -> look up the character's map -> reply with that game server's
   IP and port; on `REQUEST_PLAYERDATA` send the file; on `ENTERGAMECONFIRM` mark the account playing.
   Check: the player spawns on the `default` map.
7. **Save / logout**: `SAVEPLAYERDATA`, `SAVEPLAYERDATALOGOUT`, `NOSAVELOGOUT`; write the file; clear
   the session. Check: log out, log in, items and position kept.
8. **Server change**: `SAVEPLAYERDATA_REPLY` -> save -> `RESPONSE_SAVEPLAYERDATA_REPLY`;
   `SETACCOUNTWAITSTATUS` -> session "changing"; client `ENTERGAME` kind `NEW_TOWLSBUTMLS` -> route by
   the new map. Check with two game servers hosting different maps.
9. **Duplicate login**: playing account logs in again -> reply "already playing" or, on the force
   option, `REQUEST_FORCEDISCONECTACCOUNT` to the owning server (`client-log-protocol.md` 10,
   `game-log-protocol.md` 4).
10. **Guilds**: create / disband / member updates; guild-status byte on load.
11. **Housekeeping**: game server disconnect -> mark its accounts offline after its last saves; timeout
    of "changing server" sessions; shutdown messages; accept and log the GM / item / crusade logs.
12. **Password change** and the remaining client requests.

## 5. Client and game server setup

- **Client address patch**: the 2.20 client ignores `login.cfg` and uses a hardcoded Log address
  (`client-log-protocol.md` 1). Either patch the address in the executable or source (the string
  and port are at `CLI Game.cpp:15543-15556`), rebuild with a `DEF_LANGUAGE` value <= 3 so `login.cfg`
  is honoured, or give the Log server host that IP on a private network. [V code / I options]
- **Game server**: build without `DEF_DBGAMESERVER`. The exported tree defines `DEF_CHINESE` (`SRV GlobalDef.h:16`), and
  `DEF_CHINESE`, `DEF_JAPAN` and `DEF_USA` each switch on both `DEF_DBGAMESERVER` and `DEF_DEFENCEHACKING` (`SRV
  GlobalDef.h:72-78`). So pick `DEF_KOREA` or `DEF_INTERSERVER` (neither sets them), or delete the two defines by hand. The
  country flag also fixes the build date and version string (`SRV Version.h:3-28`): `DEF_CHINESE` 429 / minor 19,
  `DEF_KOREA` 207, `DEF_USA` 222, `DEF_INTERSERVER` 306 (all minor 20), no flag 213 / minor 19. Set the gate's `build-date`
  (section 2, step 2) to the value of the flag you build with. Keep or drop `DEF_DEFENCEHACKING` consistently
  with the Log server's re-entry port; `GServer.cfg` `log-server-address` must be the Log server's
  client-reachable address (it is sent to clients on a server change); `internal-log-server-port` =
  the Log server's game-server port; gate address/port; maps. The game server takes its own IP from
  `gethostname()`, so on a multi-homed host make sure the first address is the one clients reach
  (`topology.md`). [V]
- **Build dates** match between HGserver and gate cfg. [V]
- **Alternative to writing a Log server**: run the official MLS/WLS pair with the five patches in `official-compat-220.md`
  section 7; the game server must still be rebuilt in FILE mode (`official-compat-220.md` section 1). [V]

## 6. Effort estimate

Estimates for one developer familiar with C/C++ and Win32 sockets; not measured. [I]

| Server | Work | Estimate |
|---|---|---|
| Gate | build the official source, write the cfg, test | 0.5 - 1 day |
| Log server, minimal (steps 1-8) | skeleton from the gate, protocol from the docs, text storage | 1.5 - 2 weeks |
| Log server, full (steps 9-12, hardening, a few days of play testing) | | +1 - 2 weeks (total 2 - 4 weeks) |

## 7. Open questions to settle by testing

- Exact MsgIDs the Log server used for its replies to the client: the client ignores the MsgID and
  reads only MsgType for most responses (`client-log-protocol.md` 4); use the matching
  `MSGID_RESPONSE_*` values. [I]
- Whether item configs went as three messages (assumed) and in which order the configs were sent. [U]
- Account-status byte and the billing codes: send 0 / never use. [U]
- The trailing byte after the character text in `RESPONSE_PLAYERDATA` (`dwSize - 19`): append one NUL. [I]
- Starting items for new characters. [U]
- Whether the Log server used its gate connection for anything beyond monitoring. [U]
- Official-binary setup: the WLS dials the MLS public port 2981 (`main-log-server-port`), so the allow-list patch 4
  (`official-compat-220.md` section 7) is not needed [V]; test that the MLS accepts the WLS registration on that public socket. [U]

# Gate server 2.19: subsystems

How the pieces of the official Gate server fit together. Function-level detail with callers and callees:
`functions.md`; fields and limits: `data-structures.md`; wire layouts: `../servers/game-gate-protocol.md`.
Tags and line numbers as in `functions.md` (`GATE` = `sources/gateserver-librehelbreath` @ `0b994cd`;
[V] verified, [I] inferred, [U] unverified).

## Contents
1. Process shape and threads
2. Sockets and the event loop
3. Admission and registration
4. Relay (stock messages, totals, monitors)
5. Parties
6. Shutdown and monitor control
7. Failure and cleanup matrix
8. UI
9. Config and files

## 1. Process shape and threads

One Win32 GUI process, `WinMain` -> `InitApplication` -> `InitInstance` -> `Initialize` -> `EventLoop`
(`GATE Wmain.cpp:122-133`). [V]

| Thread | Does | Shares |
|---|---|---|
| Main (message loop) | everything: socket events, timer ticks, key events, painting, all file writes except the user-count log | owns `G_pGateCore`, the client table, the party tables |
| Multimedia timer thread (winmm) | only `PostMessage(WM_USER_TIMERSIGNAL)` every 3000 ms (`Wmain.cpp:323-339`) | the window handle |
| Logging thread (`ThreadProc`) | wakes each second; every 61st second writes each game server's name and user count to `GameServerLog` (`Wmain.cpp:44-73`) | reads `m_pClientList[1..499]` and the `CClient` fields without locking ([GATE-019](../issues/gate.md#gate-019-the-logging-thread-reads-shared-state-without-locking)) [V]; stops when `G_bThreadFlag` is cleared in the core destructor (`GateCore.cpp:50`) |

Everything else is single-threaded, so the gate has no locks and no reentrancy: a handler runs to completion before the
next message. The consequence is that a slow handler stalls every peer and the 3 s timer is only a posted message
(ticks queue up behind work) [I].

Lifetime: `Initialize` creates the core, reads the cfg, starts the logging thread, creates the listener and the timer,
logs "(!) Gate Server Listening..." (`Wmain.cpp:211-241`). `OnDestroy` tears down in the order core, listener, winsock,
timer (`Wmain.cpp:245-254`). Close box asks "Quit gate server?" first (`Wmain.cpp:104-108`). [V]

## 2. Sockets and the event loop

### 2.1 Listener and accept

- One listening `XSocket` on `gate-server-address:gate-server-port`, backlog 5, `WSAAsyncSelect(FD_ACCEPT)` posting
  `WM_USER_ACCEPT` = `WM_USER+100` (`Wmain.cpp:234-235`, `XSocket.cpp:456-489`). [V]
- `bListen`'s return value is not examined; the process logs "Listening" whether or not the bind succeeded ([GATE-001](../issues/gate.md#gate-001-listen-failure-is-not-detected)) [V].
- Each `WM_USER_ACCEPT` -> `OnAccept` -> `CGateCore::bAccept`, which accepts exactly one connection per message
  (`GateCore.cpp:231-283`). The accepted socket is registered with `WSAAsyncSelect(FD_READ | FD_WRITE | FD_CLOSE)` and
  message `WM_USER+101+slot` on the same window (`XSocket.cpp:491-520`). [V]

### 2.2 One message number per peer

`WM_ONCLIENTSOCKETEVENT` is `WM_USER+101` (`UserMessages.h:3`); the peer in slot *i* gets `WM_USER+101+i`; `WndProc`
routes anything in `[WM_USER+101, WM_USER+101+500)` to `OnClientSocketEvent` (`Wmain.cpp:110-112`). Slot 0 is never
allocated, so there are at most **499** peers (`DEF_MAXCLIENTS` 500, loop from 1, `GateCore.cpp:238`). [V]

`WM_USER_TIMERSIGNAL` is `WM_USER + 500` (`Wmain.cpp:26`) and slot 399 maps to the same number (101 + 399);
`WndProc` tests the timer case first, so a peer in slot 399 has no event processed ([NET-008](../issues/network.md#net-008-gate-peer-table-slot-0-unused-and-slot-399-collides-with-the-timer-message)) [V arithmetic].

### 2.3 Framing and reads

Each peer's `XSocket` reads a 3-byte header (key byte, u16 total length incl. header) and then
`length - 3` body bytes into a 30000-byte buffer (`DEF_MSGBUFFERSIZE`, `Client.h:17`; `XSocket.cpp:171-256`). [V]
- A body length (`length - 3`) above 30000, i.e. a u16 length above 30003 (or below 3, which wraps) -> `MSGSIZETOOLARGE` -> the peer is dropped as a lost connection.
- A length of exactly 3 gives a zero-length message that is still delivered; `OnClientRead` then re-reads the stale
  buffer and runs the previous message's handler again ([NET-004](../issues/network.md#net-004-zero-length-frames-re-run-the-previous-message-in-the-gate)) [V code].
- Decryption happens in `pGetRcvDataPointer` only when the key byte is non-zero; the gate never encrypts its own
  output (`XSocket.cpp:421, 562-567`). [V]

### 2.4 Sending

All sends go through `XSocket::iSendMsg`, which prefixes the 3-byte header and either sends at once or queues
(ring of up to 300 unsent buffers per peer, `DEF_XSOCKBLOCKLIMIT`). Until the socket's first `FD_WRITE` everything is
queued (and `iSendMsg` returns -2 for a queued message, but -3 if the ring is full and -4 on allocation failure; those
two match no `DEF_XSOCKEVENT_*` value ([NET-005](../issues/network.md#net-005-gate-full-ring-before-first-write-is-never-detected); `XSocket.cpp:447-453`). [V]
- A "fatal send result" (`MSGSIZETOOLARGE`, `QUENEFULL`, `SOCKETERROR`, `CRITICALERROR`, `SOCKETCLOSED`) makes the
  *gate-initiated* senders drop the peer; `BLOCK` and -2 are ignored. A slow peer is therefore dropped only when its
  300-entry ring is full.
- Two senders ignore every result and never drop a peer: `SendMsgToAllGameServers` and the party replies in
  `PartyOperation` / `PartyManager` ([GATE-003](../issues/gate.md#gate-003-two-senders-never-drop-a-stuck-peer)). [V]

### 2.5 Event dispatch

`OnClientSocketEvent` maps the result of `iOnSocketEvent` to behaviour (`GateCore.cpp:190-228`):
`READCOMPLETE` -> `OnClientRead` (the MsgID switch); `BLOCK` -> log; every error/close code -> if the peer is a game
server: log CRITICAL ERROR, tell monitors (`MSGID_GAMESERVERDOWN`), clean its parties; else log a monitor loss; then
delete the peer. [V]

### 2.6 Timer tick (every 3 s)

`WndProc` -> `OnTimer(0)` runs, in order (`GateCore.cpp:670-686`): `CheckGameServerActivity`,
`SendTotalClientsToGameServer`, `CheckGlobalCommand`, `CheckAutoShutdownProcess`, `PartyManager::CheckMemberActivity`.
The shutdown countdown step (20 s gate) and the party check (2 s gate) each have their own timestamp but are driven by
this tick, so they fire on the first tick after their interval. [V]

## 3. Admission and registration

1. **Allow-list at accept.** After `accept`, the peer's dotted IP text is compared byte-for-byte (16 bytes) with each
   of the 10 `game-server-list` entries (`GateCore.cpp:246-267`). No match -> `HackAttacker.log` line plus immediate close.
   An empty list rejects everyone. [V]
2. **Authentication.** Any listed IP may then send any message in the dispatch table; the gate has no
   session state apart from `m_bIsGameServer`, which only registration (or nothing) sets ([GATE-018](../issues/gate.md#gate-018-no-authentication-beyond-the-ip-allow-list-and-a-fixed-passphrase)). [V]
3. **Registration** (`RegisterGameServerHandler`, `GateCore.cpp:451-537`): body is name[10], address[16], port u16,
   map count u8, 11 bytes per map (ignored), PID u32, build date u32. Outcome table:

| Condition | `m_sIsActivated` | Reply | Log text |
|---|---|---|---|
| build date != cfg `build-date` | -1 | REJECT | "...fail : Server(name:addr:date) build date mismatch." |
| name already held by any slot 1..499, **including the registering peer's own slot** | -2 | REJECT | "...fail : Server(name) already exists." |
| otherwise | 1 | CONFIRM | "...Success( Name | Hd | date )" |

   In all three cases the peer is flagged `m_bIsGameServer = TRUE` (line 494) and keeps its name, so a
   rejected peer remains in every game-server loop (relay, totals, silence check, shutdown; [TOPO-003](../issues/server-topology.md#topo-003-rejected-gate-peers-keep-receiving-traffic)). [V]
   The duplicate scan has no `i != iClientH` test ([GATE-004](../issues/gate.md#gate-004-duplicate-name-scan-counts-the-registering-peers-own-slot)); both reject paths dereference the slot after
   `ResponseRegisterGameServer` (`GateCore.cpp:499-505`, `:512-518`; [GATE-005](../issues/gate.md#gate-005-null-dereference-after-a-failed-registration-reply)). [V]
4. **Heartbeat.** `MSGID_GAMESERVERALIVE` (u16 user count at offset 6) refreshes `m_dwAliveTime`, stores the count,
   raises `m_sIsActivated` to 1 unless it is negative, and is forwarded to monitors (`GateCore.cpp:435-446`). A server
   silent for over 30 s goes to `m_sIsActivated = 0` (and stays connected); the next alive message brings it back [V].
   Rejected servers (-1 / -2) are never promoted.
5. **State summary** (`m_sIsActivated`): 1 active; 0 registered but silent >= 30 s ("not activated"); -1 build-date
   mismatch; -2 duplicate name. F2 lists values <= 0, F3 lists values > 0 (`GateCore.cpp:590-623`). [V]

The gate never uses the registered address or port: it identifies the server by the TCP peer IP and its slot [V].

## 4. Relay (stock messages, totals, monitors)

| Flow | Trigger | Path | Recipients |
|---|---|---|---|
| Stock message bus | `MSGID_SERVERSTOCKMSG` from any peer | `SendMsgToAllGameServers(..., FALSE)`, raw body forwarded unparsed | every `m_bIsGameServer` peer except the sender (`GateCore.cpp:304-307, 1015-1031`) |
| World user total | every tick | sum of `m_iTotalClients` of game servers -> `MSGID_TOTALGAMESERVERCLIENTS` (8 bytes) | every game server (`GateCore.cpp:788-850`) |
| Server status to monitors | alive, loss, silence, shutdown failures | 28-byte `SendMsgToMonitor(a)` | every non-game-server peer (`GateCore.cpp:710-755`) |
| Raw pass-through to monitors | `MSGID_REQUEST_ALL_SERVER_REBOOT`, `_EXEC_1DOTBAT`, `_EXEC_2DOTBAT`, `MSGID_GATEWAY_CURRENTCONN` | `SendMsgToMonitor(b)` | every non-game-server peer (`GateCore.cpp:390-407, 757-785`) |
| Monitor heartbeat | `MSGID_MONITORALIVE` | toggle `m_cMonitorStatus[MsgType]`, echo `"Total User n"` to all monitors | every non-game-server peer (`GateCore.cpp:409-423`) |
| Item log | `MSGID_ITEMLOG` | `ItemEvents.log` + window list | none (local) |

Observations:
- The bus is a blind fan-out: the gate never reads the GSM sub-type, so any new GSM record in the 2.20 game server
  (0x0D-0x12) passes unchanged. [V: no GSM constant is referenced in `GateCore.cpp`.]
- A monitor is "whoever is not a game server", including an unregistered game-server connection in the moment between
  accept and register, and a Log server that connects as a peer. [V]
- Monitor sends return on the first failing monitor after deleting it ([GATE-011](../issues/gate.md#gate-011-sendmsgtomonitor-aborts-at-the-first-failing-monitor)) [V].
- `iTotalWorldServerClients` is recomputed every tick and also shown to monitors only inside the `MONITORALIVE` echo. [V]

## 5. Parties

Purpose: the only gameplay state the gate keeps. Memory only; the tables (`m_iMemberNumList[5000]`,
`m_stMemberNameList[5000]`, `PartyManager.h:38-44`) are lost on restart. [V]

- **Id space:** party ids 1..4999; the 4999 member slots are one shared pool for all parties, 8 members per party at most
  (`DEF_MAXPARTYMEMBER`). [V]
- **Operations** (request types of `MSGID_PARTYOPERATION`, `GateCore.cpp:1147-1272`): 1 create, 2 (no-op), 3 add, 4 remove,
  5 check member, 6 get info, 7 server-change stamp. Replies use codes 1 create, 2 disbanded, 3 not-a-member, 4 add,
  5 info, 6 remove (the code is the first u16 after the MsgID). Create/info/check replies go to the requester; add, remove,
  disband and timeout notices are broadcast to **all** game servers, requester included. [V]
- **Server crossing:** the game server sends type 7 before moving a player; the gate stamps the member. Type 5 from the
  new server clears the stamp. A stamp older than 20 s makes the gate broadcast "removed" (code 6) and remove the member,
  and removing down to one member deletes the party (code 2 broadcast). [V]
- **Cleanup:** `GameServerDown` drops members registered by a dead server, only when the loss arrives as a socket event
  (see section 7). [V]
- **Current behaviour vs intent** (details in `functions.md` section 2): `m_iIndex` is the server that created/added the
  member and is not updated on a crossing ([GATE-017](../issues/gate.md#gate-017-member-server-index-is-never-updated-on-a-crossing)); `iPartyID` from the wire indexes the tables unchecked ([GATE-013](../issues/gate.md#gate-013-party-id-from-the-wire-is-an-unchecked-array-index));
  `bAddMember` accepts unknown ids and its duplicate-name step adjusts the target party instead of the party that held the
  name (`PartyManager.cpp:130-131`, [GATE-014](../issues/gate.md#gate-014-baddmember-decrements-the-wrong-party)); the master is not recorded; `GameServerDown` can leave 0-1 member parties
  without notice ([GATE-016](../issues/gate.md#gate-016-gameserverdown-leaves-small-parties-and-is-skipped-on-most-deletes)). [V code; I effects]

## 6. Shutdown and monitor control

Four independent paths exist (`GateCore.cpp`):

| Path | Trigger | Effect |
|---|---|---|
| Immediate, all | F1 + F12 held at a tick (864-909) | `MSGID_GAMESERVERSHUTDOWNED` to every game server, repeated every tick while held |
| Immediate, one | `MSGID_REQUEST_GAMESERVER_SHUTDOWN`, MsgType 0x5A8E + passphrase + server name (308-363) | `MSGID_GAMESERVERSHUTDOWNED` to servers of that name |
| Timed, all | F1+F4 (626-640) or `MSGID_REQUEST_ALLGAMESERVER_SHUTDOWN` (365-386) | flag servers, then `CheckAutoShutdownProcess` (1033-1106): notice 1 x7, notice 2 x1, then SHUTDOWNED, one step per about 21 s, about 189 s total |
| Notice only | F1+F6 / F1+F7 (648-662) | `MSGID_SENDSERVERSHUTDOWNMSG` code 1 / 2 to every game server, no flag, no shutdown |

- Authentication of the network requests is a fixed MsgType `0x5A8E` plus (for the two shutdown requests only) the literal
  `1dkld$#@01` at offset 6. A wrong MsgType deletes the sender; a wrong passphrase is ignored. REBOOT / EXEC_1 / EXEC_2
  check the MsgType only. All of these ride behind the IP allow-list. [V]
- The gate does not run anything itself: reboot and batch-file requests are *forwarded to monitors*, which are
  expected to do the work (`GateCore.cpp:401`). [V]
- Monitor display: `m_cMonitorStatus[0..9]` toggles 1 <-> 2 on each `MONITORALIVE` with that MsgType; painted as `X` (1)
  and `O` (2), `?` before the first beat; the window repaints on each heartbeat (`GateCore.cpp:413-416`,
  `Wmain.cpp:302-311`). The gate never clears these, so a dead monitor leaves its last glyph. [V]
- The network-started countdown does not reset the count or clock, so once the count has hit 0 a later network-started
  countdown never reaches the SHUTDOWNED step ([GATE-012](../issues/gate.md#gate-012-shutdown-countdown-is-not-re-armed-by-a-network-request); `functions.md` 1.6). [V code]

## 7. Failure and cleanup matrix

Which events delete a peer and what else they do. "GSD" = `PartyManager::GameServerDown`; "Mon" = tell monitors
`MSGID_GAMESERVERDOWN`.

| Event | Where | Peer deleted | Log | Mon | GSD |
|---|---|---|---|---|---|
| socket close / read error / oversize message | `OnClientSocketEvent` 207-227 | yes | yes | game servers only | game servers only |
| fatal result sending to a game server (totals, notice, shutdown, registration reply) | 558-575, 831-849, 951-969, 888-906, 1084-1102 | yes | yes | yes | **no** |
| fatal result sending to a monitor | 738-751, 768-781 | yes | yes | no | no |
| wrong MsgType on shutdown / reboot requests | 311-316 etc. | yes (the sender) | no | no | no |
| allow-list failure | 258-267 | yes (new peer) | `HackAttacker.log` | no | no |
| table full | 276-282 | new connection closed | no | no | no |
| 30 s silence | 688-708 | **no** (flag only) | yes | yes (every tick) | no |

The `GSD` column is the only place party members of a lost server are cleaned (`GateCore.cpp:221`); all fatal-send deletions
skip it [V]. Deletions during a loop in `SendMsgToMonitor`, `CheckGlobalCommand` and `CheckAutoShutdownProcess` end the
function early (the `return` after the delete) [V].

## 8. UI

The window is the operator console: `620 x 700` popup, title `Helbreath GateServer 2002-6-20`, 50-line log ring, three
static help lines, and the 10-slot monitor strip; the full look is in `../servers/ui.md` section 2. The behaviour
added by this analysis:

- Painting is lazy: `PutLogList` sets a flag, `EventLoop` calls `UpdateScreen` after each dispatched message, which
  invalidates the window (`Wmain.cpp:191-207, 270-276`). The monitor heartbeat invalidates directly (`GateCore.cpp:416`). [V]
- Key events use `WM_KEYDOWN` / `WM_KEYUP` on the main window, so the window needs focus. `F2`, `F3`, `F4`, `F6`, `F7`
  act on **release**; F1 and F12 are tracked as flags on press and release, and the F1+F12 check is made by the timer, not
  by the key event (`GateCore.cpp:579-668, 852-862, 874`). [V]
- The on-screen help reads "[F1 + F4] Shut down all game servers"; that key pair starts the timed countdown
  (`Wmain.cpp:296`, [GATE-020](../issues/gate.md#gate-020-help-text-advertises-f1f4-as-an-immediate-shutdown)). [V]
- Log lines also go to the window only; the only persistent log of game events is `ItemEvents.log` and the three files
  in section 9; a connection-loss line is not written to a file ([GATE-021](../issues/gate.md#gate-021-connection-loss-lines-never-reach-a-file)). [V]

## 9. Config and files

`GateServer.cfg` is read once at start by `bReadProgramConfigFile` (`GateCore.cpp:73-179`); there is no reload. Keys and
samples: `../servers/gate-server.md` section 2. Additional facts from this pass:

- Separators are `=`, space, comma, tab, newline (`GateCore.cpp:79`); a leading `[CONFIG]` token is ignored; `\r` is removed by
  text-mode `fopen`. [V]
- Keys are matched by prefix and each applies to the *next token only*; the allow-list needs repeated
  `game-server-list = <ip>` lines, at most 10 (no bound check in the loader, [GATE-008](../issues/gate.md#gate-008-allow-list-loader-has-no-bound)). [V]
- The shipped cfg (`git show 0b994cd:GateServer.cfg`) has address `203.234.215.188`, port 6502, `build-date = 612`, and no
  `game-server-list`. [V]
- Files written, all append: `ItemEvents.log`, `HackAttacker.log` (in the working folder),
  `GameServerLog\GameServerLogYYYYMMDD.log` (folder created on demand, `Wmain.cpp:384`). No file is read except the cfg.
  [V]
- Projects: `GateSever.dsp` (VC6) and `.vcproj` list `Client`, `GateCore`, `PartyManager`, `Wmain`, `XSocket` and link
  `Winmm.lib`, `Ws2_32.lib`, `WSOCK32.LIB`; `StrTok.*` and `ActionID.h` are not part of the build. [V]

## 10. Corrections to `../servers/gate-server.md` and `../servers/game-gate-protocol.md`

Found while mapping the source. Round 2 propagated the rows on the comma separator, allow-list and party limits, socket capacity, passphrase scope, F6 fall-through and the `GameServerLog` folder into `../servers/gate-server.md`; the other rows are not yet propagated.

| Older claim | Source says | Cite |
|---|---|---|
| gate-server 6: "The `GameServerLog` folder must already exist; `fopen` does not create it" | `_mkdir("GameServerLog")` runs before every write | `Wmain.cpp:384` |
| gate-server 3: a peer is a game server "once its registration is accepted" (cite 710-786) | the flag is set before the build-date and name checks, so rejected servers are game servers too; 710-786 is `SendMsgToMonitor` | `GateCore.cpp:494` |
| game-gate-protocol 2: stock messages go "to every other registered game server" | to every other peer with the game-server flag, including rejected ones | `GateCore.cpp:1026-1029` |
| gate-server 4: "up to 5000 parties of 8" | ids 1..4999 and one shared pool of 4999 member slots, 8 per party at most | `PartyManager.h:19-20, 38-44` |
| gate-server 3: "Capacity: 500 sockets" | 499 usable (slot 0 unused) and slot 399 collides with the timer message | `GateCore.cpp:238`, `Wmain.cpp:26` |
| gate-server 2: tokenised on "=, spaces, tabs and newlines" | commas also separate | `GateCore.cpp:79` |
| gate-server 2/3: allow-list "up to 10" | no bound; the 11th entry overflows | `GateCore.cpp:125-126` |
| gate-server 4: user-count log "every 60 s" | every 61 s (`> 60`) | `Wmain.cpp:54` |
| gate-server 4/5: shutdown "about 3 minutes" | right for F1+F4 (nine steps, about 189 s); a network-started shutdown does not reset the count or clock, so a repeat never reaches the final step | `GateCore.cpp:1055-1070, 365-386` |
| gate-server 4: shutdown requests "authenticated by a fixed MsgType and a fixed 10-byte passphrase" | only the two shutdown requests check the passphrase; reboot / batch requests check the MsgType alone; a bad MsgType deletes the sender | `GateCore.cpp:311-319, 368-377, 395-400` |
| gate-server 5: F2/F3/F4/F6/F7 shown as plain key chords | they act on key release, not press | `GateCore.cpp:579-668` |
| gate-server 5: F6 falls through into F7 "harmless" | it falls through F7 into F12 and clears the F12 flag | `GateCore.cpp:648-666` |
| gate-server 7: only `MONITORALIVE` indexes unchecked | also unchecked: `strcpy` of the shutdown-one name into a 22-byte buffer, party id used as array index, allow-list loader | `GateCore.cpp:323`, `PartyManager.cpp:119, 130` |
| gate-server 1: listener at `Wmain.cpp:235` | right, but a failed bind there is not detected, and the gate logs "Listening" regardless | `Wmain.cpp:235-240` |

Unchanged and confirmed: the build-date and duplicate-name registration rules, map list skipped, 30 s silence with no
disconnect, 20 s party crossing window, relay of `MSGID_SERVERSTOCKMSG` unparsed, the three log files and their stamp format,
the fixed cfg keys, and the verdict on hypotheses H1 and H2.

# Gate server 2.19: function map

Every function and method of the official Gate server, with purpose, callers, callees and the messages it
handles or sends. Behaviour and configuration in prose: `../servers/gate-server.md`; structure: `subsystems.md`,
`data-structures.md`; message layouts: `../servers/game-gate-protocol.md` and the shared catalogue in
`../shared/messages/` (this doc names messages by their MSGID).

## Conventions

- Source: `GATE` = `sources/gateserver-librehelbreath` at commit `0b994cd` (the original; HEAD `a2b7f1d` only
  reformats). **All line numbers are `0b994cd` lines.** Read with `git -C <repo> show 0b994cd:<file>`.
- Tags: [V] verified by reading the cited lines, [I] inferred (reasoning from verified code, stated), [U] unverified.
- `PutLogList` (Wmain.cpp:258) is called from almost everywhere; it is left out of callee lists unless it is the
  point. `iSendMsg` results: the send-failure group (MSGSIZETOOLARGE, QUENEFULL, SOCKETERROR, CRITICALERROR,
  SOCKETCLOSED) is called "fatal send result" below; `DEF_XSOCKEVENT_BLOCK` and the -2 "queued" result are
  never treated as errors by any caller [V]. Before the first FD_WRITE `iSendMsg` returns -2 (queued), -3 (ring
  full) or -4 (alloc failure), none of which matches the fatal group ([NET-005](../issues/network.md#net-005-gate-full-ring-before-first-write-is-never-detected); section 5 `iSendMsg`) [V].
- Layout offsets are body offsets (0 = MsgID u32, 4 = MsgType u16, 6 = payload), as in `game-gate-protocol.md`.
- Every handler reads fixed offsets from the received body without checking its length (`dwMsgSize` is used
  only for forwarding; [GATE-002](../issues/gate.md#gate-002-handlers-never-check-the-body-length)) [V: `GATE GateCore.cpp:285-449`].

## Count

78 function definitions in 7 files: 74 are built, 4 (`CStrTok`) are not in the project. [V]

| File | Functions |
|---|---|
| `GateCore.cpp` (CGateCore) | 24 (incl. ctor, dtor and two `SendMsgToMonitor` overloads) |
| `Client.cpp` (CClient) | 2 |
| `PartyManager.cpp` | 11 (incl. ctor, dtor) |
| `Wmain.cpp` | 18 |
| `XSocket.cpp` | 19 (incl. ctor, dtor, two free functions) |
| `StrTok.cpp` (CStrTok) | 4, **not built** (absent from `GateSever.dsp` and `.vcproj`; no caller) [V] |
| `GateCore.h`, `Client.h`, `PartyManager.h`, `XSocket.h` | declarations only; no inline bodies |

`WINMAIN.H:10` declares a global `void OnKeyUp(WPARAM, LPARAM)` that is defined nowhere and called nowhere [V].
`XSocket::bConnect` and `XSocket::iGetSocket` are defined but never called by the gate (only `bConnect` calls
itself, `XSocket.cpp:89`) [V by grep].

## 1. CGateCore (`GateCore.cpp`, `GateCore.h`)

### 1.1 Lifecycle and configuration

| Function | Line | Purpose | Callers -> callees | Messages |
|---|---|---|---|---|
| `CGateCore::CGateCore(HWND)` | 19-42 | Zero client table and monitor status, F1/F12 flags FALSE, `m_iAutoShutdownCount = 10`, build date 0, `iTotalWorldServerClients = 0`, `m_cAddress` zeroed, allocate `PartyManager`. Does **not** initialise `m_iGateServerPort`, `m_dwAutoShutdownTime`, `m_bIsAutoShutdownProcess` [V]. | `Initialize` (Wmain.cpp:220) -> `PartyManager::PartyManager` | - |
| `CGateCore::~CGateCore` | 45-60 | Set `G_bThreadFlag = FALSE`, delete every `CClient`, delete the party manager. | `OnDestroy` (Wmain.cpp:247) -> `~CClient`, `~PartyManager` | - |
| `bInit` | 63-71 | Read `GateServer.cfg`, then start the 60 s logging thread. The thread starts only if the config opened [V]. | `Initialize` (Wmain.cpp:228) -> `bReadProgramConfigFile`, `_beginthreadex(ThreadProc)` | - |
| `bReadProgramConfigFile(char*)` | 73-179 | Parse the cfg (tokeniser `strtok`, separators `= ,\t\n` - comma included). Keys: `gate-server-port` (mode 1), `build-date` (2), `game-server-list` (3, repeatable), `gate-server-address` (4); matched by `memcmp` prefix of 15/10/16/19 bytes (141-145). Returns FALSE only if the file cannot be opened (90-95). Dead fallback 157-175 (array compared to NULL, never true). | `bInit` -> `CreateFile`/`GetFileSize`, `fopen`, `PutLogList` | - |

Notes:
- `m_iGateServerPort` is uninitialised if the key is missing; `m_cGateServerAddr` is zeroed (88), so `inet_addr("")`
  fails at `bListen` [V]. The failure is invisible (see `Initialize` in section 4).
- `game-server-list` entries are stored with `strcpy(m_cAddress[iIPindex], token)` and `iIPindex++` with **no bound**
  (125-126; the loader has no bound, [GATE-008](../issues/gate.md#gate-008-allow-list-loader-has-no-bound)) [V].
  Which fields follow (`m_iBuildDate`, `iTotalWorldServerClients`, `m_cMonitorStatus`) is by declaration order
  (`GateCore.h:71-76`) [I: depends on compiler layout]. Order of keys in the cfg then matters.
- Each key is a one-shot: `cReadMode` is reset after the value token. A key whose value token is absent swallows the
  next key name as its value [V, 105-137].
- `delete cp` on a `new char[]` (151) [V], harmless on MSVC.

### 1.2 Socket events, accept and dispatch

| Function | Line | Purpose | Callers -> callees | Messages |
|---|---|---|---|---|
| `OnClientSocketEvent(UINT, WPARAM, LPARAM)` | 181-229 | Map window message to slot (`message - WM_ONCLIENTSOCKETEVENT`), ignore empty slot, call `XSocket::iOnSocketEvent`, act on the result: READCOMPLETE -> `OnClientRead`; BLOCK -> log "Socket BLOCKED!"; CONFIRMCODENOTMATCH -> delete peer (never returned by this XSocket); CRITICALERROR / QUENEFULL / MSGSIZETOOLARGE / SOCKETERROR / SOCKETCLOSED -> if game server: log, `SendMsgToMonitor(GAMESERVERDOWN)`, `PartyManager::GameServerDown`; else log "Monitor(n) connection lost"; delete the peer. All other results (ONREAD, MISMATCH, UNSENT*) are ignored [V]. | `WndProc` default branch (Wmain.cpp:111-112) -> `iOnSocketEvent`, `OnClientRead`, `SendMsgToMonitor(a)`, `GameServerDown` | sends MSGID_GAMESERVERDOWN (to monitors) |
| `bAccept(XSocket*)` | 231-283 | Take the first free slot from 1 to 499, create a `CClient`, accept the TCP connection into it with message `WM_ONCLIENTSOCKETEVENT + i`, read the peer IP, compare 16 bytes with each of the 10 allow-list entries; no match -> `PutLogFileList` ("<slot> <ip> ..." in Korean), delete the client, return FALSE. Match -> log "Accept! Add New Client", set `m_dwAliveTime = timeGetTime()`. Table full -> accept into a temporary XSocket and delete it (closes the peer, 277-279). | `OnAccept` (Wmain.cpp:319) -> `new CClient`, `XSocket::bAccept`, `bInitBufferSize`, `iGetPeerAddress`, `PutLogFileList` | - |
| `OnClientRead(int)` | 285-449 | Fetch the received body (`pGetRcvDataPointer`), switch on the MsgID at offset 0. No `default`: unknown IDs are dropped silently. Cases below. | `OnClientSocketEvent` (194) -> see cases | all inbound messages |

Notes on `bAccept`:
- Slot 0 is never used (loop starts at 1), so **499 peers at most** [V]. The accepted socket is *always* accepted
  first and dropped afterwards, so an unlisted peer gets a connect then an immediate close [V].
- Slot 399 uses message `WM_USER + 101 + 399 = WM_USER + 500`, which is `WM_USER_TIMERSIGNAL` (`Wmain.cpp:26`,
  `UserMessages.h:3`). `WndProc` matches the timer `case` first (`Wmain.cpp:80-82`), so every socket event of a peer in
  slot 399 is swallowed ([NET-008](../issues/network.md#net-008-gate-peer-table-slot-0-unused-and-slot-399-collides-with-the-timer-message)) [V arithmetic].
- `bInitBufferSize` is called a second time (244) after the constructor already did (Client.cpp:15): it frees and
  reallocates both 30008-byte buffers [V].
- `getpeername` failure is not checked (246); the string is `inet_ntoa` text [V].

`OnClientRead` cases (all `GateCore.cpp`):

| MsgID | Lines | Authentication | Action |
|---|---|---|---|
| MSGID_PARTYOPERATION | 300-302 | none | `PartyOperation` |
| MSGID_SERVERSTOCKMSG | 304-307 | none | `SendMsgToAllGameServers(iClientH, pData, size, FALSE)`: relay to all game servers except the sender |
| MSGID_REQUEST_GAMESERVER_SHUTDOWN | 308-363 | MsgType must be 0x5A8E (else the **sender is deleted**, 311-316); then 10 bytes `1dkld$#@01` at offset 6 (else silently ignored, 318-319) | Name = NUL-terminated string at offset 16, copied with `strcpy` into a 22-byte buffer (323; [GATE-010](../issues/gate.md#gate-010-shutdown-one-handler-unbounded-strcpy-and-delete-in-loop)) [V]. Send MSGID_GAMESERVERSHUTDOWNED (6 bytes) to every game server with that name (325-335); on a fatal send result delete it and send MSGID_GAMESERVERDOWN to monitors. If no name matched, log "CAN'T FIND". Does not use the shutdown flag or countdown. |
| MSGID_REQUEST_ALLGAMESERVER_SHUTDOWN | 365-386 | same two checks | Log "AUTO SHUTDOWN PROCESS STARTED!", set `m_bIsShutDown = TRUE` on every game server. Does **not** reset `m_iAutoShutdownCount` or set `m_dwAutoShutdownTime` (contrast F1+F4, 630-631); the tick in `CheckAutoShutdownProcess` does the rest. |
| MSGID_REQUEST_ALL_SERVER_REBOOT, MSGID_REQUEST_EXEC_1DOTBAT, MSGID_REQUEST_EXEC_2DOTBAT | 390-403 | MsgType 0x5A8E only (no passphrase; wrong -> sender deleted) | `SendMsgToMonitor(pData, size)`: forward the raw message to all monitors |
| MSGID_GATEWAY_CURRENTCONN | 405-407 | none | forward raw to all monitors |
| MSGID_MONITORALIVE | 409-423 | none | `m_cMonitorStatus[MsgType]` toggles 1 <-> 2 (413-415); `InvalidateRect`; reply to **all monitors** with the same MsgID, same MsgType and name field `"Total User n"` (n = `iTotalWorldServerClients`). MsgType is a u16 used as an index into a 10-byte array ([GATE-009](../issues/gate.md#gate-009-monitoralive-msgtype-is-an-unchecked-array-index)) [V]. |
| MSGID_ITEMLOG | 425-428 | none | `ItemLog` |
| MSGID_REQUEST_REGISTERGAMESERVER | 430-433 | none (allow-list at accept time only) | `RegisterGameServerHandler` |
| MSGID_GAMESERVERALIVE | 435-446 | none | `m_dwAliveTime = now`; `m_iTotalClients = u16 at offset 6`; if `m_sIsActivated >= 0` set it to 1; forward to monitors as MSGID_GAMESERVERALIVE / CONFIRM with name and count |

Notes:
- Any accepted peer may send any of these, including GAMESERVERALIVE, ITEMLOG, PARTYOPERATION, before registering;
  there is no check on `m_bIsGameServer` in `OnClientRead` [V].
- The alive handler updates `m_iTotalClients` even for rejected servers (`m_sIsActivated` -1/-2), which then still
  count in the world total [V; see `RegisterGameServerHandler`].
- The shutdown-one handler loops while deleting entries and continues after the delete (344-355), and reuses `cTemp`
  (the name) for the error text via `memcpy(cTemp, m_cName, 20)` ([GATE-010](../issues/gate.md#gate-010-shutdown-one-handler-unbounded-strcpy-and-delete-in-loop)) [V].
- A body of exactly 3 header bytes (length field 3) reaches here as a zero-length message: `pGetRcvDataPointer` returns the
  stale previous body and the switch runs on its old MsgID again ([NET-004](../issues/network.md#net-004-zero-length-frames-re-run-the-previous-message-in-the-gate); see `XSocket::_iOnRead`) [V code].

### 1.3 Registration

| Function | Line | Purpose | Callers -> callees | Messages |
|---|---|---|---|---|
| `RegisterGameServerHandler(int, char*)` | 451-537 | Parse name[10] @6, address[16] @16, port u16 @32, map count (signed char) @34, skip `11*M` bytes of maps, read PID u32 and build date u32. Store PID, **set `m_bIsGameServer = TRUE` (494) before any check**. Build date != `m_iBuildDate` -> `m_sIsActivated = -1`, store name, reject. Name already used by any slot 1..499 (509-520; the scan has no `i != iClientH` test, so the registering peer's own slot counts; [GATE-004](../issues/gate.md#gate-004-duplicate-name-scan-counts-the-registering-peers-own-slot)) -> `-2`, store name, reject. Both reject paths call `ResponseRegisterGameServer` first and then dereference `m_pClientList[iClientH]` (`GateCore.cpp:499` then `:502-505`, and `:512` then `:515-518`, numbered as `git show 0b994cd`; [GATE-005](../issues/gate.md#gate-005-null-dereference-after-a-failed-registration-reply)). Else `m_sIsActivated = 1`, store name, log success, accept. Lines 534-536 are unreachable ([GATE-006](../issues/gate.md#gate-006-registration-parser-signed-map-count-and-unreachable-tail)). | `OnClientRead` (432) -> `ResponseRegisterGameServer` | in MSGID_REQUEST_REGISTERGAMESERVER |
| `ResponseRegisterGameServer(int, BOOL)` | 539-577 | Send the 6-byte reply, MsgType CONFIRM or REJECT; on a fatal send result delete the peer, set `m_pClientList[iClientH] = NULL` and send MSGID_GAMESERVERDOWN to monitors. Both reject callers then dereference the (possibly NULL) slot, see `RegisterGameServerHandler`. | `RegisterGameServerHandler` -> `iSendMsg`, `SendMsgToMonitor(a)` | out MSGID_RESPONSE_REGISTERGAMESERVER |

Notes:
- A server rejected for build date or name still has `m_bIsGameServer == TRUE`: it is skipped by monitor broadcasts,
  included in the 30 s silence check, the world total, the stock-message relay and the shutdown flags [V from the
  loops in 1.4-1.6]. `../servers/gate-server.md` section 3 says a peer becomes a game server "once its registration is accepted"; that is wrong.
- Rejected peers keep the name (504, 517), so a later correct server of the same name fails with "already exists"
  until the first disconnects ([GATE-007](../issues/gate.md#gate-007-a-rejected-servers-name-blocks-a-later-correct-server)) [V code].
- Port and address from the body are parsed but unused (`iPort` and `cAddress` appear only in the log text); the
  `CClient::m_cAddress` that is used is the peer IP from `bAccept` [V].
- `iTotalMaps = (int)*cp` is a signed char (479-481; [GATE-006](../issues/gate.md#gate-006-registration-parser-signed-map-count-and-unreachable-tail)) [V code].

### 1.4 Timers and liveness

| Function | Line | Purpose | Callers -> callees | Messages |
|---|---|---|---|---|
| `OnTimer(int)` | 670-686 | Id 0 only. Per 3 s tick in order: `CheckGameServerActivity`, `SendTotalClientsToGameServer`, `CheckGlobalCommand`, `CheckAutoShutdownProcess`, `PartyManager::CheckMemberActivity`. | `WndProc` (Wmain.cpp:81) | - |
| `CheckGameServerActivity` | 688-708 | For every slot with `m_bIsGameServer` and `now - m_dwAliveTime > 30000` (`DEF_NORESPONSELIMIT`): log "does not response", set `m_sIsActivated` 1 -> 0, send MSGID_GAMESERVERDOWN to monitors. Does not disconnect. It repeats each tick for as long as the server stays silent [V]. | `OnTimer` -> `SendMsgToMonitor(a)` | out MSGID_GAMESERVERDOWN |
| `SendTotalClientsToGameServer` | 788-806 | Sum `m_iTotalClients` of all game servers (loop starts at 0) into `iTotalWorldServerClients`, then send it to each. | `OnTimer` -> `_SendTotalGameServerClients` | - |
| `_SendTotalGameServerClients(int, int)` | 808-850 | 8-byte MSGID_TOTALGAMESERVERCLIENTS, MsgType CONFIRM, u16 total at offset 6. Fatal send result: delete the peer, `SendMsgToMonitor(GAMESERVERDOWN)`, **return** (the caller keeps iterating, the one failed server only). | `SendTotalClientsToGameServer` | out MSGID_TOTALGAMESERVERCLIENTS |

### 1.5 Monitor and relay senders

| Function | Line | Purpose | Callers -> callees | Messages |
|---|---|---|---|---|
| `SendMsgToMonitor(DWORD, WORD, char*, short)` | 710-755 | Build a 28-byte message: MsgID, MsgType, 20 bytes copied from `pGameServerName`, i16 count at offset 26; send to every peer with `m_bIsGameServer == FALSE`. A fatal send result deletes that monitor and returns ([GATE-011](../issues/gate.md#gate-011-sendmsgtomonitor-aborts-at-the-first-failing-monitor)) [V]. | `OnClientSocketEvent` (220), `OnClientRead` (352, 422, 445), `ResponseRegisterGameServer` (574), `CheckGameServerActivity` (705), `_SendTotalGameServerClients` (847), `CheckGlobalCommand` (904), `_SendServerShutDownMsg` (967), `CheckAutoShutdownProcess` (1099) -> `iSendMsg` | out MSGID_GAMESERVERDOWN, MSGID_GAMESERVERALIVE, echo of MSGID_MONITORALIVE |
| `SendMsgToMonitor(char*, DWORD)` | 757-785 | Forward a raw body to every non-game-server peer; same delete-and-return on failure. | `OnClientRead` (402, 406) -> `iSendMsg` | forwards REBOOT / EXEC_1 / EXEC_2 / GATEWAY_CURRENTCONN |
| `SendMsgToAllGameServers(int, char*, DWORD, BOOL)` | 1015-1031 | `bIsOwnSend == TRUE`: send to every game server; FALSE: skip slot `iClientH`. "Game server" = `m_bIsGameServer`, whether activated or not. Send results are ignored ([GATE-003](../issues/gate.md#gate-003-two-senders-never-drop-a-stuck-peer)) [V]. | `OnClientRead` (306, FALSE), `PartyOperation` (1204, 1241, TRUE), `PartyManager::bDeleteParty` (105, TRUE), `PartyManager::CheckMemberActivity` (312, TRUE) -> `iSendMsg` | relays MSGID_SERVERSTOCKMSG, broadcasts MSGID_PARTYOPERATION |

Notes on the 28-byte monitor message:
- `memcpy(cp, pGameServerName, 20)` reads 20 bytes although `CClient::m_cName` is 11; the next 9 bytes are the start of the
  adjacent `m_cAddress` (declaration order, `Client.h:27-28`), so the "name" field also carries part of the peer IP
  for GAMESERVERALIVE [I: contiguous char arrays]. For GAMESERVERDOWN sent from the error paths the name was first
  copied with `memcpy(cTempName, m_cName, 20)` (e.g. 838), same effect [I].
- Monitor identity: every peer that has not sent a registration (or has a game-server flag FALSE) is a monitor, including a
  peer that connected a moment ago and has not registered yet [V].

### 1.6 Keyboard, shutdown and logging

| Function | Line | Purpose | Callers -> callees | Messages |
|---|---|---|---|---|
| `OnKeyDown(WPARAM, LPARAM)` | 852-862 | Set `m_bF1pressed` / `m_bF12pressed` on F1 / F12 press. | `WndProc` (Wmain.cpp:89) | - |
| `OnKeyUp(WPARAM, LPARAM)` | 579-668 | **Acts on key release.** F1 clears the flag. F2: list game servers with `m_sIsActivated <= 0` (reasons 0 / -1 / -2). F3: list those with `> 0`. F4 with F1 held: `m_iAutoShutdownCount = 10`, `m_dwAutoShutdownTime = now`, flag every game server `m_bIsShutDown`, log. F6 / F7 with F1 held: `SendServerShutDownMsg(1 or 2, TRUE)`. F6 without F1 falls through into F7 and then F12 (no `break`): releasing F6 or F7 without F1 clears `m_bF12pressed` [V, 648-666]. F12 clears its flag. | `WndProc` (Wmain.cpp:93) -> `SendServerShutDownMsg` | - |
| `CheckGlobalCommand` | 864-909 | If F1 and F12 are both held at the tick: log and send MSGID_GAMESERVERSHUTDOWNED (6 bytes) to every game server (loop from 0). A fatal send result deletes that server, notifies monitors and **returns**, abandoning the rest this tick. Repeats every 3 s while the keys stay down. | `OnTimer` -> `iSendMsg`, `SendMsgToMonitor(a)` | out MSGID_GAMESERVERSHUTDOWNED |
| `SendServerShutDownMsg(char, BOOL)` | 911-925 | Send the shutdown notice with code `cCode` to every game server that has `m_bIsShutDown` or (`bISShutdown` TRUE). | `OnKeyUp` (F6, F7: bISShutdown TRUE), `CheckAutoShutdownProcess` (1060, 1066) -> `_SendServerShutDownMsg` | - |
| `_SendServerShutDownMsg(int, char)` | 927-970 | 8-byte MSGID_SENDSERVERSHUTDOWNMSG, MsgType CONFIRM, u16 code (1 or 2) at offset 6. Fatal send result: delete, notify monitors, return. | `SendServerShutDownMsg` -> `iSendMsg`, `SendMsgToMonitor(a)` | out MSGID_SENDSERVERSHUTDOWNMSG |
| `CheckAutoShutdownProcess` | 1033-1106 | Sets `m_bIsAutoShutdownProcess` if any game server has `m_bIsShutDown`; returns if none. At most one step per 20 s (`now - m_dwAutoShutdownTime > 20000`, so about 21 s at a 3 s tick). Each step decrements the count (floor 0) and switches: count >= 3 or 0: `SendServerShutDownMsg(1)`; 2: "LAST SHUTDOWN MESSAGE", `SendServerShutDownMsg(2)`; 1: send MSGID_GAMESERVERSHUTDOWNED to each flagged server (a fatal send result deletes the server and returns). | `OnTimer` -> `SendServerShutDownMsg`, `iSendMsg`, `SendMsgToMonitor(a)` | out MSGID_SENDSERVERSHUTDOWNMSG, MSGID_GAMESERVERSHUTDOWNED |

Notes (all [V] from 1033-1106):
- From a count of 10 the sequence is notice 1 at counts 9..3 (7 notices), notice 2 at count 2, SHUTDOWNED at count 1:
  nine steps, about 189 s (about 3 min) [derived from the 20 s gate and the 3 s tick].
- The count is not reset when the countdown ends, and a flagged server is only unflagged by being deleted. If the
  servers stay connected after count 1, the count sits at 0 and the `default` branch re-sends notice 1 every 20 s.
- A second shutdown started by MSGID_REQUEST_ALLGAMESERVER_SHUTDOWN (which does not reset the count) therefore
  never reaches case 1 again; it only repeats notice 1. Only F1+F4 resets the count to 10 ([GATE-012](../issues/gate.md#gate-012-shutdown-countdown-is-not-re-armed-by-a-network-request)) [V].
- On a monitor-started first shutdown `m_dwAutoShutdownTime` is not assigned (`new` object, ctor does not set it)
  ([GATE-012](../issues/gate.md#gate-012-shutdown-countdown-is-not-re-armed-by-a-network-request)) [I].
- The shutdown notice is the only gate output that a game server turns into a user-visible message; the gate sends
  codes 1 and 2 only.

### 1.7 Item log and party operation

| Function | Line | Purpose | Callers -> callees | Messages |
|---|---|---|---|---|
| `ItemLog(char*)` | 972-1011 | Parse give-name[10] @6, receive-name[10] @16, item-name[20] @26, item type (signed char) @46, three i16 values @47/49/51 (body length 53). Format "Item(name type v1 v2 v3) From(a)-To(b)", append to `ItemEvents.log` and show in the window list. | `OnClientRead` (427) -> `PutItemLogFileList`, `PutLogList` | in MSGID_ITEMLOG |
| `PartyOperation(int, char*)` | 1110-1273 | Parse request: type u16 @4, game-server character handle (`iGSCH`) u16 @6, name[10] @8, party id u16 @18 (body 20 bytes). Switch on the request type; replies are built in a zeroed `cData[120]`, MsgID MSGID_PARTYOPERATION. See the table below. | `OnClientRead` (301) -> `PartyManager::*`, `SendMsgToAllGameServers`, `iSendMsg` | in/out MSGID_PARTYOPERATION |

Request-to-reply map in `PartyOperation` (reply sizes are the byte counts passed to `iSendMsg`; the data actually
written is shorter, see `data-structures.md`):

| Req | Lines | Calls | Reply |
|---|---|---|---|
| 1 create | 1148-1182 | `iCreateNewParty` | to the requester only, code 1, result byte 0/1, `iGSCH`, name, party id (0 on failure); 22 bytes |
| 2 disband | 1184-1185 | nothing | none (empty case) |
| 3 add member | 1187-1222 | `bAddMember` | success: broadcast code 4 result 1 to **all** game servers incl. the sender; failure: code 4 result 0 to the requester; 22 bytes |
| 4 remove member | 1224-1259 | `bRemoveMember` | success: broadcast code 6 result 1 to all; failure: code 6 result 0 to the requester; 22 bytes |
| 5 check member | 1261-1263 | `bCheckPartyMember` | none if the member is found (clears its server-change timer); if not found, code 3 (see PartyManager) |
| 6 get info | 1265-1267 | `bGetPartyInfo` | code 5 list to the requester |
| 7 server change | 1269-1271 | `SetServerChangeStatus` | none |

An unknown request type is ignored. The incoming `iPartyID` is a u16 (0..65535) used as an array index by the
party manager ([GATE-013](../issues/gate.md#gate-013-party-id-from-the-wire-is-an-unchecked-array-index); see 2). [V]

## 2. PartyManager (`PartyManager.cpp`, `PartyManager.h`)

All state is in memory and lost at restart. Slot index 0 is never used by any search loop (loops start at 1) except
`GameServerDown` (starts at 0, harmless since index 0 is never assigned) [V].

| Function | Line | Purpose | Callers -> callees | Messages |
|---|---|---|---|---|
| `PartyManager::PartyManager(CGateCore*)` | 14-28 | Zero `m_iMemberNumList` and `m_stMemberNameList`, remember the core, `m_dwCheckMemberActTime = timeGetTime()`. | `CGateCore::CGateCore` (41) | - |
| `~PartyManager` | 30-33 | Empty. | `~CGateCore` (59) | - |
| `iCreateNewParty(int, char*)` | 35-71 | Fail (0) if the master's name is already a member of any party. Party id = first `i >= 1` with `m_iMemberNumList[i] == 0`; that count is incremented **before** a member slot is found. Then the first free member slot gets party id, name, `m_iIndex = iClientH` (the game-server socket slot). Fails (0) if no id or slot is free; if the slot search fails the id count stays at 1 ([GATE-015](../issues/gate.md#gate-015-icreatenewparty-leaks-the-id-when-no-member-slot-is-free)) [V, 44-70]. | `PartyOperation` (1149) | out reply by caller |
| `bDeleteParty(int)` | 73-112 | Zero the count, clear every member slot of that party, broadcast code 2 + party id (written 8 bytes, sent 10) to all game servers. Returns whether any slot was cleared. Does **not** check `iPartyID` range. | `bAddMember` (131), `bRemoveMember` (167) -> `SendMsgToAllGameServers` | out MSGID_PARTYOPERATION code 2 |
| `bAddMember(int, int, char*)` | 114-152 | Refuse if `m_iMemberNumList[iPartyID] >= 8`. Remove the name from whichever slot currently holds it (122-132: not `break`, runs over the whole table). The found slot is cleared, then the code decrements and, at <= 1 member, deletes the target party `iPartyID` (the argument, `PartyManager.cpp:130-131`), not the party that held the name ([GATE-014](../issues/gate.md#gate-014-baddmember-decrements-the-wrong-party)). Then put the name in the first free slot, count++. No check that the party exists or that `iPartyID` is in range ([GATE-013](../issues/gate.md#gate-013-party-id-from-the-wire-is-an-unchecked-array-index)) [V]. | `PartyOperation` (1188) -> `bDeleteParty` | - |
| `bRemoveMember(int, char*)` | 154-176 | Find (party id, name), clear the slot, count--; at <= 1 member delete the whole party (broadcast code 2). | `PartyOperation` (1225), `CheckMemberActivity` (314) -> `bDeleteParty` | - |
| `bCheckPartyMember(int, int, int, char*)` | 179-210 | If (party, name) exists clear its `m_dwServerChangeTime` and return TRUE (no reply). Otherwise reply to the requester only: code 3 + `iGSCH` + name, passed size 20 (written 18). | `PartyOperation` (1262) -> `iSendMsg` | out code 3 |
| `bGetPartyInfo(int, int, char*, int)` | 212-248 | Reply to the requester: code 5, `iGSCH`, name, u16 member count, then each member name copied as 10 bytes with an **11-byte stride**, in table (slot) order, not join order. Size `20 + n*11 + 1`. Always returns TRUE, including an empty/unknown party. | `PartyOperation` (1266) -> `iSendMsg` | out code 5 |
| `GameServerDown(int)` | 250-266 | For every member slot whose `m_iIndex` equals the dead game server's slot: `m_iMemberNumList[party]--` and clear the slot. Does not delete a party that falls to 0 or 1 members and notifies no one ([GATE-016](../issues/gate.md#gate-016-gameserverdown-leaves-small-parties-and-is-skipped-on-most-deletes)) [V]. | `OnClientSocketEvent` (221), only for socket-event failures | - |
| `SetServerChangeStatus(char*, int)` | 268-277 | Stamp `m_dwServerChangeTime = timeGetTime()` for (party, name). | `PartyOperation` (1270) | - |
| `CheckMemberActivity` | 279-316 | Runs at most every 2 s (so every tick). For each member whose change stamp is older than 20 s: broadcast code 6, result 1, `iGSCH` 0, name, party id (22 bytes) to all game servers, then `bRemoveMember`. | `OnTimer` (683) -> `SendMsgToAllGameServers`, `bRemoveMember` | out code 6 |

Notes (all [V] unless marked):
- `m_iIndex` is the socket slot of the game server that created the entry or added the member; code 5 only clears the
  stamp on a crossing ([GATE-017](../issues/gate.md#gate-017-member-server-index-is-never-updated-on-a-crossing)) [V code].
- `GameServerDown` runs only from `OnClientSocketEvent`. Every other place that deletes a peer (all fatal-send paths,
  wrong-MsgType deletes) skips it ([GATE-016](../issues/gate.md#gate-016-gameserverdown-leaves-small-parties-and-is-skipped-on-most-deletes)) [V: single call site, 221].
- A one-member party can be left behind by `GameServerDown` or by `bAddMember` onto an unknown id, because only
  `bRemoveMember` and `bAddMember`'s duplicate-name step delete small parties (and that step decrements the target
  party, not the old one, see `bAddMember` above) [V].
- Party master is not stored; the first member has no special slot [V].
- 5000 is both the number of party ids (valid 1..4999) and the number of **member slots in total** (valid 1..4999),
  not "5000 parties of 8": 8 is only the per-party cap [V, PartyManager.h:19-20, 38-44].

## 3. CClient (`Client.cpp`, `Client.h`)

| Function | Line | Purpose | Callers -> callees |
|---|---|---|---|
| `CClient::CClient(HWND)` | 11-28 | Create the socket (`new XSocket(hWnd, 300)`) with 30000-byte buffers, zero name/address, `m_bIsGameServer = FALSE`, `m_sIsActivated = 0`, `m_bIsShutDown = FALSE`, `m_iTotalClients = 0`. `m_dwAliveTime`, `m_dwProcessHandle` are not initialised (set in `bAccept` / registration) [V]. | `CGateCore::bAccept` (241) -> `XSocket::XSocket`, `bInitBufferSize` |
| `CClient::~CClient` | 30-33 | Delete the socket (its destructor closes the connection). | every `delete m_pClientList[i]`, `~CGateCore` |

## 4. Wmain.cpp (window, timer, logs)

| Function | Line | Purpose | Callers -> callees |
|---|---|---|---|
| `ThreadProc(void*)` | 44-73 | Worker thread. Sleeps 1 s per loop; when the counter exceeds 60 (so every 61 s) writes `"(name) Total user(n)"` for each game server in slots 1..499 via `PutGameServerList`. Reads `m_pClientList` with no lock while the main thread mutates it [V]. Ends when `G_bThreadFlag` is FALSE; `_endthread()` inside a `_beginthreadex` thread [V]. | `CGateCore::bInit` (68) -> `PutGameServerList` |
| `WndProc` | 76-118 | Dispatch: `WM_USER_TIMERSIGNAL` (`WM_USER+500`) -> `OnTimer(0)` if `wParam == G_mmTimer0`; `WM_USER_ACCEPT` (`WM_USER+100`) -> `OnAccept`; `WM_KEYDOWN` / `WM_KEYUP` -> core; `WM_PAINT` -> `OnPaint`; `WM_DESTROY` -> `OnDestroy`; `WM_CLOSE` -> Yes/No box "Quit gate server?"; default: messages `WM_USER+101 .. +600` -> `OnClientSocketEvent`. The timer constant is defined without parentheses (`WM_USER + 500`). | window class (InitApplication) |
| `WinMain` | 122-133 | Build the class name `"Gate-Server%d"`, `InitApplication`, `InitInstance`, `Initialize`, `EventLoop`. | OS |
| `InitApplication` | 137-153 | `RegisterClass` (styles in `../servers/ui.md`). | `WinMain` |
| `InitInstance` | 157-187 | Title `"Helbreath GateServer 2002-6-20"`, 620 x 700 popup window, show. | `WinMain` |
| `EventLoop` | 191-207 | `PeekMessage` / `GetMessage` / dispatch; `UpdateScreen()` after every dispatched message; `WaitMessage` when idle. Returns on `WM_QUIT`. | `WinMain` -> `UpdateScreen` |
| `Initialize` | 211-241 | `_InitWinsock`, `new CGateCore`, `bInit`, create the listen `XSocket(G_hWnd, 300)`, `bListen(addr, port, WM_USER_ACCEPT)`, start the 3000 ms multimedia timer, log "(!) Gate Server Listening...". Failures at the first three steps show a message box and post quit. **The `bListen` result is not checked (235)**: a bind failure (port in use, empty/invalid address) still logs "Listening" and the gate runs without a listener [V]. | `WinMain` -> `_InitWinsock`, `CGateCore`, `bInit`, `XSocket`, `bListen`, `_StartTimer` |
| `OnDestroy` | 245-254 | Delete the core, delete the listen socket, `_TermWinsock`, stop the timer, post quit. The timer is stopped *after* the core is deleted and `G_pGateCore` is not nulled; the log thread may also touch the freed core [I: narrow race at shutdown]. | `WndProc` |
| `PutLogList(char*)` | 258-268 | Shift the 50-line ring (`char[120*50]`) down one line, copy the new text into line 0 (`strlen` bytes, unbounded), set `G_cMsgUpdated`. Main-thread only. | everywhere |
| `UpdateScreen` | 270-276 | If updated, `InvalidateRect(G_hWnd, NULL, TRUE)`. | `EventLoop` |
| `OnPaint` | 279-315 | Draw 35 log lines (newest at y = 605, 16 px steps up), three help lines at y = 10/25/40 (F1+F4, F2, F3), a rule at y = 620, and the monitor status row: slot numbers at y = 640 and `?` / `X` / `O` (status 0 / 1 / 2) at y = 655, 30 px apart. | `WndProc` |
| `OnAccept` | 317-320 | `G_pGateCore->bAccept(G_pListenSock)`. One accept per `FD_ACCEPT` message. | `WndProc` |
| `_TimerFunc` | 323-326 | Multimedia-timer callback (another thread): `PostMessage(G_hWnd, WM_USER_TIMERSIGNAL, id)`. | winmm |
| `_StartTimer(DWORD)` | 329-339 | `timeBeginPeriod(min)`, `timeSetEvent(periodic)`. | `Initialize` |
| `_StopTimer(MMRESULT)` | 343-353 | `timeKillEvent`, `timeEndPeriod`. | `OnDestroy` |
| `PutItemLogFileList(char*)` | 356-374 | Append `"(%4d:%2d:%2d:%2d:%2d) - " + text` to `ItemEvents.log` (space-padded fields from `%4d:%2d`). | `ItemLog` |
| `PutGameServerList(char*)` | 377-402 | `_mkdir("GameServerLog")` on **every call**, then append the dated line to `GameServerLog\GameServerLogYYYYMMDD.log` (file name zero-padded `%04d%02d%02d`, line stamp space-padded). So the folder is created on demand. | `ThreadProc` |
| `PutLogFileList(char*)` | 405-423 | Append the dated line to `HackAttacker.log`. | `CGateCore::bAccept` (261) |

## 5. XSocket (`XSocket.cpp`, `XSocket.h`)

The same class as the other servers; the gate uses it in message-pump mode (`WSAAsyncSelect`) with 3-byte framing
(key byte, u16 total length). The gate **never encrypts**: `iSendMsg` forces `cKey = 0` (421). It decrypts
received data if the key byte is non-zero (562-567). [V]

| Function | Line | Purpose | Callers -> callees |
|---|---|---|---|
| `XSocket::XSocket(HWND, int)` | 11-41 | Zero state: `m_cType = 0`, no buffers, header read pending (3 bytes), empty unsent ring (`DEF_XSOCKBLOCKLIMIT` 300 entries), `m_bIsWriteEnabled = FALSE`, remember window and block limit. | `CClient::CClient` (14), `Initialize` (234), `CGateCore::bAccept` (277) |
| `~XSocket` | 43-55 | Free buffers and unsent entries, `_CloseConn`. | `~CClient`, `OnDestroy` (248), `bAccept` (279) |
| `bInitBufferSize(DWORD)` | 57-71 | Allocate receive and send buffers of size + 8. | `CClient::CClient` (15), `CGateCore::bAccept` (244) |
| `iOnSocketEvent(WPARAM, LPARAM)` | 73-120 | Check type NORMAL and that `wParam` is this socket (else `SOCKETMISMATCH`); FD_CONNECT (unused), FD_READ -> `_iOnRead`, FD_WRITE -> `m_bIsWriteEnabled = TRUE` + `_iSendUnsentData`, FD_CLOSE -> `SOCKETCLOSED`. | `OnClientSocketEvent` (190) |
| `bConnect(char*, int, uint)` | 122-169 | Outbound non-blocking connect. **Unused in the gate.** | none |
| `_iOnRead` | 171-256 | Two-phase read. Header: `recv` into `m_pRcvBuffer`, then body length = u16 at offset 1 minus 3 (203, 213: `m_dwReadSize = length - 3` is compared with `m_dwBufferSize` (30000), so the u16 length must exceed 30003 for `MSGSIZETOOLARGE`; a length field below 3 wraps to a huge unsigned value and takes the same path [V]); length 3 -> empty message, `READCOMPLETE`. Body: `recv` the remainder; `READCOMPLETE` when full. One `recv` per FD_READ; `recv` 0 -> `SOCKETCLOSED`; `WSAEWOULDBLOCK` -> `BLOCK`. | `iOnSocketEvent` (105) |
| `_iSend(char*, int, BOOL)` | 260-322 | Send in a loop; if the ring head is occupied queue the new data (when `bSaveFlag`) and return `BLOCK`; on `WSAEWOULDBLOCK` queue the remainder and return `BLOCK`; queue full -> `QUENEFULL`; other errors `SOCKETERROR`. | `iSendMsg` (450) |
| `_iSend_ForInternalUse` | 326-350 | Send without queuing; returns bytes sent so far on would-block. | `_iSendUnsentData` (384) |
| `_iRegisterUnsentData` | 354-372 | Copy into the ring at the tail; 0 if the tail slot is occupied (full), -1 on allocation failure; wrap at `m_iBlockLimit`. | `iSendMsg` (448), `_iSend` (268, 300) |
| `_iSendUnsentData` | 376-413 | Flush the ring from the head; a partial send replaces the head with its remainder and returns `UNSENTDATASENDBLOCK`. | `iOnSocketEvent` (110) |
| `iSendMsg(char*, DWORD, char)` | 416-454 | Refuse bodies above `m_dwBufferSize` (30000). Build `[0][u16 size+3][body]`. Until the first FD_WRITE (`m_bIsWriteEnabled == FALSE`) the message is *queued*, and the call returns `_iRegisterUnsentData() - 3`: 1 -> **-2**, but 0 (ring full) -> **-3** and -1 (allocation failure) -> **-4** (`XSocket.cpp:447-453`). None of these equals a `DEF_XSOCKEVENT_*` value (those are -121..-136, `XSocket.h:33-48`), so no caller's fatal switch matches and a full ring before the first write is never detected. It also returns `SOCKETMISMATCH` (-121, line 427) for a peer whose socket type is no longer NORMAL; no sender tests it. Afterwards `_iSend`. Return: body size on success, a negative code on failure. | every sender in `GateCore.cpp`, `PartyManager.cpp` |
| `bListen(char*, int, uint)` | 456-489 | `socket`, `bind(addr, port)`, `listen(5)`, `WSAAsyncSelect(FD_ACCEPT)`. Returns FALSE on any failure. | `Initialize` (235) |
| `bAccept(XSocket*, uint)` | 491-520 | `accept` into the target XSocket, `WSAAsyncSelect(FD_READ | FD_WRITE | FD_CLOSE)` on the *listener's* window with message `uiMsg`, set type NORMAL, 40960-byte socket buffers. | `CGateCore::bAccept` (243, 278) |
| `_CloseConn` | 522-540 | `shutdown(SD_SEND)`, drain `recv` until error/0, `closesocket`. | `~XSocket` |
| `iGetSocket` | 542-545 | Return the SOCKET. **Unused.** | none |
| `pGetRcvDataPointer(DWORD*, char*)` | 547-569 | Return `m_pRcvBuffer + 3`, size = length - 3; decrypt in place if the key byte is non-zero. | `OnClientRead` (294) |
| `_InitWinsock` | 575-587 | `WSAStartup(2.2)`. | `Initialize` (214) |
| `_TermWinsock` | 590-593 | `WSACleanup`. | `OnDestroy` (250) |
| `iGetPeerAddress(char*)` | 597-607 | `getpeername` + `inet_ntoa` into the caller's buffer (`strcpy`). | `CGateCore::bAccept` (246) |

## 6. StrTok (`StrTok.cpp`) - not built

`CStrTok::CStrTok` (12), `~CStrTok` (21), `pGet` (26), `_bIsSeperator` (69): a Korean-era tokeniser (separators plus
CRLF). The file is not listed in `GateSever.dsp` or `GateSever.vcproj`, nothing includes `StrTok.h`, and the config
parser uses CRT `strtok` instead [V].

# System: network (framing, cipher, envelopes, dispatch)

*Size verdict (2026-10-25): one cohesive doc. Every section presupposes the frame and queue model in
sections 2-3, so it is not split; use the Contents block.*

Cluster: infrastructure. Programs: client (CLI), game server (SRV), gate (GATE), Main Log (MLS) and
World Log (WLS). Sibling docs: `session.md`, `persistence.md`, `server-topology.md`, `admin.md`.

Citations (line numbers as of 2026-10-25):
- `SRV` = `sources/hgserver-librehelbreath/src/` (line-identical to the `.claude/scratch/srv220/` export the
  older docs cite; the only differences are a cast at `Game.cpp:48960` and `Wmain.cpp:357`).
- `CLI` = `sources/client-2.20/`.
- `GATE` = `sources/gateserver-librehelbreath` numbered as commit `0b994cd` (the numbering
  `research/gate/` uses). The working-tree HEAD `a2b7f1d` is clang-formatted, so its lines are shifted.
- `WLS` / `MLS` = virtual addresses in the official binaries, as given in
  `research/servers/official-worldserver.md` / `official-mainserver.md` (no source exists).
- [V] verified in source here, [I] inferred, [U] unverified.

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

Give every Helbreath program one cheap, uniform way to exchange typed messages over TCP, so a
game server can treat a player, a Log server and the gate as "message sources" feeding one
single-threaded game loop. The player never sees it directly; what they see is its failure
modes: rubber-banding (motion reject), "connection lost" after lag, and disconnects when the client
sends malformed or too-fast traffic. For the operator it means every program is a Win32 window app
driven by socket window messages and a timer, never by worker threads [V `SRV Wmain.cpp:68-139`,
`GATE Wmain.cpp:76-118`; `research/gate/subsystems.md` section 1].

Design intent in one line: **trusted-network, hand-rolled RPC**: fixed-width little-endian structs,
a 3-byte frame, an obfuscating byte cipher on the client's outbound traffic only, and two envelope
messages (`MSGID_COMMAND_COMMON` up, `MSGID_NOTIFY` down) that multiplex most gameplay [I].

## 2. Subsystems

```
network
 +- 2.1 XSocket transport      (all 5 programs; framing, cipher, unsent ring, WSAAsyncSelect)
 +- 2.2 Socket ownership       (per program: which sockets exist and which window message each gets)
 +- 2.3 Message queue          (game server only: 100000-slot ring tagged client/log/gate)
 +- 2.4 Dispatch               (MsgProcess by source and MsgID; client: GameRecvMsgHandler / LogResponseHandler)
 |    +- 2.4.1 Envelope COMMAND_COMMON  (client -> game, DEF_COMMONTYPE_* sub-codes)
 |    +- 2.4.2 Envelope COMMAND_MOTION  (client -> game, DEF_OBJECT* actions)
 |    +- 2.4.3 Envelope NOTIFY          (game -> client, DEF_NOTIFY_* sub-codes, 1300-line builder)
 |    +- 2.4.4 Gate stock container     (MSGID_SERVERSTOCKMSG with GSM_* records)
 +- 2.5 Broadcast fan-out      (SendEventToNearClient_TypeA/B, client shortcut list)
 +- 2.6 Keep-alive and liveness (client 8 s check, server 10 s timeout, client lag counter)
```

- **2.1 XSocket transport** - intent [I]: same source lineage in three builds, so the
  wire format cannot drift. The game server links it as a prebuilt lib (`GameServerXsocket.lib`,
  `research/game-server/build-config.md`), the client and gate carry near-identical copies [V].
- **2.2 Socket ownership** [I] - intent: map every socket to its own window message so the Win32 message
  loop serialises all I/O (no locks needed).
- **2.3 Message queue** [I] - intent: decouple socket reads from game logic; all inbound traffic from
  three source kinds is drained once per 300 ms tick in `MsgProcess`, so handlers never re-enter.
- **2.4 Dispatch** [I] - intent: one switch per source; envelopes keep the top-level MsgID space small
  while the sub-code space grows with features.
- **2.5 Broadcast** [I] - intent: deliver world events only to clients that can see them (view range)
  and iterate live clients densely rather than over 2000 slots.
- **2.6 Keep-alive** [I] - intent: detect dead or stalled peers on both ends; also used as the clock
  base for the speed-hack check (owned by `admin.md`).

## 3. Functions

Function maps: `research/game-server/functions/network.md`, `support.md`;
`research/client/functions/networking.md`; `research/gate/functions.md`. Only the system-relevant
ones are listed.

| Subsystem | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | `XSocket::iSendMsg` | SRV | `XSocket.cpp:414-446` | frame + optional encrypt + send/queue [V] |
| 2.1 | `XSocket::_iOnRead` | SRV | `XSocket.cpp:169-254` | two-phase read: 3-byte header then body [V doc] |
| 2.1 | `XSocket::pGetRcvDataPointer` | SRV | `XSocket.cpp:539-561` | returns body, decrypts if key != 0 [V doc] |
| 2.1 | `XSocket::iSendMsg` | CLI | `XSocket.cpp:465-501` | client copy; encrypts with caller's key [V doc] |
| 2.1 | `XSocket::iSendMsg` | GATE | `XSocket.cpp:416-454` | gate copy; key forced 0, queues until first FD_WRITE [V doc] |
| 2.2 | `OnClientSocketEvent` | SRV | `Game.cpp:467-512` | per-client socket event; queues read, deletes on error [V] |
| 2.2 | `OnMainLogSocketEvent` / `OnSubLogSocketEvent` | SRV | `Game.cpp:4144-4191`, `42393-42455` | Log links (topology owns lifecycle) [V] |
| 2.2 | `OnGateSocketEvent` | SRV | `Game.cpp:20547-20611` | gate link (topology owns lifecycle) [V] |
| 2.2 | `OnGameSocketEvent` / `OnLogSocketEvent` | CLI | `Game.cpp:1277-1327`, `14726-14771` | client's two sockets [V doc] |
| 2.2 | `CGateCore::OnClientSocketEvent` | GATE | `GateCore.cpp:181-229` | peer slot i = message `WM_USER+101+i` [V doc] |
| 2.3 | `bPutMsgQuene` / `bGetMsgQuene` | SRV | `Game.cpp:15275-15301`, `15304-15329` | ring buffer of `CMsg` (from, data, size, index, key) [V doc] |
| 2.3 | `OnClientRead`, `OnMainLogRead`, `OnSubLogRead`, `OnGateRead` | SRV | `Game.cpp:872-885`, `4193-4204`, `42461-42472`, `20613-20624` | copy into the queue with the source tag [V doc] |
| 2.4 | `MsgProcess` | SRV | `Game.cpp:14845-15272` | drains queue; switch by source then MsgID [V] |
| 2.4.1 | `ClientCommonHandler` | SRV | `Game.cpp:15332-15594` | `DEF_COMMONTYPE_*` switch [V doc] |
| 2.4.2 | `ClientMotionHandler` | SRV | `Game.cpp:976-1126` | `DEF_OBJECT*` switch with frequency guards [V doc] |
| 2.4.3 | `SendNotifyMsg` | SRV | `Game.cpp:17692-19037` | builds every `DEF_NOTIFY_*` [V doc] |
| 2.4.3 | `NotifyMsgHandler` | CLI | `Game.cpp:29946-31462` | 133 notify cases [V doc] |
| 2.4 | `GameRecvMsgHandler` | CLI | `Game.cpp:2936-3027` | client inbound switch by MsgID [V doc] |
| 2.4 | `LogResponseHandler` | CLI | `Game.cpp:14773-15449` | Log replies, switches on MsgType only [V doc] |
| 2.4 | `bSendCommand` | CLI | `Game.cpp:1431-2275` | single outbound builder (27 MsgIDs) [V doc] |
| 2.4 | `CGateCore::OnClientRead` | GATE | `GateCore.cpp:285-449` | gate switch; no default (unknown dropped) [V doc] |
| 2.4.4 | `bStockMsgToGateServer` / `SendStockMsgToGateServer` | SRV | `Game.cpp:49290-49306`, `49309-49336` | append GSM record / flush once per second [V] |
| 2.4.4 | `ServerStockMsgHandler` | SRV | `Game.cpp:49339-49764` | parse and execute GSM records [V doc] |
| 2.4.4 | `SendMsgToAllGameServers` | GATE | `GateCore.cpp:1015-1031` | blind fan-out of the stock body [V doc] |
| 2.5 | `SendEventToNearClient_TypeA` / `_TypeB` | SRV | `Game.cpp:2890-3356`, `16406-16473` | view-range broadcast of motion / positional events [V doc] |
| 2.5 | `bAddClientShortCut` / `RemoveClientShortCut` | SRV | `Game.cpp:50382-50398`, `50400-50418` | dense live-client list [V doc] |
| 2.6 | `CheckClientResponseTime` (timeout part) | SRV | `Game.cpp:3850-3866` | drop clients silent > 10 s [V] |
| 2.6 | `CGame::OnTimer` (keep-alive part) | CLI | `Game.cpp:4266-4301` | send CHECKCONNECTION every 8 s; lag counter [V] |
| 2.6 | `CheckConnectionHandler` | SRV | `Game.cpp:48937-49039` | keep-alive receiver + clock comparison [V] |

## 4. Commands and messages

From `research/systems/commands/by-system.md` (envelope and session rows) and
`research/shared/messages/`.

- Envelopes: `MSGID_COMMAND_COMMON` (C->S, 56 `DEF_COMMONTYPE_*`), `MSGID_COMMAND_MOTION` (C->S, 17
  motion codes), `MSGID_NOTIFY` (S->C, 141 `DEF_NOTIFY_*`), `MSGID_SERVERSTOCKMSG` (game<->gate, 18
  `GSM_*`). Counts: `research/systems/commands/by-system.md` last paragraph [V doc].
- Event broadcasts: `MSGID_EVENT_MOTION`, `MSGID_EVENT_LOG`, `MSGID_EVENT_COMMON`, `MSGID_DYNAMICOBJECT`
  (entity system owns the content) [V doc].
- Liveness: `MSGID_COMMAND_CHECKCONNECTION` (C->S, client time u32 at offset 6, `CLI Game.cpp:1855-1866`),
  `MSGID_RESPONSE_CHECKCONNECTION` (S->manager only) [V].
- Generic sub-codes: `DEF_MSGTYPE_CONFIRM 0x0F14`, `DEF_MSGTYPE_REJECT 0x0F15`
  (`research/servers/wire-format.md`) [V doc].
- Value collisions handled by context: `0x0FA314DB` is both the Magic.cfg push and `MSGID_EVENT_COMMON`;
  `0x0FA314DC` both the Skill.cfg push and `MSGID_COMMAND_COMMON` (`research/servers/official-worldserver.md`
  3.1). Safe only because the game server switches on source first (`SRV Game.cpp:14876-15268`) [V].

## 5. Data

| Item | Value | Cite |
|---|---|---|
| Frame | `[key u8][len u16 = body+3][body]` | `research/servers/wire-format.md` [V doc] |
| Body | `[MsgID u32][MsgType u16][payload]`, LE, unaligned, fixed-width zero-padded strings | `SRV MessageIndex.h:2-3` [V doc] |
| Cipher | `b += (i ^ key); b ^= (key ^ (n - i))`, decrypt reverses; key 0 = plain | `SRV XSocket.cpp:435-440, 554-559` [V doc] |
| Who encrypts | client: random key 1..255 per message (`CLI Game.cpp:1445`); game server, gate, Log replies: key 0; WLS->MLS and MLS->WLS: random key | wire-format; `official-worldserver.md` 2 [V doc] |
| Buffer | `DEF_MSGBUFFERSIZE` 30000 per socket | `SRV Client.h:24` [V doc] |
| Unsent ring | 300 blocks for server sockets, 15 for client sockets (`DEF_CLIENTSOCKETBLOCKLIMIT`) | `research/game-server/data-structures.md` XSocket [V doc] |
| Queue | `m_pMsgQuene[100000]` of `CMsg`; sources 1 client, 2 log, 3 gate | `research/game-server/data-structures.md` [V doc] |
| Per-socket window messages | SRV: `WM_ONCLIENTSOCKETEVENT + i` (`Game.cpp:427`), `WM_ONLOGSOCKETEVENT + 1..10` (`Game.cpp:3733`), `WM_ONGATESOCKETEVENT`; GATE: `WM_USER+101+slot`; CLI: `WM_USER+2000/2001` | [V] |
| Client limits | `DEF_MAXCLIENTS` 2000 slots, slot 0 unused | `SRV Game.cpp:417` loop from 1 [V] |

## 6. Flow

**F1. A client command reaches a handler and its result returns** (example: equip an item):
1. CLI `bSendCommand(MSGID_COMMAND_COMMON, DEF_COMMONTYPE_EQUIPITEM, ...)` builds the body and calls
   `m_pGSock->iSendMsg(cMsg, size, cKey)` with a random key (`CLI Game.cpp:1431-2275`, key `1445`) [V doc].
2. SRV `WndProc` -> `OnClientSocketEvent` (`Game.cpp:467-512`): `iOnSocketEvent` returns READCOMPLETE,
   `m_dwTime` is stamped (this is what the 10 s timeout watches) and `OnClientRead` queues the body with
   source `DEF_MSGFROM_CLIENT` and the key [V].
3. Next 300 ms tick: `OnTimer` -> `GameProcess` -> `MsgProcess` (`Game.cpp:3677`, `5045-5050`,
   `14845`) drains the queue [V].
4. `MsgProcess` case `MSGID_COMMAND_COMMON` -> `ClientCommonHandler` -> the item handler
   (`Game.cpp:15040-15041`, `15332`) [V].
5. Result goes back as `SendNotifyMsg(..., DEF_NOTIFY_*)` -> `iSendMsg(cData, size)` key 0
   (`Game.cpp:17692-19037`) and, if visible to others, `SendEventToNearClient_TypeA` [V doc].
6. CLI `OnGameSocketEvent` -> `GameRecvMsgHandler` -> `NotifyMsgHandler` -> `NotifyMsg_*` -> UI
   (`CLI Game.cpp:1277, 2936, 29946`) [V doc].

**F2. Unknown or malformed client message**: `MsgProcess` default branch logs "Unknown message
received! Delete Client" and calls `DeleteClient(iClientH, TRUE, TRUE)` (`SRV Game.cpp:15083-15086`) [V].
A frame body > 30000 returns `MSGSIZETOOLARGE` and `OnClientSocketEvent` deletes the client with
save (`Game.cpp:509`) [V].

**F3. Cross-server event (stock message)** (example: shout across servers):
1. SRV A handler calls `bStockMsgToGateServer(record)` which appends a `GSM_*` record to
   `m_cGateStockMsg` (`Game.cpp:49290-49306`) [V doc].
2. Every 1 s `OnTimer` -> `SendStockMsgToGateServer` sends `MSGID_SERVERSTOCKMSG` with all records and a
   trailing 0 (`Game.cpp:3713`, `49309-49336`) [V].
3. GATE `OnClientRead` -> `SendMsgToAllGameServers(iClientH, ..., FALSE)`: forwards the raw body to every
   other game-server peer, never parsing it (`GATE GateCore.cpp:304-307, 1015-1031`) [V doc].
4. SRV B `MsgProcess` (source gate) -> `ServerStockMsgHandler` executes each record
   (`Game.cpp:14892-14893`, `49339-49764`) [V].

## 7. Rules and invariants

- Frame body <= 30000 bytes in every program; larger is a fatal read and the peer is dropped
  (`research/servers/wire-format.md`) [V doc].
- No sequence number, checksum, session token or handshake; the "confirm code" in
  `SRV XSocket.h:5` is never checked (wire-format) [V doc]. Identity on a game-server socket is
  whatever `MSGID_REQUEST_INITPLAYER` claimed, re-checked only by the Log server's player-data lookup
  (`session.md`) [I].
- A full send queue counts as a dead peer: server-to-client sends treat `QUENEFULL` as fatal and
  delete the client (example `SRV Game.cpp:4974-4983`, InitPlayerData) [V].
- Handlers are never re-entered: all game-server I/O is queued and drained on the timer tick; gate
  and WLS run handlers to completion on the window thread [V `SRV Game.cpp:14876`;
  `research/gate/subsystems.md` 1].
- Client keep-alive period 8 s (`CLI Game.cpp:4272-4277`) < server timeout `DEF_CLIENTTIMEOUT` 10 s
  (`SRV Game.h:59`, used `Game.cpp:3850`) [V]. Any received message refreshes the timer, not only the
  keep-alive (`Game.cpp:485` stamps `m_dwTime` on read) [V doc].
- Client lag rule: the check at `CLI Game.cpp:4288-4299` is gated on `dwTime - m_dwCheckChatTime > 2000`, but
  `m_dwCheckChatTime` is set to `m_dwTime` (the stamp of the last received packet: `1297`, `14746`, `15515`),
  not to the current time. So the check runs at most every 2 s of received traffic, and on every 1 s
  `OnTimer` tick (`Wmain.cpp:857`) while the server is silent. If 6 or more motion commands are unanswered
  (`m_cCommandCount >= 6`) the lag counter rises; at 7 the client drops to "connection lost". Effect: about 7 s
  of silence with >= 6 outstanding commands, not the 14 s of the source comment [V code, I effect].
- Stock messages flush at most once per second; a record must fit the remaining stock buffer
  (`SRV Game.cpp:49290-49306`) [V doc].
- Gate peers: at most 499 (slot 0 unused); slot 399 shares an id with the timer message
  (`research/gate/subsystems.md` 2.2; [NET-008](../issues/network.md#net-008-gate-peer-table-slot-0-unused-and-slot-399-collides-with-the-timer-message)) [V doc].

## 8. Couplings

Out of network into other systems:
- Every gameplay system is reached through `ClientCommonHandler` / `ClientMotionHandler` / `SendNotifyMsg`
  (owners: gameplay clusters). Coupling points: the three functions and the sub-code enums
  `DEF_COMMONTYPE_*`, `DEF_OBJECT*`, `DEF_NOTIFY_*` [V].
- Movement/combat anti-speed checks sit inside the motion dispatch (`ClientMotionHandler` calls
  `bCheckClientMoveFrequency` / `bCheckClientAttackFrequency`, `SRV Game.cpp:976-1126`) -> `admin.md`.
- Chat, crusade, party, guild, GM use the stock container -> `bStockMsgToGateServer` callers listed in
  `research/game-server/functions/network.md` [V doc].
- Session: `OnClientSocketEvent` error paths call `DeleteClient` (save/logout policy in `session.md`).

Into network from other systems:
- `server-topology.md` owns socket creation, registration and reconnection; network only frames
  and queues.
- `persistence.md` / `session.md` produce the largest bodies (player record, config pushes up to ~30000
  bytes) and are what pins the 30000 limit [I].
- Client presentation (other cluster) consumes `NotifyMsgHandler` and the dialog ids it opens.

## 9. Current behaviour vs intent

- The cipher key travels in clear in byte 0 and only client output is encrypted ([NET-001](../issues/network.md#net-001-cipher-is-obfuscation-not-security)).
- `CMisc::bDecode` on name, account and password returns without transforming ([NET-002](../issues/network.md#net-002-field-level-obfuscation-cmiscbdecode-is-a-no-op)).
- The envelope id overlaps the config-push ids; the dispatcher switches on the source socket first ([NET-003](../issues/network.md#net-003-envelope-id-collision-with-config-pushes)).
- A zero-length frame re-runs the previous message in the gate ([NET-004](../issues/network.md#net-004-zero-length-frames-re-run-the-previous-message-in-the-gate)).
- A full gate ring before the first write is not reported as a socket event ([NET-005](../issues/network.md#net-005-gate-full-ring-before-first-write-is-never-detected)).
- No thread is created in the game server; it runs single-threaded ([NET-006](../issues/network.md#net-006-game-server-multithreading-is-dead)).
- An unknown message after init deletes the client with a save ([NET-007](../issues/network.md#net-007-unknown-message-saves-the-character-after-init)).

## 10. Open questions

- Whether the original game-server `XSocket` lib (`GameServerXsocket.lib`) matches `SRV XSocket.cpp`
  byte for byte: the lib is prebuilt and was not disassembled [U].
- The exact client receive-side handling when a server reply arrives encrypted: the client decrypts any
  key, but no server sends one, so untested [U].

## 11. Doc discrepancies

- `research/game-server/functions/network.md` says `bAccept` "rejects (accepts into a dummy socket)";
  confirmed, but note the rejection also covers `m_bOnExitProcess` and the full table (`SRV Game.cpp:403-416, 456-461`).
  No conflict.
- `research/gate/` line numbers refer to `0b994cd`; the working-tree HEAD `a2b7f1d` is a clang-format
  pass with shifted lines (for example `RegisterGameServerHandler` is at 437 in HEAD, 451 in `0b994cd`) [V].

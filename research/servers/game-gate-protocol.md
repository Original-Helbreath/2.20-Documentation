# Game server <-> Gate server protocol

Tags and [V]/[I]/[U] as in `topology.md`; framing in `wire-format.md`. Layouts come from both
ends: the game server 2.20 (`SRV`) and the official gate v2.19 (`GATE` @ `0b994cd`, read with
`git show 0b994cd:<file>`). Where they differ, both are given. Offsets are body offsets
(0 = MsgID, 4 = MsgType, 6 = payload).

## Contents
1. Connection
2. Message table, game -> gate
3. Message table, gate -> game
4. Stock messages (GSM records)
5. Party operations
6. Shutdown and monitor messages
7. Failure behaviour
8. 2.19 gate vs 2.20 game server

## 1. Connection

- One TCP socket from each game server to `gate-server-address:gate-server-port` (6502 in the sample
  cfg), opened at init (`SRV Game.cpp:808-810`). The gate only listens (`GATE Wmain.cpp:234-235`). [V]
- The gate keys are always 0 (`GATE XSocket.cpp:421-422`); the game server also sends with key 0. [V]
- On accept the gate compares the peer IP with its `game-server-list` allow-list and drops unknown
  peers (`GATE GateCore.cpp:231-283`). Peers that are not game servers but send monitor messages are
  treated as monitors (`GATE GateCore.cpp:217-223, 710-755`). [V]

## 2. Game -> Gate

| MsgID | Value | Body size | Payload | Game side | Gate side |
|---|---|---|---|---|---|
| `MSGID_REQUEST_REGISTERGAMESERVER` | 0x0512A3F4 | 2.20: 39 + 11*M + 4 = 43 + 11*M; 2.03: 39 + 11*M | 6: name 10; 16: IP 16; 32: port u16; 34: map count M u8; 35: M x map 11; then PID u32 (`GetCurrentProcessId`); then build date u32 (2.20 only) | `SRV Game.cpp:20668-20711` | `GATE GateCore.cpp:451-537`: build date must equal cfg `build-date`; duplicate name -> REJECT; maps skipped unread (476-481); reply built at 539 |
| `MSGID_GAMESERVERALIVE` | 0x12A01002 | 8 | 6: total clients on this server u16 | every 3 s from the main timer (`SRV Game.cpp:3682-3684`), skipped while `m_bIsLogSockAvailable` is FALSE (`SRV Game.cpp:20714-20729`) | stores the count, sets `m_sIsActivated = 1` when it is >= 0 (revives a silent server; works for any peer, registered or not), forwards to monitors (`GATE GateCore.cpp:435-445`) |
| `MSGID_PARTYOPERATION` | 0x3C00123A | 50 (20 used) | 5 | `SRV Game.cpp:20646-20647` | `GATE GateCore.cpp:300-301, 1110-1274`, `GATE PartyManager.cpp` |
| `MSGID_SERVERSTOCKMSG` | 0x3AE90013 | `m_iIndexGSS + 1` | 4 | flushed every 1 s if non-empty (`SRV Game.cpp:20650-20651`) | relayed byte-for-byte to every other registered game server, not parsed (`GATE GateCore.cpp:304-307, 1015-1031`) |
| `MSGID_ITEMLOG` | 0x210A914D | 53 | 47 bytes of item-transfer data | case exists, comment says "not used", no caller (`SRV Game.cpp:20654-20666`) [V by grep] | `ItemLog`, appends to `ItemEvents.log` (`GATE GateCore.cpp:425, 972-1013`) |

The gate replies to registration with a 6-byte `MSGID_RESPONSE_REGISTERGAMESERVER` (0x0512A3F5),
MsgType CONFIRM or REJECT. [V]

## 3. Gate -> Game

Game-side: `OnGateRead` queues the message (`SRV Game.cpp:20613-20624`); `MsgProcess` dispatches it
(`SRV Game.cpp:14885-14968`).

| MsgID | Value | Payload | Game server action |
|---|---|---|---|
| `MSGID_RESPONSE_REGISTERGAMESERVER` | 0x0512A3F5 | MsgType CONFIRM / REJECT | CONFIRM -> gate ready (one of the readiness conditions); REJECT -> log and stop [V] |
| `MSGID_TOTALGAMESERVERCLIENTS` | 0x12A01005 | 6: total users in the world u16 | stored, shown in the status line (`Players(a/b - c/d)`) [V] |
| `MSGID_SERVERSTOCKMSG` | 0x3AE90013 | GSM records (4) | `ServerStockMsgHandler` `SRV Game.cpp:49339-49764` [V] |
| `MSGID_PARTYOPERATION` | 0x3C00123A | party response (5) | `PartyOperationResultHandler` (`SRV Game.cpp:50972`) [V] |
| `MSGID_SENDSERVERSHUTDOWNMSG` | 0x20000000 | 6: code u16 (1 = first notice, 2 = last notice) | broadcast shutdown notice to players and forward `SENDSERVERSHUTDOWNMSG` to the Log server (`SRV Game.cpp:14896-14920`) [V] |
| `MSGID_GAMESERVERSHUTDOWNED` | 0x14000000 | none | shutdown code 1, tell the Log server (`SRV Game.cpp:14921-14937`) [V] |

## 4. Stock messages (GSM records)

Container: `wire-format.md`. Records are appended by `bStockMsgToGateServer` (`SRV Game.cpp:49290-49336`)
and parsed by `ServerStockMsgHandler` (`SRV Game.cpp:49339-49764`). Codes from `SRV NetMessages.h:453-471`.
The gate does not look inside. All sizes exclude the code byte unless stated.

| Code | Name | Payload after the code byte | Receiver action |
|---|---|---|---|
| 0x01 | `GSM_REQUEST_FINDCHARACTER` | server ID u16; client handle u16; target name 10; requester name 10 | if the target is here, answer with 0x02 |
| 0x02 | `GSM_RESPONSE_FINDCHARACTER` | server ID u16; client handle u16; target 10; requester 10; location text 14 | only the server whose random `m_wServerID_GSS` matches tells the requester |
| 0x03 | `GSM_GRANDMAGICRESULT` | map 10; u16 x5; then structure counts (see handler) | crusade meteor result |
| 0x04 | `GSM_GRANDMAGICLAUNCH` | u16 magic type; u16 side | crusade |
| 0x05 | `GSM_COLLECTEDMANA` | u16 Aresden; u16 Elvine | crusade mana totals |
| 0x06 | `GSM_BEGINCRUSADE` | u32 crusade GUID | start crusade locally |
| 0x07 | `GSM_ENDCRUSADE` | winner side + 16 bytes | end crusade |
| 0x08 | `GSM_MIDDLEMAPSTATUS` | count s16; count x {type u8, side u8, x s16, y s16} | middleland structure map |
| 0x09 | `GSM_SETGUILDTELEPORTLOC` | guild GUID i32; x i32; y i32; map 10 | guild teleport point |
| 0x0A | `GSM_CONSTRUCTIONPOINT` | guild GUID i32; points i32 | crusade construction points |
| 0x0B | `GSM_SETGUILDCONSTRUCTLOC` | guild GUID i32; x i32; y i32; map 10 | |
| 0x0C | `GSM_CHATMSG` | type u8 (10 = GM notice, 1 = guild chat); guild GUID i32; sender 10; len u16; text (record 18 + len) | deliver to matching players |
| 0x0D | `GSM_WHISFERMSG` (sic) | target 10; len u16; the raw client chat message | deliver if the target is here |
| 0x0E | `GSM_DISCONNECT` | name 10 | disconnect that character if here (duplicate-login cleanup) |
| 0x0F | `GSM_REQUEST_SUMMONPLAYER` | name 10; map 10; x u16; y u16 | GM summon across servers |
| 0x10 | `GSM_REQUEST_SHUTUPPLAYER` | server ID u16; client handle u16; GM 10; minutes u16; player 10 (27 bytes) | mute |
| 0x11 | `GSM_RESPONSE_SHUTUPPLAYER` | same 27 bytes | confirm to GM |
| 0x12 | `GSM_REQUEST_SETFORCERECALLTIME` | u16 minutes | world-wide force recall time. The case falls into `default`, which sets `bFlag = TRUE` and ends the record loop, so records after it in the same stock message are not processed (`SRV Game.cpp:49744-49754`, loop `:49350`, [GATE-026](../issues/gate.md#gate-026-gsm_request_setforcerecalltime-falls-through-into-default)) [V code] |

[V for codes and leading fields; inner crusade structures follow the handler and are not re-listed]

## 5. Party operations

Request (game -> gate), always 50 bytes sent (`SRV Game.cpp:20627-20648`):

| Off | Size | Field |
|---|---|---|
| 0 | 4 | `MSGID_PARTYOPERATION` |
| 4 | 2 | request type |
| 6 | 2 | client handle on the sender |
| 8 | 10 | character name |
| 18 | 2 | party ID |
| 20 | 30 | zero padding |

The gate stores the sender's index (`m_iIndex`) only for create and add; every reply goes to the **current**
requester (`iClientH`), and the stored index is used only by `GameServerDown` (`PartyManager.cpp:56-62`, `:136-141`;
`GateCore.cpp:1148-1182`).
[V layout / I purpose of the handle: echoed back so the game server finds the client]

| Type | Request | Gate behaviour (`GATE PartyManager.cpp`) |
|---|---|---|
| 1 | create party (name = master) | allocate a party ID (max `DEF_MAXPARTY` 5000), reply type 1 |
| 2 | dissolve party | no-op (`GATE GateCore.cpp:1184-1185`) |
| 3 | add member | join if under 8 members; reply type 4; on success broadcast to all servers |
| 4 | remove member | reply type 6 (dismiss); broadcast on success; party deleted when the count drops to 1 or fewer (`<= 1`, `PartyManager.cpp:167`; type 2 broadcast) |
| 5 | check member | sent at enter-game; if the character is not in the claimed party, reply type 3 (clear) |
| 6 | info / list | reply type 5 with member names |
| 7 | server-change mark | member stays listed for up to 20 s while changing server (`GATE PartyManager.cpp:268-316`, `GATE GateCore.cpp:1269-1271`) |

Responses (gate -> game):

| Type | Meaning | Size sent | Payload after the type u16 (offset 6) |
|---|---|---|---|
| 1 | create result | 22 | result u8 (1 ok, 0 fail); client handle u16; name 10; party ID u16 (`GATE GateCore.cpp:1148-1182`) |
| 2 | party deleted (to all servers) | 10 | party ID u16 (`GATE PartyManager.cpp:93-105`) |
| 3 | not a member, clear | 20 | client handle u16; name 10 (`GATE PartyManager.cpp:190-207`) |
| 4 | join result | 22 | as type 1; on success also broadcast to all servers (`GATE GateCore.cpp:1187-1224`) |
| 5 | party info | 20 + 11*n + 1 | client handle u16; name 10; count u16; n x member name 10 at an 11-byte stride (`GATE PartyManager.cpp:212-245`) |
| 6 | dismiss result | 22 | as type 1; success broadcast (`GATE GateCore.cpp:1224-1258`); also broadcast when a changing member times out (`GATE PartyManager.cpp:279-312`) |

Bytes used are 21 for types 1/4/6 (sent 22) and 18 for type 3 (sent 20); trailing zeros. [V]

Party state is memory only; no file is written (`GATE PartyManager.cpp`). [V]

## 6. Shutdown and monitor messages (gate side only)

| MsgID | Value | From | Effect |
|---|---|---|---|
| `MSGID_REQUEST_GAMESERVER_SHUTDOWN` | 0x3AE8670A | monitor | MsgType must be 0x5A8E and a fixed 10-byte passphrase must follow; shuts down one server |
| `MSGID_REQUEST_ALLGAMESERVER_SHUTDOWN` | 0x3AE8770A | monitor | same check; starts the timed world shutdown |
| `MSGID_REQUEST_ALL_SERVER_REBOOT`, `..._EXEC_1DOTBAT`, `..._EXEC_2DOTBAT` | 0x3AE8270A, 0x3AE8370A, 0x3AE8470A | monitor | MsgType must be 0x5A8E; forwarded unchanged to every monitor (a remote-agent tool on each host would act) |
| `MSGID_GATEWAY_CURRENTCONN` | 0x3B000000 | any | forwarded to monitors |
| `MSGID_MONITORALIVE` | 0x3AE8570A | monitor | MsgType = monitor slot 0..9; flips the slot status 1/2 (drawn as X/O); answers all monitors with "Total User n" |
| `MSGID_GAMESERVERDOWN` | 0x12A01004 | gate -> monitors | a game server's socket closed, a send failed, or no alive for 30 s |

`GATE GateCore.cpp:308-449`, IDs `GATE NetMessages.h:308, 383-396`. `MSGID_ITEMLOG` is 0x210A914D
(`GATE NetMessages.h:348`). A "monitor" is simply any accepted peer that never registered as a game
server (`m_bIsGameServer == FALSE`, `GATE GateCore.cpp:710-786`); it receives every game server's
alive notice every 3 s and every down notice. The passphrase is in the source and is deliberately not copied here.
Game-server status notices to monitors are 28 bytes: MsgID, type, server name 20, total u16
(`GATE GateCore.cpp:710-755`). [V]

World shutdown by the operator (F1+F4 on the gate window, or `ALLGAMESERVER_SHUTDOWN`): the counter
starts at 10 and drops every 20 s; counts 9..3 send `SENDSERVERSHUTDOWNMSG` code 1, count 2 sends code
2, count 1 sends `GAMESERVERSHUTDOWNED` - about 3 minutes in all (`GATE GateCore.cpp:626-639,
1033-1106`). F1+F12 sends `GAMESERVERSHUTDOWNED` to every server at once (`GATE GateCore.cpp:864-909`). [V]

## 7. Failure behaviour

- Gate socket lost: the game server reconnects; a counter rises every 3 s; above 20 (about a minute)
  it shuts down with code 4 (`SRV Game.cpp:43099-43121`). [V]
- No `GAMESERVERALIVE` for 30 s: the gate only logs and notifies monitors; it does not drop the
  server (`CheckGameServerActivity`). [V]
- `GAMESERVERALIVE` is sent only while the Log server link is up, so a game server that loses the Log
  server also goes quiet on the gate. [V]

## 8. 2.19 gate vs 2.20 game server

- Registration: 2.20 appends PID and build date; the 2.19 gate reads both and checks the build date
  (`GATE GateCore.cpp:451-520`). So the 2.19 gate already expects the 2.20 format, and a 2.03 game
  server would fail its build-date check. [V]
- Build date: the gate repo cfg has 612; the 2.20 game server sends `DEF_BUILDDATE` (defined in `Version.h`,
  included from `Game.h:37`, not `GlobalDef.h`) for its build flags (China 429, Korea 207, USA 222, InterServer 306, others 213). Set the gate cfg to match. [V]
- GSM 0x0D-0x12 are missing from the 2.19 gate's `NetMessages.h` but pass through unchanged because
  the gate does not parse stock messages. [V]
- Party ops exist on both sides with the same layouts. [V]

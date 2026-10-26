# Gate server 2.19: data structures and limits

Classes, fields, constants and wire layouts of the official Gate server. Tags and citation form as in `functions.md`
(`GATE` = `sources/gateserver-librehelbreath` @ `0b994cd`; [V] verified, [I] inferred, [U] unverified).
Sizes of structs assume a 32-bit MSVC build with default packing [I]; the field order is [V].

## Contents
1. Constants and limits
2. CGateCore
3. CClient
4. PartyManager
5. XSocket
6. Globals (Wmain.cpp)
7. Wire layouts the gate reads and writes
8. Message IDs the gate uses

## 1. Constants and limits

| Name | Value | Source | Meaning and effective limit |
|---|---|---|---|
| `DEF_MAXCLIENTS` | 500 | `GateCore.h:26` | size of `m_pClientList`; slot 0 never used -> **499** peers (`GateCore.cpp:238`); slot 399 shares its message number with the timer ([NET-008](../issues/network.md#net-008-gate-peer-table-slot-0-unused-and-slot-399-collides-with-the-timer-message)) |
| `DEF_MAXGAMESERVERS` | 20 | `GateCore.h:27` | unused anywhere ([GATE-023](../issues/gate.md#gate-023-unused-constants-globals-and-functions)) [V by grep] |
| `DEF_NORESPONSELIMIT` | 30000 ms | `GateCore.h:28` | silence limit for a game server |
| `DEF_MAXMONITORS` | 10 | `GateCore.h:29` | size of `m_cMonitorStatus`; used only as display slots, not a peer limit; index is unchecked |
| allow-list entries | 10 x 16 bytes | `GateCore.h:71` | `m_cAddress[10][16]`; the loader has no bound |
| `DEF_MSGBUFFERSIZE` | 30000 | `Client.h:17` | max body per message, send and receive (+8 bytes slack per buffer) |
| `DEF_XSOCKBLOCKLIMIT` | 300 | `XSocket.h:50` | unsent-message ring size (array size and the `iBlockLimit` passed everywhere) |
| `DEF_MAXPARTY` | 5000 | `PartyManager.h:19` | size of both party tables; usable 1..4999 for ids **and** for member slots |
| `DEF_MAXPARTYMEMBER` | 8 | `PartyManager.h:20` | members per party |
| shutdown count start | 10 | `GateCore.cpp:26, 630` | countdown steps (nine are taken) |
| shutdown step gate | 20 000 ms | `GateCore.cpp:1050` | min interval between countdown steps |
| party change timeout | 20 000 ms | `PartyManager.cpp:291` | server-change stamp age before removal |
| party check gate | 2000 ms | `PartyManager.cpp:286` | min interval of the stamp scan |
| timer period | 3000 ms | `Wmain.cpp:238` | main tick |
| user-count log | 61 s | `Wmain.cpp:54` | `iCurrentUserTime > 60` with a 1 s sleep |
| log ring | 50 lines x 120 bytes | `Wmain.cpp:30` | window log; 35 lines painted |
| authentication constants | `0x5A8E`, `"1dkld$#@01"` | `GateCore.cpp:311, 318, 368, 376, 395` | MsgType and passphrase of the control messages |
| memory per peer | about 60 KB | `Client.cpp:14-15` | two 30008-byte buffers plus the unsent arrays; 499 peers is about 30 MB [derived] |

## 2. CGateCore (`GateCore.h:31-80`)

One instance (`G_pGateCore`, `Wmain.cpp:35`).

| Field | Type | Meaning | Init |
|---|---|---|---|
| `m_hWnd` | HWND | main window, used for `InvalidateRect` and to create sockets | ctor (23) |
| `m_cGateServerAddr` | char[16] | listen address | zeroed in `bReadProgramConfigFile` (88) |
| `m_iGateServerPort` | int | listen port | **uninitialised** unless the cfg key is present |
| `m_bF1pressed`, `m_bF12pressed` | BOOL | held-key flags | ctor (24) |
| `m_bIsAutoShutdownProcess` | BOOL | recomputed every tick: any flagged game server exists | **uninitialised** until the first `CheckAutoShutdownProcess` (1042) |
| `m_iAutoShutdownCount` | int | countdown, start 10, floor 0 | ctor (26); reset only by F1+F4 |
| `m_dwAutoShutdownTime` | DWORD | time of the last countdown step | **uninitialised** until F1+F4 |
| `m_cAddress` | char[10][16] | allow-list IP strings | zeroed (37) |
| `m_iBuildDate` | int | required build date from the cfg | 0 (29) |
| `iTotalWorldServerClients` | int | sum of game-server user counts, refreshed per tick | 0 (39) |
| `m_cMonitorStatus` | char[10] | per-slot heartbeat display state 0 / 1 / 2 | zeroed (34-35) |
| `m_pClientList` | CClient*[500] | the peer table; NULL = free | NULL (31-32) |
| `m_pPartyManager` | PartyManager* | owned | `new` (41) |

Declaration order (relevant for the overflow of the allow-list loader, `GateCore.h:60-79`): `m_hWnd`, address, port, key
flags, shutdown fields, `m_cAddress`, `m_iBuildDate`, `iTotalWorldServerClients`, `m_cMonitorStatus`, `m_pClientList`,
`m_pPartyManager`. [V]

## 3. CClient (`Client.h:19-37`)

One per accepted connection, a game server or a monitor.

| Field | Type | Meaning |
|---|---|---|
| `m_bIsGameServer` | BOOL | FALSE = monitor/unregistered; TRUE from the first registration attempt, accepted or not (`GateCore.cpp:494`) |
| `m_bIsShutDown` | BOOL | flagged for the timed shutdown; cleared only by deletion |
| `m_cName` | char[11] | 10-character game-server name, NUL-terminated; empty until registration |
| `m_cAddress` | char[16] | peer IP text from `getpeername`; the field right after `m_cName` |
| `m_dwProcessHandle` | DWORD | the PID from the registration (logged as "Hd:"), otherwise unused |
| `m_dwAliveTime` | DWORD | `timeGetTime()` of accept or of the last `GAMESERVERALIVE`; uninitialised until `bAccept` sets it |
| `m_iTotalClients` | int | user count from the last alive message; 0 at start |
| `m_sIsActivated` | short | 1 active, 0 silent >= 30 s, -1 build-date mismatch, -2 duplicate name; initial 0 |
| `m_pXSock` | XSocket* | owned; deleting the client closes the connection |

Table semantics: slot number = array index = the number embedded in the socket message; it is reused as soon as a peer
is deleted. `PartyManager` stores slot numbers of game servers (`m_iIndex`), so a reused slot can be mistaken for a
dead server's (inferred effect, verified mechanism). [V/I]

## 4. PartyManager (`PartyManager.h:23-48`)

| Field | Type | Meaning |
|---|---|---|
| `m_iMemberNumList[5000]` | int[] | member count per party id (id = index, 1..4999); 0 = id free |
| `m_stMemberNameList[5000]` | struct[] | pool of member slots (1..4999): `m_iPartyID` (0 = free slot), `m_iIndex` (game-server slot of the creating/adding server), `m_cName[12]` (10 used), `m_dwServerChangeTime` (0 = no pending crossing) |
| `m_pGateCore` | CGateCore* | back pointer (for broadcasts and the client table) |
| `m_dwCheckMemberActTime` | DWORD | last stamp scan (2 s gate) |

The party id is not stored in a party record: a party is "the set of slots with that `m_iPartyID`" and its count in
`m_iMemberNumList`. The two can drift (see `GameServerDown` and the id leak in `iCreateNewParty`). [V]

`m_iMemberNumList` is indexed by the wire id (u16) in `bAddMember` (119, 130, 143), `bDeleteParty` (82) and
`GameServerDown` (260) without a range test; only ids that the gate itself issued are in 1..4999. [V]

## 5. XSocket (`XSocket.h:52-101`)

| Field | Type | Meaning |
|---|---|---|
| `m_cType` | char | 0 none, 1 listen, 2 normal, 3 shut down |
| `m_pRcvBuffer`, `m_pSndBuffer` | char* | allocated by `bInitBufferSize`, size + 8 |
| `m_dwBufferSize` | DWORD | max body (30000 for peers) |
| `m_Sock` | SOCKET | the socket |
| `m_cStatus` | char | 11 reading header, 12 reading body |
| `m_dwReadSize`, `m_dwTotalReadSize` | DWORD | bytes still due / already read in the current phase |
| `m_pAddr[30]`, `m_iPortNum` | | remembered connect target (unused by the gate) |
| `m_pUnsentDataList[300]`, `m_iUnsentDataSize[300]`, `m_sHead`, `m_sTail` | | ring of queued messages |
| `m_uiMsg`, `m_hWnd`, `m_iBlockLimit` | | window message for this socket, window, ring size (300) |
| `m_WSAErr`, `m_bIsAvailable`, `m_bIsWriteEnabled` | | last Winsock error, connect flag (unused), set by the first `FD_WRITE` (sends before that are queued) |

Frame (both directions): byte 0 key (the gate sends 0; non-zero on receive is decrypted), bytes 1-2 u16 total length
including the 3 header bytes, then the body (`XSocket.cpp:416-454, 171-256`). Max total 30003. [V]
`DEF_XSOCKEVENT_*` result codes are -121..-136 (`XSocket.h:33-48`); `ONREAD`, `MISMATCH`, `UNSENT*` and `UNKNOWN` are
ignored by the gate. [V]

## 6. Globals (`Wmain.cpp:26-40`)

| Name | Type | Use |
|---|---|---|
| `G_hWnd` | HWND | main window |
| `G_cMsgList[120*50]`, `G_cMsgUpdated` | char[], BOOL | window log ring and repaint flag |
| `G_cTxt[120]` | char[] | shared scratch for log lines (main thread) |
| `G_pListenSock` | XSocket* | the listener |
| `G_pGateCore` | CGateCore* | the core |
| `G_mmTimer0` | MMRESULT | timer id compared in `WndProc` |
| `G_iQuitProgramCount` | int | unused ([GATE-023](../issues/gate.md#gate-023-unused-constants-globals-and-functions)) [V by grep] |
| `G_bThreadFlag` | BOOL | stop flag of the logging thread |
| `szAppClass[32]` | char[] | window class name `Gate-Server<hInstance>` |

Messages: `WM_USER_TIMERSIGNAL = WM_USER + 500` (Wmain.cpp:26), `WM_USER_ACCEPT = WM_USER + 100`,
`WM_ONCLIENTSOCKETEVENT = WM_USER + 101` (`UserMessages.h:2-3`); the first and last macros have no parentheses. [V]

## 7. Wire layouts the gate reads and writes

Offsets are body offsets; all integers little-endian; strings are fixed-width, NUL padded. Layouts are those the gate
code actually parses or builds; the sender-side view is in `../servers/game-gate-protocol.md`.

**In: `MSGID_REQUEST_REGISTERGAMESERVER`** (`GateCore.cpp:465-490`): 6 name[10]; 16 address[16] (log only); 32 port u16
(ignored); 34 map count (signed char); 35 maps, 11 bytes each (skipped); then PID u32, build date u32.

**In: `MSGID_GAMESERVERALIVE`** (`435-446`): 4 MsgType (ignored); 6 total clients u16 (`cp = pData + DEF_INDEX2_MSGTYPE + 2`
= offset 6, `GateCore.cpp:438-440`). [V]

**In: `MSGID_PARTYOPERATION`** (`1118-1134`, body 20): 4 request type u16; 6 `iGSCH` u16; 8 name[10]; 18 party id u16.

**In: `MSGID_ITEMLOG`** (`982-1005`, body 53): 6 giver[10]; 16 receiver[10]; 26 item name[20]; 46 item type (signed char);
47 v1 i16; 49 v2 i16; 51 v3 i16.

**In: `MSGID_MONITORALIVE`** (`409-423`): 4 MsgType u16 = monitor slot index 0..9 (unchecked).

**In: shutdown requests** (`308-386`): 4 MsgType u16 = 0x5A8E; 6 passphrase[10] = `1dkld$#@01`; for the one-server
request 16 server name (NUL-terminated string, unbounded). Reboot and batch requests: MsgType 0x5A8E only, then forwarded raw.

**Out: 6-byte replies** `MSGID_RESPONSE_REGISTERGAMESERVER` and `MSGID_GAMESERVERSHUTDOWNED` (MsgType `0x0F14` CONFIRM
or `0x0F15` REJECT; the shutdown message is always CONFIRM).

**Out: 8-byte** `MSGID_TOTALGAMESERVERCLIENTS` (6 total u16) and `MSGID_SENDSERVERSHUTDOWNMSG` (6 code u16: 1 or 2),
MsgType CONFIRM.

**Out: monitor message, 28 bytes**: 4 MsgType (CONFIRM, or the monitor's own MsgType for the heartbeat echo); 6 name[20];
26 count i16 (sent only for `GAMESERVERALIVE`; 0 otherwise). For a game-server name taken from `m_cName` the bytes
after the 11th are the start of `m_cAddress` [I]; the heartbeat echo carries `"Total User n"`.

**Out: party messages** (all begin `MSGID_PARTYOPERATION` at 0; "sent" = the size passed to `iSendMsg`, "written" = bytes
actually filled, the rest zero from `ZeroMemory`):

| Code (offset 4, u16) | Meaning | Layout after offset 6 | Written | Sent | To |
|---|---|---|---|---|---|
| 1 | create reply | 6 result u8; 7 `iGSCH` u16; 9 name[10]; 19 party id u16 | 21 | 22 | requester |
| 2 | party disbanded | 6 party id u16 | 8 | 10 | all game servers |
| 3 | not a member | 6 `iGSCH` u16; 8 name[10] | 18 | 20 | requester |
| 4 | add reply / notice | as code 1 | 21 | 22 | requester (fail) or all (ok) |
| 5 | party info | 6 `iGSCH` u16; 8 name[10]; 18 count u16; 20 + 11*k name[10] | 20 + 11n - 1 | 20 + 11n + 1 | requester |
| 6 | remove reply / notice | as code 1 (timeout notice has `iGSCH` = 0) | 21 | 22 | requester (fail) or all |

## 8. Message IDs the gate uses

Values from `GATE NetMessages.h`; names refer to the shared message catalogue. The gate never references any `GSM_*`
sub-type (`NetMessages.h:400-411`) and never handles the many other IDs defined in that header (it is a copy of the
game-wide header).

| MsgID | Value | Direction | Source line |
|---|---|---|---|
| `MSGID_REQUEST_REGISTERGAMESERVER` | 0x0512A3F4 | in | NetMessages.h:227 |
| `MSGID_RESPONSE_REGISTERGAMESERVER` | 0x0512A3F5 | out | :228 |
| `MSGID_GAMESERVERALIVE` | 0x12A01002 | in, out (monitors) | :305 |
| `MSGID_GAMESERVERDOWN` | 0x12A01004 | out (monitors) | :308 |
| `MSGID_TOTALGAMESERVERCLIENTS` | 0x12A01005 | out | :309 |
| `MSGID_GAMESERVERSHUTDOWNED` | 0x14000000 | out | :322 |
| `MSGID_SENDSERVERSHUTDOWNMSG` | 0x20000000 | out | :347 |
| `MSGID_ITEMLOG` | 0x210A914D | in | :348 |
| `MSGID_REQUEST_ALL_SERVER_REBOOT` | 0x3AE8270A | in -> monitors | :383 |
| `MSGID_REQUEST_EXEC_1DOTBAT` | 0x3AE8370A | in -> monitors | :384 |
| `MSGID_REQUEST_EXEC_2DOTBAT` | 0x3AE8470A | in -> monitors | :385 |
| `MSGID_MONITORALIVE` | 0x3AE8570A | in, echoed | :386 |
| `MSGID_REQUEST_GAMESERVER_SHUTDOWN` | 0x3AE8670A | in | :389 |
| `MSGID_REQUEST_ALLGAMESERVER_SHUTDOWN` | 0x3AE8770A | in | :390 |
| `MSGID_SERVERSTOCKMSG` | 0x3AE90013 | in -> game servers | :395 |
| `MSGID_GATEWAY_CURRENTCONN` | 0x3B000000 | in -> monitors | :396 |
| `MSGID_PARTYOPERATION` | 0x3C00123A | in, out | :413 |
| `DEF_MSGTYPE_CONFIRM` / `REJECT` | 0x0F14 / 0x0F15 | out | :4-5 |

Three pairs of IDs in this header share a value, none among the IDs the gate dispatches ([GATE-024](../issues/gate.md#gate-024-header-message-ids-collide); see `../shared/messages/README.md`). [V by reading the header]

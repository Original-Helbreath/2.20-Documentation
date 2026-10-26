# Wire format (XSocket)

All four programs (client, HGserver, gate, Log) use the same `XSocket` class over TCP. The HGserver
2.20 and 2.03 copies are identical (`git diff` empty); the client and gate copies differ only in
send queuing and key handling, noted below. Tags as in `topology.md`.

## Frame

```
offset size  field
0      1     key       (u8)  0 = plain, otherwise cipher key for the body
1      2     length    (u16 LE) total frame length INCLUDING these 3 header bytes
3      n     body      n = length - 3
```

- Sender: `SRV XSocket.cpp:414-446` (`m_pSndBuffer[0] = cKey; *(WORD*)(buf+1) = dwSize + 3`). [V]
- Receiver: reads exactly 3 header bytes, then `length - 3` body bytes (`SRV XSocket.cpp:174-253`). A
  length of 3 (empty body) is a complete message. A body larger than the socket buffer returns
  `DEF_XSOCKEVENT_MSGSIZETOOLARGE` and the peer is dropped (`SRV XSocket.cpp:211-216`; callers at
  `SRV Game.cpp:4170-4176`, `GATE GateCore.cpp:207-227`). [V]
- Buffer size is `DEF_MSGBUFFERSIZE = 30000` on HGserver and gate (`SRV Client.h:24`,
  `GATE Client.h:17`) and 30000 on the client (`CLI Game.cpp:15358`). So **the body must be <= 30000
  bytes** and the length field fits a u16. [V]
- There is no magic number, checksum, sequence number or handshake. The header comment mentions a
  "confirm code" (`SRV XSocket.h:5`) but no code checks one. [V]

## Body cipher

Applied to the body only, when key != 0. Byte-wise, n = body length, i = 0..n-1, all arithmetic mod 256:

```
encrypt: b[i] = b[i] + (i XOR key);  b[i] = b[i] XOR (key XOR (n - i))
decrypt: b[i] = b[i] XOR (key XOR (n - i));  b[i] = b[i] - (i XOR key)
```

`SRV XSocket.cpp:435-440` (encrypt), `SRV XSocket.cpp:554-559` (decrypt). The C code uses signed
`char` and `int`; only the low 8 bits matter, so a byte implementation with `& 0xFF` on `i`, `n - i`
and `key` is exact. [V, the mod-256 equivalence is derived]

Who uses a key:
- Client -> any server: random key `(rand() % 255) + 1`, never 0, new per message
  (`CLI Game.cpp:1445`), passed to `iSendMsg` for log and game messages (`CLI Game.cpp:1535-1803, 1903`).
  A few game messages are sent without a key (`CLI Game.cpp:1852`). [V]
- HGserver -> anyone: key 0 (every `iSendMsg(buf, size)` call omits it, for example
  `SRV Game.cpp:4266, 4831, 20647`). [V]
- Gate -> anyone: key forced to 0 (`GATE XSocket.cpp:421-422`). [V]
- Log server -> client / game server: [I] key 0. The client decrypts whatever key arrives
  (`CLI Game.cpp:14743` calls `pGetRcvDataPointer`, which decrypts if key != 0), so 0 is safe.

A replacement must therefore **decrypt on receive** (clients always encrypt) and may send plain.

## Body layout

```
offset size  field
0      4     MsgID   (u32 LE)  DEF_INDEX4_MSGID  = 0
4      2     MsgType (u16 LE)  DEF_INDEX2_MSGTYPE = 4
6      ...   payload
```

`SRV MessageIndex.h:2-3`, `GATE MessageIndex.h:2-3`. [V]

- MsgID values: `SRV NetMessages.h`. MsgType is a sub-code: `DEF_MSGTYPE_CONFIRM 0x0F14`,
  `DEF_MSGTYPE_REJECT 0x0F15` (`SRV NetMessages.h:4-5`), a `DEF_LOGRESMSGTYPE_*` result
  (`SRV NetMessages.h:293-321`), a request kind (party ops, enter-game kind), or 0. [V]
- Multi-byte fields are written through pointer casts on x86 (`*(WORD*)cp = ...`), so they are
  **little-endian and unaligned, with no padding**. [V]
- Strings are fixed width (names 10, passwords 10, guild names 20, map names 10, addresses 15 or 16,
  item names 20, world name 30), copied with `memcpy`, zero-padded, and **not NUL-terminated when
  full** (a 10-character name fills all 10 bytes). Readers copy into an 11-byte zeroed buffer
  (`SRV Game.cpp:1512-1550`). [V]
- Declared sizes sometimes exceed the bytes written (zero padding follows), for example
  `MSGID_REQUEST_CREATENEWGUILD` writes 70 bytes but sends 75 (`SRV Game.cpp:4592-4597`), party ops
  write 20 and send 50 (`SRV Game.cpp:20647`). Readers must accept trailing zeros. [V]

## Connection behaviour

- All sockets are non-blocking with `WSAAsyncSelect`; a full send queue (300 blocks,
  `DEF_XSOCKBLOCKLIMIT`, `SRV XSocket.h:51`) is treated as a dead peer (`SRV Game.cpp:4757-4789`). [V]
- On a failed connect the socket retries forever (`SRV XSocket.cpp:85-91`). [V]
- Client-side log connections are one request per TCP connection: the client opens a socket,
  sends one request when connected (`CLI Game.cpp:3041-3079`), and deletes the socket after the first
  response (`CLI Game.cpp:15446-15448`). [V]
- The client's send path queues until `FD_CONNECT` (`m_bIsWriteEnabled`, client-only change, also in
  the gate copy: `GATE XSocket.cpp` vs `SRV XSocket.cpp` diff). No wire effect. [V]

## Gate stock-message container

`MSGID_SERVERSTOCKMSG` (0x3AE90013) carries several small records:
`[MsgID u32][MsgType u16 = 0x0F14][record][record]...[0x00]`. Each record starts with a one-byte
`GSM_*` code; a zero byte (or any unknown code) ends the list. Built in `SRV Game.cpp:851-860,
49290-49336`, sent with `m_iIndexGSS + 1` bytes so the trailing 0 is included (`SRV Game.cpp:20650-20651`),
parsed in `SRV Game.cpp:49339-49764`. Record layouts: `game-gate-protocol.md`. [V]

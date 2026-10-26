# Official World Log Server (WorldServer.exe, linked 2002-11-29)

Static map of the Siementech World Log Server (WLS) binary in `.claude\scratch\v382core\WorldServer.exe`
(106 496 bytes, window title "Helbreath World-Log-Server V2.15", format string at `0x418b3c` with
arguments 2, 15 at `0x40c0d7`). The binary was never run. Analysis: Ghidra 12.1.4 decompiler (this
program is fully analysed; 51 handlers renamed in the Ghidra project) plus a capstone disassembly for
cross-checks. All addresses are virtual addresses; file offset = VA - 0x400000.

Tags: **[V]** seen in the decompile or disassembly at the cited address; **[I]** inferred from
structure, names or the 2.20 sources. Offsets in message tables are from the start of the message body
(0 = MsgID u32, 4 = MsgType u16, 6 = first payload byte), as in `wire-format.md`.

## Contents
1. Process model, sockets and config
2. Message queue and dispatcher
3. Messages from game servers (internal port)
4. Messages from clients (world port)
5. Messages to and from the Main Log Server
6. Account connection-state machine
7. Files read and written
8. Ghidra function names

## 1. Process model, sockets and config

- Win32 window app; WinSock 2 via `WS2_32.dll` ordinals; all sockets are the usual `XSocket` class
  (`WSAAsyncSelect`, 3-byte header, byte cipher), the same framing as `wire-format.md` [V: socket
  class at `0x40c7f0`, send `0x40cfd0`, receive `0x40c950`].
- Config `WLserver.cfg` read by `ReadWLServerConfig` `0x401610` [V]. Keys and storage:

| Key | Stored at (this+) | Use |
|---|---|---|
| `world-server-name` | +0x08, 30 bytes | sent to the MLS in every world message |
| `world-server-address` | +0x27, 16 | sent to the MLS in registration |
| `world-server-port` | +0x3c | client listen port (opened on HOME key) |
| `internal-world-server-port` | +0x40 | game-server listen port (opened at start) |
| `backup-drive-letter` | +0x4c | printed, no other use found [I] |
| `main-log-server-address` | +0x4d, 16 | MLS connect target |
| `main-log-server-port` | +0x64 | MLS connect port |
| `game-server-list` | +0x68, up to 10 x 16 | IP allow-list for the internal port |

- **Pack config evidence** [V cfg]: `.claude/scratch/v382core/WLserver.cfg:9-10` has `main-log-server-address = 195.154.77.227`
  and `main-log-server-port = 2981`, the MLS **public** `log-server-port` (`HMLServer.cfg:3`); `internal-world-server-port`
  is 3840 (`WLserver.cfg:7`), equal to the MLS internal port on the same IP. The 2.20 sample `SRV WLserver.cfg:11` says 4500.
  **Answered [V]:** the start-up connect dials exactly `main-log-server-port` (this+0x64; `ReadWLServerConfig` 0x401938 region,
  `InitializeWorldServer` 0x4014e0 (connect loop at 0x401590) -> `FUN_0040ca50` -> `connect`, reconnect `SendMsgToMLS` 0x4041dd), so 2981 in the pack and
  never 4500 (`research/review/round2/wls-port.md`). Unverified [U]: MLS acceptance on a public socket (runtime test).
- Start-up (`InitializeApplication` `0x40c1c0`, `InitializeWorldServer` `0x4014e0`) [V]:
  1. read the config;
  2. open **10 outgoing sockets to the MLS** (`main-log-server-address:port`), one every 200 ms,
     window messages 0x464..0x46D;
  3. listen on `internal-world-server-port` (window message 0x446) for game servers.
- On the HOME key (`HandleKeyUpActivate` `0x40c550`, `WM_KEYUP` with VK 0x24): listen on
  `world-server-port` (window message 0x43c) for clients, then send `MSGID_WORLDSERVERACTIVATED` to the
  MLS [V]. Only one client port is opened; the README's "WLS listens on 3 ports" describes the MLS
  (see `official-mainserver.md`) [V].
- Accept (`HandleAcceptSocket` `0x4019e0`) [V]: up to 9 999 accepted sockets share one table
  (this+0x10c). A socket accepted on the internal port is closed unless its peer IP equals one of the
  `game-server-list` entries (16-byte compare, 10 entries); client-port sockets are not filtered.
- Window-message ranges: 0x464..0x46D = MLS sockets (`HandleMainLogSocketEvent` `0x401d40`),
  0x52D..0x2C3B = accepted sockets (message is `slot + 0x52C`, slot 1..9999; 0x52C is never used) (`HandleClientSocketEvent` `0x401bd0`) [V, `0x40beb0`].

## 2. Message queue and dispatcher

Every received message (from any accepted socket, source 1, or from an MLS socket, source 2) is copied
into a 10 000-entry ring queue (`EnqueueMsgFromSocket` `0x401ff0`) and processed later by
`ProcessMsgQueue_WLS` `0x4021b0` (posted as window message 0x432) [V]. **The dispatcher switches on the
MsgID only; it does not look at the source**, so any peer may send any of these IDs [V]:

| MsgID | Name (2.20 header) | Handler | Normal sender |
|---|---|---|---|
| 0x0512A3E3 | *not in 2.20 headers* (game-server registration) | `HandleRegisterGameServer` `0x402510` | game server |
| 0x0C152210 | `MSGID_REQUEST_PLAYERDATA` | `HandleRequestPlayerData` `0x403640` | game server |
| 0x0DF3074F | `MSGID_REQUEST_SAVEPLAYERDATALOGOUT` | `HandleSavePlayerData(logout=1, reply=0)` `0x404e80` | game server |
| 0x0DF30750 | `MSGID_REQUEST_NOSAVELOGOUT` | `HandleNoSaveLogout` `0x405780` | game server |
| 0x0DF3076F | `MSGID_REQUEST_SAVEPLAYERDATA` | `HandleSavePlayerData(0, 0)` | game server |
| 0x0DF30770 | `MSGID_REQUEST_SAVEPLAYERDATA_REPLY` | `HandleSavePlayerData(1, 1)` | game server |
| 0x0FC94204 | `MSGID_REQUEST_CREATENEWCHARACTER` | `HandleCreateNewCharacter` `0x405850` | MLS (relayed) |
| 0x0FC94205 | `MSGID_REQUEST_ENTERGAME` | `HandleRequestEnterGame` `0x405440` | client |
| 0x0FC94207 | `MSGID_REQUEST_DELETECHARACTER` | `HandleDeleteCharacter` `0x407cc0` | MLS (relayed) |
| 0x0FC94208 | `MSGID_REQUEST_CREATENEWGUILD` | `HandleCreateNewGuild` `0x409170` | game server |
| 0x0FC9420A | `MSGID_REQUEST_DISBANDGUILD` | `HandleDisbandGuild` `0x409940` | game server |
| 0x0FC9420C | `MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN` | `HandleGuildAddGuildsman` `0x409d30` | game server |
| 0x0FC9420D | `MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN` | `HandleGuildDelGuildsman` `0x409fa0` | game server |
| 0x12A01006 | `MSGID_ENTERGAMECONFIRM` | `HandleEnterGameConfirm` `0x4033f0` | game server |
| 0x14000000 | `MSGID_GAMESERVERSHUTDOWNED` | `HandleGameServerShutdowned` `0x4033c0` | game server |
| 0x15000000 | `MSGID_REQUEST_FORCEDISCONECTACCOUNT` | `HandleForceDisconnectAccount` `0x40ab30` | MLS |
| 0x167C0A31 / 32 | `MSGID_REQUEST_SAVEARESDEN/ELVINEOCCUPYFLAGDATA` | `HandleSaveOccupyFlagData(1/2)` `0x408f80` | game server (not the 2.20 one: it never sends these, `SRV Game.cpp:3762-3764, 4424-4468, 47997`) |
| 0x20000000 | `MSGID_SENDSERVERSHUTDOWNMSG` | `SendServerShutdownToMLS` `0x40b410` | game server |
| 0x210A914E | `MSGID_GAMEMASTERLOG` | `WriteGameMasterLog` `0x40b470` | game server |
| 0x210A914F | `MSGID_GAMEITEMLOG` | `WriteGameItemLog` `0x40b6a0` | game server |
| 0x210A9150 | `MSGID_GAMECRUSADELOG` | `WriteCrusadeLog` `0x40b8d0` | game server |
| 0x23AB2200 | `MSGID_REQUEST_CHARINFOLIST` | `HandleRequestCharInfoList` `0x404370` | MLS |
| 0x25000198 | `MSGID_REQUEST_SETACCOUNTINITSTATUS` | `HandleMLSInitStatusMsg` `0x40a9a0` | MLS |
| 0x25000199 | `MSGID_REQUEST_SETACCOUNTWAITSTATUS` | `HandleGSWaitStatusMsg` `0x40adf0` | game server |
| 0x280120A0 | `MSGID_RESPONSE_REGISTER_WORLDSERVERSOCKET` | `HandleRegisterWorldServerSocketResponse` `0x40b3e0` | MLS |

Unknown IDs are dropped silently [V]. Replies to game servers and clients are sent with key 0
(unencrypted, `0x40cfd0(..., 0)`); messages to the MLS use a random key (`SendMsgToMLS` `0x403fd0`
calls `rand` `0x40db1b`) [V].

## 3. Messages from game servers (internal port)

All [V] at the handler cited unless marked.

### 3.1 Registration 0x0512A3E3 -> 0x0512A3F5

Request layout (`0x402510`): 6 server name 10; 16 address 16 (text); 32 port u16; 34 map count M
(signed byte); 35 M x map name 11. This is exactly the 2.20 `MSGID_REQUEST_REGISTERGAMESERVER` layout
(`game-log-protocol.md` 3), **only the MsgID differs** (2.20 sends 0x0512A3F4).

Processing order:
1. reject (REJECT) if the server name is already registered or 100 servers exist;
2. register each map (`RegisterMap` `0x402a80`, max 100 maps; duplicate map -> REJECT);
3. push config files in this **call order** in `HandleRegisterGameServer` (each as its own message via
   `SendConfigFileToGameServer` `0x402be0`, IDs assigned at `0x402ffe-0x40304e`): `Npc.cfg` 0x0FA314DA,
   `Item.cfg`, `Item2.cfg` (0x0FA314D9 each), `Magic.cfg` 0x0FA314DB, `Skill.cfg` 0x0FA314DC, `Item3.cfg`
   0x0FA314D9, `Quest.cfg` 0x0FA40001, `BuildItem.cfg` 0x0FA40002, `DupItemID.cfg` 0x0FA40003,
   `Noticement.txt` 0x0FA40004, `Potion.cfg` 0x0FA314DE; the eleven sends are `&&`-chained, so any failure
   (including Potion.cfg) -> REJECT. Then `GameData\AresdenOccupyFlag.txt` 0x17081034 and
   `GameData\ElvineOccupyFlag.txt` 0x17081035 (failure ignored) [V decompile of `0x402510`: the filename
   strings are at VA 0x4175A4 Npc, 0x417598 Item, 0x41758C Item2, 0x417568 Item3, 0x417520 Potion; this
   matches `decomp/WorldServer/HandleRegisterGameServer.c`. The filename table inside
   `SendConfigFileToGameServer` (`0x402c08-0x402f14`) has a different order and is not the call order;
   ID per file I, by matching the order of the two lists];
4. reply `MSGID_RESPONSE_REGISTERGAMESERVER` 0x0512A3F5, MsgType 0x0F14 (CONFIRM) or 0x0F15 (REJECT),
   6-byte body (`SendRegisterGameServerResponse` `0x4029c0`);
5. forward the server to the MLS as `MSGID_REGISTER_WORLDSERVER_GAMESERVER` (section 5).

So the configs come **before** the CONFIRM (confirms the order inferred in `game-log-protocol.md` 2).
Note: the Magic.cfg push ID 0x0FA314DB is also `MSGID_EVENT_COMMON` and the Skill.cfg push ID 0x0FA314DC is
also `MSGID_COMMAND_COMMON` in the client/game headers (`../shared/messages/README.md`).
Never sent: `MSGID_WORLDCONFIGCONTENTS` 0x0FA40005, `MSGID_NPCITEMCONFIGCONTENTS` 0x0FA40006,
`MSGID_WLSERVERFILECONTENTS` 0x0FA40007, `MSGID_TELEPORTLISTCONTENTS` 0x0FA40008 (no such immediate in
the binary) ([SETUP-004](../issues/setup-2.20.md#setup-004-the-config-push-the-world-log-server-never-sends)) [V].

### 3.2 Player data

| MsgID | Layout read | Behaviour |
|---|---|---|
| `REQUEST_PLAYERDATA` 0x0C152210 | 6 char 10; 16 account 10 (password and IP not read) | open `Character\AscII<c>\<char>.txt` (`<c>` = decimal value of the first name byte); the file's `account-name` must equal the request's account (`ReadCharOwnerLevelMapFromFile` `0x40aff0`). Reply `RESPONSE_PLAYERDATA` 0x0C152211: CONFIRM 0x0F14, 6 char 10, 16 account-status byte = 2, 17 guild-status byte, 18 the whole file text, body size `19 + file length` (one NUL after the text); REJECT 0x0F15 with 6 char 10, body 16 bytes, if the file is missing or the account differs [V `0x403976-0x403a4a`] |
| guild-status byte | | `CheckGuildStatusForPlayerData` `0x403b10`: reads `character-guild-name/-rank/-GUID` from the file; if the guild is not `NONE`, opens `Guild\AscII<c>\<guild>.txt` and compares `guild-GUID`; 0 if missing or different, else 1 [V strings, I exact compare] |
| `SAVEPLAYERDATA*` 0x0DF3076F / 0x0DF3074F / 0x0DF30770 | 6 char 10; 16 account 10; 26 password 10; 36 count flag (byte); 37 file text | writes the text as `Character\AscII<c>\<char>.txt` (creates folders, `0x4052f0`); **FILE mode only**, there is no binary (DB-mode) parser. Logout variants with count flag 1 also send `CLEARACCOUNTSTATUS` to the MLS and set the account NULL. The `_REPLY` variant answers `RESPONSE_SAVEPLAYERDATA_REPLY` 0x0C152212 with 6 char 10 [V `0x404e80`, `0x405266`] |
| `NOSAVELOGOUT` 0x0DF30750 | 16 account 10; 36 count flag | flag 1 -> `CLEARACCOUNTSTATUS` to MLS, account NULL [V `0x405780`] |

### 3.3 Account-state messages

| MsgID | Layout read | Behaviour |
|---|---|---|
| `ENTERGAMECONFIRM` 0x12A01006 | 6 account 10; 16 password 10; 26 game server name 10; 52 level i32 | only an INIT or WAIT account with a password or level mismatch is refused: send that game server `REQUEST_FORCEDISCONECTACCOUNT` 0x15000000, MsgType 0x0F14, 6 account 10, 16-byte body (on a fatal send result the handler also calls `HandleGameServerConnectionLost` and closes that socket). Every other case sets PLAYING on that server: INIT/WAIT with matching data, an unknown account, and an account already PLAYING (its server name is overwritten) [V `0x4033f0`] |
| `SETACCOUNTWAITSTATUS` 0x25000199 | 6 account 10; 16 level i32 | account -> WAIT (server change) [V `0x40adf0`] |
| `GAMESERVERSHUTDOWNED` 0x14000000 | none | marks that game server state 3; nothing else uses the flag [V `0x4033c0`] |
| `SENDSERVERSHUTDOWNMSG` 0x20000000 | none | forwarded to the MLS with body "A3E9D1G0U7" (10 bytes), MsgType 0 [V `0x40b410`] |

### 3.4 Guilds, occupy flags, logs

| MsgID | Layout read | Behaviour |
|---|---|---|
| `CREATENEWGUILD` 0x0FC94208 | 6 char 10; 36 guild 20; 56 location 10; 66 GUID i32 | creates `Guild\AscII<c>\<guild>.txt`; reply `RESPONSE_CREATENEWGUILD` 0x0FC94209 confirm/reject with char name (reject if the file exists or cannot be created) [V `0x409170`] |
| `DISBANDGUILD` 0x0FC9420A | 6 char 10; 36 guild 20 | deletes the guild file; reply 0x0FC9420B [V `0x409940`] |
| `UPDATEGUILDINFO_NEW/DELGUILDSMAN` 0x0FC9420C / D | 6 char 10; 16 guild 20 | rewrites the `[GUILDSMAN]` list (`guildsman-name = %s` lines) [V `0x409d30`, `0x409fa0`] |
| `SAVE*OCCUPYFLAGDATA` 0x167C0A31 / 32 | raw text | writes `GameData\AresdenOccupyFlag.Txt` / `ElvineOccupyFlag.Txt` [V `0x408f80`] |
| `GAMEMASTERLOG` / `GAMEITEMLOG` / `GAMECRUSADELOG` | text | appended with a timestamp to `GMLogData\GMEvents%04d%02d%02d.log`, `ItemLogData\ItemEvents...log`, `EKEventsLog\EKEvents...log` [V] |

All 2.20 game-server -> Log layouts in `game-log-protocol.md` 3 match these offsets (FILE mode) [V
both sides].

### 3.5 Game server lost

`HandleGameServerConnectionLost` `0x403170` [V]: sends `MSGID_REQUEST_REMOVEGAMESERVER` to the MLS
(6 world name 30; 36 game server name 10; 40 data bytes), sends `CLEARACCOUNTSTATUS` for every account whose
stored server name is that server (and sets them NULL), and deletes its maps.

## 4. Messages from clients (world port)

Only `MSGID_REQUEST_ENTERGAME` 0x0FC94205 is useful from a client (`HandleRequestEnterGame`
`0x405440`) [V]:
- reads 6 char name 10, 16 map name 10, 26 account 10. **MsgType (the enter kind), password, level,
  world name and command-line token are not read.**
- map lookup (`FindGameServerByMap` `0x4056c0`): no game server has the map -> `RESPONSE_ENTERGAME`
  0x0FC94206, MsgType 0x0F21 (REJECT), reason byte 3, 7-byte body. A reason-5 branch exists but
  `0x4056c0` only returns -1 or 0, so it is unreachable [V].
- the account must exist in the state table (any state; set by the MLS's INITSTATUS or a game server's
  WAITSTATUS); otherwise the socket is closed without a reply [V].
- CONFIRM: MsgType 0x0F22, 6 game server address 16, 22 port u16, **body 24 bytes, no game-server
  name** [V `0x405634`].

## 5. Messages to and from the Main Log Server

Full table, both directions: `official-mainserver.md` section 5 (the same layouts seen from the MLS).
WLS side [V]:
- First MLS socket to connect sends `MSGID_REGISTER_WORLDSERVER` 0x23AA210E (`0x404240`), then
  `MSGID_REGISTER_WORLDSERVER_GAMESERVER` for every game server already registered (`0x40ae50`). Every
  later socket sends `MSGID_REGISTER_WORLDSERVERSOCKET` 0x23AA210F (`0x4042e0`). Both: 6 world name
  30; 36 world address 16; 52 world port u32; body 56 (data 50) [V].
- A socket becomes usable for sends only after the MLS answers `RESPONSE_REGISTER_WORLDSERVERSOCKET`
  0x280120A0 on it (`0x40b3e0` sets this+0x27560[i] = 1) [V].
- `SendMsgToMLS` `0x403fd0` round-robins over usable sockets; on send failure it recreates that socket
  [V].

## 6. Account connection-state machine

Table at this+0xa06c, 5 000 entries of 0x30 bytes: +4 state, +5 account 11, +0x10 password 11, +0x1b
game server name 11, +0x28 level, +0x2c last-change time (`timeGetTime`) [V `SetAccountStatus`
`0x40a210`, `GetAccountStatus` `0x40a8f0`]. Log texts name the states [V strings `0x418850-0x418944`]:

| Value | Name | Entered by |
|---|---|---|
| 0 / no entry | NULL | logout with count flag, NOSAVELOGOUT flag 1, game server lost, timeout |
| 1 | INIT | MLS `SETACCOUNTINITSTATUS` (account, password, level) |
| 2 | WAIT | game server `SETACCOUNTWAITSTATUS` (level) |
| 3 | PLAYING | `ENTERGAMECONFIRM` (server name) |

- INIT received while PLAYING: keeps PLAYING, stores the new password and level and sends that game
  server `MSGID_REQUEST_CHECKACCOUNTPASSWORD` 0x2654203A (6 account 10, 16 password 10, 26 level i32;
  30-byte body) [V `0x40a493`]. This is how the 2.20 game server's CHECKACCOUNTPASSWORD handler gets
  used.
- Timeout: every 10th timer tick (`CheckAccountStatusTimeout` `0x40aa30`), INIT or WAIT older than
  20 000 ms -> `CLEARACCOUNTSTATUS` to the MLS and NULL [V].

## 7. Files read and written

| Path | R/W | Content |
|---|---|---|
| `WLserver.cfg` | R | section 1 |
| `Npc.cfg`, `Item.cfg`, `Item2.cfg`, `Item3.cfg`, `Magic.cfg`, `Skill.cfg`, `Potion.cfg`, `Quest.cfg`, `BuildItem.cfg`, `DupItemID.cfg`, `Noticement.txt` | R | pushed verbatim |
| `GameData\AresdenOccupyFlag.txt`, `GameData\ElvineOccupyFlag.txt` | R/W | pushed / saved |
| `Character\AscII<c>\<name>.txt` | R/W | character file (format below) |
| `Guild\AscII<c>\<guild>.txt` | R/W | guild file (below) |
| `GMLogData\`, `ItemLogData\`, `EKEventsLog\` daily logs, `Events.log` | W | logs |

**New character file** written by `HandleCreateNewCharacter` `0x405850` (strings `0x417ce0-0x418404`,
order as written) [V strings, I exact numeric values]:

```
[NAME-ACCOUNT]
character-name     = <name>
account-name       = <account>
account-password   = <password>          plaintext, until the first game-server save rewrites the file
[STATUS]
character-location   = NONE
character-guild-name = NONE
character-guild-rank = -1
character-loc-map  = default
character-loc-x   = -1
character-loc-y   = -1
character-HP / -MP / -SP / -DefenseRatio = <computed>
character-LEVEL    = 1
character-STR/INT/VIT/DEX/MAG/CHARISMA = <from request>
character-LUCK     = 10
character-EXP      = 0
character-LU_Pool  = 0
character-EK-Count = 0
character-PK-Count = 0
sex-status / skin-status / hairstyle-status / haircolor-status / underwear-status = <from request>
appr1..appr4 = <appr1 = hairstyle<<8 | haircolor<<4 | underwear (V 0x4059fe), others 0>
[ITEMLIST]
character-item = Dagger 1 0 0 0 0 0 0 0 0 300 0
character-item = RecallScroll / RedPotion / BluePotion / GreenPotion (same numbers)
character-item = KneeTrousers(M) ...      and Shirt(M) or Chemise(W) with a colour (%d) field
[MAGIC-SKILL-MASTERY]
magic-mastery     = 100 x '0'
skill-mastery     = ...
skill-SSN     = ...
[ITEM-EQUIP-STATUS]
item-equip-status = 0000011000... (male) / 0000010000... (female)
[EOF]
```

This settles the "original kit unknown" item in `persistence.md` 6.

**Guild file** (`0x409170`, strings `0x4185b8-0x418728`) [V strings]:

```
;Guild file - Updated YYYY/MM/DD/HH/MM     (or ";Just created")
[GUILD-INFO]
guildmaster-name     = <char>
guild-GUID = <GUID>
guild-location       = <aresden|elvine>
[GUILDSMAN]
guildsman-name       = <char>             one line per member
```

## 8. Ghidra function names

Renamed in the Ghidra project (program WorldServer.exe): the 51 handler names used in this document,
for example `ProcessMsgQueue_WLS` `0x4021b0`, `HandleRegisterGameServer` `0x402510`,
`HandleRequestPlayerData` `0x403640`, `HandleSavePlayerData` `0x404e80`, `HandleRequestEnterGame`
`0x405440`, `SetAccountStatus` `0x40a210`, `SendMsgToMLS` `0x403fd0`. An end-of-line comment marks the
registration ID at `0x40221c`. Pseudo-C of the core handlers: `decomp/WorldServer/`.

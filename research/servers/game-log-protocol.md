# Game server <-> Log server protocol (v2.20)

Tags and [V]/[I]/[U] as in `topology.md`; framing in `wire-format.md`. Offsets are body offsets
(0 = MsgID u32, 4 = MsgType u16, 6 = payload). "Log server" here means whatever listens on the
game server's `log-server-address:internal-log-server-port` (the WLS in the 2.20 deployment).

## Contents
1. Sockets and routing
2. Startup sequence
3. Game -> Log messages
4. Log -> Game messages
5. Config-file push
6. Player data: request, load reply, save (binary DB mode and text FILE mode)
7. Account state machine (inferred)
8. Failure behaviour
9. Messages defined but not used by the game server

## 1. Sockets and routing

- One **main** socket, connected at init (`SRV Game.cpp:799-801`), plus **10 sub sockets**
  (`DEF_MAXSUBLOGSOCK`, `SRV Game.h:103`) opened one per second by the 1-second timer
  (`SRV Game.cpp:3729-3738`). All go to the same address and port. [V]
- The main socket carries registration and shutdown notices (`SRV Game.cpp:4257-4382`). Everything
  per-player goes round-robin over available sub sockets (`_bCheckSubLogSocketIndex`,
  `SRV Game.cpp:43123-43154`); a failed send is retried once on the next sub socket
  (`SRV Game.cpp:4757-4789`). [V]
- Messages received on any of the 11 sockets enter one queue (`SRV Game.cpp:4193-4204, 42461-42470`),
  so **the Log server may reply on any socket of that game server**; the game server matches replies by
  character name, not by socket. [V]
- Sub sockets send nothing on connect (`SRV Game.cpp:42407-42414`). The Log server must accept
  unannounced connections and attribute them to a game server by peer IP or simply by the content of
  later messages [I]. The original must have tolerated 11 connections per game server [V].

## 2. Startup sequence

1. Main socket connects; game server sends `MSGID_REQUEST_REGISTERGAMESERVER` (`SRV Game.cpp:4153-4157`). [V]
2. Log server sends the config files (section 5), then `MSGID_RESPONSE_REGISTERGAMESERVER` CONFIRM. [V order: the official WLS sends 11 configs, then the CONFIRM (corrected from official binary, WorldServer.exe 0x402510); note it expects the registration as MsgID 0x0512A3E3, not 0x0512A3F4, see `official-compat-220.md`]
   Reason: the CONFIRM handler prints "(!!!) STOPPED!" for every config not yet received
   (`SRV Game.cpp:15126-15191`), so the original evidently sent the configs first. The flags are
   independent, so a later config still lets the server start. [V handler / I order]
3. When all configs, Log CONFIRM, gate CONFIRM and 10 connected sub sockets are present, the game
   server opens for players (`SRV Game.cpp:3693-3708`). [V]

## 3. Game -> Log

| MsgID | Value | Socket | Body size | Payload (offset: field) | Sent at |
|---|---|---|---|---|---|
| `MSGID_REQUEST_REGISTERGAMESERVER` | 0x0512A3F4 | main | 35 + 11*M | 6: server name 10; 16: game server IP 16 (text); 32: port u16; 34: map count M u8; 35: M x map name 11 | `SRV Game.cpp:4345-4382` |
| `MSGID_REQUEST_PLAYERDATA` | 0x0C152210 | sub | 52 | 6: char name 10; 16: account 10; 26: password 10; 36: client IP 15 | `SRV Game.cpp:4384-4422`, called from `RequestInitPlayerHandler` 1608 |
| `MSGID_ENTERGAMECONFIRM` | 0x12A01006 | sub | 56 | 6: account 10; 16: password 10; 26: game server name 10; 36: client IP 16; 52: level i32 | `SRV Game.cpp:4305-4343`, after load success 4991 |
| `MSGID_REQUEST_SAVEPLAYERDATA` | 0x0DF3076F | sub | see 6.3 | autosave every 30 min outside fight zones | `SRV Game.cpp:3928-3933`, `SRV Game.h:60` |
| `MSGID_REQUEST_SAVEPLAYERDATA_REPLY` | 0x0DF30770 | sub | see 6.3 | save before a server change; Log must answer | `SRV Game.cpp:23369, 23437, 23470, 23512` |
| `MSGID_REQUEST_SAVEPLAYERDATALOGOUT` | 0x0DF3074F | sub | see 6.3 | normal logout / disconnect | `SRV Game.cpp:2800` |
| `MSGID_REQUEST_NOSAVELOGOUT` | 0x0DF30750 | sub | 37 | 6: name 10; 16: account 10; 26: password 10; 36: count flag u8 | `SRV Game.cpp:4524-4556`; at 2802, 2828 (not-yet-loaded or no-save disconnects) |
| `MSGID_REQUEST_SETACCOUNTWAITSTATUS` | 0x25000199 | sub | 20 | 6: account 10; 16: level i32 | `SRV Game.cpp:4281-4303`; client left for a server change, 2852 |
| `MSGID_REQUEST_SETACCOUNTINITSTATUS` | 0x25000198 | sub | 20 | same layout | defined in the switch, never called ([SESS-004](../issues/session.md#sess-004-setaccountinitstatus-from-the-game-server-is-dead)) [V] |
| `MSGID_REQUEST_CREATENEWGUILD` | 0x0FC94208 | sub | 75 (70 used) | 6: char 10; 16: account 10; 26: password 10; 36: guild name 20; 56: guild town ("aresden"/"elvine") 10; 66: guild GUID i32 | `SRV Game.cpp:4558-4598`; GUID made by the game server from the clock, 16711 |
| `MSGID_REQUEST_DISBANDGUILD` | 0x0FC9420A | sub | 56 | 6: char 10; 16: account 10; 26: password 10; 36: guild name 20 | `SRV Game.cpp:4600-4633` |
| `MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN` | 0x0FC9420C | sub | 36 | 6: char 10; 16: guild name 20 | `SRV Game.cpp:4635-4660`; from 19085 |
| `MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN` | 0x0FC9420D | sub | 36 | same | `SRV Game.cpp:4662-4687`; from 19128, 36766 |
| `MSGID_GAMEMASTERLOG` | 0x210A914E | sub | 6 + len | 6: ASCII text, no terminator | `SRV Game.cpp:4688-4709` |
| `MSGID_GAMEITEMLOG` | 0x210A914F | sub | 6 + len | text | `SRV Game.cpp:4711-4731` |
| `MSGID_GAMECRUSADELOG` | 0x210A9150 | sub | 6 + len | text | `SRV Game.cpp:4733-4753` |
| `MSGID_GAMESERVERSHUTDOWNED` | 0x14000000 | main | 6 | none; "this game server is shutting down" | `SRV Game.cpp:4269-4279`; from 11019, 14860, 14929, 40820 |
| `MSGID_SENDSERVERSHUTDOWNMSG` | 0x20000000 | main | 6 | none; forwarded after the gate's shutdown announcement so the WLS refuses new users (comment 14917) | `SRV Game.cpp:4257-4267, 14918` |

MsgType is `DEF_MSGTYPE_CONFIRM` (0x0F14) on all of them. [V]

Count flag ("카운팅 플래그", counting flag): TRUE for a normal logout, FALSE when the game server kicks a
duplicate login (`SRV Game.cpp:1564`) or saves for a server change (`SRV Game.cpp:23369`). [V]
Meaning [I]: whether the Log server should count this as a logout (clear the account's in-game state,
stop billing time).

## 4. Log -> Game

Dispatch: `SRV Game.cpp:15090-15268`.

| MsgID | Value | MsgType | Payload | Game server action | Cite |
|---|---|---|---|---|---|
| `MSGID_RESPONSE_REGISTERGAMESERVER` | 0x0512A3F5 | CONFIRM / REJECT | none | CONFIRM sets `m_bIsLogSockAvailable` | `SRV Game.cpp:15126-15191` |
| `MSGID_RESPONSE_PLAYERDATA` | 0x0C152211 | CONFIRM / REJECT | 6.2 | CONFIRM -> load; REJECT with byte 16 == 1 -> client gets INITPLAYER REJECT ("world full"); other REJECT -> disconnect as "not existing character" | `SRV Game.cpp:4796-4853` |
| `MSGID_RESPONSE_SAVEPLAYERDATA_REPLY` | 0x0C152212 | any | 6: char name 10 | send `DEF_NOTIFY_SERVERCHANGE` to that player | `SRV Game.cpp:36354-36372` |
| `MSGID_REQUEST_CHECKACCOUNTPASSWORD` | 0x2654203A | any | 6: account 10; 16: password 10; 26: level i32 | disconnect any client of that account whose password or level differ | `SRV Game.cpp:46052-46083` |
| `MSGID_REQUEST_FORCEDISCONECTACCOUNT` | 0x15000000 | count (capped at 10) | 6: account 10 | send `DEF_NOTIFY_FORCEDISCONN` (count) to that account's clients | `SRV Game.cpp:15100-15104, 38098-38117` |
| `MSGID_RESPONSE_CREATENEWGUILD` | 0x0FC94209 | `DEF_LOGRESMSGTYPE_CONFIRM` / `REJECT` | 6: char name 10 | confirm -> rank 0 (master), tell client; reject -> guild "NONE" | `SRV Game.cpp:16547-16610` |
| `MSGID_RESPONSE_DISBANDGUILD` | 0x0FC9420B | CONFIRM / REJECT | 6: char name 10 | confirm -> notify guild, clear guild | `SRV Game.cpp:16751-16800` |
| `MSGID_GUILDNOTIFY` | 0x0DF30760 | any | 6: char 10; 16: guild 20 | parsed, no action in 2.20 | `SRV Game.cpp:20497-20514` |
| config contents (13 IDs) | see 5 | any | raw text | decode | `SRV Game.cpp:15198-15268` |

The guild result check also needs the character at level >= 20 and charisma >= 20 (level >= 100 with
`DEF_CHINESEGUILDLIMIT`, which the exported tree enables) or the reply is ignored
(`SRV Game.cpp:16563-16570`, `SRV GlobalDef.h:65-70`). [V]

## 5. Config-file push

Payload = the text of a game-data file starting at offset 6. The decoder receives `pData + 6` and the
full body size, copies that many bytes into a zeroed buffer and tokenises it with the same parser as a
file on disk (example `SRV Game.cpp:9480-9496`). Six extra bytes are read past the text; harmless
because the receive buffer is larger. [V]

| MsgID | Value | File (in `SRV`) | Required to start? | Cite |
|---|---|---|---|---|
| `MSGID_ITEMCONFIGURATIONCONTENTS` | 0x0FA314D9 | `Item.cfg`, `Item2.cfg`, `Item3.cfg` | yes | `SRV Game.cpp:15204-15208` |
| `MSGID_NPCCONFIGURATIONCONTENTS` | 0x0FA314DA | `Npc.cfg` | yes | `SRV Game.cpp:15210-15214` |
| `MSGID_MAGICCONFIGURATIONCONTENTS` | 0x0FA314DB | `Magic.cfg` | yes | `SRV Game.cpp:15216-15219` |
| `MSGID_SKILLCONFIGURATIONCONTENTS` | 0x0FA314DC | `Skill.cfg` | yes | `SRV Game.cpp:15221-15224` |
| `MSGID_PORTIONCONFIGURATIONCONTENTS` | 0x0FA314DE | `portion.cfg` | yes | `SRV Game.cpp:15231-15234` |
| `MSGID_QUESTCONFIGURATIONCONTENTS` | 0x0FA40001 | `Quest.cfg` | yes | `SRV Game.cpp:15226-15229` |
| `MSGID_BUILDITEMCONFIGURATIONCONTENTS` | 0x0FA40002 | `BuildItem.cfg` | yes | `SRV Game.cpp:15198-15202` |
| `MSGID_DUPITEMIDFILECONTENTS` | 0x0FA40003 | `DupItemID.cfg` | no | `SRV Game.cpp:15245-15248` |
| `MSGID_NOTICEMENTFILECONTENTS` | 0x0FA40004 | noticement text (not in tree) [U name] | no | `SRV Game.cpp:15250-15253` |
| `MSGID_WORLDCONFIGCONTENTS` | 0x0FA40005 | world config: keys `world-server-max-user`, `world-server-max-level`, `world-server-forcerecall-time` (file not in tree) | no | `SRV Game.cpp:15255-15258, 25070-25141` |
| `MSGID_NPCITEMCONFIGCONTENTS` | 0x0FA40006 | NPC drop config (file not in tree) | no | `SRV Game.cpp:15260-15263` |
| `MSGID_WLSERVERFILECONTENTS` | 0x0FA40007 | `WLserver.cfg` (only `world-server-port` used) | yes in `DEF_DEFENCEHACKING` builds | `SRV Game.cpp:15236-15243, 25019-25066` |
| `MSGID_TELEPORTLISTCONTENTS` | 0x0FA40008 | teleport list (file not in tree) | no | `SRV Game.cpp:15265-15267` |

- Each item file is under 30000 bytes (27886 / 29585 / 9283) and ends with `[ENDITEMLIST]`; item IDs
  do not overlap (1-187, 188-..., ...-688). The decoder only fails on a duplicate ID
  (`SRV Game.cpp:9514-9522`). So the three files were sent as three separate
  `ITEMCONFIGURATIONCONTENTS` messages, since one concatenated message would exceed 30000 bytes. [V sizes;
  method confirmed: corrected from official binary, WorldServer.exe 0x402510 sends Item.cfg, Item2.cfg,
  Item3.cfg one by one]
- The game server never opens these files itself (no `fopen` of them in `SRV Game.cpp`; file opens are
  listed in `topology.md`). Their presence in the server folder is an artefact of a shared
  working folder. [V]

## 6. Player data

### 6.1 Build mode

`DEF_DBGAMESERVER` chooses the payload format (`SRV Game.cpp:5549, 6943, 9477`):
- **DB mode** (binary): enabled in the exported tree because `DEF_CHINESE` is defined
  (`SRV GlobalDef.h:16, 72-73`); also USA, Japan, Taiwan, InterServer, Test builds. [V]
- **FILE mode** (text, the character file itself): Korean builds and 2.03. [V]

For recreation, FILE mode is the simpler target: the save payload is the file to write and the load
payload is the file to send back (section 6.4). [I]

### 6.2 MSGID_RESPONSE_PLAYERDATA (Log -> Game)

| Off | Size | Field |
|---|---|---|
| 4 | 2 | MsgType: `DEF_LOGRESMSGTYPE_CONFIRM` (0x0F14) or `REJECT` (0x0F15) |
| 6 | 10 | character name (the game server finds the client by this) |
| 16 | 1 | account status (ignored since v1.43); on REJECT: 1 = world full, else "no such character" |
| 17 | 1 | guild status: 0 = the character's guild no longer exists -> game server clears it and notifies (`SRV Game.cpp:4937-4946`) |
| 18 | ... | DB mode: character ID u32 (`m_dwCharID`) then block C (6.5) **without** the bank-modified byte; FILE mode: character file text |

`SRV Game.cpp:4805-4840, 4871-4890`. The decoder gets size `dwSize - 19` (`SRV Game.cpp:4890`); in
FILE mode that is the text length minus one, so the Log server should append one byte (a NUL or
newline) after the text [I]. [V layout]

Validation the game server applies to loaded data (fail -> client dropped as "data error",
`SRV Game.cpp:4890-4896`): level-up points 0..3 each and in total; each stat 10..200; stat sum ==
(level-1)*3+70; character and account names pass `bCheckValidName`; penalty-block date in the past;
every item name exists in the item config (`SRV Game.cpp:5953-5959, 6263-6323`). [V]

### 6.3 Save messages (Game -> Log)

`MSGID_REQUEST_SAVEPLAYERDATA`, `..._REPLY`, `..._SAVEPLAYERDATALOGOUT` share one layout
(`SRV Game.cpp:4469-4522`):

| Off | Size | DB mode | FILE mode |
|---|---|---|---|
| 6 | 10 | character name | same |
| 16 | 10 | account | same |
| 26 | 10 | password (plaintext) | same |
| 36 | 4 | character ID u32 | - |
| 40 / 36 | 1 | count flag | count flag |
| 41 / 37 | ... | block C (6.5) | character file text (6.4) |

Body size sent: DB `41 + iSize` with `iSize = 582 + 4 + 8*S + 2 + 55*I + 2 + 50*B + 4`
(`SRV Game.cpp:6940`), which is **larger than the bytes written** (fixed part written is 443 bytes;
items 53 not 55; bank items 48 not 50): the tail is zero padding. FILE mode `37 + strlen(text)`. [V; the
byte counts are derived from `SRV Game.cpp:6428-6941`]

### 6.4 FILE-mode character text

Written by `_iComposePlayerDataFileContents` FILE variant (`SRV Game.cpp:8917-9474`), read back by
the FILE decoder (`SRV Game.cpp:6945-8916`, key list 8591-8712). Full format: `persistence.md`. The
same text is what the game server writes to `Character_<M>_<D>_<H>_<Min>\AscII<n>\<name>.txt` when no
Log socket is available (`SRV Game.cpp:38736-38790`). [V]

### 6.5 DB-mode block C (binary character record)

Offsets relative to the start of block C. Writer `SRV Game.cpp:6444-6933`, reader `SRV Game.cpp:5582-6188`.
s16 = signed 16-bit, i32 / u32 = 32-bit, all little-endian.

| Off | Type | Field | | Off | Type | Field |
|---|---|---|---|---|---|---|
| 0 | s16 | char ID num1 | | 191 | char[10] | locked map name |
| 2 | s16 | char ID num2 | | 201 | i32 | locked map time |
| 4 | s16 | char ID num3 | | 205 | s16 x3 | penalty block year, month, day |
| 6 | s16 | level | | 211 | char[20] | guild name ("NONE" if rank -1) |
| 8 | s16 x6 | STR, VIT, DEX, INT, MAG, CHR | | 231 | i32 | guild GUID (-1 if none) |
| 20 | i32 | experience | | 235 | s16 | guild rank |
| 24 | s16 x6 | level-up points STR, VIT, DEX, INT, MAG, CHR | | 237 | s16 x3 | fightzone number, reserve time, ticket number |
| 36 | u8 | sex | | 243 | s16 x5 | quest, quest ID, current count, reward type, reward amount |
| 37 | s16 x4 | skin, hair style, hair colour, underwear | | 253 | u8 | quest completed |
| 45 | i32 | appearance colour | | 254 | i32 x6 | special event ID, war contribution, crusade duty, crusade GUID, construction points, rating |
| 49 | s16 x4 | appr1..appr4 | | 278 | i32 x3 | HP, MP, SP |
| 57 | char[10] | location ("aresden"/"elvine"/"NONE") | | 290 | i32 x3 | EK count, PK count, reward gold |
| 67 | char[10] | map name | | 302 | s16 x2 | hunger, super-attack left |
| 77 | s16 x2 | x, y | | 306 | i32 x7 | time left: shut-up, rating, force recall, firm stamina, dead penalty; party ID; gizon upgrade left |
| 81 | char[100] | profile ("__________" if empty) | | 334 | char[100] | magic mastery, one ASCII '0'/'1' per spell |
| 181 | s16 | admin level | | 434 | s16 | down-skill index |
| 183 | i32 | contribution | | 436 | s16 | skill count S |
| 187 | i32 | special ability time | | 438 | S x 8 | {s16 skill ID, s16 mastery, i32 SSN} for each skill with mastery > 0 (of 60) |

Then: `s16` item count I, I x 53-byte item records; save only: `u8` bank-modified flag; `s16` bank
count B, B x 48-byte bank records.

Item record (53): name char[20]; count u32; touch effect type s16; touch values 1..3 s16 x3; colour
s16; special effect values 1..3 s16 x3; current lifespan i32; attribute u32; equipped u8; inventory
x s16; inventory y s16 (`SRV Game.cpp:6807-6863`). Bank record (48): the same minus equipped/x/y
(`SRV Game.cpp:6882-6927`). Max 50 items, 120 bank items (`SRV Client.h:25-26`). [V]

## 7. Account state machine (inferred)

From the messages above [I]:
- `ENTERGAMECONFIRM` -> account "playing on server <name>, level, IP".
- `SAVEPLAYERDATALOGOUT` / `NOSAVELOGOUT` with count flag TRUE -> account offline.
- `SAVEPLAYERDATA_REPLY` then `SETACCOUNTWAITSTATUS` -> account "changing server": keep it reserved until
  the client re-enters with `NEW_TOWLSBUTMLS`, or time it out.
- A login while "playing" -> `PLAYING`; force login -> `REQUEST_FORCEDISCONECTACCOUNT` to the owning
  game server.
- `GAMESERVERSHUTDOWNED` -> mark that server's maps unavailable and, after its saves, its accounts offline.
- `CHECKACCOUNTPASSWORD` lets the Log server push a password change or level correction to a live
  session [I purpose].

## 8. Failure behaviour

- Main log socket lost -> game server begins shutdown immediately (code 3) (`SRV Game.cpp:4174-4189`). [V]
- No usable sub socket -> shutdown (`SRV Game.cpp:43140-43149`); a lost sub socket is reconnected
  (`SRV Game.cpp:42433-42453`). [V]
- Logout save cannot be sent -> local text file (`SRV Game.cpp:2800, 38736-38790`). [V]

## 9. Defined but unused by HGserver 2.20

`MSGID_REGISTER_WORLDSERVER` (0x23AA210E), `..._WORLDSERVERSOCKET` (0x23AA210F),
`..._WORLDSERVER_GAMESERVER` (0x23AB211F), `MSGID_REQUEST/RESPONSE_CHARINFOLIST` (0x23AB2200/1),
`MSGID_REQUEST_REMOVEGAMESERVER` (0x2400000A), `MSGID_REQUEST_CLEARACCOUNTSTATUS` (0x24021EE0),
`MSGID_WORLDSERVERACTIVATED` (0x27049D0C), `MSGID_RESPONSE_REGISTER_WORLDSERVERSOCKET` (0x280120A0),
`DEF_MSGID_ANNOUNCEACCOUNT` (0x13000000), `MSGID_ACCOUNTINFOCHANGE`, `MSGID_IPINFOCHANGE`,
`MSGID_ANNOUNCEACCOUNTNEWPASSWORD`, `MSGID_REQUEST/RESPONSE_IPIDSTATUS`, `..._ACCOUNTCONNSTATUS`,
`..._CLEARACCOUNTCONNSTATUS`, `MSGID_REQUEST_NOCOUNTINGSAVELOGOUT`, `MSGID_REQUEST_BLOCKACCOUNT`,
`MSGID_IPTIMECHANGE`, `MSGID_ACCOUNTTIMECHANGE`, `MSGID_REQUEST/RESPONSE_IPTIME`,
`MSGID_REQUEST_REGISTERDBSERVER` (`SRV NetMessages.h:262-437`). A grep of `SRV Game.cpp` and
`CLI Game.cpp` finds no use. They are almost certainly the MLS <-> WLS protocol (world registration,
character-list relay, account/IP billing state). Their layouts are **unknown**. [V unused / I purpose] Layouts now known for the ones the official binaries use: `official-mainserver.md` section 5 (corrected from official binary, MainServer.exe 0x403220, WorldServer.exe 0x4021b0).

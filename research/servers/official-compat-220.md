# Official MainServer / WorldServer versus the v2.20 game server and client

Item-by-item comparison of the two official log binaries (`official-mainserver.md`,
`official-worldserver.md`) with the documented 2.20 protocol (`client-log-protocol.md`,
`game-log-protocol.md`) and the 2.20 sources (`CLI` = `sources\client-2.20`, `SRV` =
`.claude\scratch\srv220`). Tags: [V] checked on both sides (binary address and source line); [I]
inferred. Nothing was run; all of this is static.

## Contents
1. Verdict
2. Client <-> MLS
3. Client <-> WLS (server change)
4. Game server <-> WLS
5. MLS <-> WLS
6. Missing messages
7. Patch list
8. Open questions

## 1. Verdict

Each blocker below is tracked as an entry in [issues/setup-2.20.md](../issues/setup-2.20.md).

**Not usable as-is. Usable with a few small binary patches plus a 2.20 game server rebuilt in FILE
mode with two source changes.** Against the exported 2.20 game-server tree as configured
(`DEF_CHINESE`, hence `DEF_DBGAMESERVER` and `DEF_DEFENCEHACKING`, `SRV GlobalDef.h:16, 72-78`) the
binaries cannot work at all [V]:
- the game server never starts: under `DEF_DEFENCEHACKING` it waits for `MSGID_WLSERVERFILECONTENTS`
  (`SRV Game.cpp:3698-3701, 15180-15187`), which this WLS never sends (no 0x0FA40007 immediate) [V];
- player data would be binary block C (`game-log-protocol.md` 6.5) while the WLS only stores and returns
  the character text file (`HandleSavePlayerData` `0x404e80`, `HandleRequestPlayerData` `0x403640`) [V];
- the registration MsgID differs (0x0512A3F4 sent by `SRV Game.cpp:4156`, 0x0512A3E3 expected at
  WLS `0x40221c`), so the WLS ignores the game server completely [V].

The client side is much closer: every client <-> MLS MsgID and request layout matches; only the
version words in the login reply differ (fixable in two bytes) [V].

Minimum working combination [I, from the items below; untested]:
1. MLS: patch the version words (2 bytes) and one internal-port allow-list string; optionally fix the
   `Hack =` account-creation bug (4 bytes).
2. WLS: patch the registration ID (1 byte), **or** change `MSGID_REQUEST_REGISTERGAMESERVER` to
   0x0512A3E3 in the 2.20 game-server source.
3. Game server: build without `DEF_DBGAMESERVER` and without `DEF_DEFENCEHACKING`, and make
   `DEF_NOTIFY_SERVERCHANGE` carry the WLS's `world-server-port` instead of `internal-log-server-port`
   (section 3).
4. Client: the hard-coded login address patch already required (`client-log-protocol.md` 1).
5. Config: put the game server's IP in `game-server-list` of `WLserver.cfg`.

## 2. Client <-> MLS

| Item | 2.20 client | MLS binary | Status |
|---|---|---|---|
| Port | `log-server-port` or hard-coded 2848 (`CLI Game.cpp:15543-15556`) | `log-server-port`, `+1`, `+2` (`0x413344-0x41346c`) | config / client patch [V] |
| LOGIN 0x0FC94201 request | 6 acct 10, 16 pw 10, 26 world 30 (`CLI Game.cpp:1682-1701`) | same offsets (`0x403811-0x403946`) | match [V] |
| LOGIN CONFIRM header | upper 2, lower 20 required | **4, 0x60** (`0x403eee`, `0x403f05`) | **mismatch, 2-byte patch** [V] |
| CONFIRM payload | status, 6 dates, count, 65-byte slots, 2 x i32 (`CLI Game.cpp:14935-15089`) | built by MLS + WLS in that exact layout (`0x403eb3-0x404050`, WLS `0x404370`) | match [V] |
| Slot record | 65 bytes, valid flag 0 -> 50 bytes | WLS writes 65 bytes or flag 0 + 50 (`0x404370`) | match [V] |
| World name | "WS1" forced (`CLI Game.cpp:15516-15523`) | must equal a registered **and activated** world (`0x403abb`) | set `world-server-name = WS1` [V] |
| LOGIN error types | 0x0F15, 0x0F16, 0x0F17, 0x0A02 | same values | match [V] |
| CREATENEWACCOUNT 0x0FC94202 | 264-byte request | handled; replies 0x0F18/19/1A | match, but password written as `Hack =` (`0x40c0c5`) so the new account cannot log in [V] |
| CREATENEWCHARACTER 0x0FC94204 | 77-byte layout | same 71 data bytes forwarded (`0x4091d2`); stats 10..14 sum 70 | match [V] |
| CREATE reply | 0x0F1C name + list, 0x0F1D, 0x0F1E | WLS sends these types through the relay (`0x405850`, `0x409520`) | match [V types; list layout I] |
| DELETECHARACTER 0x0FC94207 | 66-byte layout, MsgType = slot | handled; reply 0x0F1F via WLS | match [V IDs, I list layout] |
| ENTERGAME 0x0FC94205 | 200 bytes; kinds 0x0F1C / 0x0F1D | same offsets (`0x40696f-0x406a58`); both kinds handled | match [V] |
| ENTERGAME CONFIRM | reads address 16, port, **game-server name 20** (`CLI Game.cpp:15331-15360`) | body 24, no name (`0x407031-0x407097`) | harmless: the name only goes back in INITPLAYER, which the game server does not read past the observer flag (`SRV Game.cpp:1520-1553`) [V both; client reads stale buffer bytes, I] |
| ENTERGAME REJECT reasons | 1..7 | 3, 4, 5 used | match [V] |
| PLAYING / FORCEDISCONN | 0x0F20 / 0x0F23 | same | match [V] |
| CHANGEPASSWORD 0x0FC94210 | expects type 0x0A00 / 0x0A01; ignores MsgID | replies MsgID 0x0FC94211 with 0x0A00 / 0x0A01 | match [V] |
| Field cipher `bEncode` | not used for login fields | decode stub returns early (`0x411add`) | match [V] |

## 3. Client <-> WLS (server change)

- The client connects to the address and port in `DEF_NOTIFY_SERVERCHANGE` and sends ENTERGAME kind
  0x0F1F `NEW_TOWLSBUTMLS` (`CLI Game.cpp:45039-45090`) [V]. The WLS accepts ENTERGAME on its
  `world-server-port` and ignores the kind (`0x405440`) [V]: compatible.
- Which port the game server announces: `DEF_DEFENCEHACKING` builds use the pushed `WLserver.cfg`
  (never pushed by this WLS -> port 0); other builds use `internal-log-server-port`, i.e. the WLS
  **internal** port (`SRV Game.cpp:5290, 5336, 18646-18667`; 5336 is the `internal-log-server-port` key test, 5334 is `game-server-port`) [V]. On that port the WLS closes every
  peer not listed in `game-server-list` (`0x4019e0`) [V], so a client would be dropped. Hence the
  required game-server source change: send the WLS world port (for example a new `gserver.cfg` key) [I].
- The WLS requires an account entry (WAIT, set by the game server's `SETACCOUNTWAITSTATUS`) [V]; the
  2.20 game server sends it before the client leaves (`SRV Game.cpp:2852`) [V]. Entries time out after
  20 s (`0x40aa30`) [V].

## 4. Game server <-> WLS

| 2.20 message | ID / layout match | Notes |
|---|---|---|
| `REQUEST_REGISTERGAMESERVER` | **ID differs** (F4 vs E3); layout identical (6 name 10, 16 addr 16, 32 port u16, 34 count, 35 maps) | 1-byte patch at file offset 0x221D, or change the source define [V] |
| `RESPONSE_REGISTERGAMESERVER` 0x0512A3F5 | match; CONFIRM after all configs | [V `0x4029c0`] |
| config pushes 0x0FA314D9..DC, 0x0FA314DE, 0x0FA40001..04 (0x0FA314DD is `MSGID_PLAYERITEMLISTCONTENTS`, not a config push) | match | all seven required by the 2.20 start check are sent (`SRV Game.cpp:3693-3698`) [V] |
| occupy-flag pushes 0x17081034 / 35 | match | [V] |
| `SAVEARESDEN/ELVINEOCCUPYFLAGDATA` 0x167C0A31 / 32 | **never sent by the 2.20 game server** (send `case` in a `/* */` block, all callers commented out, `SaveOccupyFlagData` returns first: `SRV Game.cpp:3762-3764, 4424-4468, 47997`) | the WLS handler (`0x408f80`) is never reached, so with this WLS crusade occupy flags are never saved; a functional gap for the "usable with patches" verdict (see `../shared/formats/server-program-cfg.md`) [V] |
| `WORLDCONFIGCONTENTS`, `NPCITEMCONFIGCONTENTS`, `WLSERVERFILECONTENTS`, `TELEPORTLISTCONTENTS` | **never sent** | optional except WLSERVER under `DEF_DEFENCEHACKING` [V]; effect of no NPC-item config under `DEF_NPCITEM` (on in `DEF_CHINESE`) not checked [U] |
| `REQUEST_PLAYERDATA` | match | password and IP fields ignored by the WLS [V] |
| `RESPONSE_PLAYERDATA` | FILE-mode layout match (status byte 2, guild byte, text, +1 NUL; `SRV Game.cpp:4805-4890`) | **DB mode incompatible** [V] |
| `SAVEPLAYERDATA`, `_LOGOUT`, `_REPLY` | FILE-mode layout match (count flag at 36, text at 37) | DB mode incompatible (char ID at 36) [V] |
| `RESPONSE_SAVEPLAYERDATA_REPLY` 0x0C152212 | match (6 name 10) | [V] |
| `NOSAVELOGOUT` | match | [V] |
| `ENTERGAMECONFIRM` | match (level at 52) | [V `0x4033f0`] |
| `SETACCOUNTWAITSTATUS` | match | [V] |
| `SETACCOUNTINITSTATUS` | the WLS reads the MLS layout (password at 16, level at 26), not the 2.20 game-server layout (level at 16) | the 2.20 game server never sends it, so no conflict [V] |
| `CHECKACCOUNTPASSWORD` 0x2654203A | match (6 acct, 16 pw, 26 level) | sent by the WLS when INIT arrives for a PLAYING account [V] |
| `FORCEDISCONECTACCOUNT` 0x15000000 | match; MsgType 0x0F14 is read as the count and capped at 10 (`SRV Game.cpp:38098-38117`) | [V] |
| guilds 0x0FC94208..0D, replies 09 / 0B | match | [V] |
| logs 0x210A914E..50 | match | [V] |
| `GAMESERVERSHUTDOWNED`, `SENDSERVERSHUTDOWNMSG` | match | [V] |
| sub sockets (10 per game server, silent) | WLS accepts any listed IP and dispatches by MsgID; replies on the receiving socket | compatible [V] |
| `GUILDNOTIFY` 0x0DF30760 | not sent by the WLS | the 2.20 game server ignores it anyway [V] |

## 5. MLS <-> WLS

Both binaries come from the same pack and agree on every layout in `official-mainserver.md` 5 [V].
Two faults:
- The MLS internal port accepts only five hard-coded address prefixes (`0x402894`); a local setup needs
  one string patched [V] **if the WLS dials the internal port**. It does not: `WorldServer.exe` dials `main-log-server-port`
  (2981, the MLS public port, `WLserver.cfg:10`; `ReadWLServerConfig` 0x401938 region, `InitializeWorldServer` 0x4014e0 (connect loop at 0x401590)), and the
  check runs only for connections accepted on the internal port (flag 1 at 0x40289c in accept handler 0x402770), so patch 4 is
  not needed for the 2981 setup [V bytes]. Whether the MLS accepts WLS registration on a public socket is still unverified (section 8).
- The world shutdown message can never be accepted (code compared at the wrong offset, `0x40f5ff`) [V];
  harmless for normal play [I].

## 6. Missing messages

- **2002-12-08 world-server security (`DEF_DEFENCEHACKING`)**: `MSGID_WLSERVERFILECONTENTS`
  0x0FA40007 does not exist in the WLS. The WLS (linked 2002-11-29) predates the feature [V date,
  V absence].
- **Teleport list** `MSGID_TELEPORTLISTCONTENTS` 0x0FA40008, **world config** 0x0FA40005 (max users,
  max level, force-recall time) and **NPC item config** 0x0FA40006: absent [V]. The 2.20 game server
  treats them as optional and keeps defaults [V `SRV Game.cpp:15255-15267`; defaults I].
- `MSGID_REQUEST_RETRIEVEITEM` 0x0DF30751 and the billing family (`IPTIME`, `NOCOUNTINGSAVELOGOUT`):
  absent in both binaries; the 2.20 game server does not send them [V grep].

## 7. Patch list

File offset = VA - 0x400000 in both files for `.text`, `.rdata` and `.data` (raw and virtual
offsets are equal there), which covers all five patches [V section table]. It does **not** hold for
every section: MainServer `.idata` (VA 0x5E000, raw 0x5B000) and `.reloc` (VA 0x60000, raw 0x5D000),
and WorldServer's unusual fourth section `.khe\x07` (VA 0x1D000, raw 0x1A000, raw size 0) [V]. Bytes
verified in the files.

| File | Offset | Old | New | Effect |
|---|---|---|---|---|
| MainServer.exe | 0x3EF1 | `04` | `02` | CONFIRM upper version 2 (`mov word [eax], 4` at VA 0x403eee) |
| MainServer.exe | 0x3F08 | `60` | `14` | CONFIRM lower version 20 (VA 0x403f05) |
| MainServer.exe | 0xC0C6 | `80 12 45 00` | `F0 1A 45 00` | create-account writes `account-password = ` (string 0x451af0) instead of `Hack = ` |
| MainServer.exe | 0x51224 | `211.115.208.` | e.g. `127.0.0.1` + NUL padding (12 bytes) | allow a local WLS on the internal port (compare length 12, `0x40289e`); **optional**: not needed when the WLS dials the public port 2981 (the shipped `WLserver.cfg`), only if it is pointed at 3840 (section 5) |
| WorldServer.exe | 0x221D | `E3` | `F4` | accept 2.20's `REQUEST_REGISTERGAMESERVER` 0x0512A3F4 (`cmp eax, imm32` at VA 0x40221c) |

[V old bytes; effect of each patch I until tested.]

## 8. Open questions

- **Does the MLS accept a WLS registration on a public socket (2981)?** [U, runtime test needed]. The port question (R2-05) is
  answered: the WLS dials `main-log-server-port` = 2981, patch 4 is not needed (section 5; `research/review/round2/wls-port.md`).
  What remains untested is that the MLS dispatcher takes the WLS registration from a public-port socket.
- Exact meaning of MLS status 2 / 4 and the `FORCECHANGEPASSWORD` branch in ENTERGAME [U].
- Whether the "Extasis fix" changed anything beyond the title (no pristine MLS to diff) [U].
- New-character HP / MP / SP / DefenseRatio formulas in `0x405850` (decompile is broken by the large
  stack frame) [U].
- `DEF_NPCITEM` behaviour with no NPC-item config [U].

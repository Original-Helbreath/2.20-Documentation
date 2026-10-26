# Official Main Log Server (MainServer.exe, linked 2002-11-26)

Static map of the Main Log Server (MLS) binary in `.claude\scratch\v382core\MainServer.exe` (393 216
bytes). The window title format "HB Main-Log-Server v%d.%d Extasis fix (Snoopy81)" is called with 3, 51
(`0x4128e0`), so this is **v3.51 with a third-party "Extasis fix" by Snoopy81**, not a pristine
Siementech build; any oddity below may come from that fix [I]. It is an MSVC **debug** build (debug
CRT, `_CrtDbgReport`, `chkesp`, incremental-link jump table at `0x401005`). The binary was never run.

Method: Ghidra has the program imported but only 47 functions were created (no function at the entry
point `0x417700`), so the decompiler is unusable for it. Everything below comes from a linear capstone
disassembly of `.text` with the jump table resolved and string and import references annotated. The
only Ghidra change is plate comments at the handler addresses listed in section 8. A later review
re-checked what a decompiler-free read can reach (message IDs, strings, patch sites, allow-list
lengths, version words, the `Hack =` push: all hold) but could not reproduce the handler addresses,
offsets and branch structure here or in `decomp/MainServer/*.c`: those stay **unverified (no
decompiler output for MainServer.exe)**, not wrong. Tags as in
`official-worldserver.md`: **[V]** seen in the disassembly at the cited address; **[I]** inferred.
Body offsets: 0 = MsgID, 4 = MsgType, 6 = payload.

## Contents
1. Sockets, ports and config
2. Dispatcher and message table
3. Client-facing messages
4. Account file
5. Main <-> World protocol
6. Account connection state
7. Admin and billing messages
8. Ghidra comments and helper map

## 1. Sockets, ports and config

- Config `HMLserver.cfg` (`0x4022ad` pushes the name `0x451030`) [V]. Keys [V strings `0x451040-0x451090`]:
  `log-server-address`, `log-server-port`, `internal-log-server-port`, `backup-drive-letter`.
- Listeners (`sub_414de0` = create, bind, listen) [V `0x412c44`, `0x413344-0x41346c`]:

| When | Port | Window msg | Log text |
|---|---|---|---|
| start | `internal-log-server-port` (3840) | 0x446 | "(!) Internal-log-Socket Listening." |
| HOME key | `log-server-port` (2981 in the sample cfg) | 0x43c | "Log-Socket Listening... Server Activated." |
| HOME key | `log-server-port + 1` | 0x43d | "Log-Socket2 ..." |
| HOME key | `log-server-port + 2` | 0x43e | "Log-Socket3 ..." |

  The internal port is for World Log Servers; the three public ports are for clients. These are the
  "2881, 2882, 2883" of the README, which wrongly gives them to the WLS (see `official-worldserver.md` 1).
- **Internal-port IP allow-list** (accept handler `0x402770` (thunk `0x40123a`; `0x4027ae` is inside it), check at `0x402894-0x402957`, internal-port flag test at `0x40289c`) [V]: a socket
  accepted on the internal port is closed with "(HACK?) Unpermitted internal port connection from(%s)"
  unless the peer address starts with one of: `211.115.208.` (12 chars), `80.32.153.` (10),
  `203.234.215.` (12), `80.32.96.48.` (12), `.80.32.153..` (11, can never match). Compare helper
  `sub_415dc0` (strncmp-like, I). A WLS on `127.0.0.1` or a private LAN is refused unless one string is
  patched (section "patches" in `official-compat-220.md`).
- **Pack config evidence** [V cfg]: `.claude/scratch/v382core/HMLServer.cfg:2-4` has `log-server-address = 195.154.77.227`,
  `log-server-port = 2981`, `internal-log-server-port = 3840`; the pack's `WLserver.cfg:9-10` points the WLS at that same
  address with `main-log-server-port = 2981`, i.e. the MLS **public** port, and its own internal port is also 3840
  (`WLserver.cfg:7`). The public ports have no allow-list (table above). **Answered [V]:** the WLS binary dials
  `main-log-server-port` (2981, public), not the internal port and not the 4500 of the 2.20 sample (`SRV WLserver.cfg:11`);
  the allow-list runs only for connections accepted on the internal port (flag 1 at 0x40289c, accept handler 0x402770; the
  `211.115.208.` entry is at 0x451224), so patch 4 is not needed for this setup (`research/review/round2/wls-port.md`). All MLS
  listen ports are config-driven. **Unverified [U, runtime test]:** that the MLS accepts WLS registration on a public socket. The
  two programs still cannot share one host with both internal ports at 3840.
- Client table at this+0x48, 10 000 entries; each client object has +4 type (2 = world-server socket),
  +5 account 11, +0x10 cookie, +0x14 peer IP, +0x28 XSocket, +0x2c world index [V field use across
  handlers; names I].

## 2. Dispatcher and message table

Queue consumer `0x403220` (pops with `sub_403000`, switch on the MsgID at `[buffer+0]`) [V]. As in the
WLS, the switch does not check which socket type sent the message.

| MsgID | Name | Handler | Sender |
|---|---|---|---|
| 0x0FC94201 | `MSGID_REQUEST_LOGIN` | `0x4036f0` | client |
| 0x0FC94202 | `MSGID_REQUEST_CREATENEWACCOUNT` | `0x40b890` | client |
| 0x0FC94203 | `MSGID_RESPONSE_LOG` (relay from WLS) | `0x409520` | WLS |
| 0x0FC94204 | `MSGID_REQUEST_CREATENEWCHARACTER` | `0x408a00` | client |
| 0x0FC94205 | `MSGID_REQUEST_ENTERGAME` | `0x406840` | client |
| 0x0FC94207 | `MSGID_REQUEST_DELETECHARACTER` | `0x40adc0` | client |
| 0x0FC94210 | `MSGID_REQUEST_CHANGEPASSWORD` | `0x40caa0` | client |
| 0x0512A3F6 | `MSGID_REQUEST_REGISTERDBSERVER` | `0x40d360` (logs "(O) DB server connected!") | admin/DB tool |
| 0x13000001 | `MSGID_ACCOUNTINFOCHANGE` | `0x40d3e0` | admin, IP-checked |
| 0x14E91202 | `MSGID_REQUEST_ACCOUNTCONNSTATUS` | `0x40d930` | admin, IP-checked |
| 0x14E91204 | `MSGID_REQUEST_CLEARACCOUNTCONNSTATUS` | `0x40de50` | admin, IP-checked |
| 0x2900AD10 | `MSGID_REQUEST_BLOCKACCOUNT` | `0x40ebd0` | admin, IP-checked |
| 0x2900AD22 | `MSGID_ACCOUNTTIMECHANGE` | `0x40f720` | admin |
| 0x20000000 | `MSGID_SENDSERVERSHUTDOWNMSG` | `0x40f5d0` | WLS |
| 0x23AA210E | `MSGID_REGISTER_WORLDSERVER` | `0x405820` | WLS |
| 0x23AA210F | `MSGID_REGISTER_WORLDSERVERSOCKET` | `0x405cb0` | WLS |
| 0x23AB211F | `MSGID_REGISTER_WORLDSERVER_GAMESERVER` | `0x405110` | WLS |
| 0x23AB2201 | `MSGID_RESPONSE_CHARINFOLIST` | `0x406510` | WLS |
| 0x2400000A | `MSGID_REQUEST_REMOVEGAMESERVER` | `0x407f40` | WLS |
| 0x24021EE0 | `MSGID_REQUEST_CLEARACCOUNTSTATUS` | `0x408900` | WLS |
| 0x27049D0C | `MSGID_WORLDSERVERACTIVATED` | `0x40e630` | WLS |

All IDs match `SRV NetMessages.h` and `CLI NetMessages.h` values [V]. Client replies are sent with
`sub_414c40(msg, size, key)` [V]; key 0 for the replies checked (`push 0` before the call at `0x403b66`
etc.) [V].

Field cipher: `sub_411ac0` (called on account and password with the frame key, `0x403874`, `0x4038fe`)
would undo the client's `CMisc::bEncode` (`CLI Misc.cpp:253-262`), but it returns 1 at `0x411add`
before its loop, so fields are used as received [V]. The 2.20 client does not call `bEncode` on login
fields (grep) [V], so this is consistent.

## 3. Client-facing messages

### 3.1 LOGIN 0x0FC94201 (`0x4036f0`)

Reads 6 account 10, 16 password 10, 26 world name 30 [V `0x403811-0x403946`]. Steps [V]:
1. read the account file (section 4) through `sub_404400`, which returns the character count (-1 if no
   file) and the stored name, password, character names, status, dates;
2. world check: one of the 4 world slots (this+0x9c88[1..4]) must have that name (+0x328) **and** be
   activated (+0x38c == 1, set by `WORLDSERVERACTIVATED`); else `RESPONSE_LOG` 0x0FC94203 MsgType
   0x0A02 NOTEXISTINGWORLDSERVER [V `0x403abb-0x403b86`];
3. no file or name mismatch -> 0x0F17 NOTEXISTINGACCOUNT;
4. `sub_40eb20(account) == 1` (5-minute block list, section 7) -> 0x0F15 REJECT with three zero i32
   (body 18);
5. status 0 (block date not yet passed) -> 0x0F15 REJECT with block year, month, day i32 (body 18);
6. password mismatch -> 0x0F16 PASSWORDMISMATCH and the log line "(!) Account(%s) Password Wrong";
7. success: store the account in the client object, make a cookie `rand() % 1000000 + timeGetTime()`
   (+0x10), and send **`MSGID_REQUEST_CHARINFOLIST` to the WLS** of that world (section 5). The client's
   reply comes back from the WLS as a ready-made `RESPONSE_LOG` CONFIRM.

The CONFIRM header the MLS supplies carries **version words 4 and 0x60 (= 4.96)**, written at
`0x403eee` and `0x403f05` [V]. A 2.20 client requires 2 and 20 (`CLI GlobalDef.h:16-24`) [V].

Status byte (account status, client offset 10): `sub_404400` sets 0 when `account-block-date` is today
or later, 4 when `account-valid-date` is unset, else 2 [V `0x404c51-0x404c98`; meaning of 2/4 I].

### 3.2 CREATENEWACCOUNT 0x0FC94202 (`0x40b890`)

Creates `Account\AscII<c>\<account>.txt` [V]. Replies `RESPONSE_LOG` with 0x0F18 NEWACCOUNTCREATED,
0x0F19 NEWACCOUNTFAILED (cannot create file) or 0x0F1A ALREADYEXISTINGACCOUNT [V `0x40bdef`, `0x40bec8`,
`0x40c5af`]. **The password line is written as `Hack = <password>`** (string `0x451280`) instead of
`account-password = ` (`0x451af0`), at `0x40c0c5` [V]. Since login reads `account-password`, an account
made this way cannot log in until the file is edited ([PERSIST-006](../issues/persistence.md#persist-006-account-creation-is-broken-in-the-mls-binary); patch in [SETUP-007](../issues/setup-2.20.md#setup-007-mainserver-account-creation-writes-hack--byte-patch)) [I consequence]. Accounts were evidently created
by an outside tool (the README's HBmaster) [I].

### 3.3 CREATENEWCHARACTER 0x0FC94204 (`0x408a00`)

Reads the 2.20 client layout (6 name, 16 account, 26 password, 36 world 30, 66..76 gender, skin, hair
style, hair colour, underwear, STR, VIT, DEX, INT, MAG, CHR) [V copy of 0x47 = 71 bytes at `0x4091d2`].
Checks [V]: password against the account file ("Account password not match!" -> RESPONSE_LOG REJECT
type); each stat 10..14 and sum 70, plus a name check `sub_411b70` (rejects `,` `=` space newline and
similar) -> "HACKED character data!!!". Then forwards to the WLS (section 5). The client's reply comes
from the WLS through `RESPONSE_LOG` relay (`0x409520`); on 0x0F1C NEWCHARACTERCREATED the MLS appends
`account-character = <name>` (world "WS1") or `account-character-<world> = <name>` under
`[CHARACTERS]` in the account file [V strings `0x409922-0x409988`].

### 3.4 DELETECHARACTER 0x0FC94207 (`0x40adc0`)

Checks the account file and password, the connection state (`sub_407ac0`), and that the name is one of
the account's characters; **removes the name from the account file before** forwarding the request to
the WLS (`sub_409a30` at `0x40b42f`, forward at `0x40b5a0`) [V order]. The WLS deletes the character
file and replies 0x0F1F CHARACTERDELETED with the new list through the relay [V WLS `0x407cc0`].

### 3.5 ENTERGAME 0x0FC94205 (`0x406840`)

Reads the 2.20 layout: 6 name 10, 16 map 10, 26 account 10, 36 password 10, 46 level i32, 50 world 30,
80 command-line token 120 [V copies at `0x40696f-0x406a58`]. Outcome codes [V constants; branch
meaning I where noted]:

| Condition | Reply |
|---|---|
| world or map's game server not registered (`sub_4078a0` returns 1 / 2) | `RESPONSE_ENTERGAME` 0x0FC94206, 0x0F21 REJECT, reason 3 |
| `sub_4078a0` returns -2 | 0x0F21, reason 5 |
| `sub_404400` result `[ebp-0x4b0]` == 0 (I: blocked or unreadable) | 0x0F21, reason 4 |
| same result == 4 | `RESPONSE_LOG` 0x0A05 FORCECHANGEPASSWORD [V constant, I which file field drives it] |
| character not in the account or password wrong | 0x0F21, reason 4 |
| kind 0x0F1C (NEW), account not in game | 0x0F22 CONFIRM: 6 address 16, 22 port u16, **body 24, no game-server name**; then `SETACCOUNTINITSTATUS` to the WLS (6 account 10, 16 password 10, 26 level i32) |
| kind 0x0F1C, account in game | 0x0F20 PLAYING |
| kind 0x0F1D (force) | `sub_40e420`: `REQUEST_FORCEDISCONECTACCOUNT` 0x15000000 to the WLS, reply 0x0F23 FORCEDISCONN |

### 3.6 CHANGEPASSWORD 0x0FC94210 (`0x40caa0`)

Rewrites `account-password = %s` and `account-change-password = y m d` in `[ACCOUNT-STATUS]`; reply
`MSGID_RESPONSE_CHANGEPASSWORD` 0x0FC94211 with MsgType 0x0A00 success or 0x0A01 fail [V]. The client
ignores the MsgID and reads only the type (`client-log-protocol.md` 1) [V], so this works.

## 4. Account file

Path `Account\AscII<c>\<account>.txt`, `<c>` = decimal value of the account's first byte, built with
"Account", "\\", "AscII%d", ".txt" at `0x40394c-0x403a13` [V]. Read by `sub_404400` (keys at
`0x404969-0x404a37`); written by `0x40b890` (create), `0x40caa0` (password), `0x409520`/`0x40adc0`
(character list), `0x40d3e0` (valid date / status), `0x40f720` (valid time) [V strings].

Written on creation, in this order [V `0x40c043-0x40c555`]:

```
Account-generated:  Time(y/m/d/h/m)  IP(a.b.c.d)
[NAME]
account-name     = <account>
[PASSWORD]
Hack = <password>                     intended: account-password = <password>  (plaintext)
[ACCOUNT-STATUS]
account-valid-time = 0
account-valid-date = y m d
account-change-password = y m d
[ETC]
account-Gender   = ...
account-Age      = ...
account-Birth-Year = / -Month = / -Day =
account-RealName = 
account-SSN      = ...
account-Email    = ...
account-Quiz     = ...
account-Answer   = ...
account-created-by = <command-line token>
[CHARACTERS]
account-character  = <name>           world "WS1"; other worlds: account-character-<world> = <name>
```

Keys the reader uses: `account-password`, `account-name`, `account-Gender` (compared with "MALE"),
`account-Age`, `account-valid-date`, `account-valid-time`, `account-block-date`,
`account-change-password`, `account-character` / `account-character-%s` [V]. Admin messages also write
`account-status = ` [V `0x40d790`]. Passwords are plaintext, at most 10 bytes on the wire [V].

## 5. Main <-> World protocol

All WLS -> MLS messages are encrypted with a random key; MLS -> WLS use `sub_406000(world, id, type,
data, size, socket)`, also with a random key (`rand & 0xff`, `0x406207`) [V]. MsgType is 0x0F14 unless
noted. Offsets are body offsets.

| Dir | MsgID | Layout | Effect | Cite |
|---|---|---|---|---|
| W->M | `REGISTER_WORLDSERVER` 0x23AA210E | 6 world name 30; 36 world address 16; 52 world port u32 | new world slot (max 4; duplicate name refused and logged); client object type 2; reply below | MLS `0x405820`, WLS `0x404240` [V] |
| W->M | `REGISTER_WORLDSERVERSOCKET` 0x23AA210F | same | attach an extra socket to that world | MLS `0x405cb0`, WLS `0x4042e0` [V] |
| M->W | `RESPONSE_REGISTER_WORLDSERVERSOCKET` 0x280120A0 | 10 zero bytes, sent on that socket | WLS marks the socket usable | MLS `0x405b7e`, `0x405f0f`; WLS `0x40b3e0` [V] |
| W->M | `REGISTER_WORLDSERVER_GAMESERVER` 0x23AB211F | 6 world 30; 36 game server name 10; 46 address 16; 62 port u32; 66 map count u32; 70 maps 11 each | MLS stores game server + maps for ENTERGAME routing; duplicate name refused | MLS `0x405110`, WLS `0x402902`, `0x40afad` [V] |
| W->M | `WORLDSERVERACTIVATED` 0x27049D0C | 6 world name 30 | world +0x38c = 1; logins for this world now allowed | MLS `0x40e6d1`, WLS `0x40c627` [V] |
| W->M | `REQUEST_REMOVEGAMESERVER` 0x2400000A | 6 world 30; 36 game server name 10 | delete that server and its maps; its accounts cleared (`sub_408430`) | MLS `0x407f40`, WLS `0x403236` [V; account clear I] |
| W->M | `REQUEST_CLEARACCOUNTSTATUS` 0x24021EE0 | 6 account 10 | MLS clears the account's in-game entry, only if the sender is a world socket | MLS `0x408900` (type check `0x408971`), WLS several [V] |
| W->M | `SENDSERVERSHUTDOWNMSG` 0x20000000 | 6 "A3E9D1G0U7"; MsgType 0 | intended: mark the world shut down. The MLS compares the code against the message start (MsgID bytes), not offset 6, so it always logs "(XXX) WorldServerShutDown Msg Error!" | MLS `0x40f5ff-0x40f62d`, WLS `0x40b410` [V; "always" I] |
| M->W | `REQUEST_CHARINFOLIST` 0x23AB2200 | 6 account 10; 16 cookie u32; 20 17-byte CONFIRM prefix (upper u16, lower u16, status u8, 6 x u16 dates); 37 count u8; 38 account time i32; 42 IP time i32; 46 count x name 10 | WLS reads each character file and answers | MLS `0x40407f`, WLS `0x404370` [V] |
| W->M | `RESPONSE_CHARINFOLIST` 0x23AB2201 | 6 account 10; 16 cookie; 20 a whole client message: `RESPONSE_LOG` 0x0FC94203, type 0x0F14, then the 2.20 login CONFIRM payload (prefix, count, 65-byte slots, two time i32) | MLS finds the client by account + cookie and forwards bytes 20.. unchanged | WLS `0x4046f3`, MLS `0x406510` [V] |
| M->W | `REQUEST_CREATENEWCHARACTER` 0x0FC94204 | 6..76 the client request (71 bytes); 77 cookie u32; 81 count u8; 82 count x existing name 11 | WLS creates the file and answers | MLS `0x409274`, WLS `0x405850` [V] |
| M->W | `REQUEST_DELETECHARACTER` 0x0FC94207 | 6 name 10; 16 account 10; 26..29 cookie [I]; 30 count u8; 31 count x name 11 (body 25 + 11N) | WLS deletes and answers | MLS `0x40b5a0`, WLS `0x407cc0` [V sizes, I field 26] |
| W->M | `RESPONSE_LOG` 0x0FC94203 (relay) | 6 account 10; 16 cookie; 20 inner MsgID u32; 24 inner MsgType u16; 26 inner payload | MLS updates the account file on 0x0F1C (adds) and forwards the inner message to the client | MLS `0x409520`, WLS `0x405c93-0x4082ff` [V] |
| M->W | `REQUEST_SETACCOUNTINITSTATUS` 0x25000198 | 6 account 10; 16 password 10; 26 level i32 | WLS INIT state (section 6 of `official-worldserver.md`) | MLS `0x40719f`, WLS `0x40a9a0` [V] |
| M->W | `REQUEST_FORCEDISCONECTACCOUNT` 0x15000000 | 6 account 10 | WLS forwards to the owning game server, or clears the account if none | MLS `0x40e581`, `0x40fb16`; WLS `0x40ab30` [V] |

Registration sequence [V both sides]: the WLS opens 10 sockets; the first sends REGISTER_WORLDSERVER
then one REGISTER_WORLDSERVER_GAMESERVER per known game server, the others send
REGISTER_WORLDSERVERSOCKET; the MLS answers each with RESPONSE_REGISTER_WORLDSERVERSOCKET on that
socket. Game servers registering later are forwarded one by one. Pressing HOME on the WLS sends
WORLDSERVERACTIVATED; only then does LOGIN for that world succeed.

Not used by either binary: `REGISTERDBSERVER` reply, `ANNOUNCEACCOUNT` 0x13000000,
`IPINFOCHANGE`, `ANNOUNCEACCOUNTNEWPASSWORD`, `IPIDSTATUS`, `NOCOUNTINGSAVELOGOUT`, `IPTIMECHANGE`,
`REQUEST/RESPONSE_IPTIME` (no immediates found) [V].

## 6. Account connection state

The MLS keeps its own table at this+0x9c9c (10 000 entries, 0x60 bytes) through `sub_408430(mode,
account, world, ...)`: mode 1 creates or updates an entry (world, time), mode 0 deletes it [V calls;
field layout I]. ENTERGAME uses it to answer PLAYING; `CLEARACCOUNTSTATUS` from a WLS and
`REMOVEGAMESERVER` delete entries. The fine-grained INIT / WAIT / PLAYING / NULL machine lives in the
WLS (`official-worldserver.md` 6); the MLS only knows "in game in world X" or not [I from the call
pattern].

## 7. Admin and billing messages

[V strings and IDs; purpose I]
- `ACCOUNTINFOCHANGE`, `ACCOUNTCONNSTATUS` (reply 0x14E91203), `CLEARACCOUNTCONNSTATUS` (reply
  0x14E91205), `BLOCKACCOUNT` (5-minute block, reply 0x14E91205) are accepted only from the hard-coded
  addresses 203.234.215.99/.170/.182/.183/.184/.186/.188 (`0x40d448-0x40d4dc`); Korean log text "attempt
  from unpermitted IP". A hard-coded account `romeo87920` appears in these handlers (`0x40da1e`).
- `ACCOUNTTIMECHANGE` writes `account-valid-time = %d`.
- A status line counts Korean and foreign users by IP (`203.234.215.`, `211.51.62.243/245`, `0x40fc8c`).

## 8. Ghidra comments and helper map

Plate comments set in the Ghidra project (program MainServer.exe) at: `0x403220`, `0x4036f0`,
`0x404400`, `0x405110`, `0x405820`, `0x405cb0`, `0x406000`, `0x406510`, `0x406840`, `0x407f40`,
`0x408430`, `0x408900`, `0x408a00`, `0x409520`, `0x40adc0`, `0x40b890`, `0x40caa0`, `0x40e630`,
`0x40f5d0`, `0x402770`, `0x403eee`.

CRT helpers identified from use [I]: `sub_415630` memset, `sub_416620` memcpy, `sub_415550` strcat,
`sub_415540` strcpy, `sub_416590` strcmp, `sub_415dc0` strncmp/memcmp, `sub_416550` rand,
`sub_415780` operator new, `sub_412f20` log line, `sub_414c40` XSocket send, `sub_406000` send to WLS.
Pseudo-C reconstructed from the disassembly: `decomp/MainServer/`.

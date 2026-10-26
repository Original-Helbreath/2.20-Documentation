# Review: official log server docs (servers/official-mainserver.md, official-worldserver.md, official-compat-220.md, servers/decomp/)

Method: read-only. Bytes read with Python from `.claude/scratch/v382core/MainServer.exe` (393 216 bytes) and
`WorldServer.exe` (106 496 bytes). Message IDs compared with the four `NetMessages.h` files by script. Ghidra HTTP API
**did respond** on 127.0.0.1:8089 (programs: WorldServer.exe with 432 functions, MainServer.exe with 47 functions, as the
docs say); I used `get_function_by_address`, `list_functions` and `decompile_function` for WorldServer.exe. MainServer.exe has
no function at any handler address the doc names (`0x4036f0`, `0x403220`, ...), so its handler claims could not be
decompiled; they were checked only through bytes, strings and immediates, and the rest is **unverified (no decompiler
output for MainServer.exe)**. Severity as in gate.md. The binaries were not run.

## A. The five byte patches (official-compat-220.md section 7): all verified

| File | Offset | Doc old bytes | Bytes found | Context bytes | Verdict |
|---|---|---|---|---|---|
| MainServer.exe | 0x3EF1 | `04` | `04` | `66 c7 00 04 00` at 0x3EEE (`mov word [eax],4`, imm16 at 0x3EF1) | verified |
| MainServer.exe | 0x3F08 | `60` | `60` | `66 c7 00 60 00` at 0x3F05 (imm16 at 0x3F08); 0x60 = 96, so "4.96" is right | verified |
| MainServer.exe | 0xC0C6 | `80 12 45 00` | `80 12 45 00` | `68 80 12 45 00` (push) at 0xC0C5 = VA 0x40C0C5; string at VA 0x451280 is `Hack = `; new bytes `F0 1A 45 00` = VA 0x451AF0 which holds `account-password = ` | verified |
| MainServer.exe | 0x51224 | `211.115.208.` | `211.115.208.` + 4 NUL | pushed at VA 0x4028A0 with `6a 0c` (compare length 12) | verified |
| WorldServer.exe | 0x221D | `E3` | `E3` | `3d e3 a3 12 05` at 0x221C = `cmp eax, 0x0512A3E3`; new `F4` gives 0x0512A3F4 | verified |

Also verified: the version call `3, 51` (`68 33 00 00 00 6a 03` before the title push at 0x4128E7), title strings
(`HB Main-Log-Server v%d.%d Extasis fix (Snoopy81)` at 0x45229C; `Helbreath World-Log-Server V%d.%d` at 0x418B3C),
the five internal-port allow-list strings and their compare lengths (pushes at 0x4028A0/C6/EC, 0x402912, 0x402938 with lengths
12, 10, 12, 12 and 11: `211.115.208.`, `80.32.153.`, `203.234.215.`, `80.32.96.48.`, `.80.32.153.`), the seven admin addresses
(`203.234.215.99/.170/.182/.183/.184/.186/.188`, strings at 0x451E60-0x451ED8) and `211.51.62.243/.245`, `romeo87920` at
0x451F38, `A3E9D1G0U7` at 0x4520A8 (MLS) and 0x418A3C (WLS), config key strings at 0x451040-0x451090 and
`HMLserver.cfg` at 0x451030. The sample configs (`HMLServer.cfg`: 2981, 3840; `WLserver.cfg`: 2881, 3840) match the docs.

## B. Message IDs

- 21 MLS dispatcher IDs: every ID in the section 2 table occurs as a 32-bit immediate in the dispatcher region
  (0x4032A8-0x403406; `0x20000000` at 0x40339F), and every hex value matches `NetMessages.h`.
- 87 name/hex pairs across the three docs (plus the WLS dispatcher table, config-push IDs `0x0FA314D9..DE`, `0x0FA40001..08`,
  `0x17081034/35`, `0x167C0A31/32`, `0x25000198/99`, `0x2654203A`, `0x15000000`, `0x0DF30760`, `0x0DF30751`, `0x210A914E..50`)
  match the headers; **0 mismatches**. The only IDs absent from every header are the WLS registration ID `0x0512A3E3` and
  `0x13000000` (`ANNOUNCEACCOUNT`, named as unused).
- Note for readers: `0x0FA314DB` (Magic.cfg push) is also `MSGID_EVENT_COMMON`, and `0x0FA314DC` (Skill.cfg push) is also
  `MSGID_COMMAND_COMMON` in the client/server headers (see shared.md S1); the docs do not mention it.

## C. Ghidra / decompile spot-checks (WorldServer.exe, 6 handlers; 5 required)

| # | Function (address) | Claim in doc | Decompiled result | Verdict |
|---|---|---|---|---|
| 1 | `HandleRequestEnterGame` 0x405440 | reads name @6, map @16, account @26, ignores MsgType/password/level; map not found -> 0x0F21 reason 3, body 7; reason 5 branch unreachable; CONFIRM 0x0F22, address 16 + port u16, body 24; account must exist else close with no reply | reads `param_2+6/0x10/0x1a` (10 bytes each); `FindGameServerByMap` returns `0xff` (-1) or a low byte of 0 (never -2), so the `-2` branch is dead; `0xf21` + reason 3 + size 7; `0xf22`, 16 + 2 bytes, size `0x18`; `GetAccountStatus == 0` -> close, no reply | verified |
| 2 | `CheckAccountStatusTimeout` 0x40AA30 | every 10th tick, INIT or WAIT older than 20 000 ms, 5000 entries at this+0xA06C, sends CLEARACCOUNTSTATUS to MLS then NULL | counter `> 9` resets, state byte `== 1 or 2`, `20000 <`, loop of 5000 at +0xA06C, `SendMsgToMLS(0x24021ee0, 0xf14, ..., 10)` then `SetAccountStatus(0)` | verified |
| 3 | `HandleRegisterGameServer` 0x402510 | name 10 @6, address 16 @16, port u16 @32, map count signed @34, maps 11 each @35; duplicate name / 100 servers rejected; configs sent in the listed order; flags last; CONFIRM after | offsets match; 100-slot loops; **file order differs** (see W1) | verified except order |
| 4 | `HandleEnterGameConfirm` 0x4033F0 | account @6, password @16, server name @26, level @52; mismatch -> FORCEDISCONNECT 0x15000000/0x0F14, 16-byte body | offsets and constants match (`param_2+0x34` = 52; `0x15000000`, `0xf14`, size `0x10`) | verified (detail in W2) |
| 5 | `HandleSavePlayerData` 0x404E80 and `HandleRequestPlayerData` 0x403640 | name @6, account @16, count flag @36, text @37; reply `0x0C152211`, guild byte @17, text @18, size len+19, REJECT 0x0F15 | `param_2+6/0x10/0x14`, `*(char*)(param_2+0x24)`, `param_2+0x25`; `0xc152211`, `0xf14`, byte at +0x11, text at +0x12, size `len+0x13`; `0xf15` path present | verified |
| 6 | `HandleAcceptSocket` 0x4019E0 | up to 9 999 sockets; internal-port peers compared 16 bytes against 10 `game-server-list` entries; client-port not filtered; messages 0x52C.. | slots 1..9999; four 4-byte compares against this+0x68, 10 entries, only when the accept came from the internal listener; message `slot + 0x52C` | verified (detail W3) |

Also checked: all 43 `` `Name` `0xADDR` `` pairs in official-worldserver.md match the function names in Ghidra (43 of 43), and exactly 51 non-library
functions carry the renamed handler names ("51 handlers renamed" is exact). `MainServer.exe`: 47 functions, as stated.

## D. Issues

| # | Doc | Entry | What is wrong | Evidence | Severity |
|---|---|---|---|---|---|
| W1 | servers/official-worldserver.md | 3.1 Registration, step 3 (lines 107-113) | Gives the config push order as Npc, Item, Item2, **Item3**, Magic, Skill, **Potion**, Quest, BuildItem, DupItemID, Noticement. The actual call order in `HandleRegisterGameServer` is Npc, Item, Item2, Magic, Skill, **Item3**, Quest, BuildItem, DupItemID, Noticement, **Potion**, then the two flag files. The doc's "[V file order 0x402c08-0x402f14]" refers to the filename table inside `SendConfigFileToGameServer` (0x402be0), which is a different order from the calls. "(any failure -> REJECT)" is attached to Noticement.txt but covers all eleven `&&`-chained sends including Potion.cfg. `servers/decomp/WorldServer/HandleRegisterGameServer.c` already has the right order, so the two documents contradict each other. | decompile of 0x402510: calls with `DAT_004175a4` (Npc.cfg), `DAT_00417598` (Item.cfg), `DAT_0041758c` (Item2.cfg), Magic, Skill, `DAT_00417568` (Item3.cfg), Quest, BuildItem, DupItemID, Noticement, Potion (`s_Potion_cfg_00417520`); strings read from the file at VA 0x4175A4, 0x417598, 0x41758C, 0x417568, 0x417520 | wrong |
| W2 | servers/official-worldserver.md | 3.3 `ENTERGAMECONFIRM` | "if the account is INIT or WAIT and password and level equal those stored, or the account is unknown -> PLAYING". The code sets PLAYING for **every** state except an INIT/WAIT mismatch, so an account already in PLAYING (3) is also overwritten with the new server name. On a failed send of the FORCEDISCONNECT reply the handler also calls `HandleGameServerConnectionLost` and closes that socket (not mentioned). | decompile of 0x4033F0: `if ((iVar2 == 1) \|\| (iVar2 == 2)) { ... mismatch -> force-disconnect; return }` then unconditional `SetAccountStatus(this,3,...)` | imprecise |
| W3 | servers/official-worldserver.md | 1, window-message ranges | "0x52C..0x2C3B = accepted sockets": the message is `slot + 0x52C` with slot 1..9999, so the range is 0x52D..0x2C3B (0x52C is never used). | decompile of 0x4019E0 (`iVar6 + 0x52c`, loop from `iVar6 = 1`) | imprecise |
| W4 | servers/official-compat-220.md | 7, intro sentence | "File offset = VA - 0x400000 in both files (raw and virtual section offsets are equal) [V section table]". True for `.text`, `.rdata`, `.data` in both files and therefore for all five patches, but **false for other sections**: MainServer `.idata` (VA 0x5E000, raw 0x5B000) and `.reloc` (0x60000 / 0x5D000), and WorldServer's fourth section `.khe\x07` (VA 0x1D000, raw 0x1A000, raw size 0). The doc also does not mention that unusual section. | PE section tables read with Python (`bytes.py`) | imprecise |
| W5 | servers/official-worldserver.md, official-compat-220.md | 3.4 table / section 4 | `SAVE*OCCUPYFLAGDATA` (0x167C0A31/32) is listed as sent by a game server, and compat section 4 has no row for it (only the 0x17081034/35 push is marked "match"). The 2.20 game server never sends these messages (send case in a block comment, all callers commented out; `SaveOccupyFlagData` returns first), so with this WLS the crusade occupy flags are never saved. This is a functional gap for the "usable with patches" verdict. | `srv220/Game.cpp:3762-3764, 4424-4468, 47997` (see shared.md F5) | missing |
| W6 | servers/official-compat-220.md | 3, "(SRV Game.cpp:5290, 5334, 18646-18667)" | 5334 is the `game-server-port` key; the `internal-log-server-port` key test is at 5336 (and its value is stored at 5290). | `srv220/Game.cpp:5333-5339` | imprecise |
| W7 | servers/decomp/MainServer/*.c, servers/official-mainserver.md | whole file | The 47 Ghidra functions of MainServer.exe do not include any handler; all MLS handler addresses, offsets and branch structure in the doc and in `decomp/MainServer/*.c` rest on a capstone listing that I could not reproduce (no capstone module here, no Ghidra functions). Spot-checks that were possible (IDs, strings, patch sites, allow-list lengths, version words, the `Hack =` push) all hold. | `list_open_programs`: MainServer.exe `function_count 47`; `get_function_by_address 0x4036f0` -> none | unverified (not an error) |

### Other verified statements (no issue)

- official-mainserver.md: HMLserver.cfg keys, internal and public ports, allow-list string list with the 11-char dead entry, `Hack =` writer at 0x40C0C5, version words at 0x403EEE/0x403F05, client `bEncode` at `Misc.cpp:253-262` and `GlobalDef.h:16-24` version 2.20, client login layout 56 bytes (`Game.cpp:1682-1701`), hard-coded 2848 (`Game.cpp:15543-15556`), admin address list and status-line addresses, `A3E9D1G0U7` constant.
- official-compat-220.md: sections 1, 2, 4 and 5 statements that I could test: registration ID differs (F4 vs E3), `DEF_CHINESE` at `GlobalDef.h:16` and `DEF_DBGAMESERVER`/`DEF_DEFENCEHACKING` at 72-78, the start check list at `Game.cpp:3693-3701`, the `WLSERVERFILECONTENTS` wait at 15180-15187, `SETACCOUNTWAITSTATUS` send at 2852, WLS never pushes 0x0FA40005-08.
- official-worldserver.md: dispatcher table (IDs and handler names) and the config push IDs; player-data offsets; state-machine table address and timeout; registration body offsets and the 100-server and 100-map limits; the MLS forward layout also appears consistently in `decomp/WorldServer/HandleRegisterGameServer.c` and `official-mainserver.md` section 5.

## Totals

wrong 1 (W1), imprecise 4 (W2, W3, W4, W6), missing 1 (W5), unverified-by-tool 1 (W7).
Patches: 5 of 5 verified. Message IDs: 87 pairs + 21 dispatcher immediates, 0 errors. Ghidra: reachable; 6 handlers decompiled (5 required), 43 of 43 name/address pairs match.

## Items checked

5 byte patches (offset, old bytes, surrounding instruction bytes, new-string targets); 21 MLS dispatcher immediates; 87 name/hex pairs; 43 Ghidra name/address pairs; 6 decompiled handlers (about 40 offsets and constants); 12 binary strings/addresses (titles, config keys, allow-list strings with lengths, admin addresses, constants); 2 PE section tables; 8 source cites in the compat doc. Not checked: MLS handler bodies (login, create account, delete character, enter game), WLS character/guild file layouts (`HandleCreateNewCharacter`, guild handlers) beyond the file-name strings, and the `decomp/MainServer` pseudo-C.

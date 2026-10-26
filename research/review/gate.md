# Review: gate docs (gate/functions.md, subsystems.md, data-structures.md; servers/gate-server.md, game-gate-protocol.md)

Method: read-only comparison against `git show 0b994cd:<file>` of `sources/gateserver-librehelbreath` (files
extracted to `.claude/scratch/review/gate/` to get line numbers; GateCore.cpp 1274 lines, XSocket.cpp 607,
Wmain.cpp 422, PartyManager.cpp 316 - these match every range in the docs). Severity: **wrong** = the source
contradicts the doc; **imprecise** = right in spirit, wrong in a detail, range or cite; **missing** = real
behaviour the docs do not record. `verified` = read at the cited line. Nothing was run.

## A. Issues in the new gate docs

| # | Doc | Entry | What is wrong | Evidence | Severity |
|---|---|---|---|---|---|
| G1 | gate/functions.md | 2 PartyManager, row `bAddMember` (line 204) | States the old-party step "decrements that party and deletes it at <= 1 member". The code decrements and deletes the **target** party (`iPartyID`, the function argument), not the party that held the name. The old party's slot is cleared but its `m_iMemberNumList` count is not decremented (it stays one too high), and the destination party is decremented and, if that leaves 1 or 0, deleted (broadcast code 2) before the name is added. Rejoining one's own party can therefore delete it. | `PartyManager.cpp:130` `m_iMemberNumList[iPartyID]--;`, `:131` `bDeleteParty(iPartyID)`; the found slot's own id is zeroed at 125 and never used | wrong |
| G2 | gate/functions.md | 1.3 row `RegisterGameServerHandler` and note; subsystems.md 3 table ("name already held by another slot") | "Name already used by any other slot (509-520)". The scan covers slots 1..499 **including the registering peer's own slot**. A second REGISTER on the same connection (name stored by the first, accepted or rejected) fails as "already exists" and sets `m_sIsActivated = -2` even for a server that was accepted; a registrant whose 10-byte name is all zero matches any peer that has not registered (their `m_cName` is zeroed). | `GateCore.cpp:509-511` (no `i != iClientH` test); `Client.cpp:19` zeroes `m_cName` | wrong |
| G3 | gate/functions.md | 1.3 `RegisterGameServerHandler` / `ResponseRegisterGameServer` | Not recorded: `ResponseRegisterGameServer` can delete the peer on a fatal send result and set `m_pClientList[iClientH] = NULL` (`GateCore.cpp:539-577`), yet both reject paths call it **first** and then dereference the slot (`m_pClientList[iClientH]->m_sIsActivated`, `->m_cName`). A fatal send result on the 6-byte reply is a NULL dereference in the gate (crash). Trigger is narrow (sends before the first FD_WRITE are queued, so it needs a full queue or dead socket afterwards). | `GateCore.cpp:498` then `:501, :504` (build-date path) and `:512` then `:515, :518` (duplicate-name path) | missing |
| G4 | gate/functions.md Conventions + 5 `iSendMsg`; subsystems.md 2.4 | `iSendMsg` return values | Docs say the queued result is -2 and the "fatal send group" is the error set. Before the first `FD_WRITE` the function returns `_iRegisterUnsentData() - 3`: 1 -> -2, but **0 (ring full) -> -3 and -1 (alloc failure) -> -4**. Neither equals any `DEF_XSOCKEVENT_*` value (those are -121..-136), so no caller's fatal switch matches and a full ring before the first write is never detected. `iSendMsg` also returns `SOCKETMISMATCH` (-121) for a peer whose socket type is no longer NORMAL; no sender tests it. | `XSocket.cpp:447-453`, `:427-429`; `XSocket.h:33-48` | imprecise |
| G5 | gate/functions.md section 5 `_iOnRead`; subsystems.md 2.3 | oversize limit | "a length above 30000 -> MSGSIZETOOLARGE". The test is on the body: `m_dwReadSize = length - 3` is compared with `m_dwBufferSize` (30000), so the u16 length must exceed 30003. data-structures.md section 5 ("Max total 30003") is right, so the docs disagree with each other. | `XSocket.cpp:203, 213` | imprecise |
| G6 | gate/subsystems.md | 5 "Weak points" | The list omits the wrong-id decrement in `bAddMember` (G1), which is the only weak point that corrupts member counts on the normal "join a second party" path. | `PartyManager.cpp:130-131` | missing |

### Verified exactly (no issue found)

- Function inventory: 78 definitions = 24 CGateCore + 2 CClient + 11 PartyManager + 18 Wmain + 19 XSocket + 4 CStrTok. Every start line in functions.md matches a definition. `StrTok.*` appear in no project file (`GateSever.dsp`/`.vcproj`: no `StrTok`; only `StrTok.cpp` includes `StrTok.h`); `WINMAIN.H:10` declares `OnKeyUp` with no definition.
- **Slot 399 vs `WM_USER+500`**: `WM_ONCLIENTSOCKETEVENT + 399 = WM_USER + 500 = WM_USER_TIMERSIGNAL` (`UserMessages.h:2-3`, `Wmain.cpp:26`); the timer `case` is first in `WndProc` (`Wmain.cpp:80-82`), the default branch only handles messages not matched earlier, so slot 399 events are swallowed. The `bAccept` loop is `i = 1..499` (`GateCore.cpp:238`). Verified.
- **Unbounded allow-list**: `strcpy(m_cAddress[iIPindex], token); iIPindex++;` with no bound (`GateCore.cpp:125-126`), array `char m_cAddress[10][16]` (`GateCore.h:71`), following members `m_iBuildDate`, `iTotalWorldServerClients`, `m_cMonitorStatus` (`GateCore.h:73-76`). Verified.
- Registration flags: `m_bIsGameServer = TRUE` at `GateCore.cpp:494` before the build-date and name checks; lines 534-536 unreachable; port and address parsed but unused.
- Shutdown arithmetic (9 steps, ~189 s from F1+F4 at 20 s gate and 3 s tick), F6 and F7 falling through to F12 (`GateCore.cpp:648-666`), early `return` after a delete in `CheckGlobalCommand`, `SendMsgToMonitor` (both) and `CheckAutoShutdownProcess`, `GameServerDown` single call site (`:221`).
- data-structures.md: all 22 constant rows and cites (`GateCore.h:26-29,71`, `Client.h:17`, `XSocket.h:50`, `PartyManager.h:19-20`, `GateCore.cpp:26,630,1050`, `PartyManager.cpp:286,291`, `Wmain.cpp:30,54,238`, `0x5A8E` and passphrase sites 311/318/368/376/395); all 17 MSGID values and `NetMessages.h` line numbers in section 8 (227, 228, 305, 308, 309, 322, 347, 348, 383-386, 389, 390, 395, 396, 413) and `DEF_MSGTYPE_*` (4-5); the two `MSGID` collision pairs named there exist (`0x2900AD30`, `0x0FA314DB`; a third pair `0x0FA314DC` also exists, see shared.md S1).
- Wire layouts in data-structures.md section 7: registration offsets 6/16/32/34/35, alive count at 6, party request (body 20), item log (body 53), monitor message 28 bytes, all reply "written/sent" sizes, reply code table.
- Wmain facts: 3 s timer, log ring 50 x 120, 35 painted lines, y positions 605/10/25/40/620/640/655, title and 620 x 700 size, `_mkdir` per write (`Wmain.cpp:384`), `bListen` result unchecked (`:235`).

## B. Contradictions: new gate docs versus the older two

The 14 rows of gate/subsystems.md section 10 were all re-checked and the **source supports the new docs in every row** (older doc wrong). Additional contradictions or errors not in section 10:

| # | Older doc | Entry | Older doc says | Source and new docs say | Evidence | Severity |
|---|---|---|---|---|---|---|
| C1 | servers/game-gate-protocol.md | 5, request type 4 | "party deleted when empty (type 2)" | deleted when the count drops to **1 or fewer** (`<= 1`) | `PartyManager.cpp:167`, `:131` | wrong |
| C2 | servers/gate-server.md 9; game-gate-protocol.md 8 | build-date constants | cites `SRV GlobalDef.h` | `DEF_BUILDDATE` is in `Version.h` (InterServer 306, USA 222, Korea 207, Chinese 429, else 213) | `.claude/scratch/srv220/Version.h:1-30`, included from `Game.h:37` | wrong |
| C3 | servers/game-gate-protocol.md | 4 stock table | `GSM_WHISPERMSG` (0x0D), `GSM_SETFORCERECALLTIME` (0x12) | header spells `GSM_WHISFERMSG` and `GSM_REQUEST_SETFORCERECALLTIME` | `srv220/NetMessages.h` (GSM block, lines 453-470) | imprecise |
| C4 | servers/game-gate-protocol.md | 2 `MSGID_GAMESERVERALIVE` gate column | "stores the count, forwards to monitors" | also sets `m_sIsActivated = 1` when it is >= 0 (revives a silent server; works for any peer, registered or not) | `GateCore.cpp:442-443` | imprecise |
| C5 | servers/game-gate-protocol.md | 5 note under the request table | "the gate also records which game server sent it (by socket) so the reply goes back there" | `m_iIndex` is stored only for create and add; every reply goes to the **current** requester (`iClientH`); the stored index is used only by `GameServerDown` | `PartyManager.cpp:56-62`, `:136-141`; `GateCore.cpp:1148-1182` | imprecise |

Older-doc claims checked and correct: registration body size `39 + 11*M + 4` (`SRV Game.cpp:20703`), alive every 3 s skipped without Log link, 6-byte shutdown messages, `GAMESERVERDOWN` 28 bytes, `PARTYOPERATION` 50 bytes sent, 20 used, `SRV GateServer.cfg` having only address and port, `H1`/`H2` verdicts, cfg key cites 107-145.

## Totals

New gate docs: wrong 2 (G1, G2), imprecise 2 (G4, G5), missing 2 (G3, G6).
Older docs (not edited): wrong 2 (C1, C2), imprecise 3 (C3, C4, C5).

## Items checked

78 function definitions (start line by grep; behaviour read for every function in GateCore.cpp, PartyManager.cpp, Wmain.cpp, XSocket.cpp, Client.cpp); 22 constants; 17 message IDs and their header lines; 14 section-10 corrections; 12 older-doc claims; about 60 caller/callee line cites compared individually. Not checked: compiler-layout claims tagged [I] (the allow-list overflow target and the 20-byte `m_cName` read spilling into `m_cAddress`), which depend on MSVC member padding; effects tagged [I] in the docs were not re-argued.

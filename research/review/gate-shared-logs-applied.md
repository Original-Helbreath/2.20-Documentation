# Applied review findings: gate, shared, log servers

Sources: `gate.md`, `shared.md`, `log-servers.md` in this folder. Each row re-checked at the cited line
before editing. Catalogue rows were fixed in `.claude/scratch/shared-ref/gen_messages.py`
(`strip_comments`, `PARENT_OVERRIDE`) and regenerated, not hand-edited.

## gate (12 rows: 11 applied, 1 rejected)

| Row | Result | Where / note |
|---|---|---|
| G1 | applied | gate/functions.md `bAddMember` + section 2 note; subsystems.md section 5 weak points (re-checked `PartyManager.cpp:130-131`) |
| G2 | applied | gate/functions.md 1.3; subsystems.md section 3 table and text (`GateCore.cpp:509-511`) |
| G3 | applied | gate/functions.md 1.3 both rows; subsystems.md section 3. Lines re-numbered per `git show 0b994cd`: 499/502 and 512/515 (review said 498/512, off by one) |
| G4 | applied | gate/functions.md Conventions and `iSendMsg`; subsystems.md 2.4 (`XSocket.cpp:447-453`) |
| G5 | applied | gate/functions.md `_iOnRead`; subsystems.md 2.3 (`XSocket.cpp:203,213`) |
| G6 | applied | subsystems.md section 5 weak points |
| C1 | applied | servers/game-gate-protocol.md request type 4 |
| C2 | applied | servers/game-gate-protocol.md section 8, servers/gate-server.md 9: `Version.h`, `Game.h:37` |
| C3 | applied | servers/game-gate-protocol.md stock table (`GSM_WHISFERMSG`, `GSM_REQUEST_SETFORCERECALLTIME`) |
| C4 | applied | servers/game-gate-protocol.md `MSGID_GAMESERVERALIVE` row |
| C5 | applied | servers/game-gate-protocol.md section 5 note |
| (data-structures collision list) | applied | gate/data-structures.md now names all three pairs (follows S1) |
| "not checked" [I] items | rejected | Not findings; nothing to apply |

## shared messages (5 rows: 4 applied, 1 partly rejected)

| Row | Result | Note |
|---|---|---|
| S1 | applied | README Hex bullet names the three pairs and says hex is not a unique key |
| S2 | applied (one sub-claim rejected) | Generator now strips `/* */`; regenerated (client-game 229, client-log 39, game-log 49, game-game 18, game-gate 17, main-world 11, admin-log 6, unused 21 = 390). README text and counts updated. SAVE*OCCUPYFLAGDATA now unused. `DEF_NOTIFY_SKILLTRAINFAIL` Game 2.20 now `-`. **Rejected sub-claim:** `MSGID_RESPONSE_INITPLAYER` Game 2.03 `Game.cpp:5798` is live code (the enclosing block comment 5278-5656 ends before it), so the hit stays |
| S3 | applied | `PARENT_OVERRIDE`: `DEF_LOGRESMSGTYPE_*` -> `MSGID_RESPONSE_LOG`, `DEF_ENTERGAMEMSGTYPE_*` -> `MSGID_REQUEST_ENTERGAME`, `DEF_ENTERGAMERESTYPE_*` -> `MSGID_RESPONSE_ENTERGAME`; README documents it |
| S4 | applied | `GSM_*` parent -> `MSGID_SERVERSTOCKMSG` |
| S5 | applied | README Docs column states the `research/servers` restriction |

## shared formats (9 rows: 9 applied, 0 rejected)

| Row | Result | Note |
|---|---|---|
| F1 | applied | amd-map.md: index at `Map.cpp:480`, `m_sTileDataSize` at 479, repeats at 694 and 702, heap overrun for SizeY > SizeX |
| F2 | applied | amd-map.md: client `m_tile[752][752]` (`MapData.h:65`) |
| F3 | applied | mapdata-txt.md: end marker optional (3 of 59: 47 `.txt` + 12 `.TXT`); function at 25556 |
| F4 | applied | server-content-cfg.md Npc.cfg: 24 fields, placeholder removed (`cReadModeB` 1..24, reset at 19991) |
| F5 | applied | server-program-cfg.md OccupyFlag: never saved in 2.20 (`Game.cpp:4424-4468, 47997`) |
| F6 | applied | server-content-cfg.md Item.cfg field 2: cp949 "Giant Sword" prefix |
| F7 | applied | client-files.md intro |
| F8 | applied | server-program-cfg.md logs: `DebugMsg.log` (`Wmain.cpp:486`) |
| F9 | applied | server-program-cfg.md `game-server-map` row: 5296-5308, abort conditions |

## log servers (7 rows: 6 applied, 1 recorded as unverified)

| Row | Result | Note |
|---|---|---|
| W1 | applied | official-worldserver.md 3.1: call order Npc, Item, Item2, Magic, Skill, Item3, Quest, BuildItem, DupItemID, Noticement, Potion (checked against `decomp/WorldServer/HandleRegisterGameServer.c`, which agrees; the `0x402510` decompile itself was not re-run); "any failure -> REJECT" covers all eleven |
| W2 | applied | official-worldserver.md 3.3 ENTERGAMECONFIRM (re-verified by decompile of `0x4033f0`: PLAYING for every state except INIT/WAIT mismatch; fatal send closes socket) |
| W3 | applied | official-worldserver.md section 1: 0x52D..0x2C3B (decompile of `0x4019e0`: slot from 1, `+0x52c`) |
| W4 | applied | official-compat-220.md section 7 (section tables re-read with Python: `.idata`, `.reloc`, `.khe\x07`) |
| W5 | applied | official-compat-220.md section 4 new row; official-worldserver.md 3.4 table note |
| W6 | applied | official-compat-220.md section 3: 5336 (re-checked `Game.cpp:5333-5339`) |
| W7 | rejected as an edit target | Not an error; recorded as a caveat in official-mainserver.md Method (handler data unverified, no decompiler output) |

Also added: official-worldserver.md 3.1 note on the shared push IDs 0x0FA314DB/DC (log-servers.md section B note).

## Counts

| Area | Applied | Rejected |
|---|---|---|
| gate (G1-G6, C1-C5, collision list; 12 rows incl. the not-a-finding item) | 11 | 1 (not a finding) |
| shared messages (S1-S5) | 5 (S2 with one sub-claim rejected: INITPLAYER 2.03 5798 is live) | 0 whole rows |
| shared formats (F1-F9) | 9 | 0 |
| log servers (W1-W7) | 6 | 1 (W7: unverified caveat recorded, not an error) |

## Not checked / caveats

- Generated files were not hand-edited; `gen_messages.py` was the only file outside `research\` touched.
  A copy of the pre-regeneration catalogue is in `.claude/scratch/shared-ref/msgs_before/`.
- G3/G4 effect lines tagged [I] (trigger conditions) were not re-argued.
- No tool was run against the gate or log server binaries beyond read-only Ghidra decompile requests (WorldServer.exe via `program=` parameter; no program switch).

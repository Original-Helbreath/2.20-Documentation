# Round 2 applied: gate, shared, log servers

Findings of `gate-shared-logs.md` (R2-01..R2-16), each re-checked against source first. Doc paths under `research/`.
Generator: `.claude/scratch/shared-ref/gen_messages.py`; regenerated catalogue diffed to `.claude/scratch/shared-ref/msgs_before_r2/`
(row-level diff: `.claude/scratch/fix2/rowdiff.out`). Totals: 16 applied (R2-05 as an open question, not as a fix), 0 rejected.

| id | result | doc file:line | reason / evidence |
|---|---|---|---|
| R2-01 | applied | `shared.md:10, 12, 14, 23`; `shared/messages/README.md:61, 63, 68` | counts now 237 / 39 / 41 / 22 (main-world 10, gate 17, game 18, admin 6; sum 390), equal to `grep -c '^| 0x'` per file. The review's 229 / 49 / 21 were from before the fixes below |
| R2-02 | applied | generator `DEF_RE` (`0[xX]`); `shared/messages/client-game.md` row `DEF_NOTIFY_CANNOTCONSTRUCT` | `NetMessages.h` defines it `0X0BA1`; row is now 0x0BA1 and sorts after 0x0B9A. 0 value conflicts still |
| R2-03 | applied | `servers/world-log-server.md:61` | 3840 cited from `Lserver.cfg:5` / `WLserver.cfg:8`; `gserver.cfg:8, 11` shown as placeholders 111 / 22 (verified) |
| R2-04 | applied | `servers/world-log-server.md:117` | `Game.cpp:5336` is the `internal-log-server-port` test, 5334 is `game-server-port` (verified) |
| R2-05 | applied as open question | `world-log-server.md:45-52, 60`; `topology.md:66`; `official-mainserver.md:51-56`; `official-worldserver.md:41-45`; `official-compat-220.md:113-115, 143, 150-152` | pack cfgs verified (`v382core/WLserver.cfg:9-10` 2981, `HMLServer.cfg:2-4` 2981 public / 3840, `WLserver.cfg:7` 3840; the review's `WLserver.cfg:11` is line 10). Recorded as unresolved; patch 4 is not called unnecessary, only "needed if the WLS dials the internal port" |
| R2-06 | applied | `topology.md:40-42`; `world-log-server.md:37-39` | receive side [V]; WLS sending 0x0FA40007 re-tagged [I] (`official-compat-220.md` 6: absent in the WLS) |
| R2-07 | applied | `servers/gate-server.md:29, 36, 55, 77-78, 80-82, 96, 111`; `servers/recreation-plan.md:37`; `gate/subsystems.md:245` | verified in `gate219`: `seps = "= ,\t\n"` (`GateCore.cpp:79`), no allow-list bound, `DEF_MAXPARTY 5000` pool, F6 falls through F7 to F12 (`:648-666`), only the two shutdown requests check the passphrase, `_mkdir` at `Wmain.cpp:384`. Plan step now optional |
| R2-08 | applied | `topology.md:67` | row labelled as the old single Log server; key sets cross-referenced |
| R2-09 | applied | `servers/recreation-plan.md:26, 89-96, 101` | `GlobalDef.h:72-78` (CHINESE/JAPAN/USA set both flags), `Version.h` dates and minor versions checked; official-binary route added |
| R2-10 | applied | `servers/recreation-plan.md:50` | `Noticement.txt` 0x0FA40004 (`Game.cpp:15250`) optional; `WLserver.cfg` DEFENCEHACKING-only |
| R2-11 | applied (doc note, generator header) | `shared/messages/unused.md:6`; `shared/messages/README.md` caveats | note added to the generated `unused.md` and README; no outgoing-message scan added. Both messages verified in `official-mainserver.md:159, 171` |
| R2-12 | applied | generator `SLASHED`; `unused.md:22-23` | the two SAVE*OCCUPYFLAGDATA rows now carry the Official cell (`official-worldserver.md:81`) and `off` in Ver |
| R2-13 | applied | generator `NO_INHERIT`; `unused.md` (INPUTKEYCODEFAILED row); README caveat | row left main-world (back to 10), now in unused |
| R2-14 | applied | `servers/game-gate-protocol.md:79` | `Game.cpp:49744-49754` case has no `break`, `default` sets `bFlag = TRUE` (verified); sender batching unverified, said so |
| R2-15 | applied | `servers/official-compat-220.md:88` | 0x0FA314DD = `MSGID_PLAYERITEMLISTCONTENTS` (`NetMessages.h:243`) |
| R2-16 | applied | `servers/topology.md:70-72` | one sentence: ports are config, sample values named |

## Doc-discrepancy fixes (done in the generator, `research/systems/` untouched)

| item | result | evidence |
|---|---|---|
| dead client commands filed under Log link (`REQ_TRAINSKILL`, `REQ_GETOCCUPYFLAG`, `REQUEST_HELP`, `BANGUILD`, `NOTIFY_HELP`, `HELPFAILED`, `MSGID_ADMINUSER`, `MSGID_MANAGER_SHUTDOWN`) | applied: moved game-log -> client-game, basis rewritten (`DEAD_CLIENT_LINK`) | all sit in the client branch of `MsgProcess` / `ClientCommonHandler` (`Game.cpp:14966-15088, 15415, 15471, 15541`) or `SendNotifyMsg` (`17903, 17930, 36785, 49281, 49286`); no reference in `sources/client-2.20/*.cpp` (grep) |
| `SUCCESSBANGUILDMAN`, `CHARISMA` (and `CANNOTJOINMOREGUILDSMAN`, `LIMITEDLEVEL`) marked used | applied: stay in client-game, basis says server never sends (`NEVER_SENT`) | `Game.cpp:36782` call with no `SendNotifyMsg` case; `18906, 18948, 18838` packing cases only, no caller (grep) |
| `CLEARGUILDNAME` parent | applied: parent `MSGID_EVENT_COMMON` (`EXACT_PARENT`) | four sends as `MSGID_EVENT_COMMON` (`Game.cpp:16779, 19078, 19149, 36789`); value 0x0A25 also `REQ_GETOCCUPYFIGHTZONETICKET` (`NetMessages.h:56, 59`), noted in README |

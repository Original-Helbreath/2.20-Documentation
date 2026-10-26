# Game-server review findings: applied / rejected

Each review row was re-checked against `.claude/scratch/srv220/` before editing. Edit scripts: `.claude/scratch/applier/a_*.py` (all ran with 0 failed replacements).

## Applied (by review file and doc)

- part1 rows 1-81 and G1-G3 -> startup.md, network.md, character.md, combat-magic.md, items.md, mechanics.md (G1: a `cfg:` note was added at the top of each function doc; G3: `_bItemLog` inactive-block notes).
- part2 rows 1-66 -> npc.md, map-movement.md (CMap placeholders replaced with text read from Map.cpp), crusade-quests.md, guild-party.md, chat-gm.md, data-structures.md. Row 64 (active/inactive note) is covered by the per-entry notes.
- part3 B1-B22 -> build-config.md (switch chains, versions, TESTSERVER/TAIWAN/KOREA/CONSULTATION details, dead-name overloads, function-total derivation).
- P1-P3 -> persistence.md. T1-T6 -> timers.md (ThreadProc dead via `G_pGame = NULL` assignment; 300 ms base tick; delayed events).
- U1-U12 -> ui-logging.md (per-overload `_bItemLog` caller lists: handle form vs name form, active vs inactive; OnKeyUp member vs empty free function; log file names).
- S1-S19 -> support.md (XSocket framing/buffers/block limit, CMisc direction helpers, ~49 constructor/destructor placeholders replaced, Initialize/OnDestroy).
- I1, I2 -> research/game-server.md.

## Rejected or only partly applied (code disagreed with the review)

- part1 row 32: down-skill fallback applies only when the picked skill is at 0; doc kept to that narrower statement.
- part1 row 43: reviewer mislabelled which party is the criminal; doc kept per code.
- part1 row 58 and G4: stale, the docs already list those notifies.
- part1 row 77: wording "not meant for party" contradicted by the code comment; not applied.
- part2/map: bGetMoveable is not "teleport/water/farm-aware"; doc states the real checks (20-tile edge margin, owner, move-allowed, temp-blocked).
- bSetItemToBankItem at 27858 does contain `iCalcTotalWeight`; the reviewer's claim that it did not was rejected.
- I3 (commit id 1697e41): unverified, left as is.

## Not done

- items.md ReqRepairItemHandler [I] repair formula was not upgraded to [V].
- Exact per-row counts were not tallied row by row after context compaction: applied is every row above except the rejections listed, so roughly 7 rejected plus 1 unverified out of about 250 rows (derived, not measured).

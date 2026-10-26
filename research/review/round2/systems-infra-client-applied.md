# Applied log: systems infra-client (round 2)

Framing per coordinator guidance: intent vs current behaviour, neutral wording; exploit commentary removed
(SE-1 / SE-4 / AD-6 rewritten as "no login check" / "applies only to" facts).

| id | status | doc:location | note |
|---|---|---|---|
| NE-1 | applied | network.md ~194 (section 7) | lag cadence: last-receive stamp, ~7 s under silence; verified `CLI Game.cpp:4288-4301` |
| NE-2 | applied | network.md 2.1 intent, section 9 "Unknown message" | "same source lineage, three builds"; "after init" |
| NE-3 | applied | network.md 2.1-2.6 | tagged [I] |
| SE-1 | applied | session.md section 9 bullet 1 | (a)-(c) added, neutral; decomp HandleEnterGameConfirm checked |
| SE-2 | applied | session.md sec 1, F-table 2.6, section 7, lag row | 11 ticks; lag cadence |
| SE-3 | applied | session.md design intent | tagged [I], rephrased |
| SE-4 | applied | session.md F4 + section 10 | consequence recorded, WLS handler [U]; speculation about victim reset removed |
| SE-5 | applied | session.md section 9 bullet 2 | `Game.cpp:1507` guard |
| SE-6 | applied | session.md F1 | gateway scope note |
| PE-1 | applied | persistence.md section 9 bullet 1 | offset shift, text mode |
| PE-2 | applied | persistence.md section 7 | load-time cleanup + refund |
| PE-3 | applied | persistence.md section 8 | 6097/6237 calls, 6099/6239 logs |
| PE-4 | applied | persistence.md intents | [I], GlobalDef.h:268 quoted |
| PE-5 | no change | - | already [V doc] |
| ST-1 | applied | server-topology.md section 9 bullet 1 | game-server-list |
| ST-2 | applied | server-topology.md F4 | counter reset semantics |
| ST-3 | applied | server-topology.md intents | [I]; 2.9 rewritten |
| ST-4 | applied | server-topology.md section 9 | 15180 added |
| ST-5 | applied | server-topology.md F5, section 7 | ~19 s |
| ST-6 | applied | server-topology.md section 9 | gate tail note |
| AD-1 | applied | admin.md section 9 | gap checks act in function |
| AD-2 | applied | admin.md table rows (94, 132) | deleted + refunded |
| AD-3 | applied | admin.md intents | [I] |
| AD-4 | handoff: research/game-server/mechanics.md:48 | "character", tumbling window; admin.md F4 stated correctly now |
| AD-5 | applied | admin.md row 92, section 9 | elapsed-time wording |
| AD-6 | applied | admin.md section 9 | no admin-level test, bAccept only |
| RE-1 | applied | rendering.md painter's window | |
| RE-2 | applied | rendering.md eviction | |
| RE-3 | applied | rendering.md intro | [I] |
| UI-1 | applied | ui.md section 9 | bullet restated |
| UI-2 | applied | ui.md | 35792 |
| UI-3 | applied | ui.md | lag note |
| UI-4 | no change | - | recorded as checked |
| AU-1..AU-4 | applied | audio.md section 9 / section 5 | rain, slider loop flag, sweep gating, music-on |
| AU-5 | not applied | audio.md | optional, low conf, not added |
| CS-1 | applied | client-state.md 172, 210 | |
| CS-2 | no change | - | keep [I] |
| CS-3 | applied | client-state.md 108 | |
| CS-4 | applied | client-state.md intent | [I] |
| X-1, X-2, X-4, X-5 | applied | all four lag docs; persistence/admin; topology; persistence 217 | |
| X-3 | handoff: mechanics.md (= AD-4) | |
| extra | applied | server-topology.md 2.3 intent | config pushes come from log server; cites config-files.md, Game.cpp:15201-15267 |

Handoffs (docs not owned): mechanics.md:48 (AD-4). NE-1 lag claim in other docs: none beyond the four owned
(network, session, ui, client-state), all fixed.
Counts (rows): applied 41 (incl. extra, X), rejected 0, handoff 2 (AD-4, X-3), no change 3 (PE-5, UI-4, CS-2), not applied 1 (AU-5).

# Split log: infra-client

Issue files created in `research/issues/` (entry counts): network 8, session 8, persistence 6, server-topology 9, admin 12, rendering 13, ui 4, audio 7, client-state 9, client-resolution 7, gate 26, setup-2.20 17. All links were checked by script (anchors resolve).

## Entries (id: source)

- NET-001..007: systems/network.md section 9 (lines 224-249 before the edit); NET-008: systems/network.md section 7 (gate peers bullet, line 201-202).
- SESS-001..008: systems/session.md section 9 (254-293).
- PERSIST-001..006: systems/persistence.md section 9 (194-218). WLS-checks-no-password bullet merged into SESS-001.
- TOPO-001..009: systems/server-topology.md section 9 (228-261). World user cap bullet merged into SESS-007.
- ADMIN-001..012: systems/admin.md section 9 (217-249). `_Manager_Shutdown` merged into TOPO-007.
- RENDER-001..005, 007, 009: systems/rendering.md section 9 (151-163); RENDER-002..008 also client/rendering/porting-notes.md "Bugs" (99-110); RENDER-007, 008, 010, 011, 012: client/rendering/directdraw.md "Known defects" (254-268); RENDER-013: client/rendering/blitters.md:117.
- UI-001..004: systems/ui.md section 9 (171-187); UI-003 also client/functions/input.md:26.
- AUDIO-001..007: systems/audio.md section 9 (106-126).
- CLIENT-STATE-001..007: systems/client-state.md section 9 (197-216); -004 also client/functions/world-map.md:32, client/data-structures.md:55; -005 client/functions/networking.md:28; -008 client/functions/characters.md:46; -009 client/functions/support.md:24,30,31,37.
- CLIENT-RESOLUTION-001..005: client/resolution/change-plan.md "Bugs found on the way" (67-78); -001 also window-setup.md:108; -006 client/resolution/method.md:36 and values-screen-game.md:32-35; -007 window-setup.md:58-61.
- GATE-001..024: gate/subsystems.md (2.1, 1, 2.4, 3, 4, 5, 6, 8, 9, 10), gate/functions.md (header, 1.1-1.7, 2), gate/data-structures.md (21, 22, 129, 206-209). GATE-025: servers/gate-server.md:42. GATE-026: servers/game-gate-protocol.md:79.
- SETUP-001..017: servers/official-compat-220.md (1-8), systems/server-topology.md section 9, client/build-config.md, client/assets.md, servers/client-log-protocol.md 1, systems/crafting.md (potion keyword).

## Docs edited

systems/network.md, session.md, persistence.md, server-topology.md, admin.md, rendering.md, ui.md, audio.md, client-state.md (section 9 renamed "Current behaviour vs intent" with link lines; network.md section 7 gate-peers bullet); client/rendering/porting-notes.md (Bugs table -> "Current behaviour: bugs to replicate or fix"), directdraw.md ("Known defects" -> "Current behaviour: known defects"), frame-pipeline.md, blitters.md; client/resolution/change-plan.md, window-setup.md, variables.md; client/functions/characters.md, input.md, networking.md, support.md, world-map.md; client/data-structures.md, build-config.md, assets.md; gate/subsystems.md, functions.md, data-structures.md; servers/gate-server.md, game-gate-protocol.md, game-log-protocol.md, client-log-protocol.md, official-mainserver.md, official-worldserver.md, official-compat-220.md (pointer line to setup-2.20.md only; the patch table is kept as documentation).

Left unchanged: gate.md (index only), client/resolution/method.md "Known limits" (method notes, not defects), values-*.md and coincidental*.md (data tables).

## Possible duplicates for the main session

- ADMIN-008 (shut-up units) and CHAT-009 (same defect).
- CLIENT-STATE-008 (bCheckExID overflow) and the chat system.
- SETUP-015 (potion keyword) and crafting; SETUP-012 relates to CHAT-001/CHAT-003.
- GATE-026 (stock-message loop fall-through) with any game-server network/stock-message entry.
- NET-004/005/008 are the canonical entries for the gate wire defects; gate docs link to them rather than to GATE entries.

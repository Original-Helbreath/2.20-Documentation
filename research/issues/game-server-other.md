# Issues: game-server-other

Scope: defects and divergences found in the game-server function maps, mechanics page and build-config page that belong to economy, social, crusade and infrastructure systems rather than to the world-combat systems (items, shops, chat and GM commands, guilds, parties, crusade, quests, networking, persistence, logging, build switches). IDs use the prefix GSO; each entry names its intended system. Entries that duplicated another system's issue were merged into that system's entry and left here as stubs. `SRV` = `sources/hgserver-librehelbreath/src/`; [V] verified, [I] inferred, [U] unverified.

### GSO-001 Monster-drop party test lowers the threshold for nearly every kill
- **Program**: game server
- **System**: item (monster drops)
- **Kind**: bug
- **Intent**: the source comment says party play should give a +10 % drop bonus.
- **Now**: the condition tests `m_iPartyStatus != DEF_PARTYSTATUS_PROCESSING`, which is true for status 0 (no party) and 2 (member) and false only for 1 (transient create/join/dismiss), so nearly every kill has the threshold lowered by 900: about 24.01 % instead of the 15.01 % a non-party player would otherwise get. The likely intended test is `== DEF_PARTYSTATUS_CONFIRM`.
- **Where**: SRV Game.cpp:44667-44671
- **Evidence**: [V]; [I] on the intended test
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/mechanics.md:231 (also :183); research/game-server/functions/items.md:562

### GSO-002 CMisc::bEncode and bDecode are no-ops

Merged into [NET-002](network.md#net-002-field-level-obfuscation-cmiscbdecode-is-a-no-op).

### GSO-003 The 2.20 source builds and reports as version 2.19
- **Program**: game server
- **System**: build (version reporting)
- **Kind**: build
- **Intent**: report v2.20.
- **Now**: Version.h under DEF_CHINESE reports "2"."19", build 429, so this "2.20" source reports itself as v2.19 (window title and version fields). Other regions report 2.20 (INTERSERVER build 306, USA 222, KOREA 207).
- **Where**: SRV Version.h; SRV Wmain.cpp:189-202
- **Evidence**: [V]
- **Blocks 2.20 setup**: unknown (matters if the client or log server checks the reported version)
- **Source doc**: research/game-server/mechanics.md:233; research/game-server/build-config.md:9

### GSO-004 Two GM enable commands are protected by hard-coded passwords in source

Merged into [ADMIN-003](admin.md#admin-003-gm-enable-passwords-are-compiled-in-literals).

### GSO-005 Monster gold drop dereferences the killer without a NULL check
- **Program**: game server
- **System**: item (monster drops)
- **Kind**: bug
- **Intent**: pay gold to the killer.
- **Now**: the gold branch dereferences `m_pClientList[killer]` without a NULL check.
- **Where**: SRV Game.cpp:44704
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/mechanics.md:183

### GSO-006 Charisma discount floor is off by one
- **Program**: game server
- **System**: shop
- **Kind**: bug
- **Intent**: the discount stops at half price.
- **Now**: when the discounted price would be at or below half of the undiscounted price, the discount is set so the price ends 1 below half (`cost2/2 - 1`).
- **Where**: SRV Game.cpp:16820-17092 (RequestPurchaseItemHandler); under DEF_V219 the crusade-winner floor at :16936-16940
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/mechanics.md:188

### GSO-007 Repair confirmation has no repairer-type check

Merged into [SHOP-006](shop.md#shop-006-repair-confirm-skips-the-npc-type-rule-sell-quote-skips-the-shop-zone-check).

### GSO-008 The drop probability DeleteNpc passes is used only by an inactive branch
- **Program**: game server
- **System**: item (monster drops)
- **Kind**: dead code
- **Intent**: a 1-in-65 (55 in Middleland) chance of an NPC item drop.
- **Now**: `DeleteNpc` passes `iItemprobability` = 65 (55 in Middleland), but the value is used only in the inactive non-DEF_NPCITEM branch (47260-47547) as a 1-in-N dice test; the active DEF_NPCITEM branch (47193-47258) never reads it.
- **Where**: SRV Game.cpp:30105-30110, 47187-47550
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/mechanics.md:185; research/game-server/functions/items.md:639

### GSO-009 Structure.cpp, CrusadeCore.cpp and rusadeCore.cpp are not built in the VC6 project
- **Program**: game server
- **System**: build
- **Kind**: build
- **Intent**: crusade code split across extra source files.
- **Now**: HGserver.dsp lists no .cpp for them; CrusadeCore.cpp is fully commented out. The CMake build globs every .cpp under src/ and so compiles them.
- **Where**: HGserver.dsp; sources/hgserver-librehelbreath/CMakeLists.txt:5
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/build-config.md:7

### GSO-010 DEF_CHINESECANDYEVENT has no effect
- **Program**: game server
- **System**: build
- **Kind**: dead code
- **Intent**: December 2002 candy drops from NPC types 55/61.
- **Now**: the only use is commented out.
- **Where**: SRV Game.cpp:44883
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/build-config.md:16

### GSO-011 Misspelt build switches leave blocks dead
- **Program**: game server
- **System**: build
- **Kind**: build
- **Intent**: DEF_TAIWAN / DEF_TAIWANLOG guards.
- **Now**: `DEF_TIAWAN` (Game.cpp:36665; also named in the `/closeconn` level check) and `TAIWANLOG` without the DEF_ prefix (Game.cpp:34585, `ReqRepairItemCofirmHandler`) are never defined, so those blocks are dead. `/closeconn` has a Taiwan admin-level branch under `DEF_TIAWAN` that can never compile (merged from COMMANDS-017). [V]
- **Where**: SRV Game.cpp:36665, 34585
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/build-config.md:53-54; research/game-server/functions/chat-gm.md:129; research/game-server/functions/items.md:5; research/systems/commands/overloaded.md:103; chat-commands.md:37

### GSO-012 Several functions exist only inside commented-out blocks
- **Program**: game server
- **System**: build
- **Kind**: dead code
- **Intent**: earlier versions of functions that now have a live twin.
- **Now**: inside `/* */`: bReadProgramConfigFile@5053, bCreateNewNpc@10368 (5-argument version), CalculateAttackEffect@14279, bCheckTotalSkillMasteryPoints@32645/32741/32811 (the Korean comment calls it the "problem infinite loop"), iGetPlayerABSStatus@37735 and the older two-argument iGetPlayerABSStatus(int iWhatH, int iRecvH) at 37560-37684, iGetNpcRelationship@37802. Each has a live overload or twin, which is why name-based caller scans merge them.
- **Where**: SRV Game.cpp as listed
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/build-config.md:62

### GSO-013 Start-up is gated on the WLserver file push under DEF_DEFENCEHACKING

Merged into [TOPO-002](server-topology.md#topo-002-defencehacking-is-a-world-server-security-switch-not-anti-cheat).

### GSO-014 Guild chat from other servers stops at the first uninitialised client

Merged into [CHAT-006](chat.md#chat-006-remote-guild-delivery-stops-at-the-first-uninitialised-client).

### GSO-015 RequestAdminUserMode has an empty body

Merged into [ADMIN-010](admin.md#admin-010-requestadminusermode-is-empty).

### GSO-016 bCheckBadWord is a stub

Merged into [CHAT-001](chat.md#chat-001-language-filter-is-client-side-only).

### GSO-017 /reservefightzone toggles a flag on every call
- **Program**: game server
- **System**: chat-gm (fightzone)
- **Kind**: bug
- **Intent**: reserve or free a fightzone.
- **Now**: every call toggles `m_iFightzoneNoForceRecall` (0/1) and notifies -5/-4, even with no argument; with an argument it flips zone N between reserved-by-GM (-1) and free (0).
- **Where**: SRV Game.cpp:36611-36617, 36626-36642
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/chat-gm.md:118

### GSO-018 /polymorph parses and does nothing

Merged into [ADMIN-011](admin.md#admin-011-polymorph-parses-and-does-nothing).

### GSO-019 Bad-word-monitor mute uses a different time unit and the notify sends 600

Merged into [CHAT-009](chat.md#chat-009-mute-units-disagree-between-shutup-and-the-bwm-monitor).

### GSO-020 Occupy flags can never be planted

Merged into [CRUSADE-001](crusade.md#crusade-001-occupy-flags-are-retired-but-still-wired).

### GSO-021 Occupy-flag persistence and removal code is dead

Merged into [CRUSADE-001](crusade.md#crusade-001-occupy-flags-are-retired-but-still-wired).

### GSO-022 Five NPC-talk stubs are never called

Merged into [QUEST-006](quest.md#quest-006-stub-giver-functions-are-never-called).

### GSO-023 Crusade-result loss log writes /10 but the award is /5

Merged into [CRUSADE-005](crusade.md#crusade-005-reward-arithmetic-drift).

### GSO-024 CreateCrusadeStructures indexes three mana stones unconditionally

Merged into [CRUSADE-006](crusade.md#crusade-006-mana-stone-selection-overflow).

### GSO-025 CollectedManaHandler guards the Elvine log line with the Aresden amount

Merged into [CRUSADE-009](crusade.md#crusade-009-collected-mana-log-guarded-by-the-wrong-sides-value).

### GSO-026 Meteor-strike damage formulas are overwritten

Merged into [CRUSADE-008](crusade.md#crusade-008-meteor-damage-formulas-are-overwritten).

### GSO-027 Only MONSTERHUNT and GOPLACE quests can complete
- **Program**: game server
- **System**: quest
- **Kind**: incomplete
- **Intent**: all configured quest types complete.
- **Now**: `_bCheckIsQuestCompleted` completes only MONSTERHUNT (location flag set and count >= max) and GOPLACE (location flag set and within range of x/y); every other quest type never completes in this function.
- **Where**: SRV Game.cpp:42088-42126 (42100, 42110)
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/crusade-quests.md:188

### GSO-028 The old party-create path is dead

Merged into [PARTY-005](party.md#party-005-dead-legacy-per-server-party-path).

### GSO-029 Guild join approval does not check the caller is a guildmaster

Merged into [GUILD-001](guild.md#guild-001-join-and-dismiss-approval-have-no-server-side-authority-check).

### GSO-030 Guild leave approval does not check membership of the caller's guild

Merged into [GUILD-001](guild.md#guild-001-join-and-dismiss-approval-have-no-server-side-authority-check).

### GSO-031 Guild co-op exp share is disabled

Merged into [GUILD-007](guild.md#guild-007-dead-or-unwired-guild-features).

### GSO-032 SendGuildMsg carries a developer note about wrong pointer arithmetic
- **Program**: game server
- **System**: guild
- **Kind**: bug
- **Intent**: send a guild event to every online member.
- **Now**: a source comment says "### BUG POINT" with the position wrong and pointer arithmetic wrong; whether it was fixed is not established.
- **Where**: SRV Game.cpp:20419-20495
- **Evidence**: [V] the comment; [U] the effect
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/guild-party.md:122

### GSO-033 Guild inter-server notify and the dismiss command are stubs

Merged into [GUILD-007](guild.md#guild-007-dead-or-unwired-guild-features).

### GSO-034 PartyOperationResultHandler bounds check uses && so it never fires

Merged into [PARTY-003](party.md#party-003-bounds-check-in-partyoperationresulthandler-case-3-is-never-true).

### GSO-035 /joinparty does not require the target to be the party master

Merged into [PARTY-002](party.md#party-002-joinparty-skips-consent-and-leaves-a-ghost-gate-member).

### GSO-036 ForceDisconnectAccount does not disconnect

Merged into [SESS-008](session.md#sess-008-force-disconnect-relies-on-client-co-operation).

### GSO-037 Manager-program shutdown command is a no-op

Merged into [TOPO-007](server-topology.md#topo-007-manager-shutdown-is-dead).

### GSO-038 Sub-log socket probe reads past the array when sockets are down
- **Program**: game server
- **System**: network (log connection)
- **Kind**: bug
- **Intent**: find a usable sub-log socket round-robin.
- **Now**: the probe loop does `index++` without wrapping, so when sockets are down it reads `m_pSubLogSock[10..19]` out of bounds before the 10-try cutoff.
- **Where**: SRV Game.cpp:43123-43154 (43137)
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/network.md:285

### GSO-039 Local fallback save writes the binary record in text mode

Merged into [PERSIST-002](persistence.md#persist-002-fallback-save-writes-binary-in-text-mode).

### GSO-040 ThreadProc never runs

Merged into [NET-006](network.md#net-006-game-server-multithreading-is-dead).

### GSO-041 DelayEventProcess is an empty stub and the DAMAGEOBJECT delay event does nothing
- **Program**: game server
- **System**: infrastructure (timers)
- **Kind**: dead code
- **Intent**: delayed damage events.
- **Now**: `DelayEventProcess` is empty; in `DelayEventProcessor` the DAMAGEOBJECT type is a no-op.
- **Where**: SRV Game.cpp:20414-20417, 33071
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/timers.md:38, :52

### GSO-042 Scroll depletion is never logged
- **Program**: game server
- **System**: item
- **Kind**: bug
- **Intent**: the Korean comment says magic scrolls must be logged on depletion.
- **Now**: `ItemDepleteHandler` logs only when `bIsLog` is set and the item type is not consume/eat/use-deplete/use-deplete-dest/material, and never for ids 380-382.
- **Where**: SRV Game.cpp:30510-30548
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/items.md:194

### GSO-043 _bItemLog null guard tests an array and the function can dereference NULL
- **Program**: game server
- **System**: logging
- **Kind**: bug
- **Intent**: skip logging when the giver is missing.
- **Now**: the guard `m_pClientList[iGiveH]->m_cCharName == NULL` tests an array and is never true, and it dereferences a possibly NULL client.
- **Where**: SRV Game.cpp:42704-42816 (42708)
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/ui-logging.md:68

### GSO-044 Most item-log call sites are inactive

Merged into [ITEM-007](item.md#item-007-dead-taiwanlog-item-log-pair-the-shipped-pair-logs-only-good-items).

### GSO-045 Unused logging and hotkey code
- **Program**: game server
- **System**: infrastructure (logging, window)
- **Kind**: dead code
- **Intent**: item-log file buffer; hotkeys.
- **Now**: `PutItemLogFileList` has no live caller (the only call is commented out, Game.cpp:42490); the free function `OnKeyUp` is empty and uncalled (WndProc calls the CGame member); in the member `OnKeyUp`, F2 and End are dead.
- **Where**: SRV Wmain.cpp:332-334, 418-458; SRV Game.cpp:32884-32973
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/ui-logging.md:160, :139, :30

### GSO-046 _bGetIsStringIsNumber accepts a minus sign anywhere and the empty string
- **Program**: game server
- **System**: infrastructure (config parsing)
- **Kind**: bug
- **Intent**: test that a token is a number.
- **Now**: returns TRUE if every character is a digit or '-' (a '-' anywhere), and the empty string counts as a number.
- **Where**: SRV Game.cpp:9966-9973 (9969-9972)
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/support.md:78

### GSO-047 CMisc::GetDirPoint steps the pointers instead of the values
- **Program**: game server
- **System**: infrastructure (helpers)
- **Kind**: bug
- **Intent**: offset a point one tile in a direction 1..8.
- **Now**: `*pY--` and `*pX++` step the pointer, not the value, so it changes nothing useful; no callers.
- **Where**: SRV Misc.cpp:216-229
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/support.md:180

### GSO-048 Unused helper functions in CMisc
- **Program**: game server
- **System**: infrastructure (helpers)
- **Kind**: dead code
- **Intent**: cursor position reader; one-off map-file converter.
- **Now**: `CMisc::GetMyCursorPos` has no callers; `CMisc::Temp` (converts middleland maps to a 824-wide header) has its only call commented out (Game.cpp:523).
- **Where**: SRV Misc.cpp:21-28, 299-369
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/support.md:153, :207

### GSO-049 CClient destructor does not free the warehouse list
- **Program**: game server
- **System**: session (memory)
- **Kind**: bug
- **Intent**: free all per-client allocations.
- **Now**: the destructor deletes the XSocket and every inventory item; the warehouse list is not freed there.
- **Where**: SRV Client.cpp:304-314
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/support.md:34

### GSO-050 PutLogList copies the message without a bound
- **Program**: game server
- **System**: infrastructure (logging)
- **Kind**: bug
- **Intent**: store a line of at most 120 characters.
- **Now**: the new line is written at slot 0 (120 chars) with no bound check on the `memcpy`.
- **Where**: SRV Wmain.cpp:287-296
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/ui-logging.md:116

# Split log: world-combat (round 2)

Group world-combat: issues moved out of the documentation into `research/issues/`. Entry ids with their source doc:line; then docs edited. Line numbers are those of the docs before the edit.

## issues/map.md (9 entries)

- MAP-001 Teleport request mode byte is the low byte of the message id - research/systems/map.md:195
- MAP-002 Forced recall case '0' has no body and no break - research/systems/map.md:200
- MAP-003 Enemy-town recall block ignores NOPENALTY; resurrection-zone check is dead - research/systems/map.md:203
- MAP-004 Crusade-winner shop discount price computed and never used - research/systems/map.md:205
- MAP-005 Enemy-building pre-check is client-side and compiled only for DEF_LANGUAGE 3 - research/systems/map.md:206
- MAP-006 Beginner-dungeon level 30-80 gate exists only in the client - research/systems/map.md:207
- MAP-007 Map without initial points leaves uninitialised coordinates - research/systems/map.md:211
- MAP-008 Old bReadProgramConfigFile is a dead commented block - research/systems/map.md:213
- MAP-009 CMap::iAnalyze is an empty stub - research/systems/map.md:215

## issues/tile.md (5 entries)

- TILE-001 Tile index is x + y*SizeY, wrong for non-square maps - research/systems/tile.md:111; possible duplicate of ITEM-002 (item)
- TILE-002 bGetMoveable assigns to its by-value pTopItem parameter (land-mine feature unfinished) - research/systems/tile.md:115
- TILE-003 ClearDeadOwner and ClearSectorInfo have no callers - research/systems/tile.md:120
- TILE-004 Client ignores the occupy status in move confirms - research/systems/tile.md:121
- TILE-005 NEWDYNAMICOBJECT and DELDYNAMICOBJECT notifies are never sent - research/systems/tile.md:123

## issues/movement.md (5 entries)

- MOVEMENT-001 Coordinate mismatch on a move is returned but never acted on - research/systems/movement.md:139
- MOVEMENT-002 Move-frequency check trusts the client-supplied timestamp - research/systems/movement.md:142
- MOVEMENT-003 OBJECTDAMAGEMOVE bypasses the frequency check and run SP cost - research/systems/movement.md:144
- MOVEMENT-004 Client locateable test reads one past the cell array - research/systems/movement.md:148
- MOVEMENT-005 Hunter portal blocks are hard-coded coordinates in the client - research/systems/movement.md:150

## issues/entity.md (11 entries)

- ENTITY-001 Taming is a stub and unreachable on land - research/systems/entity.md:155
- ENTITY-002 RemoveFromTarget turns every guild war unit into a 5-minute summon - research/systems/entity.md:158
- ENTITY-003 bSetNpcFollowMode and bSetNpcAttackMode use an uninitialised index - research/systems/entity.md:161
- ENTITY-004 SetSummonMobAction target mode: unbounded copy and unzeroed name buffer - research/systems/entity.md:163
- ENTITY-005 bCreateNewNpc declared BOOL but returns the slot index - research/systems/entity.md:166
- ENTITY-006 MobGenerator user-count scaling inactive; crusade break stops generation on later maps - research/systems/entity.md:168
- ENTITY-007 CalcNextWayPointDestination divides by zero - research/systems/entity.md:170
- ENTITY-008 bSerchMaster (master/follower packs) is dead - research/systems/entity.md:171
- ENTITY-009 Stale tile owner handles repaired lazily at read time - research/systems/entity.md:172
- ENTITY-010 TargetSearch self-skip break abandons the rest of the column - research/game-server/functions/npc.md:70
- ENTITY-011 _cGetSpecialAbility leaves the result uninitialised for some kinds - research/game-server/functions/npc.md:241

## issues/gathering.md (6 entries)

- GATHERING-001 Generator and processor loops use break where continue was meant - research/systems/gathering.md:140
- GATHERING-002 Crop count can never fall once a map reaches 200 - research/systems/gathering.md:143
- GATHERING-003 Planted crop may land on a non-farm neighbouring tile - research/systems/gathering.md:146
- GATHERING-004 Hoe-life test uses || where && was meant - research/systems/gathering.md:148; possible duplicate of ITEM-001 (item)
- GATHERING-005 Planting keeps a commented crusade-builder guard copied from construction code - research/systems/gathering.md:150
- GATHERING-006 Meat gathering (GET value 1) requires water and could never work on land - research/systems/gathering.md:152

## issues/combat.md (7 entries)

- COMBAT-001 Same-side town protection is compiled out in the selected build - research/systems/combat.md:193; also research/systems/pk-law.md:195 (same-side town protection off in the export)
- COMBAT-002 Cast motion carries a hard-coded magery value - research/systems/combat.md:198; also research/systems/magic.md:161
- COMBAT-003 Duplicated victim branches in ClientKilledHandler - research/systems/combat.md:201; also research/systems/pk-law.md:202
- COMBAT-004 CheckClientResponseTime breaks out of the client loop after a deletion - research/systems/combat.md:206; also research/systems/character.md:162
- COMBAT-005 Fightzone resurrection after 30 s is commented out - research/systems/combat.md:214
- COMBAT-006 Crusade kill-exp cut applies on every map - research/game-server/mechanics.md:230
- COMBAT-007 Hammer and staff combo bonuses borrow other weapons' tables - research/game-server/mechanics.md:118; research/game-server/functions/combat-magic.md:386

## issues/magic.md (3 entries)

- MAGIC-001 bCheckClientMagicFrequency is defined and never called - research/systems/magic.md:25
- MAGIC-002 Weather bonus table is incomplete - research/systems/magic.md:163
- MAGIC-003 Guild-war spell bans are compiled out - research/systems/magic.md:166

## issues/skills.md (5 entries)

- SKILLS-001 Skill 14 is hammer in code but cooking in Skill.cfg - research/systems/skills.md:133
- SKILLS-002 Skills 16, 17, 18, 20 and 22 are defined but unimplemented - research/systems/skills.md:136
- SKILLS-003 REQ_TRAINSKILL and SKILLTRAINFAIL are dead - research/systems/skills.md:139
- SKILLS-004 The 700-point cap is not enforced until a down-skill has been chosen - research/systems/skills.md:141
- SKILLS-005 The v2.19 SSN doubling applies only on the skill-index path - research/systems/skills.md:21

## issues/character.md (7 entries)

- CHARACTER-001 No max-level stop inside the level-up loop - research/systems/character.md:169
- CHARACTER-002 DEF_LEVELLIMIT is documented as a trial limit but used as the hunger threshold - research/systems/character.md:172
- CHARACTER-003 Unused per-stat flags in bCheckLevelUp - research/systems/character.md:175
- CHARACTER-004 LIMITEDLEVEL and CHARISMA notifies have no server sender - research/systems/character.md:177
- CHARACTER-005 Restart HP ignores the MaxHPdown divisor - research/game-server/functions/character.md:396
- CHARACTER-006 m_iLimitedUserExp is unused - research/game-server/functions/character.md:176
- CHARACTER-007 ResurrectPlayer has no live caller - research/game-server/functions/character.md:412

## issues/pk-law.md (7 entries)

- PK-LAW-001 Traveller flag is never cleared after citizenship - research/systems/pk-law.md:189
- PK-LAW-002 Jail routing tests location name, respawn routing tests map name - research/systems/pk-law.md:197
- PK-LAW-003 Force-recall residual-time branch is dead code - research/systems/pk-law.md:200
- PK-LAW-004 Traveller-on-innocent PK branch is mostly unreachable - research/systems/pk-law.md:204
- PK-LAW-005 Citizenship can be granted on any map - research/systems/pk-law.md:207
- PK-LAW-006 Bounty collection has no city-hall check on the server - research/systems/pk-law.md:172
- PK-LAW-007 Hunter-mode level thresholds disagree - research/game-server/functions/character.md:432; research/systems/pk-law.md:150

## issues/game-server-other.md (50 entries)

- GSO-001 Monster-drop party test lowers the threshold for nearly every kill - research/game-server/mechanics.md:231 (also :183); research/game-server/functions/items.md:562
- GSO-002 CMisc::bEncode and bDecode are no-ops - research/game-server/mechanics.md:232; research/game-server/functions/support.md:186, :192; research/game-server/functions/character.md:13; possible duplicate of NET-002 (network), possibly NET-001
- GSO-003 The 2.20 source builds and reports as version 2.19 - research/game-server/mechanics.md:233; research/game-server/build-config.md:9
- GSO-004 Two GM enable commands are protected by hard-coded passwords in source - research/game-server/mechanics.md:220
- GSO-005 Monster gold drop dereferences the killer without a NULL check - research/game-server/mechanics.md:183
- GSO-006 Charisma discount floor is off by one - research/game-server/mechanics.md:188
- GSO-007 Repair confirmation has no repairer-type check - research/game-server/mechanics.md:189; research/game-server/functions/items.md:297; possible duplicate of SHOP-006 (shop)
- GSO-008 The drop probability DeleteNpc passes is used only by an inactive branch - research/game-server/mechanics.md:185; research/game-server/functions/items.md:639
- GSO-009 Structure.cpp, CrusadeCore.cpp and rusadeCore.cpp are not built in the VC6 project - research/game-server/build-config.md:7
- GSO-010 DEF_CHINESECANDYEVENT has no effect - research/game-server/build-config.md:16
- GSO-011 Misspelt build switches leave blocks dead - research/game-server/build-config.md:53-54; research/game-server/functions/chat-gm.md:129; research/game-server/functions/items.md:5
- GSO-012 Several functions exist only inside commented-out blocks - research/game-server/build-config.md:62
- GSO-013 Start-up is gated on the WLserver file push under DEF_DEFENCEHACKING - research/game-server/build-config.md:23; research/game-server/functions/network.md:13; possible duplicate of PERSIST-001 (persistence)
- GSO-014 Guild chat from other servers stops at the first uninitialised client - research/game-server/functions/chat-gm.md:26; possible duplicate of CHAT-006 (chat)
- GSO-015 RequestAdminUserMode has an empty body - research/game-server/functions/chat-gm.md:56
- GSO-016 bCheckBadWord is a stub - research/game-server/functions/chat-gm.md:64
- GSO-017 /reservefightzone toggles a flag on every call - research/game-server/functions/chat-gm.md:118
- GSO-018 /polymorph parses and does nothing - research/game-server/functions/chat-gm.md:172
- GSO-019 Bad-word-monitor mute uses a different time unit and the notify sends 600 - research/game-server/functions/chat-gm.md:210; possible duplicate of CHAT-009 (chat)
- GSO-020 Occupy flags can never be planted - research/game-server/functions/crusade-quests.md:56, :67; possible duplicate of CRUSADE-001 (crusade)
- GSO-021 Occupy-flag persistence and removal code is dead - research/game-server/functions/crusade-quests.md:89, :96, :244, :343; possible duplicate of CRUSADE-001 and PERSIST-003
- GSO-022 Five NPC-talk stubs are never called - research/game-server/functions/crusade-quests.md:45
- GSO-023 Crusade-result loss log writes /10 but the award is /5 - research/game-server/functions/crusade-quests.md:313; possible duplicate of CRUSADE-005 (crusade)
- GSO-024 CreateCrusadeStructures indexes three mana stones unconditionally - research/game-server/functions/crusade-quests.md:351; possible duplicate of CRUSADE-006 (crusade)
- GSO-025 CollectedManaHandler guards the Elvine log line with the Aresden amount - research/game-server/functions/crusade-quests.md:370; possible duplicate of CRUSADE-009 (crusade)
- GSO-026 Meteor-strike damage formulas are overwritten - research/game-server/functions/crusade-quests.md:504; possible duplicate of CRUSADE-008 (crusade)
- GSO-027 Only MONSTERHUNT and GOPLACE quests can complete - research/game-server/functions/crusade-quests.md:188
- GSO-028 The old party-create path is dead - research/game-server/functions/guild-party.md:13, :161; possible duplicate of PARTY-005 (party)
- GSO-029 Guild join approval does not check the caller is a guildmaster - research/game-server/functions/guild-party.md:67; possible duplicate of GUILD-001 (guild)
- GSO-030 Guild leave approval does not check membership of the caller's guild - research/game-server/functions/guild-party.md:90; possible duplicate of GUILD-001 (guild)
- GSO-031 Guild co-op exp share is disabled - research/game-server/functions/guild-party.md:113; possible duplicate of GUILD-007 (guild)
- GSO-032 SendGuildMsg carries a developer note about wrong pointer arithmetic - research/game-server/functions/guild-party.md:122
- GSO-033 Guild inter-server notify and the dismiss command are stubs - research/game-server/functions/guild-party.md:133, :154; possible duplicate of GUILD-007 (guild)
- GSO-034 PartyOperationResultHandler bounds check uses && so it never fires - research/game-server/functions/guild-party.md:190; possible duplicate of PARTY-003 (party)
- GSO-035 /joinparty does not require the target to be the party master - research/game-server/functions/guild-party.md:241; possible duplicate of PARTY-002 (party)
- GSO-036 ForceDisconnectAccount does not disconnect - research/game-server/functions/network.md:215; possible duplicate of SESS-008 (session)
- GSO-037 Manager-program shutdown command is a no-op - research/game-server/functions/network.md:236
- GSO-038 Sub-log socket probe reads past the array when sockets are down - research/game-server/functions/network.md:285
- GSO-039 Local fallback save writes the binary record in text mode - research/game-server/functions/persistence.md:60; possible duplicate of PERSIST-002 (persistence)
- GSO-040 ThreadProc never runs - research/game-server/functions/timers.md:16; research/game-server/functions/support.md:400; possible duplicate of NET-006 (network)
- GSO-041 DelayEventProcess is an empty stub and the DAMAGEOBJECT delay event does nothing - research/game-server/functions/timers.md:38, :52
- GSO-042 Scroll depletion is never logged - research/game-server/functions/items.md:194
- GSO-043 _bItemLog null guard tests an array and the function can dereference NULL - research/game-server/functions/ui-logging.md:68
- GSO-044 Most item-log call sites are inactive - research/game-server/functions/ui-logging.md:78, :70; possible duplicate of ITEM-007 (item)
- GSO-045 Unused logging and hotkey code - research/game-server/functions/ui-logging.md:160, :139, :30
- GSO-046 _bGetIsStringIsNumber accepts a minus sign anywhere and the empty string - research/game-server/functions/support.md:78
- GSO-047 CMisc::GetDirPoint steps the pointers instead of the values - research/game-server/functions/support.md:180
- GSO-048 Unused helper functions in CMisc - research/game-server/functions/support.md:153, :207
- GSO-049 CClient destructor does not free the warehouse list - research/game-server/functions/support.md:34
- GSO-050 PutLogList copies the message without a bound - research/game-server/functions/ui-logging.md:116

Total entries: 115

## Docs edited

- systems/map.md - section 9 renamed Current behaviour vs intent, 9 links (MAP-001..009)
- systems/tile.md - section 9 renamed, 5 links (TILE-001..005)
- systems/movement.md - section 9 renamed, 5 links (MOVEMENT-001..005)
- systems/entity.md - section 9 renamed, 9 links (ENTITY-001..009)
- systems/gathering.md - section 9 renamed, 6 links (GATHERING-001..006)
- systems/combat.md - section 9 renamed, 5 links (COMBAT-001..005)
- systems/magic.md - section 9 renamed, line 25 link, 3 links (MAGIC-001..003, COMBAT-002)
- systems/skills.md - section 9 renamed, 5 links (SKILLS-001..005)
- systems/character.md - section 9 renamed, 5 links (CHARACTER-001..004, COMBAT-004)
- systems/pk-law.md - section 9 renamed, line 172 and hunter-mode links (PK-LAW-001..007, COMBAT-001, COMBAT-003)
- game-server/mechanics.md - section 13 renamed Current behaviour vs intent (4 links); inline links; CR-2 upgrade line points at ../systems/crafting.md
- game-server/build-config.md - inline links GSO-003, -009, -010, -011, -012, -013; dead-code list reduced to a neutral line
- game-server/data-structures.md - one link (CHARACTER-002); no other issue content found
- game-server/functions/map-movement.md - RequestTeleportHandler description made neutral (MAP-001, -002, -004); links TILE-002, -003, MAP-009, GATHERING-002
- game-server/functions/combat-magic.md - links MAGIC-002, COMBAT-007
- game-server/functions/character.md - links CHARACTER-005..007, PK-LAW-005, PK-LAW-007, GSO-002
- game-server/functions/npc.md - links ENTITY-001..003, 005..008, 010, 011, MAP-007
- game-server/functions/chat-gm.md, crusade-quests.md, guild-party.md, network.md, persistence.md, timers.md, support.md, ui-logging.md, items.md - links to GSO entries (and MAGIC-001, PK-LAW-003)

Link check: scripted validation of every issues link in the edited docs (anchors and relative depth) - verified, 0 bad.

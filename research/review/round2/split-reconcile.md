# Issues register reconciliation (round 2)

Method: heading kept on every non-canonical copy (old anchors still resolve); body replaced by "Merged into [ID](link)."; extra detail, locations and source docs appended to the canonical entry. Canonical entry sits in the file of the owning system. Scripts: .claude\scratch\split\reconcile\ (merge.py, fix.py, index.py, linkcheck.py); pre-merge copy in issues_backup\.

## Merges (kept id <- merged ids)

- issues/trade.md TRADE-008 <- WAREHOUSE-001
- issues/party.md PARTY-005 <- COMMANDS-002, GSO-028
- issues/crusade.md CRUSADE-001 <- COMMANDS-003, COMMANDS-013, GSO-020, GSO-021, PERSIST-003
- issues/crusade.md CRUSADE-004 <- COMMANDS-004
- issues/crusade.md CRUSADE-005 <- GSO-023; CRUSADE-006 <- GSO-024; CRUSADE-008 <- GSO-026; CRUSADE-009 <- GSO-025
- issues/guild.md GUILD-001 <- GSO-029, GSO-030
- issues/guild.md GUILD-007 <- COMMANDS-005, COMMANDS-014 (dissmiss part), GSO-031, GSO-033
- issues/guild.md GUILD-008 <- COMMANDS-009 (CANNOTJOINMOREGUILDSMAN part)
- issues/character.md CHARACTER-004 <- COMMANDS-009 (LIMITEDLEVEL, CHARISMA part)
- issues/admin.md ADMIN-011 <- GSO-018, COMMANDS-014 (polymorph part)
- issues/admin.md ADMIN-010 <- GSO-015, COMMANDS-012 (ADMINUSER part)
- issues/admin.md ADMIN-003 <- GSO-004
- issues/chat.md CHAT-009 <- ADMIN-008, GSO-019
- issues/chat.md CHAT-006 <- GSO-014; CHAT-001 <- GSO-016
- issues/server-topology.md TOPO-007 <- GSO-037, COMMANDS-012 (manager shutdown part); TOPO-002 <- GSO-013
- issues/network.md NET-002 <- GSO-002; NET-006 <- GSO-040
- issues/shop.md SHOP-006 <- GSO-007
- issues/party.md PARTY-003 <- GSO-034; PARTY-002 <- GSO-035
- issues/session.md SESS-008 <- GSO-036; SESS-004 <- COMMANDS-007
- issues/persistence.md PERSIST-002 <- GSO-039
- issues/item.md ITEM-007 <- GSO-044, ADMIN-009, COMMANDS-006
- issues/gathering.md GATHERING-004 <- ITEM-001 (the two readings differ on severity; kept as [U])
- issues/tile.md TILE-001 <- ITEM-002
- issues/quest.md QUEST-006 <- GSO-022
- issues/game-server-other.md GSO-011 <- COMMANDS-017
- issues/skills.md SKILLS-003 <- COMMANDS-001, COMMANDS-010
- issues/arena.md ARENA-008 <- COMBAT-005
- issues/gate.md GATE-013 (with GATE-014, GATE-017) <- PARTY-004 (stub points at all three)

Total: 49 stubs.

## Reviewed and not merged

- TRADE-007 vs GUILD-007 (different defects: form leak vs dead features).
- SHOP-010 vs MAP-004 (shop price test vs dead cPrice in teleport tail); no crusade entry covers SHOP-010.
- SETUP-003, -004, -016 are setup cross-reference entries; SETUP-016 link repointed from PERSIST-003 to CRUSADE-001.
- SETUP-015: no crafting entry exists; "possible duplicate" note replaced. GATE-026: no game-server stock-message entry exists; note removed.
- NET-004/005/008 kept as the gate wire entries; GATE-024 (header id collisions) differs from NET-003.
- CLIENT-STATE-008: no chat entry exists; note removed. ENTITY-001/SKILLS-002 only cross-reference.
- Remaining GSO entries have no equivalent elsewhere.

## Files with links changed

- research/systems/crusade.md (section 8 exp-cut line now links COMBAT-006).
- research/issues/setup-2.20.md (SETUP-016 -> CRUSADE-001).
- Stubs in other docs' links were left as they are: stub headings keep their anchors.
- New: research/issues.md (index). No top-level research index lists research docs, so none was edited.

## Link check

569 links to issues/ checked over research/ (excluding review/): 0 missing files, 0 missing anchors.

# Split log: economy-social

Docs edited (section 9 renamed "Current behaviour vs intent" with neutral lines and links; Contents updated where present): systems/item.md, crafting.md, shop.md, trade.md, warehouse.md, chat.md, party.md, guild.md, crusade.md, arena.md, quest.md; systems/commands/overloaded.md (dead table linked), chat-commands.md (3 rows + alias note), doc-discrepancies.md (4 rows linked). commands.md, by-system.md, method.md, msgids.md, common-types.md, notify-types.md, motion.md, gate-relay.md, other-subtypes.md: no change (catalogue tables; status columns kept as data). Inline edits also in party.md (section 3 dead line, F5, data row), guild.md (F1, section 4), crusade.md (F6, section 8, section 11), chat.md (section 11), quest.md (section 10).

Issue files created (107 entries): item 7, crafting 8, shop 10, trade 9, warehouse 2, chat 11, party 7, guild 9, crusade 9, arena 9, quest 8, commands 18.

## Entries (id: source doc:line before edit)
- ITEM-001..007: item.md:209, 211, 215, 217, 220, 222, 226
- CRAFTING-001..008: crafting.md:140, 144, 147, 149, 151, 154, 158, 160
- SHOP-001..010: shop.md:127, 130, 133, 136, 139, 141, 144, 146, 148, 150
- TRADE-001..009: trade.md:116, 120, 123, 125, 127, 129, 131, 133, 135
- WAREHOUSE-001..002: warehouse.md:91, 96
- CHAT-001..011: chat.md:164, 167, 170, 172, 176, 178, 180, 182, 184, 186, 188
- PARTY-001..007: party.md:144, 146, 153, 155, 157, 110, 79
- GUILD-001..009: guild.md:157, 162, 164, 167, 103-107/169, 173, 177, 93/138/178, 180
- CRUSADE-001..009: crusade.md:212, 216, 219, 223, 224, 226, 229, 234, 237
- ARENA-001..009: arena.md:119, 121, 124, 126, 128, 130, 133, 135, 137
- QUEST-001..008: quest.md:131, 134, 138, 141, 143, 146, 174, 159
- COMMANDS-001..018: systems/commands/overloaded.md:75-94, 103, 33; chat-commands.md:7, 37, 40, 50

## Kept as documentation (design or neutral)
item 9.7 (DupItemID refund); crafting 9.7 (failure destroys item above +0); warehouse 9.2, 9.4, 9.5; party 9.1, 9.7; guild 9.7; crusade 9.7, 9.10; quest 9.7; overloaded envelopes, straddlers, collisions, build-switch tables in overloaded.md.

## Possible duplicates for reconciliation
ITEM-002 (map tile addressing); WAREHOUSE-001 / TRADE-008 (no distance check); TRADE-007 / GUILD-007; SHOP-010 (crusade winner field, crusade home); COMMANDS-002 / PARTY-005; COMMANDS-003, -013 / CRUSADE-001; COMMANDS-004 / CRUSADE-004; COMMANDS-005, -009, -014 / GUILD-007, GUILD-008; crusade.md section 8 points to a combat issue (the /4 crusade exp cut) with no slug.

## Setup blockers
None certain. "unknown": CRUSADE-002, -006, -007; QUEST-007, -008; COMMANDS-015; ITEM-002.

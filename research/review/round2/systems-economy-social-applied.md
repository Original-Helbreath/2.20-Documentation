# Round-2 systems economy-social fixes: applied log

Format: id | applied/rejected/handoff | doc file | note. Framing per coordinator: intent vs current behaviour, neutral wording.

IT-1 | applied | item.md header | three switches + DEF_NPCITEM link, verified GlobalDef.h:65-75
IT-2 | applied | item.md 9.1 | 12866 -> 12864
IT-3 | applied | item.md section 5 | types 2, 4, 10 added; UseItemHandler passes 3,6,7,8,9,11 (verified 30645-30652)
IT-4 | applied | item.md 9.5 | 12 per tile (Tile.h:15), 13th drop deletes oldest, Map.cpp:324-326
CR-1 | applied | crafting.md 7 | cap no-op keeps stone, verified 52062-52090
CR-2 | handoff | game-server/mechanics.md section 8 upgrade line | not my doc; point it at crafting.md
CR-3 | applied | crafting.md 7 | DEF_TESTSERVER footnote
SH-1 | applied | shop.md 9.10, 7 | winner 0 matches neutrals; verified S 16903, 47918
SH-2 | applied | shop.md 7 | quarter of list, comment "half of half"
SH-3 | applied | shop.md 1 | tagged [I]
SH-4 | applied | shop.md 5 | funds[0] not zeroed at boot
TR-1 | applied | trade.md 9.2 | unsigned compare, 0 passes
TR-2 | applied | trade.md 6 | mismatch applies to live slots only, verified 17145-17161
WH-1 | no action | warehouse.md | reviewer: none; doc unchanged
CH-1 | applied | chat.md F2, 7, 9.4 | gate failure -> mode 0, verified 10593-10606, 10701-10717, 11090-11100
CH-2 | applied | chat.md 5, 10 | mute persisted (5866, 6731)
CH-3 | applied | chat.md 7, 9.9 | 600 ticks = 30 min vs 10 min comment
CH-4 | applied | chat.md 1, 9.1 | intent clauses tagged [I]
CH-5 | applied | chat.md 3 | single-notice guard 36329-36330
PA-1 | applied | party.md F5 | small-gain / single-member / zero-living branches
PA-2 | applied | party.md 3 | gate line numbers (PartyOperation 1110; PartyManager 35, 250, 268)
PA-3 | applied | party.md 5 | flat 5000 name array
PA-4 | applied | party.md 9.3 | CONFIRM-target requirement, reply 4 to requester only
GU-1 | applied | guild.md 9.5 | reject resets to NONE, verified 16593-16601
GU-2 | applied | guild.md F1 | silent gate, gold subtraction without re-check
GU-3 | applied | guild.md 10 | extra GUILDWARMODE sites
GU-4 | applied | guild.md 7 | retagged: game server tests cGuildStatus only
CU-1 | applied | crusade.md F1, 8 | winner field zeroed at start; shop coupling
CU-2 | applied | crusade.md F4 | cNum forced to 1, verified 48717, 48727
CU-3 | applied | crusade.md F6 | clamp-before-bonus order
CU-4 | applied | crusade.md 8 | link to game-server/mechanics.md 13.1 and :53
AR-1 | applied | arena.md 1 | softer-death clause demoted to [U]
AR-2 | applied | arena.md F3 | consume only when teleporting
AR-3 | applied | arena.md 9.6 | -4 refusal while number non-zero
QU-1 | applied | quest.md F4 | no contribution on cannot-carry
QU-2 | applied | quest.md 1 | intent tagged [I]

Extra task (config evidence, measured by script with cp949 decoding):
- quest.md section 10: live Quest.cfg (2.03 copy): 40 rows, types 1 x28 and 7 x12 only; all assign type -1; rows 35-40 are the
  indexes the code aborts (S 42052-42058); 3.51 has 120 rows (112/6/2 of types 1/7/10), 3.82 84 rows all type 1.
- crusade.md section 10, 9.8: 2.03 Crusade.cfg 16 rows all type 40 (8 aresden, 8 elvine); 3.51 has 21 (+41 per town, 3 x 42 on middleland);
  WarSchedule only in 3.51 (middleland, schedule 6 22 30 = Saturday 22:30, Sunday = 0); CGT-Aresen: no Npc.cfg copy has it
  (2.03 has no structure NPCs, 3.51 CGT-Aresde, 3.82 CGT-Aresden); code uses CGT-Aresden at 48091 and 48763.
- crafting.md section 10: Potion / BuildItem chosen copies and counts from config-files.md (balance not analysed).
- guild.md section 10: tickets 88/89 names from 2.03/3.51 Item.cfg; price column not mapped, left [U].
- Not done: unverified whether crusade.md 9.2 "defect" wording needs neutralising beyond accuracy; left as is.

Counts: applied 32, rejected 0, handoff 1 (CR-2 -> game-server/mechanics.md), no action 1 (WH-1).

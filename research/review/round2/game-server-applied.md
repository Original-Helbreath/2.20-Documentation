# Round-2 game-server fixes: applied log

Format: id | applied/rejected | doc file:line | note

R2-01 | applied | game-server/functions/items.md:287 | repair quote now [V] with categories/repairer types
R2-02 | applied | game-server/functions/items.md:297 (mechanics.md:189 pending, see R2-02b) | confirm handler facts verified 34524-34596
R2-09 | applied | game-server/functions/items.md:113 | first stack only, verified 19178-19195
R2-10 | applied | game-server/functions/items.md:194 | verified 30519-30528
R2-11 | applied | game-server/functions/items.md:350 | verified 37201-37252
R2-12 | applied | game-server/functions/items.md:453 | verified 40655-40686
R2-02b | applied | game-server/mechanics.md:189 | repairer check scoped to quote; verified 34447,34476,34524,34596
R2-06 | applied | game-server/mechanics.md:225 | verified Game.cpp:52793,27235,2085,4917 (character.md:431 already correct)
R2-15 | applied | game-server/mechanics.md:41 | verified 3944-3956
R2-16 | applied | game-server/mechanics.md:48 | verified 4027-4057
R2-17 | applied | game-server/mechanics.md:188 | verified 16820 is function start
R2-18 | applied | game-server/mechanics.md:195 | verified 38827-38917
R2-19 | applied | game-server/mechanics.md:27,31 | verified 42328-42338, 20318, 3883
R2-20 | applied | game-server/mechanics.md:142 | verified 20911-20927
R2-04 | applied | game-server/data-structures.md:116 | verified XSocket.cpp:39,367,392; Client.h:22; Game.h:51
R2-05 | applied | game-server/data-structures.md:113-114 | verified DelayEvent.h, Quest.h
R2-08 | applied | game-server/build-config.md:7-8 | CMakeLists globs src/*.cpp (verified); dsp lists no .cpp for Structure/Crusade/rusade; claim that only 2 casts differ taken from review, not re-diffed (marked U)
R2-22 | applied | game-server.md:2 and 15 function-doc headers :3, mechanics.md:3 | tagged [U: no VCS metadata]
R2-07 | applied | game-server/functions/ui-logging.md:68-72 and 80-84 | reordered both _bItemLog entries (sig, handle/name callers, merged list on one line, callees)
R2-03 | applied | game-server/functions/character.md:6 and combat-magic.md:6 | cfg: note inserted; confirmed cfg lines at character.md:116-162, combat-magic.md:46-129
R2-13 | applied | game-server/functions/crusade-quests.md:361 and :370 | verified 48137-48195 (also SendCollectedMana early return/zeroing)
R2-14 | applied | game-server/functions/crusade-quests.md:146,156,177,188 | verified 41913-42126; note the crusade rule is: during crusade only assign-type 1, outside it never type 1 (review wording was ambiguous)
R2-21 | applied | game-server/functions/network.md:287, npc.md:177, character.md:306 | verified 43123-43154, 29982-29992, 36241; 3 s tick of the 3876 loop inferred from the m_dwGameTime3 > 3000 block at 3743 (not traced to its closing brace)

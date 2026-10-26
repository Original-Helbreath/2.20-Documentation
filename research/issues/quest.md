# Issues: quest

Scope: defects and divergences in the quest system (City Hall offer, accept, tracking, reward, Quest.cfg data) in the 2.20 game server. Split out of `research/systems/quest.md`.

### QUEST-001 Most quest types are unimplemented; the time limit is never read
- **Program**: game server
- **System**: quest (tracking, completion)
- **Kind**: incomplete
- **Intent**: `Quest.h` lists 12 types (assassination, delivery, escort, guard, build, supply, strike, flag, time-limited hunt) as a larger plan.
- **Now**: only MONSTERHUNT and GOPLACE can complete; types 2-6 and 8-12 have no completion logic (S 42099-42122); the time-limit field is never read: the parser writes `m_iTimeLimit` and nothing else touches it (S 41696). [V]
- **Where**: S Game.cpp:42099-42122, 41696; S Quest.h:19-30
- **Evidence**: read in source; grep.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/quest.md:131

### QUEST-002 War teleport uses quest type 10 instead of 11
- **Program**: game server
- **System**: quest (accept)
- **Kind**: data
- **Intent**: the type list names 11 SENDTOBATTLE for "go straight to the battle".
- **Now**: the war teleport branch tests type 10 (STRATEGICSTRIKE) (S 41987; Quest.h:28-29) - either the type numbers shifted or the config uses 10 for the teleport [I]. The 2.03 Quest.cfg has no type-10 row (and no assign-type-1 row), so this path is unused with that file; the 3.51 Quest.cfg has two type-10 rows. [V]
- **Where**: S Game.cpp:41984-41991; S Quest.h:28-29
- **Evidence**: read in source and configs.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/quest.md:134

### QUEST-003 Accept trusts the client
- **Program**: game server
- **System**: quest (accept)
- **Kind**: client-only check / security
- **Intent**: accept only a pending offer.
- **Now**: `QuestAcceptedHandler` indexes `m_pQuestConfigList[m_iAskedQuest]` without a NULL or pending-offer check and never clears `m_iAskedQuest` (S 41984-41998); a `QUESTACCEPTED` with no offer dereferences slot 0 (NULL) [I crash risk]; a running quest can be replaced by re-sending accept [I]. [V]
- **Where**: S Game.cpp:41984-41998
- **Evidence**: read in source; effects inferred.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/quest.md:138

### QUEST-004 Reward can be re-rolled by re-asking
- **Program**: game server
- **System**: quest (offer)
- **Kind**: bug
- **Intent**: a random reward slot chosen when the quest is offered.
- **Now**: the reward slot (1d3) is chosen at offer time and overwritten on every talk before accepting (S 41958, 39205); a player can talk until the preferred reward appears [I]. [V]
- **Where**: S Game.cpp:41958, 39205
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/quest.md:141

### QUEST-005 Reward label mismatch for experience rewards
- **Program**: game server
- **System**: quest (offer, reward)
- **Kind**: bug
- **Intent**: name the experience reward in the offer dialog.
- **Now**: the offer path names the reward "exp" only for type -10 (S 39199), but rewards use -1 and -2 for exp (S 41410, 41423), so exp offers show an empty reward name [I display]. The reward text literal is Korean (`"경험치"`) in an English build (S 41417). [V]
- **Where**: S Game.cpp:39199, 41410, 41417, 41423
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/quest.md:143

### QUEST-006 Stub giver functions are never called
- **Program**: game server
- **System**: quest (offer)
- **Kind**: dead code
- **Intent**: quests from guild hall, shops, blacksmith, warehouse and wizard tower NPCs.
- **Now**: five `_iTalkToNpcResult_*` functions exist with dialogue codes 111-136 reserved, but are never called. [V doc] `NpcTalkHandler` calls quest logic only for iWho 4 (city hall) and 21 (guard); the five stubs return -4 (merged from GSO-022). [V]
- **Where**: S Game.cpp:42128-42151, 39220-39228; S Game.cpp:39153-39239
- **Evidence**: doc crusade-quests.md.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/quest.md:146; research/game-server/functions/crusade-quests.md:45

### QUEST-007 Quest.cfg rows 35-40 are listed but aborted on environment check
- **Program**: game server
- **System**: quest (offer, tracking), data
- **Kind**: data
- **Intent**: rows deleted in v2.181 should not be offered.
- **Now**: rows 35-40 are exactly the indexes `_CheckQuestEnvironment` aborts (S 42052-42058). The offer filter has no such exclusion, so the catalogue still lists them; a player who accepts one has it cleared and receives `QUESTABORTED` on the environment check [I effect]. Confirmed for the 2.03 copy. [V]
- **Where**: S Game.cpp:41978-42003, 42052-42058
- **Evidence**: read in source and the 2.03 Quest.cfg.
- **Blocks 2.20 setup**: unknown (depends on the Quest.cfg the operator pushes)
- **Source doc**: research/systems/quest.md:174

### QUEST-008 The 2.03 Quest.cfg has no crusade-only quests
- **Program**: game server
- **System**: quest (offer), data
- **Kind**: data
- **Intent**: during a crusade the board switches to war tasks only (assign type 1).
- **Now**: all 2.03 rows have assign type -1 (none is crusade-only), so while `m_bIsCrusadeMode` is set `__iSearchForQuest` finds no quest (S 41940). [V]
- **Where**: S Game.cpp:41940
- **Evidence**: read in source; measured over the 2.03 Quest.cfg.
- **Blocks 2.20 setup**: unknown (depends on the Quest.cfg the operator pushes)
- **Source doc**: research/systems/quest.md:159

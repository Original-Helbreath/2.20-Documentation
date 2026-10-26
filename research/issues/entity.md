# Issues: entity

Scope: defects and divergences in players and NPCs as world objects: object ids, view updates, spawning, AI, summons, taming (documented in `research/systems/entity.md`). Paths and tags as in that doc: `SRV` = `sources/hgserver-librehelbreath/src/`, `CLI` = `sources/client-2.20/`; "npc" = `research/game-server/functions/npc.md`; [V] verified, [I] inferred, [U] unverified.

### ENTITY-001 Taming is a stub and unreachable on land
- **Program**: game server
- **System**: entity
- **Kind**: incomplete
- **Intent**: skill-driven conversion of a monster into a pet.
- **Now**: both branches of the level test in `_TamingHandler` end in `break`; nothing changes. It is also unreachable on land: `iCalculateUseSkillItemEffect` returns before the skill switch unless the target tile is water and a 1d105 roll beat the skill.
- **Where**: SRV Game.cpp:46136-46138, 33726-33730
- **Evidence**: [V npc], [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/entity.md:155

### ENTITY-002 RemoveFromTarget turns every guild war unit into a 5-minute summon
- **Program**: game server
- **System**: entity
- **Kind**: bug
- **Intent**: per the comment, when the commander dies.
- **Now**: whenever any player of that guild GUID is removed as a target (teleport, disconnect, invisibility, resurrection), every guild war unit becomes a 5-minute summon.
- **Where**: SRV Game.cpp:12270-12277
- **Evidence**: [V npc]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/entity.md:158

### ENTITY-003 bSetNpcFollowMode and bSetNpcAttackMode use an uninitialised index
- **Program**: game server
- **System**: entity
- **Kind**: bug
- **Intent**: find the NPC by name and set follow/attack mode.
- **Now**: both find the NPC by the first 5 bytes of the name with an uninitialised index.
- **Where**: SRV Game.cpp:9985 (`bSetNpcFollowMode`), 35934 (`bSetNpcAttackMode`)
- **Evidence**: [V npc]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/entity.md:161

### ENTITY-004 SetSummonMobAction target mode: unbounded copy and unzeroed name buffer
- **Program**: game server
- **System**: entity
- **Kind**: security
- **Intent**: `/tgt <name>` sets a summon's attack target.
- **Now**: target mode copies `dwMsgSize` bytes into `cBuff[256]` without a bound, and `cTargetName[11]` is never zeroed before a short `memcpy`, so the 10-byte `memcmp` against a padded name depends on stack garbage; `/tgt` may fail for short names.
- **Where**: SRV Game.cpp:39926, 39936-39944
- **Evidence**: [V code; I effect]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/entity.md:163

### ENTITY-005 bCreateNewNpc declared BOOL but returns the slot index
- **Program**: game server
- **System**: entity
- **Kind**: bug
- **Intent**: create an NPC and report success.
- **Now**: declared `BOOL`, returns the slot index; `__bSetAgricultureItem` relies on that (gathering.md). Slot 0 would read as failure.
- **Where**: SRV Game.cpp:10043-10365
- **Evidence**: [V npc]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/entity.md:166

### ENTITY-006 MobGenerator user-count scaling inactive; crusade break stops generation on later maps
- **Program**: game server
- **System**: entity
- **Kind**: bug
- **Intent**: scale spawns by user count (`DEF_CHECKUSERNPCNUM`); skip middleland during crusade.
- **Now**: the user-count scaling is inactive and overwritten anyway; the middleland-during-crusade `break` stops generation on every later map instead of skipping one.
- **Where**: SRV Game.cpp:28988 (`MobGenerator`)
- **Evidence**: [V npc]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/entity.md:168

### ENTITY-007 CalcNextWayPointDestination divides by zero
- **Program**: game server
- **System**: entity
- **Kind**: bug
- **Intent**: pick the next waypoint or random-area destination.
- **Now**: divides by zero with one waypoint or a zero-width area.
- **Where**: SRV Game.cpp:29960
- **Evidence**: [V npc]
- **Blocks 2.20 setup**: unknown (depends on map-info waypoint data)
- **Source doc**: research/systems/entity.md:170

### ENTITY-008 bSerchMaster (master/follower packs) is dead
- **Program**: game server
- **System**: entity
- **Kind**: dead code
- **Intent**: monsters following a master in packs.
- **Now**: its only call is commented out.
- **Where**: SRV Game.cpp:11606
- **Evidence**: [V npc]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/entity.md:171

### ENTITY-009 Stale tile owner handles repaired lazily at read time
- **Program**: game server
- **System**: entity
- **Kind**: bug
- **Intent**: tile owner handles are always valid.
- **Now**: the compose functions repair stale handles ("sometimes NULL despite a handle"); evidence that the owner-clear paths leak, patched at read time.
- **Where**: SRV Game.cpp:3401-3418
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/entity.md:172

### ENTITY-010 TargetSearch self-skip break abandons the rest of the column
- **Program**: game server
- **System**: entity
- **Kind**: bug
- **Intent**: skip the searching NPC's own tile and continue scanning.
- **Now**: the self-skip `break` abandons the rest of that column of the search square.
- **Where**: S Game.cpp:11695
- **Evidence**: [V npc]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/npc.md:70

### ENTITY-011 _cGetSpecialAbility leaves the result uninitialised for some kinds
- **Program**: game server
- **System**: entity
- **Kind**: bug
- **Intent**: pick a random special ability for a mob.
- **Now**: `cSA` is left uninitialised for kinds outside 1-9.
- **Where**: S Game.cpp:44129-44207
- **Evidence**: [V npc]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/npc.md:241

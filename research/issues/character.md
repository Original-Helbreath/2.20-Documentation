# Issues: character

Scope: defects and divergences in stats, levels, exp, level-up, stat points, hunger, regeneration and majestic points (documented in `research/systems/character.md`). Paths and tags as in that doc: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`; [V] verified, [I] inferred, [U] unverified. The `break` instead of `continue` in the per-client upkeep loop is registered once, as COMBAT-004 in `combat.md`.

### CHARACTER-001 No max-level stop inside the level-up loop
- **Program**: game server
- **System**: character
- **Kind**: bug
- **Intent**: level capped at `m_iPlayerMaxLevel` (180).
- **Now**: `bCheckLevelUp` checks max level only on entry (S :27218); a single large exp settlement can raise the level past `m_iPlayerMaxLevel` inside the while loop (S :27245-27298). The exp table extends to 299 so it does not crash (mechanics.md section 2).
- **Where**: S Game.cpp:27218, 27245-27298
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/character.md:169

### CHARACTER-002 DEF_LEVELLIMIT is documented as a trial limit but used as the hunger threshold
- **Program**: game server
- **System**: character
- **Kind**: dead code
- **Intent**: the trial-version level limit (comment at S Game.h:173-174).
- **Now**: used only as the hunger threshold (S :3883); the trial-account system (comment "Account Status 1 trial", S :27243) is gone.
- **Where**: S Game.h:173-174; S Game.cpp:3883, 27243
- **Evidence**: [V]; [I] that the trial system was removed
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/character.md:172

### CHARACTER-003 Unused per-stat flags in bCheckLevelUp
- **Program**: game server
- **System**: character
- **Kind**: dead code
- **Intent**: per-stat notify of which stats rose.
- **Now**: the flags bStr..bChr are set and never read.
- **Where**: S Game.cpp:27242-27276
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/character.md:175

### CHARACTER-004 LIMITEDLEVEL and CHARISMA notifies have no server sender
- **Program**: game server
- **System**: character
- **Kind**: dead code
- **Intent**: tell the client about a level limit and a charisma change.
- **Now**: both have packing and client cases but no server caller (overloaded.md). Packing cases are at S Game.cpp:18838 (LIMITEDLEVEL) and 18906 (CHARISMA) (merged from COMMANDS-009). [V]
- **Where**: client notify handlers; no server caller; S Game.cpp:18838, 18906
- **Evidence**: [V per overloaded.md]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/character.md:177; research/systems/commands/overloaded.md:85

### CHARACTER-005 Restart HP ignores the MaxHPdown divisor
- **Program**: game server
- **System**: character
- **Kind**: bug
- **Intent**: respawn HP equals the character's maximum HP.
- **Now**: `RequestRestartHandler` sets HP = 3*Vit + 2*Level + Str/2 and ignores the MaxHPdown divisor that `iGetMaxHP` applies; MP and SP are not restored.
- **Where**: S Game.cpp:46246-46321
- **Evidence**: [V]; [I] that the omission is unintended
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/character.md:396

### CHARACTER-006 m_iLimitedUserExp is unused
- **Program**: game server
- **System**: character
- **Kind**: dead code
- **Intent**: limit exp of trial users.
- **Now**: the field exists and is not used; the traveller cap uses `m_iLevelExp20`.
- **Where**: S Game.cpp:27531-27545 (`bCheckLimitedUser`)
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/character.md:176

### CHARACTER-007 ResurrectPlayer has no live caller
- **Program**: game server
- **System**: character
- **Kind**: dead code
- **Intent**: resurrect a player after 30 s in a fightzone.
- **Now**: its only caller (Game.cpp:4087) is inside a `/* */` block; the function moves the player to the map initial point, restores HP/MP/SP and re-runs `RequestInitDataHandler`.
- **Where**: S Game.cpp:50853-50876, 4087
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/character.md:412

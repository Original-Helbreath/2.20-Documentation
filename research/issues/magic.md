# Issues: magic

Scope: defects and divergences in spells, mana, casting and effects (documented in `research/systems/magic.md`). Paths and tags as in that doc: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`, `CFG` = Magic.cfg; [V] verified, [I] inferred, [U] unverified. The constant-magery cast pose is registered once, as COMBAT-002 in `combat.md`.

### MAGIC-001 bCheckClientMagicFrequency is defined and never called
- **Program**: game server
- **System**: magic
- **Kind**: dead code
- **Intent**: a 1500 ms throttle between casts.
- **Now**: never called, so the 100 ms `m_dwRecentAttackTime` gate is the only cast throttle.
- **Where**: S Game.cpp:52645
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/magic.md:25

### MAGIC-002 Weather bonus table is incomplete
- **Program**: game server
- **System**: magic
- **Kind**: data
- **Intent**: electric spells gain and fire spells lose from weather across all circles.
- **Now**: Mass-Lightning-Arrow (56), Lightning-Strike (74) and Mass-Fire-Strike (61) are not in `iGetWhetherMagicBonusEffect`, so later electric/fire spells ignore weather; likely the table was not updated as circles 6-8 were added.
- **Where**: S Game.cpp:37465
- **Evidence**: [V] table contents and CFG names; [I] on the cause
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/magic.md:163

### MAGIC-003 Guild-war spell bans are compiled out
- **Program**: game server
- **System**: magic
- **Kind**: build
- **Intent**: DEF_GUILDWARMODE spell bans and a special-ability ban.
- **Now**: both are compiled out (GlobalDef.h:8); a guild-war ruleset that was built and shelved.
- **Where**: S Game.cpp:20853; S GlobalDef.h:8
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/magic.md:166

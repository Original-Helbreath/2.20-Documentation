# Issues: combat

Scope: defects and divergences in melee and ranged attacks, hit and damage, criticals, death and corpse handling (documented in `research/systems/combat.md`). Paths and tags as in that doc: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`; [V] verified, [I] inferred, [U] unverified.

### COMBAT-001 Same-side town protection is compiled out in the selected build
- **Program**: game server
- **System**: combat
- **Kind**: build
- **Intent**: citizens of one side cannot damage each other in their own town (`DEF_SAMESIDETOWNPROTECT`); `DEF_ALLPLAYERPROTECT` protects every player.
- **Now**: only the beginner (neutral) protection is compiled. `DEF_SAMESIDETOWNPROTECT` sits under DEF_USA / DEF_CONSULTATION (S GlobalDef.h:48, :85), so same-side citizens in their own town can damage each other in the DEF_CHINESE build selected in GlobalDef.h:16 (that this is the shipped build is [I]); `DEF_ALLPLAYERPROTECT` is commented out everywhere. The town guard (`bAnalyzeCriminalAction`) is the only in-town deterrent and melee calls it only on a killing blow (S :13745), so non-lethal griefing in town spawns no guard.
- **Where**: S Game.cpp:12570, 12588, 12592, 13745; S GlobalDef.h:16, 48, 83, 85
- **Evidence**: [V]; [I] that DEF_CHINESE is the shipped build
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/combat.md:193; also research/systems/pk-law.md:195 (same-side town protection off in the export)

### COMBAT-002 Cast motion carries a hard-coded magery value
- **Program**: game server
- **System**: combat
- **Kind**: bug
- **Intent**: the broadcast sends the caster's magery (the source comment names the magic ability level).
- **Now**: `iTemp = 10` is hard-coded in the cast branch of the combat dispatcher. Belongs to magic but sits in `ClientMotionHandler`.
- **Where**: S Game.cpp:1112-1118
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/combat.md:198; also research/systems/magic.md:161

### COMBAT-003 Duplicated victim branches in ClientKilledHandler
- **Program**: game server
- **System**: combat
- **Kind**: incomplete
- **Intent**: a guild-specific rule for guildsman victims versus citizens.
- **Now**: the `m_iGuildRank == -1` (citizen) and the guildsman branches are byte-for-byte the same decisions; the planned rule was never written.
- **Where**: S Game.cpp:19350-19414
- **Evidence**: [V]; [I] on the intent
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/combat.md:201; also research/systems/pk-law.md:202

### COMBAT-004 CheckClientResponseTime breaks out of the client loop after a deletion
- **Program**: game server
- **System**: combat
- **Kind**: bug
- **Intent**: skip only the deleted client slot and keep processing the other clients that tick.
- **Now**: `break` (not `continue`) follows any per-client action that can delete the client (S :4015, :4028, :4058, :4067, :4091, :4113, :4132; all in one `for` loop with no inner loop or switch, S :3853). The deleters are the skill-message flood kick (`DeleteClient`, S :4020) and the speed-hack block (S :4050); the next `break` (:4028, :4058) then leaves the whole loop, so later clients are not processed in that tick. The idle-timeout delete (S :3860-3867) is an if / else-if branch and does not leave the loop; force recall does not delete the client, and the `break` at :4082 is inside a comment.
- **Where**: S Game.cpp:3853-4132
- **Evidence**: [V]; [I] on the intent
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/combat.md:206; also research/systems/character.md:162

### COMBAT-005 Fightzone resurrection after 30 s is commented out

Merged into [ARENA-008](arena.md#arena-008-fight-zone-auto-resurrect-removed-but-dead-time-still-recorded).

### COMBAT-006 Crusade kill-exp cut applies on every map
- **Program**: game server
- **System**: combat
- **Kind**: bug
- **Intent**: the divide-by-4 on kill exp during a crusade was meant to exempt `default`, `arefarm` and `elvfarm`.
- **Now**: the divide by 4 is inside `if (m_bIsCrusadeMode == TRUE)`; the map test inside uses `||` and is always true, so during a crusade the cut applies on every map. Only kills worth more than 10 exp are cut, and only the killer's final share (exp/3 plus the remainder; damage-time exp is not divided).
- **Where**: S Game.cpp:14523-14527
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/mechanics.md:230

### COMBAT-007 Hammer and staff combo bonuses borrow other weapons' tables
- **Program**: game server
- **System**: combat
- **Kind**: data
- **Intent**: [I] each weapon skill has its own combo table.
- **Now**: `iGetComboAttackBonus` gives skill 14 (hammer) the `___iCAB6` table (the bow table, all zeros) so hammers never gain a combo bonus, and skill 21 (staff) the `___iCAB10` table (the axe table).
- **Where**: S Game.cpp:39362-39394
- **Evidence**: [V]; [I] on the intent
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/mechanics.md:118; research/game-server/functions/combat-magic.md:386

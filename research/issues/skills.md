# Issues: skills

Scope: defects and divergences in the skill list, skill gain and mastery (documented in `research/systems/skills.md`). Paths and tags as in that doc: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`, `SCFG` = Skill.cfg; [V] verified, [I] inferred, [U] unverified.

### SKILLS-001 Skill 14 is hammer in code but cooking in Skill.cfg
- **Program**: game server
- **System**: skills
- **Kind**: data
- **Intent**: skill 14 is the hammer weapon skill.
- **Now**: code treats 14 as hammer (`_iGetWeaponSkillType` S :39309, combat list S :28859, Dex cap S :28921 "v2.16 fix") but SCFG names it cooking. Hammer weapons were bolted onto an unused cooking slot [I]; the client name depends on its own SKILLCFG.TXT [U].
- **Where**: S Game.cpp:39309, 28859, 28921; SCFG
- **Evidence**: [V] code and SCFG; [I]/[U] as noted
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/skills.md:133

### SKILLS-002 Skills 16, 17, 18, 20 and 22 are defined but unimplemented
- **Program**: game server
- **System**: skills
- **Kind**: incomplete
- **Intent**: UO-inspired skills (stealing, musicianship, item identification, taming, armour manufacturing).
- **Now**: they exist in SCFG but have no stat-cap case in `CalculateSSN_SkillIndex` (default branch, S :28951) and no gameplay caller was found. Taming has a dispatch (the DEF_SKILLEFFECTTYPE_TAMING case in `iCalculateUseSkillItemEffect` calls `_TamingHandler`, S :33737) whose body is a stub (see ENTITY-001).
- **Where**: S Game.cpp:28951, 33737
- **Evidence**: [V]; [I] on the intent
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/skills.md:136

### SKILLS-003 REQ_TRAINSKILL and SKILLTRAINFAIL are dead
- **Program**: game server
- **System**: skills
- **Kind**: dead code
- **Intent**: teaching a skill by NPC.
- **Now**: the `REQ_TRAINSKILL` handler is commented out and the client never sends it; the fail notify exists only inside a `/* */` block. Teaching was replaced by manuals [I]. The `SKILLTRAINFAIL` send is a hand-built send inside the commented block (S 25498) and the client has a case for it (merged from COMMANDS-001 and COMMANDS-010). [V]
- **Where**: S Game.cpp:15541-15543, 25443-25525; S Game.cpp:25498
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/skills.md:139; research/systems/commands/overloaded.md:75, 86

### SKILLS-004 The 700-point cap is not enforced until a down-skill has been chosen
- **Program**: game server
- **System**: skills
- **Kind**: bug
- **Intent**: keep the total of all skills at 700 or below.
- **Now**: with `m_iDownSkillIndex` unset, `bCheckTotalSkillMasteryPoints` finds nothing to lower and returns, so the cap only bites once the player has picked a down-skill.
- **Where**: S Game.cpp:32531
- **Evidence**: [V ch]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/skills.md:141

### SKILLS-005 The v2.19 SSN doubling applies only on the skill-index path
- **Program**: game server
- **System**: skills
- **Kind**: bug
- **Intent**: faster combat progression since v2.19: SSN doubled for skills 4-10, 14 and 21.
- **Now**: the doubling is only inside `CalculateSSN_SkillIndex` (S :28852-28863); the weapon-hit path `CalculateSSN_ItemIndex` does not double, so magic casts (skill 4, S :22806) and bare-hand hits (skill 5, S :14260) are doubled but melee weapon hits are not.
- **Where**: S Game.cpp:28700, 28852-28863
- **Evidence**: [V]; [I] that the asymmetry is unintended
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/skills.md:21

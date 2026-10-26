# Issues: pk-law

Scope: defects and divergences in PK and criminal status, town alignment, penalties, rewards, jails, force recall and hunter mode (documented in `research/systems/pk-law.md`). Paths and tags as in that doc: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`; [V] verified, [I] inferred, [U] unverified. The off same-side town protection is registered as COMBAT-001 and the duplicate victim branches as COMBAT-003, both in `combat.md`.

### PK-LAW-001 Traveller flag is never cleared after citizenship
- **Program**: game server
- **System**: pk-law
- **Kind**: bug
- **Intent**: a fresh citizen loses traveller protections.
- **Now**: `iSetSide` sets `m_bIsNeutral = TRUE` for travellers but never resets it (S :52894; the only other assignment is the constructor, S Client.cpp:214), and `RequestCivilRightHandler` changes `m_cSide` without touching it. Until the next login a fresh citizen still (a) cannot damage innocent players (S :12603, :31216), (b) loses only 1/3 exp and drops no items on death (S :28296, :28307), and (c) keeps beginner-town immunity (S :12593): a free protection window.
- **Where**: S Game.cpp:52894, 27547-27615, 12603, 31216, 28296, 28307, 12593; S Client.cpp:214
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/pk-law.md:189

### PK-LAW-002 Jail routing tests location name, respawn routing tests map name
- **Program**: game server
- **System**: pk-law
- **Kind**: bug
- **Intent**: a killer or enemy-town death goes to the enemy jail consistently.
- **Now**: `ApplyPKpenalty` jails by map location name (S :28030, :28045) while respawn routing tests the map name "elvine"/"aresden" (S :2673, :2695, :46278, :46300); a death on a sub-map whose location is the enemy town (e.g. a building) does not route to the enemy jail.
- **Where**: S Game.cpp:28030, 28045, 2673, 2695, 46278, 46300
- **Evidence**: [V]; [I] on which maps are affected
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/pk-law.md:197

### PK-LAW-003 Force-recall residual-time branch is dead code
- **Program**: game server
- **System**: pk-law
- **Kind**: dead code
- **Intent**: reset a longer remaining time to a short value when force recall is re-armed.
- **Now**: `SetForceRecallTime` has a "residual time" branch (S :52754-52776) that would reset a longer remaining time to 1 tick (about 3 s; it sets 1, not a 1-minute cap, S :52773-52774), but both callers zero `m_iTimeLeft_ForceRecall` immediately before calling it (S :2090-2113, :23695-23719), so it never runs. The v2.19 2002-11-28 change (3 min Mon-Thu, S :52737-52740; Korea keeps 1 min under DEF_CONSULTATION) is the only live value.
- **Where**: S Game.cpp:52754-52776, 2090-2113, 23695-23719
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/pk-law.md:200

### PK-LAW-004 Traveller-on-innocent PK branch is mostly unreachable
- **Program**: game server
- **System**: pk-law
- **Kind**: dead code
- **Intent**: penalise a traveller who kills an innocent.
- **Now**: a neutral attacker cannot damage an innocent (S :12603, :31216), yet `ClientKilledHandler` still applies `ApplyPKpenalty` for that case (S :19341, :19357, :19390); only indirect damage paths (fields; PLAYER_INDIRECT is excluded) could reach it.
- **Where**: S Game.cpp:12603, 31216, 19341, 19357, 19390
- **Evidence**: [V]; [I] on reachability
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/pk-law.md:204

### PK-LAW-005 Citizenship can be granted on any map
- **Program**: game server
- **System**: pk-law
- **Kind**: client-only check
- **Intent**: citizenship is chosen at a city hall.
- **Now**: the handler uses the current map's location name with no city-hall check on the server; a location other than aresden/elvine leaves side 0 but replies success.
- **Where**: S Game.cpp:27562-27567
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/pk-law.md:207

### PK-LAW-006 Bounty collection has no city-hall check on the server
- **Program**: game server
- **System**: pk-law
- **Kind**: client-only check
- **Intent**: bounty gold is redeemed at the city hall.
- **Now**: `GetRewardMoneyHandler` has no location check; the city-hall requirement is a client-side menu condition only.
- **Where**: S Game.cpp:28438
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/pk-law.md:172

### PK-LAW-007 Hunter-mode level thresholds disagree
- **Program**: game server
- **System**: pk-law
- **Kind**: bug
- **Intent**: civilian (hunter) mode for citizens below level 100.
- **Now**: `RequestHuntmode` rejects only `level > 100 && !hunter`, so a level-100 player can enable it; `bCheckLevelUp` forces it off at level >= 100 (S :27235) and the login check forces it off above 100 (S :2085).
- **Where**: S Game.cpp:52793, 27235, 2085
- **Evidence**: [V]; [I] on the effect
- **Blocks 2.20 setup**: no
- **Source doc**: research/game-server/functions/character.md:432; research/systems/pk-law.md:150

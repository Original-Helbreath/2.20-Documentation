# Issues: crusade

Scope: defects and divergences in the crusade war loop (schedule, duties, points, structures, mana and meteor, results), the energy sphere and the retired occupy flags in the 2.20 game server. Split out of `research/systems/crusade.md`.

### CRUSADE-001 Occupy flags are retired but still wired
- **Program**: game server
- **System**: crusade (occupy flags)
- **Kind**: dead code / incomplete
- **Intent**: plant EK-bought flags to claim Middleland tiles.
- **Now**: `GetOccupyFlagHandler` still consumes up to 12 EK for a flag item [V doc S 39988-40023], but `__bSetOccupyFlag` returns FALSE before doing anything [V doc S 39508], save is a no-op called every 3 min [V doc S 47997], load and random-flag removal have no callers [V doc]. Players can still buy a flag that cannot be planted [I]. `SETOCCUPYFLAG` quest type and the world log's occupy-flag save handlers are leftovers. Detail: `__bSetOccupyFlag` returns FALSE unconditionally right after the "no flag in crusade mode" comment (S 39508), so 39510-39890 is unreachable and both callers always get FALSE. `SaveOccupyFlagData` returns at its first statement (47997) yet `OnTimer` calls it every 3 min (3760-3763); `_iComposeFlagStatusContents` is reachable only from it; `_bDecodeOccupyFlagSaveFileContents` and `_DeleteRandomOccupyFlag` have no call site; the world log's save handler is never reached. `REQ_GETOCCUPYFLAG` has a live handler (S 15471) but no client sender, and the sends of `MSGID_REQUEST_SAVEARESDENOCCUPYFLAGDATA` / `...ELVINE...` and `MSGID_COLLECTEDMANA` exist only in comments (S 14865-14866, 48142) (merged from COMMANDS-003, COMMANDS-013, GSO-020, GSO-021, PERSIST-003). [V]
- **Where**: S Game.cpp:39978-40140, 39499-39890, 47991-48016; S Game.cpp:15471, 14865-14866, 48142, 40307-40454, 46637-46725, 3760-3763, 39988, 40013-40023
- **Evidence**: doc crusade-quests.md.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crusade.md:212; research/systems/commands/overloaded.md:77, 90; research/game-server/functions/crusade-quests.md:56, :67, :89, :96, :244, :343; systems/persistence.md section 9

### CRUSADE-002 The once-a-day start guard is broken
- **Program**: game server
- **System**: crusade (start)
- **Kind**: bug
- **Intent**: refuse a second crusade start on the same weekday.
- **Now**: `m_iLatestCrusadeDayOfWeek` is set only while it is -1 (S 50282-50285): after the first crusade every later start on that weekday is refused until restart (GM command included), while starts on other weekdays are never limited [I effect]. [V]
- **Where**: S Game.cpp:50282-50285
- **Evidence**: read in source; effect inferred.
- **Blocks 2.20 setup**: unknown
- **Source doc**: research/systems/crusade.md:216

### CRUSADE-003 Command tools have no server-side role check
- **Program**: game server / client
- **System**: crusade (command tools)
- **Kind**: client-only check / security
- **Intent**: only commanders and engineers (and crusade mode) can set points and summon units.
- **Now**: `RequestSetGuildTeleportLocHandler`, `RequestSetGuildConstructLocHandler`, `RequestGuildTeleportHandler` and `RequestSummonWarUnitHandler` test neither duty, rank nor crusade mode [V: no `m_iCrusadeDuty`/`m_bIsCrusadeMode`/`m_iGuildRank` reference in S 48453-48935]; only the client dialogs restrict them. `SelectCrusadeDutyHandler` also accepts any duty value at any time (S 49042-49063). [V]
- **Where**: S Game.cpp:48453-48935, 49042-49063
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crusade.md:219

### CRUSADE-004 Help call is unreachable from the client
- **Program**: client / game server
- **System**: crusade (command tools)
- **Kind**: dead code
- **Intent**: soldiers call for help; the commander sees their position.
- **Now**: `REQUEST_HELP` is never sent and `NOTIFY_HELP` is ignored. [V doc notify-types.md] The server handler is live (S 15415, 49268-49288); the reply `NOTIFY_HELPFAILED` has no client case either (merged from COMMANDS-004). [V]
- **Where**: S Game.cpp:49268-49287; S Game.cpp:15415
- **Evidence**: doc notify-types.md.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crusade.md:223; research/systems/commands/overloaded.md:78, 83

### CRUSADE-005 Reward arithmetic drift
- **Program**: game server
- **System**: crusade (results)
- **Kind**: bug
- **Intent**: payout by the documented formula.
- **Now**: contribution is capped at 300000 at payout vs 500000 while fighting, the clamp is applied before the level bonus is added so the paid total can exceed it, and the loss log writes /10 vs /5 paid. [V doc] The loss case awards `contribution/5` and the log line writes `/10` (merged from GSO-023). [V]
- **Where**: S Game.cpp:47846, 47893, 47849-47893; S Game.cpp:47862-47893
- **Evidence**: doc crusade-quests.md / source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crusade.md:224; research/game-server/functions/crusade-quests.md:313

### CRUSADE-006 Mana stone selection overflow
- **Program**: game server
- **System**: crusade (structures)
- **Kind**: bug
- **Intent**: pick 3 random type-42 mana stone rows.
- **Now**: with 3 or fewer type-42 rows it marks `iMSIndex[0..2]` unconditionally; with fewer than 3, `iMSIndex[k] = -1` writes `iMSSelected[-1]` [I memory effect]. [V] Merged from GSO-024 (same defect, game-server function view). [V]
- **Where**: S Game.cpp:48040-48045
- **Evidence**: read in source.
- **Blocks 2.20 setup**: unknown (the chosen 2.03 Crusade.cfg has no type-42 rows)
- **Source doc**: research/systems/crusade.md:226; research/game-server/functions/crusade-quests.md:351

### CRUSADE-007 Cannon tower kit uses the literal name CGT-Aresen
- **Program**: game server
- **System**: crusade (structures)
- **Kind**: data
- **Intent**: build Aresden's cannon tower from the full town name.
- **Now**: the kit path builds it from the literal `"CGT-Aresen"` (S 47670); the other two code paths use `"CGT-Aresden"` (S 48091, 48763). No Npc.cfg copy holds `CGT-Aresen`: 2.03 has no crusade-structure NPCs at all (30 NPCs, types 10-35); 3.51 names it `CGT-Aresde`; 3.82 `CGT-Aresden`. As written the Aresden kit finds no NPC definition under that name [I]. [V]
- **Where**: S Game.cpp:47670, 48091, 48763
- **Evidence**: read in source and config copies.
- **Blocks 2.20 setup**: unknown
- **Source doc**: research/systems/crusade.md:229

### CRUSADE-008 Meteor damage formulas are overwritten
- **Program**: game server
- **System**: crusade (meteor)
- **Kind**: dead code
- **Intent**: earlier meteor damage formulas.
- **Now**: they are overwritten by `iDice(1, level) + level`. [V doc] The first two formulas are overwritten, so damage is level+1 to 2*level, capped at 255 (merged from GSO-026). [V]
- **Where**: S Game.cpp:49873-49876
- **Evidence**: doc crusade-quests.md.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crusade.md:234; research/game-server/functions/crusade-quests.md:504

### CRUSADE-009 Collected-mana log guarded by the wrong side's value
- **Program**: game server
- **System**: crusade (mana)
- **Kind**: bug
- **Intent**: log each side's mana from its own value.
- **Now**: the Elvine log is guarded by the Aresden value (copy-paste). [V doc] The Elvine line is guarded by `wAresdenMana > 0` (merged from GSO-025). [V]
- **Where**: S Game.cpp:48173-48195
- **Evidence**: doc crusade-quests.md.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crusade.md:237; research/game-server/functions/crusade-quests.md:370

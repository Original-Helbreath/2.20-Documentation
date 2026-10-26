# Applied log: systems world-combat (round 2)

Status: A applied, R rejected, H handoff. Docs under research/systems unless stated.

| id | status | doc | note |
|---|---|---|---|
| COMBAT-1 | A | combat.md sec 7 | tile mask 0x6 melee / 0x5 magic added |
| COMBAT-2 | A | combat.md sec 7 | crit refill sources |
| COMBAT-3 | A | combat.md sec 3 | _CheckAttackType reworded |
| COMBAT-4 | A | combat.md sec 7, 8 | melee halving vs magic +40% mana |
| COMBAT-5, 6 | A | combat.md sec 10 | closed / filled |
| COMBAT-7 | A | combat.md sec 7 | ALLPLAYERPROTECT commented out everywhere |
| COMBAT-8 | A | combat.md sec 7 | item-granted ability, 1200 at login |
| COMBAT-9 | A | combat.md sec 9 | :4082 removed; deleters are :4020, :4050 |
| COMBAT-10, 11 | A | combat.md | :19262; [I] on shipped build |
| PKLAW-1..8, 10 | A | pk-law.md | residual branch dead; 1-tick building recall; hunter spell limits; 3 thresholds; GM-close-only bisle; client-only reward check; stone dropped; reward exp bypasses GetExp; cites |
| PKLAW-9 | R | pk-law.md | doc makes no "hunters don't raise EK" claim; nothing to retag |
| MAP-1..7 | A | map.md | cites, 60x60 cells, client condition DEF_LANGUAGE==3, [I] on -1 |
| TILE-1 | R | tile.md | index x+y*SizeY, array SizeX*SizeY: overruns whenever SizeY>SizeX ((SY-1)(SY-SX)>0); doc already right |
| TILE-2, 3 | A | tile.md | ClearOwner wording; cite 1221 |
| MOVE-1..4 | A | movement.md | cites, SP wording, one-sided turn, attack-rate link |
| ENT-1..4 | A | entity.md | Ogre 90 s, crop early return, /tgt rule, taming caller aligned |
| GATH-1..5 | A | gathering.md | all applied |
| MAGIC-1..4 | A | magic.md | all applied |
| SKILL-1..5 | A | skills.md, mechanics.md sec 3 | SSN doubling only in SkillIndex path; flood counter is crafting only; C:8521 |
| CHAR-1..3 | A | character.md | break list, >3000 ms tick, hunger-0 regen |
| AD-4 | A | mechanics.md:48 and character.md | per character, fixed window restarted by passing check |
| Task 1 | A | combat.md, character.md | loop DOES exit early: live `break` at :4028/:4058 after DeleteClient at :4020/:4050; :4082 is in a comment and removed; timeout delete does not exit the loop |
| Task 3 links | verified | all owned docs | every *.md named resolves (script); one abbreviated ".../map-movement.md" exists |
| Task 3 party | A | mechanics.md:45 | attacker unit/10 is per own hit (bIsAttackerOwn, >1 member), not "made the kill"; party.md F5 is correct, no handoff |
| Task 3 commands | verified | combat/pk-law | message names and S line cites match commands/common-types.md, msgids.md, notify-types.md |

# Game mechanics in the v2.20 HGserver

All formulas below were read in the v2.20 export (`.claude/scratch/srv220/`, commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]). They are
tagged [V] (verified in code) unless marked [I] (inferred). Citations use `SRV Game.cpp:line`, the line of
the function or statement. "1dN" means `iDice(1,N)`, a uniform roll in 1..N (`SRV Game.cpp:20208`).
Integer division applies everywhere unless a cast is shown. Build switches change some rules: see
`build-config.md`. Function details are in `functions/*.md`.

No web sources were used. The code is the authority for what this build does.

## Contents
1. Character stats and regeneration
2. Experience and levels
3. Skills
4. Physical combat
5. Magic
6. Death, PK and rewards
7. Monster drops
8. Economy: shops, repair, upgrade, carry weight
9. Gathering and crafting: fishing, mining, alchemy, manufacturing, farming
10. Poison, hunger and timed checks
11. Chat
12. Crusade, hunter mode and town rules
13. Current behaviour vs intent

## 1. Character stats and regeneration
- [V] Max HP = Vit*3 + Level*2 + Str/2, then reduced by the MaxHPdown side effect as `iRet - iRet/MaxHPdown` (a divisor, not a percentage) (`SRV Game.cpp:42328`-`42338`).
- [V] Max MP = 2*Mag + 2*Level + Int/2 (`SRV Game.cpp:20343`).
- [V] Max SP = 2*Str + 2*Level (`SRV Game.cpp:20375`).
- [V] Each stat is capped at 200 (DEF_CHARPOINTLIMIT). A level-up gives 3 points (DEF_TOTALLEVELUPPOINT), spent through the LU_* allocation the client set with MSGID_LEVELUPSETTINGS (`SRV Game.cpp:27210`, `:27310`).
- [V] HP regen every 15 s (DEF_HPUPTIME): 1d(Vit), raised to Vit/2 if lower, plus the HP stock and static stock, plus AddHP %. MaxHPdown also reduces each regen tick the same way (`iTemp - iTemp/MaxHPdown`, `:20318`-`20319`). When hunger is 30 or less the interval grows by (30 - hunger) s; hunger only falls for level >= DEF_LEVELLIMIT non-admins (`:3883`-`3888`). There is no regen at hunger 0, when dead, or while the pretend-corpse skill (19) is active (`SRV Game.cpp:20297`).
- [V] MP regen every 20 s: 1d(Mag) + AddMP % (`SRV Game.cpp:20343`).
- [V] SP regen every 10 s: 1d(Vit/3) + AddSP %, plus 15, 10 or 5 at level 20, 40 or 60 or below (`SRV Game.cpp:20375`).
- [V] Max carry weight = (Str + Level) * 500 (`SRV Game.cpp:28572`). Gold weighs 1/20 per coin (`SRV Game.cpp:47173`).
- [V] Defence ratio = Dex*2 + armour DR (`SRV Game.cpp:35281` in CalcTotalItemEffect); the AddDR bonus is added to the target's defence inside iCalculateAttackEffect (`SRV Game.cpp:12667`), not in CalcTotalItemEffect.

## 2. Experience and levels
- [V] Exp needed for level L: E(L) = E(L-1) + L*(50 + L*(L/17)^2), integer arithmetic (`SRV Game.cpp:27183`). bInit builds the table up to level 299 (`SRV Game.cpp:765`). DEF_MAXLEVEL150 (off) remaps levels 140-180.
- [V] Max level is DEF_PLAYERMAXLEVEL 180, overridable by world-server-max-level (`SRV Game.cpp:25070`).
- [V] Exp is first collected in m_iExpStock. Every 10 s CalcExpStock moves it into exp. On no-penalty/no-reward maps (map type 1) it is discarded. Each level-up also pays 100 gold at levels 1-5 and 300 gold at levels 6-20 (`SRV Game.cpp:36374`).
- [V] Auto exp: every 6 min (DEF_AUTOEXPTIME), a player who gained less than Level exp in that window receives Level exp, but only if adding it would not reach the next level table entry; the grant goes through GetExp (low-level multiplier and party rules apply) then CalcExpStock [V for the timer, `SRV Game.cpp:3827`; guards and grant `:3944`-`3956`].
- [V] GetExp (`SRV Game.cpp:51636`):
  - At level 80 or below, exp is multiplied by 1.025 + (80 - L)*0.025.
  - Above level 80 on arefarm/elvfarm, exp is quartered; at level 100+ on a farm, it is cut to 1/10.
  - Party share applies when the player is a confirmed member, the party has more than one member, and the gain is at least 10. The 5-minute activity test applies to the exp RECEIVER (the player whose kill it is), not to each member; members count if their HP is above 0. Under DEF_V219 the bonus is 0, 2, 5, 7, 10, 14, 17 or 20 % for 1..8 counted members (1 member = 0 %). The total is split equally, and the attacker gets an extra unit/10 when the exp is for the attacker's own hit (`bIsAttackerOwn`) and more than one member counts (`SRV Game.cpp:51688`-`51765`; see `party.md` F5).
- [V] At max level, exp beyond the next table entry is reset and turns into one Gizon (majestic) upgrade point, up to 37 (DEF_MAXGIZONPOINT) (`SRV Game.cpp:27210`).
- [V] Travellers (side neutral) cannot pass level 19: when exp reaches the level-20 table value it is clamped to that value minus 1 (`SRV Game.cpp:27531`). DEF_LEVELLIMIT is used only in the hunger rule (`SRV Game.cpp:3883`) ([CHARACTER-002](../issues/character.md#character-002-def_levellimit-is-documented-as-a-trial-limit-but-used-as-the-hunger-threshold)).
- [V] Speed-hack catcher: gaining more than 300000 exp within an hour sets a one-month block date on the character (the `m_iPenaltyBlock*` fields in the character record, saved with it; the block is per character, not per account). The window is a fixed hour that restarts after each passing check (`m_dwSpeedHackCheckTime`, 3,600,000 ms; the start time and exp snapshot are reset only when the check passes); the block date is next month, same day (year rollover handled). Compiled out under DEF_TESTSERVER (`SRV Game.cpp:4027`-`4057`).

### Exp from monsters
- [V] An NPC's exp is ExpDice d4 + ExpDice, plus special-ability bonus % (`SRV Game.cpp:20037`). HP is HitDice d4 + HitDice when HitDice is 5 or less, otherwise HD*4 + HD + 1d(HD).
- [V] Two thirds of the exp (m_iNoDieRemainExp = exp - exp/3) is paid in pieces as damage is dealt, at the end of iCalculateAttackEffect (`SRV Game.cpp:12306`). The killer gets exp/3 plus whatever is left (`SRV Game.cpp:14449`).
- [V] During crusade, per-hit exp is divided by 3 and kill exp by 4 when it exceeds 10. See [COMBAT-006](../issues/combat.md#combat-006-crusade-kill-exp-cut-applies-on-every-map).
- [V] A player above level 100 gets nothing from rabbits and cats (NPC types 55 and 56).
- [V] AddExp % from rare items applies.

## 3. Skills
- [V] A skill rises when its use counter (SSN) exceeds m_iSkillSSNpoint[mastery+1]. That threshold is L for L up to 50 and L*L/10 above (`SRV Game.cpp:27195`, `:28700`, `:28838`).
- [V] Since v2.19, skills 4-10, 14 and 21 gain SSN twice as fast, but only through CalculateSSN_SkillIndex (`SRV Game.cpp:28838`-`28863`). Weapon hits go through CalculateSSN_ItemIndex and are not doubled (`:14219`-`14223`); in practice magic casts and bare-hand hits are doubled ([SKILLS-005](../issues/skills.md#skills-005-the-v219-ssn-doubling-applies-only-on-the-skill-index-path)).
- [V] Caps by stat (`SRV Game.cpp:28700`, `:28838`), with max 100:

  | Cap | Skills |
  |---|---|
  | Str*2 | 0 mining, 5 hand, 13 manufacturing (2 under the item path) |
  | Level*2 | 3 magic resistance |
  | Mag*2 | 4 magic, 21 staff |
  | Dex*2 | 1 fishing, 6 bow, 7-11 melee and shield, 14 hammer |
  | Int*2 | 2, 12 alchemy, 15, 19 pretend corpse |
  | Vit*2 | 23 poison resistance |

- [V] The total across all skills is capped at 700 (DEF_MAXSKILLPOINTS). bCheckTotalSkillMasteryPoints lowers the skill the player picked with SetDownSkillIndex (1 point, or the whole skill if its mastery is 20 or less). If the picked skill is already at 0, it takes the skill with the lowest SSN among those with mastery 21 or more, excluding the skill just raised; if no skill was picked it finds nothing and returns FALSE (`SRV Game.cpp:32531`).
- [I] Skill names per index follow Skill.cfg, which the Log server pushes. The indices above come from how the code uses them.

## 4. Physical combat
iCalculateAttackEffect (`SRV Game.cpp:12306`) does all of this. The client side is in iClientMotion_Attack_Handler (`SRV Game.cpp:11285`).

Attack value:
- [V] Bare hands: 1d(Str/12). Hit ratio = base HitRatio + skill 5.
- [V] Melee weapon: item dice + bonus, times (1 + Str/5/100).
- [V] Bow: item dice + the item attack bonus + 1d(Str/20).
- [V] Custom item min/max attack clamps the result.

Critical (SuperAttack, attack mode 20 or more, needs m_iSuperAttackLeft and the related weapon skill mastery at 100; otherwise _CheckAttackType downgrades it to a plain attack, `SRV Game.cpp:38193`):
- [V] Damage + Level %, then by weapon skill:

  | Skill | Damage | Hit |
  |---|---|---|
  | 6 bow | +10 % | +30 |
  | 7 dagger | x2 | |
  | 8 long sword | +10 % | +30 |
  | 10 axe | +20 % | |
  | 14 hammer | +20 % | +20 |
  | 21 staff | +20 % | +50 |

  Then +100 hit and the custom attack value.
- [V] Criticals: +1 for every 15 hits taken (capped at Level/10), and +1 per 13 ticks of 3 s from the client timer (`SRV Game.cpp:3827`).
- [V] Dash attack: +20 hit; long sword +10 %, axe and hammer +20 %.

Hit chance:
- [V] Hit ratio also gets +AddAR and a Dex-over-50 bonus. Bow hit ratio drops 5, 10 or 25 % in rain levels 1-3.
- [V] Target defence: player DR as in section 1. Shield magic adds +40 (protection type 3) or +100 (type 4), but only against attackers with a two-hand-slot non-arrow weapon and against NPC attackers; bare-hand and right-hand attackers never get it (`SRV Game.cpp:12944`-`12991`). Arrow protection (type 1) makes bows miss. An attack from behind (both facing the same way) halves DR.
- [V] Hit % = HR/DR*50, clamped to 15-99 (DEF_MINIMUMHITRATIO 15 was 10 in 2.03).
- [V] Attackers with hunger 10 or less or SP 0 also miss 1 in 10.

Damage:
- [V] Berserk doubles non-critical damage. +AddPhysicalDamage. Near-attack mode halves it.
- [V] Against a player, subtract 1d(Vit/10) - 1. The minimum is 1, or 0 when an NPC attacks.
- [V] Combo bonus (count 2-4), tables at `SRV Game.cpp:39355` plus AddCD:

  | Skill | Table |
  |---|---|
  | 5 hand | 0,0,0,1,2 |
  | 6 bow | 0 |
  | 7 | 0,0,1,2,3 |
  | 8 | 0,0,1,3,5 |
  | 9 fencing | 0,0,2,4,8 |
  | 10 axe | 0,0,1,2,3 |
  | 14 hammer | uses the bow table (zeros) |
  | 21 staff | uses the axe table (0,0,1,2,3) |

  Count 1 or less gives no bonus (`SRV Game.cpp:39362`-`39394`) ([COMBAT-007](../issues/combat.md#combat-007-hammer-and-staff-combo-bonuses-borrow-other-weapons-tables)).

- [V] Multipliers: fightzone x1.33. A crusade soldier hitting a player does x2 at attacker level 80 or below, x1.7 at 100 or below, else x1.33.

Against players:
- [V] Lag protection (DEF_RAGPROTECTIONTIME 7000 ms): if the target client's last received packet is more than 7 s old (m_dwTime, set on accept and on every read), the attack does no damage. It protects silent or lagging clients, not freshly logged-in or teleported ones (`SRV Game.cpp:13147`, `:31212`).
- [V] Hit location: body 50 %, legs 25 %, arms 15 %, head 10 %. Armour on that slot absorbs up to 80 %. A shield parries with chance = skill 11 and absorbs up to 80 %.
- [V] Weapon special abilities 1-5, armour special abilities 50-52; luck saves 1 in 10.
- [V] Armour wear per hit: hammer 20/30/35, axe 3, other 1. Applies only against the enemy side, never to neutrals. Hammers can strip armour.
- [V] Poison weapon abilities 5, 6 and 61.
- [V] Knockback at 40+ damage (60+ in a fightzone).

Against NPCs:
- [V] Small/medium vs large dice by NPC size. A negative AbsDamage is physical absorption. NPCs with action limit 1, 2 or 6 are invulnerable. NPCs counter-attack 1 time in 3. Hitting a grand magic generator (type 41) drains its mana.

After each hit:
- [V] Weapon SSN +1, or +1d(victim level or HD) on a kill (doubled when the ATTACKER has 3 HP or less, `SRV Game.cpp:14221`). Weapon life -1 (more in rain); no life is lost when the ATTACKER is neutral (side 0) (`SRV Game.cpp:14242`).

## 5. Magic
PlayerMagicHandler (`SRV Game.cpp:20842`), resistance (`:32301`), damage (`:31094`).

- [V] Cast chance = magery/100 * MCProb[circle] + (Int - 50)/2 (Int term only when Int > 50; magery 0 counts as 1; item-triggered casts use magery 100; `:20911`-`20927`), where MCProb = 300, 250, 200, 150, 100, 80, 70, 60, 50, 40 for circles 1-10. Level penalty: with d = |circle - level/10|, subtract |d*P[circle] - (level%10)/10*P[circle]| where P = 5, 5, 8, 8, 10, 14, 28, 32, 36, 40 for circles 1-10; when the circle is below the level's circle, add 5 per step instead (`SRV Game.cpp:20930`-`20943`). Rain removes 4, 8 or 20 %. A staff with special type 10 adds value*3.
- [V] Mana cost = Value1 from Magic.cfg. Safe-attack mode costs 40 % more outside a fightzone. ManaSaveRatio reduces it.
- [V] Hit ratio = magery + (Mag - 50) + circle adjustment + AddAR. Circle 9+ adds 10000, which defeats protection type 2.
- [V] Resist: MR = skill 3 + AddMR + (Mag - 50) + AddResistMagic. Protection 5 always blocks. Protection 2 blocks when hit ratio is below 1000. HR/MR*50, clamped 15-99, is the chance the spell lands (a roll of 1d100 at or below it means it lands; at 100 or more it always lands), not the resist percent. Admins (level above 0) always resist (`SRV Game.cpp:32301`-`32367`).
- [V] Ice resist chance = 2*AddAbsWater %. A warm potion gives 30 s immunity. Water-attribute NPCs never freeze (`SRV Game.cpp:32370`).
- [V] Spot damage = XdY + Z, times (1 + Mag/3.3/100), + AddMagicalDamage. Fightzone x1.33. Crusade x1.7, x1.5 or x1.33 by level band, only when the attacker is a crusade soldier (duty 1) hitting a player; a protection type 2 target takes half (`SRV Game.cpp:31144`, `:31347`). The per-hit crusade /3 (iCalculateAttackEffect) is separate. Elemental absorption and AddAbsMD apply. Rings 335 (-20 %) and 337 (-10 %) absorb damage and lose life.
- [V] Weather affects spells through iGetWhetherMagicBonusEffect (`SRV Game.cpp:37465`): in weather 1-3 spells 10, 37, 43, 51 get +1 and spells 20, 30 get -1. The code comment says electric up and fire down; the spell-to-element mapping is [I] ([MAGIC-002](../issues/magic.md#magic-002-weather-bonus-table-is-incomplete)).
- [V] Under DEF_GUILDWARMODE (off), spells 31, 40, 41, 46, 54, 55 and 73 are blocked (`SRV Game.cpp:20853`) ([MAGIC-003](../issues/magic.md#magic-003-guild-war-spell-bans-are-compiled-out)).

## 6. Death, PK and rewards
ClientKilledHandler (`SRV Game.cpp:19253`) picks the outcome:
- A neutral (traveller) victim -> ApplyPKpenalty on the killer if the victim was innocent, PK_KillReward if the victim was a criminal.
- A citizen or guildsman victim: a traveller killer gets ApplyPKpenalty if the victim was innocent and nothing if the victim was a criminal; a same-side killer gets ApplyPKpenalty (innocent victim) or PK_KillReward (criminal victim); an other-side killer gets EnemyKillReward.
- A kill by PLAYER_INDIRECT only logs; it has no penalty or reward.

Victim penalty tier:

| Victim | Tier |
|---|---|
| Innocent killed by an enemy | 2 |
| Criminal (PK 1-3) | 3 |
| Murderer (PK 4-11) | 6 |
| Slaughterer (PK 12+) | 12 |
| Innocent killed by an NPC | 1, with item drop |

- [V] ApplyPKpenalty (`SRV Game.cpp:27986`): PK count +1. Exp loss = d(victimLv/2+1, 50) + d(attackerLv/2+1, 50). Rating -10, city crime count +1. If the PK happened inside the killer's own town, the killer is jailed in arejail/elvjail for 180 time units, decremented by 3 per 3 s tick, so about 3 min [I]. DEF_PKPENALTY (off) skips the town test. No penalty in safe mode for an innocent attacker.
- [V] PK_KillReward (`SRV Game.cpp:28058`): only if the killer's PK count is 0; reward gold += iGetExpLevel(victim exp) * 3, capped at DEF_MAXREWARDGOLD.
- [V] EnemyKillReward (`SRV Game.cpp:28083`):
  - Only if the killer's PK count is 0.
  - Exp (3d(3*L) + L)/3 with L = iGetExpLevel(victim exp).
  - EK +1 if that level is 30+ (80+ when the attacker is 80+) and the victim died outside their own town (victim location differs from the map).
  - Gold 1dL.
  - In crusade, war contribution += exp*5 and construction points += the victim's m_iLevel/2.
- [V] ApplyCombatKilledPenalty (`SRV Game.cpp:28263`): PK count -1. During crusade only the PK count is reduced and the function returns (no exp loss, no drops). Exp loss 1d(5*tier*level), divided by 3 for neutrals. Item drops = tier, minus 1 below level 80, and only for non-neutral players.
- [V] _PenaltyItemDrop (`SRV Game.cpp:28338`): a Stone of Sacrifice is consumed instead of any drop. Owner-bound and special-ability items are protected, and luck saves 1 in 10.
- [V] Criminal action (`SRV Game.cpp:35838`): hitting a non-hostile player in aresden, elvine or default spawns a town guard that targets the attacker. It is not called in crusade, and melee calls it only when the hit kills (`SRV Game.cpp:13744`); spells call it from PlayerMagicHandler.
- [V] Player relationship codes (`SRV Game.cpp:37498`): 0 neutral, 1 ally, 2 enemy, 3 same guild, 4 other guild same side, 5 guild master, 6 PK of same town, 7 threatening.

## 7. Monster drops
- [V] No drops from summons, from NPC types 21, 34 or 66, or when the killer is not a player (`SRV Game.cpp:44638`).
- [V] Map item events are checked first (`SRV Game.cpp:45634`).
- [V] Drop happens when 1d10000 >= iItemprobability. It starts at 8500 and is lowered by 900 when the killer exists and its m_iPartyStatus is not DEF_PARTYSTATUS_PROCESSING (NULL 0, PROCESSING 1, CONFIRM 2, so the test is true for non-party players and confirmed members alike) (`SRV Game.cpp:44667`-`44671`). Derived chance: threshold 7600 -> 2401/10000 = 24.01 %; without the deduction 8500 -> 1501/10000 = 15.01 %. The code comment (44665) says the intent was a +10 % bonus for party play; see [GSO-001](../issues/game-server-other.md#gso-001-monster-drop-party-test-lowers-the-threshold-for-nearly-every-kill). The gold branch dereferences m_pClientList[killer] without a NULL check (44704) ([GSO-005](../issues/game-server-other.md#gso-005-monster-gold-drop-dereferences-the-killer-without-a-null-check)). No source gives an earlier base rate (the 6500 at 44686 is the gold-versus-item split).
- [V] Of drops, 65 % are gold: 1d(ExpDice*2) + ExpDice + AddGold %. Otherwise 90 % come from the potion/stone table and the rest are rare weapons or armour by the NPC's gen level 1-10, with random prefix/suffix attributes (_AdjustRareItemValue, `SRV Game.cpp:45951`).
- [V] DeleteNpc (`SRV Game.cpp:30033`) runs at corpse removal, not at death, and can add at most one item: alchemy materials from a hard-coded per-type table (`:30117`-`30290`), or an NpcItem drop through bGetItemNameWhenDeleteNpc. It passes iItemprobability = 65 (55 in Middleland) (`:30105`-`30110`), but that value is only used in the inactive non-DEF_NPCITEM branch as a 1-in-65 / 1-in-55 dice test (`:47350`); the active DEF_NPCITEM branch (`:47193`-`47258`) never reads it. Types 30, 31 and 32 with no item give gold (item 90) ([GSO-008](../issues/game-server-other.md#gso-008-the-drop-probability-deletenpc-passes-is-used-only-by-an-inactive-branch)).

## 8. Economy
- [V] Purchase (`SRV Game.cpp:16820`, RequestPurchaseItemHandler): Charisma discount of (Cha - 10)/4 %, floored near half price: when the discounted price would be at or below half of the undiscounted price, the discount is set so the price ends 1 below half (`cost2/2 - 1`, an off-by-one). Under DEF_V219 the crusade-winning side gets 10 % off (`:16902`, floor `:16936`-`16940`) ([GSO-006](../issues/game-server-other.md#gso-006-charisma-discount-floor-is-off-by-one)).
- [V] Repair price = price/2 - (curLife/maxLife * 0.5 * price) = (price/2) * (1 - life ratio); at 0 life it is price/2. Weapons (categories 1-10) can only be repaired by repairer type 24, armour and tools (11-13, 43-50) by type 15 (`SRV Game.cpp:34428`-`34500`). The repairer-type check is in the quote (ReqRepairItemHandler) only; the confirm step (ReqRepairItemCofirmHandler) just needs the shop flag, charges the same price and pays it into the player's side city funds (`:34524`, `:34596`) ([GSO-007](../issues/game-server-other.md#gso-007-repair-confirmation-has-no-repairer-type-check)).
- [V] Upgrade (the full upgrade system is in [crafting.md](../systems/crafting.md)): success by current +level: 20, 15, 9, 8, 7, 7, 5, 5, 3, 2 %. Crafted items (attribute bit 1 set, value2 above 100) add value2/10, /20 or /40 to the chance depending on the base chance, and bonus stones double it. Which item class triggers the bonus is unverified (`SRV Game.cpp:52441`-`52485`).
- [V] Guild creation under DEF_CHINESEGUILDLIMIT: level 100, Charisma 20, 1,000,000 gold, not neutral, not hunter, not in the PK town state, and not during crusade (`SRV Game.cpp:16642`, `:16669`-`16673`). Without the switch it is level 20 and Charisma 20 (`:16676`). The gold is taken in the Log-server reply handler ResponseCreateNewGuildHandler (`:16584`-`16586`), which re-checks level and Charisma.

## 9. Gathering and crafting
- [V] Fishing: catch when skill > 1d105 for simple fishing. FishProcessor shifts event-fishing chance every 4 s by skill vs difficulty (`SRV Game.cpp:33700`, `:36947`).
- [V] Mining: needs a pickaxe (weapon type 25) equipped with a non-zero hand appearance bit and skill 0 > 0 (`:38908`-`38917`); success when 1d100 <= skill 0 - mineral difficulty, floored at 1 (`SRV Game.cpp:38880`). Mineral type = 1d(generator level) (`:38827`); difficulty/remaining by type 1-6 is 10/20, 15/15, 20/10, 50/8, 70/6, 90/4 (`:38861`-`38867`).
- [V] Alchemy: skill 12 against recipe difficulty (`SRV Game.cpp:38261`).
- [V] Manufacturing: skill 13; quality comes from material values (`SRV Game.cpp:43525`).
- [V] Farming (v2.19): crops are NPC type 64 on farm tiles, with success from bProbabilityTable (`SRV Game.cpp:52945`, `:53189`).

## 10. Poison, hunger and timed checks
- [V] Poison every 12 s: 1d(poison level) damage, never lethal. Cure chance = skill 23 - 10 + AddPR, at least 10 %. Resist roll = skill 23 + AddPR (`SRV Game.cpp:35975`, `:36007`).
- [V] Hunger drops 1 per minute, but not below level 20 or for admins (`SRV Game.cpp:3827`).
- [V] Autosave every 30 min. Client timeout 10 s (`SRV Game.cpp:3827`).
- [V] Force recall from the enemy town after the configured stay time (SetForceRecallTime, `SRV Game.cpp:52716`).

## 11. Chat
ChatMsgHandler (`SRV Game.cpp:10539`). Prefixes:

| Prefix | Channel | Cost |
|---|---|---|
| `$` | party | SP 3 |
| `@` | guild (level > 1) | SP 3 |
| `!` | shout (level > 10) | SP 5 |
| `^` | cross-server guild / admin | needs SP 5, level above 10 and a guild; deducts SP 3 |
| `~` | own side | SP 3 |
| `/` | command | |
| `#` | escapes whisper | |

- [V] During crusade, enemies cannot hear each other (modes 0, 1, 2 and 10, not 3 or 4). The SP cost applies only while m_iTimeLeft_FirmStaminar is 0. Admins (level above 0) send `^` to all servers and use mode 10 (`SRV Game.cpp:10659`-`10678`, `:11076`, `:11104`-`11111`).
- [V] GM commands are parsed inside ChatMsgHandler. Two are protected by hardcoded passwords written as literal prefixes in ChatMsgHandler: `/enableadmincreateitem` and `/enableadmincommand` (`SRV Game.cpp:10932`, `:10937`). The functions they call (`:46323`, `:46341`) only check the admin level (create-item at least 3, or 4 under DEF_TAIWAN; command at least 1) and set m_bIsAdminCreateItemEnabled / m_bIsAdminCommandEnabled. The values are in the source and are not repeated here ([GSO-004](../issues/game-server-other.md#gso-004-two-gm-enable-commands-are-protected-by-hard-coded-passwords-in-source)).
- [V] Under DEF_CHINESE, `/who` needs admin level 3 (`SRV Game.cpp:10753`).

## 12. Crusade, hunter mode and town rules
- [V] Crusade duties: 1 soldier, 2 builder, 3 commander (SelectCrusadeDutyHandler, `SRV Game.cpp:49042`).
- [V] Hunter (civilian) mode, DEF_V219: enabling needs PK count 0, a cityhall map and level 100 or below (`SRV Game.cpp:52793`). Hunters cannot be attacked and cannot attack outside crusade; they are reset by bCheckLevelUp at level 100+ (`:27235`) and at login above level 100 (`:2085`, `:4917`) (`SRV Game.cpp:52782`, `:52825`).
- [V] DEF_BEGINNERTOWNPROTECT (on): neutral innocents in NONPK towns cannot be attacked (`SRV Game.cpp:12592`).
- [V] Energy sphere: 1 chance in 2000 per 3 s, only with 500+ users online (`SRV Game.cpp:46993`).

## 13. Current behaviour vs intent
1. [V] Crusade kill exp: the divide by 4 is inside `if (m_bIsCrusadeMode == TRUE)`, so it applies only while a crusade is running; the map test inside uses `||` and is always true, so during a crusade it applies on every map, including `default`, `arefarm` and `elvfarm` ([COMBAT-006](../issues/combat.md#combat-006-crusade-kill-exp-cut-applies-on-every-map)). Only kills worth more than 10 exp are cut, and only the killer's final share (exp/3 plus the remainder; damage-time exp is not divided).
2. [V] Drop chance: the party condition tests `!= DEF_PARTYSTATUS_PROCESSING` (`SRV Game.cpp:44667`-`44671`), which is true for status 0 (no party) and 2 (member) and false only for 1 (transient create/join/dismiss), so nearly every kill has the threshold lowered by 900: about 24.01 % instead of the 15.01 % ([GSO-001](../issues/game-server-other.md#gso-001-monster-drop-party-test-lowers-the-threshold-for-nearly-every-kill)).
3. [V] CMisc::bEncode/bDecode return before doing anything, so the XOR string encoding is a no-op (`SRV Misc.cpp:232`) ([GSO-002](../issues/game-server-other.md#gso-002-cmiscbencode-and-bdecode-are-no-ops)).
4. [V] Version.h under DEF_CHINESE reports 2.19 (build 429), so this "2.20" source builds and reports as v2.19 ([GSO-003](../issues/game-server-other.md#gso-003-the-220-source-builds-and-reports-as-version-219)).

# System: magic (spells, mana, casting, effects, magic types)

*Cohesive single doc: learning, casting and effects form one pipeline. Formula detail lives in
`research/game-server/mechanics.md` section 5 and is pointed to, not repeated.*

Sources: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`,
`CFG` = `sources/hgserver-librehelbreath/bin/config/Magic.cfg` (EUC-KR; decoded copy in
`.claude/scratch/systems/combat/Magic.cfg.utf8.txt`). Tags [V]/[I]/[U] as in `combat.md`.

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flow -
7. Rules - 8. Couplings - 9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose
A ten-circle spell book that a character buys spell by spell, gated by Int and gold, and casts with mana.
Intent visible in code:
- **Magery as a skill, Int as a gate, Mag as power**: Int decides what may be learned (CFG ReqInt) and adds
  cast chance above 50; skill 4 (magic ability) scales cast chance; Mag scales damage and resistance
  (`S Game.cpp:20911-20929`, mech 5) [V].
- **Level-appropriate circles**: a large per-circle penalty when the circle is above level/10 stops low
  characters spamming high spells bought early (`S Game.cpp:20930-20943`) [V].
- **Casters are unarmed**: casting requires no shield/two-hander and only a staff-class weapon (types 35-38)
  in the right hand (`S Game.cpp:20885-20897`) [V] - the mage/fighter split is enforced by equipment.
- **Weather and terrain matter**: rain lowers cast chance and shifts electric up / fire down.
- **One spell per 100 ms per player** (the `m_dwRecentAttackTime` gate) and a failed cast costs no mana. The separate 1500 ms `bCheckClientMagicFrequency` (S :52645) is never called, so the 100 ms gate is the only cast throttle ([MAGIC-001](../issues/magic.md#magic-001-bcheckclientmagicfrequency-is-defined-and-never-called)).

## 2. Subsystems
| # | Subsystem | Intent | Nests in |
|---|---|---|---|
| 2.1 | Spell config | 100 spell slots pushed from the log server; index/10 + 1 = circle | startup |
| 2.2 | Learning (study) | buy at the wizard tower or learn from a scroll; Int + gold gates | shop / item use |
| 2.3 | Free magery grant | GETMAGICABILITY sets skill 4 to 20 once | skills |
| 2.4 | Cast motion | animation and position check, cancels invisibility and skill use | MOTION |
| 2.5 | Cast resolution | learned check, equipment, cast chance, mana | COMMON MAGIC |
| 2.6 | Targeting and resistance | hit ratio vs magic resistance, protections, ice resistance | 2.7 |
| 2.7 | Effect types | 27 DEF_MAGICTYPE_* behaviours (damage spot/area/linear, heal, SP, teleport, summon, create, protect, hold, invisibility, fields, possession, confuse, poison, berserk, polymorph, tremor, ice, armour break) | 2.5 |
| 2.8 | Timed effects | delay events remove protections, invisibility, berserk, ice, hold | DelayEventProcessor |
| 2.9 | Dynamic fields | fire wall / poison cloud / spike field objects that tick damage | DynamicObjectEffectProcessor |
| 2.10 | NPC casting | monsters use the same effect functions with their own hit ratio | NPC AI |
| 2.11 | Summon control | /hold, /tgt, /free steer summoned creatures by chat | chat |
| 2.12 | Client spell UI | Magic dialog, mana-cost preview, F-key shortcuts | client |

## 3. Functions
Details in `research/game-server/functions/combat-magic.md` (cm) and client `magic-effects.md`.

| Subsystem | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | _bDecodeMagicConfigFileContents | game server | S Game.cpp:24173 | parses MSGID_MAGICCONFIGURATIONCONTENTS into m_pMagicConfigList[100] [V cm] |
| 2.1 | bInitMagicCfgList | client | (config-loading.md) | client loads its own magic list for the dialog [V map] |
| 2.2 | RequestStudyMagicHandler | game server | S Game.cpp:24898 | Int, gold (purchase), negative-price block, wizard-tower flag, already-known check; sets m_cMagicMastery[i]=1 [V] |
| 2.2 | _iGetMagicNumber | game server | S Game.cpp:25363 | name -> index, ReqInt, cost [V cm] |
| 2.2 | iSetSide (tower flag) | game server | S Game.cpp:52927-52930 | `m_bIsOnTower` set only on map "wzdtwr" (v2.20) [V] |
| 2.2 | DlgBoxClick_MagicShop | client | C Game.cpp:22720 | sends REQ_STUDYMAGIC [V map] |
| 2.3 | GetMagicAbilityHandler | game server | S Game.cpp:46144 | skill 4 := 20, re-check 700 cap [V ch] |
| 2.4 | iClientMotion_Magic_Handler | game server | S Game.cpp:20773 | position only, clears skill use / invisibility, RESPONSE_MOTION [V cm] |
| 2.5 | PlayerMagicHandler | game server | S Game.cpp:20842-22814 | whole cast resolution and per-type effect switch [V] |
| 2.5 | UseMagic | client | C Game.cpp:48061 | client-side rejections (not learned, HP 0, mana short, item in hand, skill in use) [V map] |
| 2.5 | iGetManaCost | client | C Game.cpp:48009 | mirrors server mana cost incl. safe-mode surcharge [V map] |
| 2.6 | bCheckResistingMagicSuccess | game server | S Game.cpp:32301 | resist roll; trains skill 3 [V cm] |
| 2.6 | bCheckResistingIceSuccess | game server | S Game.cpp:32370 | freeze resist [V cm] |
| 2.6 | iGetWhetherMagicBonusEffect | game server | S Game.cpp:37465 | weather +-1 per spell [V cm] |
| 2.7 | Effect_Damage_Spot / _DamageMove | game server | S Game.cpp:31094 / :31646 | damage, own PvP filters (not identical to melee: tile mask 0x5, no safe-mode halving, see combat.md section 7), death [V] |
| 2.7 | Effect_HpUp_Spot / SpDown / SpUp | game server | S Game.cpp:32161 / :32209 / :32267 | heal, stamina drain/restore [V cm] |
| 2.7 | SetInvisibilityFlag / SetBerserkFlag / SetIceFlag | game server | S Game.cpp:33165 / :33188 / :33212 | status bits + broadcast [V cm] |
| 2.7 | RequestTeleportHandler | game server | S Game.cpp:23324 region | recall (type 8) uses the map system, honours locked maps [V] |
| 2.8 | DelayEventProcessor | game server | S Game.cpp:33021 | expires timed effects, sends MAGICEFFECTOFF [V def] |
| 2.9 | DynamicObjectEffectProcessor | game server | S Game.cpp:33283 | field damage; also calls resist checks and death handlers [V def] |
| 2.9 | CheckFireBluring | game server | S Game.cpp:39241 | fire spreads onto coal items [V cm] |
| 2.10 | NpcMagicHandler | game server | S Game.cpp:22817 | NPC casts [V cm] |
| 2.12 | DrawDialogBox_Magic / DlgBoxClick_Magic | client | C Game.cpp:39369 / :42455 | spell list per circle, cast on click [V map] |
| 2.12 | NotifyMsg_MagicEffectOn / Off | client | C Game.cpp:44553 / :44441 | status icons and messages [V] |
| 2.12 | NotifyMsg_MagicStudySuccess | client | C Game.cpp:44713 | learned message [V] |

## 4. Commands and messages
- Casting is split over two packets (`overloaded.md`): `MSGID_COMMAND_MOTION` `DEF_OBJECTMAGIC` (spell in dX,
  animation; S Game.cpp:1112) then `MSGID_COMMAND_COMMON` `DEF_COMMONTYPE_MAGIC` (target, `iV3-100` = spell;
  S :15580) [V].
- Learning: `DEF_COMMONTYPE_REQ_STUDYMAGIC` (S :15536); scroll learning arrives through
  `DEF_COMMONTYPE_REQ_USEITEM` -> UseItemHandler (S :30626) [V]. `DEF_COMMONTYPE_GETMAGICABILITY` (S :15443).
- Notifies: `MAGICSTUDYSUCCESS`, `MAGICSTUDYFAIL`, `MP`, `MAGICEFFECTON`, `MAGICEFFECTOFF` [V by-system].
- Events: `MSGID_EVENT_COMMON` `DEF_COMMONTYPE_MAGIC` (spell visual at a point), `EVENT_MOTION`
  `OBJECTMAGIC` (cast pose), `OBJECTDAMAGE` with 0 damage (failed cast flinch, S :20995) [V].
- Slash: `/hold`, `/tgt <name>`, `/free` (summon control), GM `/setinvi` [V by-system].
- Config: `MSGID_MAGICCONFIGURATIONCONTENTS` from the world log server (game-log protocol).

## 5. Data
- `CMagic` config row (S Magic.h): type (DEF_MAGICTYPE_*, S Magic.h:14-41), delay, last (duration),
  `m_sValue1` mana cost, ranges, three dice triples (V4-V12), `m_iGoldCost`, ReqInt, category, attribute
  (CFG header line) [V].
- Spell table (CFG): 50 rows, circles 1-10 by index/10; e.g. 0 Magic-Missile, 1 Heal, 12 Recall,
  20 Fire-Ball, 32 Invisibility, 43 Lightning, 50 Berserk, 61 Mass-Fire-Strike, 70 Bloody-Shock-Wave,
  73 Cloud-Kill, 80 Illusion, 90 Mass-Illusion [V CFG].
- CClient: `m_cMagicMastery[100]` (learned 0/1, persisted), `m_iMP`, `m_iManaSaveRatio`,
  `m_cMagicEffectStatus[]`, `m_sStatus` bits (0x10 invisibility, berserk, ice, protection 0x0F00 per
  C DrawNpcName), `m_dwRecentAttackTime` (shared with melee), `m_iSpecialWeaponEffectType/Value` (staff
  bonus) [V at uses].
- Static tables `_tmp_iMCProb[]`, `_tmp_iMLevelPenalty[]` (cast chance by circle) used at S :20911-20943 [V].

## 6. Flow
**A. Learn a spell at the wizard tower**
1. Client MagicShop dialog -> `REQ_STUDYMAGIC` (C :22720) [V map].
2. ClientCommonHandler (S :15536) -> RequestStudyMagicHandler (S :24898): name -> index (S :24915);
   purchase path requires `m_iGoldCost >= 0`, gold >= cost, `m_bIsOnTower` (S :24922-24926); already
   known -> silent return (S :24930); Int >= ReqInt -> gold deducted, `m_cMagicMastery = 1` (S :24932-24941)
   -> `NOTIFY_MAGICSTUDYSUCCESS`, else `MAGICSTUDYFAIL` [V].
3. Persisted with the character at the next save (character/persistence).

**B. Cast**
1. Client: Magic dialog or F-key -> UseMagic (C :48061) local checks -> enters pointing mode; click
   -> MOTION OBJECTMAGIC [V map].
2. Server: iClientMotion_Magic_Handler (S :20773) -> broadcast cast pose with spell id (S :1112-1118) [V].
3. Client finishes the animation and sends COMMON MAGIC -> ClientCommonHandler (S :15580) ->
   PlayerMagicHandler (S :20842).
4. PlayerMagicHandler gates (S :20869-20899): bounds, spell exists, learned unless item effect, map allows
   attack, right hand empty or staff type 35-38, no left-hand/two-hand item, >= 100 ms since last
   attack/cast [V].
5. Cast chance (S :20911-20965), mana cost (S :20971-20988), failure roll -> 0-damage flinch broadcast,
   no mana spent (S :20990-20997); hunger <= 10 or SP 0 -> 10 % extra failure (S :21000-21004); MP short ->
   silent return (S :21007) [V]. Category-1 (damage) spells are blocked on tiles with the safe-zone / magic-protection attribute (`iGetAttribute(..., 0x5)`, S :21044-21047), unlike melee, which tests mask 0x6.
   Hunter rules, outside crusade (S :21060-21085): a hunter cannot cast on a player of the other side (nor onto a hunter of the other side), and a hunter casting on a non-hunter is refused for SPDOWN_AREA, SUMMON, PROTECT, HOLDOBJECT, INVISIBILITY, BERSERK, POISON and HPUP_SPOT (`pk-law.md`) [V].
6. Per-type switch: effect functions, resist checks (2.6), criminal-action checks for hostile spells
   (S :22331-22361, :22600), skill-4 SSN gain via CalculateSSN_SkillIndex [V cm].
7. Effects reach clients as EVENT_COMMON MAGIC (visual), EVENT_MOTION OBJECTDAMAGE, NOTIFY_HP /
   MAGICEFFECTON; MP deducted and NOTIFY_MP sent [V cm].
8. Timed effects are registered as delay events and removed by DelayEventProcessor (S :33021) ->
   MAGICEFFECTOFF -> client NotifyMsg_MagicEffectOff (C :44441) [V].

**C. Scroll cast** - UseItemHandler calls PlayerMagicHandler with `bItemEffect = TRUE`: the learned check
is skipped and magery counts as 100, but mana, equipment and map gates still apply (S :20879, :20917) [V].

## 7. Rules and invariants
Cast chance, level penalty, mana, hit ratio, resist and damage formulas: `mechanics.md` section 5.
Verified additions:
- Circle = spell index / 10 + 1; spell index must be 0-99 (S :20876, :20911) [V].
- Magery 0 counts as 1 (S :20912-20915) [V].
- Rain: cast chance -1/24, -1/12, -1/5 for weather 1-3 (S :20949-20953; the code comment says 4, 8, 20 %) [V].
- Staff special type 10 adds value*3 to cast chance (S :20957-20963) [V].
- Safe-attack mode outside a fightzone: mana + (cost/2 - cost/10), i.e. +40 % (S :20971-20975); the client
  preview uses the same expression (C :48009) [V].
- Weather bonus: spells 10 Energy-Bolt, 37 Lightning-Arrow, 43 Lightning, 51 Lightning-Bolt get +1;
  20 Fire-Ball, 30 Fire-Strike get -1 (S :37465; names from CFG) - so the comment "electric up, fire down"
  matches the names [V].
- Study: Int >= ReqInt, cost paid only on purchase, negative cost = not purchasable (S :24898-24941) [V].

## 8. Couplings
| Direction | Other system (owner) | Coupling point |
|---|---|---|
| magic -> | combat (`combat.md`) | Effect_Damage_Spot shares legality filters, lag protection (S :31212), ClientKilledHandler / NpcKilledHandler |
| magic -> | skills (`skills.md`) | skill 4 magery (cast chance, SSN), skill 3 magic resist (SSN on resist), skill 23 poison resist |
| magic -> | character (`character.md`) | Int/Mag/level, MP regen and max MP, hunger/SP failure rule |
| magic -> | pk-law (`pk-law.md`) | bAnalyzeCriminalAction on hostile spells (S :22331 ff) |
| magic -> | map/teleport (world cluster) | RequestTeleportHandler for Recall; locked-map (jail) override (S :23324) |
| magic -> | world/entity | summon via bCreateNewNpc; possession; polymorph; dynamic objects |
| magic -> | items (items cluster) | Create-Food creates items (`_bAddClientItemList`), mana-save items, staff specials, rings 335/337 absorb |
| magic -> | crusade (world/social) | crusade multipliers in Effect_Damage_Spot; DEF_GUILDWARMODE blocks |
| -> magic | item use (items cluster) | scrolls call PlayerMagicHandler / RequestStudyMagicHandler (UseItemHandler S :30626) |
| -> magic | shop / location | wizard tower flag from iSetSide (S :52931) |
| -> magic | client presentation | bAddNewEffect visuals (C Game.cpp:5891), DrawObject_OnMagic (C :9894) |

## 9. Current behaviour vs intent
- [V] **Cast pose sends a constant magery of 10** (S :1112-1118) although the source comment names the magic ability level; any client effect scaled by it is constant ([COMBAT-002](../issues/combat.md#combat-002-cast-motion-carries-a-hard-coded-magery-value)).
- [V] **Weather table is incomplete**: Mass-Lightning-Arrow (56), Lightning-Strike (74) and Mass-Fire-Strike (61) are not in iGetWhetherMagicBonusEffect, so later electric/fire spells ignore weather (S :37465; CFG names) ([MAGIC-002](../issues/magic.md#magic-002-weather-bonus-table-is-incomplete)).
- [V] DEF_GUILDWARMODE spell bans (S :20853) and the special-ability ban are compiled out (S GlobalDef.h:8) ([MAGIC-003](../issues/magic.md#magic-003-guild-war-spell-bans-are-compiled-out)).
- [V] Failed casts are free (mana is checked and spent after the failure roll), which makes spamming high-circle spells cheap for underlevelled casters except for time [I on intent].
- [I] Cast chance has no Int cap: Int above 50 adds (Int-50)/2 even beyond 100 %, so for low circles Int investment is wasted once the result reaches 100.

## 10. Open questions
- The full meaning of every CFG column after the dice triples (category, attribute) beyond what the
  header comment names [U]; attribute codes appear to be 1 earth, 2 air, 3 fire, 4 water by the garbled
  header comment, not confirmed.
- Whether the client's own magic list (bInitMagicCfgList) can disagree with the server CFG and what the
  client then shows [U].
- Effect-by-effect rules for the 27 types (possession, confuse, polymorph durations) were not traced one
  by one; per-type details belong in a sidecar if needed.

## 11. Doc discrepancies
- `combat-magic.md` and `mechanics.md` 5 tag the weather element mapping [I]; the CFG names make it [V]
  (electric spells up, fire spells down).

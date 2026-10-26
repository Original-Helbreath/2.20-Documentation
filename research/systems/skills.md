# System: skills (skill list, skill gain, mastery)

*Cohesive single doc. Cap tables live in `research/game-server/mechanics.md` section 3 and are pointed to.*

Sources: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`,
`SCFG` = `sources/hgserver-librehelbreath/bin/config/Skill.cfg` (EUC-KR; decoded copy in
`.claude/scratch/systems/combat/Skill.cfg.utf8.txt`). Tags [V]/[I]/[U].

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flow -
7. Rules - 8. Couplings - 9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose
Use-based skill growth in the Ultima Online tradition: a skill rises by doing it, every rise is capped by
a governing stat (stat x 2) and the character's total is capped at 700 points, so specialising in one
area means letting another decay. The player picks which skill gives way ("down skill"). Intent:
- **Learn by doing** - every successful swing, cast, resist, catch or craft adds SSN (skill-use points)
  to the skill that was used (`S Game.cpp:28700`, `:28838`) [V].
- **Stats govern ceilings** - Str, Dex, Int, Mag, Vit and level each cap a skill group (mech 3) [V].
- **Forced specialisation** - the 700 total cap (DEF_MAXSKILLPOINTS, S Game.h:84) [V].
- **Faster combat progression since v2.19** - the SSN value is doubled for skills 4-10, 14 and 21, but only inside CalculateSSN_SkillIndex (S :28852-28863); the weapon-hit path CalculateSSN_ItemIndex does not double. In practice magic casts (skill 4, S :22806) and bare-hand hits (skill 5, S :14260) are doubled; melee weapon hits are not [V].
- **Active skills** - some skills are invoked (mining via attack, fishing, pretend corpse, alchemy,
  manufacturing) and succeed with probability = mastery % (S :33832-33838) [V].

## 2. Subsystems
| # | Subsystem | Intent | Nests in |
|---|---|---|---|
| 2.1 | Skill config | 24 skills (0-23) with effect type and values, pushed from the log server | startup |
| 2.2 | SSN accrual and mastery rise | use-based growth, threshold curve, stat caps | called from every using system |
| 2.3 | Total-points cap and down-skill | keep the sum <= 700 by lowering the chosen skill | 2.2 |
| 2.4 | Active skill use | invoke a skill (REQ_USESKILL); success = 1d100 <= mastery | COMMON |
| 2.5 | Skill-using status | one use per skill at a time; cancelled by moving, attacking, casting | per client |
| 2.6 | Skill acquisition | manuals (item use) and the one-off magery grant; the TRAINSKILL command is dead | items |
| 2.7 | Weapon-skill binding | weapon class -> skill; mastery feeds hit ratio and gates criticals | combat |
| 2.8 | Special abilities | timed weapon/armour powers with a 20-minute cooldown (activation here, effect in combat) | combat |
| 2.9 | Client skill UI | skill list dialog, down-skill selection, crafting skill dialog | client |

**Skill list** (index, SCFG name translated, governing cap from S :28883-28950 / mech 3) [V names, V caps]:
0 mining (Str), 1 fishing (Dex), 2 farming (Int, v2.19), 3 magic resistance (Level), 4 magic ability /
magery (Mag), 5 hand-to-hand (Str), 6 archery (Dex), 7 short sword (Dex), 8 long sword (Dex), 9 fencing
(Dex), 10 axe (Dex), 11 shield (Dex), 12 alchemy (Int), 13 weapon manufacturing (Str), 14 "cooking" in
SCFG but hammer in code (Dex), 15 first aid (Int), 16 stealing, 17 musicianship, 18 item identification,
19 pretend corpse (Int), 20 taming, 21 staff (Mag), 22 armour manufacturing, 23 poison resistance (Vit).
Skills 16, 17, 18, 20, 22 fall to the default case (no stat cap, S :28951-28953) and no gameplay caller was found [I].

## 3. Functions
Details in `research/game-server/functions/character.md` (ch).

| Subsystem | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | _bDecodeSkillConfigFileContents | game server | S Game.cpp:24487 | parse Skill.cfg into m_pSkillConfigList[60] [V ch] |
| 2.1 | bInitSkillCfgList | client | C Game.cpp:8521 | client loads SKILLCFG.TXT locally ("wrong information" box at C :899) [V] |
| 2.2 | _iCalcSkillSSNpoint | game server | S Game.cpp:27195 | threshold table: L (L <= 50), L*L/10 above [V ch] |
| 2.2 | CalculateSSN_SkillIndex | game server | S Game.cpp:28838 | add SSN by skill index; x2 for 4-10, 14, 21 (this function only); rise, cap, notify SKILL [V] |
| 2.2 | CalculateSSN_ItemIndex | game server | S Game.cpp:28700 | same, skill taken from the weapon item, no doubling; +1 hit ratio on rise [V ch] |
| 2.3 | bCheckTotalSkillMasteryPoints | game server | S Game.cpp:32531 | enforce 700 cap via down-skill [V ch] |
| 2.3 | SetDownSkillIndexHandler | game server | S Game.cpp:39397 | store m_iDownSkillIndex, notify DOWNSKILLINDEXSET [V ch] |
| 2.3 | NotifyMsg_DownSkillIndexSet | client | C Game.cpp:25064 | store index, unlock dialog 15 [V map] |
| 2.4 | UseSkillHandler | game server | S Game.cpp:33802 | validate, success roll, per-type effect (pretend corpse etc.) [V] |
| 2.4 | iCalculateUseSkillItemEffect | game server | S Game.cpp:33700 | fishing/use-with-item outcomes (gathering, items cluster) [V def] |
| 2.5 | ClearSkillUsingStatus | game server | S Game.cpp:33674 | clear all using flags, cancel event fishing [V] |
| 2.5 | ClearSkillUsingStatus | client | C Game.cpp:34184 | "skill use stopped" message, close dialogs [V map] |
| 2.6 | TrainSkillResponse | game server | S Game.cpp:25385 | manual learned: set skill if 0, re-check cap, SKILLTRAINSUCCESS [V ch] |
| 2.6 | GetMagicAbilityHandler | game server | S Game.cpp:46144 | magery := 20 [V ch] |
| 2.7 | _iGetWeaponSkillType | game server / client | S Game.cpp:39309 / C :23769 | weapon -> skill [V cm] |
| 2.8 | ActivateSpecialAbilityHandler | game server | S Game.cpp:47113 | start ability, cooldown [V cm] |
| 2.9 | DrawDialogBox_Skill / DlgBoxClick_Skill | client | C Game.cpp:41134 / :42843 | skill list, use / down-skill [V map] |
| 2.9 | NotifyMsg_Skill / SkillUsingEnd / SkillTrainSuccess | client | C Game.cpp:45138 / :45228 / :45205 | update mastery, end-of-use message [V] |

## 4. Commands and messages
From `by-system.md` "skills".
- Client -> game (`MSGID_COMMAND_COMMON`): `REQ_USESKILL` (S :15507 -> UseSkillHandler(iV1 skill, iV2, iV3)),
  `REQ_SETDOWNSKILLINDEX` (S :15475), `REQUEST_ACTIVATESPECABLTY` (S :15431), `GETMAGICABILITY` (S :15443);
  `REQ_TRAINSKILL` (S :15541) has a commented body and the client never sends it (`overloaded.md`) [V].
- Game -> client notifies: `SKILL`, `SKILLUSINGEND`, `SKILLTRAINSUCCESS`, `DOWNSKILLINDEXSET`,
  `SPECIALABILITYENABLED`, `SPECIALABILITYSTATUS`; `SKILLTRAINFAIL` has no live sender (`overloaded.md`) [V].
- Config: `MSGID_SKILLCONFIGURATIONCONTENTS` from the world log server.

## 5. Data
- CClient: `m_cSkillMastery[DEF_MAXSKILLTYPE]`, `m_iSkillSSN[]`, `m_bSkillUsingStatus[]`,
  `m_iSkillUsingTimeID[]`, `m_iDownSkillIndex` (persisted as `character-downskillindex`),
  `m_iSkillMsgRecvCount` (flood guard), `m_iHitRatio` (adjusted on rise/fall), `m_iSpecialAbilityTime`,
  `m_bIsSpecialAbilityEnabled` [V at uses; persisted names from S Game.cpp:8917 compose].
- Game: `m_iSkillSSNpoint[]` built in bInit from _iCalcSkillSSNpoint (S :27195) [V ch].
- CSkill config (S Skill.h): name, `m_sType` (DEF_SKILLEFFECTTYPE_*, e.g. PRETEND), `m_sValue1..` [V at S :33836].
- SCFG columns after the name: use flag (0 passive / 1 active / 2 special), then values [I from rows].

## 6. Flow
**A. Passive gain during combat** - iCalculateAttackEffect -> CalculateSSN_ItemIndex (weapon) or
CalculateSSN_SkillIndex (e.g. skill 11 shield, 3 magic resist via bCheckResistingMagicSuccess, 23 poison
resist via bCheckResistingPoisonSuccess) -> SSN += value (x2 only on the CalculateSSN_SkillIndex path for skills 4-10, 14, 21, S :28852-28863; weapon hits via ItemIndex are x1) -> if
mastery < 100 and SSN > threshold[mastery+1]: mastery++, stat-cap test reverts it if exceeded
(S :28876-28953) -> bCheckTotalSkillMasteryPoints -> `NOTIFY_SKILL` -> client NotifyMsg_Skill (C :45138) [V].

**B. Over the 700 cap** - bCheckTotalSkillMasteryPoints (S :32531): repeatedly lowers the down-skill by 1
(or to 0 if mastery <= 20), resets its SSN, lowers hit ratio if the related weapon is equipped, notifies
SKILL; with no down-skill chosen nothing is lowered and the function returns FALSE [V ch].

**C. Active use (pretend corpse example)** - client Skill dialog -> `REQ_USESKILL` -> UseSkillHandler
(S :33802): index valid, config present, not already in use (S :33811-33815); 1d100 > mastery -> SKILLUSINGEND
(S :33832-33838); type PRETEND value 1: not in a fightzone, no corpse on the tile, no owner on the 8 adjacent
tiles (non-admin), invisibility removed, SSN +1 (S :33843-33899) [V]. While in use, HP/MP/SP regen stop
(TimeHitPointsUp S :20297) and stance toggling is refused (S :20518) [V ch].

**D. Learning from a manual** - item use -> UseItemHandler (S :30626) -> TrainSkillResponse (S :25385) ->
mastery set when the skill was 0 -> cap check -> SKILLTRAINSUCCESS [V ch].

## 7. Rules and invariants
Threshold curve, caps per stat and the 700 cap: `mechanics.md` section 3. Verified additions:
- **Mastery 0 never grows** (S :28848): a skill must be acquired (creation, manual, magery grant) before use
  trains it [V].
- **Dead players gain nothing** (S :28845) [V].
- **SSN resets to 0 on each rise** (S :28891 etc.); overflow above the threshold is discarded [V].
- **Hard max 100** (S :28876) [V].
- **One active use per skill at a time** (S :33815) [V]; more than one crafting message per 3 s tick
  disconnects the client (`m_iSkillMsgRecvCount >= 2`, S Game.cpp:4016-4024); the counter is incremented only by
  the crafting handlers ReqCreatePortionHandler (S :38273) and BuildItemHandler (S :43537), not by UseSkillHandler [V].
- **Critical attacks require weapon mastery 100** (S :38193) [V cm].
- **Special ability cooldown** 1200 s (S Client.h:37), counted -3 per 3 s tick (S :3965) [V]. The ability is granted by an equipped item (CalcTotalItemEffect, DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY, S :35760-35812) and is cancelled on unequip; the counter starts at 1200 at login (S Client.cpp:228) [V].

## 8. Couplings
| Direction | Other system (owner) | Coupling point |
|---|---|---|
| skills <- | combat (`combat.md`) | CalculateSSN_* per hit and kill; weapon mastery -> hit ratio, crits, shield parry chance = skill 11 |
| skills <- | magic (`magic.md`) | skill 4 cast chance, skill 3 resistance; SSN on cast and on resist |
| skills <- | gathering / crafting (items cluster) | mining (_CheckMiningAction), fishing (ReqGetFishThisTimeHandler, iCalculateUseSkillItemEffect), alchemy (ReqCreatePortionHandler), manufacturing (BuildItemHandler), farming |
| skills <- | items (items cluster) | manuals via UseItemHandler -> TrainSkillResponse; CalcTotalItemEffect recomputes hit ratio from mastery |
| skills -> | character (`character.md`) | stat caps read Str/Dex/Int/Mag/Vit/Level; regen suppressed while play-dead |
| skills -> | persistence (infrastructure) | mastery, SSN and down-skill saved in the player record |
| skills -> | client presentation | Skill and SkillDlg dialogs, F-key shortcuts |

## 9. Current behaviour vs intent
- [V] **Skill 14 identity**: code treats 14 as the hammer weapon skill (_iGetWeaponSkillType S :39309, combat list S :28859, Dex cap S :28921 "v2.16 fix") but SCFG names it cooking; the client name depends on its own SKILLCFG.TXT [U] ([SKILLS-001](../issues/skills.md#skills-001-skill-14-is-hammer-in-code-but-cooking-in-skillcfg)).
- [V] **Unimplemented skills**: stealing (16), musicianship (17), item identification (18), taming (20), armour manufacturing (22) exist in SCFG, have no stat-cap case in CalculateSSN_SkillIndex (default branch, S :28951) and no gameplay caller was found; taming has a dispatch (S :33737) whose body is a stub, see entity.md ([SKILLS-002](../issues/skills.md#skills-002-skills-16-17-18-20-and-22-are-defined-but-unimplemented)).
- [V] **REQ_TRAINSKILL / SKILLTRAINFAIL** have no live handler (S :15541-15543; fail notify only inside a /* */ block, S :25443-25525); skills are taught by manuals ([SKILLS-003](../issues/skills.md#skills-003-req_trainskill-and-skilltrainfail-are-dead)).
- [V] **No down-skill chosen = no enforcement**: with m_iDownSkillIndex unset the cap function finds nothing and returns (ch, S :32531) ([SKILLS-004](../issues/skills.md#skills-004-the-700-point-cap-is-not-enforced-until-a-down-skill-has-been-chosen)).
- [V] The SSN doubling since v2.19 applies on the CalculateSSN_SkillIndex path only; weapon hits are x1 ([SKILLS-005](../issues/skills.md#skills-005-the-v219-ssn-doubling-applies-only-on-the-skill-index-path)).
- [V] The commented anti-hack skill-use counter (S :33819-33828) was replaced by the per-tick message count in CheckClientResponseTime.

## 10. Open questions
- How new characters receive starting masteries (log server character creation, not in this source) [U].
- Exact semantics of SCFG value columns for active skills 0-2 (mining/fishing/farming "1 1 6") [U].
- Whether client SKILLCFG.TXT for 2.20 names 14 "Hammer" (file not in the source tree) [U].

## 11. Doc discrepancies
- `mechanics.md` 3 lists skill 2 and 15 under the Int cap without names and calls 14 "hammer"; SCFG names
  2 farming, 15 first aid, 14 cooking. Code behaviour (Int cap, hammer) matches mechanics.md; the
  discrepancy is between code and the shipped-looking SCFG [V].
- `mechanics.md` 3 says the Str*2 cap covers "13 manufacturing (2 under the item path)"; not re-checked [U].

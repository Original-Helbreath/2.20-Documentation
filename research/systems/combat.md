# System: combat (melee and ranged attacks, hit and damage, criticals, death, corpse)

*Cohesive single doc (one pipeline, sections presuppose each other); formula tables are NOT repeated
here - they live in `research/game-server/mechanics.md` section 4 and are pointed to.*

Sources: `S` = `sources/hgserver-librehelbreath/src/` (2.20 export, line numbers equal the
`SRV Game.cpp` citations in the function maps), `C` = `sources/client-2.20/`. Tags: [V] read in source,
[I] inferred, [U] unverified. Sibling docs: `magic.md`, `skills.md`, `character.md`, `pk-law.md`.

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flow -
7. Rules and invariants - 8. Couplings - 9. Current behaviour vs intent - 10. Open questions -
11. Doc discrepancies

## 1. Purpose
Real-time, server-authoritative swing-for-swing fighting. The player clicks a target; the client plays
the swing and sends one motion; the server alone decides hit, damage and death and tells everyone
nearby. Design pressures visible in the code:
- **Fairness against lag and cheats** - every attack is re-validated (position, frequency, weapon, target
  tile) and a lagging target cannot be damaged (`S Game.cpp:13147`, `:31212`) [V].
- **Weapon identity** - each weapon class is its own skill (5-10, 14, 21) with its own combo table,
  critical bonus and armour-wear behaviour, so a "build" is a weapon choice [V, `S Game.cpp:39309`, `:39362`].
- **Risk and reward of PvP** - town protections, party/same-side immunity and safe-attack mode keep
  accidental PvP rare; death costs exp and items scaled by criminal tier (detail in `pk-law.md`).
- **Progress paid as you fight** - 2/3 of a monster's exp is paid per hit (settled fact).

## 2. Subsystems
| # | Subsystem | Intent | Nests in |
|---|---|---|---|
| 2.1 | Stance (peace/combat) toggle | visible "weapon drawn" state; required before attacking | client input, appearance bits |
| 2.2 | Attack motion validation | reject desynced, too-fast or impossible swings before any math | MOTION handler |
| 2.3 | Attack-type resolution (normal / critical / dash) | criticals are a scarce, earned resource; dash is a movement-attack | 2.2 |
| 2.4 | Critical (super-attack) economy | refill over time, cap by level | timer tick |
| 2.5 | Engagement legality (PvP filters) | who may hit whom, by side, town, party, hunter, safe mode | 2.6 entry |
| 2.6 | Hit resolution | attack ratio vs defence ratio, 15-99 % | iCalculateAttackEffect |
| 2.7 | Damage resolution | weapon dice, Str, criticals, combos, multipliers, hit location, armour/shield absorb | 2.6 |
| 2.8 | On-hit side effects | special abilities, poison, ice, armour wear, hammer strip, knockback, weapon wear, skill gain, per-hit exp | 2.7 |
| 2.9 | NPC-specific branch | size dice, invulnerable types, counter-attack, grand-magic-generator drain | 2.6-2.8 |
| 2.10 | Death | player death (ClientKilledHandler) and NPC death (NpcKilledHandler), dying broadcast | 2.7 |
| 2.11 | Corpse and restart | corpse stays as a "dead owner" on the tile; player restarts in a respawn map; NPC corpse removed later by DeleteNpc | 2.10 |
| 2.12 | Special abilities (weapon/armour) | timed activated powers with cooldown | skills.md owns activation; combat consumes |

## 3. Functions
Function details are in `research/game-server/functions/combat-magic.md` (cm) and `character.md` (ch); this
table gives the role only.

| Subsystem | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | ToggleCombatModeHandler | game server | S Game.cpp:20518 | flips the stance bit in appearance, broadcasts; refuses while dead/play-dead [V ch] |
| 2.1 | CommandProcessor | client | C Game.cpp:35701 | world click -> move / attack / cast decision [V map] |
| 2.2 | ClientMotionHandler | game server | S Game.cpp:976 | dispatches MOTION subtypes; ATTACK at :1085, ATTACKMOVE (dash) at :1070 [V] |
| 2.2 | iClientMotion_Attack_Handler | game server | S Game.cpp:11285 | position check, 7-msg speed window, target by object id, calls 2.6, replies RESPONSE_MOTION [V cm] |
| 2.2 | bCheckClientAttackFrequency | game server | called S Game.cpp:1082, :1101 | attack-rate hack check (map: network.md) [V call site] |
| 2.3 | _CheckAttackType | game server | S Game.cpp:38193 | downgrades crit / bow modes: no arrows, bow type with a non-bow weapon (appearance type < 40), no crits left, or mastery of the claimed type's skill < 100 [V]; other weapon classes are not matched to the claimed type |
| 2.3 | _iGetAttackType | client | C Game.cpp:23713 | client picks crit code only when the weapon skill is 100 (mirror of the server rule) [V map] |
| 2.4 | CheckClientResponseTime (crit block) | game server | S Game.cpp:4115-4124 | +1 crit every 13 ticks of 3 s (39 s), cap Level/10, notifies SUPERATTACKLEFT [V] |
| 2.5 | iCalculateAttackEffect (head) | game server | S Game.cpp:12336-12632 | all PvP legality filters, see section 7 [V] |
| 2.5 | iGetPlayerRelationship | game server | S Game.cpp:37498 | relation code 0-7 used by safe mode [V cm] |
| 2.6-2.8 | iCalculateAttackEffect | game server | S Game.cpp:12306-14275 | the whole hit/damage/side-effect pipeline; returns exp [V cm] |
| 2.7 | _iGetWeaponSkillType | game server | S Game.cpp:39309 | weapon class -> skill index [V cm] |
| 2.7 | iGetComboAttackBonus | game server | S Game.cpp:39362 | combo bonus per skill and count [V cm] |
| 2.8 | ArmorLifeDecrement | game server | S Game.cpp:52488 | armour durability loss [V def] |
| 2.8 | bCheckResistingPoisonSuccess | game server | S Game.cpp:36007 | poison weapon resist; trains skill 23 [V ch] |
| 2.8 | SetIceFlag | game server | S Game.cpp:33212 | frozen status bit + broadcast [V cm] |
| 2.8 | CalculateSSN_ItemIndex / _SkillIndex | game server | S Game.cpp:28700 / :28838 | weapon skill gain per hit (owner: skills.md) [V ch] |
| 2.9 | NpcBehavior_Attack | game server | S Game.cpp:11805 | NPC swings enter the same iCalculateAttackEffect [V def] |
| 2.10 | ClientKilledHandler | game server | S Game.cpp:19253 | player death, penalty dispatch (pk-law.md) [V cm] |
| 2.10 | NpcKilledHandler | game server | S Game.cpp:14449 | NPC death: drop, kill exp, quest, crusade credit [V cm] |
| 2.10 | NotifyMsg_Killed | client | C Game.cpp:44331 | locks commands, HP 0, shows "you died" + restart hint; killer name deliberately hidden [V] |
| 2.11 | RequestRestartHandler | game server | S Game.cpp:46246 | respawn map choice, HP restore (character.md / pk-law.md) [V ch] |
| 2.11 | DeleteClient (dead branch) | game server | S Game.cpp:2645-2705 | logging out dead rewrites the save map the same way as restart [V] |
| 2.11 | DeleteNpc | game server | S Game.cpp:30033 | corpse removal and late drop (items/world clusters) [V mech] |
| 2.12 | ActivateSpecialAbilityHandler | game server | S Game.cpp:47113 | starts ability; cooldown 1200 s (DEF_SPECABLTYTIMESEC, S Client.h:37) [V] |
| 2.12 | CheckClientResponseTime (ability block) | game server | S Game.cpp:3961-3980 | cooldown -3 per tick, expiry clears Appr4 bits and broadcasts [V] |

## 4. Commands and messages
From `research/systems/commands/by-system.md` "combat" and `overloaded.md`.
- Client -> game: `MSGID_COMMAND_MOTION` with `DEF_OBJECTATTACK`, `DEF_OBJECTATTACKMOVE` (dash),
  `DEF_OBJECTDAMAGEMOVE` (victim reporting its own knockback) [V S Game.cpp:1061-1101].
  `MSGID_COMMAND_COMMON` with `DEF_COMMONTYPE_TOGGLECOMBATMODE` (:15575), `TOGGLESAFEATTACKMODE`
  (:15584), `REQUEST_ACTIVATESPECABLTY` (:15431) [V]. `MSGID_REQUEST_RESTART` (:14974) [V].
- Game -> client: `MSGID_RESPONSE_MOTION` (`OBJECTMOTION_ATTACK_CONFIRM` / `OBJECTMOTION_REJECT`);
  `MSGID_EVENT_MOTION` broadcasts `OBJECTATTACK`, `OBJECTATTACKMOVE`, `OBJECTDAMAGE`, `OBJECTDAMAGEMOVE`,
  `OBJECTDYING`; notifies `HP`, `KILLED`, `SAFEATTACKMODE`, `SUPERATTACKLEFT`, `DAMAGEMOVE`,
  `ITEMLIFESPANEND`, `SPECIALABILITYSTATUS`, `SPECIALABILITYENABLED` [V by-system].
- GM: `/attack <name>` (NPCs attack a player), `/setattackmode <0/1>` (map-wide attack enable,
  DEF_TAIWAN only) - owned by the GM cluster.
- Overloaded: knockback is a round trip - the server sends `NOTIFY_DAMAGEMOVE`, the victim's client
  moves itself and reports `OBJECTDAMAGEMOVE`, which the server re-validates as a move (`overloaded.md`;
  S Game.cpp:1061-1068) [V].

## 5. Data
- CClient fields read/written (S Client.h): `m_iHP`, `m_bIsKilled`, `m_iDefenseRatio`, `m_iHitRatio`,
  `m_cAttackDiceThrow_SM/_L`, `m_cAttackDiceRange_SM/_L`, `m_cAttackBonus_SM/_L` (set by CalcTotalItemEffect,
  S Game.cpp:35250), `m_iSuperAttackLeft`, `m_iSuperAttackCount`, `m_iComboAttackCount`, `m_bIsSafeAttackMode`,
  `m_iSpecialAbilityTime`, `m_bIsSpecialAbilityEnabled`, `m_iSpecialAbilityLastSec`, `m_dwRecentAttackTime`,
  `m_dwTime` (last packet, lag protection), `m_iLastDamage`, `m_sAppr2` (weapon and stance bits 0xF000), `m_sAppr4` (ability glow) [V at the cited uses].
- Persisted: `super-attack-left`, `special-ability-time` in the player record (S Game.cpp:8917 text form;
  the DEF_DBGAMESERVER binary form at :6428 is the active one under DEF_CHINESE) [V field names; I active form].
- Config: Item.cfg weapon dice/specials (items cluster), Npc.cfg HitDice/defence/AbsDamage (world/entity),
  map flags `m_bIsAttackEnabled`, `m_bIsFightZone`, `m_cType` NOPENALTY_NOREWARD (map cluster).
- Constants: DEF_MINIMUMHITRATIO 15 / DEF_MAXIMUMHITRATIO 99 (S Game.h:178,182), DEF_RAGPROTECTIONTIME 7000
  (:88), DEF_SPECABLTYTIMESEC 1200 (S Client.h:37) [V].

## 6. Flow
**A. Player melee or bow attack**
1. Client: click on an enemy in combat stance -> CommandProcessor (C Game.cpp:35701) -> `_iGetAttackType`
   (C :23713) picks normal or crit code -> `bSendCommand(MSGID_COMMAND_MOTION, DEF_OBJECTATTACK, ...)` [V map].
2. Game: MsgProcess `MSGID_COMMAND_MOTION` (S :15043) -> ClientMotionHandler (S :976) -> case ATTACK (S :1085):
   `_CheckAttackType` downgrades the type (S :1087, :38193) [V].
3. iClientMotion_Attack_Handler (S :11285): position/speed/target checks; on reject sends
   OBJECTMOTION_REJECT (S :1099); empty tile may become a mining hit (S :11402) [V cm].
4. iCalculateAttackEffect (S :12306): legality filters (S :12336-12632) -> hit roll -> damage -> on-hit
   effects -> HP change -> `NOTIFY_HP` to victim, `EVENT_MOTION OBJECTDAMAGE` to the area [V cm].
5. Back in ClientMotionHandler: a crit consumed decrements `m_iSuperAttackLeft` (S :1090-1094); the swing
   is broadcast as `OBJECTATTACK` (S :1096); `bCheckClientAttackFrequency` (S :1101) [V].
6. If HP <= 0: ClientKilledHandler / NpcKilledHandler (section C).

**B. Dash attack** - `DEF_OBJECTATTACKMOVE`: the move is validated first, then the attack runs with
`wType = 1` forced (crit flag cleared, comment v2.12) and `bIsDash = TRUE` (+hit, weapon-dependent damage)
(S :1070-1079) [V].

**C. Death and corpse (player)**
1. ClientKilledHandler (S :19253): ignores double death (`m_bIsKilled`, :19262), fightzone timing (:19266),
   sets `m_bIsKilled`, cancels exchange, broadcasts `OBJECTDYING`, sends `NOTIFY_KILLED` with attacker
   name (:19305) [V]. No penalty on NOPENALTY_NOREWARD maps (:19319) [V]. Penalty/reward dispatch: `pk-law.md`.
2. The body stays as a dead owner (`SetDeadOwner`, callee list cm) - it blocks play-dead on that tile
   (S :33852-33858) and is what the dying/dead sprites draw (C DrawObject_OnDead C Game.cpp:11710) [V].
3. Client shows the restart prompt (C :44331). Restart -> `MSGID_REQUEST_RESTART` -> RequestRestartHandler
   (S :46246): respawn map (traveller `default`; citizen jail / resurr1/2 / farm), HP = full formula,
   hunger 100, MP and SP not restored [V ch]. Logging out dead instead runs the same map rewrite inside
   DeleteClient (S :2645-2705) [V].

**D. NPC death** - NpcKilledHandler (S :14449): dying broadcast, `NpcDeadItemGenerator`, killer exp
(exp/3 + remainder), quest counters, crusade construction/war contribution, explosive death spells; the
corpse is removed later by DeleteNpc (S :30033), which may add a second (material) drop [V cm, mech 7].

## 7. Rules and invariants
Formula tables (attack value, crit table, combo table, hit-location split, armour wear, knockback) are in
`mechanics.md` section 4; the rules below are the gates and the ones verified anew here.
- **Legality filters, in order** (S Game.cpp:12336-12632) [V]: attacker map must allow attack (:12339);
  a hunter cannot hit players outside crusade (:12342); attacker must be in combat stance
  (`m_sAppr2 & 0xF000`, :12354); target alive (:12562); innocent hunter targets are immune outside crusade
  (:12565); DEF_BEGINNERTOWNPROTECT: innocent neutral in a NONPK town is immune (:12592, active);
  DEF_SAMESIDETOWNPROTECT (:12588) is compiled OUT in the DEF_CHINESE build selected in S GlobalDef.h:16 (it is defined only under DEF_USA / DEF_CONSULTATION, :48, :85) [V]; DEF_ALLPLAYERPROTECT (:12570) is commented out everywhere, even inside DEF_CONSULTATION (GlobalDef.h:83) [V]; target must
  still stand on the clicked tile (:12597); admins are immune (:12600); a neutral attacker cannot damage an
  innocent player (:12603); same party is immune (:12606). For a player target the safe-zone /
  physical-protection tile check comes first in the damage path: `iGetAttribute(..., 0x6)` on both the attacker's
  and the target's tile, non-zero -> no damage (S :12906-12907) [V]. This is the main safe-town mechanism for
  PvP melee. Magic uses mask 0x5 instead (S :31242 for area effects, :21046 for category-1 spells; see magic.md).
- **Safe-attack mode, melee** (S :12612-12632) [V]: against enemy/criminal relations (codes 2, 6, 7) the attack
  still lands but at HALF attack value; against anyone else it does nothing, except in a fightzone where a
  different guild takes half damage. Safe mode is therefore a damage tax, not only a friendly-fire guard.
  Spells are different: the halving does not apply to magic. Safe mode adds a +40 % mana surcharge outside
  fightzones (S :20972-20976) and, for area spells, the same relation test either lets the spell through or
  returns with no effect (S :31224-31239) [V]. The melee filters above are not reused by Effect_Damage_Spot.
- **Hit chance** clamp 15-99 % (S Game.h:178, :182) [V]; hunger <= 10 or SP 0 adds a 10 % miss [V mech].
- **Lag protection**: target silent > 7000 ms takes no damage (S :13147) [V mech].
- **Criticals**: refill 1 per 39 s up to Level/10 (S :4115-4124); a crit needs mastery 100 in the claimed
  type's skill (S :38193-38257); dash never crits (S :1075-1077) [V]. Other refill sources, all capped at
  Level/10 and all notifying SUPERATTACKLEFT [V]: (a) being hit in melee: `m_iSuperAttackCount` is shared with
  the 3 s timer and a hit-taken count over 14 grants one (S :13847-13857); (b) the AddChargeCritical item
  effect, a percent chance per hit taken (S :13767-13775), also on magic hits (S :31384, :31936). The crit
  bonus switch uses `m_sUsingWeaponSkill` (S :38193-38257 area).
- **Minimum damage** 1 vs players, 0 when an NPC attacks [V mech].
- **Death detection backstop**: after every move/run/damage-move the server checks HP <= 0 and calls
  ClientKilledHandler (S :1045, :1056, :1067, :1080) [V].
- **Special ability**: cooldown 1200 s counted down 3 per 3 s tick; `SPECIALABILITYENABLED` sent when the
  counter reads exactly 3 (S :3961-3963) [V]. The ability itself is granted by an equipped item (CalcTotalItemEffect,
  `DEF_ITEMEFFECTTYPE_DEFENSE_SPECABLTY`, S :35760-35812) and is cancelled on unequip; the counter starts at
  1200 at login (CClient constructor, S Client.cpp:228) [V].

## 8. Couplings
| Direction | Other system (owner) | Coupling point |
|---|---|---|
| combat -> | items (items/trade cluster) | CalcTotalItemEffect (S :35250) sets every attack/defence field; ItemDepleteHandler and ArmorLifeDecrement on wear; `_iGetArrowItemIndex`; ReleaseItemHandler on hammer strip |
| combat -> | skills (`skills.md`) | CalculateSSN_ItemIndex / _SkillIndex per hit; weapon mastery gate on crits; shield skill 11 parry |
| combat -> | character (`character.md`) | iGetMaxHP, hunger/SP miss rule, GetExp per hit (S :12306 tail) |
| combat -> | pk-law (`pk-law.md`) | ClientKilledHandler -> ApplyPKpenalty / rewards; bAnalyzeCriminalAction on a killing blow (S :13745) |
| combat -> | party / guild (social cluster) | `m_iPartyID` immunity (:12606), guild GUID in fightzone safe mode (:12622), party exp split in GetExp (S :51678-51768; `party.md` F5: split among living confirmed members, the attacker's own hit adds unit/10) |
| combat -> | crusade (world/social) | crusade multipliers, construction points in ClientKilledHandler/NpcKilledHandler, `bStockMsgToGateServer` (gate hop) |
| combat -> | world/entity | NpcKilledHandler drops, DeleteNpc corpse, NPC counter-attack, energy sphere goal (bCheckEnergySphereDestination) |
| combat -> | map (world cluster) | `m_bIsAttackEnabled`, fightzone, map type, `m_iIsOnTown` from iSetSide (S :52857) |
| combat -> | gathering (items cluster) | `_CheckMiningAction` when an attack hits an empty tile (S :11402) |
| -> combat | magic (`magic.md`) | Effect_Damage_Spot reuses the death handlers; its legality filters are separate and partly different (magic tile mask 0x5, no safe-mode halving; section 7) |
| -> combat | network/anti-cheat (infrastructure) | bCheckClientAttackFrequency, DEF_RAGPROTECTIONTIME uses `m_dwTime` set on every socket read |
| -> combat | client presentation | DrawObject_OnAttack / OnDamage / OnDying / OnDead (C Game.cpp:8691, :11710), `_iGetFOE` colouring |

## 9. Current behaviour vs intent
- [V] **Safe-attack mode halves damage against enemies** (S :12612-12619). The toggle is presented as
  "no friendly damage"; players who leave it on fight at half strength. Likely deliberate (cost of safety)
  but undocumented anywhere player-facing in the client [I].
- [V] **Town protection is narrower than the switch names suggest**: only the beginner (neutral) protection
  is compiled; same-side citizens in their own town can damage each other in the DEF_CHINESE build selected in GlobalDef.h:16, and melee calls the town guard only on a killing blow (S :13745) ([COMBAT-001](../issues/combat.md#combat-001-same-side-town-protection-is-compiled-out-in-the-selected-build)).
- [V] **Cast motion carries a fixed magery value**: `iTemp = 10` is hard-coded in the cast branch of the dispatcher (S :1112-1118) ([COMBAT-002](../issues/combat.md#combat-002-cast-motion-carries-a-hard-coded-magery-value)).
- [V] **Duplicated victim branches** in ClientKilledHandler: the `m_iGuildRank == -1` (citizen) and the
  guildsman branches make the same decisions (S :19350-19414) ([COMBAT-003](../issues/combat.md#combat-003-duplicated-victim-branches-in-clientkilledhandler)).
- [V] **Killer name is sent then hidden**: the server sends the attacker name in NOTIFY_KILLED
  (S :19305) and the client comments out its display ("it only starts fights", C :44355-44364).
- [V] **Dead-player loop break**: CheckClientResponseTime uses `break` after any per-client action that can delete the client, so after the skill-message flood kick or the speed-hack block later clients are not processed in that tick (S :3853-4132; shared with character.md) ([COMBAT-004](../issues/combat.md#combat-004-checkclientresponsetime-breaks-out-of-the-client-loop-after-a-deletion)).
- [V] Fightzone resurrection after 30 s is commented out (S :4081-4090) ([COMBAT-005](../issues/combat.md#combat-005-fightzone-resurrection-after-30-s-is-commented-out)).

## 10. Open questions
- Closed: DEF_OBJECTMOTION_ATTACK_CONFIRM and plain CONFIRM both only decrement `m_cCommandCount` on the client
  (C Game.cpp:35515-35523) [V].
- Attack-rate limits [V]: 7 attack messages must span at least 80*8*7-3000 = 1480 ms or the client is deleted
  (S :11304-11317); bCheckClientAttackFrequency deletes on a client-clock gap under 500 ms (S :52627).
  Whether these match the animation timing of each weapon speed is still open (DEF_WEAPONSPEEDLIMIT is
  commented out, S GlobalDef.h:1) [U].
- The fightzone "time input" branch (S :19266) was not traced into the arena system (social cluster).

## 11. Doc discrepancies
- `mechanics.md` section 4 says "Near-attack mode halves it". The halving verified here is the
  **safe-attack** halving (S :12612-12619); `bNearAttack` is a separate parameter not re-checked [V/U].
- `mechanics.md` 6 calls the protection "DEF_PKPENALTY (off)"; confirmed off - it is defined only under
  DEF_CONSULTATION, which only DEF_KOREA defines (S GlobalDef.h:26-29, :82-87) [V]. No discrepancy, cause added.

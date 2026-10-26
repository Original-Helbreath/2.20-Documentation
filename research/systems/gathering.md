# System: gathering (fishing, mining, farming)

*Three sibling subsystems sharing one pattern (map points -> dynamic object or crop NPC -> skill roll ->
item dropped on the floor); kept as one doc with a Contents block. Written 2026-10-25 by the world-cluster
agent.*

Paths and tags as in `map.md`: `SRV` = `sources/hgserver-librehelbreath/src/`, `CLI` = `sources/client-2.20/`,
lines as of 2026-10-25; [V] read in source (or in the reviewed function map named beside it), [I] inferred,
[U] unverified. "items" = `research/game-server/functions/items.md`.

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flows -
7. Rules and invariants - 8. Couplings - 9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose

Peaceful ways to earn items, experience and skill: fish in water (quick casts, or a "big fish" mini-game
where the catch chance rises and falls), mine ore and gems from veins that appear at fixed points, and since
v2.19 plant seeds on farmland and hoe the crop until it yields. All three feed crafting and the economy
(fish and ore sell, ore feeds manufacturing, crops feed potions) [I]. Mining exists in 2.20 (the brief asked
"if any": yes).

## 2. Subsystems

| Subsystem | Intent |
|---|---|
| 2.1 Skill-use pipeline | using a tool item starts a timed skill use that any action cancels (shared by fishing; also taming) |
| 2.2 Simple fishing | cast at water, 1d105 vs Fishing skill, a common fish (item 100) drops at your feet |
| 2.3 Event fishing | a fish shadow (DO) nearby turns the cast into a chance mini-game; pull when you like |
| 2.4 Fish population | fish shadows spawned at map `fish-point`s up to `max-fish`, expiring after 10-30 min |
| 2.5 Mining | pickaxe swing at a vein: Mining skill minus vein difficulty vs 1d100; veins deplete |
| 2.6 Mineral population | veins spawned at `mineral-point`s up to `max-mineral`, type up to the map's generator level |
| 2.7 Farming: planting | seed item on a farm tile creates a crop NPC (type 64) |
| 2.8 Farming: harvesting | hoe hits advance growth stages; drops by skill-vs-crop table |
| 2.9 Crop population cap | at most 200 crops per map, crops wither after 5 min |

## 3. Functions

| Subsystem | Function | Where (server unless noted) | Role |
|---|---|---|---|
| 2.1 | item use with `m_sRelatedSkill` -> `bRegisterDelayEvent(DEF_DELAYEVENTTYPE_USEITEM_SKILL)` | SRV Game.cpp:31060-31085 [V] | tool life -1, skill busy flag, mastery captured now |
| 2.1 | `DelayEventProcessor` case USEITEM_SKILL | SRV Game.cpp:33045-33068 [V] | still busy and same use id -> `iCalculateUseSkillItemEffect`, `DEF_NOTIFY_SKILLUSINGEND` |
| 2.2/2.3 | `iCalculateUseSkillItemEffect` | SRV Game.cpp:33700-33799 [V] | 1d105 roll, water test, GET (fish/meat) or TAMING |
| 2.3 | `iCheckFish` | SRV Game.cpp:36901-36945 [V] | fish DO within 2 tiles -> allocate, chance 1, `DEF_NOTIFY_EVENTFISHMODE` |
| 2.3 | `FishProcessor` | SRV Game.cpp:36947-36986 [V] | every 4 s (mechanics.md) nudge the chance by skill - difficulty |
| 2.3 | `ReqGetFishThisTimeHandler` | SRV Game.cpp:37201-37252 [V] | pull: 1d100 <= chance |
| 2.4 | `FishGenerator`, `iCreateFish`, `bDeleteFish` | SRV Game.cpp:37255-37357, 36810, 36861 [V] | spawn / remove fish, cancel anglers |
| 2.4 | `AdminOrder_CreateFish` | SRV Game.cpp:36990 [V chat-gm map] | `/createfish` |
| 2.5 | `iClientMotion_Attack_Handler` -> `_CheckMiningAction` | SRV Game.cpp:11402, 38880-39121 [V] | swing on a vein |
| 2.6 | `MineralGenerator`, `iCreateMineral`, `bDeleteMineral` | SRV Game.cpp:38796, 38817-38877, 39124-39151 [V] | vein lifecycle; vein blocks walking |
| 2.7 | `_bDepleteDestTypeItemUseEffect` case `DEF_ITEMEFFECTTYPE_ITEMFARM` -> `__bSetAgricultureItem` | SRV Game.cpp:39476-39485, 52945-53058 [V] | plant |
| 2.8 | `iCalculateAttackEffect` crop branch, `bProbabilityTable`, `bCropsItemDrop` | SRV Game.cpp:12780-12880, 53189-53215, 53065-53138 [V] | harvest |
| 2.9 | `CMap::bAddCropsTotalSum`, `bRemoveCropsTotalSum`; `NpcProcess` crop expiry | SRV Map.cpp:773-796; Game.cpp:10516-10532 [V / V npc] | cap and withering |
| client | `DrawDialogBox_Fishing`, notify handlers for EVENTFISHMODE / FISHCHANCE / FISHSUCCESS / FISHFAIL / FISHCANCELED | CLI (ui-dialogs.md) [V name per map] | the fishing mini-game dialog |

## 4. Commands and messages

From by-system.md (gathering row) [V per doc]: client `DEF_COMMONTYPE_REQ_GETFISHTHISTIME` (pull; dispatched
at SRV Game.cpp:15487) [V]; notifies `DEF_NOTIFY_EVENTFISHMODE`, `FISHCHANCE`, `FISHSUCCESS`, `FISHFAIL`,
`FISHCANCELED`, `NOMOREAGRICULTURE`, `AGRICULTURESKILLLIMIT`, `AGRICULTURENOAREA`, `SKILLUSINGEND` (skills);
item use `DEF_COMMONTYPE_REQ_USEITEM` (item cluster); mining and harvesting ride `MSGID_COMMAND_MOTION`
`DEF_OBJECTATTACK`; results as `MSGID_EVENT_COMMON` `DEF_COMMONTYPE_ITEMDROP`; veins and fish as
`MSGID_DYNAMICOBJECT`. GM `/createfish <x> <y> <item>`.

## 5. Data

- Skills: mastery index 0 = mining, 1 = fishing, 2 = farming (used at 38914, 36960, 52960/12787) [V].
- `CFish` (`m_pFish[200]`): item, difficulty, DO handle, `m_sEngagingCount` (max 30 anglers,
  `DEF_MAXENGAGINGFISH`, Game.h:97). `CMineral` (`m_pMineral[200]`): type 1-6, difficulty, remain, DO handle.
- CClient: `m_iAllocatedFish`, `m_iFishChance`, `m_bSkillUsingStatus[60]`, `m_iSkillUsingTimeID[60]`.
- CNpc crop fields: `m_iNpcCrops` (1 watermelon, 2 pumpkin, 3 garlic, 4 barley), `m_iCropsSkillLV`,
  `m_iBuildCount` (growth counter), appearance `m_sAppr2 = crop << 8 | stage`.
- CMap: fish points / `m_iMaxFish` / `m_iCurFish`; mineral points / `m_iMaxMineral` / `m_iCurMineral` /
  `m_bMineralGenerator` / `m_cMineralGeneratorLevel`; `m_iTotalAgriculture`; tile `m_bIsWater`,
  `m_bIsFarmingAllowed`.
- Config: map info `fish-point`, `max-fish`, `mineral-generator`, `mineral-point`, `max-mineral`
  (mapdata-txt.md); `.amd` farm flag 0x20; Item.cfg tool items (rod `m_sRelatedSkill`, pickaxe weapon type 25,
  hoe weapon type 27, seeds effect type ITEMFARM with value1 = crop, value2 = crop level); `GlobalDef.h:3`
  `DEF_MAXAGRICULTURE 200` [V].

## 6. Flows

### 6.1 Fishing
1. Player uses a rod on a water tile: tool life -1, delay event registered for the skill's `m_sValue2`
   seconds, skill busy (31060-31085) [V]. Moving cancels it (`ClearSkillUsingStatus` in the move handler,
   movement.md) [V].
2. Event fires: still busy and same use id -> `iCalculateUseSkillItemEffect` (33045-33068) [V].
3. Roll: fail unless mastery > 1d105 (33726-33727); target must be water (33730); skill counter +1 (33734) [V].
4. Fish shadow within 2 tiles and not over-subscribed -> event mode: allocate, chance 1, notify
   `EVENTFISHMODE` with price/2, sprite, name (36901-36945) [V]. Otherwise item 100 (fish) dropped on the
   angler's tile, `FISHSUCCESS`, exp 2d5 (33758-33780) [V].
5. Event mode: `FishProcessor` raises the chance by 1d(max(1,(skill-diff)/10)) when (skill - diff) > 1d100,
   else (roll above the skill; an equal roll changes nothing, `break` at 36956) lowers it, clamped 1..99, notifying `FISHCHANCE` (36947-36986) [V].
6. Player pulls (`REQ_GETFISHTHISTIME`): success if chance >= 1d100 -> the fish item drops on the tile, exp
   `GetExp(iDice(difficulty,6))`, fishing skill +difficulty, `FISHSUCCESS`, fish deleted and all other
   anglers get `FISHCANCELED`; else `FISHFAIL` (37201-37252, items map) [V].

### 6.2 Mining
Pickaxe attack on a tile: the attack handler calls `_CheckMiningAction` when the target is not a creature
(11402) [V]. It breaks invisibility, requires weapon appearance type 25 and the weapon drawn
(`m_sAppr2 & 0xF000`), mining skill > 0; success if 1d100 <= skill - difficulty (floor 1); skill +1; yields
by vein type; remain -1, depleted veins removed and the tile unblocked (38880-38925, 39124-39151) [V].

### 6.3 Farming
1. Plant: seed used on a target tile -> `__bSetAgricultureItem` (39476-39480): refuse if the map has 200
   crops (`NOMOREAGRICULTURE`), if the seed level > farming skill (`AGRICULTURESKILLLIMIT`), if a stationary
   NPC stands there or the tile lacks the farm flag (`AGRICULTURENOAREA`); create NPC "Crops" with a
   naming value and store crop type and level (52954-53045) [V].
2. Grow / harvest: hoe (weapon type 27) attacks on a crop with farming skill >= 20 and character level >= 20
   (12782-12789). Each hit that passes the skill-vs-level table (or lands on a stage boundary) costs hoe
   life and decrements `m_iBuildCount` (12864-12879); at counts 18 and 8 the crop changes stage and may drop
   produce, at 1 the drop is probabilistic (`bProbabilityTable`, zero when the skill tier is below the crop tier) while the crop is always deleted afterwards (12806-12810), so a crop can be removed without producing anything; skill gain applies only when skill <= crop level + 10 (12800) [V]. Produce items 820-823
   with exp 3d10 or 4d10 (53065-53100) [V].
3. Unharvested crops are deleted after `DEF_PLANTTIME` 5 min (npc NpcProcess) [V npc].

## 7. Rules and invariants

- Fishing base roll: mastery must exceed 1d105, so even 100 % skill fails sometimes (comment 33725) [V].
- Event fish chance clamped 1..99 (36973-36980) [V]; max 30 anglers per fish (Game.h:97) [V].
- Fish spawn: per map 1 in 4 per generator pass, point +-1 jitter, eight fish species with difficulty ranges plus a rare-item case (13 of 150 rolls give an item; the other rolls leave the item id 0 and the difficulty unset, so that pass presumably creates nothing [I]),
  lifetime 10, 20 or 30 min (37255-37357) [V].
- Veins: `MineralGenerator` runs on the 10 s sector-info timer (S :3811-3816); per map 1 in 6 per pass (38802); veins never expire (no lifetime is set in iCreateMineral); type 1d(level); difficulty/remain 10/20, 15/15, 20/10, 50/8,
  70/6, 90/4 (38861-38866) [V]. Veins block movement while present (tile.md) [V].
- Farming: 200 crops per map; skill and level 20 to harvest; seed level <= farming mastery [V].
- Crop tables: `SKILLCROPSTABLE` / `CROPSDROPTABLE` 9x9 by (skill-20)/10 and (crop-20)/10; outside 20-100
  -> 0 (53141-53215) [V].

## 8. Couplings

Out: tile (water/farm flags, DOs, item drops), entity (crops are NPCs: `bCreateNewNpc`, `DeleteNpc`,
naming values; `NpcProcess` expiry), skills (`CalculateSSN_SkillIndex`, mastery indices 0/1/2, skill busy
flags, `SKILLUSINGEND`), character (`GetExp`), items (`_bInitItemAttr`, tool lifespan,
`ReleaseItemHandler` when the hoe breaks), magic (mining breaks invisibility), combat (mining and harvesting
are branches inside the attack path), map (fish/mineral points and caps from map info), delay events
(infrastructure). In: movement (any step cancels a pending cast), combat (`iClientMotion_Attack_Handler`,
`iCalculateAttackEffect`), gm (`/createfish`), crafting (consumes produce, ore) [I].

## 9. Current behaviour vs intent

- `FishProcessor` uses `break` when an angler's fish is gone (36956), ending the pass for every later angler; `FishGenerator` (37268, 37274) and `MineralGenerator` (38807) `break` on a bad point or allocation failure, ending the pass for every later map ([GATHERING-001](../issues/gathering.md#gathering-001-generator-and-processor-loops-use-break-where-continue-was-meant)).
- `CMap::bRemoveCropsTotalSum` only decrements while the count is below 200 (Map.cpp:786); a map that reaches 200 crops keeps farming blocked until restart ([GATHERING-002](../issues/gathering.md#gathering-002-crop-count-can-never-fall-once-a-map-reaches-200)).
- Planting checks the farm flag at the clicked tile, but `bCreateNewNpc` places the crop with `bGetEmptyPosition`, which may use a neighbouring non-farm tile (10160) ([GATHERING-003](../issues/gathering.md#gathering-003-planted-crop-may-land-on-a-non-farm-neighbouring-tile)).
- Hoe-life code tests `(iTemp != -1) || (pItem != NULL)` (12864); a hoe must be equipped to reach it ([GATHERING-004](../issues/gathering.md#gathering-004-hoe-life-test-uses--where--was-meant)).
- The planting function keeps a commented "crusade builder only" guard copied from the construction-kit code (52952-52953) ([GATHERING-005](../issues/gathering.md#gathering-005-planting-keeps-a-commented-crusade-builder-guard-copied-from-construction-code)).
- Simple fishing's GET skill value 1 (meat, item 99) also requires a water tile (33730 precedes the switch) ([GATHERING-006](../issues/gathering.md#gathering-006-meat-gathering-get-value-1-requires-water-and-could-never-work-on-land)).
- Mining's yield code and items.md list `DEF_NOTIFY_ITEMLIFESPANEND` for `bDeleteMineral`; the body sends no notify (see 11).

## 10. Open questions

- Which skill config entries use `DEF_SKILLEFFECTTYPE_GET` value 1 (meat) in shipped Skill.cfg [U].
- Exact mining yield table per vein type (38925-39100, not transcribed) [U].
- `FishProcessor` call period: mechanics.md says 4 s; the `OnTimer` call site was not re-read [U].

## 11. Doc discrepancies

- `research/game-server/functions/items.md` `bDeleteMineral`: lists `DEF_NOTIFY_ITEMLIFESPANEND` and Korean
  comments about armour lifespan. The body (SRV Game.cpp:39124-39151) only broadcasts the DO removal,
  clears the tile, unblocks it and decrements the count; the notify and comments are a token-scan merge from
  a neighbouring function [V].

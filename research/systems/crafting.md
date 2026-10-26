# System: crafting (alchemy, manufacturing, item upgrade)

Sources: server `sources/hgserver-librehelbreath/src/` (S), client `sources/client-2.20/` (C). Tags:
[V] read in source, [I] inferred, [U] unverified. Item model and attribute bits: `item.md`.

## 1. Purpose

Crafting turns gathered materials into better goods and gives non-combat characters a progression path.
Three crafts exist in 2.20:
- **Alchemy** (skill 12): put up to six ingredients in the alchemy pot, get a potion if the combination
  matches a recipe in Potion.cfg and the skill roll succeeds.
- **Manufacturing** (skill 13): smelt ores into ingots and forge weapons/armour from ingots per
  BuildItem.cfg. Material purity sets the product's quality (+/- %) and durability, and the product is
  flagged custom-made (bit 0), which lifts its level limit and boosts later upgrades.
- **Item upgrade**: raise a weapon/shield/armour's +level (attribute bits 28-31) with Stones of Xelima
  (weapons) or Merien (armour) at falling odds, where failure above +0 destroys the item; or, for
  "majestic" (Gizon) hero items, spend upgrade points earned past max level for a guaranteed step.
Materials come from gathering (mining, fishing, farming) and monster drops, owned elsewhere [V, below].

## 2. Subsystems

| Sub | Intent | Entry |
|---|---|---|
| A Recipe tables | Data-driven recipes pushed from the log server | `_bDecodePortionConfigFileContents` S 38608, `_bDecodeBuildItemConfigFileContents` 43156 |
| B Alchemy | Ingredient set -> potion, skill-gated, failure costs nothing | `ReqCreatePortionHandler` 38261-38604 |
| C Manufacturing | Recipe name + six slots -> item, quality from purity | `BuildItemHandler` 43525-43943 |
| C1 Purity and quality | Material purity (spec value 2) capped by skill; product quality % and life | 43628-43745 |
| D Stone upgrade | +level with Xelima (656) / Merien (657) stones, destroy on failure above +0 | `RequestItemUpgradeHandler` 51812-52438, `bCheckIsItemUpgradeSuccess` 52441-52485 |
| E Majestic (Gizon) upgrade | Hero items upgraded with points from exp past max level; owner-bound | 51855-51989, 51990-52025, 52294+; points granted in `bCheckLevelUp` 27218-27229 |
| F Client crafting UI | Pot/anvil dialogs, recipe browser, upgrade dialog | C `DrawDialogBox_SkillDlg` 41233, `_bCheckCurrentBuildItemStatus` 24410, `DrawDialogBox_ItemUpgrade` 47539 |
| (feeder) Gathering | Ores/gems (mining), fish, crops supply materials | `_CheckMiningAction` 38880, fishing 36810-37357, farming 52945-53215 - see item map and mechanics.md section 9 |

## 3. Functions

| Sub | Function | Prog | file:line | Role |
|---|---|---|---|---|
| A | `_bDecodePortionConfigFileContents` | GS | S 38608-38734 | potion: name, 12 shorts (6 id/count pairs), skill limit, difficulty (38652-38701) [V] |
| A | `_bDecodeBuildItemConfigFileContents` | GS | S 43156-43523 | build item: name, skill limit, 6 x (id, count, value), average value, max skill, attribute; max value = sum(value*100) (43475) [V] |
| B | `ReqCreatePortionHandler` | GS | S 38261-38604 | validate, sort, match, roll, consume, create [V] |
| C | `BuildItemHandler` | GS | S 43525-43943 (live body to 43785; 43787-43942 is a commented-out old version) | roll, match, purity, create, consume, skill/exp [V] |
| D | `RequestItemUpgradeHandler` | GS | S 51812-52438 | per category: blocked ids, stones, Gizon path [V] |
| D | `bCheckIsItemUpgradeSuccess` | GS | S 52441-52485 | odds table, crafted bonus, bonus stone x2, logs UPGRADESUCCESS/FAIL [V] |
| E | `bCheckLevelUp` | GS | S 27210-27229 | at max level, exp overflow -> +1 Gizon point (cap 37) [V] |
| - | `CalculateSSN_SkillIndex`, `GetExp` | GS | skills cluster | skill growth and small exp [V map] |
| F | `_bDecodeBuildItemContents`, `__bDecodeBuildItemContents` | C | C 24271-24407 | client recipe list [V client map] |
| F | `_bCheckCurrentBuildItemStatus` | C | C 24410-24582 | which recipes the bag can make [V client map] |
| F | `bItemDrop_SkillDialog`, `bDlgBoxPress_SkillDlg` | C | C (ui-dialogs.md) 23592 | fill pot/anvil slots [V client map] |
| F | `DrawDialogBox_ItemUpgrade`, `DlgBoxClick_ItemUpgrade`, `bItemDrop_ItemUpgrade` | C | C 47539, 20580, 42134 | upgrade dialog, sends UPGRADEITEM [V client map] |

## 4. Commands and messages

- Client -> GS COMMON: REQ_CREATEPOTION (S 15483; REQ_CREATEPORTION is the alias name in by-system.md),
  BUILDITEM (15447), UPGRADEITEM (15390) [V]. Payload for potion/build is read raw from `pData + 11`
  (38281, 43539) [V].
- GS -> client: NOMATCHINGPORTION, LOWPORTIONSKILL, PORTIONFAIL, PORTIONSUCCESS, BUILDITEMSUCCESS (quality,
  item type), BUILDITEMFAIL, ITEMATTRIBUTECHANGE, ITEMUPGRADEFAIL (1 maxed, 2 not upgradable, 3 not
  enough points), GIZONITEMUPGRADELEFT, GIZONITEMCANGE, plus ITEMOBTAINED / CANNOTCARRYMOREITEM [V].
- Log server -> GS at boot: MSGID_PORTIONCONFIGURATIONCONTENTS 0x0FA314DE, MSGID_BUILDITEMCONFIGURATIONCONTENTS
  0x0FA40002 (`shared/messages/game-log.md`:21, 23) [V doc].
- Logs: UPGRADESUCCESS / UPGRADEFAIL (good items only); MAKE only under inactive DEF_TAIWANLOG (43755) [V].

## 5. Data

- `CPortion` (S Potion.h): `m_cName`, `m_sArray[12]` (id, count pairs sorted by id descending, -1/0
  padding), `m_iSkillLimit`, `m_iDifficulty`; `m_pPortionConfigList[500]` (S Game.h:98, 748) [V].
- `CBuildItem` (S BuildItem.h): name, item id, skill limit, material id/count/value x6, `m_iMaxValue`,
  `m_iAverageValue`, `m_iMaxSkill`, `m_wAttribute`; `m_pBuildItemList[300]` (Game.h:58, 785) [V].
- Item fields used: `m_sItemSpecEffectValue2` = material purity or product quality %,
  `m_sItemSpecEffectValue1` = crafted max life, `m_dwAttribute` bit 0 crafted, bits 16-31 recipe
  attribute, bits 28-31 upgrade level [V].
- `CClient::m_cSkillMastery[12]` alchemy, `[13]` manufacturing; `m_iGizonItemUpgradeLeft` (persisted,
  S 5892 / 6757); `m_sCharIDnum1-3` (owner check for Gizon items) [V].
- Config files: Potion.cfg ("potion" lines, 38713), BuildItem.cfg ("BuildItem" lines, 43501) [V].

## 6. Flows

**Alchemy.** Client fills the pot dialog -> REQ_CREATEPOTION with 6 bag indices -> S 38261: indices valid
(38297-38300) -> group duplicates into (index, count) (38302-38322) -> counts available (38325-38332) ->
bubble-sort by item id descending (38335-38352) -> 12-value array -> compare with every recipe
(38401-38414) -> none: NOMATCHINGPORTION -> skill < limit: LOWPORTIONSKILL -> roll 1d100 against
(skill - difficulty, min 1, +5 at skill 100) (38431-38447) -> fail: PORTIONFAIL, nothing consumed ->
success: skill +1 SSN (38460), ingredients consumed (38469-38476), PORTIONSUCCESS, exp 2d(difficulty/3)
(38480), potion added or dropped at feet (38482-38597) [V].

**Manufacturing.** Client picks a recipe and fills 6 slots -> BUILDITEM(name, 6 indices) -> S 43525:
compact slots (43562-43571) -> roll 1d100 <= skill 13 or BUILDITEMFAIL (43580-43588) -> slots must
exist (43591-43596) -> find recipe by name, skill >= limit (43599-43605) -> match each recipe material to
a distinct slot by id and count, summing purity*value with purity above skill reduced by half the excess
(43616-43644) -> quality % = total/max*100 (43654-43661) -> create item, set crafted bit (43664-43674) ->
material output (type 12): purity = skill/2 + 1d(skill/2+1)-1, unique touch id (43676-43685) -> gear:
recipe attribute into bits 16-31, quality = (q - avg)/(100 - avg)*100 or (q - avg)/avg*100, max life
scaled by quality and capped at 2x, colour 2 (43687-43745) -> `bAddItem`, BUILDITEMSUCCESS(quality,
type) -> consume materials (43761-43774) -> skill +1 only below the recipe's max skill (43777-43778) ->
exp 2d(limit/4) (43781) [V].

**Stone upgrade (weapon example, category 1).** Upgrade dialog -> UPGRADEITEM(bag index) -> S 51812:
level from bits 28-31, refuse at 15 (51824-51829) -> blocked legendary ids refuse (51845-51853) ->
Xelima present? roll in `bCheckIsItemUpgradeSuccess` -> fail: item destroyed if level >= 1, stone
consumed (52051-52059) -> success: level +1 up to 10 (crafted) or 7 (normal), stone consumed
(52062-52090) -> ITEMATTRIBUTECHANGE [V].

**Gizon upgrade.** Hero/majestic ids (e.g. 703, 709, 717, 718, 727, 736, 737) -> must be owner-bound to this
character (touch values = char id 1-3, 51871-51876) -> cost x(x+6)/8 + 2 points (51869) -> points
deducted, GIZONITEMUPGRADELEFT -> level +1 without a roll; at +0 some ids transform into new ids 736/737
(51889-51970) [V].

## 7. Rules and invariants

- Alchemy success = 1d100 <= max(1, skill12 - difficulty) (+5 at skill 100; the +5 is skipped on a `DEF_TESTSERVER`
  build where the skill is forced to 100, S 38432-38439); requires skill12 >= recipe
  limit (S 38423-38443). No location or tool check on the server [V].
- Alchemy and manufacturing failures consume nothing (38443-38446, 43583-43587) [V].
- Manufacturing roll = 1d100 <= skill13, done before validation (43580-43583) [V].
- Manufacturing skill grows only while below the recipe's max skill (43777) [V].
- Crafted gear life: base life * (1 + quality/100), applied only if <= 2x base (43722-43741) [V].
- Upgrade odds by current level 0..9: 20, 15, 9, 8, 7, 7, 5, 5, 3, 2 % (52449-52461); x2 with the bonus
  flag (Merien path passes TRUE, 52133, 52232); crafted items with quality > 100 add quality/20 (base 8-20 %)
  or /40 (base <= 7 %) (52464-52471) [V].
- Caps: crafted weapons +10, normal weapons +7, Gizon items +15, item 717 +7 (51832-51838, 52064-52080)
  [V]. At the cap a winning roll writes nothing and the stone is not consumed (the `ItemDepleteHandler` call sits in
  the not-at-cap branch, S 52062-52090) [V]. Gizon points cap at 37 (DEF_MAXGIZONPOINT, S Game.h:206) [V].
- Not upgradable: ancient-prefix (type 9) weapons (52032-52036), strong-prefix (type 8) shields/armour
  (52105-52109, 52213-52217), listed legendary ids [V].

## 8. Couplings

- item: `_bInitItemAttr`, `_bAddClientItemList`, `bAddItem`, `SetItemCount`, `ItemDepleteHandler` [V].
- skills: `m_cSkillMastery[12/13]`, `CalculateSSN_SkillIndex(…, 12|13, 1)` [V].
- character/exp: `GetExp` small rewards; Gizon points from `bCheckLevelUp` at max level (27218-27229) [V].
- combat: crafted quality (`m_sItemSpecEffectValue2`) and upgrade bits feed damage/defence in
  `CalcTotalItemEffect` / attack code (combat cluster) [I from field use].
- gathering/drops: material supply via `_CheckMiningAction`, fishing, `DeleteNpc` alchemy drops
  (S 30117-30290) [V map].
- logging: `_bItemLog` UPGRADE* [V]; persistence of `m_iGizonItemUpgradeLeft` [V].
- shop: crafted items sell by category like any item; no special price for quality [V by absence in
  ReqSellItemHandler].

## 9. Current behaviour vs intent

Defects are in [issues/crafting.md](../issues/crafting.md); the neutral lines are:

1. Manufacturing matches materials by `z` (43621-43637) but consumes by slot position `x` with the recipe's count for material `x` (43761-43772) ([CRAFTING-001](../issues/crafting.md#crafting-001-manufacturing-consumes-the-wrong-slots)).
2. The ingredient index bound is `> DEF_MAXITEMS` (43594), so index 50 passes ([CRAFTING-002](../issues/crafting.md#crafting-002-off-by-one-slot-bound-in-builditemhandler)).
3. `pItem->m_sItemSpecEffectValue2` is read after `bAddItem` (43752-43753); built items never stack, so it does not trigger in practice ([CRAFTING-003](../issues/crafting.md#crafting-003-crafted-item-read-after-badditem-may-have-deleted-it)).
4. The `if (iProb > 20)` crafted-upgrade bonus branch (52465) never runs because the base odds top out at 20 ([CRAFTING-004](../issues/crafting.md#crafting-004-crafted-upgrade-bonus-branch-is-unreachable)).
5. The manufacturing skill roll precedes validation, so an invalid request can still send BUILDITEMFAIL and a valid low-skill request can be retried at no cost ([CRAFTING-005](../issues/crafting.md#crafting-005-manufacturing-skill-roll-happens-before-the-request-is-validated)).
6. The alchemy recipe search has no break, so the last of two recipes with the same ingredient array wins ([CRAFTING-006](../issues/crafting.md#crafting-006-alchemy-recipe-search-has-no-break)).
7. [V] Upgrade failure at +0 keeps the item but still consumes the stone; above +0 it destroys the item - intended (v2.172 comment "fails from +1 to +2 -> disappears", 52056).
8. `BuildItemHandler` prints every crafted item to the server log window unconditionally ("testcode", 43747-43749) ([CRAFTING-007](../issues/crafting.md#crafting-007-builditemhandler-prints-every-crafted-item-to-the-server-log-window)).
9. The old manufacturing body (43787-43942) is commented out; it consumed materials before creating the item and used spec value 1 as purity ([CRAFTING-008](../issues/crafting.md#crafting-008-old-manufacturing-body-left-commented-out)).

## 10. Open questions

- Whether the client restricts alchemy/manufacturing to a pot/anvil location; the server does not [U].
- The full list of Gizon-upgradable ids per category (only category 1 was read in detail; categories 5,
  6, 15 follow the same shape at 52101-52438) [V shape, U full list].
- No official 2.20 Potion.cfg or BuildItem.cfg exists; both are pushed by the world log, not read from disk. Catalogued
  in `research/shared/formats/config-files.md`: Potion, chosen copy 3.51 `Potion.cfg` (19 recipes; the 2.03 `portion.cfg`,
  14 recipes in cp949 Korean, uses the keyword `portion` and so does not match the 2.20 parser test for `potion`, S 38713);
  BuildItem, chosen copy 2.03 (75 recipes, cp949 Korean names, 23 fields; the 3.51 copy has 97). Recipe balance was not
  analysed here [U].

# System: item (definitions, instances, inventory, equipment, use, floor, durability, drops)

*Verdict (2026-10-25): one cohesive doc with a Contents block, not split. Every subsystem shares the
same CItem instance model and the same inventory/weight helpers; readers come for the whole object
lifecycle. Shop, warehouse, trade and crafting are separate docs that build on this one.*

Sources: server `sources/hgserver-librehelbreath/src/` (S), client `sources/client-2.20/` (C). Server
line numbers equal the `SRV` numbers in `research/game-server/functions/items.md`. Tags: [V] read in
source, [I] inferred, [U] unverified. Active build switches (S GlobalDef.h:1-25): DEF_CHINESE, DEF_V219,
DEF_BEGINNERTOWNPROTECT; DEF_CHINESE implies DEF_DBGAMESERVER (binary player records) and also DEF_CHINESEGUILDLIMIT, DEF_NPCITEM and
DEF_CHINESECANDYEVENT (S GlobalDef.h:65-69, 73-75); DEF_NPCITEM selects which block of `bGetItemNameWhenDeleteNpc`
compiles (S Game.cpp:47190-47196).

## Contents
1. Purpose
2. Subsystems
3. Functions
4. Commands and messages
5. Data
6. Flows
7. Rules and invariants
8. Couplings
9. Current behaviour vs intent
10. Open questions
11. Doc discrepancies

## 1. Purpose

Items are the loot-and-gear economy of Helbreath: everything a character carries, wears, drinks, sells,
banks or trades is a `CItem` instance stamped from a central definition table that the operator ships as
`Item.cfg`. The player experiences a 50-slot bag limited by weight (Str and level), equipment slots that
change appearance and stats, consumables (potions, scrolls, food, skill manuals), weapons and armour that
wear out and must be repaired, and monster drops with random rare prefixes. The operator experiences a
data-driven catalogue, an item log of "good" items, and an anti-duplication purge (DupItemID.cfg) that
deletes known cloned items at login and refunds them as reward gold [V, sections below].

## 2. Subsystems

| # | Subsystem | Intent | Nests in / used by |
|---|---|---|---|
| A | Definition table | One authoritative catalogue (Item.cfg, up to 5000 ids) pushed by the log server so all game servers agree | everything below |
| B | Instance factory | `_bInitItemAttr` copies a definition into a new CItem; per-instance fields (count, life, colour, attribute bits, touch ids) diverge afterwards | A -> every creator (shop, drops, crafting, GM) |
| C | Inventory and weight | 50 slots, stackables merge by name, carry limit (Str+Level)*500, gold weighs 1/20 | B; used by shop, trade, warehouse, crafting |
| D | Equipment | Slot rules, level/gender/stat/weight limits, appearance bits, stat recomputation | C; feeds combat (CalcTotalItemEffect) |
| E | Use | Consumables by effect type: HP/MP/SP, scrolls (magic), skill/magic manuals, dye, flags, kits, seeds, show-map, skill tools | C; reaches magic, skills, crusade, farming |
| F | Floor (drop / pick-up) | Items lie on map tiles, 12 per tile, visible to nearby players | C, map tiles |
| G | Durability | Weapons wear on hits (more in rain), armour wears when struck, tools wear on use; 0 life unequips; repair at shops | D; repair in shop.md |
| H | Drop generation | Monster kill drops (gold, potions, rare gear with prefixes), map item events, corpse-removal NpcItem drops, PK death penalty drops | B, F; entity/combat own the triggers |
| I | Identity and anti-dupe | Touch-effect ids (random + date) on shop/crafted/drop items; DupItemID.cfg purge at login; retired item type -1 refunded | B, persistence |
| J | Item log | `_bItemLog` sends MSGID_GAMEITEMLOG for whitelisted "good" items and gold > 10000 | every subsystem; world log writes ItemLogData |
| K | Persistence | Inventory, equip flags, positions and bank serialised in the binary player record | C, warehouse; character/infrastructure own transport |

## 3. Functions

Full rows: `research/game-server/functions/items.md` (server), `research/client/functions/items.md` and
`ui-dialogs.md` (client). Only the role in the system is given here. Program: GS = game server.

| Sub | Function | Prog | file:line | Role |
|---|---|---|---|---|
| A | `_bDecodeItemConfigFileContents` | GS | S Game.cpp:9480-9854 | parse Item.cfg into `m_pItemConfigList[5000]`; negative price = not for sale (9688-9693) [V] |
| B | `_bInitItemAttr(pItem, name)` / `(pItem, id)` | GS | S Game.cpp:9857 / 9911 | linear name or id lookup; life starts at max (9881) [V] |
| C | `_bAddClientItemList` | GS | S Game.cpp:15904-15962 | weight check, merge stackables by name, else first free slot [V] |
| C | `iCalcTotalWeight` | GS | S Game.cpp:34605-34636 | recompute load; also picks the alter-item-drop (sacrifice stone) slot [V] |
| C | `_iCalcMaxLoad` | GS | S Game.cpp:28572-28577 | (Str*500 + Level*500) [V] |
| C | `iGetItemWeight` | GS | S Game.cpp:47173-47184 | weight*count, id 90 (Gold) /20, min 1 [V] |
| C | `dwGetItemCount` / `SetItemCount` (name, index) | GS | S Game.cpp:19178 / 19197 / 19230 | first stack count; set count, delete at 0 via ItemDepleteHandler, NOTIFY_SETITEMCOUNT [V] |
| C | `ItemDepleteHandler` | GS | S Game.cpp:30510-30548 | remove a used/sold item, NOTIFY_ITEMDEPLETED_ERASEITEM [V] |
| C | `_SetItemPos` | GS | S Game.cpp:40626-40653 | store client bag icon positions (UI only) [V map] |
| D | `bEquipItemHandler` | GS | S Game.cpp:15965-16403 | equip rules (section 7) [V] |
| D | `ReleaseItemHandler` | GS | S Game.cpp:19523-19674 | unequip, clear appearance bits [V map] |
| D | `CalcTotalItemEffect` | GS | S Game.cpp:35250 | recompute all equipment bonuses (combat cluster owns the formulas) [V map] |
| D | `CheckUniqueItemEquipment` | GS | S Game.cpp:40655-40686 | owner-bound items worn by others are stripped and deal 10d10 damage [V map] |
| E | `UseItemHandler` | GS | S Game.cpp:30626-31091 | dispatch by item type and effect type (30645-30650, 30657+) [V] |
| E | `_bDepleteDestTypeItemUseEffect` | GS | S Game.cpp:39410-39496 | targeted items: occupy flag, construction kit, dye, seed [V map] |
| F | `DropItemHandler` | GS | S Game.cpp:15597-15723 | drop all or part of a stack; split stacks are re-created from the definition [V] |
| F | `iClientMotion_GetItem_Handler` | GS | S Game.cpp:15725-15902 | pick up the top item of the player's own tile [V] |
| F | `CMap::bSetItem` / `CMap::pGetItem` | GS | S Map.cpp:315-334 / 338-366 | 12-item LIFO stack per tile [V] |
| G | weapon/armour wear in `iCalculateAttackEffect`, `CalculateAttackEffect` | GS | S Game.cpp:13377-13400, 14225-14250, 14435 | per-hit life loss (combat cluster) [V] |
| G | `ArmorLifeDecrement` | GS | S Game.cpp:52488-52593 | magic-hit armour wear, not same side [V map] |
| H | `NpcDeadItemGenerator` (+`WithItemEvent`, `bCheckInItemEventList`) | GS | S Game.cpp:44638 / 45634 / 45759 | kill drops (mechanics.md section 7) [V map] |
| H | `_AdjustRareItemValue` | GS | S Game.cpp:45951-45988 | apply prefix effects: agile -speed, light -weight, strong/ancient +max life [V] |
| H | `bGetItemNameWhenDeleteNpc` | GS | S Game.cpp:47187-47550 | NpcItem table; only 47193-47258 compiled [V map] |
| H | `_PenaltyItemDrop` | GS | S Game.cpp:28338 | death penalty drops, sacrifice stone protects [V map] |
| I | `_bDecodeDupItemIDFileContents` / `_bCheckDupItemID` | GS | S Game.cpp:45794 / 45932-45949 | load and match clone fingerprints (touch type + 3 values); sets refund price [V] |
| I | `bCheckAndConvertPlusWeaponItem` | GS | S Game.cpp:50878-50927 | migrate legacy "+1" names into attribute bits [V map] |
| J | `_bItemLog` (4 overloads, 2 active) / `_bCheckGoodItem` | GS | S Game.cpp:42704-42900 (active `#else` of DEF_TAIWANLOG at 42703) | whitelist filter then bSendMsgToLS(MSGID_GAMEITEMLOG) [V] |
| K | `_bDecodePlayerDatafileContents` / `_iComposePlayerDataFileContents` | GS | S Game.cpp:5551-6426 / 6428-6941 | inventory + bank in/out of the binary record [V] |
| - | `bItemDrop_*`, `_bCheckDraggingItemRelease`, `ItemEquipHandler`, `EraseItem`, `_iCalcTotalWeight`, `InitItemList` | C | C Game.cpp:4595, 41965-42208, 48407, 31988, 22921, 17689 | drag/drop UI, send commands, mirror server state [V client map] |

## 4. Commands and messages

From `research/systems/commands/by-system.md` (item row) and `shared/messages/`.

- Client -> GS (MSGID_COMMAND_COMMON subtypes): ITEMDROP, EQUIPITEM, RELEASEITEM, REQ_USEITEM,
  UPGRADEITEM (crafting), GIVEITEMTOCHAR (trade/warehouse); dispatch S Game.cpp:15511-15572 [V].
- Client -> GS motion: OBJECTGETITEM (pick-up) via ClientMotionHandler [V map].
- Client -> GS MSGIDs: MSGID_REQUEST_SETITEMPOS (S 15004), MSGID_REQUEST_RETRIEVEITEM (warehouse) [V].
- GS -> client notifies: ITEMOBTAINED, CANNOTCARRYMOREITEM, SETITEMCOUNT, ITEMDEPLETED_ERASEITEM,
  DROPITEMFIN_ERASEITEM, DROPITEMFIN_COUNTCHANGED, ITEMRELEASED, ITEMLIFESPANEND, ITEMCOLORCHANGE,
  ITEMPOSLIST, SHOWMAP [V map].
- GS -> nearby clients: MSGID_EVENT_COMMON / DEF_COMMONTYPE_ITEMDROP and SETITEM (floor sprite) [V].
- Log server -> GS at boot: MSGID_ITEMCONFIGURATIONCONTENTS, MSGID_DUPITEMIDFILECONTENTS,
  MSGID_NPCITEMCONFIGCONTENTS (the official world server never sends the last one,
  `research/servers/official-worldserver.md`:132) [V doc].
- GS -> log server: MSGID_GAMEITEMLOG (text, written to `ItemLogData\ItemEvents*.log`) [V doc + 42696].
- GM slash: `/createitem`, `/enableadmincreateitem 147258` (gm cluster) [V by-system].

## 5. Data

- `CItem` (S Item.h:69-115) [V]: definition fields copied by `_bInitItemAttr` (type, equip pos, effect
  type + 6 values, max life, sprite, price, weight, appearance, speed, level/gender limit, special
  effect values, related skill, category, for-sale flag, colour) and per-instance fields: `m_dwCount`,
  `m_wCurLifeSpan` (int), `m_sTouchEffectType` + 3 values (identity), `m_sItemSpecEffectValue1-3`
  (crafted life / purity or quality), `m_dwAttribute` (bit 0 crafted, 8-15 sub effect, 16-23 main
  effect, 28-31 upgrade level; `research/game-server/data-structures.md` CItem).
- Item types (S Item.h:32-45): -1 retired, 1 equip, 2 apply, 3 use-deplete, 4 install, 10 use-skill-enable-dialog-box
  (2, 4 and 10 are not usable through `UseItemHandler`, which passes only 3, 6, 7, 8, 9, 11, S 30645-30652 [V]), 5 consume (stackable), 6 arrow
  (stackable), 7 eat, 8 use-skill, 9 use-perm, 11 use-deplete-dest, 12 material. Only types 5 and 6
  stack (15916-15934) [V]; potions are not stackable [I from type list].
- Categories (used by shop/repair): 1-10 weapons, 11-13 clothes/armour, 14-42 goods, 43-50 tools
  (S Game.cpp:33948, 33967, 34472) [V].
- `CClient` (S Client.h:25-26, 120-124): `m_pItemList[50]`, `m_ItemPosList[50]`, `m_bIsItemEquipped[50]`,
  `m_pItemInBankList[120]`, `m_sItemEquipmentStatus[]`, `m_cArrowIndex`, `m_iCurWeightLoad`,
  `m_iAlterItemDropIndex`, `m_iRewardGold` [V].
- `CTile::m_pItem[12]` (S Tile.h:15, 29) [V]. Floor items are memory only: not saved, no expiry timer
  found [I: no timer reference to tile items].
- Config: `Item.cfg` (also Item2/Item3 on the official server), `DupItemID.cfg`, NpcItem config,
  map-file item events (S Game.cpp:26850-26925) [V]. Client: `contents\contents%d.txt` shop menus,
  ItemName.cfg [V client map].
- Persisted (binary record): per item name, count, touch type/values, colour, spec values 1-3, current
  life, attribute, plus bag position; bank list after a `m_bIsBankModified` byte (S Game.cpp:6855-6890) [V].

## 6. Flows

**Pick-up.** Client motion OBJECTGETITEM -> `iClientMotion_GetItem_Handler` S 15725: rejects if dead or
the position does not match the server's (15741) -> `CMap::pGetItem` pops the top item (Map.cpp:338) ->
`_bAddClientItemList` (weight, merge, slot) -> NOTIFY_ITEMOBTAINED to the picker and an EVENT_COMMON
SETITEM so neighbours redraw the next item on the tile -> `_bItemLog(GET)` for good items [V map +
15735-15770].

**Drop.** C `bItemDrop_ExternalScreen` (C 4654) sends COMMONTYPE_ITEMDROP (or GIVEITEMTOCHAR when a
character is under the cursor) -> S `DropItemHandler` 15597: name must match the slot (15620); partial
stack builds a fresh CItem from the definition (15624-15637) and reduces the slot; full drop releases
equipment -> `CMap::bSetItem` on the player's tile -> EVENT_COMMON ITEMDROP broadcast ->
NOTIFY_DROPITEMFIN_* -> `iCalcTotalWeight` [V].

**Equip.** C `ItemEquipHandler` (C 48407) -> COMMONTYPE_EQUIPITEM -> S `bEquipItemHandler` 15965 (rules in
section 7) -> releases conflicting slots -> sets appearance bits -> `CalcTotalItemEffect` ->
EVENT_MOTION NULLACTION so others see the new look [V map].

**Use.** COMMONTYPE_REQ_USEITEM -> S `UseItemHandler` 30626: only types 3, 9, 6, 7, 8, 11 accepted
(30645-30652); effect switch for 3/7 (30657+); magic scrolls call PlayerMagicHandler; manuals call
TrainSkillResponse / RequestStudyMagicHandler; 11 goes to `_bDepleteDestTypeItemUseEffect`; 8 registers
a delayed skill event (31079) -> ItemDepleteHandler -> NOTIFY_ITEMDEPLETED_ERASEITEM [V].

**Wear and break.** Hit lands -> attacker weapon life -1 (+1d2/1d3 in rain for blades) unless the
attacker is neutral (S 14228-14246); struck armour life -iDownValue unless neutral (13377-13398) -> at 0:
NOTIFY_ITEMLIFESPANEND + ReleaseItemHandler -> player repairs at a shop (shop.md) [V].

**Login.** World log server sends the player record -> S `_bDecodePlayerDatafileContents` 5551: each
item rebuilt by name (5953), rare values re-applied (6071), sacrifice stones at 0 life reset to 1,
type -1 items deleted and their price added to reward gold (6088-6094), DupItemID matches deleted,
logged and refunded (6097-6104); bank the same (6110-6245); refund lands in `m_iRewardGold` (6419) [V].

## 7. Rules and invariants

- Carry limit `(Str + Level) * 500` (S 28576); pick-up/buy/receive fails when current + new weight exceeds
  it (15911-15920) [V]. Gold weighs 1/20 per coin, every stack weighs at least 1 (47180-47181) [V].
- 50 bag slots (Client.h:25); stackables (types 5, 6) merge into the first same-name slot (15923-15936) [V].
- Equip (S 15965-16403) [V]: type must be equip; life > 0; level limit ignored for crafted items
  (attribute bit 0); gender limit by body type 1-3 male, 4-6 female (15984-15999); item weight must not
  exceed Str*100 (16003); body/legs/arms/head check stat limit EffectValue4 (10-15 = Str..Chr) against
  EffectValue5; one special-ability item at a time; two-hand vs hands and full-body conflicts.
- Neutral (traveller) characters' weapons and armour never wear (13389, 14242) [V].
- Floor: 12 items per tile; the 13th drop deletes the oldest (Map.cpp:325-326) [V]. Pick-up only from
  the player's own tile (15741) [V].
- Owner-bound items (touch type 1) worn by someone else are removed and deal 10d10 damage, checked from
  CheckClientResponseTime (40655) [V map].
- Rare prefix effects on load and on drop: agile speed -1, light weight -4%/step, strong/ancient max
  life +7%/step (45958-45985) [V].
- Item log filter: gold only above 10000, otherwise a hard-coded id whitelist (`_bCheckGoodItem`);
  RETRIEVE not logged unless forced (42711-42713) [V].

## 8. Couplings

Out of item (this system calls):
- combat/character: `CalcTotalItemEffect` (equipment stats), appearance bits `m_sAppr*` [V map].
- magic: `PlayerMagicHandler` from scrolls (UseItemHandler) [V map].
- skills: `TrainSkillResponse`, `CalculateSSN_SkillIndex`, delayed USEITEM_SKILL events (fishing,
  taming) [V map].
- crusade: occupy flags, construction kits via `_bDepleteDestTypeItemUseEffect` [V map].
- gathering/farming: seeds `__bSetAgricultureItem` [V map].
- logging/infrastructure: `bSendMsgToLS(MSGID_GAMEITEMLOG)` [V].
- map: `CMap::bSetItem`/`pGetItem`, `SendEventToNearClient_TypeB` [V].

Into item (other systems call):
- combat: weapon/armour wear and `ReleaseItemHandler` (S 12867, 13391, 14245, 52511) [V].
- entity/NPC death: `NpcDeadItemGenerator`, `DeleteNpc` -> `bGetItemNameWhenDeleteNpc` [V map].
- PK/death: `_PenaltyItemDrop` -> `DropItemHandler(bByPlayer FALSE)` [V map].
- session/persistence: `_bDecodePlayerDatafileContents`, `_iComposePlayerDataFileContents` [V].
- guild: items 88 (join form) and 89 (leave form) consumed in `GiveItemHandler` (trade.md) [V].
- quest/cityhall: `_iTalkToNpcResult_Cityhall` -> `_bCheckItemReceiveCondition`, `_bAddClientItemList`;
  `GetRewardMoneyHandler` pays `m_iRewardGold` [V map].
- shop, warehouse, trade, crafting: all mutate inventory through `_bAddClientItemList`, `SetItemCount`,
  `ItemDepleteHandler`, `iCalcTotalWeight`.

## 9. Current behaviour vs intent

Defects are in [issues/item.md](../issues/item.md); the neutral lines are:

1. The farming tool wear check uses `||` where `&&` was meant, so a player with no right-hand item reads `m_pItemList[-1]` when the crop roll passes ([ITEM-001](../issues/item.md#item-001-farming-tool-wear-null-check-uses-or-instead-of-and)).
2. Tile addresses use `m_pTile + sX + sY*m_sSizeY` everywhere; used consistently, it only aliases tiles on maps wider than tall ([ITEM-002](../issues/item.md#item-002-tile-address-multiplies-by-the-tile-height-instead-of-the-width)).
3. Lottery items (effect 23) roll but both outcomes are empty; the item is consumed for nothing ([ITEM-003](../issues/item.md#item-003-lottery-items-roll-but-both-branches-are-empty)).
4. Split stacks (drop, give, exchange) are re-created from the definition, so the original stack's touch id, colour and attribute do not travel with the split part ([ITEM-004](../issues/item.md#item-004-split-stacks-lose-instance-fields)).
5. The 13th item dropped on a full tile deletes the oldest one with no log ([ITEM-005](../issues/item.md#item-005-floor-overflow-deletes-the-oldest-item-with-no-log)).
6. The bank-load weight guard logs a copy-pasted "GemStone of Sacrifice" message and its `dTemp < 0` test on a DWORD never fires ([ITEM-006](../issues/item.md#item-006-bank-load-weight-guard-has-a-copy-pasted-message-and-a-dead-check)).
7. [V] DupItemID refund uses the price stored in DupItemID.cfg (`_bCheckDupItemID` 45944), so the operator decides compensation per cloned item, paid as reward gold at the cityhall (6101, 6419). Design.
8. The `#if DEF_TAIWANLOG` `_bItemLog` pair (42473-42702) is not compiled; the shipped pair filters to good items, so most drops and trades are never logged ([ITEM-007](../issues/item.md#item-007-dead-taiwanlog-item-log-pair-the-shipped-pair-logs-only-good-items)).

## 10. Open questions

- Exact Item.cfg column order in the shipped files and which ids are in `_bCheckGoodItem` beyond 616 -
  the list continues past what was read [U].
- Whether the client enforces any distance before ITEMDROP/give (server does not check distance for
  give) [U].
- Whether floor items are ever cleared (map reset, server restart only) [I: restart only].

## 11. Doc discrepancies

- `research/game-server/functions/items.md` RequestPurchaseItemHandler says "own-town/shop check"; the
  own-town test is commented out (S 16834-16835); only `m_bIsOnShop` is checked (16841) [V].
- `research/systems/commands/by-system.md` files warehouse under item; this analysis gives it its own
  doc (`warehouse.md`) per the cluster brief. Not an error, a different cut.

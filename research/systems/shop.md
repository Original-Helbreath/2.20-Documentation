# System: shop (NPC buy, sell, repair, pricing, city funds)

Sources: server `sources/hgserver-librehelbreath/src/` (S), client `sources/client-2.20/` (C). Tags:
[V] read in source, [I] inferred, [U] unverified. Item model, weight and inventory helpers: `item.md`.

## 1. Purpose

Shops are the gold faucet and sink between players and the world: players buy basic gear, potions and
tools, sell loot for gold, and pay blacksmiths/shopkeepers to restore worn equipment. Design levers
visible in the code: Charisma gives a discount, the side that won the last crusade pays 10 % less,
neutrals (travellers) sell for half of the normal sell price, and money paid in appears meant to fill the town
treasury (`m_stCityStatus[side].iFunds`; the only support is a v1.4 comment at S 17046) [I]. Since v2.18-2.19 shops only
work inside the shop interior maps, as an anti-hack measure [V, sections below].

## 2. Subsystems

| Sub | Intent | Notes |
|---|---|---|
| A Shop zone | Gate every money exchange to the shop interiors | `m_bIsOnShop` set in `iSetSide` from the map name (bsmith, gldhall, gshop) S Game.cpp:52910-52914 [V] |
| B Catalogue | Which items a shop offers | client-side only: `contents\contents%d.txt` loaded by C `_LoadShopMenuContents` (C Game.cpp:7939); server only checks the item's for-sale flag [V] |
| C Buy | Turn gold into items | `RequestPurchaseItemHandler` S 16820-17092 |
| D Sell (quote, confirm, list) | Turn items into gold, two-step so the player sees the price | `ReqSellItemHandler` 33917, `ReqSellItemConfirmHandler` 34103, `RequestSellItemListHandler` 46601 |
| E Repair (quote, confirm) | Gold sink tied to durability | `ReqRepairItemHandler` 34428, `ReqRepairItemCofirmHandler` 34507 |
| F Price modifiers | Charisma, crusade winner, neutral, rare-prefix value | inside C and D; winner flag also sent to the client as `cPrice` |
| G City funds | Town treasury fed by purchases, repairs, charged teleports | `m_stCityStatus[3].iFunds` (S Game.h:768-773) - write-only |

Related but owned elsewhere: magic shop (`RequestStudyMagicHandler`, magic cluster, needs `m_bIsOnTower`),
charged teleports (map cluster, S 24104-24110), cityhall reward gold (`GetRewardMoneyHandler`, quest/PK).

## 3. Functions

| Sub | Function | Prog | file:line | Role |
|---|---|---|---|---|
| A | `iSetSide` | GS | S Game.cpp:52857 (shop flag 52910-52914) | sets `m_bIsOnShop`, `m_bIsOnWarehouse`, `m_bIsOnTower` on every map entry [V] |
| A | `RequestInitDataHandler`, `InitPlayerData`, `RequestTeleportHandler` | GS | S 1938-1944 / 4949-4955 / 23540-23546 | compute `cPrice` (-10 for crusade winner in a shop) and send it to the client (2052, 23658) [V] |
| B | `_LoadShopMenuContents`, `__bDecodeContentsAndBuildItemForSaleList` | C | C Game.cpp:7939, 7973-8118 | read the shop menu file by NPC type [V] |
| B | `DrawDialogBox_Shop`, `DlgBoxClick_Shop` | C | C 40677, 42761 | shop window, sends REQ_PURCHASEITEM [V client map] |
| C | `RequestPurchaseItemHandler` | GS | S 16820-17092 | price, discount, gold check, add item, take gold, add funds [V] |
| D | `ReqSellItemHandler` | GS | S 33917-34101 | quote: NOTIFY_SELLITEMPRICE or CANNOTSELLITEM [V] |
| D | `ReqSellItemConfirmHandler` | GS | S 34103-34426 | sell: remove item, add gold (or drop it) [V] |
| D | `RequestSellItemListHandler` | GS | S 46601-46635 | up to 12 (index, amount) pairs sold through the confirm handler [V] |
| D | `DlgBoxClick_ItemSellorRepair`, `DlgBoxClick_SellList`, `bItemDrop_SellList` | C | C 23012, 20674, 42067 | single sell/repair dialog and the multi-sell list (MSGID_REQUEST_SELLITEMLIST) [V client map] |
| E | `ReqRepairItemHandler` | GS | S 34428-34505 | quote with repairer type check [V] |
| E | `ReqRepairItemCofirmHandler` | GS | S 34507-34603 | repair, take gold, add funds [V] |
| - | `dwGetItemCount`, `SetItemCount(name)`, `_bAddClientItemList`, `iCalcTotalWeight` | GS | see item.md | gold reads/writes and inventory [V] |

## 4. Commands and messages

- Client -> GS COMMON subtypes: REQ_PURCHASEITEM (S 15531), REQ_SELLITEM (15503), REQ_SELLITEMCONFIRM
  (15499), REQ_REPAIRITEM (15495), REQ_REPAIRITEMCONFIRM (15491); TALKTONPC opens the dialog flow (15479)
  [V]. REQ_LISTCONTENTS is dead (`systems/commands/overloaded.md`:66).
- Client -> GS MSGID: MSGID_REQUEST_SELLITEMLIST 0x2900AD30 (S 14970), same value as REQUEST_IPTIME on
  another link (`shared/messages/README.md`:32) [V doc].
- GS -> client: ITEMPURCHASED, NOTENOUGHGOLD, SELLITEMPRICE, CANNOTSELLITEM (codes 1 not sellable,
  2 broken, 4 too heavy), ITEMSOLD, REPAIRITEMPRICE, CANNOTREPAIRITEM (1 not repairable, 2 wrong NPC),
  ITEMREPAIRED, ITEMOBTAINED (gold), CANNOTCARRYMOREITEM [V].
- `cPrice` byte in the init-data and teleport responses tells the client the crusade-winner discount
  (S 2052, 23658) [V].

## 5. Data

- `CItem::m_wPrice` (DWORD), `m_bIsForSale` (from a non-negative Item.cfg price, S 9688-9693),
  `m_cCategory`, `m_wCurLifeSpan`/`m_wMaxLifeSpan`, `m_dwAttribute` prefix bits [V].
- `CClient::m_bIsOnShop`, `m_iCharisma`, `m_cSide` [V].
- `CGame::m_iCrusadeWinnerSide`, `m_stCityStatus[side].iFunds` (__int64, indexed by `m_cSide` 0..2; the boot reset at
  S 379-383 and 730-734 zeroes only [1] and [2], so the neutral slot [0] starts from static zero; never read, never
  persisted) [V by grep of `iFunds`, Game.h:768-773].
- `m_pGold` (template Gold item used for weight quotes, S 33960) [V].
- Client `contents\contents<type>.txt` shop menus [V].

## 6. Flows

**Buy.** NPC talk -> client opens the shop dialog with the menu file for that NPC type (C 18200) -> click
buy -> REQ_PURCHASEITEM(name, count) -> S 16820: must be `m_bIsOnShop` (16841); "10Arrows"/"100Arrows"
names map to Arrow x10/x100 (16851-16858); per unit: `_bInitItemAttr`, must be for sale (16874), stamp
touch id = random/random/MMDD (16884-16899); price with winner -10 % (16902-16914), Charisma discount
(16924-16934), half-price floor (16938-16940); gold check (16945); `_bAddClientItemList` -> 48-byte
ITEMPURCHASED -> `SetItemCount("Gold", gold - price)` (17042) -> `iFunds += price` (17047) [V].

**Sell.** Drag item to the sell dialog -> REQ_SELLITEM(slot, NPC type 15/24, count) -> S 33917 quote (no
shop-zone check, 33940-33946) -> NOTIFY_SELLITEMPRICE -> player confirms -> REQ_SELLITEMCONFIRM -> S 34103:
needs `m_bIsOnShop` (34124) -> recompute price -> NOTIFY_ITEMSOLD -> SetItemCount or ItemDepleteHandler
-> new Gold stack via `_bAddClientItemList`, else dropped at the player's feet (34398) [V]. Multi-sell:
MSGID_REQUEST_SELLITEMLIST -> 12 pairs -> confirm handler each, stopping at the first empty slot (46627)
[V].

**Repair.** Double-click equipment at a repairer -> REQ_REPAIRITEM(slot, NPC type) -> S 34428 quote with
NPC-type check -> NOTIFY_REPAIRITEMPRICE -> REQ_REPAIRITEMCONFIRM -> S 34507: `m_bIsOnShop` (34524),
recompute, gold check (34556), life = max (34582), NOTIFY_ITEMREPAIRED, take gold (34590),
`iFunds += price` (34596) [V].

## 7. Rules and invariants

- Buy price (DEF_V219 active) [V S 16902-16945]: base = price*count, 0.9x rounded when `m_iCrusadeWinnerSide == m_cSide` (see 9.10);
  discount% = (Cha - 10)/4 (integer); final price never at or
  below half of the undiscounted base: floor = base/2 - 1 after the `+1` adjustment.
- Sell price [V S 33948-33958, 34136-34253]: categories 11-50 sell at price/2 * count; categories 1-10 at
  (cur/max life) * 0.5 * price * count, plus rare-prefix value: main prefix multiplier (light 2, strong 2,
  agile 3, critical 4, sharp 5, poison 6, righteous 15, ancient 20) and sub-prefix multiplier (2/4/6)
  scaled by a value table (10 %...900 %), and only 2/3 of each prefix bonus is paid (34080, 34248).
  Neutrals get half of the already-halved price, i.e. a quarter of list for categories 11-50 (the code comment says
  "half of half", S 33947-33955). Clamp 1..1,000,000. Broken (life 0) weapons cannot be sold. Other categories:
  code 1 "cannot sell".
- Sell quote refuses when the gold would exceed carry weight (code 4, S 33960, 34087); the confirm does not
  re-check and drops the gold on the floor instead (34398) [V].
- Repair price [V S 34453-34497, 34536-34551]: price/2 at 0 life, else price/2 - (cur/max)*0.5*price.
  Quote: weapons (1-10) need NPC type 24 (blacksmith), clothes/tools (11-13, 43-50) NPC type 15
  (shopkeeper); other categories code 1. Confirm needs only the shop zone.
- Shop zone: bsmith, gldhall, gshop map prefixes (S 52910-52912) [V].

## 8. Couplings

- item: `_bInitItemAttr`, `_bAddClientItemList`, `SetItemCount`, `ItemDepleteHandler`, `dwGetItemCount`,
  `iCalcTotalWeight`, `_iCalcMaxLoad` [V].
- crusade: `m_iCrusadeWinnerSide` drives the 10 % discount (S 16903) [V].
- character: `m_iCharisma` discount; `m_cSide` (neutral half price, funds side) [V].
- map/session: `iSetSide` sets the shop flag on every map change (called from RequestInitDataHandler
  2081, InitPlayerData 4913, RequestTeleportHandler 23535/23690) [V].
- entity/NPC talk: NPC type ids 15 (shopkeeper) and 24 (blacksmith) arrive from the client as
  `cSellToWhom`/`cRepairWhom`; the server never checks that such an NPC is near [V].
- map (charged teleport) also credits `iFunds` (S 24110) [V].
- logging: `_bItemLog(SELL)` for good items (34258, 34286); BUY and REPAIR logs only under the inactive
  DEF_TAIWANLOG / misspelled `TAIWANLOG` (17089, 34585) [V].

## 9. Current behaviour vs intent

Defects are in [issues/shop.md](../issues/shop.md); the neutral lines are:

1. `iFunds` is incremented at S 17047, 24110, 34596 and zeroed at boot; nothing reads, spends or saves it ([SHOP-001](../issues/shop.md#shop-001-city-funds-are-write-only)).
2. The shop catalogue lives only on the client; the server sells any item with a non-negative Item.cfg price to anyone in a shop map ([SHOP-002](../issues/shop.md#shop-002-shop-catalogue-is-enforced-only-on-the-client)).
3. The charged purchase price goes through `WORD wTempPrice` (S 17030, 17042, 17047) while the gold check uses the full int, so purchases over 65535 gold are charged mod 65536 ([SHOP-003](../issues/shop.md#shop-003-purchase-price-truncated-to-16-bits)).
4. The sell quote uses `(int)d3` (S 33984) and the confirm `(short)d3` (34154), so a weapon whose half-price exceeds 32767 sells for 1 gold ([SHOP-004](../issues/shop.md#shop-004-sell-quote-and-confirm-use-different-integer-types-for-weapons)).
5. Repair uses `short sPrice` (S 34431, 34509); items priced above 65535 can never be repaired ([SHOP-005](../issues/shop.md#shop-005-repair-price-held-in-a-short)).
6. Only the repair quote checks the NPC type, and only the sell confirm checks the shop zone ([SHOP-006](../issues/shop.md#shop-006-repair-confirm-skips-the-npc-type-rule-sell-quote-skips-the-shop-zone-check)).
7. `ReqSellItemHandler` accepts any client-named NPC type 15 or 24 and never checks such an NPC is near (33942-33944) ([SHOP-007](../issues/shop.md#shop-007-sellrepair-npc-type-is-trusted-from-the-client)).
8. Charisma below 10 would produce a surcharge at 16924, but login rejects Charisma < 10 (S 6301); Cha 10-13 gives 0 % ([SHOP-008](../issues/shop.md#shop-008-negative-charisma-discount-path-is-unreachable)).
9. The BUY log call after `delete pItem` (17033-17090) is compiled out (DEF_TAIWANLOG off) ([SHOP-009](../issues/shop.md#shop-009-use-after-free-in-the-dead-buy-log-path)).
10. The crusade-winner discount test is `m_iCrusadeWinnerSide == m_cSide` (S 16903). The winner field is 0 at boot, after a draw and from the start of every crusade (`LocalStartCrusadeMode`, S 47918), and neutrals have `m_cSide` 0, so with no winner recorded neutrals get the 10 % discount (and `cPrice = -10` is shown) and during a running crusade the real winner's discount is off ([SHOP-010](../issues/shop.md#shop-010-crusade-winner-discount-matches-neutrals-and-misses-the-real-winner)).

## 10. Open questions

- What the official shop menu files `contents*.txt` contain and which NPC type maps to which file [U: not
  in the source trees read].
- Whether any later build read `iFunds` (e.g. for crusade structures or taxes) [U].

## 11. Doc discrepancies

- `research/game-server/functions/items.md` (RequestPurchaseItemHandler) says "own-town/shop check": the
  own-town check is commented out (S 16834-16835) [V].

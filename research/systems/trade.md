# System: trade (give item, two-way exchange)

Sources: server `sources/hgserver-librehelbreath/src/` (S), client `sources/client-2.20/` (C). Tags:
[V] read in source, [I] inferred, [U] unverified. Inventory helpers: `item.md`; warehouse deposits that
ride on the give command: `warehouse.md`.

## 1. Purpose

Two ways for players to move items between characters. **Give** is a one-way hand-over: drag an item
onto a character (or NPC) and it moves at once; it is also the transport for guild forms, warehouse
deposits and "drop at feet". **Exchange** is the safe, two-sided swap: both players see each other's
offer in a trade window and the swap happens only when both confirm, with weight and slot checks so
neither side can lose an item to a full bag. Exchange is one item (or one stack) per side [V].

## 2. Subsystems

| Sub | Intent | Entry |
|---|---|---|
| A Give | Instant hand-over; the server decides by the target what it means | `GiveItemHandler` S Game.cpp:17094-17690 |
| A1 Stack split | Give part of a stack (types 5, 6) by creating a new item | 17118-17346 |
| A2 Target resolution | Tile owner, cross-checked with the client-sent object id | 17141-17161, 17358-17378 |
| A3 Overloaded outcomes | guild join/leave forms, warehouse (Howard), guild officer (Kennedy), floor drop | `systems/commands/overloaded.md`:27 |
| B Exchange session | Pair two players, block re-entry, track names to detect handle reuse | `ExchangeItemHandler` 40826, `_ClearExchangeStatus` 41336 |
| C Offer | Choose or replace the offered item | `SetExchangeItem` 40919 |
| D Commit | Both confirmed -> validate -> swap -> log | `ConfirmExchangeItem` 40966-41194, `bAddItem` 41209 |
| E Cancel and teardown | Cancel button, death, teleport, disconnect all clear both sides | `CancelExchangeItem` 41325; callers of `_ClearExchangeStatus` |

## 3. Functions

| Sub | Function | Prog | file:line | Role |
|---|---|---|---|---|
| A | `GiveItemHandler` | GS | S 17094-17690 | all give outcomes [V] |
| A | `bItemDrop_ExternalScreen` | C | C Game.cpp:4654-4830 | drop on a character sends GIVEITEMTOCHAR, else ITEMDROP [V client map] |
| A | quantity dialog 17 (NPC type 1000 exchange, 1002 bank) | C | C 23870-23887, 42190-42199 | asks the amount for stacks [V] |
| B | `ExchangeItemHandler` | GS | S 40826-40915 | open: validate target, set both sides' state, NOTIFY_OPENEXCHANGEWINDOW to both with the opener's item [V] |
| C | `SetExchangeItem` | GS | S 40919-40963 | record/replace this side's item, NOTIFY_SETEXCHANGEITEM to both [V] |
| C | `bItemDrop_ExchangeDialog` | C | C 23858-23903 | client allows one set per window ("must reopen", 23865) [V] |
| D | `ConfirmExchangeItem` | GS | S 40966-41194 | confirm flag; when both: checks, swap, logs, NOTIFY_EXCHANGEITEMCOMPLETE [V] |
| D | `bAddItem` | GS | S 41209-41322 | add or drop at feet; always returns TRUE [V] |
| D | `_iGetItemSpaceLeft`, `bCopyItemContents` | GS | S 41198, 50614 | free slots; log copy [V] |
| E | `CancelExchangeItem`, `_ClearExchangeStatus` | GS | S 41325, 41336 | clear both / one side, NOTIFY_CANCELEXCHANGEITEM [V] |
| E | `DlgBoxClick_Exchange` | C | C 23905-23952 | confirm / cancel buttons [V client map] |
| E | `DlgBoxClick_NpcActionQuery` guard | C | C 42528-42532 | blocks NPC actions while the exchange window is open ("2002.10.31 anti-fraud") [V] |

## 4. Commands and messages

- Client -> GS COMMON: GIVEITEMTOCHAR (S 15545), EXCHANGEITEMTOCHAR (15550), SETEXCHANGEITEM (15463),
  CONFIRMEXCHANGEITEM (15459), CANCELEXCHANGEITEM (15455) [V].
- GS -> client: ITEMOBTAINED (receiver, 53 bytes), GIVEITEMFIN_COUNTCHANGED, GIVEITEMFIN_ERASEITEM,
  CANNOTGIVEITEM, CANNOTCARRYMOREITEM, DROPITEMFIN_ERASEITEM; QUERY_JOINGUILDREQPERMISSION and
  QUERY_DISMISSGUILDREQPERMISSION (guild); OPENEXCHANGEWINDOW, SETEXCHANGEITEM, EXCHANGEITEMCOMPLETE,
  CANCELEXCHANGEITEM [V].
- Near clients: EVENT_COMMON ITEMDROP when a give falls to the floor [V].
- Log: `_bItemLog(GIVE / DROP / EXCHANGE)` -> MSGID_GAMEITEMLOG for good items [V].

## 5. Data

`CClient` exchange state (S Client.h): `m_bIsExchangeMode`, `m_bIsExchangeConfirm`, `m_iExchangeH`
(partner handle), `m_cExchangeName` (partner name, guards against handle reuse),
`m_cExchangeItemIndex`, `m_iExchangeItemAmount`, `m_cExchangeItemName` [V uses at 40879-40893].
Nothing is persisted; a session ends on cancel, completion, death, teleport or disconnect [V].
Object ids on the wire: < 10000 player handle, >= 10000 NPC handle + 10000 (S 17144-17160) [V].

## 6. Flows

**Give to a player (whole item).** Drop on character -> GIVEITEMTOCHAR(slot, amount, dX, dY, objectId,
name) -> S 17094: name must match (17111) -> `ReleaseItemHandler` (17351) -> owner at (dX, dY) must equal
objectId (17361-17378) -> item 88 to a same-side guild master: join request, item consumed (17410-17427)
-> item 89 to own guild master outside crusade: leave request (17431-17447) -> else
`_bAddClientItemList(receiver)`: ITEMOBTAINED to receiver; on failure the item drops at the giver's feet
and the receiver gets CANNOTCARRYMOREITEM (17535-17566) -> giver gets GIVEITEMFIN_ERASEITEM (17672) ->
slot cleared (17681) -> weight recalculated [V].

**Give part of a stack.** Count > amount for types 5/6 -> new item from the definition with that count,
source reduced first (17123-17138) -> same target logic; failures drop at the giver's feet [V].

**Exchange.** EXCHANGEITEMTOCHAR(slot, amount, dX, dY, objectId) -> S 40826: target must be a player not
already trading, not in combat stance (`m_sAppr2 & 0xF000`) and not on a fight-zone map (40872-40873) ->
both enter exchange mode, opener's item recorded (40879-40893) -> OPENEXCHANGEWINDOW to both (40895-40907)
-> partner drags an item -> SETEXCHANGEITEM -> S 40919 records it, SETEXCHANGEITEM to both -> each clicks
confirm -> S 40966 sets own confirm; on the second confirm: names cross-check (40982-40984), items still
present and same names (40995-41008), weight left on both sides (41011-41032), at least one free slot on
both (41035), stack counts still sufficient (41048, 41079) -> `bAddItem` B->A and A->B (41107-41108) ->
logs (41113-41114) -> source slots reduced or cleared and unequipped (41122-41160) -> both states reset,
EXCHANGEITEMCOMPLETE to both (41163-41176) [V].

## 7. Rules and invariants

- Self-give is refused (17182) [V]. Give target must be the owner at the clicked tile; a mismatch
  between the client's object id and a live slot turns the give into a floor drop (an id naming an empty slot is not
  cross-checked and falls back to the tile owner) (17149, 17157, 17366, 17374) [V].
- Give to any NPC other than Howard/Kennedy drops the item at the giver's feet (17332-17343, 17649-17668);
  there is no NPC hand-in mechanic here [V].
- Kennedy + item 89 (outside crusade): immediate guild leave for non-masters, -300 exp floored at 0
  (17598-17624) [V].
- Exchange is blocked under DEF_GUILDWARMODE (40839-40842, switch off) [V].
- Exchange needs one free slot on each side and enough remaining weight for the incoming item
  (41011-41040); weight is computed before the outgoing item leaves, so it is conservative [V].
- Any teleport, death or disconnect cancels the session (`_ClearExchangeStatus` callers:
  RequestTeleportHandler, ClientKilledHandler, DeleteClient) [V items.md map].

## 8. Couplings

- item: `_bAddClientItemList`, `SetItemCount`, `ReleaseItemHandler`, `iCalcTotalWeight`, `_bInitItemAttr`,
  `CMap::bSetItem` [V].
- guild (social cluster): items 88/89 produce QUERY_JOIN/DISMISS notifies; Kennedy path edits
  `m_cGuildName`, `m_iGuildRank`, `m_iGuildGUID` directly (17611-17614) and broadcasts appearance [V].
- warehouse: `bSetItemToBankItem` (both overloads) from the Howard branches (17316, 17578) [V].
- character: exp loss on Kennedy leave (17620) [V].
- map/teleport and combat death: cancel exchange via `_ClearExchangeStatus` [V].
- combat stance read from `m_sAppr2` (40872) [V].
- logging: GIVE/DROP/EXCHANGE item logs [V].

## 9. Current behaviour vs intent

Defects are in [issues/trade.md](../issues/trade.md); the neutral lines are:

1. `SetExchangeItem` (40919-40963) replaces the offered item without clearing either side's confirm flag; only the client blocks a second set per window ([TRADE-001](../issues/trade.md#trade-001-exchange-offer-can-be-swapped-after-the-partner-confirms)).
2. Non-stack exchange overwrites `m_dwCount` with the client's amount (41066, 41097); amount 0 passes the bound ([TRADE-002](../issues/trade.md#trade-002-non-stack-exchange-overwrites-the-item-count-with-the-client-amount)).
3. 40835 returns when already in exchange mode, so the "hack" branch at 40845-40849 never runs ([TRADE-003](../issues/trade.md#trade-003-exchange-re-entry-guard-is-dead-code)).
4. `ConfirmExchangeItem` has an empty null check marked "BUG POINT" (41140-41143) followed by an unconditional dereference (41145) ([TRADE-004](../issues/trade.md#trade-004-confirmexchangeitem-has-an-empty-null-check-before-a-dereference)).
5. `GiveItemHandler` reads `m_pItemList[sItemIndex]` (17106) before the range check (17107) ([TRADE-005](../issues/trade.md#trade-005-giveitemhandler-reads-the-item-slot-before-the-range-check)).
6. A self-give of a partial stack reduces the source count (17134) before the self-give test returns (17182-17186) ([TRADE-006](../issues/trade.md#trade-006-self-give-of-a-partial-stack-loses-items)).
7. The item 88 join path jumps to REMOVE_ITEM_PROCEDURE (17426) and sets the slot to NULL without deleting the CItem; the form is consumed even if the master later rejects ([TRADE-007](../issues/trade.md#trade-007-guild-join-form-item-88-leaks-the-citem)).
8. Give and exchange accept any (dX, dY) on the target's map; the server shows no distance check ([TRADE-008](../issues/trade.md#trade-008-no-server-side-distance-check-for-give-and-exchange)).
9. The requester's own combat stance is not checked, only the partner's (40872) ([TRADE-009](../issues/trade.md#trade-009-exchange-checks-only-the-partners-combat-stance)).

## 10. Open questions

- Client-side range limit for give/exchange (the bank and exchange drops fake the position to avoid a
  "too far" message, C 42191, 23880) [U for the normal give path].
- Whether the official server build also lacked the confirm reset (commonly known exploit) [U].

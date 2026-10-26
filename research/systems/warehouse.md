# System: warehouse (bank storage)

Sources: server `sources/hgserver-librehelbreath/src/` (S), client `sources/client-2.20/` (C). Tags:
[V] read in source, [I] inferred, [U] unverified. Item model: `item.md`; the deposit command is the give
command described in `trade.md`.

## 1. Purpose

Per-character storage of up to 120 items beyond the 50-slot, weight-limited bag. It lets players keep
gear and gold safe from death drops and carry limits. The warehouse keeper NPC is **Howard**; depositing
is "giving" an item to Howard, withdrawing is a separate request by bank slot. Since v2.20 (2002-12-23)
both directions only work inside a warehouse interior map (anti-hack) [V].

## 2. Subsystems

| Sub | Intent | Entry |
|---|---|---|
| A Warehouse zone | Gate bank access to the warehouse interiors | `iSetSide` sets `m_bIsOnWarehouse` for map names wrhus, arewrhus, elvwrhus (S Game.cpp:52917-52924) |
| B Deposit | Move an inventory item (or part of a stack) to the first free bank slot | `GiveItemHandler` Howard branches (17314, 17576) -> `bSetItemToBankItem` (index 27858, pointer 32409) |
| C Withdraw | Move a bank slot back, merging stackables, respecting weight and bag slots | `RequestRetrieveItemHandler` S 27619-27856 |
| D Compaction | Bank has no holes: later entries shift down after a withdrawal | 27712-27718, 27789-27795 |
| E Persistence | Bank saved with the character; a dirty flag hints whether it changed | compose S 6869-6890, decode 6110-6245, `m_bIsBankModified` |
| F Client bank UI | Dialog 39, list, drag-in, click-out | C `DrawDialogBox_Bank` 37234, `bItemDrop_Bank` 42160, `DlgBoxClick_Bank` 42399, `RetrieveItemHandler` 31887 |

## 3. Functions

| Sub | Function | Prog | file:line | Role |
|---|---|---|---|---|
| A | `iSetSide` | GS | S 52857 (52917-52924) | warehouse flag on map entry [V] |
| B | `GiveItemHandler` (Howard) | GS | S 17314-17331, 17576-17597 | route a give to Howard into the bank; failure = CANNOTITEMTOBANK and the item drops at the player's feet [V] |
| B | `bSetItemToBankItem(iClientH, sItemIndex)` | GS | S 27858-27983 | whole item: pointer moved, weight recalculated, 56-byte NOTIFY_ITEMTOBANK [V] |
| B | `bSetItemToBankItem(iClientH, pItem)` | GS | S 32409-32529 | split part of a stack (new CItem) [V] |
| C | `RequestRetrieveItemHandler` | GS | S 27619-27856 | withdraw by bank index; MSGID_RESPONSE_RETRIEVEITEM confirm(bank idx, bag idx) or reject [V] |
| E | `_iComposePlayerDataFileContents` / `_bDecodePlayerDatafileContents` | GS | S 6428-6941 / 5551-6426 | bank list in the binary player record [V] |
| F | `bItemDrop_Bank` | C | C 42160-42208 | quantity dialog for stacks (type 1002), refuses at 119 items, sends GIVEITEMTOCHAR to the stored Howard id/position [V] |
| F | `DlgBoxClick_Bank`, `RetrieveItemHandler`, `_iGetBankItemCount` | C | C 42399, 31887-31986, 23967 | send MSGID_REQUEST_RETRIEVEITEM, apply the response [V client map] |

## 4. Commands and messages

- Deposit: COMMON GIVEITEMTOCHAR to the Howard NPC (S 15545) [V].
- Withdraw: MSGID_REQUEST_RETRIEVEITEM 0x0DF30751 (S 15019) -> MSGID_RESPONSE_RETRIEVEITEM 0x0DF30752
  with DEF_MSGTYPE_CONFIRM / REJECT (`shared/messages/client-game.md`:14-15) [V].
- Notifies: ITEMTOBANK, CANNOTITEMTOBANK, CANNOTCARRYMOREITEM [V].
- Log: `_bItemLog(RETRIEVE)` - suppressed by the good-item filter unless forced; forced only for item 650
  (S 27704-27707, 42711-42713); DEPOSIT log only under inactive DEF_TAIWANLOG (27961) [V].
- Persistence rides the normal save messages to the world log server (infrastructure cluster).

## 5. Data

- `CClient::m_pItemInBankList[DEF_MAXBANKITEMS = 120]` (S Client.h:26, 122) [V].
- `CClient::m_bIsOnWarehouse`, `m_bIsBankModified` (S Client.cpp:288) [V].
- Persisted per bank item: name, count, touch type/values, colour, spec values 1-3, current life,
  attribute (S 6879-6890, decode 6113-6190) [V]. The record carries `m_bIsBankModified` as one byte
  before the bank list (6870) [V].
- Client mirror `m_pBankList` (C, items.md overview) [V doc].

## 6. Flows

**Deposit.** Drag an item onto the bank dialog -> C 42160 (amount dialog for stacks) -> GIVEITEMTOCHAR
(slot, amount, Howard x/y, Howard id) -> S `GiveItemHandler`: owner at x/y must match the id -> NPC name
"Howard" (17314/17576) -> `bSetItemToBankItem`: must be `m_bIsOnWarehouse` (27873/32423); first empty
bank slot; pointer moved; NOTIFY_ITEMTOBANK(bank slot, item data) -> `m_bIsBankModified = TRUE`
(27966/32512) -> giver gets GIVEITEMFIN_ERASEITEM / COUNTCHANGED [V].

**Withdraw.** Click a bank row -> MSGID_REQUEST_RETRIEVEITEM(bank idx) -> S 27619: zone check (27633),
index range (27636), empty slot = REJECT (27637-27646); weight of the whole stack must fit (27659-27683,
CANNOTCARRYMOREITEM) -> stackable with a same-type same-name bag item: add count, delete bank item
(27686-27699) -> else first free bag slot, position reset to (40, 30) (27765-27776) -> compaction ->
CONFIRM(bank idx, bag idx) -> `iCalcTotalWeight`, arrow index -> `m_bIsBankModified = TRUE`; no free bag
slot = REJECT (27836-27843) [V].

## 7. Rules and invariants

- 120 server slots; the client refuses at 119 (C 42205 `_iGetBankItemCount() >= DEF_MAXBANKITEMS-1`) [V].
- Deposit never merges stacks: every deposit takes a new bank slot (27876-27880, 32426-32431) [V].
- Withdraw is whole-entry only (no partial withdraw), merging into the bag stack when one exists [V].
- Warehouse zone required for both directions since v2.20 [V].
- Login caps a bank stack to 1 if its weight alone exceeds the owner's max load (S 6135-6140) [V].

## 8. Couplings

- trade: deposit is a GiveItemHandler outcome keyed on the NPC name string "Howard" (17314, 17576) [V].
- item: weight (`iGetItemWeight`, `_iCalcMaxLoad`, `iCalcTotalWeight`), `SetItemCount`, arrow index [V].
- map: `iSetSide` map-name list for the zone flag [V].
- persistence/infrastructure: bank list and `m_bIsBankModified` in the player record; DupItemID purge and
  retired-item refund also run over the bank at login (6228-6245) [V].
- PK/death: banked items are outside `_PenaltyItemDrop`'s reach (it scans the bag) [I].

## 9. Current behaviour vs intent

Defects are in [issues/warehouse.md](../issues/warehouse.md); the neutral lines are:

1. Deposit is not a command of its own: it is the give command recognised by the NPC's name. Any NPC named "Howard" accepts deposits; the server checks the warehouse map flag but not distance to Howard ([WAREHOUSE-001](../issues/warehouse.md#warehouse-001-deposit-accepts-any-npc-named-howard-at-any-distance)).
2. [V] If the bank is full or the player is outside a warehouse map, a deposit falls through to "drop at feet" (17321, 17583): the failure mode puts the item on the floor rather than leaving it in the bag.
3. `m_bIsBankModified` ("to reduce DB load", v2.13 comment, 27744) is only ever set TRUE; the only FALSE assignment is the constructor, so after the first bank change in a session every save carries the flag ([WAREHOUSE-002](../issues/warehouse.md#warehouse-002-m_bisbankmodified-is-only-ever-set-never-reset)).
4. [V] Deposit failure on socket error returns TRUE on purpose: "returning FALSE would copy the item onto the floor" (27973-27975) - a dupe the developers had found and closed.
5. [V] Withdraw logs item 650 with force for "Sacrifice bug" tracking (v2.17 comment, 27701-27707).

## 10. Open questions

- How the world log server uses the `m_bIsBankModified` byte (skips writing the bank table?) [U:
  official world server has no source; see `research/servers/official-worldserver.md`].
- Whether warehouses are shared between characters of one account (the code is per character) [V per
  character; account sharing not present].

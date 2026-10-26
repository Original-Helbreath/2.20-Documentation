# Issues: trade

Scope: defects and divergences in item give and two-way exchange in the 2.20 game server. Split out of `research/systems/trade.md`.

### TRADE-001 Exchange offer can be swapped after the partner confirms
- **Program**: game server / client
- **System**: trade (exchange)
- **Kind**: client-only check / security
- **Intent**: a confirmation applies to the offer the partner saw.
- **Now**: `SetExchangeItem` (40919-40963) replaces the offered item but never clears either side's `m_bIsExchangeConfirm`. If B has confirmed, A can swap in a worse item and confirm; the trade commits with the new item. The client blocks a second set per window (C 23865, "for safety the window must be reopened") and NPC actions while trading (C 42528), so the protection is client-side only. [V] The open question of whether the official build had the same exploit is in the doc.
- **Where**: S Game.cpp:40919-40963; C Game.cpp:23865, 42528
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/trade.md:116

### TRADE-002 Non-stack exchange overwrites the item count with the client amount
- **Program**: game server
- **System**: trade (exchange)
- **Kind**: bug
- **Intent**: non-stack items trade as one unit.
- **Now**: non-stack exchange overwrites `m_dwCount` with the client's amount (41066, 41097); amounts are only bounded by `count < amount` at open/set (40834, 40937), so a non-stack item can be traded with count 0. A negative amount is rejected only because `iAmount` is compared as unsigned in `m_dwCount < iAmount`; amount 0 passes (S 40834). [V]
- **Where**: S Game.cpp:40834, 40937, 41066, 41097
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/trade.md:120

### TRADE-003 Exchange re-entry guard is dead code
- **Program**: game server
- **System**: trade (exchange)
- **Kind**: dead code
- **Intent**: a player already in exchange mode who requests another is treated as a hack and both sides cleared.
- **Now**: 40835 returns when already in exchange mode, so the "hack" branch at 40845-40849 never runs. [V]
- **Where**: S Game.cpp:40835, 40845-40849
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/trade.md:123

### TRADE-004 ConfirmExchangeItem has an empty null check before a dereference
- **Program**: game server
- **System**: trade (exchange)
- **Kind**: bug
- **Intent**: handle a null item pointer (marked "BUG POINT").
- **Now**: the null check (41140-41143) is empty and followed by an unconditional dereference (41145): a crash path the developers saw and did not close. [V]
- **Where**: S Game.cpp:41140-41145
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/trade.md:125

### TRADE-005 GiveItemHandler reads the item slot before the range check
- **Program**: game server
- **System**: trade (give)
- **Kind**: bug
- **Intent**: range-check the index, then read the slot.
- **Now**: `m_pItemList[sItemIndex] == NULL` (17106) is read before the range check (17107). [V]
- **Where**: S Game.cpp:17106-17107
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/trade.md:127

### TRADE-006 Self-give of a partial stack loses items
- **Program**: game server
- **System**: trade (give)
- **Kind**: bug
- **Intent**: self-give is refused without side effects.
- **Now**: the source count is reduced (17134) before the self-give test returns (17182-17186) after deleting the split item, so the split amount is lost. [V]
- **Where**: S Game.cpp:17134, 17182-17186
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/trade.md:129

### TRADE-007 Guild join form (item 88) leaks the CItem
- **Program**: game server
- **System**: trade (give), guild
- **Kind**: bug
- **Intent**: consume the form properly.
- **Now**: the join path jumps to REMOVE_ITEM_PROCEDURE (17426) and sets the slot to NULL without deleting the CItem: a small leak per form; the form is consumed even if the master later rejects. [V]
- **Where**: S Game.cpp:17426
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/trade.md:131

### TRADE-008 No server-side distance check for give and exchange
- **Program**: game server / client
- **System**: trade
- **Kind**: client-only check
- **Intent**: give and exchange work only with a nearby target.
- **Now**: give and exchange accept any (dX, dY) on the map that holds the target; only the client limits range [I that the client checks; server code shows none]. [V] Deposit to the warehouse keeper is the same give command: any NPC named "Howard" accepts deposits, and the server checks the warehouse map flag but not distance to Howard (merged from WAREHOUSE-001). [V]
- **Where**: S Game.cpp:17094-17690, 40826-40915; S Game.cpp:17314, 17576, 27873, 32423
- **Evidence**: read in source; client check unverified (see the doc's open questions).
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/trade.md:133; research/systems/warehouse.md:91

### TRADE-009 Exchange checks only the partner's combat stance
- **Program**: game server
- **System**: trade (exchange)
- **Kind**: incomplete
- **Intent**: neither party may be in combat stance when an exchange opens.
- **Now**: the requester's own combat stance is not checked, only the partner's (40872). [V]
- **Where**: S Game.cpp:40872
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/trade.md:135

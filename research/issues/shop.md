# Issues: shop

Scope: defects and divergences in the NPC shop system (buy, sell, repair, price modifiers, city funds) of the 2.20 game server. Split out of `research/systems/shop.md`.

### SHOP-001 City funds are write-only
- **Program**: game server
- **System**: shop (city funds)
- **Kind**: incomplete
- **Intent**: purchase, repair and charged-teleport money was meant to fill the town treasury (v1.4 comment "add to the town's funds").
- **Now**: `iFunds` is incremented at S 17047, 24110, 34596 and zeroed at boot; nothing reads, spends or saves it. The money simply leaves the economy. [V]
- **Where**: S Game.cpp:17047, 24110, 34596; boot reset 379-383, 730-734
- **Evidence**: grep of `iFunds`, Game.h:768-773.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/shop.md:127

### SHOP-002 Shop catalogue is enforced only on the client
- **Program**: game server / client
- **System**: shop (buy)
- **Kind**: client-only check / security
- **Intent**: a shop sells only the items its NPC lists.
- **Now**: the catalogue lives in the client's `contents*.txt`. The server sells any item with a non-negative Item.cfg price to anyone standing in any shop map (S 16841, 16874); a modified client can buy items no NPC lists, from any of the three shop interiors. [V]
- **Where**: S Game.cpp:16841, 16874
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/shop.md:130

### SHOP-003 Purchase price truncated to 16 bits
- **Program**: game server
- **System**: shop (buy)
- **Kind**: bug / overflow
- **Intent**: charge the computed price.
- **Now**: the charged price goes through `WORD wTempPrice` (S 17030, 17042, 17047) while the gold check uses the full int (16945). Any purchase over 65535 gold is charged mod 65536. Buying count N of a pricey item in one request multiplies first, so large stacks hit this [I]. [V]
- **Where**: S Game.cpp:16945, 17030, 17042, 17047
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/shop.md:133

### SHOP-004 Sell quote and confirm use different integer types for weapons
- **Program**: game server
- **System**: shop (sell)
- **Kind**: bug / overflow
- **Intent**: the confirmed sale pays the quoted price.
- **Now**: quote uses `(int)d3` (S 33984), confirm `(short)d3` (34154). For a weapon whose depreciated half-price exceeds 32767 the confirm wraps negative, falls to the clamp of 1 gold, and the item is gone. [V]
- **Where**: S Game.cpp:33984, 34154
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/shop.md:136

### SHOP-005 Repair price held in a short
- **Program**: game server
- **System**: shop (repair)
- **Kind**: bug / overflow
- **Intent**: repair price computed from item price.
- **Now**: repair uses `short sPrice` (S 34431, 34509): items priced above 65535 get a negative repair price; `(DWORD)sPrice` is then huge and the repair is always "not enough gold" (34556). [V]
- **Where**: S Game.cpp:34431, 34509, 34556
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/shop.md:139

### SHOP-006 Repair confirm skips the NPC-type rule; sell quote skips the shop-zone check
- **Program**: game server
- **System**: shop (repair, sell)
- **Kind**: client-only check / security
- **Intent**: the quote rules (weapons only at a blacksmith, sales only in shops) apply to the confirm as well.
- **Now**: only the repair quote has the NPC-type rule, so a modified client repairs a weapon at the shopkeeper. The sell quote has no shop-zone check (comment "sell anywhere", S 33940); the confirm does (34124), so the quote works anywhere and the sale only in shops. [V] `ReqRepairItemCofirmHandler` needs only the shop flag, charges the same price and pays it into the player's side city funds (see SHOP-001) (merged from GSO-007). [V]
- **Where**: S Game.cpp:33940, 34124, 34507-34524; S Game.cpp:34428-34500, 34596
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/shop.md:141; research/game-server/mechanics.md:189; research/game-server/functions/items.md:297

### SHOP-007 Sell/repair NPC type is trusted from the client
- **Program**: game server
- **System**: shop (sell, repair)
- **Kind**: client-only check / security
- **Intent**: sell and repair talk to a shopkeeper (15) or blacksmith (24) NPC.
- **Now**: `ReqSellItemHandler` NPC switch only accepts 15 and 24 (33942-33944); `cSellToWhom` is otherwise unused, so any shop NPC type the client names as 15/24 works. The server never checks that such an NPC is near. [V]
- **Where**: S Game.cpp:33942-33944
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/shop.md:144

### SHOP-008 Negative Charisma discount path is unreachable
- **Program**: game server
- **System**: shop (buy)
- **Kind**: dead code
- **Intent**: Charisma discount (Cha - 10)/4.
- **Now**: Charisma below 10 would give a negative discount (a surcharge) at 16924, but the login decoder rejects Charisma < 10 (S 6301), so the path is unreachable; Cha 10-13 gives 0 %. [V]
- **Where**: S Game.cpp:16924, 6301
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/shop.md:146

### SHOP-009 Use-after-free in the dead BUY log path
- **Program**: game server
- **System**: shop (buy, item log)
- **Kind**: dead code / bug
- **Intent**: log the purchase.
- **Now**: `_bItemLog(BUY, ..., pItem)` is called after `delete pItem` (17033-17090); compiled out (DEF_TAIWANLOG off), so it never runs. [V]
- **Where**: S Game.cpp:17033-17090
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/shop.md:148

### SHOP-010 Crusade-winner discount matches neutrals and misses the real winner
- **Program**: game server
- **System**: shop (price modifiers), crusade
- **Kind**: bug
- **Intent**: only the side that won the last crusade gets the 10 % discount (S 862, 1942-1943, 4953, 23544).
- **Now**: the test is `m_iCrusadeWinnerSide == m_cSide` (S 16903). The winner field is 0 at boot, after a draw, and from the start of every crusade (`LocalStartCrusadeMode`, S 47918), and neutrals have `m_cSide` 0. With no winner recorded, neutrals get the 10 % discount (and `cPrice = -10` is shown); during a running crusade the real winner side's discount is off. [V]
- **Where**: S Game.cpp:16903, 47918, 862, 1942-1943, 4953, 23544
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/shop.md:150

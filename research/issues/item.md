# Issues: item

Scope: defects and divergences in the item system (definitions, instances, inventory, floor, use, durability, drops, anti-dupe, item log) of the 2.20 game server. Split out of `research/systems/item.md`; the doc describes the intended behaviour and what the code does now.

### ITEM-001 Farming tool wear null check uses OR instead of AND

Merged into [GATHERING-004](gathering.md#gathering-004-hoe-life-test-uses--where--was-meant).

### ITEM-002 Tile address multiplies by the tile height instead of the width

Merged into [TILE-001](tile.md#tile-001-tile-index-is-x--ysizey-wrong-for-non-square-maps).

### ITEM-003 Lottery items roll but both branches are empty
- **Program**: game server
- **System**: item (use)
- **Kind**: incomplete
- **Intent**: lottery items (effect 23) were meant to award a prize on a win.
- **Now**: both outcomes are empty (S 30678-30688); the item is consumed for nothing. [V]
- **Where**: S Game.cpp:30678-30688
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/item.md:215

### ITEM-004 Split stacks lose instance fields
- **Program**: game server
- **System**: item (instances)
- **Kind**: bug
- **Intent**: a part split from a stack should carry the stack's instance data.
- **Now**: split stacks (drop, give, exchange) are re-created from the definition (S 15624, 17124, 41055), so touch id, colour and attribute of the original stack do not travel with the split part. Harmless for gold and arrows; would matter for any stackable with instance data [I]. [V]
- **Where**: S Game.cpp:15624, 17124, 41055
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/item.md:217

### ITEM-005 Floor overflow deletes the oldest item with no log
- **Program**: game server
- **System**: item (floor)
- **Kind**: bug
- **Intent**: a tile holds 12 items (a rule); the loss of the 13th-drop victim was presumably not meant to be silent.
- **Now**: the 13th item dropped on a tile deletes the oldest one (Map.cpp:324-326) with no log: an unlogged item sink. [V]
- **Where**: S Map.cpp:324-326
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/item.md:220

### ITEM-006 Bank-load weight guard has a copy-pasted message and a dead check
- **Program**: game server
- **System**: item (persistence, warehouse)
- **Kind**: bug / dead code
- **Intent**: log a bank stack that is over the weight limit when loading the player record.
- **Now**: the log line says "GemStone of Sacrifice to lifespan 0" (S 6135-6139), a copy-pasted message; `if (dTemp < 0)` on a DWORD is dead (6133). [V]
- **Where**: S Game.cpp:6133-6139
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/item.md:222

### ITEM-007 Dead TAIWANLOG item log pair; the shipped pair logs only good items
- **Program**: game server
- **System**: item (item log)
- **Kind**: dead code
- **Intent**: log item events for audit.
- **Now**: the `#if DEF_TAIWANLOG` `_bItemLog` pair (42473-42702) logs every action but is not compiled; the shipped Korean/Chinese pair filters to good items, so most drops and trades are never logged. [V] Detail: the by-name `_bItemLog` handles six actions but its only compiled callers are DeleteNpc, NpcDeadItemGenerator and NpcDeadItemGeneratorWithItemEvent (all NEWGENDROP); every other by-name call passes a NULL item inside an inactive DEF_TAIWANLOG block, and handle-form calls inside DEF_TAIWANLOG blocks (AdminOrder_CreateItem, BuildItemHandler, RequestCivilRightHandler, RequestPurchaseItemHandler, bSetItemToBankItem) are inactive too. `MSGID_ITEMLOG` has a packing case (comment "not used", S 20654-20657) and a gate handler (G GateCore.cpp:410) but no `SendMsgToGateServer` caller. Merged from GSO-044, ADMIN-009 and COMMANDS-006. [V]
- **Where**: S Game.cpp:42473-42702, active pair from 42703; S Game.cpp:42704-42870, 20654-20657; G GateCore.cpp:410
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/item.md:226; research/game-server/functions/ui-logging.md:78, :70; systems/admin.md section 9; research/systems/commands/overloaded.md:81

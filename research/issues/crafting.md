# Issues: crafting

Scope: defects and divergences in alchemy, manufacturing and item upgrade (stone and Gizon) in the 2.20 game server. Split out of `research/systems/crafting.md`.

### CRAFTING-001 Manufacturing consumes the wrong slots
- **Program**: game server
- **System**: crafting (manufacturing)
- **Kind**: bug
- **Intent**: consume exactly the items and counts that matched each recipe material.
- **Now**: materials are matched by `z` (S 43621-43637) but consumed by position `x` with the recipe's count for material `x` (43761-43772). When the client's slot order differs from the recipe order, or counts differ between materials, the wrong items lose the wrong counts. With count-1 ingots the effect is usually invisible [I]. [V]
- **Where**: S Game.cpp:43621-43637, 43761-43772
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crafting.md:140

### CRAFTING-002 Off-by-one slot bound in BuildItemHandler
- **Program**: game server
- **System**: crafting (manufacturing)
- **Kind**: bug
- **Intent**: reject bag indices outside 0..49.
- **Now**: `cElementItemID[i] > DEF_MAXITEMS` (43594) allows index 50, reading past `m_pItemList[50]`. [V]
- **Where**: S Game.cpp:43594
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crafting.md:144

### CRAFTING-003 Crafted item read after bAddItem may have deleted it
- **Program**: game server
- **System**: crafting (manufacturing)
- **Kind**: bug
- **Intent**: read the product's quality after it is added to the bag.
- **Now**: `pItem->m_sItemSpecEffectValue2` is read after `bAddItem` (43752-43753), which deletes the item when it merges into a stack. Built items are equip/material types that never stack, so it does not trigger in practice [I]. [V]
- **Where**: S Game.cpp:43752-43753
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crafting.md:147

### CRAFTING-004 Crafted-upgrade bonus branch is unreachable
- **Program**: game server
- **System**: crafting (stone upgrade)
- **Kind**: dead code
- **Intent**: add a quality-based bonus to the upgrade odds for crafted items (base 8-20 % or /40 for base <= 7 %).
- **Now**: `if (iProb > 20)` (52465) can never be true because the base odds top out at 20. [V]
- **Where**: S Game.cpp:52465
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crafting.md:149

### CRAFTING-005 Manufacturing skill roll happens before the request is validated
- **Program**: game server
- **System**: crafting (manufacturing)
- **Kind**: bug
- **Intent**: validate the request, then roll.
- **Now**: the roll (1d100 <= skill 13) happens before materials are validated, so a failed roll on an invalid request still sends BUILDITEMFAIL; a valid request at low skill can be retried endlessly at no cost (time only, rate limited by `m_iSkillMsgRecvCount` [I]). [V]
- **Where**: S Game.cpp:43580-43588
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crafting.md:151

### CRAFTING-006 Alchemy recipe search has no break
- **Program**: game server
- **System**: crafting (alchemy)
- **Kind**: bug
- **Intent**: use the recipe that matches the ingredient array.
- **Now**: the search over recipes has no `break` (38401-38414); if two recipes share an ingredient array the last one wins silently. [V]
- **Where**: S Game.cpp:38401-38414
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crafting.md:154

### CRAFTING-007 BuildItemHandler prints every crafted item to the server log window
- **Program**: game server
- **System**: crafting (manufacturing)
- **Kind**: bug (debug leftover)
- **Intent**: debug output only ("testcode").
- **Now**: every crafted item is printed to the server log window unconditionally (43747-43749). [V]
- **Where**: S Game.cpp:43747-43749
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crafting.md:158

### CRAFTING-008 Old manufacturing body left commented out
- **Program**: game server
- **System**: crafting (manufacturing)
- **Kind**: dead code
- **Intent**: superseded version kept for reference.
- **Now**: the old body (43787-43942) is commented out; it consumed materials before creating the item and used spec value 1 as purity, showing the purity model moved to spec value 2. [V]
- **Where**: S Game.cpp:43787-43942
- **Evidence**: read in source.
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/crafting.md:160

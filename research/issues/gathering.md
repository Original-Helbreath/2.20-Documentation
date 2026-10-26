# Issues: gathering

Scope: defects and divergences in fishing, mining and farming (documented in `research/systems/gathering.md`). Paths and tags as in that doc: `SRV` = `sources/hgserver-librehelbreath/src/`, `CLI` = `sources/client-2.20/`; [V] verified, [I] inferred, [U] unverified.

### GATHERING-001 Generator and processor loops use break where continue was meant
- **Program**: game server
- **System**: gathering
- **Kind**: bug
- **Intent**: skip one angler, point or allocation failure and carry on with the rest.
- **Now**: `FishProcessor` uses `break` when an angler's fish is gone (36956), ending the pass for every later angler. `FishGenerator` (37268, 37274) and `MineralGenerator` (38807) `break` on a bad point or allocation failure, ending the pass for every later map.
- **Where**: SRV Game.cpp:36956, 37268, 37274, 38807
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/gathering.md:140

### GATHERING-002 Crop count can never fall once a map reaches 200
- **Program**: game server
- **System**: gathering
- **Kind**: bug
- **Intent**: decrement the per-map crop count when a crop is removed.
- **Now**: `CMap::bRemoveCropsTotalSum` only decrements while the count is below 200; once a map reaches 200 crops the count can never fall, so farming on that map stays blocked until restart.
- **Where**: SRV Map.cpp:786
- **Evidence**: [V code; I effect]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/gathering.md:143

### GATHERING-003 Planted crop may land on a non-farm neighbouring tile
- **Program**: game server
- **System**: gathering
- **Kind**: bug
- **Intent**: the crop stands on the clicked farm tile.
- **Now**: planting checks the farm flag at the clicked tile, but `bCreateNewNpc` then places the crop with `bGetEmptyPosition`, which may move it to a neighbouring non-farm tile.
- **Where**: SRV Game.cpp:10160
- **Evidence**: [V code; I effect]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/gathering.md:146

### GATHERING-004 Hoe-life test uses || where && was meant
- **Program**: game server
- **System**: gathering
- **Kind**: bug
- **Intent**: both the hoe slot index and the item pointer must be valid.
- **Now**: the test is `(iTemp != -1) || (pItem != NULL)`; harmless only because a hoe must be equipped to reach it. ITEM-001 (merged here) reads the same test as a dereference of `m_pItemList[-1]` when the player has no right-hand item and the crop roll passes; GATHERING-004 above says it is harmless because a hoe must be equipped to reach it. The two readings were not reconciled [U].
- **Where**: SRV Game.cpp:12864
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/gathering.md:148; research/systems/item.md:209

### GATHERING-005 Planting keeps a commented crusade-builder guard copied from construction code
- **Program**: game server
- **System**: gathering
- **Kind**: dead code
- **Intent**: farming was cloned from the crusade construction kit; the builder-only guard does not apply.
- **Now**: the guard is commented out and left in `__bSetAgricultureItem`.
- **Where**: SRV Game.cpp:52952-52953
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/gathering.md:150

### GATHERING-006 Meat gathering (GET value 1) requires water and could never work on land
- **Program**: game server
- **System**: gathering
- **Kind**: incomplete
- **Intent**: simple-fishing GET skill value 1 yields meat (item 99).
- **Now**: the water-tile test (33730) precedes the switch, so meat gathering, if ever configured, could never work on land.
- **Where**: SRV Game.cpp:33730
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/gathering.md:152

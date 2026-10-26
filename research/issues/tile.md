# Issues: tile

Scope: defects and divergences in the tile grid, flags, occupancy, item stacks and dynamic objects (documented in `research/systems/tile.md`). Paths and tags as in that doc: `SRV` = `sources/hgserver-librehelbreath/src/`, `CLI` = `sources/client-2.20/`; [V] verified, [I] inferred, [U] unverified.

### TILE-001 Tile index is x + y*SizeY, wrong for non-square maps
- **Program**: game server
- **System**: tile
- **Kind**: bug
- **Intent**: row-major tile index (x + y*SizeX).
- **Now**: the index is `x + y*m_sSizeY` everywhere. Self-consistent, so square maps work; for non-square maps tiles alias, and for SizeY > SizeX the last rows overrun the `CTile[SizeX*SizeY]` allocation (amd-map.md "Differences"). ITEM-002 (merged here) records the same `m_pTile + sX + sY*m_sSizeY` addressing (Map.cpp:322, 346) from the item side: on maps wider than they are tall, floor items of two tiles would share a stack [I].
- **Where**: SRV Map.cpp:186-715, Game.cpp:1358, 2268, 3375, 37377, 39522; S Map.cpp:322, 346
- **Evidence**: [V code; impact I]
- **Blocks 2.20 setup**: unknown (only if a shipped map is non-square; only arebrk11, 150x150, measured)
- **Source doc**: research/systems/tile.md:111; research/systems/item.md:211

### TILE-002 bGetMoveable assigns to its by-value pTopItem parameter (land-mine feature unfinished)
- **Program**: game server
- **System**: tile
- **Kind**: incomplete
- **Intent**: v2.172 "mine on the floor" feature: the walk test hands back the top item on the tile so the move handler can trigger land-mine reagents (items 540/541).
- **Now**: `bGetMoveable(..., CItem* pTopItem)` assigns to its by-value parameter, so the caller never sees the item; the consuming code in the move handler is commented out.
- **Where**: SRV Map.cpp:247; SRV Game.cpp:1252-1307
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/tile.md:115

### TILE-003 ClearDeadOwner and ClearSectorInfo have no callers
- **Program**: game server
- **System**: tile
- **Kind**: dead code
- **Intent**: clear a tile's dead owner and sector info.
- **Now**: both functions exist and are never called.
- **Where**: SRV Map.cpp (`ClearDeadOwner`, `ClearSectorInfo`)
- **Evidence**: [V mm] (map-movement.md)
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/tile.md:120

### TILE-004 Client ignores the occupy status in move confirms
- **Program**: client
- **System**: tile
- **Kind**: dead code
- **Intent**: v1.3 territory display from `m_iOccupyStatus`.
- **Now**: the server reports `m_iOccupyStatus` in every move confirm; the client's assignment is commented out (`//m_iOccupyStatus = ...`), so the territory display is dead.
- **Where**: CLI Game.cpp:35574; SRV Game.cpp:1357-1359
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/tile.md:121

### TILE-005 NEWDYNAMICOBJECT and DELDYNAMICOBJECT notifies are never sent
- **Program**: game server
- **System**: tile
- **Kind**: dead code
- **Intent**: notify clients of dynamic-object creation/removal.
- **Now**: `DEF_NOTIFY_NEWDYNAMICOBJECT` / `DELDYNAMICOBJECT` are defined but never sent; `MSGID_DYNAMICOBJECT` replaced them.
- **Where**: by-system.md entity row
- **Evidence**: [V per doc]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/tile.md:123

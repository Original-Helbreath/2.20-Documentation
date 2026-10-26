# Issues: movement

Scope: defects and divergences in walking, running, turning, collision, position sync, speed-hack checks and client prediction (documented in `research/systems/movement.md`). Paths and tags as in that doc: `SRV` = `sources/hgserver-librehelbreath/src/`, `CLI` = `sources/client-2.20/`; [V] verified, [I] inferred, [U] unverified.

### MOVEMENT-001 Coordinate mismatch on a move is returned but never acted on
- **Program**: game server
- **System**: movement
- **Kind**: incomplete
- **Intent**: per the comment at 1143, stale commands sent before the client saw a reject are dropped.
- **Now**: a coordinate mismatch returns 2, but the MOVE/RUN cases never act on 2 (only STOP/ATTACK/GETITEM/MAGIC send a reject). It works because the client already resynced on the earlier reject.
- **Where**: SRV Game.cpp:1033-1035, 1143-1144
- **Evidence**: [V]; [I] on the intent
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/movement.md:139

### MOVEMENT-002 Move-frequency check trusts the client-supplied timestamp
- **Program**: game server
- **System**: movement
- **Kind**: security
- **Intent**: catch speed hacks by a 250 ms gap between moves.
- **Now**: `bCheckClientMoveFrequency` uses `dwClientTime` from the packet, so it only catches naive speed hacks; the 7-step server-clock window is the real check.
- **Where**: SRV Game.cpp:1018-1020, 52698-52703
- **Evidence**: [V code; I effect]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/movement.md:142

### MOVEMENT-003 OBJECTDAMAGEMOVE bypasses the frequency check and run SP cost
- **Program**: game server
- **System**: movement
- **Kind**: security
- **Intent**: knock-back step triggered by a damage notify.
- **Now**: `OBJECTDAMAGEMOVE` goes through the walk path with no frequency check and no run SP; a forged damage-move bypasses the 250 ms check but not the 7-step window (it increments the walk counter).
- **Where**: SRV Game.cpp:1061-1068
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/movement.md:144

### MOVEMENT-004 Client locateable test reads one past the cell array
- **Program**: client
- **System**: movement
- **Kind**: bug
- **Intent**: local walk test of a cell inside the map window.
- **Now**: the inclusive window test in `bGetIsLocateable` / `bIsTeleportLoc` reads one past the cell array (world-map.md).
- **Where**: CLI MapData.cpp:867-875
- **Evidence**: [V map]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/movement.md:148

### MOVEMENT-005 Hunter portal blocks are hard-coded coordinates in the client
- **Program**: client
- **System**: movement
- **Kind**: client-only check
- **Intent**: keep hunters (civilian mode) out of the town to middleland portals outside crusade.
- **Now**: `_bCheckMoveable` hard-codes portal coordinates with map indices 3/11; the server equivalent is a recall after arrival (map.md sect. 7).
- **Where**: CLI Game.cpp:1402-1429
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/movement.md:150

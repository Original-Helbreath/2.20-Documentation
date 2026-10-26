# Issues: map

Scope: defects and divergences in maps, `.amd` loading, map info, teleport/recall, cross-server routing and map access rules (documented in `research/systems/map.md`). Paths and tags as in that doc: `SRV` = `sources/hgserver-librehelbreath/src/`, `CLI` = `sources/client-2.20/`; [V] verified, [I] inferred, [U] unverified.

### MAP-001 Teleport request mode byte is the low byte of the message id
- **Program**: game server
- **System**: map
- **Kind**: bug
- **Intent**: "tile teleport": `RequestTeleportHandler` was meant to read a mode character ('0'-'3') from the packet.
- **Now**: `MSGID_REQUEST_TELEPORT` hands the whole packet to the handler, so `pData[0]` is the low byte of the MSGID (0x0EA03201 -> 0x01), never '0'-'3'. It works only because the portal branch is chosen by tile lookup. Off a portal tile the switch matches nothing and the handler falls to `RTH_NEXTSTEP`, re-sending init data for the current map after the player was removed and re-added.
- **Where**: SRV Game.cpp:15029, 23377; SRV NetMessages.h:342
- **Evidence**: [V code 15029, 23377; I effect]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/map.md:195

### MAP-002 Forced recall case '0' has no body and no break
- **Program**: game server
- **System**: map
- **Kind**: bug
- **Intent**: forced recall to a level-limit map (the source comment says so).
- **Now**: case '0' is empty and falls through into '1', so forced recall is a plain recall; it also obeys the level-80 farm rule and the locked map.
- **Where**: SRV Game.cpp:23379-23383
- **Evidence**: [V] fall-through; [I] that this is unintended
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/map.md:200

### MAP-003 Enemy-town recall block ignores NOPENALTY; resurrection-zone check is dead
- **Program**: game server
- **System**: map
- **Kind**: dead code
- **Intent**: block recall from enemy towns and from the resurrection zone.
- **Now**: the enemy-town recall block compares `m_cLocationName` while recall '1' ignores the map type `NOPENALTY`; the commented "no recall from resurrection zone" check is dead.
- **Where**: SRV Game.cpp:23268-23281, 23385
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/map.md:203

### MAP-004 Crusade-winner shop discount price computed and never used
- **Program**: game server
- **System**: map
- **Kind**: dead code
- **Intent**: shop discount for the crusade winner, computed in the teleport tail.
- **Now**: `cPrice` is computed and never used.
- **Where**: SRV Game.cpp:23540-23547
- **Evidence**: [V mm] (map-movement.md)
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/map.md:205

### MAP-005 Enemy-building pre-check is client-side and compiled only for DEF_LANGUAGE 3
- **Program**: client
- **System**: map
- **Kind**: client-only check
- **Intent**: refuse entry to an enemy town's buildings before sending the teleport request.
- **Now**: the client pre-check is compiled only under `#if DEF_LANGUAGE == 3`; the server enforces the same rule independently by a 1-tick recall for a sided non-admin entering an enemy farm or building (see pk-law.md), so the pre-check is cosmetic.
- **Where**: CLI Game.cpp:45570-45610; SRV Game.cpp:2126-2144, 23729-23750
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/map.md:206

### MAP-006 Beginner-dungeon level 30-80 gate exists only in the client
- **Program**: client
- **System**: map
- **Kind**: client-only check
- **Intent**: keep players outside level 30-80 out of the beginner dungeon.
- **Now**: the gate is in the client only; the server's own guard is the map's `level-limit`/`upper-level-limit`, checked on a timer after arrival.
- **Where**: CLI Game.cpp:45541-45570; SRV Game.cpp:4061
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/map.md:207

### MAP-007 Map without initial points leaves uninitialised coordinates
- **Program**: game server
- **System**: map
- **Kind**: bug
- **Intent**: `bGetEmptyPosition` falls back to the map's initial point when no free tile is found.
- **Now**: if a map has no initial points, `GetMapInitialPoint` writes nothing and `bGetEmptyPosition` copies uninitialised locals into the player position.
- **Where**: SRV Game.cpp:11499-11503, 42357
- **Evidence**: [V]
- **Blocks 2.20 setup**: unknown (only for a map file lacking initial points)
- **Source doc**: research/systems/map.md:211

### MAP-008 Old bReadProgramConfigFile is a dead commented block
- **Program**: game server
- **System**: map
- **Kind**: dead code
- **Intent**: config reader with its own `game-server-map` branch.
- **Now**: the older `bReadProgramConfigFile` (5053-5200, branch at 5151) sits inside a `/* */` block; the live reader is at 5202, called from 791.
- **Where**: SRV Game.cpp:5053-5200
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/map.md:213

### MAP-009 CMap::iAnalyze is an empty stub
- **Program**: game server
- **System**: map
- **Kind**: dead code
- **Intent**: "where is the fighting" analysis of sector activity.
- **Now**: empty stub with no callers; the sector heat map is the working replacement.
- **Where**: SRV Map.cpp (`CMap::iAnalyze`)
- **Evidence**: [V mm] (map-movement.md)
- **Blocks 2.20 setup**: no
- **Source doc**: research/systems/map.md:215

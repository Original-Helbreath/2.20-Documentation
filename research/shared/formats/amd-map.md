# .amd map file

Binary tile map, one per map name. Read by the client (drawing and walkability) and the game
server (walkability, teleport, farm, water). No program in the stack writes it (built by an
external map editor) [I]. Tags: [V] read in code, [I] inferred.

## Locations

- Client: `MAPDATA\<name>.amd`, 60 files in `downloads/retail_2001/extracted/MAPDATA/` [V listing];
  loaded by `CMapData::OpenMapDataFile` `sources/client-2.20/MapData.cpp:718`.
- Game server: `mapdata\<name>.amd` (`.claude/scratch/srv220/Map.cpp:422-425`), loaded by
  `CMap::_bDecodeMapDataFileContents` `srv220/Map.cpp:410` [V].

## Layout

| Offset | Size | Content |
|---|---|---|
| 0 | 256 | ASCII header, NUL bytes treated as spaces: `MAPSIZEX = <w> MAPSIZEY = <h> TILESIZE = <n>` [V `srv220/Map.cpp:436-470`; sample `srv220/mapdata/arebrk11.amd` = `150 150 10`] |
| 256 | w*h*n | tiles, row-major (y outer, x inner) [V both loaders] |

Tile record (n = 10 in every known file) [V `sources/client-2.20/MapData.cpp:735-757`, `srv220/Map.cpp:476-505`] (index at 480):

| Byte | Type | Field | Used by |
|---|---|---|---|
| 0-1 | int16 | tile sprite (value 19 = water) | client draw; server `m_bIsWater` |
| 2-3 | int16 | tile sprite frame | client |
| 4-5 | int16 | object sprite | client |
| 6-7 | int16 | object sprite frame | client |
| 8 | flags | 0x80 = not walkable, 0x40 = teleport tile, 0x20 = farming allowed (server, v2.19+) | both (0x20 server only) |
| 9 | - | unused by both loaders | - |

Check: arebrk11 is 225256 bytes = 256 + 150*150*10 [V file size].

## Differences and defects

- The client ignores `TILESIZE` and always steps 10 bytes (`MapData.cpp:731`); the server honours
  it (`m_sTileDataSize` used by the `ReadFile` at `Map.cpp:479`) [V].
- Server tile index is `m_pTile + ix + iy*m_sSizeY` (`srv220/Map.cpp:480`; 477 is only the `for`
  line), correct only for square maps (it should be `iy*m_sSizeX`). The same idiom is repeated at
  `Map.cpp:694` and `:702` (no-attack rectangles). For `SizeY > SizeX` the highest index
  `(X-1)+(Y-1)*Y` exceeds the `CTile[X*Y]` allocation (`Map.cpp:474`), so it is a heap overrun, not
  only a transposed layout [V; impact I]. The client uses `m_tile[x][y]` [V].
- The client stores tiles in a fixed `m_tile[752][752]` (`MapData.h:65`) and `OpenMapDataFile`
  (`MapData.cpp:735-757`) does not check the size, so a map with a side above 752 overruns it [V].
- `srv220/Misc.cpp:305-309` contains a one-off tool that merges `middleland1.amd` and
  `middleland2.amd` into `middleland.amd.result` [V], not part of normal runtime.

Sprite and tile graphics (`.pak`) are out of scope.

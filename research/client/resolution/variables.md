# Proposed resolution variables

The logical variables that replace the hard-coded values catalogued in the `values-*.md` files. Each has a
derivation from four base values, and the value it must produce at 640x480 so a first refactor can be
checked for zero behaviour change. Values at 640x480 are verified against the cited source; the
anchoring choices (right, centre, bottom) are proposals (*inferred* design intent).

## Contents
- Base values
- Screen and buffers
- World view
- Server-coupled map window (do not change client-side only)
- HUD and chat
- Dialogs
- Menus
- Window and cursor
- Values deliberately not turned into variables

## Base values

| Name | 640x480 | Meaning |
|---|---|---|
| `SCREEN_W`, `SCREEN_H` | 640, 480 | logical back-buffer size (the only new configuration inputs) |
| `TILE` | 32 | world cell size in pixels (fixed by art and protocol) |
| `HUD_H` | 53 | icon-panel art height (dialog 30 `sSizeY`, Game.cpp:730) |
| `ART_W`, `ART_H` | 640, 480 | size of the fixed-size art canvases (icon panel, menu backgrounds) |

## Screen and buffers

| Name | Formula | 640x480 | Replaces |
|---|---|---|---|
| `SCREEN_MAX_X`, `SCREEN_MAX_Y` | `SCREEN_W-1`, `SCREEN_H-1` | 639, 479 | mouse clamp, PutPixel, DrawShadowBox full-screen, top bar |
| `PDBGS_W`, `PDBGS_H` | `SCREEN_W+TILE`, `SCREEN_H+TILE` | 672, 512 | tile cache surface and its clip rect |
| `BACKBUF_BYTES` | `lPitch * SCREEN_H` | - | ClearBackB4 (DXC_ddraw.cpp:524) |

## World view

| Name | Formula | 640x480 | Replaces |
|---|---|---|---|
| `VIEW_W` | `SCREEN_W` | 640 | DrawObjects/DrawBackground x bounds, shadow clip x |
| `VIEW_H` | `SCREEN_H - HUD_H` | 427 | play-area bottom: DrawObjects/DrawBackground y bounds, shadow clip y, guide-map clamp |
| `PICK_MAX_Y` | `VIEW_H + 4` | 431 | object picking (Game.cpp:2390, 2513) |
| `SRV_CENTER_TX`, `SRV_CENTER_TY` | **constants, not derived from the screen** | 10, 7 | `/32)+10`, `/32)+7`, `player-10`, `player-7`, centre-tile maths (43 rows). They are the server's window offsets (`iComposeInitMapData(m_sX - 10, m_sY - 7)`, hgserver-hbxdevteam Game.cpp:1901), so they stay 10 and 7 whatever `SCREEN_W` is (round-2 V1; an earlier version derived them from the screen, which would desynchronise `PLAYER_CELL`) |
| `VIEW_CENTER_X`, `VIEW_CENTER_Y` | pixel centre derived from the screen: `VIEW_W/2`, `TILE*ceil(VIEW_H/(2*TILE))` | 320, 224 | view easing target `-288-32`, `-224` (Game.cpp:2542-2543). A pixel position, separate from the server tile offset above; they coincide at 640x480 only |
| `LOD_CENTER_TY` | `SRV_CENTER_TY + 1` | 8 | animation level-of-detail centre (MapData.cpp:1734) |
| `LISTEN_X`, `LISTEN_Y` | `SCREEN_W/2`, `SCREEN_H/2` | 320, 240 | sound pan/distance (54 rows); note the listener y (240) is **not** the player's screen y (224) - keep for fidelity or switch to `VIEW_CENTER_Y` deliberately |
| `VIEW_TILES_X`, `VIEW_TILES_Y` | `SCREEN_W/TILE + 1`, `SCREEN_H/TILE + 1` | 21, 16 | observer panning map-edge limits `32*21`, `32*16` |
| `OVR_L`, `OVR_T`, `OVR_B` | constants | 128, 224, 352 | DrawObjects overscan for tall sprites (4/7/11 tiles); art-dependent, not screen-dependent - keep |
| `OVR_LT`, `OVR_TT` | `OVR_L/TILE`, `OVR_T/TILE` | 4, 7 | DrawObjects start indices `sPivotX-4`, `sPivotY-7` (Game.cpp:2299, 2301); the 7 equals `VIEW_CENTER_TY` by coincidence - keep them separate |
| `TILE_LOOP_MAX_X`, `TILE_LOOP_MAX_Y` | `VIEW_W + 48`, `VIEW_H + 48` | 688, 475 | DrawBackground loops (Game.cpp:15854-15857) |
| `CELL_VIS_MAX_X`, `CELL_VIS_MAX_Y` | `VIEW_W + 16`, `VIEW_H + 48` | 656, 475 | per-cell visibility (Game.cpp:2308) |
| `CHAT_CULL` | `(-100, 0, SCREEN_W+160, SCREEN_H+120)` | (-100,0,800,600) | chat-bubble cull (Game.cpp:35119) |
| `WEATHER_BOX` | x `pivot*32 + 100 .. +1040`, y `pivot*32 - 360 .. +440` (second branch `+0 .. +800`, Game.cpp:23249) | as source | weather spawn (Game.cpp:23213-23249); it covers the server window, so it follows `SRV_WIN_*`, not the screen |

## Server-coupled map window (do not change client-side only)

| Name | Formula | Value | Source |
|---|---|---|---|
| `SRV_WIN_W`, `SRV_WIN_H` | server constant | 21, 16 | client ShiftMapData (MapData.cpp:806-860); server `iComposeInitMapData` (hgserver-hbxdevteam Game.cpp:2145-2146) |
| `SRV_WIN_X0`, `SRV_WIN_Y0` | `4+5`, `5+5` | 9, 10 | first valid cell in `m_pData` (Game.cpp:46001-46005, MapData.cpp:812-856) |
| `PLAYER_CELL_X`, `PLAYER_CELL_Y` | `SRV_WIN_X0 + SRV_CENTER_TX`, `SRV_WIN_Y0 + SRV_CENTER_TY` | 19, 17 | `sX + 14 + 5`, `sY + 12 + 5` (Game.cpp:45987-45988) |
| `MAPDATA_W`, `MAPDATA_H` | `SRV_WIN_W + 19`, `SRV_WIN_H + 19` | 40, 35 | MapData.h:26-27 |

`SRV_CENTER_TX/TY` are the server's `-10`/`-7`, fixed constants: changing them client-side desynchronises
the player position inside the map window. Do not express them as a function of `SCREEN_W`/`VIEW_H`.

Not variables (removed in round 2): `SKY_START_Y` (-460) and `BOLT_START_Y` (-800). Those literals are offsets
from the effect's own world position, not from the view (`coincidental.md`).

## HUD and chat

| Name | Formula | 640x480 | Replaces |
|---|---|---|---|
| `HUD_X0` | `(SCREEN_W - ART_W) / 2` | 0 | x origin of icon and gauge panel (dialogs 29, 30) |
| `HUD_Y0` | `SCREEN_H - HUD_H` (= `VIEW_H`) | 427 | y origin; every literal y in 434-478 on the panel becomes `HUD_Y0 + (y - 427)` |
| `HUD(x, y)` | `(HUD_X0 + x, HUD_Y0 + y - 427)` | identity | 51 icon/gauge rows (`values-hud.md`) |
| `CHAT_BAR_Y0`, `CHAT_BAR_Y1` | `VIEW_H - 14`, `VIEW_H + 2` | 413, 429 | chat-input shadow bar (Game.cpp:35132) |
| `CHAT_INPUT_X`, `CHAT_INPUT_Y` | `10`, `VIEW_H - 13` | 10, 414 | `StartInputString(10, 414, ...)` (8 rows) |
| `CHAT_BASE_Y` | `VIEW_H - 105` | 322 | lower event list `322 + i*15` (Game.cpp:45506-45526) |
| `SNOWLINE_Y` | `VIEW_H - 2` | 425 | X-mas snow pile `478 - 53` (Game.cpp:23147) |
| `SKILLMSG_X`, `SKILLMSG_Y` | `VIEW_CENTER_X - 69`, `VIEW_CENTER_Y + 4` | 251, 228 | "using a skill" text (Game.cpp:45532) |

The upper event list (`10 + i*15`), the F1 help window (23,90) and the FPS text (10,100) are top-left
anchored: no variable needed.

## Dialogs

| Name | Formula | 640x480 | Use |
|---|---|---|---|
| `DLG_AREA` | `(0, 0) - (SCREEN_W, VIEW_H)` | 640x427 | region dialogs live in |
| `DLG_RIGHT(x)` | `SCREEN_W - (ART_W - x)` | x | right-anchored defaults (337-class dialogs, guide map 512) |
| `DLG_CENTER_X(x)` | `x + (SCREEN_W - ART_W) / 2` | x | centred defaults |
| `DLG_BOTTOM(y)` | `y + (VIEW_H - 427)` | y | bottom-anchored defaults |
| `DLG_CENTER_Y(y)` | `y + (VIEW_H - 427) / 2` | y | play-area-centred defaults |
| `GUIDEMAP_SIZE` | constant | 128 | guide map clamp `SCREEN_W-128-20`, `VIEW_H-128-20` (Game.cpp:17881-17882, 32242-32243) |
| `GUIDEMAP_SNAP` | `SCREEN_W/2`, `VIEW_H/2`, `SCREEN_W - w`, `VIEW_H - h` | 320, 213, ... | quadrant snap (Game.cpp:35882-35885) |
| `DLG_REOPEN_MAX_X`, `_Y` | `SCREEN_W - 20`; `VIEW_H - 27` -> `VIEW_H - 17` | 620; 400 -> 410 | EnableDialogBox clamp (Game.cpp:18576-18577) |
| `QUERY_W`, `QUERY_H` | dialog 20 size (`sSizeX/Y`, Game.cpp:669-670) | 252, 87 | replace the literal `235`, `100` in the item-drop query clamp (30 rows). Today `QUERY_MAX_X = 639-235 = 404`, `QUERY_MAX_Y = 479-100 = 379` do not match the dialog size, so at the right edge the box (252 wide) reaches x=656, 16 px past the screen |
| `QUERY_MAX_X`, `QUERY_MAX_Y` | `SCREEN_MAX_X - QUERY_W`, `SCREEN_MAX_Y - QUERY_H` (640x480: 387, 392) | 404, 379 today | the 640x480 behaviour is kept in phase 0 and corrected afterwards; it clamps to the full screen, so the box may also overlap the HUD |

Per-dialog proposals are in `values-dialogs.md` (table generated from the constructor). Dragging is
unclamped today (Game.cpp:35849-35850, [CLIENT-RESOLUTION-002](../../issues/client-resolution.md#client-resolution-002-dialog-drag-is-unclamped)); a port should clamp to `DLG_AREA`.

## Menus

| Name | Formula | 640x480 |
|---|---|---|
| `MENU_X0`, `MENU_Y0` | `(SCREEN_W - ART_W)/2`, `(SCREEN_H - ART_H)/2` | 0, 0 |
| `MENU(x, y)` | `(MENU_X0 + x, MENU_Y0 + y)`, mouse `ms - MENU_X0/Y0` before hit tests | identity |

One offset per menu screen replaces all 891 menu rows; nothing inside a menu is re-anchored.

## Window and cursor

| Name | Formula | 640x480 |
|---|---|---|
| `WIN_X`, `WIN_Y` | `cx - SCREEN_W/2`, `cy - SCREEN_H/2`, with `cx, cy` = work-area centre | as source |
| `WIN_Y_BIAS` | `cy -= 40` when `cy > SCREEN_H/2 + 40` | threshold 280 |
| `CURSOR_HOME` | `(SCREEN_W - 130, VIEW_H - 27)` | (510, 400) | **conditional, language 2 only** (`DEF_HTMLCOMMOM`, runs only while the internet dialog `G_bDlg1` is open): Wmain.cpp:197, 204; main-menu `m_DInput.m_sX = 400, m_sY = 240` is a menu point `MENU(400, 240)` |

## Values deliberately not turned into variables

See `coincidental.md`: offsets inside dialogs, frame times, map coordinates, sprite ids, buffer sizes,
dialog art sizes (e.g. dialog 5 is 320x320, dialog 39 is 413 high) and the overscan constants.

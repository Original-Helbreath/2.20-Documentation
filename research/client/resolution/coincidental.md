# Coincidental values: do not change

Numbers that look like screen sizes but are not, so a search-and-replace does not break them. Every
row the enumeration found is in `coincidental-lines.md` (106 COINC, 114 FP, 129 PITCH_OK rows); this
page groups them and adds look-alikes the patterns did not target. Verified by reading each cited line.

## Groups found by the enumeration

| Group | Example | Rows | Why it is not screen-dependent |
|---|---|---|---|
| Offsets inside a dialog | `msX >= sX + 240` (Game.cpp:17548), `PutAlignedString(sX+24, sX+240, ...)` (37134), `sY + 224` (38456) | 63 | relative to the dialog origin; dialog art is fixed size |
| Animation frame times | `m_sFrameTime = 240` (MapData.cpp:47, 54, 290, 297) | 4 | milliseconds |
| Map tile coordinates | `m_sPlayerX == 240` (Game.cpp:45625-45666) | 7 | teleport spots in map cells |
| Sprite / tile ids, switch labels | `MakeTileSpr("maptiles4", 320, ...)` (Game.cpp:3723), `case 224:` (2566), `case 429:` (2636) | 7 | resource indices |
| Buffer sizes | `char _cURLName[320]` (Game.cpp:142, 179), `cRealFn[512]` (Misc.cpp:442) | 3 | string lengths |
| Effect-relative start offsets | `m_mY = sY - 460`, `m_mY = dY*32 - 460` (Game.cpp:6394, 6924), thunder `dY*32 - m_sViewPointY - 800` (7704, 7708, 7712) | 5 | offsets from the effect's own world position (cast or strike cell), not from the view; they do not change with the screen size. `-460 = 427+33` and `-800 = 480+320` are numerology, not derivations (round-2 K1; earlier versions classed these VIEW) |
| Dialog art sizes | dialog 5 `sSizeX/Y = 320` (Game.cpp:571-572) **only under `DEF_FEEDBACKCARD`, language 2**; every other build has 310x170 (575-578). Dialog 39 `sSizeY = 413` (784) | 3 | size of the dialog bitmap. Dialog 39 at 413 high exactly fits above the chat bar (413 = `VIEW_H - 14`) - re-check it if `VIEW_H` ever shrinks below 427 |
| Chat-bubble age | `iLoc > 320`, `iLoc > 352` (Game.cpp:21421-21422) | 2 | timer thresholds |
| Colours | `PutString2(..., 240,240,70)` (Game.cpp:33458), dye `RGB(...)` tables (960-987) | 1 + FP | RGB components |
| Per-object world offsets | `m_sViewPointY + iy - 321` etc. (Game.cpp:2608-2632) | 10 | smoke/fire attachment points on map objects, world pixels |
| Guide-map region tests | `shX > 301 && ... shY > 239` (Game.cpp:17998) | 1 | map cells: `shX/shY` come from `m_sPlayerX-64 + (msX-sX)` when zoomed, else the offset scaled by `m_sMapSizeX/128` (17897-17905, 17957-17969); the values exceed 128, so they are not art pixels |
| Dead cursor code | `G_Oldpoint.x = 320` (Game.cpp:161-163), `SetCursorPos(10,10)` (285, Wmain.cpp:189) | 7 | commented out |
| `EnableDialogBox(21, 0, 480, 0)` | Game.cpp:18546 | 1 | `sV1` argument (a value passed to the dialog), not a coordinate |
| FP: ids and text numbers | `EnableDialogBox(18, 813, ...)` (Game.cpp:19218 and others), `EnableDialogBox(20, n, ...)` | 114 | regex hits with no screen literal |
| PITCH_OK | `pDst += m_sBackB4Pitch`, `m_sPitch` | 129 | already resolution-independent |

## Look-alikes outside the patterns (found while reading)

| Where | Value | Meaning |
|---|---|---|
| Game.cpp:19604-19605, 19760-19761, 46780-46781, 47049-47050, 47236-47237 | `tX > 722`, `tY > 650` | Middleland map coordinates (crusade teleport clamp) |
| Game.cpp:31010, 41998 | `sX > 170`, `dX > 170` | inventory slot area inside the inventory dialog |
| Game.cpp:17903, 17961, 32250 | `m_sMapSizeX-128` | guide map scroll limit in map cells (128 = art size) |
| Game.cpp:2680-2681 | `rand() % 400`, `rand() % 300` | X-mas tree bulb spread around a tree, world pixels |
| DXC_ddraw.cpp:478-512 | `Sum >= 32` | 5-bit colour channel saturation |
| Game.cpp:23131 | `#define MAXNUM 1000` | X-mas snow pile array |

## Values that look coincidental but are not

- `213` (Game.cpp:17950, 35884) is `VIEW_H / 2`.
- `431` (Game.cpp:2390, 2513) is `VIEW_H + 4`, the picking limit.
- `322` (Game.cpp:45506-45526) is `VIEW_H - 105`, the lower event list.
- `512` as dialog 9 `sX` (Game.cpp:600) is `SCREEN_W - 128` (guide map, top-right).
- `400`/`410`/`620` in EnableDialogBox (Game.cpp:18576-18577) are screen clamps.
- `510, 400` cursor home (Wmain.cpp:197) is a screen point above the icon panel; it exists only in the
  language-2 HTML-dialog build (`DEF_HTMLCOMMOM`) and runs only while the internet dialog is open.

(`-460` / `-800` effect start heights were listed here before round 2; they are effect-relative, see the
table above.)

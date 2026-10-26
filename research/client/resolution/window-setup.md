# Window and DirectDraw setup, seen from the screen size

How the 2.20 client creates its window and surfaces and **where 640x480 is assumed**. This builds on
`..\rendering\directdraw.md` (class layout, blend tables, lost surfaces, GDI text), which is not
repeated. Source `sources\client-2.20`; every line below verified by reading the source in this pass
unless marked *inferred*.

## Contents
- Chain of objects
- Window
- Cooperative level and display mode
- Surfaces
- Presenting
- Clipping
- Pitch and pixel format
- Input coordinates
- Every place the size is assumed (summary)
- Hard limits that are not in the client
- Disagreements with existing docs

## Chain of objects

`WinMain` -> `InitInstance` creates `G_hWnd` (Wmain.cpp:324-335) -> `CGame::bInit` -> `DXC_ddraw::bInit(hWnd)`
(DXC_ddraw.cpp:54-190) -> `DXC_dinput::bInit` (DXC_dinput.cpp:33-60). Fullscreen is the release default,
windowed the `_DEBUG` default (`m_bFullMode`, DXC_ddraw.cpp:34-38); `ChangeDisplayMode` (244-328) rebuilds
the other mode at run time.

## Window

- `WS_POPUP`, no frame, always **640x480**, at `(cx-320, cy-240)` where `cx`,`cy` are half of
  `SM_CXFULLSCREEN`/`SM_CYFULLSCREEN` (the desktop work area, not the full screen) and `cy` is reduced by 40
  when `cy > 280` (Wmain.cpp:326-331). The same centring formula is repeated: **3 live copies** (the window
  creation at Wmain.cpp:326-331, `DXC_ddraw::bInit` 99-114 for `SetWindowPos` and `m_rcFlipping`, and
  `ChangeDisplayMode` 269-284) plus **2 build-conditional copies** that exist only in the language-2 HTML
  build (`#ifdef DEF_HTMLCOMMOM`, language 2 only, GlobalDef.h:171-179) and run only while the internet
  dialog `G_bDlg1` is open: Wmain.cpp:161-211 (`SetCursorPos(510,400)` and its windowed variant 201-204) and
  Game.cpp:35252-35304 (windowed cursor sprite, 35273-35276). They must stay identical: the windowed blit
  and the cursor maths both recompute the window origin instead of asking the window. The "windowed cursor"
  code is not part of the normal fullscreen or windowed game loop.
- `WM_SETCURSOR` hides the OS cursor (Wmain.cpp:213-215); the game draws its own cursor sprite.

## Cooperative level and display mode

| Mode | Cooperative level | Display mode | Primary |
|---|---|---|---|
| Fullscreen | `DDSCL_EXCLUSIVE \| DDSCL_FULLSCREEN` (DXC_ddraw.cpp:69, again 291) | `SetDisplayMode(640, 480, 16, 0, 0)` (72, again 294) - no enumeration, no fallback, failure aborts `bInit` | flip chain, 1 back buffer `m_lpBackB4flip` (77-86) |
| Windowed | `DDSCL_NORMAL` (DXC_ddraw.cpp:96, again 266) | desktop mode kept, any depth | plain primary, no flip chain (105-111) |

## Surfaces

| Surface | Size | Memory | Where | Notes |
|---|---|---|---|---|
| primary `m_lpFrontB4` | display | video | DXC_ddraw.cpp:81, 111 | |
| flip back `m_lpBackB4flip` | 640x480 (display) | video | :86 | fullscreen only, never drawn into directly |
| back buffer `m_lpBackB4` | **640x480** | system | :119, :315 | all compositing |
| tile cache `m_lpPDBGS` | **672x512** = (640+32)x(480+32) | system | :123, :319 | one tile of slack for sub-tile scroll |
| sprite sheets | per image | system | Sprite.cpp:79 via `pCreateOffScreenSurface` | |

`CSprite::PutSpriteFastFrontBuffer` (Sprite.cpp:483-572) blits straight to the primary `m_lpFrontB4`, bypassing
the back buffer; it has no literal size and no caller ([CLIENT-RESOLUTION-007](../../issues/client-resolution.md#client-resolution-007-putspritefastfrontbuffer-is-unused-and-bypasses-the-back-buffer)).

`pCreateOffScreenSurface(WORD, WORD)` (DXC_ddraw.cpp:330-344) rounds the width up to a multiple of 4
and requests no pixel format, so every surface inherits the primary's format.

## Presenting

`iFlip` (DXC_ddraw.cpp:192-242): fullscreen `BltFast(0,0, back, &m_rcFlipping={0,0,640,480})` then
`Flip(DDFLIP_WAIT)` (215-216) - this is the path of the language 3/4 builds (no `DEF_USING_WIN_IME`), i.e. the
shipped language 4; windowed `Blt(&m_rcFlipping, back, NULL, DDBLT_WAIT)` (224), where
`m_rcFlipping` is the window's desktop rectangle. Because the windowed path is a `Blt` with a
destination rectangle, DirectDraw would stretch if the rectangle differed from 640x480: the commented
`//SetRect( &m_rcFlipping, 0, 0, 1152, 864 );` at DXC_ddraw.cpp:223 shows someone tried exactly that.
With `DEF_USING_WIN_IME` (languages 1, 2, 5) fullscreen instead presents with `Blt(NULL, back)` to the whole
primary (199, no flip, the 1:1 `m_rcFlipping` logic does not apply), and the language-2 HTML build adds a third
branch that does the same while `G_hInternetWnd` is open (205-213). DXC_ddraw.cpp:198-219.

## Clipping

- DirectDraw clipper on the primary only (`InitFlipToGDI`, DXC_ddraw.cpp:620-655); it does not clip the
  software blitters.
- Software clip rectangle `m_rcClipArea`, global state read by every `CSprite::Put*` (320 references in
  Sprite.cpp): set to 640x480 at DXC_ddraw.cpp:60, widened to 672x512 while the tile cache is drawn and reset
  to 640x480 (Game.cpp:15852, 15867).
- Routines with their **own** literal bounds instead of the clip rect: `PutShadowSprite` x<640, y<427
  (Sprite.cpp:742, 760), `DXC_ddraw::PutPixel` 0-639/0-479 (DXC_ddraw.cpp:585), `CGame::DrawLine`/`DrawLine2`
  0-639/0-479 (Game.cpp:23347, 23381, 23455, 23489). `DrawShadowBox` (DXC_ddraw.cpp:528-579) does **no**
  clipping; every caller passes in-range literals.

## Pitch and pixel format

- The back buffer is locked once; `m_pBackB4Addr` and `m_sBackB4Pitch = (short)ddsd.lPitch >> 1` (pixels)
  are kept (DXC_ddraw.cpp:127-131). Every blitter indexes with that pitch or the sprite's own `m_sPitch`; no
  blitter hard-codes 640 or a shift such as `<< 9` (grep `\* ?640|<< ?(9|10)` finds only colour packing,
  verified). Pitch is a `short`: safe up to 16383-pixel rows.
- `ClearBackB4` clears `lPitch * 480` bytes (DXC_ddraw.cpp:524): height hard-coded.
- 16 bpp only: fullscreen forces 16; `_TestPixelFormat` (425-450) recognises 565, 555 and BGR565 by the
  red mask; windowed on a 32-bit desktop gets `m_cPixelFormat = 0` and `WORD*` writes into a 32-bit surface
  (see `..\rendering\directdraw.md`, Known defects).

## Input coordinates

DirectInput mouse, `DISCL_EXCLUSIVE | DISCL_FOREGROUND` (DXC_dinput.cpp:49), **relative** mickeys
accumulated into `m_sX/m_sY` and clamped to 0-639 / 0-479 (DXC_dinput.cpp:82-88). The OS cursor position is
read once at start (40-41) and never again, so the game's mouse is independent of window position and
scale. Consequence for scaling: a stretched window needs no mouse remapping, but a larger logical screen
needs the clamp changed. `msX == 640` / `msY == 480` tests for observer edge-panning (Game.cpp:35718-35730)
are never true because the clamp is 639/479 ([CLIENT-RESOLUTION-001](../../issues/client-resolution.md#client-resolution-001-observer-panning-right-and-bottom-never-fires); verified by reading; not run).

## Every place the size is assumed (summary)

| Area | Count (rows) | Detail |
|---|---|---|
| Platform: mode, window, surfaces, mouse | 33 (28 SCREEN + 5 BUFFER) | `values-platform.md` |
| Full-screen rectangles in game code | 54 | `values-screen-game.md` |
| World view: loops, centre, listener, picking, server window | 152 | `values-world-view.md` |
| HUD: icon/gauge panel, chat bar, event lists | 72 (+9 top-left, no change) | `values-hud.md` |
| Dialogs: default table, runtime placement, clamps | 128 | `values-dialogs.md` |
| Menus: 640x480 canvases | 891 | `values-menus.md` |

Counts are source lines (a line can carry several literals), measured by the enumeration in `method.md`.
Not every line is compiled in the shipped build; `method.md` (Build conditionals) gives the split
(793 of 1330 screen-dependent lines are live in the language-4 build).

## Hard limits that are not in the client

The server decides how much of the world the client knows. In the HBX server source
(`sources\hgserver-hbxdevteam\Game.cpp`, read only) the initial map data is a **21x16** tile block at
(player-10, player-7) (`iComposeInitMapData` loops `iy < 16`, `ix < 21`, lines 2145-2146; called with
`m_sX - 10, m_sY - 7` at 1901), and object/motion events are sent to clients within a range that differs per message: X +-10, Y +-8 plus
`sRange` (2886-2889, 3125-3128); X +-9, Y +-7 (2931-2934); strict `>`/`<` tests giving X +-9, Y +-6
(9515-9518). So X +-9..10 and Y +-6..8. hgserver-librehelbreath has the same init block
(`src/Game.cpp:2060`, 2271-2272, -10/-7, 21x16). The client mirrors this: `ShiftMapData` copies a 21x16 window
(MapData.cpp:806-860), the player sits at pivot+(19,17) (Game.cpp:45987-45988) and the view centre is tile
(10,7). A world view wider than 21 tiles (672 px) or taller than about 15 tiles (7 above, 7-8 below the
player, 480 px) shows cells the server never sent. See `change-plan.md`.

## Disagreements with existing docs

Source is ground truth; the docs named are not edited here.

| Doc | Says | Source shows |
|---|---|---|
| `..\rendering\frame-pipeline.md`:79 | DrawBackground re-renders "22x15 tiles" | 22 columns, and 15 **or 16** rows depending on the sub-tile offset: the row loop runs to `427+48` (play area), not 480 (Game.cpp:15854-15857). The 672x512 cache is therefore never filled below about y 464-496; the icon panel hides it |
| `..\rendering\directdraw.md`:58 | window moved up 40 px "if the screen is taller than 560" | the test is on `SM_CYFULLSCREEN`, the work area without the taskbar, halved: `cy > 280` (Wmain.cpp:326-328) |
| `..\rendering\directdraw.md`:153 | windowed present is a "stretch-free" Blt | it is a stretching `Blt` that happens to be 1:1 because `m_rcFlipping` is 640x480; a 1152x864 rectangle was tried and commented out (DXC_ddraw.cpp:223) |

No other size claim in `..\rendering\` or `..\functions\` disagrees with the source (checked by grep for
640/480/427/431/672/512/tile-count phrases).

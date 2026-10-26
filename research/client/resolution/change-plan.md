# Change plan: order and risk of replacing the hard-coded values

Proposal (*inferred*: nothing here was built or run). Row counts are measured (`method.md`).

## Contents
- Decide first: scale or enlarge
- Phase order
- Riskiest areas
- Bugs found on the way

## Decide first: scale or enlarge

1. **Scale the 640x480 frame** (integer or aspect-correct stretch). Touches only the present step and
   window size: the windowed path already presents with `Blt(&m_rcFlipping, ...)`, which stretches when
   the rectangle is larger (a 1152x864 attempt is commented at DXC_ddraw.cpp:223). The mouse is relative
   DirectInput clamped to 0-639/0-479 (DXC_dinput.cpp:82-88), so it needs no remapping. About 10 rows
   change. Gameplay and server untouched.
2. **Enlarge the logical screen** (more pixels of UI, same world). HUD, dialogs and menus re-anchor;
   the world view can grow only up to the server window (21x16 tiles = 672x512 px including the
   partial edge tiles) - in practice SCREEN_W <= 672 and VIEW_H <= about 480 without a server change.
3. **Enlarge the world view** beyond that. Requires the server to send a larger window
   (`iComposeInitMapData` / `iComposeMoveMapData`, the move-strip tables `_tmp_iMoveLocX/Y`, and every
   `-10/+10, -7/+7` event range in the server) **and** the client's `ShiftMapData`, `MAPDATASIZEX/Y`,
   `SRV_WIN_*` and `PLAYER_CELL_*` together. Protocol-visible: old and new clients cannot share a server.
   Out of scope for a client-only change (the server is another repository). Prior art: hgserver-librehelbreath
  carries a commented-out wider move-strip table (`_tmp_iMoveLocX/Y[9][55]`, src/Game.cpp:120-252) beside the
  live `[9][37]` tables (lines 23, 71).

## Phase order (for option 2 or 3)

| Phase | What | Rows | Risk | Check |
|---|---|---|---|---|
| 0 | Add a header with every variable in `variables.md` set to its 640x480 value; replace literals 1:1 | 1330 lines: 793 live in the shipped build (language 4), 312 only in other builds, 127 dead in every build, 98 commented (`method.md`, Build conditionals) | low, mechanical | build must be byte-identical or behaviour-identical at 640x480 |
| 1 | Platform: `SetDisplayMode` from config with mode enumeration and fallback, window size, back buffer and tile cache, mouse clamp, `ClearBackB4`, centring formula in one helper (3 live copies today: `bInit`, `ChangeDisplayMode`, Wmain.cpp:326-328; 2 more exist only in the language-2 HTML build) | 33 | medium: mode may not exist at 16 bpp on modern drivers; windowed on 32-bit desktop is already broken | start in both modes, Alt-Tab, ChangeDisplayMode toggle |
| 2 | Full-screen rects: shadow boxes, top bar, DrawLine clip, menu full-screen hit rects (incl. the 4 dead `_DrawBlackRect` rows) | 54 | low; `DrawShadowBox` is unclipped, so a wrong bound writes outside the buffer | fade-in, top message, menus |
| 3 | HUD: `HUD_X0/HUD_Y0` for icon/gauge panel, chat bar, lower event list, picking limit | 72 + picking | medium: the panel art is 640 wide, so wider screens need side fill; picking/drawing must agree on `VIEW_H` | click every icon, chat input, exp tooltip |
| 4 | World view: `VIEW_W/VIEW_H` in DrawObjects/DrawBackground/shadow clip, centre tile, listener, panning | 152 | **high**: see riskiest areas; centre values are server-coupled | walk to map edges, observer mode, weather, sound panning |
| 5 | Dialogs: anchor table, re-open clamp, guide map clamp/snap, item-drop clamp; add a drag clamp | 128 | medium: saved positions (if any) and dialogs opened relative to others (Game.cpp:18213, 18482, 18490) | open every dialog at both sizes |
| 6 | Menus: one `MENU_X0/MENU_Y0` offset per screen (draw and mouse) and fill outside the canvas | 891 rows, ~25 functions | low per screen, large volume; the `UpdateScreen_OnSelectCharacter(sX, sY, ...)` variant already takes an origin (Game.cpp:29437) | every login-flow screen in each language build |

## Riskiest areas

- **Tile cache under the HUD.** `DrawBackground` fills the 672x512 cache only down to `427+48`
  (Game.cpp:15854) but blits a full 640x480 window (15870). Rows below about y 464 are filled only
  for some sub-tile offsets (draw y is `iy-16+sModY`: for `sModY <= 5` the tile rows end at y 464 and rows
  464-479 of the 480-row blit are stale; for `sModY > 5` they end at y 496); the icon panel hides them today. Moving or shrinking the HUD exposes stale
  pixels unless the loop bound becomes `SCREEN_H + 48`.
- **Blitters with private bounds.** `PutShadowSprite` clips to 640x427 itself (Sprite.cpp:742, 760);
  `PutPixel`, `DrawLine`, `DrawLine2` clip to 640x480; `DrawShadowBox` does not clip at all
  (DXC_ddraw.cpp:528-579). A larger buffer with old bounds leaves shadows cut at x 640; a smaller one
  writes past the buffer.
- **Global clip rectangle.** `m_rcClipArea` is swapped to 672x512 and back around the cache render
  (Game.cpp:15852, 15867); any new size must update both the init (DXC_ddraw.cpp:60) and the restore.
- **Fixed-size buffers.** Back buffer 640x480 and tile cache 672x512 are allocated twice each (init and
  `ChangeDisplayMode`, DXC_ddraw.cpp:119/123 and 315/319); `ChangeDisplayMode` already leaks the old cache
  (`..\rendering\directdraw.md`). The client map window `m_pData[40][35]` (MapData.h:26-27) is
  protocol-sized, not screen-sized.
- **Server-coupled centre.** `SRV_CENTER_TX/TY` (10, 7; fixed constants, not derived from the screen) appear as literals in 43 centre-tile rows,
  in the player placement `pivot + 14 + 5` / `+ 12 + 5` (Game.cpp:45987-45988) and in the server's
  `-10/-7`. Moving the player off-centre client-side is safe only if those stay consistent.
- **Sound listener.** 54 rows use `320`/`240` as listener; `240` is the screen centre, not the player
  (224). A naive `VIEW_CENTER_Y` substitution changes volume/pan slightly.
- **Pitch is a `short`** (DXC_ddraw.cpp:131): fine to 16383 px wide, so not a practical limit.
- **16 bpp only.** Any new mode must be a 16-bit mode in fullscreen; modern drivers often emulate 16 bpp
  poorly. Not a size issue, but the first thing a resolution change will hit.

## Current behaviour: dialog and input behaviour found on the way (verified by reading, not run)

Entries live in [issues/client-resolution.md](../../issues/client-resolution.md).

- Observer panning tests `msX == 640` and `msY == 480` while the mouse clamps to 639/479, so observers pan only left and up (`Game.cpp:35718-35730`, [CLIENT-RESOLUTION-001](../../issues/client-resolution.md#client-resolution-001-observer-panning-right-and-bottom-never-fires)).
- Dragging a dialog is unclamped (`Game.cpp:35849-35850`, [CLIENT-RESOLUTION-002](../../issues/client-resolution.md#client-resolution-002-dialog-drag-is-unclamped)); only re-open clamps it (max clamps at 18576-18577; 18578-18579 push a dialog back by 20 when its right or bottom edge is above 10).
- The item-drop query is clamped to the full screen, so it may cover the HUD, and the clamp uses 235x100 while dialog 20 is 252x87 (`Game.cpp:4717-4768`, [CLIENT-RESOLUTION-003](../../issues/client-resolution.md#client-resolution-003-item-drop-query-is-clamped-to-the-full-screen), [CLIENT-RESOLUTION-004](../../issues/client-resolution.md#client-resolution-004-item-drop-clamp-uses-the-wrong-dialog-size)). `QUERY_W/H` in `variables.md` is the planned replacement.
- Default dialog rectangles: dialog 10 ends at y 435 (`Game.cpp:606-609`), dialog 22 at y 434 (`679-682`), dialog 29 at y 487 (`721-724`) ([CLIENT-RESOLUTION-005](../../issues/client-resolution.md#client-resolution-005-default-dialog-rectangles-overlap-the-icon-panel-or-leave-the-screen)).

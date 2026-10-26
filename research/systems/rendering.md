# System: client rendering

*Cohesive doc read as a whole picture; deliberately a summary that links the deep rendering docs
(`../client/rendering/`, `../client/resolution/`) instead of repeating them.*

Program: client only (`sources/client-2.20/`). Tags: [V] verified in source by this pass; [Vd] verified
by the linked doc's own source review, not re-read here; [I] inferred; [U] unverified.

## Contents
- 1 Purpose
- 2 Subsystems
- 3 Functions
- 4 Commands and messages
- 5 Data
- 6 Flow
- 7 Rules and invariants
- 8 Couplings
- 9 Intent vs implementation
- 10 Open questions
- Doc discrepancies

## 1 Purpose

Show a 640x480, 16-bit isometric-looking (reads as top-down 32x32 tile [I]) world with dozens of
layered, dyed, animated characters on 1999-2001 PCs with no 3D card and little RAM. The player sees
smooth walking, coloured armour, translucent spells, day/night and weather; the operator never
touches it. Every design choice serves "cheap on a Pentium II": all surfaces in system memory, CPU
blending through lookup tables, frames skipped when nothing moved, sprites decoded only when first
drawn and evicted after a minute [Vd `../client/rendering/blitters.md` "Summary"; I for the motive].

## 2 Subsystems

| Subsystem | Intent | Nests in |
|---|---|---|
| R1 Presentation (DirectDraw) | one 640x480x16 back buffer composed in system memory, flipped (fullscreen) or blitted (windowed) | root |
| R2 Frame pipeline | decide whether to draw at all, then paint world, effects, UI, cursor in a fixed order | uses R1, R3-R6 |
| R3 Sprite store (.pak) | 20000 lazy `CSprite` slots; load headers at start, pixels on first draw, evict after 60 s | feeds R4 |
| R4 Blitters | colour-keyed copy plus table-driven additive / 50-70-25% / shadow / tint / night routines | used by R2, R5, R6 |
| R5 World painter | background tile cache, row-by-row painter's order of items, corpses, characters, objects, trees; mouse picking while painting | inside R2 |
| R6 Effects and weather | spell/projectile/explosion effects, light pools, rain/snow, night tint, camera shake | inside R2 |
| R7 Text | GDI text on the back buffer plus three sprite fonts | used by R2 and the UI system |
| R8 Resolution constraints | 640x480 baked into ~thousands of literals and the server view window | cross-cutting |

## 3 Functions

Only the anchor functions; full indexes are `../client/functions/rendering.md` and
`../client/functions/characters.md`.

| Sub | Function | Where | Role |
|---|---|---|---|
| R1 | `DXC_ddraw::bInit` | DXC_ddraw.cpp:54-190 | create DD7, mode 640x480x16, back buffer and 672x512 background cache in system memory, blend tables, GDI font [Vd] |
| R1 | `DXC_ddraw::iFlip` | DXC_ddraw.cpp:192-242 | present; reports `DDERR_SURFACELOST` [Vd] |
| R1 | `DXC_ddraw::ChangeDisplayMode` | DXC_ddraw.cpp:244-328 | windowed/fullscreen toggle from `OnSysKeyUp` [Vd] |
| R1 | `CGame::RestoreSprites` | Game.cpp:1330-1334 | rebuild lost sprite surfaces [Vd] |
| R2 | `EventLoop` | Wmain.cpp:373-386 | busy PeekMessage loop, idle -> `UpdateScreen` [Vd] |
| R2 | `CGame::UpdateScreen` | Game.cpp:1106-1235 | sets `G_dwGlobalTime`, dispatches by game mode [Vd] |
| R2 | `CGame::UpdateScreen_OnGame` | Game.cpp:34802-35456 | the in-game frame [Vd; draw/skip gate V at 35106-35404 via doc] |
| R2 | `CMapData::iObjectFrameCounter` | MapData.cpp:1711-3381 | advance every animation; its return decides next frame's redraw [V start line] |
| R3 | `CSprite::CSprite` / `_iOpenSprite` / `_iCloseSprite` | Sprite.cpp:26-58, 2347-2366, 2368-2377 | lazy load and free [Vd] |
| R3 | `CMyDib::CMyDib` | Mydib.cpp:9-36 | read one BMP out of `sprites\%s.pak` [Vd] |
| R3 | `CGame::ReleaseUnusedSprites` | Game.cpp:15618-15660 | evict sprites idle > 60 s, sounds idle > 30 s [V 15628-15657] |
| R3 | `CGame::UpdateScreen_OnLoading` | Game.cpp:3596-4253 | create all sprite objects in 26 steps, sounds at the end [V 4222-4250 for sounds] |
| R4 | `CSprite::Put*` (25 routines) | Sprite.cpp:100-3259 | see `../client/rendering/blitters.md` "Routine reference" |
| R4 | `CSprite::_SetAlphaDegree` | Sprite.cpp:2840-2931 | in-place night tint of a sheet [Vd] |
| R5 | `CGame::DrawBackground` | Game.cpp:15841-15878 | re-render tiles into the 672x512 cache only on tile-boundary crossing [Vd] |
| R5 | `CGame::DrawObjects` | Game.cpp:2278-2933 | painter's order per cell, picking, cursor frame choice [V cursor lines 2297-2930] |
| R5 | `CGame::DrawObject_On<Action>` | Game.cpp:8691-14xxx | compose body + 10 equipment layers per action [Vd `../client/functions/characters.md`] |
| R5 | `CGame::CalcViewPoint` | Game.cpp:1238-1274 | camera easing toward the player [Vd] |
| R6 | `CGame::DrawEffectLights` / `DrawEffects` | Game.cpp:7762-7937 / 6954-7732 | light pools under, effect sprites over objects [Vd] |
| R6 | `CGame::bAddNewEffect` | Game.cpp:5891-6953 | create an effect; also plays its sound and camera shake [V 5995-6010] |
| R6 | `CGame::DrawWhetherEffects` / `SetWhetherStatus` | Game.cpp:23085-23172 / 23258 | rain/snow particles and rain loop sound [V 23265, 23283] |
| R7 | `CGame::PutString*`, `PutString_SprFont*`, `PutString_SprNum` | Game.cpp:8125-8324 | GDI and sprite-font text [Vd] |

## 4 Commands and messages

Rendering sends nothing. It reacts to state written by message handlers (`client-state.md`):
- `DEF_NOTIFY_TIMECHANGE` -> `NotifyMsg_TimeChange` sets `G_cSpriteAlphaDegree` (night) and forces a
  background redraw (Game.cpp:25166-25181) [Vd `../client/functions/networking.md`].
- `DEF_NOTIFY_WHETHERCHANGE` -> `SetWhetherStatus` (Game.cpp:25152-25164) [Vd].
- `MSGID_EVENT_MOTION`, `MSGID_EVENT_LOG`, `MSGID_DYNAMICOBJECT`, `MSGID_EVENT_COMMON` (effects,
  ground items) fill `CMapData`/`m_pEffectList`, which the painter reads next frame [V dispatch
  Game.cpp:2943-2993].
- Local slash commands `/showframe`, `/enabletogglescreen` (ui-only in
  `commands/by-system.md`) toggle the FPS text and the windowed toggle [Vd].

## 5 Data

- `DXC_ddraw m_DDraw` (Game.h:637): `m_lpBackB4` (640x480 sysmem), `m_lpPDBGS` (672x512 background
  cache), `m_pBackB4Addr`/`m_sBackB4Pitch` retained pointer, `m_rcClipArea`, blend tables
  `m_lTrans*[64][64]` mirrored in globals `G_lTrans*` (Wmain.cpp:40-44) [Vd `directdraw.md`].
- `m_pSprite[20000]`, `m_pTileSpr[500]`, `m_pEffectSpr[100]` (Game.h:69-71, 640-642) [Vd].
- `CMapData::m_stFrame[80][15]` frame count and frame time per owner type and action
  (MapData.h:71) [Vd]; `CTile` live/dead owner, item, dynamic object, effect fields (Tile.h:24-71).
- `m_cDetailLevel` 0-2 (cycled by a key in `OnKeyUp`, Game.cpp:28640-28642) [V], `m_iCameraShakingDegree`,
  `m_cWhetherEffectType`, `G_cSpriteAlphaDegree`.
- Files: `sprites\*.pak` (format: `../shared/formats/` and the user's PAKLib; the client reading
  only the fields it needs is by design), `mapdata\*.amd` tiles (`../client/functions/world-map.md`).
  No .cfg of its own.

## 6 Flow

**One gameplay frame** (detail: `../client/rendering/frame-pipeline.md` "One in-game frame"):
1. `EventLoop` finds the queue empty -> `UpdateScreen` -> `UpdateScreen_OnGame` (Wmain.cpp:373-386;
   Game.cpp:1106) [Vd].
2. If last frame's `iUpdateRet == 0`: draw only cursor and top message, no flip [Vd].
3. Else: `DrawBackground` -> `DrawEffectLights` -> `DrawObjects` -> `DrawEffects` ->
   `DrawWhetherEffects` -> `DrawChatMsgs` -> `DrawDialogBoxs` -> chat bar -> `ShowEventList` ->
   dragged item -> `DrawTopMsg` -> fade-in -> cursor -> `iFlip` [Vd].
4. After the flip: `iObjectFrameCounter` (also fires footstep / monster sounds, see `audio.md`),
   `bEffectFrameCounter`, then `CommandProcessor` with the hit-test results from this frame's
   painting (UI system) [Vd; sound V MapData.cpp:2066].

**Sprite first use**: draw call -> `_iOpenSprite` -> `CMyDib` reads the BMP from the pak ->
`pCreateOffScreenSurface` (sysmem) -> colour key -> optional night tint -> blit; 60 s idle ->
`ReleaseUnusedSprites` frees it (Game.cpp:15628) [V 15628; rest Vd].

**Day/night change**: server notify -> `G_cSpriteAlphaDegree` changes -> every `bAlphaEffect` sheet
re-tints (reloads) on its next draw [Vd `blitters.md` "Night"].

## 7 Rules and invariants

- Resolution 640x480, 16 bpp; fullscreen release default, windowed in `_DEBUG` (DXC_ddraw.cpp:34-38) [Vd].
- Every off-screen surface is `DDSCAPS_SYSTEMMEMORY`, width rounded up to a multiple of 4
  (DXC_ddraw.cpp:330-344) [Vd].
- Redraw only when some object/effect advanced a frame; animation times per type and action,
  defaults 60/70/42/78/88/150/70 ms (MapData.cpp:25-41); up to 3 frames skipped to keep game time
  (MapData.cpp:1927-1932) [Vd].
- Sprite eviction 60 s, sound eviction 30 s if not looping, swept every 8 s (Game.cpp:15628-15657) [V]; the sweep runs only while the window is active
  (`if (m_bIsProgramActive) ReleaseUnusedSprites()`, `Game.cpp:4270-4275`), so eviction pauses when minimised [V].
- Painter's window: the loop starts 7 rows above the top screen row (`indexY = sDivY + sPivotY - 7`) and runs `iy`
  from `-sModY-224` to 779 in steps of 32, about 32 rows (roughly 9-10 rows below the 15-row screen); columns -4..+4
  beyond the 20-column screen (Game.cpp:2299-2303) [Vd].
- Mouse picking only above y = 431 (the HUD line) [Vd].
- Invisible units (status 0x10) drawn 25% for self/allies, skipped for enemies (Game.cpp:13769-13774) [Vd].
- The server only sends objects inside a fixed window, so the visible area is not purely a client
  choice (`../client/resolution/window-setup.md`) [Vd].

## 8 Couplings

| Direction | Coupling point | Other system |
|---|---|---|
| out | `CommandProcessor` consumes `m_sMCX/m_sMCY`, `m_cMCName`, `sFocus*` set by `DrawObjects` | `ui.md` (input) |
| out | `iObjectFrameCounter` calls `PlaySound` on animation frames | `audio.md` |
| in | `CMapData` cells, `m_pEffectList`, player appearance fields written by handlers | `client-state.md` |
| in | appearance words `sAppr1-4`, `iApprColor`, status bits (invisibility 0x10, weapon speed `& 0xF`) | server item/equip and magic systems: `item.md`, `magic.md` (other clusters) [U path] |
| in | day/night and weather notifies | server `map.md` (other cluster) [U path] |
| in | crusade structure markers drawn by `DrawBackground` | `crusade.md` (other cluster) [U path] |
| in | `_iGetFOE` friend/enemy colouring from status/nation bits | `pk-law.md`, `guild.md` (other clusters) [U path] |
| both | `DrawDialogBoxs` is called inside the frame; dialogs draw with the same blitters | `ui.md` |

## 9 Current behaviour vs intent

- The FPS display is `count >> 7`, +6 if under 10 ([RENDER-001](../issues/rendering.md#render-001-fps-display-is-not-a-real-frame-rate)).
- Guide-map full mode divides both axes by map width ([RENDER-002](../issues/rendering.md#render-002-guide-map-full-mode-divides-both-axes-by-map-width)).
- The energy-ball pak name `"Effect5.pak"` gets a second `.pak` appended and 40 slots are built from one entry ([RENDER-003](../issues/rendering.md#render-003-energy-ball-sprite-pak-name-is-wrong)).
- `PutTransSprite` ignores `alphaDepth`; `PutRevTransSprite` indexes past row 63 ([RENDER-004](../issues/rendering.md#render-004-puttranssprite-ignores-alphadepth), [RENDER-005](../issues/rendering.md#render-005-putrevtranssprite-indexes-past-row-63)).
- BGR565 is detected but not handled; 32-bit desktop windowed mode is wrong ([RENDER-007](../issues/rendering.md#render-007-bgr565-and-32-bit-desktops-render-wrongly)).
- New sprites are decoded on first draw on the render thread ([RENDER-009](../issues/rendering.md#render-009-first-draw-decoding-hitches-on-the-render-thread)).
- The pixel-based sound pan in `bAddNewEffect` saturates ([AUDIO-001](../issues/audio.md#audio-001-pixel-based-sound-pan-saturates)).
- Full list with replicate/fix advice: `../client/rendering/porting-notes.md` "Current behaviour: bugs to replicate or fix".

## 10 Open questions

- Whether retail 2.20 binaries used the same `MMXCPY.DLL`/`Dib16.dll` paths: the source never loads
  them (`blitters.md` "Helper DLLs") [U] - needs a binary diff.
- Exact draw order inside `DrawObject_OnStop` past Game.cpp:14050 is inferred by the existing doc [I].

## Doc discrepancies

- `../client/rendering/frame-pipeline.md` step 5.8 says the gauge and icon panels sit at "the bottom
  slots 39-40". In source they are the last two entries of `m_cDialogBoxOrder` (Game.cpp:949-950),
  which `DrawDialogBoxs` draws last (Game.cpp:18024) and `_iCheckDlgBoxFocus` hit-tests first
  (Game.cpp:17455-17456): they are on **top** of every dialog. "Bottom" is only true of array
  position. See `ui.md`.

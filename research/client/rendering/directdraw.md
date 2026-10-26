# DirectDraw layer (DXC_ddraw)

How the 2.20 client talks to DirectDraw 7: surfaces, display mode, pixel format, flip/blit,
clipping, Lock handling and lost-surface recovery. Source: `sources\client-2.20\DXC_ddraw.cpp` /
`.h` (verified unless marked *inferred*). Per-pixel routines are in `blitters.md`; the per-frame
order is in `frame-pipeline.md`.

## Contents
- Class layout
- Initialisation (`bInit`)
- Surfaces
- Display mode and windowed vs fullscreen
- Pixel format detection and colour storage
- Presenting a frame (`iFlip`)
- Clipping
- Lock/Unlock and pitch: the retained-pointer trick
- Lost-surface recovery
- Text through GDI
- Screenshots
- Other members
- Known defects

## Class layout

`DXC_ddraw` (DXC_ddraw.h:21-75) is a plain struct-like class embedded by value in `CGame`
(`m_DDraw`, Game.h:637). Members that matter:

| Member | Meaning |
|---|---|
| `m_lpDD4` | `IDirectDraw7*` (named "4" from an older DX6 version) |
| `m_lpFrontB4` | primary surface |
| `m_lpBackB4flip` | the flip-chain back buffer (fullscreen only) |
| `m_lpBackB4` | **the real compositing buffer**: a 640x480 off-screen surface in system memory |
| `m_lpPDBGS` | "Pre-Draw Background Surface": a 672x512 system-memory cache of the tile layer |
| `m_pBackB4Addr`, `m_sBackB4Pitch` | raw `WORD*` into `m_lpBackB4` and its pitch in pixels (not bytes) |
| `m_rcClipArea` | the software clip rectangle used by every `CSprite::Put*` routine |
| `m_rcFlipping` | source rect for fullscreen flip / destination rect on the desktop in windowed mode |
| `m_cPixelFormat` | 0 unknown, 1 RGB565, 2 RGB555, 3 BGR565 (detected but never handled) |
| `m_lTrans{G,RB}{100,70,50,25,2}[64][64]`, `m_lFade{G,RB}[64][64]` | blend tables (see `blitters.md`) |
| `m_bFullMode` | TRUE fullscreen (release default), FALSE windowed (`_DEBUG` default) (DXC_ddraw.cpp:34-38) |
| `m_hDC`, `m_hFontInUse` | GDI DC of the back buffer while text is drawn, and the per-language font |

The operator `new`/`delete` overrides (HeapAlloc with `HEAP_ZERO_MEMORY`) appear on almost every
class in the client; they zero-initialise objects, which the code relies on (*inferred*).

## Initialisation (`bInit`, DXC_ddraw.cpp:54-190)

Called once from `CGame::bInit` (Game.cpp:903) after the content files are read and before any
sprite exists.

1. `m_rcClipArea = {0,0,640,480}` (line 60).
2. `DirectDrawCreateEx(NULL, &m_lpDD4, IID_IDirectDraw7)` (line 62). Any failure shows
   "This program requires DirectX7.0a!" (Game.cpp:904).
3. Fullscreen: `SetCooperativeLevel(EXCLUSIVE|FULLSCREEN)`, `SetDisplayMode(640,480,16)`, a
   primary with `DDSCAPS_FLIP|COMPLEX` and **one** back buffer (comment shows it was 2 before
   v1.3), then `GetAttachedSurface` for `m_lpBackB4flip` (lines 65-89).
4. Windowed: `SetCooperativeLevel(NORMAL)`, the 640x480 popup window is centred (`cy -= 40` if
   the screen is taller than 560), a plain primary is created, and `m_rcFlipping` becomes the
   window's screen rectangle (lines 90-115).
5. `InitFlipToGDI` attaches a clipper to the primary (line 117, see Clipping).
6. `m_lpBackB4 = pCreateOffScreenSurface(640,480)` and `m_lpPDBGS = pCreateOffScreenSurface(672,512)`
   (lines 119-124).
7. Locks the back buffer once, stores `m_pBackB4Addr` and `m_sBackB4Pitch = lPitch >> 1`, unlocks
   (lines 127-131). See "retained-pointer trick".
8. `_TestPixelFormat()` then fills all blend tables in a 64x64 double loop, twice: once into the
   members and once into the `G_lTrans*` globals defined in Wmain.cpp:40-44 (lines 134-169). The
   globals exist so `CSprite` can reach them without a `DXC_ddraw` pointer (Wmain.cpp:37 comment:
   "global for speed").
9. Creates the per-language GDI font (lines 171-186): Taiwan MingLiu 12, China MingLiu 12 (GB2312
   charset), Korea "바탕" (Batang) 12, **English Comic Sans MS 16**, Japan MS PGothic 12. All
   `NONANTIALIASED_QUALITY`, which matters on a 16-bit surface (*inferred*: anti-aliased text
   would be blended against the colour key).

## Surfaces

`pCreateOffScreenSurface` (DXC_ddraw.cpp:330-344) is the only surface factory besides the primary:

```cpp
if ((wSzX % 4) != 0) wSzX += 4 - (wSzX % 4);           // width rounded up to a multiple of 4
ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
```

- **Every** off-screen surface is in system memory: the back buffer, the background cache and
  every sprite sheet (Sprite.cpp:79 calls the same factory). Nothing is placed in video memory.
  This is deliberate: the client's blending is done by the CPU with direct pointer writes, and
  CPU reads from video memory on 2001 AGP cards were extremely slow (*inferred* rationale).
- Consequence: `BltFast` from a sprite to the back buffer is a system-to-system blit done by the
  DirectDraw HEL (software emulation), i.e. a colour-keyed `memcpy`. Only the final upload in
  `iFlip` crosses to video memory (*inferred* from the caps).
- No pixel format is requested, so the surface takes the primary's format: 16-bit in fullscreen,
  **whatever the desktop is** in windowed mode (see Known defects).
- Width padding to 4 pixels keeps rows 8-byte aligned (*inferred* intent).

## Display mode and windowed vs fullscreen

- Fixed 640x480x16. There is no mode enumeration and no other resolution.
- Toggle: `ChangeDisplayMode` (DXC_ddraw.cpp:244-328) releases the flip surfaces and primary,
  restores the display mode, and rebuilds in the other mode. It is bound to a key in
  `CGame::OnKeyUp` when `m_bToggleScreen` is TRUE (Game.cpp:28592-28595); `m_bToggleScreen` is
  TRUE only in `_DEBUG` builds (Game.cpp:446-450) or after a hidden toggle (Game.cpp:34021,
  *inferred* to be a chat or key command; see `functions\input.md`).
- The window itself is a `WS_POPUP` 640x480 with no frame, class name `Client-I<hInstance>`,
  title "Helbreath Crusade" (Wmain.cpp:275, 330). `WM_SETCURSOR` hides the Windows cursor; the
  game draws its own sprite cursor (Wmain.cpp:213-215).

## Pixel format detection and colour storage

`_TestPixelFormat` (DXC_ddraw.cpp:425-450) reads the **back buffer's** red mask:

| dwRBitMask | m_cPixelFormat | Layout |
|---|---|---|
| 0xF800 | 1 | RGB 5:6:5 (R 15-11, G 10-5, B 4-0) |
| 0x7C00 | 2 | RGB 5:5:5 (bit 15 unused) |
| 0x001F | 3 | BGR 5:6:5 - detected, **no code path handles it** |
| other | 0 | e.g. a 32-bit desktop in windowed mode - nothing handles it |

Colours are never stored as COLORREF in hot paths. They are split into per-channel integers in the
native bit depth:

- `CMisc::ColorTransfer(fmt, COLORREF, &R,&G,&B)` (Misc.cpp:214-251) packs to 16 bits and unpacks:
  565 gives R 0-31, G 0-63, B 0-31; 555 gives all 0-31.
- `DXC_ddraw::ColorTransferRGB` (DXC_ddraw.cpp:657-686) does the same directly from the COLORREF
  bytes.
- `CGame::bInit` precomputes the two dye palettes this way (Game.cpp:960-988): `m_wR/G/B[16]` for
  cloth/armour and `m_wWR/WG/WB[16]` for weapons/shields. Entry 0 of `m_wR` is RGB(100,100,100),
  the neutral grey that sprite art is drawn in; dyes are applied as the *difference*
  `m_wR[c] - m_wR[0]` (see `blitters.md`, PutSpriteRGB).
- `PutPixel` (DXC_ddraw.cpp:581-599) takes 8-bit R,G,B and truncates (`>>3`, `>>2`).

Because channel values are format-native, a G delta of `-3` means 3/63 in 565 but 3/31 in 555:
the same code gives visibly different results per format (see `porting-notes.md`).

## Presenting a frame (`iFlip`, DXC_ddraw.cpp:192-242)

Fullscreen, normal build (lines 215-216):

```cpp
ddVal = m_lpBackB4flip->BltFast(0, 0, m_lpBackB4, &m_rcFlipping, DDBLTFAST_NOCOLORKEY);
ddVal = m_lpFrontB4->Flip(m_lpBackB4flip, DDFLIP_WAIT);
```

So a frame is: compose in system memory, copy the whole 640x480 to the video-memory back buffer
(no `DDBLTFAST_WAIT`, it was commented out), then page-flip with vsync wait. The flip chain is
used only to avoid tearing; nothing is ever drawn into `m_lpBackB4flip` directly.

Variants:
- `DEF_USING_WIN_IME` (Chinese/Taiwanese/Japanese IME builds, see `..\build-config.md`): instead of
  flipping, `Blt` straight to the primary (unconditional, no flip chain) and, while the IME edit
  window `G_hEditWnd` exists and the Blt did not return `DDERR_SURFACELOST`, call
  `FlipToGDISurface()` so the Windows IME composition window stays visible (lines 198-202).
- `DEF_HTMLCOMMOM` (web dialog builds): same trick while the HTML dialog `G_hInternetWnd` is open
  (lines 205-213).
- Windowed: one stretch-free `Blt(&m_rcFlipping, m_lpBackB4, NULL, DDBLT_WAIT)` to the primary
  (line 224); the clipper keeps it inside the window.

There is no dirty-rectangle scheme at this level: the full frame is always uploaded. Frame
skipping happens one level up (`frame-pipeline.md`: `iUpdateRet == 0` skips drawing and flipping).

## Clipping

Two separate mechanisms:

1. **DirectDraw clipper** on the primary only (`InitFlipToGDI`, DXC_ddraw.cpp:620-655). It is
   created only if the driver reports `DDCAPS2_CANRENDERWINDOWED`; otherwise the function returns
   `E_FAIL` silently and the game runs without one. The clipper is released immediately after
   `SetClipper` (the surface keeps the reference). It serves GDI windows over the game (IME,
   message boxes) and windowed mode.
2. **Software clip rectangle** `m_rcClipArea`, applied by hand in every `CSprite::Put*` routine
   before any blit (e.g. Sprite.cpp:120-156). `BltFast` itself never clips, so this is mandatory.
   The rectangle is changed temporarily: `DrawBackground` sets it to 672x512 while it renders
   tiles into the cache surface and restores 640x480 afterwards (Game.cpp:15852, 15867). Note the
   clip rectangle has no notion of the destination surface; it is simply global state.

Two routines bypass it: `PutShadowSprite` clips per pixel to x 0-639, **y 0-426** (the play area
above the 53-pixel icon panel, Sprite.cpp:742) and `PutShadowSpriteClip` does clip against
`m_rcClipArea` like the other routines (Sprite.cpp:796-831) and then adds a per-pixel left test
(Sprite.cpp:848).

## Lock/Unlock and pitch: the retained-pointer trick

This is the most important hack in the renderer.

```cpp
if (m_lpBackB4->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL) != DD_OK) return FALSE;
m_pBackB4Addr  = (WORD *)ddsd.lpSurface;
m_sBackB4Pitch = (short)ddsd.lPitch >> 1;
m_lpBackB4->Unlock(NULL);                          // DXC_ddraw.cpp:128-131
```

The surface is locked **once**, the pointer is kept, and the surface is unlocked. From then on
every software blitter writes through `m_pBackB4Addr` with no Lock. Sprites do the same with
`m_pSurfaceAddr` (Sprite.cpp:2358-2361). This is legal by no DirectDraw contract; it works
because system-memory surfaces are ordinary heap blocks that DirectDraw never moves (*inferred*).
It saves a Lock/Unlock pair per sprite per frame (hundreds per frame).

Corollaries a port must know:
- The pitch is stored in pixels (`lPitch >> 1`), and as a `short`.
- The pointer is re-fetched only after `Restore()` on surface loss (DXC_ddraw.cpp:235-237) and in
  `CSprite::iRestore` (Sprite.cpp:692-694). After `ChangeDisplayMode` the new back buffer's
  pointer is re-fetched (lines 324-327), but sprite pointers are untouched (they are system
  memory and survive).
- Mixing `BltFast` (DirectDraw HEL) and raw writes on the same unlocked surface works only because
  both are synchronous CPU operations in system memory.
- `CSprite::_pMakeSpriteSurface` reads the colour key from the pixel at (0,0) after a Lock/Unlock
  (Sprite.cpp:86-92) - it reads `ddsd.lpSurface` *after* `Unlock`, the same assumption.

## Lost-surface recovery

- `iFlip` checks for `DDERR_SURFACELOST` (only the result of the last call, the Flip or the Blt)
  and then calls `Restore()` on front and back, re-fetches `m_pBackB4Addr`, and returns
  `DDERR_SURFACELOST` (DXC_ddraw.cpp:227-240).
- `UpdateScreen_OnGame` reacts with `RestoreSprites()` (Game.cpp:35403), which calls
  `CSprite::iRestore` on all 20000 sprite slots (Game.cpp:1330-1334). `iRestore` returns early
  unless the sheet is loaded and `IsLost()`; otherwise it restores, re-fetches the pointer and
  re-paints the bitmap from the .pak (Sprite.cpp:677-701). System-memory surfaces are not
  normally lost, so this path is mostly defensive (*inferred*).
- Tile and effect sprite arrays (`m_pTileSpr`, `m_pEffectSpr`) are not restored, nor is
  `m_lpPDBGS`; `m_lpBackB4flip` is not restored explicitly (it is restored with its primary).
- Alt-Tab: `WM_ACTIVATEAPP` only toggles `m_bIsProgramActive` and DirectInput acquisition and
  re-runs `bCheckImportantFile` (Wmain.cpp:123-158), which only checks that `CONTENTS\badword.txt`
  exists (Game.cpp:8500-8518; the TREES1.PAK size check is commented out; the error text says
  "checksum" but nothing is summed; `_iGetFileCheckSum` has no caller). While inactive the
  main loop stops drawing (Wmain.cpp:382-384), so the first flip after return hits the lost
  primary and triggers the recovery above.

## Text through GDI

`_GetBackBufferDC` (DXC_ddraw.cpp:601-607) calls `m_lpBackB4->GetDC`, selects the language font
and sets transparent background mode; `_ReleaseBackBufferDC` releases it. `TextOut` and `DrawText`
are thin wrappers. Every `CGame::PutString*` call that is not told the DC is already held pays a
full GetDC/ReleaseDC (Game.cpp:8261, 8302) - the expensive part of text drawing. Callers that print
many lines (`DrawChatMsgs`, dialog bodies) take the DC once and pass `bIsPreDC = TRUE`. Details in
`frame-pipeline.md` (Text).

## Screenshots

`Screenshot(filename, surface)` (DXC_ddraw.cpp:690-837) is a textbook GetDC + `BitBlt` to a
compatible bitmap + `GetDIBits` writer; the caller passes the back buffer and the name
`Save\HelShot%03d.bmp` (Game.cpp:25593-25598, `CGame::CreateScreenShot`). Output is a
`BI_BITFIELDS` 16-bit BMP (*inferred* from GDI behaviour on a 16-bit DC).

## Other members

- `ClearBackB4` (lines 519-526): Lock, `memset` 0 over `lPitch*480` bytes, Unlock.
- `DrawShadowBox(x1,y1,x2,y2,type)` (lines 528-579): type 0 halves every pixel in the box with
  `(p & 0xF7DE) >> 1` (565) / `(p & 0x7BDE) >> 1` (555) - a 50% darken by mask-and-shift; types 1
  and 2 fill with fixed dark greys (0x38E7/0x1CE7 and 0x1863/0x0C63). Used for chat input bars,
  the fade-in on entering the game and dialog backgrounds. No clipping: callers must stay on
  screen.
- `iSetColorKey` / `_dwColorMatch` (lines 346-417): the textbook DirectDraw colour-match helper
  (GetPixel/SetPixel round-trip) and a WORD variant. Used once per sprite sheet load
  (Sprite.cpp:2353).

## Current behaviour: known defects

Entries live in [issues/rendering.md](../../issues/rendering.md).

- With `m_cPixelFormat` 3 (BGR565) or 0 the software blitters fall through and `BltFast` sprites still draw; windowed mode on a 32-bit desktop uses wrong `WORD*` arithmetic ([RENDER-007](../../issues/rendering.md#render-007-bgr565-and-32-bit-desktops-render-wrongly)).
- `_dwColorMatch(WORD)` writes the key as a `DWORD` to the first pixel (lines 409-411), also zeroing pixel (1,0) of every sheet ([RENDER-008](../../issues/rendering.md#render-008-pixel-10-zeroed-in-every-sprite-sheet)).
- `ChangeDisplayMode` does not free the old `m_lpPDBGS`, and `m_lpBackB4` is released without being set to NULL first (lines 249-320, [RENDER-010](../../issues/rendering.md#render-010-changedisplaymode-leaks-and-can-leave-dangling-pointers)).
- `iFlip` ignores the `BltFast` result ([RENDER-011](../../issues/rendering.md#render-011-iflip-ignores-the-bltfast-result)).
- In fullscreen without `DDCAPS2_CANRENDERWINDOWED`, GDI dialogs (MessageBox) are invisible ([RENDER-012](../../issues/rendering.md#render-012-gdi-dialogs-invisible-in-some-fullscreen-modes)).

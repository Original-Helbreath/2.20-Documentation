# Porting notes: rendering

What a modern port (32-bit RGBA, GPU or SDL) must reproduce for the 2.20 client to look the same,
and which tricks can be dropped. Each rule cites the code it comes from (verified unless marked
*inferred*); background in `directdraw.md`, `blitters.md`, `frame-pipeline.md`.

## Contents
- Decide the reference format first
- Must replicate exactly
- Replicate approximately (or accept a visible change)
- Obsolete: drop these
- Bugs: replicate or fix, but decide
- A minimal port design

## Decide the reference format first

Every blend works on quantised 5- or 6-bit channels, and some maths differs between 565 and 555.
Pick one as the reference look. 565 is the likely common case on 2001 hardware (*inferred*;
most cards of the time exposed 565 at 16 bpp). A faithful port renders into a 16-bit 565 buffer
in software exactly like the original and converts to 32-bit only at present time; a
"modernised" port uses 8-bit channels and accepts small differences. The rules below give the
exact maths so either choice is deliberate.

## Must replicate exactly

1. **Additive "transparency".** Every `PutTransSprite*` is `dst + src*a`, clamped per channel,
   with a in {1.0, 0.7, 0.5, 0.25}; src*a is truncated toward zero in channel units
   (`_CalcMaxValue`, DXC_ddraw.cpp:452-464). It is not alpha blending. On a GPU: blend
   `ONE, ONE` with the source pre-scaled by a, or `SRC_ALPHA, ONE` with constant alpha a.
2. **Real 50% only where `PutTransSprite2` is used**: `(s + d) / 2` per channel, integer
   division (DXC_ddraw.cpp:466-468). Used for translucent dialogs and trees hiding the player.
3. **Subtractive effects** (`PutRevTransSprite`): `max(0, d - (s + depth))` per channel, depth
   doubled on G in 565 only (Sprite.cpp:3224-3226, 3244-3246).
4. **Darkening by mask-and-shift**: shadows and "fade" use `(p & 0xE79C) >> 2` in 565, i.e. each
   channel becomes `floor(c / 4)` (R and B 0-7, G 0-15); `DrawShadowBox` uses `(p & 0xF7DE) >> 1`,
   each channel `floor(c / 2)`. In 8-bit terms: multiply by 0.25 or 0.5 after truncation to the
   16-bit grid.
5. **Procedural shadows**: take every third source row, squash to 1/3 height, shear left by one
   pixel per destination row, anchor at the sprite bottom, darken to 1/4, clip to y < 427
   (`PutShadowSprite`, Sprite.cpp:734-749). Overlap darkens repeatedly. There is no shadow art for
   characters.
6. **Dye tinting = channel offset, not multiply.** `PutSpriteRGB` writes
   `clamp(src + (dye - grey), 1, max)` per channel, where grey is `m_wR[0]` = RGB(100,100,100)
   converted to channel units and the dyes are the 16-entry tables at Game.cpp:960-988. The floor
   is 1, not 0 (Wmain.cpp:863, 868). Computing this in 8-bit space gives slightly different results
   because the offsets are quantised to 5/6 bits first; for an exact match quantise the dye
   difference the same way.
7. **Glare and tinted additive**: `PutTransSpriteRGB` = `max(d, min(max, d + s + delta))`
   (G_iAddTransTable, Wmain.cpp:872-878, used at Sprite.cpp:2614). The weapon glare delta pulses
   0..25..0 in channel units, stepped once per player animation frame (Game.cpp:35439-35454).
8. **Night tint**: R-3, G-3, B+2 in native channel units on tiles, objects, characters and
   monsters only, never on UI or effects (`_SetAlphaDegree`, Sprite.cpp:2858-2862). In 565 the G
   change is -3/63, in 555 -3/31: **565 nights are less green-shifted than 555 nights**. Lights
   are drawn additively on top; there is no darkness overlay. A shader-based port should apply the
   tint per sprite (a uniform), not as a full-screen pass, or the UI will darken.
9. **Pivot and frame rectangles** from the .pak (`dX = x + pivotX`), the 32-pixel tile grid, and
   the view-point easing in `CalcViewPoint` (Game.cpp:1238-1274), including the 4-pixel step cap.
10. **Draw order** per cell as in `frame-pipeline.md` (item, corpse, object, map object, tree),
    direction-dependent weapon and mantle order (`_cDrawingOrder`, `_cMantleDrawingOrder`,
    Game.cpp:57-59), lights before objects, effects after.
11. **Tree translucency rule**: only when the player's body rectangle lies completely inside the
    tree's rectangle and detail level is 2 (Game.cpp:2654-2655).
12. **Pixel-exact picking**: the 7x7 non-key test around the cursor (`_bCheckCollison`,
    Sprite.cpp:3018-3028) and "last drawn wins". Bounding-box picking changes which target is
    clicked in crowds.
13. **Animation timing** per type and action from `CMapData::m_stFrame`, with up to 3 skipped
    frames when late (MapData.cpp:1927-1932). The look of movement depends on it more than on the
    frame rate.
14. **Colour key = the converted (0,0) pixel** of each image. After conversion to 32-bit, use an
    exact match of the 16-bit value (or of the original palette index), not of the 24-bit colour:
    two palette colours that collapse to the same 16-bit value both become transparent in the
    original (*inferred* consequence of GDI conversion before the key is read).

## Replicate approximately (or accept a visible change)

- **GDI text**. The English build uses Comic Sans MS 16 non-antialiased with a 3-pass RGB(5,5,5)
  drop outline (Game.cpp:8268-8274). Use a bitmap font rasterised from Comic Sans 16 without
  smoothing, or accept the change. CJK builds need their 12-pixel fonts.
- **The fade-in**: two or one 50% darkenings over the first 6 frames (Game.cpp:35374-35375), so
  the timing depends on frame count, not time.
- **The FPS counter** is massaged (+6 under 10, Game.cpp:35393); show a real one instead.
- **Rain/snow particle counts** (120/300/600) and sprites are easy to keep exact.

## Obsolete: drop these

- System-memory surfaces, retained Lock pointers, `BltFast` vs software paths, the
  PDBGS background cache, `iUpdateRet` frame skipping, the sprite cache with 60 s eviction, and the
  lazy per-draw .pak loading. On modern hardware load every sheet at start (or by map) as a texture.
- The duplicated member/global blend tables and the 64x510 add tables: replace with arithmetic or
  GPU blend state.
- Pixel-format detection and the 555 branch, once the reference look is decided.
- Lost-surface recovery, `FlipToGDISurface` for IME/HTML dialogs, the DirectDraw clipper.
- Night reload-from-disk: keep the original pixels and tint in the shader.
- `ChangeDisplayMode` window/fullscreen rebuild: use a resizable window with integer scaling of
  the 640x480 frame.
- The helper DLLs `MMXCPY.DLL`, `Dib16.dll`, `Offs.dll`, `DDRAW7.DLL` are not used by the client at
  all (`blitters.md`, last section).

## Current behaviour: bugs to replicate or fix (decide when porting)

Entries live in [issues/rendering.md](../../issues/rendering.md); the code currently does the following.

- Guide-map full mode uses map width for both axes (`Game.cpp:17924-17925`, [RENDER-002](../../issues/rendering.md#render-002-guide-map-full-mode-divides-both-axes-by-map-width)).
- The Energy-Ball sprites are created with the name `"Effect5.pak"` and built from entry 0 for all 40 slots (`Game.cpp:3874`, [RENDER-003](../../issues/rendering.md#render-003-energy-ball-sprite-pak-name-is-wrong)).
- `PutTransSprite` ignores `alphaDepth` (`Sprite.cpp:873`, [RENDER-004](../../issues/rendering.md#render-004-puttranssprite-ignores-alphadepth)).
- `PutRevTransSprite` indexes past row 63 (`Sprite.cpp:3224`, [RENDER-005](../../issues/rendering.md#render-005-putrevtranssprite-indexes-past-row-63)).
- The X-mas snow loop is `i <= MAXNUM` (`Game.cpp:23163`, only with `DEF_XMAS`, [RENDER-006](../../issues/rendering.md#render-006-x-mas-snow-loop-reads-past-the-array)).
- BGR565 and 32-bit desktops are not handled (`DXC_ddraw.cpp:425-450`, [RENDER-007](../../issues/rendering.md#render-007-bgr565-and-32-bit-desktops-render-wrongly)).
- Pixel (1,0) of every sheet is zeroed (`DXC_ddraw.cpp:409-411`, [RENDER-008](../../issues/rendering.md#render-008-pixel-10-zeroed-in-every-sprite-sheet)).

## A minimal port design

1. Decode each .pak image once to RGBA using the PAKLib reader; set alpha 0 where the 16-bit
   converted value equals the 16-bit converted (0,0) pixel.
2. Keep a 640x480 render target; draw in the original order with four blend modes: opaque-keyed,
   additive with constant a, average (50%), subtractive.
3. Implement shadows as a second draw of the same sprite with a shear/squash transform, sampling
   every third row (nearest filtering), multiplied by 0.25 and clipped to y < 427.
4. Tints as per-draw uniforms: signed channel offset with floor 1 (dyes), additive tint with
   max(dst) (glare), night offset for alpha-effect sheets.
5. Quantise colours to 565 in the shader before blending if pixel-exact output is wanted.
6. Present with integer scaling; keep the game logic at the original per-object frame times.

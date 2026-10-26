# Pixel routines (blitters)

Every routine in the 2.20 client that writes pixels, what maths it does, and what it costs.
Sources: `Sprite.cpp`, `Mydib.cpp`, `DXC_ddraw.cpp`, `Wmain.cpp` (tables), `Game.cpp` (lines,
lightning). Verified unless marked *inferred*. Surfaces and the retained-pointer trick are in
`directdraw.md`.

## Contents
- Summary: there is no assembly
- How a sprite sheet gets into memory
- The common prologue (frame lookup, clipping, lazy load, night check)
- Colour key
- The lookup tables
- Routine reference (one section per routine)
- Lines and lightning
- Cost per pixel
- Helper DLLs shipped with the source (MMXCPY, Dib16, Offs, DDRAW7)

## Summary: there is no assembly

- No `__asm`, no MMX intrinsics, no `LoadLibrary` of a blitting DLL anywhere in the client
  sources (grep of all `.cpp/.h`, verified). Every pixel loop is plain C compiled by VC6.
- No run-length encoding and no pre-clipped data: a sprite sheet is a plain uncompressed bitmap
  converted to a 16-bit surface; every frame is a rectangle of that sheet; transparency is a
  colour key tested per pixel.
- Speed comes from four things: (1) colour-keyed `BltFast` (DirectDraw HEL) for the opaque
  majority of draws; (2) 64x64 and 64x510 lookup tables that replace every add, clamp and scale;
  (3) writing through retained surface pointers without Lock/Unlock; (4) skipping whole frames
  when nothing moved (`frame-pipeline.md`).

## How a sprite sheet gets into memory

`CSprite` is one *sprite* (one ASD entry inside a .pak), holding `m_iTotalFrame` frame
rectangles (`stBrush`: sx, sy, szx, szy, pvx, pvy - Sprite.h:22-30).

1. Constructor (Sprite.cpp:26-58) reads only metadata from the already open .pak handle: the
   entry offset at `24 + n*8`, the frame count at entry+100, the 12-byte frame table, and stores
   `m_dwBitmapFileStartLoc = entry + 108 + 12*frames` (the embedded BMP). **No pixels are
   loaded** ("load on first draw", comment line 57).
2. First draw calls `_iOpenSprite` (Sprite.cpp:2347-2366) -> `_pMakeSpriteSurface`
   (Sprite.cpp:66-97): `CMyDib` reopens `sprites\<name>.pak` **by name** (Mydib.cpp:16-17), seeks to
   the BMP, reads it whole; a system-memory surface of the bitmap size is created; the bitmap is
   painted with GDI `SetDIBitsToDevice` through the surface DC (Mydib.cpp:46). **GDI does the
   8-bit-palette (or 24-bit) to 565/555 conversion** - there is no converter in the client.
3. The colour key is whatever 16-bit value pixel (0,0) became (Sprite.cpp:91-92); it is set as the
   DirectDraw source colour key (Sprite.cpp:2353), the surface pointer and pitch are retained
   (Sprite.cpp:2358-2361), and `_SetAlphaDegree` applies the night tint if needed.
4. `CGame::ReleaseUnusedSprites` (Game.cpp:15618-15660), run every 8 s by `OnTimer`
   (Game.cpp:4272-4274), releases any sheet not drawn for 60 s (`m_dwRefTime`), unless
   `m_bOnCriticalSection` is set. The metadata object stays, so the next draw reloads from disk.

The .pak format is defined by the user's library, which is authoritative:
[PAKLib](https://github.com/shadowevil/PAKLib) (write-up) and
[PAKLibC](https://github.com/shadowevil/PAKLibC) (C port). Mapping of the client's reads onto it
(PAKLib README read 2026-10-25; client offsets verified in source):

| PAKLib field | Client code |
|---|---|
| sprite count, Int32 at 20 | read by `CGame::MakeSprite/MakeTileSpr/MakeEffectSpr` to cap the loop (Game.cpp:3550-3551); never read by `CSprite` |
| sprite table at 24, 8 bytes per entry (offset, length) | `SetFilePointer(24 + n*8)`, reads the offset only (Sprite.cpp:41-42); the **length field is ignored** |
| entry header, 100 bytes (`<Sprite File Header>` + padding) | skipped unchecked (`iASDstart+100`, Sprite.cpp:45); the client never validates either magic |
| rectangle count, Int32 at entry+100 | `m_iTotalFrame` (Sprite.cpp:47) |
| rectangles at entry+104, 12 bytes: x, y, w, h, pivotX, pivotY (Int16) | read straight into `stBrush` (sx, sy, szx, szy, pvx, pvy), same order (Sprite.cpp:50-52) |
| 4 padding bytes, then image at entry+108+12n | `m_dwBitmapFileStartLoc` (Sprite.cpp:49) |
| image bytes "raw sprite image" | parsed as a Windows BMP: 14-byte `BITMAPFILEHEADER`, then `bfSize-14` bytes (Mydib.cpp:19-21). The client trusts `bfSize`, not the PAKLib length |

The client skips most of the format on purpose, to read fast. Summary of what it depends on:

| Structure | Client | Where |
|---|---|---|
| file magic `<Pak file header>` + 3 pad bytes | **skipped** (never read) | - |
| sprite count at 20 | read only by `Make*` loaders; per-sprite constructors called directly (`new CSprite(h, ..., n)`) never check it | Game.cpp:3550, 3605-3615 |
| table entry offset at 24+8n | **read**, trusted | Sprite.cpp:41-42 |
| table entry length at 28+8n | **ignored** | - |
| sprite magic `<Sprite File Header>` + 80 pad bytes | **skipped** (`+100`) | Sprite.cpp:45 |
| rectangle count at +100 | **read**, trusted (allocates `count` brushes) | Sprite.cpp:47, 50 |
| rectangles at +104 | **read** as one block | Sprite.cpp:52 |
| 4 pad bytes after rectangles | **assumed** present (image offset is +108+12n) | Sprite.cpp:49 |
| image | **assumed** to be a BMP; `BITMAPFILEHEADER.bfSize` decides how much is read; info header, palette and bits are passed to GDI | Mydib.cpp:19-27, 46 |

Client-specific behaviour that is not part of the format: the colour key is the converted pixel
(0,0) of the image; the pivot is added to the draw position (`dX = sX + pvx`, so pivots are
normally negative). PAKLib does not cover colour keys.

Consequences (*inferred* from the reads above):
- **Still loads in the client:** wrong or missing file magic, wrong sprite magic or padding
  content, a wrong sprite count (when the constructor is called directly, or when the count is at
  least the number of sprites the loader asks for), wrong or zero entry lengths, junk after the
  image, an image whose `bfSize` is correct even if the PAKLib length is not. Such files can fail
  in a strict reader like PAKLib while working in the game.
- **Breaks the client:** a wrong entry offset (reads garbage as the frame count and allocates
  from it), a wrong rectangle count, missing 4-byte padding (the BMP header is read 4 bytes off),
  an image that is not a BMP or has a wrong `bfSize` (`new char[bfSize-14]` with a garbage size; no
  check of `CreateFile` or `ReadFile` results in `CMyDib`), and fewer images than the loader asks
  for with a count field that overstates them. A rectangle that falls outside the bitmap is not
  checked either: software blitters would read past the surface.
- `Make*` loaders silently create fewer sprites if the count is smaller than requested
  (`if (i < iTotalimage)`, Game.cpp:3554); the missing slots stay NULL; blits on them are skipped by
  the `this == NULL` check, direct member reads of them crash (see the prologue).

Index layout per sprite is in `..\assets.md` and the `Make*Spr` calls in
`..\functions\config-loading.md`.

## The common prologue

All 25 `CSprite::Put*` routines (plus `_GetSpriteRect`, `_SetAlphaDegree`, `_bCheckCollison`, 28 guarded methods; constructors, `_pMakeSpriteSurface`, `iRestore`, `_iOpen/_iCloseSprite` are unguarded) start with the same ~80 lines, copy-pasted (e.g.
Sprite.cpp:873-950):

1. `if (this == NULL) return;` - callers index `m_pSprite[]` without checking, so a blit on a
   missing sprite is skipped (relies on calling a non-virtual method through NULL, undefined
   behaviour that VC6 tolerates; a modern compiler may delete the check). This holds only for
   calls into CSprite: about 165 sites read members of `m_pSprite[x]` directly (`->m_rcBound`,
   `->m_stBrush`, `->m_sPivotX`; e.g. Game.cpp:2755, 4533, 8199, 8995, 9235) and fault on a NULL
   slot (`_GetSpriteRect` on NULL returns without writing, then `->m_rcBound.left` at 4533 faults).
2. Frame range check, `m_bOnCriticalSection = TRUE` (guards against `ReleaseUnusedSprites`). It
   is set at entry (Sprite.cpp:883, 108, 713); the early `return` paths (clipped out, `_iOpenSprite`
   failure, szx==0 at 960, `_pMakeSpriteSurface` 74/80/88) leave it TRUE ([RENDER-013](../../issues/rendering.md#render-013-early-returns-leave-the-sprite-critical-section-flag-set)).
3. `dX = sX + pvx; dY = sY + pvy` - the pivot is a per-frame offset stored in the .pak.
4. Clip against `m_rcClipArea` by shrinking `sx/szx/sy/szy`; fully clipped sets
   `m_rcBound.top = -1` and returns. The `BltFast` routines test `<= 0`, the software routines
   test `< 0` and then bail on zero size separately.
5. Lazy load (`_iOpenSprite`) and, for sheets created with `bAlphaEffect`, the day/night check:
   if `m_cAlphaDegree != G_cSpriteAlphaDegree`, either tint in place (going to night) or close
   and reload from disk (going to day) (Sprite.cpp:938-950).
6. `m_rcBound` = the clipped screen rectangle (used for hit-testing and the tree-fade test).

`m_dwRefTime = dwTime` is recorded for the cache.

## Colour key

- `BltFast` routines pass `DDBLTFAST_SRCCOLORKEY`; DirectDraw skips key pixels.
- Software routines test `if (pSrc[ix] != m_wColorKey)` per pixel. `_NoColorKey` variants omit the
  test and process the full rectangle (used for light glows whose background is black: adding
  black is a no-op, so the key test is unnecessary and the branch is saved).
- The night tint never produces the key: if the tinted value equals it, blue is bumped by one
  (Sprite.cpp:2889-2891).

## The lookup tables

Built once at start-up; all are `long`/`int` (32-bit) arrays.

| Table | Built at | Index | Value |
|---|---|---|---|
| `G_lTransRB{100,70,50,25}[d][s]`, `G_lTransG*` | DXC_ddraw.cpp:155-168 via `_CalcMaxValue(s,d,ch,1,a)` (452-489) | dst channel, src channel | `min(max, d + (int)(s*a))` - **additive**, src scaled by a = 1.0 / 0.7 / 0.5 / 0.25 |
| `G_lTransRB2`, `G_lTransG2` | same, method 2 | dst, src | `(s + d) / 2` - true 50% average |
| `m_lFadeRB`, `m_lFadeG` (members only) | DXC_ddraw.cpp:152-153 via `_CalcMinValue` (491-517) | dst, src | `max(0, d - s)` - **subtractive** |
| `m_lTrans*` (members) | DXC_ddraw.cpp:137-150 | | identical copies of the `G_lTrans*` globals |
| `G_iAddTable31[c][delta+255]`, `G_iAddTable63` | Wmain.cpp:859-870 | channel 0-63, delta+255 (0-509) | `clamp(c + delta, 1, 31 or 63)` - note the **floor is 1, not 0** |
| `G_iAddTransTable31[v+255][d]`, `...63` | Wmain.cpp:872-878 | value+255, dst | `v < d ? d : min(v, 31 or 63)` - "never darker than the destination" |

"max" is 31 for R/B; for G it is 63 in 565 and 31 in 555 (DXC_ddraw.cpp:471-486). The tables are
sized 64 wide for every channel so one table serves both formats.

Memory (derived): 10 global trans tables x 16 KB + 12 member tables x 16 KB + 4 add tables x
127.5 KB = about 862 KB, a large share of a 2001 L2 cache budget; each pixel touches three rows.

Precision quirk: `_CalcMaxValue` truncates `s * 0.7` with `(int)`, so 70% of a 5-bit 1 is 0.

## Routine reference

"Uses" counts are call sites in Game.cpp + MapData.cpp (grep, verified).

### Opaque copies (DirectDraw `BltFast`)
| Routine | Line | Destination | Key | Uses | Used for |
|---|---|---|---|---|---|
| `PutSpriteFast` | Sprite.cpp:100 | back buffer | yes | 613 | almost everything: bodies, armour, UI, cursor, items |
| `PutSpriteFastNoColorKey` | :293 | back buffer | no | 2 | full-screen backgrounds, guide map |
| `PutSpriteFastNoColorKeyDst` | :390 | any surface | no | 1 | tiles into the background cache (Game.cpp:15861) |
| `PutSpriteFastDst` | :195 | any surface | yes | 0 | unused |
| `PutSpriteFastFrontBuffer` | :483 | primary | yes | 0 | unused |
| `PutSpriteFastWidth` | :578 | back buffer | yes | 3 | HP/MP/SP bars: width cut to the gauge value (Game.cpp:20158) |
| `PutShiftSpriteFast` | :3032 | back buffer | yes | 1 | zoomed guide map: a fixed 128x128 window scrolled by (shX, shY) over a big map image (Game.cpp:17905) |

Cost: one HEL blit; per pixel a key compare and a 2-byte copy, done in DirectDraw's own code.

### Additive "trans" family
All share one loop shape, e.g. `PutTransSprite` 565 path (Sprite.cpp:965-977):

```cpp
if (pSrc[ix] != m_wColorKey) {
  pDst[ix] = (WORD)((G_lTransRB100[(pDst[ix]&0xF800)>>11][(pSrc[ix]&0xF800)>>11]<<11)
                  | (G_lTransG100 [(pDst[ix]&0x7E0)>>5 ][(pSrc[ix]&0x7E0)>>5 ]<<5)
                  |  G_lTransRB100[(pDst[ix]&0x1F)     ][(pSrc[ix]&0x1F)     ]);
}
```

In words: split dst and src into channels with masks and shifts, look up `dst + src*a` clamped
per channel, reassemble. The 555 path is the same with masks 0x7C00/0x3E0/0x1F. This is a
saturating **additive** blend: black adds nothing, so effect sprites drawn on black need no
alpha channel. That is how every glow, fire, lightning and spell looks.

| Routine | Line | Table | Key | Uses | Notes |
|---|---|---|---|---|---|
| `PutTransSprite` | :873 | 100 | yes | 115 | full additive; the `alphaDepth` argument is not used ([RENDER-004](../../issues/rendering.md#render-004-puttranssprite-ignores-alphadepth)) |
| `PutTransSprite_NoColorKey` | :1001 | 100 | no | 45 | light glows |
| `PutTransSprite70` / `_NoColorKey` | :1124 / :1254 | 70 | yes / no | 4 / 11 | |
| `PutTransSprite50` / `_NoColorKey` | :1377 / :1507 | 50 | yes / no | 13 / 15 | |
| `PutTransSprite25` / `_NoColorKey` | :1629 / :1759 | 25 | yes / no | 84 / 6 | invisible characters ("bInv": own player, allies, Energy-Ball, Wyvern) are drawn at 25% additive (Game.cpp:14015) |
| `PutTransSprite2` | :1882 | 2 (average) | yes | 55 | real 50% translucency: transparent dialog boxes (`DrawNewDialogBox` with `m_bDialogTrans`, Game.cpp:19530), trees in front of the player (Game.cpp:2659) |
| `PutShiftTransSprite2` | :1998 | 2 | yes | 1 | translucent zoomed guide map |

### Subtractive
`PutRevTransSprite(x, y, frame, time, alphaDepth)` (Sprite.cpp:3130-3259): per channel
`m_lFadeRB[d][s + alphaDepth]`, i.e. `dst - (src + depth)` clamped at 0 (565 adds the depth twice
to G to match its extra bit, line 3225; 555 adds it once, 3245). Darkens the screen in the shape
of the sprite: used for shadowy spell effects (Game.cpp:7240-7648, 6 uses). `alphaDepth` grows
with the frame number so the effect fades. The index `s + depth` is not range-checked; past 63
it reads into the next row of the 64x64 table (*inferred* harmless garbage).

### Darkening: shadows and "fade"
- `PutShadowSprite` (Sprite.cpp:703-771) builds a **projected shadow procedurally** from the
  sprite silhouette - there is no shadow art. Only every third source row is visited
  (`iy += 3`), each source pixel lands at `x = sX+pvx+ix+(iy-szy)/3`, `y = sY+pvy+(iy+2*szy)/3`:
  a vertical squash to 1/3 plus a shear to the left, anchored at the feet. The destination pixel
  becomes `(p & 0xE79C) >> 2` (565) / `(p & 0x739C) >> 2` (555): the mask clears the low two bits
  of each channel so the shift cannot bleed between channels - a 75% darken in one AND and one
  shift. Each destination pixel is bounds-checked against 0-639 x 0-426 individually. Because rows
  are skipped and several source pixels can land on one destination, overlapping pixels are
  darkened again (double shadows darker, *inferred*). 36 uses: every character and monster body,
  lamp posts and signs (Game.cpp:2563).
- `PutShadowSpriteClip` (Sprite.cpp:774-870): the same projection walked bottom-up with
  `pDst -= pitch + 1`; used when the character is near the left edge (`sX < 50`, Game.cpp:14043). 32 call sites in Game.cpp; the two read (8975-8978, 14042) are gated by `if (sX < 50)` and only at detail level != 0; the other 30 were not individually read.
- `PutFadeSprite` (Sprite.cpp:2118-2230) and `PutFadeSpriteDst` (:2232): the same
  `(p & 0xE79C) >> 2` darken over the sprite's own shape, no projection. Despite the name there is
  no fade over time. Used for tree shadows when the tree turns translucent (Game.cpp:2658) and one
  effect (Game.cpp:7216).
- `DXC_ddraw::DrawShadowBox` type 0: `(p & 0xF7DE) >> 1`, a 50% darken of a rectangle
  (DXC_ddraw.cpp:536-553).

### Tinting and colour replacement
- `PutSpriteRGB(x,y,f, dR,dG,dB)` (Sprite.cpp:2379-2509): **replaces** the destination with the
  source shifted by a signed per-channel delta: `G_iAddTable31[src_ch][delta+255]`. 334 uses. This
  is how every dye works: art is drawn around neutral grey and the caller passes
  `m_wR[color] - m_wR[0]` (armour, cloth) or `m_wWR[color] - m_wR[0]` (weapons, shields), e.g.
  Game.cpp:2367-2368, 14020. Colour indices come from the 4-bit fields of the object's
  `iApprColor` (weapon bits 28-31, shield 24-27, armour 20-23, mantle 16-19, arm 12-15, pants
  8-11, boots 4-7, helm 0-3; Game.cpp:13745-13752). At detail level 0 all dyes are forced to 0
  (Game.cpp:13734-13743). Hair uses `_GetHairColorRGB` (Game.cpp:5150) the same way. Sprite fonts
  are coloured with it too (Game.cpp:8137). The table floor of 1 means a dyed channel never
  reaches 0 (*inferred*: keeps dyed pixels from colliding with a pure-black key).
- `PutTransSpriteRGB` (Sprite.cpp:2512-2644, 188 uses) and `_NoColorKey` (:2646, 0 uses): additive
  blend first, then the tint, then "not darker than dst":
  `G_iAddTransTable31[ G_lTransRB100[d][s] + delta + 255 ][d]`. Used for weapon glare - a pulsing
  red/green/blue add with `m_iDrawFlag` ramping 0..25..0 (Game.cpp:14023-14028, 35439-35454) - and
  for coloured light pools (Game.cpp:2600-2602, 7762+). Note the 565 path mixes `G_lTransRB100`
  and `m_pDDraw->m_lTransRB100` for blue (Sprite.cpp:2614); they hold identical values.
  `iRedPlus255` etc. are `short` here but `int` in `PutSpriteRGB`.

### Night (in-place surface tint)
`_SetAlphaDegree` (Sprite.cpp:2840-2931) rewrites the whole loaded sheet in place when
`G_cSpriteAlphaDegree` becomes 2 (night): R-3, G-3, B+2 in native channel units, clamped, key
pixels untouched. Only sheets constructed with `bAlphaEffect = TRUE` are affected (tiles, trees,
objects, bodies, armour, monsters - see the `Make*` calls); UI and effects are created with FALSE
and never tinted. Going back to day cannot undo a clamped value, so the sheet is released and
reloaded from disk on its next draw (Sprite.cpp:174-175). The server sends the degree in the
init/teleport data (Game.cpp:25171, 45944). Cost: one pass over each sheet once per day/night
change, zero per frame afterwards - the trick that makes night free at runtime.

### Hit-testing and bounds (no writes)
- `_bCheckCollison(x,y,frame,msX,msY)` (Sprite.cpp:2933-3030): mouse picking against the actual
  pixels: a 7x7 window around the cursor is scanned in the sheet for any non-key pixel. 76 uses
  (characters, items, trees for the X-mas effect).
- `_GetSpriteRect` (Sprite.cpp:2774-2838): fills `m_rcBound` without drawing (16 uses).

## Lines and lightning

- `CGame::DrawLine` (Game.cpp:23299-23405): Bresenham, each pixel added with
  `m_DDraw.m_lTransRB100/G100` (additive, full strength), clipped per pixel to 640x480. `DrawLine2`
  (Game.cpp:23407-23512) is identical with the 50% tables.
- `CGame::_DrawThunderEffect` (6 call sites, Game.cpp:7704-7726) builds jagged lightning from several `DrawLine`/`DrawLine2`
  segments with random offsets (see `..\functions\magic-effects.md`).
- `DXC_ddraw::PutPixel` (DXC_ddraw.cpp:581-599): plain write, used for the guide-map frame and the
  observer-mode cursor cross.

## Cost per pixel

Counted from the C source; actual instruction counts depend on VC6 code generation (*inferred*).

| Routine | Memory reads | Table lookups | Writes | Notes |
|---|---|---|---|---|
| BltFast keyed | 1 | 0 | 1 | DirectDraw HEL loop, fastest path |
| PutTransSprite* | 2 (src, dst) | 3 two-dimensional | 1 | ~6 mask/shift pairs, 1 branch |
| PutTransSprite2 | 2 | 3 | 1 | same shape |
| PutRevTransSprite | 2 | 3 | 1 | plus 3 adds for depth |
| PutSpriteRGB | 1 (src only) | 3 | 1 | dst not read |
| PutTransSpriteRGB | 2 | 6 (3 nested) | 1 | the most expensive per pixel |
| PutShadowSprite | 1 per 3 src rows + dst | 0 | 1 | 2 divides and 4 compares per pixel for the projection and bounds |
| PutFadeSprite / DrawShadowBox | 1-2 | 0 | 1 | one AND + one shift |

Pixels are touched one `WORD` at a time; there is no 32-bit pairing of pixels, no unrolling and
no span skipping of key runs. The indexing `pDst[ix]` against a row pointer advanced by the pitch
is the only pointer trick (*inferred*: VC6 keeps `pSrc`, `pDst`, `ix` in registers).

## Helper DLLs shipped with the source

Checked with pefile (`.claude\scratch\client-map\dll_info.py`, `dll_exports.py`), verified:

| File | PE timestamp (UTC) | Version resource | Exports | Build |
|---|---|---|---|---|
| `MMXCPY.DLL` | 2001-04-05 08:26 | none | `mmxcpy` (1) | VC6 Debug, PDB `C:\My dll\dll 창고\MMXCPY\Debug\mmxcpy.pdb` |
| `Dib16.dll` | 2001-08-27 11:14 | none | `DIB16_Init`, `_Load`, `_Get_Bits`, `_Get_bi`, `_Get_Pos_Add`, `_Set_Pixel_hdc`, `_ReleaseAll` | VC6 Debug, PDB `C:\b\My dll\dll 창고\Dib16\Debug\Dib16.pdb`; imports only `SetPixel` from GDI |
| `Offs.dll` | 2001-04-06 08:28 | none | `OFFS_Init`, `_Create_Offs`, `_Get_Offs`, `_Get_Width`, `_Get_Height`, `_Blt_ColorKey`, `_Blt_ColorKey_Rect`, `_ReleaseAll` | VC6 Debug, PDB `C:\My dll\dll 창고\Offs\Debug\Offs.pdb` |
| `DDRAW7.DLL` | 2001-03-21 01:47 | none | `DDRAW7_Init`, `_Flip`, `_Get_pDD`, `_Get_pDDSPrimary`, `_Get_pDDSBack`, `_Release` | VC6 Debug, PDB `C:\b-\dll-\DDRAW7\Debug\DDraw7.pdb`; imports `DirectDrawCreateEx` |

Findings:
- **Origin.** None carries a version resource (no CompanyName, FileDescription or FileVersion).
  All four are Debug builds (debug CRT strings, `dbgheap.c`, `HeapValidate` imports) made with
  linker 6.0 from a personal folder named `dll 창고` ("dll storage"). They are a developer's own
  helper libraries, not Microsoft files. This contradicts the belief that Dib16.dll was an
  official Microsoft DLL: no Microsoft DLL ships as a debug build with a personal PDB path and no
  version resource (verified from the PE; the claim about origin is *inferred* from that evidence).
- **Purpose** (*inferred* from names and imports): `DDRAW7` wraps DirectDraw 7 setup and flip;
  `Offs` manages off-screen surfaces and colour-keyed blits; `Dib16` loads DIBs into 16-bit
  buffers; `MMXCPY` is an MMX memory copy (it contains `CPUID`, `MOVQ` and `EMMS` opcodes, the
  pattern of a CPU-detect-then-MMX copy). Together they look like an earlier or experimental
  rendering layer by the same team, timestamps March-August 2001.
- **Use by the 2.20 client: none.** No source file includes a header for them, calls an export,
  or loads them with `LoadLibrary`; there is no import library for them in the folder (the `.lib`
  files present are `Cint`, `DesDll`, `search`, `StrTok` and DirectX/Windows SDK libraries). The
  client's own `DXC_ddraw` + `CSprite` replace all of their functionality. They are leftovers in
  the source drop (verified by grep). The shipped executables agree: the 2.02 `HelGame.exe`
  (PE 2001-08-21) and the 2.23 patch `HelGame.exe` (PE 2003-04-15, packed: one import per DLL)
  import only KERNEL32, USER32, GDI32, ADVAPI32, DDRAW, DINPUT, DSOUND, IMM32, WINMM, WS2_32 and
  `search.dll` (verified with pefile, `.claude\scratch\client-map\imports.py`), and the 2.02
  install ships none of the four helper DLLs.
- The DLLs the client does use: `search.dll` (`__FindHackingDll__`, anti-hack, loaded by an
  obfuscated name, Wmain.cpp:241-273), `ClientLogon.dll` (Korean billing COM object, `#import`,
  Wmain.cpp:28), `cjconnector.dll` (Korean billing, Wmain.cpp:648), `Riched20.dll` (IME builds).
  `DesDll.dll` (`DecryptFunc`) is referenced only by a commented-out prototype (Wmain.cpp:68).

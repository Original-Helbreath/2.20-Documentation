# Effect transparency: why effects are drawn on black

How the 2.20 client draws soft, semi-transparent spell effects without any alpha channel, why the
effect art is rendered on black, and what that means for anyone editing, re-keying or porting
those sprites. The routine-level maths is in `blitters.md`; this page explains the design behind
it. Verified in source unless marked *inferred*; the artists' reasons are inferred from the art and
the code, since no design notes survive.

## Contents
- Summary
- Two kinds of transparency
- How additive blending turns black into "transparent"
- Why the programmers chose additive blending
- Why the artists rendered on black
- What the colour key does for an effect
- The other blends, and why they need their own art
- Evidence in the 2.20 files
- Consequences for editing and re-keying
- Consequences for a port

## Summary

- The client has **no per-pixel alpha**. A sprite pixel is either skipped (it equals the colour key)
  or drawn.
- Spell effects still look soft because they are drawn with a **saturating additive blend**:
  `screen = min(screen + sprite, max)` per colour channel (`PutTransSprite` and its 70/50/25%
  variants, `blitters.md` "Additive trans family").
- Under addition, **black adds nothing**, so black is transparent without any key test, and a dark
  pixel adds only a little light. The brightness of each pixel works as its opacity.
- So the effect art is rendered **on black**: the glow fades into black, and the blend turns that fade
  into a fade over whatever is on screen.
- **Semi-transparent additive effects must have a black background.** Any other background colour
  would be added to the screen wherever it shows through, including inside every soft edge.
- The colour key is almost irrelevant for these sprites. It only saves the work of adding black, and
  the `_NoColorKey` variants skip the key test entirely for that reason.

## Two kinds of transparency

| | Colour-keyed sprites | Additive effects |
|---|---|---|
| Draw call | `PutSpriteFast` (DirectDraw `BltFast` with a source colour key) | `PutTransSprite`, `PutTransSprite70/50/25`, `_NoColorKey` variants |
| Per pixel | equal to the key: skipped; otherwise copied as is | added to the screen, clamped per channel |
| Transparency | on or off | graded: brightness is opacity |
| Background colour | whatever the key is; the client takes pixel (0,0) after converting the sheet to 16 bits (Sprite.cpp:91-92) | must be black |
| Soft edges | impossible; any anti-aliased edge pixel is drawn solid | free: dark edge pixels add little |
| Used for | bodies, armour, items, tiles, UI (613 call sites) | glows, fire, lightning, spells (about 290 call sites) |

Call-site counts are from `blitters.md` (grep of Game.cpp and MapData.cpp).

## How additive blending turns black into "transparent"

The 565 path of `PutTransSprite` (Sprite.cpp:965-977) splits the screen pixel and the sprite pixel
into red, green and blue, and looks up `min(max, dst + src)` per channel in a table built at start-up
(`G_lTransRB100`, `G_lTransG100`; DXC_ddraw.cpp:155-156 via `_CalcMaxValue`, :452).

Worked through for a blue spell over grass (channel values in 8-bit terms for readability; the
client works in 5/6/5 bits):

| Sprite pixel | Screen before | Screen after | What you see |
|---|---|---|---|
| `000000` black background | grass `4B5A2C` | `4B5A2C` | nothing: adding zero changes nothing |
| `00002C` dark blue, outer glow | `4B5A2C` | `4B5A58` | the grass, slightly blue |
| `1030A0` mid blue | `4B5A2C` | `5B8ACC` | a strong blue tint, grass still visible |
| `C0E0FF` hot core | `4B5A2C` | `FFFFFF` (clamped) | white-hot centre |

The darker a pixel, the less it changes the screen. That is exactly what a semi-transparent pixel
does, without storing an alpha value. In compositing terms the art is *premultiplied over black*:
each pixel already holds its colour times its opacity, and black is "zero opacity".

The 70/50/25% variants scale the sprite before adding, truncating the scaled value
(`dst + (int)(src * 0.7)`, and so on, so 70% of a 5-bit 1 is 0;
DXC_ddraw.cpp:158-165). That fades a whole effect in or out with no extra art (*inferred* use: the
call sites pick the variant by effect stage or detail; see `../functions/magic-effects.md`).

## Why the programmers chose additive blending

*Inferred* from the hardware and API of the time (2001, DirectDraw 7, 16-bit colour):

- **DirectDraw has no alpha blending in software.** `BltFast` and `Blt` copy pixels and honour a
  colour key; blending had to be written by hand in C (`blitters.md` "Summary: there is no assembly").
- **16-bit surfaces have no room for alpha.** The back buffer is RGB565 or RGB555
  (`directdraw.md`); there is no fourth channel to store per-pixel opacity in the sprite surface either.
- **Additive blending needs only the two pixels.** A true alpha blend needs the source, the
  destination and a separate alpha value per pixel, plus a multiply. Additive blending needs one
  table lookup per channel (`G_lTransRB100[d][s]`), which the client already uses for everything.
- **It suits the subject.** Fire, lightning, magic and glows emit light. Adding light to the scene is
  the physically right model for them, and overlapping effects brighten towards white the way real
  light does.
- **One table serves every strength.** The 100/70/50/25 tables give fade-outs for free.

## Why the artists rendered on black

*Inferred* from the art (every effect sheet checked below) and from how the client draws it:

- **It is the only background that works with addition.** Whatever colour sits behind the glow is
  blended into every soft edge pixel. With black that blended-in part is zero; with any other colour
  it would be added to the game screen as a haze of that colour.
- **It is what a renderer produces by default.** Effects of this era were typically rendered in a 3D
  or particle package; an empty render background is black, and the anti-aliasing at the edges
  fades the effect into it. The output is premultiplied-over-black by construction, which is exactly
  the format additive blending wants.
- **One sheet works on every floor.** Because black adds nothing, the same frames look right over
  grass, sand, stone, water, dungeon floor, night tint and other effects. A background colour baked
  into the art would only match one floor.
- **No alpha channel needed in the file.** The .pak stores plain BMPs (24 bpp or paletted, no alpha;
  `blitters.md` "How a sprite sheet gets into memory"). Brightness carries the opacity instead.
- **Soft edges survive 16-bit colour.** The client converts sheets to 565 at load. A fade towards
  black keeps working in 5/6 bits, because the values only need to get small, not hit an exact value.

## What the colour key does for an effect

- The client still takes the converted 16-bit pixel (0,0) as the key (Sprite.cpp:91-92) and
  `PutTransSprite` still skips pixels equal to it (Sprite.cpp:968). For a black background that only saves adding zero.
- `PutTransSprite_NoColorKey` (45 uses) and the 70/50/25 `_NoColorKey` variants drop the test
  entirely for light glows (`blitters.md` "Colour key"): adding black is already a no-op, so the
  branch is wasted work.
- **Changing an effect's key to another colour does not make the background transparent.** Only the
  pixels exactly equal to the new key would be skipped; every soft edge pixel still contains the old
  background, and would now contain the new one if the art were re-flattened onto it.

## The other blends, and why they need their own art

Additive blending can only brighten. Anything dark uses a different call, and its art follows
different rules:

| Effect type | Call | Maths | Background requirement |
|---|---|---|---|
| Light: glow, fire, lightning, spells | `PutTransSprite*` | `dst + src` (scaled) | black, which adds nothing |
| Shadowy spell effects | `PutRevTransSprite` (Sprite.cpp:3130, 6 uses) | `dst - (src + depth)` | black, which subtracts nothing; brightness is how much it darkens |
| Translucent dialogs, trees in front of the player | `PutTransSprite2` (Sprite.cpp:1882) | `(dst + src) / 2` | the key: black is *not* free here, it darkens the screen by half, so the key test matters |
| Darkened shapes (tree shadows, one effect) | `PutFadeSprite` (Sprite.cpp:2118) | `dst / 4` in the sprite's shape | only the shape matters; the sprite's colours are ignored |
| Character shadows | `PutShadowSprite` (Sprite.cpp:703) | `dst / 4`, projected from the silhouette | no shadow art at all |

So a dark smoke or shadow effect is not drawn with the additive family; it is subtracted, or drawn
as a darkened silhouette.

## Evidence in the 2.20 files

Pixel (0,0) of every effect sheet the client loads (Game.cpp:4217-4231: `MakeEffectSpr` calls,
plus a hand-written loop for effect5 at 4221-4226), measured from the extracted sheets in
2.20-Client-Files on 2026-10-26. Only sheets the client actually loads are counted:

| Pak | Sheets loaded | Key `000000` | Other keys |
|---|---|---|---|
| effect | 10 | 10 | - |
| effect2 | 3 | 3 | - |
| effect3 | 6 | 5 | `404040` (1) |
| effect4 | 5 | 4 | `004040` (1) |
| effect5 | 7 (sheets 1-7; sheet 0 is not loaded) | 6 | `800000` (1) |
| CruEffect1 | 9 | 7 | `FFFFFF` (1), `008000` (1) |
| effect9 | 21 (the pak holds 22; the loader asks for 21) | 21 | - |
| **Total** | **61** | **56** | 5 |

`effect6`, `effect7` and `effect8` are loaded by the client but are not in the 2.20 file set
(see `../assets.md`). The five sheets with a non-black key were not examined; they may be drawn
with one of the non-additive calls above (*unverified*).

The fringe was also measured on a colour-keyed sprite for contrast: Ant.pak (key `000000`, drawn
as a monster body) has about 13,300 near-black pixels touching the background, the anti-aliasing
of art rendered on black. Drawn with `PutSpriteFast`, these show as a thin dark outline, which the
dark game floors largely hide.

## Consequences for editing and re-keying

- **Additive effect sheets: keep the key black.** Do not replace it, do not threshold the glow and do
  not run edge-band or "clear outside frames to a non-black key" fixes on them. Cleaning stray
  pixels outside the frames to black and authoring `000000` as the key is safe.
- **Re-keying an effect to another colour does not work in the original client.** Two routes and why
  each fails:
  - *Convert to true alpha, add a background layer in the key colour, flatten, store as 24 bpp.*
    Flattening blends every semi-transparent pixel with the key colour (30% blue over magenta
    becomes purple), so the halo changes colour instead of disappearing, and the additive draw adds
    that colour to the screen.
  - *Convert to true alpha, threshold to on/off, flatten onto the key.* This gives an exact key, but
    the soft glow becomes a hard edge, and the edge pixels, now un-blended to full brightness, add
    more light than the original.
- **A non-black key for soft effects needs a client change**, for example a draw call that treats the
  distance from the key as opacity, or an additive draw that subtracts the key first. Neither exists
  in 2.20.
- **Colour-keyed sprites (bodies, items, tiles)** are where re-keying and fringe cleanup apply. They
  can only be on or off per pixel, so the choices are an edge-band unmatte (keeps hue, hard edge) or
  a threshold.
- The Colour Key Editor (2.20-Tools, `colorkey-editor`) previews both: the transparency key view
  drops pixels whose RGB565 value equals the key, as `PutSpriteFast` does, and its Effect view shows
  brightness as opacity, as the additive draw does.

## Consequences for a port

- Keep additive blending for every `PutTransSprite*` call. Replacing it with alpha blending changes
  the look: additive brightens what is under the effect, alpha blending does not.
- Black-background effect art can be loaded straight into a GPU with an additive blend state
  (`src * 1 + dst * 1`, scaled by the 70/50/25 factor). No conversion or alpha channel is needed.
- If a port wants alpha-blended effects instead, convert the art with colour-to-alpha against black
  (alpha = brightest channel, colour divided by alpha) and accept the different look. Keep the
  original black sheets for the faithful mode.
- Saturation matters: the client clamps per channel, so overlapping effects go white. A
  floating-point port should clamp the same way to match.

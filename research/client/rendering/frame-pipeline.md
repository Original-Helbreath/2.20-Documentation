# Frame pipeline

One frame of the 2.20 client from message pump to flip: draw order, redraw policy, timing,
text, sprite caching, the guide map, night and weather. Verified unless marked *inferred*.
Surface details: `directdraw.md`. Pixel maths: `blitters.md`.

## Contents
- The loop and its clocks
- Redraw policy: whole frame or nothing
- One in-game frame, step by step
- Inside DrawObjects: the painter's order
- Timing, animation and the "FPS" counter
- Text rendering
- Sprite loading, caching and memory
- Guide map (minimap) and full map
- Night, lights and weather
- Other screens (menus, loading)

## The loop and its clocks

`EventLoop` (Wmain.cpp:373-386) is a classic busy loop:

```cpp
if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) { GetMessage(...); Translate; Dispatch; }
else if (G_pGame->m_bIsProgramActive) G_pGame->UpdateScreen();
else if (G_pGame->m_cGameMode == DEF_GAMEMODE_ONLOADING) G_pGame->UpdateScreen_OnLoading(FALSE);
else WaitMessage();
```

- Rendering runs whenever the queue is empty; there is no `Sleep` and no frame cap. The only
  brake is `Flip(DDFLIP_WAIT)` (vsync) in fullscreen; windowed `Blt` has no vsync, so windowed mode
  spins a CPU core (*inferred*).
- `UpdateScreen` (Game.cpp:1106-1235) stores `G_dwGlobalTime = timeGetTime()` and dispatches on
  `m_cGameMode` to one `UpdateScreen_On*` per screen (list in `..\functions\game-modes.md`).
  IME builds then poll Enter/Tab/Left with `GetAsyncKeyState` (Game.cpp:1199-1234).
- A 1-second multimedia timer (`_StartTimer(1000)`, Wmain.cpp:857; the comment says 0.5 s) posts
  `WM_USER_TIMERSIGNAL` -> `CGame::OnTimer` (Game.cpp:4266-4348): sprite/sound cache sweep and
  keep-alive every 8 s, level-up and crusade checks every 5 s, chat expiry and lag kick every 2 s,
  billing-socket heartbeat every 5 s.
- Socket events arrive as window messages (`WM_USER_GAMESOCKETEVENT`, `WM_USER_LOGSOCKETEVENT`)
  between frames; state changes from the network are applied to `CMapData` immediately and seen
  by the next frame.

## Redraw policy: whole frame or nothing

There are no dirty rectangles. In the game screen a static `iUpdateRet` decides
(Game.cpp:34805, 35106-35404):

- `iUpdateRet != 0`: draw the **entire** frame from scratch into the back buffer and flip.
- `iUpdateRet == 0`: draw nothing except the cursor sprite and the top message, and do not flip
  (the cursor is drawn into the back buffer, but the frame is not presented; *inferred*: the
  next real frame shows it).

`iUpdateRet` is the return value of the previous frame's `CMapData::iObjectFrameCounter`
(MapData.cpp:1711-3381): 0 when no object, dead body, dynamic object or effect advanced a frame,
-1 when something advanced, 1 when the player's own animation advanced, 2 when the player's action
finished (which also re-enables input: Game.cpp:35412-35415). `bEffectFrameCounter` forces -1 when
an effect advanced (Game.cpp:35410). A static scene therefore costs almost nothing; a busy scene
redraws only when some animation frame is due (every 42-150 ms per object, see Timing).

The 672x512 background cache is the second redraw saving: tiles are re-rendered only when the
camera crosses a 32-pixel tile boundary (see step 3).

## One in-game frame, step by step

`CGame::UpdateScreen_OnGame` (Game.cpp:34802-35456):

1. First frame after entering (`m_cGameModeCount == 0`): clear back buffer, start the BGM, start the
   Korean billing thread, help hint (34815-34829).
2. Read mouse (DirectInput), handle a pending Enter: guild-name entry, drop/give amount entry, or
   the chat line (34836-35079). Chat is sent with `MSGID_COMMAND_CHATMSG` (35074) at most once per
   700 ms.
3. Camera shake: jitter `m_sViewPointX/Y` by `±m_iCameraShakingDegree`, decaying by 1 per frame
   (35086-35092). The real view point is restored at the end (35424-35425).
4. Split the view point into tile index and sub-tile offset: `sDivX = (view - pivot*32) / 32`,
   `sModX = ... % 32` (35097-35103).
5. If `iUpdateRet != 0`, in this order:
   1. `DrawBackground` (Game.cpp:15841-15878): if the tile division changed or
      `m_bIsRedrawPDBGS` is set, re-render 22x15 tiles into `m_lpPDBGS` with
      `PutSpriteFastNoColorKeyDst`, temporarily widening the clip rect to 672x512; then one
      `BltFast` of the 640x480 window at offset (sModX, sModY) to the back buffer. Crusade
      construction/teleport markers are drawn on top.
   2. `DrawEffectLights` (Game.cpp:7762-7937): additive light pools under explosions and spells
      (drawn before objects so characters stand in the light).
   3. `DrawObjects` (Game.cpp:2278-2933): ground items, corpses, characters, monsters, map objects
      and trees interleaved row by row (next section).
   4. `DrawEffects` (Game.cpp:6954-7732): all spell/projectile/explosion sprites from
      `m_pEffectList[300]`.
   5. `DrawWhetherEffects` (Game.cpp:23085-23172): rain or snow particles.
   6. `DrawChatMsgs(-100, 0, 800, 600)` (Game.cpp:18826): speech bubbles over heads.
   7. `WhetherObjectFrameCounter` (advances particles; it is update logic inside the draw block).
   8. `DrawDialogBoxs` (Game.cpp:18016-18161): every open dialog in z-order
      (`m_cDialogBoxOrder[]`, the gauge panel and icon panel are always at the bottom slots 39-40,
      Game.cpp:949-950).
   9. The chat input line: a `DrawShadowBox(0,413,639,429)` bar plus the IME/edit string
      (35128-35134).
   10. `ShowEventList` (Game.cpp:45466): the scrolling system-message lines.
   11. The dragged item: its sprite (dyed with `PutSpriteRGB`) and a GDI tooltip of name, level
       limit, weight, durability and count (35141-35250).
6. `DrawTopMsg` (always) (35308).
7. Logout and restart countdowns, which can leave the game screen (35320-35372).
8. Fade-in: the first 6 frames darken the whole screen once, the first 2 frames twice, with
   `DrawShadowBox` (35374-35375). The commented-out `_DrawBlackRect` loop (35315-35318) was an
   older iris-in effect built from 16x16 black cursor frames.
9. Cursor: a 5-pixel white cross in observer mode, otherwise the cursor sprite with the frame
   chosen during `DrawObjects` (35377-35384).
10. FPS bookkeeping, optional "fps" text, then `iFlip()`; on `DDERR_SURFACELOST`,
    `RestoreSprites()` (35386-35404).
11. Update for the next frame: `iUpdateRet = m_pMapData->iObjectFrameCounter(...)`,
    `bEffectFrameCounter()`, then `CommandProcessor(...)` with the mouse's tile coordinates
    (35407-35421) - input is processed **after** the flip, using hit-test results gathered while
    drawing this frame.
12. Camera: `CalcViewPoint` (Game.cpp:1238-1274) eases the view toward `m_sViewDst` (set while
    drawing the player, Game.cpp:2542-2543) by an accelerating step capped at 4 pixels when
    within 40 pixels; it runs only when the player animated (`iUpdateRet > 0`) or every 25 ms in
    observer mode (35428-35436).
13. The weapon-glare pulse `m_iDrawFlag` ramps 0..25..0 (35439-35454).

## Inside DrawObjects: the painter's order

`DrawObjects` walks screen cells in rows from 7 tiles above to 11 below the view, and 4 tiles
left to 4 right, so tall sprites that start off-screen still draw (Game.cpp:2299-2302). For
**each cell**, in this order:

1. Ground item (dyed if it has a colour; the cursor switches to the "pick up" animation when within
   13 px, alternating every 200 ms) (2357-2382).
2. Corpse (`DrawObject_OnDead`) (2384-2387).
3. Live object, through the `DrawObject_On<Action>` routine for its current action: Stop, Move,
   DamageMove, Run, Attack, AttackMove, Magic, GetItem, Damage, Dying (2470-2510). Each routine
   layers the character itself (next list). Illusion magic replaces every other player's
   appearance with the caster's (2451-2462).
4. Map object from the tile's object layer: lamp posts and signs get a procedural shadow;
   detail level 0 skips some sprites; lamp posts at night get flickering additive light; some
   objects spawn smoke/fire effects at night (2554-2640).
5. Trees (object sprites 100-199): shadow sheet (sprite + 50) then tree. If the player is fully
   inside the tree's rectangle and detail level is 2, the shadow is darkened with
   `PutFadeSprite` and the tree is drawn 50% translucent with `PutTransSprite2` (2641-2666).
   Detail level 0 substitutes four representative tree sprites (2645-2651).
6. Dynamic objects (fire walls, ice storms, fish, minerals, spikes) and crusade structures, then
   the rest of the cell (2700-2933, see `..\functions\world-map.md`).

Mouse picking happens during drawing: each `DrawObject_*` returns whether the cursor touches the
sprite's pixels (`_bCheckCollison`), and the last hit in draw order wins, recorded into
`m_sMCX/m_sMCY`, `m_cMCName` and the `sFocus*` globals (2389-2416, 2512-2537). Only the cursor
above y = 431 counts.

Character layering inside `DrawObject_OnStop` (Game.cpp:13721+; the other actions repeat it with
their own frame maths):
- Sprite indices are computed from the appearance words: body `500 + (type-1)*120 + mode*8`,
  underwear 4580, hair 4820, body armour 5060, arm armour 5300, pants 5540, boots 5780, weapon
  `6020 + weapon*64 + mode*8 + dir-1`, shield 9100, mantle 9230, helm 9300; women use the 14580+
  and 15060+... ranges (13776-13980). Full table: `..\assets.md`.
- Order depends on facing: `_cDrawingOrder[dir]` puts the weapon first for some directions
  (Game.cpp:57, 14012), and `_cMantleDrawingOrder[]` decides whether the mantle goes behind or in
  front (Game.cpp:58-59).
- Sequence for a player: crusade friend/foe marker, NPC floor light, special-ability aura,
  [weapon], **procedural shadow** (skipped at detail level 0 and for flying/ghost types), body,
  underwear, hair (tinted), boots, pants, arm armour, body armour, mantle, helm, [weapon],
  shield, weapon glare, then name and status text (*inferred* order beyond line 14050 from the
  pattern; see `..\functions\characters.md` for the exact lines).
- Invisible units (status bit 0x10) are drawn with `PutTransSprite25` only for the player and
  allies, and skipped for enemies (13769-13774).

## Timing, animation and the "FPS" counter

- Animation is time-based per object: `CMapData::m_stFrame[type][action].m_sFrameTime` gives the
  milliseconds per frame (defaults 60 stop, 70 move, 42 run, 78 attack, 88 magic, 150 pick-up,
  70 damage; MapData.cpp:25-41, overridden per monster type later in the constructor). Attack
  frames are slowed by `(status & 0xF) * 12` ms (weapon speed), magic is 13 ms faster at magic
  skill 100, frozen units are 25% slower (MapData.cpp:1905-1922).
- If more than two frame-times passed, up to 3 frames are skipped at once (MapData.cpp:1927-1932),
  so slow machines keep game time.
- Dynamic objects and per-cell effects advance every 100 ms, corpses every 150 ms
  (MapData.cpp:1742, 1886).
- There is no global tick: drawing frequency = how often anything is due, capped by vsync.
- The "FPS" display (Game.cpp:35386-35395) adds 256 per drawn frame and 1 per skipped frame,
  then every 500 ms shows `count >> 7` - i.e. drawn frames *per second* (256/128 = 2 per drawn
  frame per half-second) - and **adds 6 if the value is below 10**. The number shown is massaged
  and not a true frame rate.

## Text rendering

Two systems:

1. **GDI on the back buffer** for all variable text (names, chat, dialogs, tooltips):
   `CGame::PutString` family (Game.cpp:8254-8324) -> `DXC_ddraw::TextOut/DrawText` with the
   per-language font. Outline/shadow is faked by drawing the string 3 times offset by one pixel in
   near-black RGB(5,5,5) and then once in colour (`cBGtype == 1`, 8268-8274); `PutString2` uses
   pure black. Password fields replace characters with `*` (8276-8294). Each call without
   `bIsPreDC` does its own `GetDC`/`ReleaseDC` on the system-memory surface. Chat word-wrap uses
   `CMisc::iGetTextLengthLoc` with the DC (Game.cpp:15739-15779) and avoids splitting a DBCS
   character by counting high-bit bytes (15746, 15752).
2. **Sprite fonts** for decorative and numeric text:
   - `PutString_SprFont` / `SprFont2` (8125-8171): sprite `DEF_SPRID_INTERFACE_FONT1` (sprfonts.pak
     entry 0), ASCII 33-122, advance from the hard-coded `__cSpace[]` table, shadow pass + coloured
     pass with `PutSpriteRGB`.
   - `PutString_SprFont3` (8173-8225): `SPRFONTS2` (interface2.pak entry 1) with 95 glyphs per
     style (`iType`), or `FONT2` when `iType == -1`; advance = glyph width from the frame table;
     optional translucent (`PutTransSpriteRGB`) mode.
   - `PutString_SprNum` (8228-8252): digits from interface2.pak entry 0 frames 6-15, drawn
     additively (damage numbers and similar), advance from `__cSpace2[]`.

The English build's GDI font is Comic Sans MS 16 (DXC_ddraw.cpp:182); all others use 12-pixel
CJK fonts.

## Sprite loading, caching and memory

- 20000 sprite slots, 500 tile slots, 100 effect slots (Game.h:69-71), all `CSprite*`.
- Loading screen (`UpdateScreen_OnLoading`, Game.cpp:3596-4253) creates every `CSprite` object in
  26 steps (`m_cLoading` 0,4,...,100), each step reading only .pak headers and frame tables. No
  bitmap is decoded at load time. The progress bar is drawn between steps by
  `UpdateScreen_OnLoading_Progress`. Sounds are created at the last step.
- Pixels are loaded on first draw (`_iOpenSprite`), and freed 60 s after the last draw by
  `ReleaseUnusedSprites` every 8 s; sounds after 30 s if not looping (Game.cpp:15618-15660). This
  keeps memory proportional to what is on screen; a full set of armour, monster and spell sheets
  would not fit 2001-era RAM (*inferred*; the code comment at Sprite.cpp:2871 says the night-tint
  loop crashed often on PCs with less than 128 MB).
- Each first draw costs a file open, a seek, a full BMP read, a GDI conversion and a surface
  allocation, on the render thread - the source of the well-known hitch when a new monster or
  spell first appears (*inferred*).
- Day/night change reloads every alpha-effect sheet from disk on its next draw (`blitters.md`).

## Guide map (minimap) and full map

- Dialog 9, `DrawDialogBox_GuideMap` (Game.cpp:17871-18014): a 128x128 box, default top right.
  Frame drawn with 4 rows/columns of `PutPixel` grey.
  - Zoomed mode (`m_bZoomMap`, default TRUE): one 1:1 map image per map index from
    Telescope.pak (sprites `DEF_SPRID_INTERFACE_GUIDEMAP + index + 1`), shown through a 128x128
    window centred on the player with `PutShiftSpriteFast` (one map pixel = one tile), or
    translucent with `PutShiftTransSprite2`.
  - Full mode: the whole map scaled into 128x128 (Telescope.pak entry 0, frame = map index); the
    player dot is `player * 128 / mapsize`; both axes divide by `m_sMapSizeX`
    (Game.cpp:17924-17925, 17932-17933, [RENDER-002](../../issues/rendering.md#render-002-guide-map-full-mode-divides-both-axes-by-map-width)).
  - A blue dot (ND_CRUSADE frame 37) marks the player; a blinking monster icon (monster.pak)
    marks a server-announced monster event for 30 s.
- Dialog 22, `DrawDialogBox_Map` (Game.cpp:24767): the city maps from Newmaps.pak; see
  `..\functions\ui-dialogs.md`.

## Night, lights and weather

- **Night** is not an overlay. It is the in-place surface tint of `_SetAlphaDegree` (R-3, G-3,
  B+2) applied once per sheet (`blitters.md`), plus light sources drawn additively on top: lamp
  posts flicker with random negative tints (Game.cpp:2596-2603), object fires spawn effect 65 every
  400 ms (2607-2638), NPCs get floor lights (13985-13994). There is no darkness mask; UI and effects
  are untinted because they were created without `bAlphaEffect`.
- **Weather** (`m_cWhetherEffectType`; the source spells weather "whether"): 1-3 rain with 120 /
  300 / 600 particles, 4-6 snow with the same counts (`DEF_MAXWHETHEROBJECTS` = 600, Game.h:89).
  Each particle is an additive sprite from effect sheet 11 (rain frames 16-19 falling, 20-24
  splash; snow frames 39+), positioned in world pixels and advanced by `WhetherObjectFrameCounter`.
  Rain also starts a looping sound (`m_pESound[38]`). `DEF_XMAS` adds snow piling up at the bottom
  of the play area and blinking lights on trees, and its draw loop reads one element past its
  1000-entry arrays (`i <= MAXNUM`, Game.cpp:23163).

## Other screens (menus, loading)

Menu screens (`UpdateScreen_OnMainMenu`, `_OnLogin`, `_OnSelectCharacter`, ...) redraw fully every
loop iteration: a full-screen background sprite with `PutSpriteFastNoColorKey` or `PutSpriteFast`,
buttons, GDI text, the cursor, `iFlip()`. They have no `iUpdateRet` gate, so they run at vsync
rate in fullscreen and flat out in windowed mode (*inferred* from the loop). Character previews on
the select/create screens use `DrawObject_OnMove_ForMenu` (Game.cpp, see `..\functions\game-modes.md`).

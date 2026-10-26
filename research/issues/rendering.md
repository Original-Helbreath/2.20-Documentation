# Issues: rendering

Defects and divergences in the client's DirectDraw layer, sprite blitters and frame pipeline. Behaviour lives in [systems/rendering.md](../systems/rendering.md) and `research/client/rendering/`. The pixel-pan saturation in `bAddNewEffect` is recorded in the audio file ([AUDIO-001](audio.md#audio-001-pixel-based-sound-pan-saturates)).

### RENDER-001 FPS display is not a real frame rate

- **Program**: client
- **System**: rendering
- **Kind**: bug
- **Intent**: show frames per second.
- **Now**: the value is massaged (`count >> 7`, +6 if under 10) [Vd].
- **Where**: `Game.cpp:35386-35395`
- **Evidence**: [Vd]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/rendering.md section 9

### RENDER-002 Guide-map full mode divides both axes by map width

- **Program**: client
- **System**: rendering
- **Kind**: bug
- **Intent**: place the player dot from per-axis map dimensions.
- **Now**: both axes use map width, so the dot is wrong on non-square maps in full-map mode [Vd].
- **Where**: `Game.cpp:17924-17925`
- **Evidence**: [Vd]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/rendering.md section 9; client/rendering/porting-notes.md "Bugs"

### RENDER-003 Energy-ball sprite pak name is wrong

- **Program**: client
- **System**: rendering
- **Kind**: bug
- **Intent**: load the energy-ball sprites (40 frames).
- **Now**: the name `"Effect5.pak"` gets a second `.pak` appended, so the file opened is `sprites\Effect5.pak.pak`, which does not exist; `CMyDib` does not check `CreateFile` (`Mydib.cpp:17`), so the type-35 monster draws garbage or nothing [I]. All 40 slots are built from entry 0 of that pak, so even with the right name they would be 40 copies of one sprite [V].
- **Where**: `Game.cpp:3874`
- **Evidence**: [Vd] [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/rendering.md section 9; client/rendering/porting-notes.md "Bugs"

### RENDER-004 PutTransSprite ignores alphaDepth

- **Program**: client
- **System**: rendering
- **Kind**: bug
- **Intent**: blend at the requested depth.
- **Now**: the argument is ignored; callers passing a depth get full strength [Vd].
- **Where**: `Sprite.cpp:873`
- **Evidence**: [Vd]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/rendering.md section 9

### RENDER-005 PutRevTransSprite indexes past row 63

- **Program**: client
- **System**: rendering
- **Kind**: bug
- **Intent**: look up a blend table row.
- **Now**: it reads the next table row past 63 [Vd].
- **Where**: `Sprite.cpp:3224`
- **Evidence**: [Vd]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/rendering.md section 9

### RENDER-006 X-mas snow loop reads past the array

- **Program**: client
- **System**: rendering
- **Kind**: bug
- **Intent**: iterate over the snow array.
- **Now**: the loop is `i <= MAXNUM`, reading one past the end (only with `DEF_XMAS`).
- **Where**: `Game.cpp:23163`
- **Evidence**: porting-notes table
- **Blocks 2.20 setup**: no
- **Source doc**: client/rendering/porting-notes.md "Bugs"

### RENDER-007 BGR565 and 32-bit desktops render wrongly

- **Program**: client
- **System**: rendering
- **Kind**: bug
- **Intent**: support the desktop's pixel format.
- **Now**: BGR565 is detected but never handled: `m_cPixelFormat` 3 or 0 make every `switch` in the software blitters fall through, so translucent effects, shadows, tints and night tint vanish while `BltFast` sprites still draw. Windowed mode on a 32-bit desktop is also wrong (`WORD*` arithmetic) [Vd] [I]; no code checks the desktop depth.
- **Where**: `DXC_ddraw.cpp:425-450`
- **Evidence**: [Vd]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/rendering.md section 9; client/rendering/directdraw.md "Known defects"

### RENDER-008 Pixel (1,0) zeroed in every sprite sheet

- **Program**: client
- **System**: rendering
- **Kind**: bug
- **Intent**: set a colour key.
- **Now**: `_dwColorMatch(WORD)` writes the key as a `DWORD` to the first pixel, which also zeroes pixel (1,0) of every sheet; invisible in practice since it is normally outside every frame rectangle [I].
- **Where**: `DXC_ddraw.cpp:409-411`
- **Evidence**: [I]
- **Blocks 2.20 setup**: no
- **Source doc**: client/rendering/directdraw.md "Known defects"

### RENDER-009 First-draw decoding hitches on the render thread

- **Program**: client
- **System**: rendering
- **Kind**: bug
- **Intent**: smooth frames.
- **Now**: first-draw decoding on the render thread causes visible hitches when new monsters or spells appear [I].
- **Where**: sprite load path
- **Evidence**: [I]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/rendering.md section 9

### RENDER-010 ChangeDisplayMode leaks and can leave dangling pointers

- **Program**: client
- **System**: rendering
- **Kind**: bug
- **Intent**: switch display mode cleanly.
- **Now**: it leaks the old `m_lpPDBGS`, and `m_lpBackB4` is released without being set to NULL first; a failure half-way leaves dangling pointers.
- **Where**: `DXC_ddraw.cpp:249-320`
- **Evidence**: directdraw.md
- **Blocks 2.20 setup**: no
- **Source doc**: client/rendering/directdraw.md "Known defects"

### RENDER-011 iFlip ignores the BltFast result

- **Program**: client
- **System**: rendering
- **Kind**: bug
- **Intent**: detect a lost back buffer at the copy.
- **Now**: the result is ignored, so a lost video back buffer is noticed only through the following `Flip`.
- **Where**: `iFlip`
- **Evidence**: directdraw.md
- **Blocks 2.20 setup**: no
- **Source doc**: client/rendering/directdraw.md "Known defects"

### RENDER-012 GDI dialogs invisible in some fullscreen modes

- **Program**: client
- **System**: rendering
- **Kind**: bug
- **Intent**: show message boxes.
- **Now**: in fullscreen without `DDCAPS2_CANRENDERWINDOWED`, GDI dialogs (MessageBox) are invisible.
- **Where**: DirectDraw layer
- **Evidence**: directdraw.md
- **Blocks 2.20 setup**: no
- **Source doc**: client/rendering/directdraw.md "Known defects"

### RENDER-013 Early returns leave the sprite critical-section flag set

- **Program**: client
- **System**: rendering
- **Kind**: bug
- **Intent**: guard a sheet against `ReleaseUnusedSprites` only while it is being drawn.
- **Now**: `m_bOnCriticalSection = TRUE` is set at entry but every early `return` (clipped out, `_iOpenSprite` failure, szx==0, `_pMakeSpriteSurface` 74/80/88) leaves it TRUE, so `ReleaseUnusedSprites` skips that sheet until a later full draw [V].
- **Where**: `Sprite.cpp:883, 108, 713, 960`; `Game.cpp:15618-15660`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: client/rendering/blitters.md

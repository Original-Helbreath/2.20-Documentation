# Issues: audio

Defects and divergences in the client's DirectSound layer, pan and volume handling and music control. Behaviour lives in [systems/audio.md](../systems/audio.md).

### AUDIO-001 Pixel-based sound pan saturates

- **Program**: client
- **System**: audio
- **Kind**: bug
- **Intent**: pan by distance from the screen centre; the tile-based formula in the animator shows the intent [I].
- **Now**: the pixel-based pan is 32x too strong: anything more than about 10 px off-centre is clamped to fully left or right [V code].
- **Where**: `Game.cpp:6008` (pixel pan); `MapData.cpp:1747` (tile formula)
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/audio.md section 9

### AUDIO-002 Sounds are tied to animation frames and can be skipped

- **Program**: client
- **System**: audio
- **Kind**: bug
- **Intent**: every footstep plays.
- **Now**: the animator skips up to 3 frames when late, so footsteps can be dropped on slow machines [Vd] [I].
- **Where**: `MapData.cpp:1927-1932`
- **Evidence**: [Vd]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/audio.md section 9

### AUDIO-003 Primary format request has no lasting effect

- **Program**: client
- **System**: audio
- **Kind**: bug
- **Intent**: set the primary buffer to 44.1 kHz.
- **Now**: `YWSound::Create` sets the primary format under PRIORITY and then drops to NORMAL; under NORMAL DirectSound mixes at its default primary format, so the request probably has no lasting effect [V code] [I effect].
- **Where**: `YWSound.cpp:40, 64, 66`
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/audio.md section 9

### AUDIO-004 Volume 0 silences instead of quietening

- **Program**: client
- **System**: audio
- **Kind**: bug
- **Intent**: the slider scales loudness.
- **Now**: volume 0 silences, and the farthest heard distance depends on the slider (see Rules in audio.md); plausibly unintended [I].
- **Where**: audio.md Rules
- **Evidence**: [I]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/audio.md section 9

### AUDIO-005 Rain loop ignores the effects slider

- **Program**: client
- **System**: audio
- **Kind**: bug
- **Intent**: the effects slider scales every effect.
- **Now**: `m_pESound[38]->Play(TRUE)` uses the defaults `lPan = 0, iVol = 0` (full volume), unlike `PlaySound` [V].
- **Where**: `Game.cpp:23265`; `SoundBuffer.h:24`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/audio.md section 9

### AUDIO-006 Music slider restarts the track without looping

- **Program**: client
- **System**: audio
- **Kind**: bug
- **Intent**: a slider change keeps the looping track playing.
- **Now**: the handler restarts the BGM with `Play(FALSE, 0, iVol)` after `bStop(TRUE)`, whereas `StartBGM` loops with `Play(TRUE, ...)`; so after a slider drag the track plays once until the next track change, and dragging restarts it each frame [V code] [I effect].
- **Where**: `Game.cpp:41931-41937`, `35493`
- **Evidence**: [V code]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/audio.md section 9

### AUDIO-007 Sound eviction pauses while the window is inactive

- **Program**: client
- **System**: audio
- **Kind**: bug
- **Intent**: free sound buffers every 8 s.
- **Now**: eviction shares the 8 s sweep gated on `m_bIsProgramActive`, so it pauses when the window is inactive [V].
- **Where**: `Game.cpp:4270-4275`
- **Evidence**: [V]
- **Blocks 2.20 setup**: no
- **Source doc**: systems/audio.md section 9

# System: client audio

*Small cohesive doc; read whole.*

Program: client only (`sources/client-2.20/`). No server code plays or chooses sounds; every trigger is
client-side. Tags: [V] verified in source by this pass; [Vd] verified by the linked doc's own source
review; [I] inferred; [U] unverified. Function index: `../client/functions/sound.md`.

## 1 Purpose

Give positional feedback (footsteps, swings, hits, monster cries, spell blasts) and a per-region
music track, on DirectSound with no 3D audio. The player hears things louder and panned toward
where they happen on screen; music changes when entering a town, dungeon or Middleland. Sound is
cosmetic: it never gates game logic, and a machine without DirectSound simply runs silent
(`m_bSoundFlag` FALSE, Game.cpp:866-867) [V].

## 2 Subsystems

| Subsystem | Intent |
|---|---|
| A1 Device | create DirectSound, try a 44.1 kHz 16-bit stereo primary, fall back silently |
| A2 Sample bank | 24 character (C), 98 monster (M), 47 effect (E) .wav files, created at the end of loading, PCM loaded lazily, 2 voices per sample, evicted after 30 s idle |
| A3 Positional one-shots | `PlaySound(type, n, dist, pan)`: distance attenuation and pan computed by the caller |
| A4 Trigger points | animation-frame sounds in the map animator, effect-creation sounds, UI clicks, notify sounds |
| A5 Music | one looping `music\*.wav` chosen from the current location; rain loop as an ambient bed |
| A6 Settings | sound and music on/off and volume sliders in the system menu |

## 3 Functions

| Sub | Function | Where | Role |
|---|---|---|---|
| A1 | `YWSound::Create` | YWSound.cpp:26-74 | DirectSound, PRIORITY level, primary SetFormat 44100/16/2, then back to NORMAL [V 40, 61-66] |
| A2 | `UpdateScreen_OnLoading` (last step) | Game.cpp:4231-4244 | `new CSoundBuffer` for `sounds\C1-24`, `M1-98`, `E1-47` [V] |
| A2 | `CSoundBuffer::CSoundBuffer` / `bCreateBuffer_LoadWavFileContents` / `_LoadWavContents` | SoundBuffer.cpp:33-65, 106-142, 144-174 | name stored, PCM read on first play [Vd] |
| A2 | `CSoundBuffer::GetIdleBuffer` | SoundBuffer.cpp:204-270 | pick a non-playing voice of 2, restore lost buffers, single-load samples restart [V 204-240] |
| A2 | `CGame::ReleaseUnusedSprites` | Game.cpp:15651-15657 | free non-looping samples idle > 30 s [V] |
| A3 | `CGame::PlaySound` | Game.cpp:22783-22817 | volume from slider and distance, cut below -20 dB, dispatch C/M/E [V] |
| A3 | `CSoundBuffer::Play` | SoundBuffer.cpp:177-201 | set volume, clamp pan to DSBPAN_LEFT/RIGHT, play or loop [V] |
| A4 | `CMapData::iObjectFrameCounter` | MapData.cpp:1711-3381 | per-cell distance/pan, footsteps on move frames 1 and 5, monster/attack/death sounds on specific frames [V 1730-1747, 2050-2110] |
| A4 | `CGame::bAddNewEffect` | Game.cpp:5891-6953 | explosion/spell sound plus camera shake at creation [V 5995-6010] |
| A4 | `NotifyMsg_*` (HP, SP, LevelUp, Charisma, ItemObtained...) | Game.cpp:43313-45274 | status sounds (e.g. sound 21 on gains) [Vd `networking.md`] |
| A5 | `CGame::StartBGM` | Game.cpp:35458-35496 | choose track by `m_cCurLocation`, skip if same file, loop [V] |
| A5 | `CGame::SetWhetherStatus` | Game.cpp:23258-23285 | rain types 1-3 loop `E38`, stop on clear [V 23265, 23283] |
| A6 | `DrawDialogBox_SysMenu` / `DlgBoxClick_SysMenu` | Game.cpp:41763-41961 / 33282-33407 | volume sliders (41921), sound/music toggles [V 41921-41922; rest Vd] |

## 4 Commands and messages

None sent. Indirect drivers: `MSGID_EVENT_MOTION` (sets the action that the animator turns into
sounds), `MSGID_EVENT_COMMON` magic effects (via `bAddNewEffect`), `DEF_NOTIFY_WHETHERCHANGE`
(rain loop), `DEF_NOTIFY_TIMECHANGE` (plays a sound, Game.cpp:25166-25181 [Vd]), map change via
`MSGID_RESPONSE_INITDATA` -> new `m_cCurLocation` -> `StartBGM` on the first game frame, if music is on (`m_bMusicStat`, Game.cpp:34827)
(Game.cpp:34815-34829 [Vd], 45938-45940 [V]).

## 5 Data

- `m_pCSound[]`, `m_pMSound[]`, `m_pESound[]` sized `DEF_MAXSOUNDEFFECTS` 110 (Game.h:72); `m_pBGM` [V].
- `DEF_MAXSOUNDBUFFERS` 2 voices per sample (SoundBuffer.h:14) [Vd].
- `m_bSoundFlag` (device present), `m_bSoundStat` (effects on), `m_bMusicStat` (music on),
  `m_cSoundVolume`, `m_cMusicVolume` 0-100, default 100 (Game.cpp:866-867, 956-957) [V].
- Files: `sounds\C*.wav`, `M*.wav`, `E*.wav`; `music\aresden|elvine|dungeon|middleland|MainTm.wav`,
  `Carol.wav` only under `DEF_XMAS` (off in GlobalDef.h:29) [V]. Not persisted: the volume settings
  are not in the registry keys `ReadSettings` handles (`Magic`, `ShortCut0..4`) [Vd startup.md; I
  that volume resets each run].

## 6 Flow

**Footstep of a walking character**: server `MSGID_EVENT_MOTION` OBJECTMOVE -> `MotionEventHandler`
-> `bSetOwner` sets action MOVE (`client-state.md`) -> each frame `iObjectFrameCounter` advances the
cell's frame; on frames 1 and 5 for human-like types it calls `PlaySound('C', 8, sDist, lPan)` with
`sDist = (|cx-dx|+|cy-dy|)/2` tiles and `lPan = -(cx-dx)*1000` from the view centre
(MapData.cpp:1744-1747, 2064-2066) -> `PlaySound` volume `(vol-100)*20 - 200*min(dist,10)`
hundredths of dB, dropped if not above -2000 (Game.cpp:22790-22797) -> `CSoundBuffer::Play`
picks a free voice and plays [V].

**Explosion**: `bAddNewEffect` computes distance in tiles from screen centre and
`lPan = -(320 - (sX - viewX))*1000` in **pixels** (Game.cpp:6001-6009) -> `PlaySound('E', 2, ...)`
and `SetCameraShakingEffect(sDist)` [V].

**Map change**: `InitDataResponseHandler` copies the new location (Game.cpp:45938-45940) ->
`ChangeGameMode(ONMAINGAME)` resets the mode counter (46049) -> first `UpdateScreen_OnGame` frame
calls `StartBGM`, which returns early if the same file is already playing (35482-35485) [V].

## 7 Rules and invariants

- Distance capped at 10; each unit -2 dB; slider 0..100 maps to -20..0 dB; any result at or below
  -20 dB is not played at all (Game.cpp:22790-22797) [V]. Consequence: effects slider at 0 = mute,
  and at 100 a sound 10 tiles away is exactly at the cut-off and silent.
- Pan clamped to +-10000 (SoundBuffer.cpp:189-190): 1000 per tile, so 10 tiles = hard pan [V].
- Max 2 simultaneous copies of one sample; a third request steals a voice or, for single-load
  samples, restarts it (SoundBuffer.cpp:220-233) [V].
- Samples idle 30 s are freed unless looping (Game.cpp:15651-15657) [V].
- `PlaySound` ignores requests when the device is missing or effects are off (22787-22788) [V];
  `bAddNewEffect` returns before creating anything when the program is inactive (Game.cpp:5910) [V].

## 8 Couplings

| Direction | Coupling point | Other system |
|---|---|---|
| in | `m_cOwnerAction`, `m_cOwnerFrame`, `m_sOwnerType` per cell, read by the animator | `client-state.md`, `rendering.md` |
| in | effect types from magic/combat events | server magic and combat systems (other clusters) [U paths] |
| in | `m_cCurLocation` from init data | server map / world-log routing (other clusters) |
| in | weather type from `DEF_NOTIFY_WHETHERCHANGE` | server map system |
| in | system-menu sliders | `ui.md` |
| out | `SetCameraShakingEffect` is called alongside the explosion sound | `rendering.md` |

## 9 Current behaviour vs intent

- The pixel-based pan in `Game.cpp:6008` clamps to full left or right beyond about 10 px; the tile-based formula in the animator is the reference ([AUDIO-001](../issues/audio.md#audio-001-pixel-based-sound-pan-saturates)).
- Sounds are tied to animation frames, and the animator skips up to 3 frames when late ([AUDIO-002](../issues/audio.md#audio-002-sounds-are-tied-to-animation-frames-and-can-be-skipped)).
- `YWSound::Create` drops from PRIORITY to NORMAL after setting the primary format ([AUDIO-003](../issues/audio.md#audio-003-primary-format-request-has-no-lasting-effect)).
- Volume 0 silences rather than quietens, and the farthest heard distance depends on the slider ([AUDIO-004](../issues/audio.md#audio-004-volume-0-silences-instead-of-quietening)).
- There is no sound priority: when both voices are busy the oldest is stolen regardless of importance.
- The rain loop plays at full volume regardless of the effects slider ([AUDIO-005](../issues/audio.md#audio-005-rain-loop-ignores-the-effects-slider)).
- The music slider handler restarts the BGM with the loop flag FALSE ([AUDIO-006](../issues/audio.md#audio-006-music-slider-restarts-the-track-without-looping)).
- Sound eviction pauses while the window is inactive ([AUDIO-007](../issues/audio.md#audio-007-sound-eviction-pauses-while-the-window-is-inactive)).

## 10 Open questions

- What each numbered C/M/E sample is (no name table in source; only call sites) [U].
- Whether the retail client persisted volume elsewhere (no registry write found by the startup map) [U].

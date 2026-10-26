# Functions: Sound and music

Source: `sources\client-2.20\` (librehelbreath ClientSources c01544d). 17 function definitions (including `#if` duplicate bodies). Generated from a col-0 signature scan, then reviewed by hand.

Column key: **Where** file:start-end (verified). **Purpose** from name pattern and the nearest source comment (inferred unless the row ends in [V], meaning re-checked against the function body by the 2026-10-25 review, or the overview says verified; comments are UTF-8 Korean in source, quoted as-is). **Callers/Callees** matched by bare name across all files (verified as text matches; same-named methods in different classes are merged). **Msgs** MSGID/DEF_NOTIFY/etc. names in the body (see `..\..\shared\messages\`). **Dlg** dialog ids enabled/disabled (ids in `..\data-structures.md`). **Files** path literals in the body.

## Overview

- DirectSound (verified): `YWSound` wraps the device, `CSoundBuffer` loads one `.wav` per buffer (sounds\C*.wav characters, M*.wav monsters, E*.wav effects). `PlaySound` pans and attenuates by distance from the player. Music: `StartBGM` picks the track from `m_cCurLocation` (Game.cpp:35458) and plays `music\*.wav`.

## Function table

| Function | Where | Lines | Purpose | Callers | Callees | Msgs | Dlg | Files |
|---|---|---|---|---|---|---|---|---|
| `DXC_dsound::DXC_dsound` | DXC_dsound.cpp:11-14 | 4 | comment: Construction/Destruction | - | - | - | - | - |
| `DXC_dsound::~DXC_dsound` | DXC_dsound.cpp:16-19 | 4 | (name only) | - | - | - | - | - |
| `CGame::PlaySound` | Game.cpp:22783-22817 | 35 | (name only) | CommandProcessor, CrusadeContributionResult, CrusadeWarResult, DlbBoxDoubleClick_Character, DlgBoxClick_Bank, DlgBoxClick_Character, DlgBoxClick_CityhallMenu, DlgBoxClick_Commander, +68 | Play | - | - | - |
| `CGame::StartBGM` | Game.cpp:35458-35496 | 39 | choose and loop the background track from m_cCurLocation: aresden, elvine, dungeon (dglv, middled1), middleland, else MainTm; Carol under DEF_XMAS with snow weather; no-op if sound is off or the track is already playing [V] | DlgBoxClick_SysMenu, OnKeyUp, SetWhetherStatus, UpdateScreen_OnGame | CSoundBuffer, Play, bStop | - | - | music\\Carol.wav, music\\MainTm.wav, music\\aresden.wav, music\\dungeon.wav, +2 |
| `CSoundBuffer::CSoundBuffer` | SoundBuffer.cpp:33-65 | 33 | comment: Construction/Destruction | StartBGM, UpdateScreen_OnLoading | - | - | - | - |
| `CSoundBuffer::~CSoundBuffer` | SoundBuffer.cpp:67-77 | 11 | (name only) | - | - | - | - | - |
| `CSoundBuffer::_bCreateSoundBuffer` | SoundBuffer.cpp:79-104 | 26 | comment: 이미 할당되어 있는 서페이스라면 또 읽지 않는다. | bCreateBuffer_LoadWavFileContents | - | - | - | - |
| `CSoundBuffer::bCreateBuffer_LoadWavFileContents` | SoundBuffer.cpp:106-142 | 37 | comment: 이미 할당되어 있는 버퍼에는 다시 로드하지 않는다. | GetIdleBuffer | _LoadWavContents, _bCreateSoundBuffer | - | - | - |
| `CSoundBuffer::_LoadWavContents` | SoundBuffer.cpp:144-174 | 31 | comment: v1.3 | bCreateBuffer_LoadWavFileContents | - | - | - | - |
| `CSoundBuffer::Play` | SoundBuffer.cpp:177-201 | 25 | (name only) | DrawDialogBox_SysMenu, PlaySound, SetWhetherStatus, StartBGM | GetIdleBuffer, SetVolume | - | - | - |
| `CSoundBuffer::GetIdleBuffer` | SoundBuffer.cpp:204-270 | 67 | comment: 사운드 버퍼 서페이스가 로스트 되었다. 서페이스를 복구한 후 로드하지 않고 없앤후 다시 할당한다. | Play | bCreateBuffer_LoadWavFileContents | - | - | - |
| `CSoundBuffer::SetVolume` | SoundBuffer.cpp:272-278 | 7 | (name only) | Play | - | - | - | - |
| `CSoundBuffer::bStop` | SoundBuffer.cpp:280-290 | 11 | (name only) | CommandProcessor, DlgBoxClick_SysMenu, DrawDialogBox_SysMenu, NotifyMsg_ForceDisconn, OnKeyUp, OnTimer, SetWhetherStatus, StartBGM, +4 | - | - | - | - |
| `CSoundBuffer::_ReleaseSoundBuffer` | SoundBuffer.cpp:292-301 | 10 | (name only) | ReleaseUnusedSprites | - | - | - | - |
| `YWSound::YWSound` | YWSound.cpp:13-16 | 4 | comment: Construction/Destruction | - | - | - | - | - |
| `YWSound::~YWSound` | YWSound.cpp:18-21 | 4 | (name only) | - | - | - | - | - |
| `YWSound::Create` | YWSound.cpp:26-74 | 49 | comment: YWSound Create | bInit | - | - | - | - |

# Functions: Magic, skills, effects, crusade

Source: `sources\client-2.20\` (librehelbreath ClientSources c01544d). 19 function definitions (including `#if` duplicate bodies). Generated from a col-0 signature scan, then reviewed by hand.

Column key: **Where** file:start-end (verified). **Purpose** from name pattern and the nearest source comment (inferred unless the row ends in [V], meaning re-checked against the function body by the 2026-10-25 review, or the overview says verified; comments are UTF-8 Korean in source, quoted as-is). **Callers/Callees** matched by bare name across all files (verified as text matches; same-named methods in different classes are merged). **Msgs** MSGID/DEF_NOTIFY/etc. names in the body (see `..\..\shared\messages\`). **Dlg** dialog ids enabled/disabled (ids in `..\data-structures.md`). **Files** path literals in the body.

## Overview

- `bAddNewEffect` allocates in `m_pEffectList`; `bEffectFrameCounter` advances them; `DrawEffects`/`DrawEffectLights` draw (effect sprite ids in `..\assets.md`). `CMagic`/`CSkill` are the config rows from magiccfg/skillcfg.
- Crusade UI (commander, constructor, soldier dialogs) and meteor-strike notices live here; the dialogs themselves are in `ui-dialogs.md`.

## Function table

| Function | Where | Lines | Purpose | Callers | Callees | Msgs | Dlg | Files |
|---|---|---|---|---|---|---|---|---|
| `CEffect::CEffect` | Effect.cpp:11-18 | 8 | comment: Construction/Destruction | - | - | - | - | - |
| `CEffect::~CEffect` | Effect.cpp:20-23 | 4 | (name only) | - | - | - | - | - |
| `CGame::bAddNewEffect` | Game.cpp:5891-6950 | 1060 | spawn a visual effect: skips types 8, 9, 11, 12, 14, 15 at detail level 0 (5899-5908), returns if the program is inactive (5911), takes the first free m_pEffectList slot and fills it per sType [V] | CommonEventHandler, DrawObjects, LogEventHandler, NotifyMsgHandler, bEffectFrameCounter, bSetDynamicObject, bSetItem, iObjectFrameCounter | PlaySound, SetCameraShakingEffect, cCalcDirection | - | - | - |
| `CGame::DrawEffects` | Game.cpp:6954-7732 | 779 | per live effect, a switch on m_sType that draws it through m_pEffectSpr[n] blitters [V] | UpdateScreen_OnGame | GetPoint, PutFadeSprite, PutRevTransSprite, PutSpriteFast, PutTransSprite, PutTransSprite2, PutTransSprite25, PutTransSprite25_NoColorKey, PutTransSprite50, PutTransSprite50_NoColorKey, +4 | - | - | - |
| `CGame::DrawEffectLights` | Game.cpp:7762-7937 | 176 | comment: Effect의 광원효과를 보조하는 효과이다. | UpdateScreen_OnGame | PutTransSprite, PutTransSprite25, PutTransSpriteRGB, PutTransSprite_NoColorKey | - | - | - |
| `CGame::bEffectFrameCounter` | Game.cpp:15881-16832 | 952 | comment: 프레임 갱신시간이 되었다. | UpdateScreen_OnGame | GetPoint, PlaySound, SetCameraShakingEffect, bAddNewEffect, cGetNextMoveDir | - | - | - |
| `CGame::SetCameraShakingEffect` | Game.cpp:19962-19975 | 14 | shake degree from distance [V] | bAddNewEffect, bEffectFrameCounter | - | - | - | - |
| `CGame::MeteorStrikeComing` | Game.cpp:19977-19993 | 17 | codes 1-4 show top message from game message 0 / 10 / 91 / 11 for the four nation cases (Elvine and non-Elvine for the Aresden strike, Aresden and non-Aresden for the Elvine strike) [V] | NotifyMsgHandler | SetTopMsg | - | - | - |
| `CGame::CrusadeWarResult` | Game.cpp:21177-21310 | 134 | comment: 텍스트 내용을 작성한다. | NotifyMsgHandler | CMsg, DisableDialogBox, EnableDialogBox, PlaySound | - | 18, 36, 37, 38 | - |
| `CGame::DrawWhetherEffects` | Game.cpp:23085-23172 | 88 | comment: 비내리는 효과 | UpdateScreen_OnGame | PutTransSprite | - | - | - |
| `CGame::_iGetWeaponSkillType` | Game.cpp:23769-23807 | 39 | comment: 현재 무장한 무기의 스킬 번호를 반환 | CommandProcessor, DrawDialogBoxs | - | - | - | - |
| `CGame::bItemDrop_SkillDialog` | Game.cpp:42210-42397 | 188 | drop of the dragged item onto SkillDialog; comment: v1.4 | _bCheckDraggingItemRelease | AddEventList, _bCheckCurrentBuildItemStatus | - | - | - |
| `CGame::GrandMagicResult` | Game.cpp:47247-47467 | 221 | comment: v2.15 건물의 HP를 보여주기 위해 수정함 | NotifyMsgHandler | CMsg, EnableDialogBox, PlaySound | - | 18 | - |
| `CGame::iGetManaCost` | Game.cpp:48009-48059 | 51 | spell mana cost: 1 if index outside 0..99; safe-attack mode adds iManaCost/2 - iManaCost/10 (about x1.4); equipped items named in a 10-entry list save a percent (10..30, 48021-48030); result floored at 1 (48055) [V] | DrawDialogBox_Magic, UseMagic | - | - | - | - |
| `CGame::UseMagic` | Game.cpp:48061-48093 | 33 | start casting: ordered rejections (index outside 0..99, not learned, HP <= 0, pointing mode, mana short, item in hand with message, skill in use with message), then toggles combat mode if needed and sets m_cCommand = DEF_OBJECTMAGIC and the cast/shortcut state (48061-48093) [V] | DlgBoxClick_Magic, OnKeyUp, UseShortCut | AddEventList, DisableDialogBox, _bIsItemOnHand, bSendCommand, iGetManaCost | DEF_COMMONTYPE_TOGGLECOMBATMODE, DEF_OBJECTMAGIC, MSGID_COMMAND_COMMON | 3 | - |
| `CMagic::CMagic` | Magic.cpp:11-14 | 4 | comment: Construction/Destruction | - | - | - | - | - |
| `CMagic::~CMagic` | Magic.cpp:16-18 | 3 | (name only) | - | - | - | - | - |
| `CSkill::CSkill` | Skill.cpp:11-18 | 8 | comment: Construction/Destruction | - | - | - | - | - |
| `CSkill::~CSkill` | Skill.cpp:20-23 | 4 | (name only) | - | - | - | - | - |

# Functions: Rendering primitives

Source: `sources\client-2.20\` (librehelbreath ClientSources c01544d). 65 function definitions (including `#if` duplicate bodies). Generated from a col-0 signature scan, then reviewed by hand.

Column key: **Where** file:start-end (verified). **Purpose** from name pattern and the nearest source comment (inferred unless the row ends in [V], meaning re-checked against the function body by the 2026-10-25 review, or the overview says verified; comments are UTF-8 Korean in source, quoted as-is). **Callers/Callees** matched by bare name across all files (verified as text matches; same-named methods in different classes are merged). **Msgs** MSGID/DEF_NOTIFY/etc. names in the body (see `..\..\shared\messages\`). **Dlg** dialog ids enabled/disabled (ids in `..\data-structures.md`). **Files** path literals in the body.

## Overview

- Full treatment in `..\rendering\directdraw.md` (surfaces, flip, pixel format), `..\rendering\blitters.md` (every `CSprite::Put*` routine and the .pak read path), `..\rendering\frame-pipeline.md` (frame order) and `..\rendering\porting-notes.md`. This file is only the function index.
- Sprite lifecycle (verified): `CSprite` constructor stores the .pak name and index; `_iOpenSprite` builds the surface on first draw via `CMyDib`; `_iCloseSprite` frees it; `CGame::ReleaseUnusedSprites` evicts after 60 s; `RestoreSprites` after lost surfaces.

## Function table

| Function | Where | Lines | Purpose | Callers | Callees | Msgs | Dlg | Files |
|---|---|---|---|---|---|---|---|---|
| `DXC_ddraw::DXC_ddraw` | DXC_ddraw.cpp:27-39 | 13 | comment: Construction/Destruction | - | - | - | - | - |
| `DXC_ddraw::~DXC_ddraw` | DXC_ddraw.cpp:41-52 | 12 | (name only) | - | - | - | - | - |
| `DXC_ddraw::bInit` | DXC_ddraw.cpp:54-190 | 137 | comment: 전체화면 모드이다. | Initialize, WndProc | InitFlipToGDI, _CalcMaxValue, _CalcMinValue, _TestPixelFormat, pCreateOffScreenSurface | - | - | - |
| `DXC_ddraw::iFlip` | DXC_ddraw.cpp:192-242 | 51 | comment: 서페이스가 손실되었다. 복구한다. | UpdateScreen_OnAgreement, UpdateScreen_OnChangePassword, UpdateScreen_OnConnecting, UpdateScreen_OnConnectionLost, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, UpdateScreen_OnGame, UpdateScreen_OnInputKeyCode, +13 | - | - | - | - |
| `DXC_ddraw::ChangeDisplayMode` | DXC_ddraw.cpp:244-328 | 85 | comment: 프론트 버퍼 생성 | OnSysKeyUp | InitFlipToGDI, pCreateOffScreenSurface | - | - | - |
| `DXC_ddraw::pCreateOffScreenSurface` | DXC_ddraw.cpp:330-344 | 15 | (name only) | ChangeDisplayMode, _pMakeSpriteSurface, bInit | - | - | - | - |
| `DXC_ddraw::iSetColorKey` | DXC_ddraw.cpp:346-353 | 8 | (name only) | _iOpenSprite | _dwColorMatch | - | - | - |
| `DXC_ddraw::iSetColorKey` | DXC_ddraw.cpp:355-362 | 8 | (name only) | _iOpenSprite | _dwColorMatch | - | - | - |
| `DXC_ddraw::_dwColorMatch` | DXC_ddraw.cpp:364-396 | 33 | (name only) | iSetColorKey | - | - | - | - |
| `DXC_ddraw::_dwColorMatch` | DXC_ddraw.cpp:398-417 | 20 | (name only) | iSetColorKey | - | - | - | - |
| `DXC_ddraw::TextOut` | DXC_ddraw.cpp:419-423 | 5 | (name only) | PutString, PutString2 | - | - | - | - |
| `DXC_ddraw::_TestPixelFormat` | DXC_ddraw.cpp:425-450 | 26 | comment: RGB 5:6:5 | bInit | - | - | - | - |
| `DXC_ddraw::_CalcMaxValue` | DXC_ddraw.cpp:452-489 | 38 | comment: v1.3 | bInit | - | - | - | - |
| `DXC_ddraw::_CalcMinValue` | DXC_ddraw.cpp:491-517 | 27 | comment: v1.3 | bInit | - | - | - | - |
| `DXC_ddraw::ClearBackB4` | DXC_ddraw.cpp:519-526 | 8 | (name only) | InitDataResponseHandler, UpdateScreen_OnChangePassword, UpdateScreen_OnConnecting, UpdateScreen_OnConnectionLost, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnGame, UpdateScreen_OnInputKeyCode, UpdateScreen_OnLoading_Progress, +12 | - | - | - | - |
| `DXC_ddraw::DrawShadowBox` | DXC_ddraw.cpp:528-579 | 52 | (name only) | DrawTopMsg, UpdateScreen_OnChangePassword, UpdateScreen_OnConnecting, UpdateScreen_OnGame, UpdateScreen_OnInputKeyCode, UpdateScreen_OnLogResMsg, UpdateScreen_OnQueryDeleteCharacter, UpdateScreen_OnQueryForceLogin, +1 | - | - | - | - |
| `DXC_ddraw::PutPixel` | DXC_ddraw.cpp:581-599 | 19 | (name only) | DrawDialogBox_GuideMap, UpdateScreen_OnGame | - | - | - | - |
| `DXC_ddraw::_GetBackBufferDC` | DXC_ddraw.cpp:601-607 | 7 | (name only) | ChatMsgHandler, DrawChatMsgBox, DrawChatMsgs, PutAlignedString, PutString, PutString2, ShowEventList | - | - | - | - |
| `DXC_ddraw::_ReleaseBackBufferDC` | DXC_ddraw.cpp:609-612 | 4 | (name only) | ChatMsgHandler, DrawChatMsgBox, DrawChatMsgs, PutAlignedString, PutString, PutString2, ShowEventList | - | - | - | - |
| `DXC_ddraw::DrawText` | DXC_ddraw.cpp:614-618 | 5 | comment: v2.15 | DrawChatMsgBox, PutAlignedString | - | - | - | - |
| `DXC_ddraw::InitFlipToGDI` | DXC_ddraw.cpp:620-655 | 36 | comment: v2.15 | ChangeDisplayMode, bInit | - | - | - | - |
| `DXC_ddraw::ColorTransferRGB` | DXC_ddraw.cpp:657-686 | 30 | (name only) | PutString_SprFont2 | - | - | - | - |
| `DXC_ddraw::Screenshot` | DXC_ddraw.cpp:690-837 | 148 | comment: GDI-compatible device context for the surface | CreateScreenShot | - | - | - | - |
| `CGame::RestoreSprites` | Game.cpp:1330-1334 | 5 | comment: 잃어버린 스프라이트 서페이스 모두를 복구시킨다. | UpdateScreen_OnAgreement, UpdateScreen_OnChangePassword, UpdateScreen_OnConnecting, UpdateScreen_OnConnectionLost, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, UpdateScreen_OnGame, UpdateScreen_OnInputKeyCode, +12 | iRestore | - | - | - |
| `CGame::_GetHairColorRGB` | Game.cpp:5150-5186 | 37 | (name only) | DrawDialogBox_Character, DrawObject_OnAttack, DrawObject_OnAttackMove, DrawObject_OnDamage, DrawObject_OnDamageMove, DrawObject_OnDead, DrawObject_OnDying, DrawObject_OnGetItem, +6 | - | - | - | - |
| `CGame::ReleaseUnusedSprites` | Game.cpp:15618-15660 | 43 | comment: 스프라이트 해제 | OnTimer | _ReleaseSoundBuffer, _iCloseSprite | - | - | - |
| `CGame::CreateScreenShot` | Game.cpp:25585-25606 | 22 | (name only) | OnKeyUp | AddEventList, Screenshot | - | - | Save\\HelShot%03d.bmp |
| `CGame::DrawVersion` | Game.cpp:33726-33741 | 16 | draw 'Ver', DEF_UPPERVERSION, '.', DEF_LOWERVERSION with sprite numerals at y 463 [V] | UpdateScreen_OnAgreement, UpdateScreen_OnChangePassword, UpdateScreen_OnConnecting, UpdateScreen_OnConnectionLost, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, UpdateScreen_OnInputKeyCode, UpdateScreen_OnLoading_Progress, +12 | ColorTransfer, PutString_SprNum, PutTransSpriteRGB | - | - | - |
| `CMyDib::CMyDib` | Mydib.cpp:9-36 | 28 | comment: KRN 파일로부터 클래스 객체를 생성한다. | - | - | - | - | sprites\\%s.pak |
| `CMyDib::~CMyDib` | Mydib.cpp:38-41 | 4 | (name only) | - | - | - | - | - |
| `CMyDib::PaintImage` | Mydib.cpp:43-47 | 5 | (name only) | _pMakeSpriteSurface, iRestore | - | - | - | - |
| `CSprite::CSprite` | Sprite.cpp:26-58 | 33 | comment: 정진광 수정, constructor OverLoad.. | MakeEffectSpr, MakeSprite, MakeTileSpr, UpdateScreen_OnLoading, bInit | - | - | - | - |
| `CSprite::~CSprite` | Sprite.cpp:60-64 | 5 | comment: 스프라이트 정보만을 읽고 이미지는 로딩하지 않는다. 최초로 그릴때 로딩한다. | - | - | - | - | - |
| `CSprite::_pMakeSpriteSurface` | Sprite.cpp:66-97 | 32 | comment: 컬러키값을 얻어온다. | _iOpenSprite | PaintImage, pCreateOffScreenSurface | - | - | - |
| `CSprite::PutSpriteFast` | Sprite.cpp:100-193 | 94 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawDialogBox_Bank, DrawDialogBox_Character, DrawDialogBox_Commander, DrawDialogBox_Constructor, DrawDialogBox_Exchange, DrawDialogBox_Fishing, DrawDialogBox_GuideMap, DrawDialogBox_IconPannel, +52 | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutSpriteFastDst` | Sprite.cpp:195-290 | 96 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | - | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutSpriteFastNoColorKey` | Sprite.cpp:293-387 | 95 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawDialogBox_GuideMap, DrawNewDialogBox | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutSpriteFastNoColorKeyDst` | Sprite.cpp:390-481 | 92 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawBackground | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutSpriteFastFrontBuffer` | Sprite.cpp:483-575 | 93 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | - | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutSpriteFastWidth` | Sprite.cpp:578-674 | 97 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawDialogBox_GaugePannel | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::iRestore` | Sprite.cpp:677-701 | 25 | comment: 서페이스가 할당되어있지 않다면 Restore할 필요가 없다. | RestoreSprites | PaintImage | - | - | - |
| `CSprite::PutShadowSprite` | Sprite.cpp:703-771 | 69 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawObject_OnAttack, DrawObject_OnAttackMove, DrawObject_OnDamage, DrawObject_OnDamageMove, DrawObject_OnDying, DrawObject_OnGetItem, DrawObject_OnMagic, DrawObject_OnMove, +4 | _iOpenSprite | - | - | - |
| `CSprite::PutShadowSpriteClip` | Sprite.cpp:774-870 | 97 | clipped shadow blit (sheared projection walked bottom-up); 32 call sites in Game.cpp, see rendering/blitters.md | DrawObject_OnAttack, DrawObject_OnAttackMove, DrawObject_OnDamage, DrawObject_OnDamageMove, DrawObject_OnDying, DrawObject_OnGetItem, DrawObject_OnMagic, DrawObject_OnMove, +3 | _iOpenSprite | - | - | - |
| `CSprite::PutTransSprite` | Sprite.cpp:873-999 | 127 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawDialogBox_Character, DrawDialogBoxs, DrawEffectLights, DrawEffects, DrawObjectFOE, DrawObject_OnAttack, DrawObject_OnAttackMove, DrawObject_OnDamage, +14 | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutTransSprite_NoColorKey` | Sprite.cpp:1001-1122 | 122 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawEffectLights, DrawEffects | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutTransSprite70` | Sprite.cpp:1124-1251 | 128 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawObject_OnMove | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutTransSprite70_NoColorKey` | Sprite.cpp:1254-1375 | 122 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawEffects, DrawObjects | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutTransSprite50` | Sprite.cpp:1377-1504 | 128 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawDialogBox_SkillDlg, DrawEffects | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutTransSprite50_NoColorKey` | Sprite.cpp:1507-1627 | 121 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawEffects, DrawObjects | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutTransSprite25` | Sprite.cpp:1629-1756 | 128 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawDialogBox_ItemUpgrade, DrawEffectLights, DrawEffects, DrawObject_OnGetItem, DrawObject_OnMove, DrawObject_OnRun, DrawObject_OnStop, UpdateScreen_OnQuit, +2 | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutTransSprite25_NoColorKey` | Sprite.cpp:1759-1879 | 121 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawEffects, DrawObjects | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutTransSprite2` | Sprite.cpp:1882-1996 | 115 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawDialogBox_Character, DrawDialogBox_GuideMap, DrawDialogBox_Inventory, DrawDialogBox_Map, DrawEffects, DrawNewDialogBox, DrawObject_OnDamageMove, DrawObject_OnGetItem, +5 | _iOpenSprite | - | - | - |
| `CSprite::PutShiftTransSprite2` | Sprite.cpp:1998-2115 | 118 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawDialogBox_GuideMap | _iOpenSprite | - | - | - |
| `CSprite::PutFadeSprite` | Sprite.cpp:2118-2230 | 113 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawEffects, DrawObjects | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutFadeSpriteDst` | Sprite.cpp:2232-2344 | 113 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | - | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::_iOpenSprite` | Sprite.cpp:2347-2366 | 20 | comment: 비트맵을 서페이스에 로드한다. | PutFadeSprite, PutFadeSpriteDst, PutRevTransSprite, PutShadowSprite, PutShadowSpriteClip, PutShiftSpriteFast, PutShiftTransSprite2, PutSpriteFast, +17 | _SetAlphaDegree, _pMakeSpriteSurface, iSetColorKey | - | - | - |
| `CSprite::_iCloseSprite` | Sprite.cpp:2368-2377 | 10 | (name only) | PutFadeSprite, PutFadeSpriteDst, PutRevTransSprite, PutShiftSpriteFast, PutSpriteFast, PutSpriteFastDst, PutSpriteFastFrontBuffer, PutSpriteFastNoColorKey, +15 | - | - | - | - |
| `CSprite::PutSpriteRGB` | Sprite.cpp:2379-2509 | 131 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawDialogBox_Bank, DrawDialogBox_Character, DrawDialogBox_Exchange, DrawDialogBox_Inventory, DrawDialogBox_ItemUpgrade, DrawDialogBox_SellorRepairItem, DrawObject_OnAttack, DrawObject_OnAttackMove, +16 | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutTransSpriteRGB` | Sprite.cpp:2512-2644 | 133 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawDialogBox_Character, DrawDialogBox_Inventory, DrawDialogBox_Skill, DrawDialogBox_SkillDlg, DrawEffectLights, DrawEffects, DrawObject_OnAttack, DrawObject_OnAttackMove, +13 | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutTransSpriteRGB_NoColorKey` | Sprite.cpp:2646-2772 | 127 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | - | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::_GetSpriteRect` | Sprite.cpp:2774-2838 | 65 | comment: v1.4 | DlbBoxDoubleClick_Inventory, DrawObjects, bDlgBoxPress_Inventory, bDlgBoxPress_SkillDlg | - | - | - | - |
| `CSprite::_SetAlphaDegree` | Sprite.cpp:2840-2931 | 92 | comment: 서페이스의 밝기가 바뀐다면 밝기를 조정한다. | PutFadeSprite, PutFadeSpriteDst, PutRevTransSprite, PutShiftSpriteFast, PutSpriteFast, PutSpriteFastDst, PutSpriteFastFrontBuffer, PutSpriteFastNoColorKey, +14 | - | - | - | - |
| `CSprite::_bCheckCollison` | Sprite.cpp:2933-3030 | 98 | (name only) | DlbBoxDoubleClick_Character, DrawDialogBox_Character, DrawObjects, bDlgBoxPress_Character, bDlgBoxPress_Inventory | - | - | - | - |
| `CSprite::PutShiftSpriteFast` | Sprite.cpp:3032-3128 | 97 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawDialogBox_GuideMap | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |
| `CSprite::PutRevTransSprite` | Sprite.cpp:3130-3259 | 130 | sprite blitter variant (clip / alpha / tint / shadow): see rendering/blitters.md | DrawEffects | _SetAlphaDegree, _iCloseSprite, _iOpenSprite | - | - | - |

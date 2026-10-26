# Functions: Config and contents loading

Source: `sources\client-2.20\` (librehelbreath ClientSources c01544d). 15 function definitions (including `#if` duplicate bodies). Generated from a col-0 signature scan, then reviewed by hand.

Column key: **Where** file:start-end (verified). **Purpose** from name pattern and the nearest source comment (inferred unless the row ends in [V], meaning re-checked against the function body by the 2026-10-25 review, or the overview says verified; comments are UTF-8 Korean in source, quoted as-is). **Callers/Callees** matched by bare name across all files (verified as text matches; same-named methods in different classes are merged). **Msgs** MSGID/DEF_NOTIFY/etc. names in the body (see `..\..\shared\messages\`). **Dlg** dialog ids enabled/disabled (ids in `..\data-structures.md`). **Files** path literals in the body.

## Overview

- Text configs are tokenised with `CStrTok`. Files and where each is read: see `..\assets.md` (contents table) and `..\build-config.md` (login.cfg and the hard-coded log server).
- `UpdateScreen_OnLoading` creates every `CSprite` in fixed id order across several frames (progress bar), using the `Make*Spr` helpers for multi-entry .pak files.

## Function table

| Function | Where | Lines | Purpose | Callers | Callees | Msgs | Dlg | Files |
|---|---|---|---|---|---|---|---|---|
| `CGame::MakeSprite` | Game.cpp:3542-3557 | 16 | (name only) | UpdateScreen_OnLoading | CSprite | - | - | sprites\\%s.pak |
| `CGame::MakeTileSpr` | Game.cpp:3559-3574 | 16 | (name only) | UpdateScreen_OnLoading | CSprite | - | - | sprites\\%s.pak |
| `CGame::MakeEffectSpr` | Game.cpp:3576-3591 | 16 | (name only) | UpdateScreen_OnLoading | CSprite | - | - | sprites\\%s.pak |
| `CGame::UpdateScreen_OnLoading` | Game.cpp:3596-4253 | 658 | per-frame update+draw for game mode Loading; comment: 정진광 수정, 함수 전체적으로..수정을 많이 했다.. | EventLoop, UpdateScreen | CSoundBuffer, CSprite, ChangeGameMode, MakeEffectSpr, MakeSprite, MakeTileSpr, UpdateScreen_OnLoading_Progress | - | - | Effect5.pak, sounds\\C%d.wav, sounds\\E%d.wav, sounds\\M%d.wav, +26 |
| `CGame::UpdateScreen_OnLoading_Progress` | Game.cpp:4255-4264 | 10 | per-frame update+draw for game mode Loading_Progress; comment: if ((m_cLoading%8) == 0) | UpdateScreen_OnLoading | ClearBackB4, DrawNewDialogBox, DrawVersion, PutSpriteFast, iFlip | - | - | - |
| `CGame::_LoadShopMenuContents` | Game.cpp:7939-7971 | 33 | (name only) | EnableDialogBox | __bDecodeContentsAndBuildItemForSaleList | - | - | .txt |
| `CGame::_bGetIsStringIsNumber` | Game.cpp:8640-8647 | 8 | (name only) | bInitMagicCfgList, bInitSkillCfgList | - | - | - | - |
| `CGame::bReadLoginConfigFile` | Game.cpp:15526-15616 | 91 | reads LoginConfigFile; comment: v2.20 내부태섭 | bInit | - | - | - | - |
| `CGame::_LoadTextDlgContents` | Game.cpp:18871-18925 | 55 | comment: cType에 따라 맞는 내용을 로드한다. | EnableDialogBox | CMsg, CStrTok, pGet | - | - | .txt |
| `CGame::_LoadGameMsgTextContents` | Game.cpp:18990-19041 | 52 | (name only) | bInit | CMsg, CStrTok, pGet | - | - | .txt |
| `CGame::_bCheckBadWords` | Game.cpp:23841-23856 | 16 | (name only) | UpdateScreen_OnCreateNewCharacter, _bDraw_OnCreateNewCharacter | bCheckBadWord | - | - | - |
| `CGame::_bDecodeBuildItemContents` | Game.cpp:23978-24024 | 47 | parses BuildItemContents; comment: v1.41 | bInit | __bDecodeBuildItemContents | - | - | .txt |
| `CGame::_bCheckBuildItemStatus` | Game.cpp:24026-24221 | 196 | comment: 현재 소지하고 있는 아이템으로 만들수 있는 BuildItem을 체크하여 DispBuildItemList를 작성한다. | EnableDialogBox, NotifyMsg_DropItemFin_EraseItem, NotifyMsg_GiveItemFin_EraseItem, NotifyMsg_ItemDepleted_EraseItem, NotifyMsg_ItemObtained | - | - | - | - |
| `CGame::bReadItemNameConfigFile` | Game.cpp:24707-24765 | 59 | reads ItemNameConfigFile | bInit | - | - | - | contents\\ItemName.cfg |
| `CGame::_LoadAgreementTextContents` | Game.cpp:26457-26507 | 51 | comment: cType에 따라 맞는 내용을 로드한다. | UpdateScreen_OnAgreement | CMsg, CStrTok, pGet | - | - | .txt |

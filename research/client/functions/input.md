# Functions: Input (keyboard, mouse, IME)

Source: `sources\client-2.20\` (librehelbreath ClientSources c01544d). 23 function definitions (including `#if` duplicate bodies). Generated from a col-0 signature scan, then reviewed by hand.

Column key: **Where** file:start-end (verified). **Purpose** from name pattern and the nearest source comment (inferred unless the row ends in [V], meaning re-checked against the function body by the 2026-10-25 review, or the overview says verified; comments are UTF-8 Korean in source, quoted as-is). **Callers/Callees** matched by bare name across all files (verified as text matches; same-named methods in different classes are merged). **Msgs** MSGID/DEF_NOTIFY/etc. names in the body (see `..\..\shared\messages\`). **Dlg** dialog ids enabled/disabled (ids in `..\data-structures.md`). **Files** path literals in the body.

## Overview

- **DirectInput** (verified): `DXC_dinput` owns the mouse only (position, buttons, wheel `m_sZ`, added v2.15 per GlobalDef.h:131). Keyboard comes through window messages: `WM_KEYDOWN`/`WM_KEYUP` to `OnKeyDown`/`OnKeyUp`, `WM_CHAR` and IME messages to the text input code.
- **Text entry**: `StartInputString` binds an edit target (buffer, length, position), `GetText`/`ReceiveString`/`EndInputString` handle typing and IME composition (Windows IME used when `DEF_USING_WIN_IME`).
- **Game input**: mouse handling for the world is inside `UpdateScreen_OnGame` (click to move, attack, cast, pick up); `CMouseInterface` holds clickable rectangles for menu screens; `UseShortCut` runs F-key shortcuts stored in the registry (see `startup.md`).

## Function table

| Function | Where | Lines | Purpose | Callers | Callees | Msgs | Dlg | Files |
|---|---|---|---|---|---|---|---|---|
| `DXC_dinput::DXC_dinput` | DXC_dinput.cpp:11-18 | 8 | comment: Construction/Destruction | - | - | - | - | - |
| `DXC_dinput::~DXC_dinput` | DXC_dinput.cpp:20-31 | 12 | (name only) | - | - | - | - | - |
| `DXC_dinput::bInit` | DXC_dinput.cpp:33-60 | 28 | (name only) | Initialize, WndProc | - | - | - | - |
| `DXC_dinput::SetAcquire` | DXC_dinput.cpp:63-73 | 11 | (name only) | WndProc | - | - | - | - |
| `DXC_dinput::UpdateMouseState` | DXC_dinput.cpp:75-94 | 20 | (name only) | OnKeyUp, UpdateScreen_OnAgreement, UpdateScreen_OnChangePassword, UpdateScreen_OnConnecting, UpdateScreen_OnConnectionLost, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, UpdateScreen_OnGame, +14 | - | - | - | - |
| `CGame::_iLoadTextDlgContents2` | Game.cpp:18929-18985 | 57 | comment: cType에 따라 맞는 내용을 로드한다. | EnableDialogBox | CMsg, CStrTok, pGet | - | - | .txt |
| `CGame::GetText` | Game.cpp:19292-19330 | 39 | (name only) | WndProc | - | - | - | - |
| `CGame::GetText` | Game.cpp:19334-19399 | 66 | comment: 그냥 여기저기서 써먹을 변수! | WndProc | GetCharKind | - | - | - |
| `CGame::GetCharKind` | Game.cpp:19410-19427 | 18 | comment: 출력 1 영문위치 | DrawChatMsgBox, GetText | - | - | - | - |
| `CGame::ClearInputString` | Game.cpp:19456-19463 | 8 | (name only); ZeroMemory(m_pInputBuffer, sizeof(m_pInputBuffer)) (19456-19463; [UI-003](../../issues/ui.md#ui-003-clearinputstring-zeroes-only-pointer-size-bytes)) [V] | OnKeyDown, UpdateScreen_OnChangePassword, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, UpdateScreen_OnGame, UpdateScreen_OnInputKeyCode, UpdateScreen_OnLogin | - | - | - | - |
| `CGame::StartInputString` | Game.cpp:19465-19488 | 24 | (name only) | CommandProcessor, DlgBoxClick_GuildMenu, EnableDialogBox, OnKeyDown, OnKeyUp, UpdateScreen_OnChangePassword, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, +3 | - | - | - | - |
| `CGame::EndInputString` | Game.cpp:19490-19512 | 23 | comment: 입력 에디트 컨트롤이 포커스를 잃었다. | CommandProcessor, DisableDialogBox, DlgBoxClick_GuildMenu, EnableDialogBox, OnKeyUp, UpdateScreen_OnChangePassword, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, +5 | - | - | - | - |
| `CGame::ReceiveString` | Game.cpp:19514-19521 | 8 | (name only) | UpdateScreen_OnGame | - | - | - | - |
| `CGame::OnSysKeyDown` | Game.cpp:28564-28578 | 15 | event handler SysKeyDown | WndProc | - | - | - | - |
| `CGame::OnSysKeyUp` | Game.cpp:28580-28608 | 29 | event handler SysKeyUp | WndProc | ChangeDisplayMode, DisableDialogBox, EnableDialogBox | - | 15 | - |
| `CGame::OnKeyUp` | Game.cpp:28610-29185 | 576 | event handler KeyUp | WndProc | AddEventList, CStrTok, CreateScreenShot, DisableDialogBox, EnableDialogBox, EndInputString, PlaySound, StartBGM, StartInputString, UpdateMouseState, +7 | DEF_COMMONTYPE_REQUEST_ACTIVATESPECABLTY, DEF_COMMONTYPE_REQ_USEITEM, DEF_COMMONTYPE_TOGGLECOMBATMODE, DEF_COMMONTYPE_TOGGLESAFEATTACKMODE, MSGID_COMMAND_CHATMSG, MSGID_COMMAND_COMMON | 1, 2, 3, 9, 10, 15, 18, 19, 35 | - |
| `CGame::OnKeyDown` | Game.cpp:29187-29254 | 68 | event handler KeyDown; comment: Page-Up | WndProc | ClearInputString, EnableDialogBox, StartInputString | - | 3 | - |
| `CGame::GetRegKey` | Game.cpp:47849-47863 | 15 | (name only) | GoHomepage | - | - | - | - |
| `CGame::UseShortCut` | Game.cpp:47938-48007 | 70 | comment: num : 0 - F1, 1 - F2, 2 - F3, 3 - F5, 4 - F6 | OnKeyUp | AddEventList, GetItemName, ItemEquipHandler, UseMagic | - | - | - |
| `CMouseInterface::CMouseInterface` | MouseInterface.cpp:11-17 | 7 | comment: Construction/Destruction | - | - | - | - | - |
| `CMouseInterface::~CMouseInterface` | MouseInterface.cpp:19-23 | 5 | (name only) | - | - | - | - | - |
| `CMouseInterface::AddRect` | MouseInterface.cpp:25-33 | 9 | (name only) | UpdateScreen_OnAgreement, UpdateScreen_OnChangePassword, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, UpdateScreen_OnInputKeyCode, UpdateScreen_OnLogResMsg, UpdateScreen_OnLogin, UpdateScreen_OnMainMenu, +6 | - | - | - | - |
| `CMouseInterface::iGetStatus` | MouseInterface.cpp:35-65 | 31 | comment: 아직 눌려진 버튼이 없다면 | UpdateScreen_OnAgreement, UpdateScreen_OnChangePassword, UpdateScreen_OnCreateNewAccount, UpdateScreen_OnCreateNewCharacter, UpdateScreen_OnInputKeyCode, UpdateScreen_OnLogResMsg, UpdateScreen_OnLogin, UpdateScreen_OnMainMenu, +6 | - | - | - | - |

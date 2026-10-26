# System: client UI and input

*Cohesive doc (screens, dialogs, HUD, input and cursor are one interaction loop); read whole.*

Program: client only (`sources/client-2.20/`). Tags: [V] verified in source by this pass; [Vd] verified
by the linked doc's own source review, not re-read here; [I] inferred; [U] unverified.

## Contents
- 1 Purpose
- 2 Subsystems
- 3 Functions
- 4 Commands and messages
- 5 Data
- 6 Flow
- 7 Rules and invariants
- 8 Couplings
- 9 Intent vs implementation
- 10 Open questions
- Doc discrepancies

## 1 Purpose

Everything the player clicks or types: the out-of-game screens (login, server and character select,
account creation), the in-game HUD, the 38 draggable dialog boxes (inventory, shops, guild,
crusade...), the mouse cursor that changes with what is under it, and the translation of clicks
into server requests. The design intent is a mouse-first MMO: one left-click walks, attacks, picks
up or casts depending on what the cursor touches; dialogs are free-floating windows the player
arranges; keyboard is for chat and hot keys. A recurring second intent is **guarding half-finished
server transactions**: dialogs that wait for a server reply cannot be closed or reused until the
reply arrives [V Game.cpp:17681-17683 comment; I for the general rule].

## 2 Subsystems

| Subsystem | Intent | Nests in |
|---|---|---|
| U1 Screen state machine | one `UpdateScreen_On<Mode>` per game mode; each screen draws and handles its own input | root |
| U2 Menu hit areas | `CMouseInterface` rectangles for out-of-game buttons | inside U1 menu screens |
| U3 Dialog manager | open/close, z-order, drag, per-dialog draw/press/click/double-click handlers | inside the OnGame screen |
| U4 HUD | always-on gauge panel (29) and icon panel (30), chat input bar, event list, top message, guide map | U3 plus fixed draws |
| U5 Cursor state machine | NULL / PRESSED / SELECTED / DRAGGING, click vs double-click vs drag, item drag-and-drop | drives U3 and U6 |
| U6 World command processor | turn the cursor's world target into one motion command, with client-side prediction | after U5 |
| U7 Keyboard and text entry | WM_KEY hot keys, function-key dialogs and shortcuts, edit-field binding, IME | parallel to U5 |
| U8 Cursor sprite | frame chosen while painting: normal, enemy, ally, pick-up, magic target, hourglass, skill | set in rendering |

## 3 Functions

Indexes: `../client/functions/game-modes.md`, `../client/functions/ui-dialogs.md`,
`../client/functions/input.md`.

| Sub | Function | Where | Role |
|---|---|---|---|
| U1 | `CGame::UpdateScreen` | Game.cpp:1106-1235 | dispatch on `m_cGameMode` [Vd] |
| U1 | `CGame::ChangeGameMode` | Game.cpp:15511-15524 | set mode, reset `m_cGameModeCount` (first-frame hooks key off it) [Vd] |
| U1 | `UpdateScreen_OnMainMenu/OnLogin/OnSelectServer/OnSelectCharacter/OnCreateNewCharacter/...` | Game.cpp:3391-3538, 27703-27999, 28395-28560, 21806-22084, 25923-26453 | English-build bodies of the menu screens [Vd `game-modes.md`] |
| U1 | `CGame::UpdateScreen_OnGame` | Game.cpp:34802-35456 | in-game screen: Enter/chat, draw, then `CommandProcessor` [Vd] |
| U2 | `CMouseInterface::AddRect` / `iGetStatus` | MouseInterface.cpp:25-33 / 35-65 | max 30 rectangles per screen [Vd] |
| U3 | `CGame::EnableDialogBox` | Game.cpp:18186-18598 | per-id open logic, clamp position, bring to top [V 18560-18598] |
| U3 | `CGame::DisableDialogBox` | Game.cpp:18600-18727 | per-id close and cleanup (may send a command) [Vd] |
| U3 | `CGame::DrawDialogBoxs` | Game.cpp:18016-18161 | draw back-to-front by `m_cDialogBoxOrder` [V 18016-18030] |
| U3 | `CGame::_iCheckDlgBoxFocus` | Game.cpp:17446-17686 | left: pick topmost dialog, start select/drag; right: close it unless locked [V] |
| U3 | `CGame::_bCheckDlgBoxClick` / `_bCheckDlgBoxDoubleClick` | Game.cpp:4351-4475 / 4477-4504 | route to `DlgBoxClick_X` / `DlbBoxDoubleClick_X` [Vd] |
| U3 | `CGame::_bCheckDraggingItemRelease` | Game.cpp:4595-4650 | item released over a dialog -> `bItemDrop_X`, else world drop/give [Vd] |
| U3 | `CGame::DrawNewDialogBox` | Game.cpp:19523-19534 | dialog frame sprite helper [Vd] |
| U4 | `DrawDialogBox_GaugePannel` / `DrawDialogBox_IconPannel` | Game.cpp:20150-20183 / 20026-20148 | HP/MP/SP bars, exp; icon buttons [Vd] |
| U4 | `CGame::DlgBoxClick_IconPannel` | Game.cpp:18741-18822 | icon buttons open dialogs, toggle combat mode [Vd] |
| U4 | `CGame::AddEventList` / `ShowEventList` | Game.cpp:5800 / 45466 | system message lines [V start lines] |
| U4 | `CGame::SetTopMsg` / `DrawTopMsg` | Game.cpp:20003 / 20012 | centred timed banner [V start lines] |
| U4 | `CGame::DrawDialogBox_GuideMap` | Game.cpp:17871-18014 | minimap (dialog 9) [Vd] |
| U5/U6 | `CGame::CommandProcessor` | Game.cpp:35701-~37090 | cursor state machine, then world command selection and send [V 35701-35835, 36846-37070] |
| U6 | `CGame::cGetNextMoveDir` / `_bCheckMoveable` | Game.cpp:1338-1400 / 1402-1429 | client-side path step [Vd] |
| U6 | `CGame::UseMagic` | Game.cpp:48061 | enter magic pointing mode [V start line] |
| U7 | `CGame::OnKeyDown` / `OnKeyUp` / `OnSysKeyUp` | Game.cpp:29187-29254 / 28610-29185 / 28580-28608 | hot keys [V key cases] |
| U7 | `CGame::UseShortCut` | Game.cpp:47938-48007 | F-key item/magic shortcut slots [Vd] |
| U7 | `StartInputString` / `EndInputString` / `GetText` / `ReceiveString` | Game.cpp:19465-19521, 19292-19399 | edit-field binding and IME [Vd] |
| U7 | `DXC_dinput::UpdateMouseState` | DXC_dinput.cpp:75-94 | DirectInput mouse x/y/buttons/wheel [Vd] |

## 4 Commands and messages

UI is the origin of most client->server traffic; it does not own the domains.
- `MSGID_COMMAND_MOTION` with `DEF_OBJECTMOVE/RUN/ATTACK/ATTACKMOVE/GETITEM/MAGIC/STOP` from
  `CommandProcessor` (Game.cpp:36833, 36945, 37001, 37030, 37059, 37070) [V] - movement/combat systems.
- `MSGID_COMMAND_COMMON` with ~40 `DEF_COMMONTYPE_*` from dialog click handlers (table in
  `../client/functions/ui-dialogs.md` "Msgs" column) [Vd]; `TOGGLECOMBATMODE` (icon panel, Tab-less
  key path), `TOGGLESAFEATTACKMODE` (Home key, Game.cpp:29100-29103) [V].
- `MSGID_COMMAND_CHATMSG` from the chat bar, at most every 700 ms (Game.cpp:35074) [Vd].
- `MSGID_REQUEST_PANNING` from observer-mode screen edges (Game.cpp:35713-35733) [V].
- Out-of-game screens: `MSGID_REQUEST_LOGIN`, `CREATENEWACCOUNT`, `CREATENEWCHARACTER`,
  `DELETECHARACTER`, `ENTERGAME`, `CHANGEPASSWORD`, `INPUTKEYCODE` to the log server (account system,
  `../servers/client-log-protocol.md`) [Vd].
- Inbound: many `DEF_NOTIFY_*` open dialogs (e.g. 23 sell/repair price, 27 exchange, 8 guild op,
  21 NPC talk) - `client-state.md` and `../client/functions/networking.md` "Dlg" column.
- Client-local `/showframe`, `/enabletogglescreen` (`commands/by-system.md` ui-only).

## 5 Data

- `m_cGameMode` and `DEF_GAMEMODE_*` -2..21 (Game.h:104-126); `m_cGameModeCount` [Vd].
- `m_stDialogBoxInfo[41]` (Game.h:566; defaults Game.cpp:543-784): position, size, `cMode`
  (per-dialog sub-state, also the lock), `sView` scroll, `sV1..` scratch, `cStr` [Vd].
  `m_bIsDialogEnabled[41]`, `m_cDialogBoxOrder[42]`; dialog id table in
  `../client/data-structures.md` [Vd].
- `m_stMCursor` (Game.h:542-554): `sCursorFrame`, `cPrevStatus`, `cSelectedObjectType`,
  `sSelectedObjectID`, drag offsets, `dwSelectClickTime`, `sClickX/Y` [V].
- `m_cCommand`, `m_sCommX/Y`, `m_wCommObjectID`, `m_bCommandAvailable`, `m_cCommandCount`,
  `m_bIsGetPointingMode`, `m_iPointCommandType` [V Game.cpp:473-485, 860, 2910-2931].
- Registry `HKCU\Software\Siementech\Helbreath\Settings`: `Magic`, `ShortCut0..4` (Game.cpp:320-438) [Vd].
- Text: `contents\*.txt` for text dialogs (`_iLoadTextDlgContents2`, Game.cpp:18929) [Vd].

## 6 Flow

**Screen machine** (English build): Loading -> MainMenu -> SelectServer -> Login -> (log server)
SelectCharacter -> (ENTERGAME, game socket) WaitInitData -> Game; side screens CreateNewAccount,
Agreement, CreateNewCharacter, QueryDeleteCharacter, QueryForceLogin, ChangePassword, LogResMsg,
ConnectionLost, VersionNotMatch, Quit [Vd `game-modes.md`; account traffic in
`../servers/client-log-protocol.md`]. `InitDataResponseHandler` ends in
`ChangeGameMode(ONMAINGAME)` (Game.cpp:46049) [V].

**Click a dialog button** (cursor machine in `CommandProcessor`, Game.cpp:35749-35835) [V]:
1. Button down, status NULL: `_iCheckDlgBoxFocus(..,1)` scans `m_cDialogBoxOrder[40..0]`; hit ->
   `EnableDialogBox` brings it to top, records drag offset, status SELECTED; miss -> PRESSED (world).
2. Button held and moved while SELECTED -> DRAGGING (panels 29/30 refuse to drag; dragging guild
   menu 7 or drop-amount 17 in input mode ends text input) (35800-35833).
3. Button up while SELECTED: same spot within `DEF_DOUBLECLICKTIME` -> `_bCheckDlgBoxDoubleClick`,
   else `_bCheckDlgBoxClick` -> `DlgBoxClick_X` -> usually `bSendCommand` (35776-35796).
   If an item was being carried, `_bCheckDraggingItemRelease` routes the drop (35790-35795).
4. Server replies with a notify; handler updates state and maybe re-opens/updates the dialog
   (`client-state.md`).

**Click the world** (Game.cpp:35915-37070) [V]: target chosen from the picking results of the
frame just drawn (`rendering.md`) -> `m_cCommand` = MOVE / RUN (shift or run mode, needs SP >= 1) /
ATTACK / ATTACKMOVE / GETITEM / MAGIC -> gates (alive, `m_bCommandAvailable`, `m_cCommandCount < 6`)
-> `bSendCommand(MSGID_COMMAND_MOTION...)` -> own position advanced immediately and
`bSetOwner` re-places the player (36945-36960). Confirm/reject handling: `client-state.md`.

**Right-click on a dialog**: `_iCheckDlgBoxFocus(..,2)` closes it unless it is 5, 6, 8, 12, 23
(mode >= 3), 24, 27, 33, 34, or 32 in mode 1/3 (Game.cpp:17680-17683) [V].

**Hot keys** (English build, `DEF_SHORTCUT` not defined, GlobalDef.h:181-183) [V]: F1 help (35),
F2/F3 `UseShortCut(1/2)`, F4 cast `m_sMagicShortCut`, F5 character (1), F6 inventory (2), F7 magic
(3), F8 skill (15), F9 chat (10), F12 system menu (19), Ins red potion, Del blue potion, Home safe
attack toggle, Ctrl+H help request, PrintScreen screenshot, Esc (Shift+Esc in observer logs out)
(Game.cpp:28784-29110). Alt held = super attack (`m_bSuperAttackMode`, 35740-35743).

## 7 Rules and invariants

- Command throttle: warning text at 5 outstanding commands, no new motion at 6
  (Game.cpp:36852-36854); `m_bCommandAvailable` re-enabled only when the player's animation ends
  (Game.cpp:35412-35415) [V / Vd]. This is a one-action-per-animation rule enforced client-side.
- Lag kick: `m_cCommandCount >= 6` at seven consecutive checks -> ConnectionLost (Game.cpp:4288-4301; comment
  "14 seconds"). The 2 s gate compares against the last-receive stamp `m_dwTime`, so the check runs on every 1 s
  tick while the server is silent: about 7 s under silence, ~14 s only with steady traffic [V code, I effect].
- No motion while standing on a teleport tile with nothing outstanding (Game.cpp:35806, 35953) [V].
- Polymorphed players (type > 6) cannot run, cast or pick up (Game.cpp:36856-36865) [V].
- Clicking an unwalkable tile within 2 tiles stops instead of pathing (v2.15, Game.cpp:36925-36928) [V].
- Dialog open clamps: y > 400 -> 410, x > 620 -> 620, nudge if almost off-screen (Game.cpp:18573-18580) [V].
- HUD panels occupy order slots 39/40 and normal dialogs only use 0..38, so HUD is always on top
  and hit-tested first (Game.cpp:949-950, 18584-18596, 17455) [V].
- Dialogs are not drawn in observer mode (Game.cpp:18021) [V].

## 8 Couplings

| Direction | Coupling point | Other system |
|---|---|---|
| out | `bSendCommand` (Game.cpp:1431) with MSGID/COMMONTYPE | every gameplay system on the server: movement, combat, item, shop, trade, guild, party, crusade, quest, magic, skills (other clusters) [U paths] |
| out | log-server requests from menu screens | account system (`../servers/client-log-protocol.md`) |
| in | picking results `m_sMCX/Y`, `m_cMCName`, `m_stMCursor.sCursorFrame` written by `DrawObjects` | `rendering.md` |
| in | dialog content: `m_pItemList`, `m_pBankList`, `m_pItemForSaleList`, `m_stQuest`, party/guild lists | `client-state.md` |
| in | notifies that open/unlock dialogs (`EnableDialogBox` from `NotifyMsgHandler`) | `client-state.md` |
| out | `PlaySound('E', 14, 5)` UI click (231 call sites in Game.cpp) [V], `StartBGM`/`bStop` from system menu | `audio.md` |
| in | `CMapData::bIsTeleportLoc`, `m_tile[][].m_bIsMoveAllowed` used for path steps | `client-state.md` (map mirror) |

## 9 Current behaviour vs intent

- The double-click branch assigns `dwSelectClickTime` to itself; the value is overwritten at 35792 ([UI-001](../issues/ui.md#ui-001-double-click-branch-assigns-dwselectclicktime-to-itself)).
- `EnableDialogBox` compacts the order array in a single pass (Game.cpp:18584-18590): one entry is nulled per call and each later entry shifts left, so the array stays contiguous (`[A,B,C,D]` minus B gives `[A,C,D]`); `DisableDialogBox` uses the same loop (`18717-18723`) [V].
- Comments at Game.cpp:949-950 swap the panel ids ([UI-002](../issues/ui.md#ui-002-panel-id-comments-are-swapped)).
- `ClearInputString` zeroes only pointer-size bytes ([UI-003](../issues/ui.md#ui-003-clearinputstring-zeroes-only-pointer-size-bytes)).
- Click-to-move prediction moves the client before the server agrees and snaps back on reject ([CLIENT-STATE-002](../issues/client-state.md#client-state-002-prediction-without-a-reconciliation-queue)).
- Function-key layout depends on `DEF_SHORTCUT` ([UI-004](../issues/ui.md#ui-004-function-key-layout-is-a-build-switch)).
- Input is processed after the flip using last frame's picking, so a click acts on what was drawn, not on where objects are now (one frame stale) [Vd frame-pipeline step 11].

## 10 Open questions

- `CommandProcessor` end line not confirmed (next definition begins at Game.cpp:37103) [U].
- F11 and some Ctrl combinations not traced [U].
- (Resolved: `DEF_DOUBLECLICKTIME` is 300 ms, Game.h:139 [V].)

## Doc discrepancies

- `../client/functions/input.md` says world mouse handling is "inside `UpdateScreen_OnGame`"; it is in
  `CommandProcessor` (Game.cpp:35701), called from `UpdateScreen_OnGame` (frame-pipeline step 11).
- `../client/rendering/frame-pipeline.md` "bottom slots 39-40" for the HUD panels: they are the
  topmost (see Rules).

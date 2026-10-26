# Values: full-screen menus (summary)

Class MENU: absolute coordinates on the 640x480 menu canvases (login, server select, character
select/create, account creation, password change, loading, quit, message screens). They are
consistent with each other and with the 640x480 background art, so the proposal is **not** to
re-anchor them one by one but to draw each menu at `(MENU_X0, MENU_Y0)` = centred canvas and
offset the mouse by the same amount (one change per screen; see `change-plan.md`).
Full-screen dims on these screens are class SCREEN (`values-screen-game.md`).
Every row is in `values-menus-lines.md`.

Generated from `.claude\scratch\resolution\classified.tsv` by `res_gendocs.py`; every row is a
source line found by the enumeration in `method.md` (verified by grep against
`sources\client-2.20`, line numbers as of this pass). Variable names are defined in `variables.md`.
The Build column is the preprocessor result (`ppeval.py`): `default` = compiled in the as-shipped
build (`DEF_LANGUAGE 4`, GlobalDef.h:1, no `_DEBUG`); `only lang N` / `only dbg` / `only opt` = compiled
only in other builds (other languages, `_DEBUG`, or an optional switch); `dead in all builds` = no
build compiles it; `commented` = inside a comment. See `method.md`, Build conditionals.

Rows: 891 (by build: default 411, other builds only 287, dead in all 127, commented 66)

| Function | Rows | First-last line |
|---|---|---|
| UpdateScreen_OnCreateNewAccount | 245 | Game.cpp:26652-27691 |
| UpdateScreen_OnCreateNewCharacter | 113 | Game.cpp:25944-26446 |
| UpdateScreen_OnSelectServer | 103 | Game.cpp:28018-28554 |
| UpdateScreen_OnSelectCharacter | 97 | Game.cpp:21563-29724 |
| UpdateScreen_OnChangePassword | 76 | Game.cpp:32689-33276 |
| UpdateScreen_OnLogResMsg | 56 | Game.cpp:31526-31801 |
| _bDraw_OnCreateNewCharacter | 44 | Game.cpp:25827-25918 |
| UpdateScreen_OnMainMenu | 34 | Game.cpp:3135-3532 |
| UpdateScreen_OnInputKeyCode | 20 | Game.cpp:48114-48390 |
| UpdateScreen_OnLogin | 15 | Game.cpp:27726-27994 |
| UpdateScreen_OnQueryDeleteCharacter | 14 | Game.cpp:29850-29898 |
| UpdateScreen_OnQueryForceLogin | 12 | Game.cpp:29346-29388 |
| _Draw_OnLogin | 12 | Game.cpp:45416-45454 |
| UpdateScreen_OnVersionNotMatch | 10 | Game.cpp:33688-33723 |
| _Draw_UpdateScreen_OnCreateNewAccount | 7 | Game.cpp:21315-21321 |
| UpdateScreen_OnConnecting | 6 | Game.cpp:25686-25709 |
| UpdateScreen_OnWaitingResponse | 6 | Game.cpp:29802-29826 |
| DrawVersion | 5 | Game.cpp:33730-33740 |
| UpdateScreen_OnWaitInitData | 4 | Game.cpp:25754-25762 |
| UpdateScreen_OnLoading_Progress | 3 | Game.cpp:4258-4262 |
| UpdateScreen_OnAgreement | 3 | Game.cpp:26578-26594 |
| UpdateScreen_OnQuit | 3 | Game.cpp:29297-29299 |
| UpdateScreen_OnConnectionLost | 2 | Game.cpp:25793-25795 |
| UpdateScreen_OnMsg | 1 | Game.cpp:15464-15464 |

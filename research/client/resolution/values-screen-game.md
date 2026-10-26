# Values: full-screen rectangles in game code

Class SCREEN rows outside the platform layer: full-screen darkening boxes (`DrawShadowBox(0,0,639,479)`),
full-screen hit rectangles, the top message bar, line clipping. All become SCREEN_W-1 / SCREEN_H-1.
Most `DrawShadowBox(0,0,639,479)` rows are commented-out fade code; they are listed so a
search-and-replace does not miss them if re-enabled.
Generated from `.claude\scratch\resolution\classified.tsv` by `res_gendocs.py`; every row is a
source line found by the enumeration in `method.md` (verified by grep against
`sources\client-2.20`, line numbers as of this pass). Variable names are defined in `variables.md`.
The Build column is the preprocessor result (`ppeval.py`): `default` = compiled in the as-shipped
build (`DEF_LANGUAGE 4`, GlobalDef.h:1, no `_DEBUG`); `only lang N` / `only dbg` / `only opt` = compiled
only in other builds (other languages, `_DEBUG`, or an optional switch); `dead in all builds` = no
build compiles it; `commented` = inside a comment. See `method.md`, Build conditionals.


Rows: 54 (by build: default 22, other builds only 6, dead in all 0, commented 26)

| Where | Function | Code | Proposed | Note | Build |
|---|---|---|---|---|---|
| Game.cpp:3261 | UpdateScreen_OnMainMenu | `//  if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:3262 | UpdateScreen_OnMainMenu | `//  if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:3383 | UpdateScreen_OnMainMenu | `//  if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:3384 | UpdateScreen_OnMainMenu | `//  if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:3534 | UpdateScreen_OnMainMenu | `//  if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:3535 | UpdateScreen_OnMainMenu | `//  if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:20015 | DrawTopMsg | `m_DDraw.DrawShadowBox(0, 0, 639, 30);` | SCREEN_W-1 | top message bar full width | default |
| Game.cpp:20018 | DrawTopMsg | `PutAlignedString(0, 639, 10, m_cTopMsg, 255,255,255);` | SCREEN_W-1 | top message bar full width | default |
| Game.cpp:21798 | UpdateScreen_OnSelectCharacter | `//  if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:21799 | UpdateScreen_OnSelectCharacter | `//  if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:22080 | UpdateScreen_OnSelectCharacter | `//  if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:22081 | UpdateScreen_OnSelectCharacter | `//  if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:22826 | _DrawBlackRect | `dcx = 40 - iSize*2;` | SCREEN_W/16 - 2*iSize, SCREEN_H/16 - 2*iSize, SCREEN_H-16-16*iSize, SCREEN_W-16-16*iSize | _DrawBlackRect border cells (dead code: only caller is commented out, Game.cpp:35316) | default |
| Game.cpp:22827 | _DrawBlackRect | `dcy = 30 - iSize*2;` | SCREEN_W/16 - 2*iSize, SCREEN_H/16 - 2*iSize, SCREEN_H-16-16*iSize, SCREEN_W-16-16*iSize | _DrawBlackRect border cells (dead code: only caller is commented out, Game.cpp:35316) | default |
| Game.cpp:22834 | _DrawBlackRect | `m_pSprite[DEF_SPRID_MOUSECURSOR]->PutSpriteFast(ix*16 + sx, 464 - iSize*16, 12, dwTime);` | SCREEN_W/16 - 2*iSize, SCREEN_H/16 - 2*iSize, SCREEN_H-16-16*iSize, SCREEN_W-16-16*iSize | _DrawBlackRect border cells (dead code: only caller is commented out, Game.cpp:35316) | default |
| Game.cpp:22839 | _DrawBlackRect | `m_pSprite[DEF_SPRID_MOUSECURSOR]->PutSpriteFast(624 - iSize*16, iy*16 + sy, 12, dwTime);` | SCREEN_W/16 - 2*iSize, SCREEN_H/16 - 2*iSize, SCREEN_H-16-16*iSize, SCREEN_W-16-16*iSize | _DrawBlackRect border cells (dead code: only caller is commented out, Game.cpp:35316) | default |
| Game.cpp:23347 | DrawLine | `if ((iResultX >= 0) && (iResultX < 640) && (iResultY >= 0) && (iResultY < 480)) {` | SCREEN_W, SCREEN_H | DrawLine per-pixel clip | default |
| Game.cpp:23381 | DrawLine | `if ((iResultX >= 0) && (iResultX < 640) && (iResultY >= 0) && (iResultY < 480)) {` | SCREEN_W, SCREEN_H | DrawLine per-pixel clip | default |
| Game.cpp:23455 | DrawLine2 | `if ((iResultX >= 0) && (iResultX < 640) && (iResultY >= 0) && (iResultY < 480)) {` | SCREEN_W, SCREEN_H | DrawLine per-pixel clip | default |
| Game.cpp:23489 | DrawLine2 | `if ((iResultX >= 0) && (iResultX < 640) && (iResultY >= 0) && (iResultY < 480)) {` | SCREEN_W, SCREEN_H | DrawLine per-pixel clip | default |
| Game.cpp:25700 | UpdateScreen_OnConnecting | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | default |
| Game.cpp:26449 | UpdateScreen_OnCreateNewCharacter | `//  if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:26450 | UpdateScreen_OnCreateNewCharacter | `//  if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:26628 | UpdateScreen_OnAgreement | `//  if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:26629 | UpdateScreen_OnAgreement | `//  if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:27141 | UpdateScreen_OnCreateNewAccount | `//  if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:27142 | UpdateScreen_OnCreateNewAccount | `//  if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:27693 | UpdateScreen_OnCreateNewAccount | `//  if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:27694 | UpdateScreen_OnCreateNewAccount | `//  if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:28164 | UpdateScreen_OnSelectServer | `//  if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:28165 | UpdateScreen_OnSelectServer | `//  if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:28387 | UpdateScreen_OnSelectServer | `//  if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:28388 | UpdateScreen_OnSelectServer | `//  if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:28556 | UpdateScreen_OnSelectServer | `//  if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:28557 | UpdateScreen_OnSelectServer | `//  if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:29279 | UpdateScreen_OnQuit | `pMI->AddRect(0,0,640,480);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | default |
| Game.cpp:29327 | UpdateScreen_OnQuit | `//  if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:29328 | UpdateScreen_OnQuit | `//  if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu (commented) | commented |
| Game.cpp:29369 | UpdateScreen_OnQueryForceLogin | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | default |
| Game.cpp:29372 | UpdateScreen_OnQueryForceLogin | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | default |
| Game.cpp:29373 | UpdateScreen_OnQueryForceLogin | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | default |
| Game.cpp:29816 | UpdateScreen_OnWaitingResponse | `m_DDraw.DrawShadowBox(0,0, 639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | default |
| Game.cpp:29874 | UpdateScreen_OnQueryDeleteCharacter | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | default |
| Game.cpp:29877 | UpdateScreen_OnQueryDeleteCharacter | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | default |
| Game.cpp:29878 | UpdateScreen_OnQueryDeleteCharacter | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | default |
| Game.cpp:31635 | UpdateScreen_OnLogResMsg | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | default |
| Game.cpp:32847 | UpdateScreen_OnChangePassword | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | only lang 2 |
| Game.cpp:32850 | UpdateScreen_OnChangePassword | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | only lang 2 |
| Game.cpp:32851 | UpdateScreen_OnChangePassword | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | only lang 2 |
| Game.cpp:33158 | UpdateScreen_OnChangePassword | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | default |
| Game.cpp:33671 | UpdateScreen_OnVersionNotMatch | `pMI->AddRect(0,0,640,480);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | default |
| Game.cpp:48255 | UpdateScreen_OnInputKeyCode | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | only lang 2 |
| Game.cpp:48258 | UpdateScreen_OnInputKeyCode | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | only lang 2 |
| Game.cpp:48259 | UpdateScreen_OnInputKeyCode | `m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W(-1), SCREEN_H(-1) | full-screen dim / full-screen hit rect on a menu | only lang 2 |

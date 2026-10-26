# Values: DirectDraw, window and input

Class SCREEN and BUFFER rows in the platform layer (surfaces, display mode, window placement,
mouse clamp, windowed cursor offset). True screen-dependent: each must follow SCREEN_W/SCREEN_H.
Generated from `.claude\scratch\resolution\classified.tsv` by `res_gendocs.py`; every row is a
source line found by the enumeration in `method.md` (verified by grep against
`sources\client-2.20`, line numbers as of this pass). Variable names are defined in `variables.md`.
The Build column is the preprocessor result (`ppeval.py`): `default` = compiled in the as-shipped
build (`DEF_LANGUAGE 4`, GlobalDef.h:1, no `_DEBUG`); `only lang N` / `only dbg` / `only opt` = compiled
only in other builds (other languages, `_DEBUG`, or an optional switch); `dead in all builds` = no
build compiles it; `commented` = inside a comment. See `method.md`, Build conditionals.


Rows: 33 (by build: default 24, other builds only 5, dead in all 0, commented 4)

| Where | Function | Code | Proposed | Note | Build |
|---|---|---|---|---|---|
| DXC_ddraw.cpp:60 | bInit | `SetRect(&m_rcClipArea, 0,0, 640, 480);` | SCREEN_W,SCREEN_H | initial software clip rect | default |
| DXC_ddraw.cpp:72 | bInit | `ddVal = m_lpDD4->SetDisplayMode(640, 480, 16,0,0);` | SCREEN_W,SCREEN_H | SetDisplayMode 640x480x16 | default |
| DXC_ddraw.cpp:88 | bInit | `SetRect(&m_rcFlipping, 0, 0, 640, 480);` | SCREEN_W,SCREEN_H | fullscreen flip source rect | default |
| DXC_ddraw.cpp:101 | bInit | `if(cy>280) cy -= 40;` | WIN_Y_BIAS | cy>280 then cy-=40 window placement | default |
| DXC_ddraw.cpp:102 | bInit | `SetWindowPos( hWnd, HWND_TOP, cx-320, cy-240, 640, 480, SWP_SHOWWINDOW);` | SCREEN_W,SCREEN_H; WIN_X=cx-SCREEN_W/2 | windowed SetWindowPos size+centre | default |
| DXC_ddraw.cpp:114 | bInit | `SetRect(&m_rcFlipping, cx-320, cy-240, cx+320, cy+240);` | cx-SCREEN_W/2 .. cx+SCREEN_W/2 | windowed blit destination rect | default |
| DXC_ddraw.cpp:119 | bInit | `m_lpBackB4 = pCreateOffScreenSurface(640, 480);` | SCREEN_W,SCREEN_H | back buffer m_lpBackB4 allocation | default |
| DXC_ddraw.cpp:123 | bInit | `m_lpPDBGS = pCreateOffScreenSurface(640+32, 480+32);` | PDBGS_W=SCREEN_W+TILE, PDBGS_H=SCREEN_H+TILE | tile cache surface | default |
| DXC_ddraw.cpp:271 | ChangeDisplayMode | `if(cy>280) cy -= 40;` | WIN_Y_BIAS | cy>280 then cy-=40 window placement | default |
| DXC_ddraw.cpp:272 | ChangeDisplayMode | `SetWindowPos( hWnd, NULL, cx-320, cy-240, 640, 480, SWP_SHOWWINDOW);` | SCREEN_W,SCREEN_H; WIN_X=cx-SCREEN_W/2 | windowed SetWindowPos size+centre | default |
| DXC_ddraw.cpp:284 | ChangeDisplayMode | `SetRect(&m_rcFlipping, cx-320, cy-240, cx+320, cy+240);` | cx-SCREEN_W/2 .. cx+SCREEN_W/2 | windowed blit destination rect | default |
| DXC_ddraw.cpp:294 | ChangeDisplayMode | `ddVal = m_lpDD4->SetDisplayMode(640, 480, 16,0,0);` | SCREEN_W,SCREEN_H | SetDisplayMode 640x480x16 | default |
| DXC_ddraw.cpp:310 | ChangeDisplayMode | `SetRect(&m_rcFlipping, 0, 0, 640, 480);` | SCREEN_W,SCREEN_H | fullscreen flip source rect | default |
| DXC_ddraw.cpp:315 | ChangeDisplayMode | `m_lpBackB4 = pCreateOffScreenSurface(640, 480);` | SCREEN_W,SCREEN_H | back buffer m_lpBackB4 allocation | default |
| DXC_ddraw.cpp:319 | ChangeDisplayMode | `m_lpPDBGS = pCreateOffScreenSurface(640+32, 480+32);` | PDBGS_W=SCREEN_W+TILE, PDBGS_H=SCREEN_H+TILE | tile cache surface | default |
| DXC_ddraw.cpp:524 | ClearBackB4 | `memset((char *)ddsd2.lpSurface, 0, ddsd2.lPitch * 480);` | SCREEN_H | ClearBackB4 memset lPitch*480 | default |
| DXC_ddraw.cpp:585 | PutPixel | `if ((sX < 0) \|\| (sY < 0) \|\| (sX > 639) \|\| (sY > 479)) return;` | SCREEN_W-1,SCREEN_H-1 | PutPixel bounds | default |
| DXC_dinput.cpp:87 | UpdateMouseState | `if (m_sX > 639) m_sX = 639;` | SCREEN_W-1,SCREEN_H-1 | DirectInput mouse clamp | default |
| DXC_dinput.cpp:88 | UpdateMouseState | `if (m_sY > 479) m_sY = 479;` | SCREEN_W-1,SCREEN_H-1 | DirectInput mouse clamp | default |
| Game.cpp:35275 | UpdateScreen_OnGame | `if(cy>280) cy -= 40;` | WIN_X/WIN_Y (cx-SCREEN_W/2, cy-SCREEN_H/2) | windowed cursor sprite offset; HTML-dialog build only (language 2, DEF_HTMLCOMMOM), runs only while G_bDlg1 | only lang 2 |
| Game.cpp:35276 | UpdateScreen_OnGame | `m_pSprite[DEF_SPRID_MOUSECURSOR]->PutSpriteFast(G_point.x - (cx-320), G_point.y - (cy-240), m_stMCursor.sCursorFrame, dwTime);` | WIN_X/WIN_Y (cx-SCREEN_W/2, cy-SCREEN_H/2) | windowed cursor sprite offset; HTML-dialog build only (language 2, DEF_HTMLCOMMOM), runs only while G_bDlg1 | only lang 2 |
| Game.cpp:35278 | UpdateScreen_OnGame | `//    G_hWnd = CreateWindowEx( 0, szAppClass, "HClient", WS_POPUP \| WS_BORDER, cx-320, cy-240,` | WIN_X/WIN_Y (cx-SCREEN_W/2, cy-SCREEN_H/2) | windowed cursor sprite offset; HTML-dialog build only (language 2, DEF_HTMLCOMMOM), runs only while G_bDlg1 (commented) | commented |
| Game.cpp:35279 | UpdateScreen_OnGame | `//          640, 480, NULL, NULL, hInstance, NULL );` | WIN_X/WIN_Y (cx-SCREEN_W/2, cy-SCREEN_H/2) | windowed cursor sprite offset; HTML-dialog build only (language 2, DEF_HTMLCOMMOM), runs only while G_bDlg1 (commented) | commented |
| Game.cpp:35374 | UpdateScreen_OnGame | `if (m_cGameModeCount < 6) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W-1,SCREEN_H-1 | fade-in full-screen darken | default |
| Game.cpp:35375 | UpdateScreen_OnGame | `if (m_cGameModeCount < 2) m_DDraw.DrawShadowBox(0,0,639,479);` | SCREEN_W-1,SCREEN_H-1 | fade-in full-screen darken | default |
| Wmain.cpp:197 | WndProc | `::SetCursorPos(510, 400);` | CURSOR_HOME_X/Y (HUD-relative) | fullscreen SetCursorPos(510,400) after IME; HTML-dialog build only (language 2, DEF_HTMLCOMMOM), runs only while G_bDlg1 | only lang 2 |
| Wmain.cpp:203 | WndProc | `if(cy>280) cy -= 40;` | WIN_X/WIN_Y + CURSOR_HOME | windowed SetCursorPos origin; HTML-dialog build only (language 2), runs only while G_bDlg1 | only lang 2 |
| Wmain.cpp:204 | WndProc | `::SetCursorPos(510 + (cx-320), 400 + (cy-240));` | WIN_X/WIN_Y + CURSOR_HOME | windowed SetCursorPos origin; HTML-dialog build only (language 2), runs only while G_bDlg1 | only lang 2 |
| Wmain.cpp:328 | InitInstance | `if(cy>280) cy -= 40;` | WIN_Y_BIAS | cy>280 then cy-=40 window placement | default |
| Wmain.cpp:330 | InitInstance | `G_hWnd = CreateWindowEx(NULL, szAppClass, "Helbreath Crusade", WS_POPUP, cx-320, cy-240,` | SCREEN_W,SCREEN_H; WIN_X/WIN_Y | CreateWindowEx 640x480 popup | default |
| Wmain.cpp:331 | InitInstance | `640, 480, NULL, NULL, hInstance, NULL);` | SCREEN_W,SCREEN_H; WIN_X/WIN_Y | CreateWindowEx 640x480 popup | default |
| Wmain.cpp:333 | InitInstance | `//  G_hWnd = CreateWindowEx( 0, szAppClass, "HClient", WS_POPUP \| WS_BORDER, cx-320, cy-240,` | SCREEN_W,SCREEN_H; WIN_X/WIN_Y | CreateWindowEx 640x480 popup (commented) | commented |
| Wmain.cpp:334 | InitInstance | `//        640, 480, NULL, NULL, hInstance, NULL );` | SCREEN_W,SCREEN_H; WIN_X/WIN_Y | CreateWindowEx 640x480 popup (commented) | commented |

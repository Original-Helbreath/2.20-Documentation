# Values: world view (tiles visible, centre, overscan, server window)

Class VIEW rows: everything that decides which world cells are drawn, where the player sits on
screen, the listener for sound, picking, weather and off-screen spawn heights, and the
server-coupled map-data window. Read `change-plan.md` (server coupling) before changing any.
Generated from `.claude\scratch\resolution\classified.tsv` by `res_gendocs.py`; every row is a
source line found by the enumeration in `method.md` (verified by grep against
`sources\client-2.20`, line numbers as of this pass). Variable names are defined in `variables.md`.
The Build column is the preprocessor result (`ppeval.py`): `default` = compiled in the as-shipped
build (`DEF_LANGUAGE 4`, GlobalDef.h:1, no `_DEBUG`); `only lang N` / `only dbg` / `only opt` = compiled
only in other builds (other languages, `_DEBUG`, or an optional switch); `dead in all builds` = no
build compiles it; `commented` = inside a comment. See `method.md`, Build conditionals.


Rows: 152 (by build: default 151, other builds only 1, dead in all 0, commented 0)

| Where | Function | Code | Proposed | Note | Build |
|---|---|---|---|---|---|
| Game.cpp:2299 | DrawObjects | `indexY = sDivY + sPivotY - 7;` | OVR_TT=7, OVR_LT=4 (overscan tiles) | DrawObjects start indices | default |
| Game.cpp:2300 | DrawObjects | `for (iy = -sModY-224; iy <= 427+352; iy += 32) {` | VIEW_W, VIEW_H, overscan OVR_L=128 OVR_T=224 OVR_B=352 | DrawObjects loop bounds | default |
| Game.cpp:2301 | DrawObjects | `indexX = sDivX + sPivotX-4;` | OVR_TT=7, OVR_LT=4 (overscan tiles) | DrawObjects start indices | default |
| Game.cpp:2302 | DrawObjects | `for (ix = -sModX-128 ; ix <= 640 + 128; ix += 32)` | VIEW_W, VIEW_H, overscan OVR_L=128 OVR_T=224 OVR_B=352 | DrawObjects loop bounds | default |
| Game.cpp:2308 | DrawObjects | `if ((ix >= -sModX) && (ix <= 640+16) && (iy >= -sModY) && (iy <= 427+32+16)) {` | VIEW_W+16, VIEW_H+48 | DrawObjects per-cell visibility test | default |
| Game.cpp:2390 | DrawObjects | `if ((bContact == TRUE) && (msY <= 431)) {` | VIEW_H+4 | object picking only above icon panel (msY<=431) | default |
| Game.cpp:2513 | DrawObjects | `if ((bContact == TRUE) && (msY <= 431)) {` | VIEW_H+4 | object picking only above icon panel (msY<=431) | default |
| Game.cpp:2542 | DrawObjects | `m_sViewDstX = (indexX*32) - 288 - 32;` | VIEW_CENTER_X (=288+32), VIEW_CENTER_Y | view easing target | default |
| Game.cpp:2543 | DrawObjects | `m_sViewDstY = (indexY*32) - 224;` | VIEW_CENTER_X (=288+32), VIEW_CENTER_Y | view easing target | default |
| Game.cpp:5913 | bAddNewEffect | `sAbsX = abs(((m_sViewPointX / 32) + 10) - dX);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:5914 | bAddNewEffect | `sAbsY = abs(((m_sViewPointY / 32) + 7) - dY);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:5962 | bAddNewEffect | `sAbsX = abs(((m_sViewPointX / 32) + 10) - sX);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:5963 | bAddNewEffect | `sAbsY = abs(((m_sViewPointY / 32) + 7)  - sY);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:5968 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - sX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:5979 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:5980 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:5987 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - sX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6000 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6001 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6008 | bAddNewEffect | `lPan = -(320 - (sX - m_sViewPointX))*1000;` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6020 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6021 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6028 | bAddNewEffect | `lPan = -(320 - (sX - m_sViewPointX))*1000;` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6130 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6131 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6167 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6168 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6175 | bAddNewEffect | `lPan = -(320 - (sX - m_sViewPointX))*1000;` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6187 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6188 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6194 | bAddNewEffect | `lPan = -(320 - (sX - m_sViewPointX))*1000;` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6223 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6224 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6241 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6242 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6248 | bAddNewEffect | `lPan = ((sX - m_sViewPointX)-320)*30;` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6265 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6266 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6272 | bAddNewEffect | `lPan = ((sX - m_sViewPointX)-320)*30;` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6286 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6287 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6293 | bAddNewEffect | `lPan = ((sX - m_sViewPointX)-320)*30;` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6303 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6304 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6310 | bAddNewEffect | `lPan = ((sX - m_sViewPointX)-320)*30;` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6328 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6329 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6335 | bAddNewEffect | `lPan = ((sX - m_sViewPointX)-320)*30;` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6345 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6346 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6352 | bAddNewEffect | `lPan = ((sX - m_sViewPointX)-320)*30;` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6371 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6372 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6378 | bAddNewEffect | `lPan = ((sX - m_sViewPointX)-320)*30;` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6408 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6409 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6416 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - sX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6460 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6461 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6468 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - sX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6488 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6489 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6496 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - sX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6510 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6511 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6518 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - (sX/32))*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6529 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6530 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6537 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - (sX/32))*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6549 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6550 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6566 | bAddNewEffect | `sAbsX = abs(320 - (sX - m_sViewPointX));` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6567 | bAddNewEffect | `sAbsY = abs(240 - (sY - m_sViewPointY));` | LISTEN_Y = SCREEN_H/2 (not VIEW_CENTER_Y) | sound distance listener y | default |
| Game.cpp:6573 | bAddNewEffect | `lPan = ((sX - m_sViewPointX)-320)*30;` | LISTEN_X = SCREEN_W/2 | sound pan/distance listener x | default |
| Game.cpp:6627 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6661 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6672 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6682 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6695 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6705 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6719 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6745 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6761 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6766 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6791 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6819 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6827 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6842 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6867 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:6897 | bAddNewEffect | `lPan = -(((m_sViewPointX / 32) + 10) - dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:15852 | DrawBackground | `SetRect(&m_DDraw.m_rcClipArea, 0,0, 640+32, 480+32);` | PDBGS_W/H; SCREEN_W/H | clip rect swap around tile cache (15867 restores 640x480, same value as DXC_ddraw.cpp:60) | default |
| Game.cpp:15854 | DrawBackground | `for (iy = -sModY; iy < 427+48 ; iy += 32)` | VIEW_H+48 / SCREEN_W+48 | DrawBackground tile loop bounds | default |
| Game.cpp:15857 | DrawBackground | `for (ix = -sModX; ix < 640+48 ; ix += 32)` | VIEW_H+48 / SCREEN_W+48 | DrawBackground tile loop bounds | default |
| Game.cpp:15867 | DrawBackground | `SetRect(&m_DDraw.m_rcClipArea, 0,0, 640, 480);` | PDBGS_W/H; SCREEN_W/H | clip rect swap around tile cache (15867 restores 640x480, same value as DXC_ddraw.cpp:60) | default |
| Game.cpp:15870 | DrawBackground | `SetRect(&rcRect, sModX, sModY, 640+sModX, 480+sModY);` | SCREEN_W,SCREEN_H | BltFast source rect from tile cache | default |
| Game.cpp:16532 | bEffectFrameCounter | `lPan = -(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16570 | bEffectFrameCounter | `sAbsX = abs(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16571 | bEffectFrameCounter | `sAbsY = abs(((m_sViewPointY / 32) + 7)  - m_pEffectList[i]->m_dY);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16576 | bEffectFrameCounter | `lPan = -(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX)*1000;` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16607 | bEffectFrameCounter | `sAbsX = abs(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16608 | bEffectFrameCounter | `sAbsY = abs(((m_sViewPointY / 32) + 7)  - m_pEffectList[i]->m_dY);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16613 | bEffectFrameCounter | `lPan = -(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16675 | bEffectFrameCounter | `sAbsX = abs(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16676 | bEffectFrameCounter | `sAbsY = abs(((m_sViewPointY / 32) + 7)  - m_pEffectList[i]->m_dY);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16681 | bEffectFrameCounter | `lPan = -(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16722 | bEffectFrameCounter | `sAbsX = abs(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16723 | bEffectFrameCounter | `sAbsY = abs(((m_sViewPointY / 32) + 7)  - m_pEffectList[i]->m_dY);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16728 | bEffectFrameCounter | `lPan = -(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16747 | bEffectFrameCounter | `sAbsX = abs(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16748 | bEffectFrameCounter | `sAbsY = abs(((m_sViewPointY / 32) + 7)  - m_pEffectList[i]->m_dY);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:16753 | bEffectFrameCounter | `lPan = -(((m_sViewPointX / 32) + 10) - m_pEffectList[i]->m_dX);` | SRV_CENTER_TX / SRV_CENTER_TY | centre tile from viewpoint | default |
| Game.cpp:23147 | DrawWhetherEffects | `if( dY == 478 - 53 )` | VIEW_H-2 | X-mas snow pile line (478-53) | only opt |
| Game.cpp:23213 | WhetherObjectFrameCounter | `m_stWhetherObject[i].sX    = (m_pMapData->m_sPivotX*32) + ((rand() % 940) - 200) + 300;` | WEATHER_SPAN_X=VIEW_W+300, spawn offset | weather spawn box around pivot | default |
| Game.cpp:23214 | WhetherObjectFrameCounter | `m_stWhetherObject[i].sY    = (m_pMapData->m_sPivotY*32) + ((rand() % 800) - 600) + 240;` | WEATHER_SPAN_X=VIEW_W+300, spawn offset | weather spawn box around pivot | default |
| Game.cpp:23248 | WhetherObjectFrameCounter | `m_stWhetherObject[i].sX    = (m_pMapData->m_sPivotX*32) + ((rand() % 940) - 200) + 300;` | WEATHER_SPAN_X=VIEW_W+300, spawn offset | weather spawn box around pivot | default |
| Game.cpp:23249 | WhetherObjectFrameCounter | `m_stWhetherObject[i].sY    = (m_pMapData->m_sPivotY*32) + ((rand() % 800) - 600) + 600;` | WEATHER_SPAN_X=VIEW_W+300, spawn offset | weather spawn box around pivot | default |
| Game.cpp:30531 | NotifyMsgHandler | `for( i=0 ; i<36 ; i++ ) bAddNewEffect(60, m_sViewPointX +(rand() % 640), m_sViewPointY +(rand() % 480), NULL, NULL, -(rand() % 80));` | SCREEN_W, SCREEN_H | random effect spawn across the view | default |
| Game.cpp:35119 | UpdateScreen_OnGame | `DrawChatMsgs(-100, 0, 800, 600);` | -100,0,SCREEN_W+160,SCREEN_H+120 | chat bubble cull rect | default |
| Game.cpp:35550 | MotionResponseHandler | `m_sViewDstX = m_sViewPointX = (m_sPlayerX-10)*32;` | SRV_CENTER_TX, SRV_CENTER_TY | snap viewpoint to player (player-10, player-7) | default |
| Game.cpp:35551 | MotionResponseHandler | `m_sViewDstY = m_sViewPointY = (m_sPlayerY-7)*32;` | SRV_CENTER_TX, SRV_CENTER_TY | snap viewpoint to player (player-10, player-7) | default |
| Game.cpp:35674 | MotionResponseHandler | `m_sViewDstX = m_sViewPointX = (m_sPlayerX-10)*32;` | SRV_CENTER_TX, SRV_CENTER_TY | snap viewpoint to player (player-10, player-7) | default |
| Game.cpp:35675 | MotionResponseHandler | `m_sViewDstY = m_sViewPointY = (m_sPlayerY-7)*32;` | SRV_CENTER_TX, SRV_CENTER_TY | snap viewpoint to player (player-10, player-7) | default |
| Game.cpp:35716 | CommandProcessor | `if ((msX == 0) && (msY == 0) && (m_sViewDstX > 32*21) && (m_sViewDstY > 32*16)) bSendCommand(MSGID_REQUEST_PANNING, NULL, 8, NULL, NULL, NULL, NULL);` | SCREEN_W/H edge, VIEW_TILES_X/Y | observer panning at screen edge (msX==640 never true: mouse clamps at 639) | default |
| Game.cpp:35718 | CommandProcessor | `if ((msX == 640) && (msY == 0) && (m_sViewDstX < 32*m_pMapData->m_sMapSizeX - 32*21) && (m_sViewDstY > 32*16)) bSendCommand(MSGID_REQUEST_PANNING, NUL` | SCREEN_W/H edge, VIEW_TILES_X/Y | observer panning at screen edge (msX==640 never true: mouse clamps at 639) | default |
| Game.cpp:35720 | CommandProcessor | `if ((msX == 640) && (msY == 480) && (m_sViewDstX < 32*m_pMapData->m_sMapSizeX - 32*21) && (m_sViewDstY < 32*m_pMapData->m_sMapSizeY - 32*16)) bSendCom` | SCREEN_W/H edge, VIEW_TILES_X/Y | observer panning at screen edge (msX==640 never true: mouse clamps at 639) | default |
| Game.cpp:35722 | CommandProcessor | `if ((msX == 0) && (msY == 480)) bSendCommand(MSGID_REQUEST_PANNING, NULL, 6, NULL, NULL, NULL, NULL);` | SCREEN_W/H edge, VIEW_TILES_X/Y | observer panning at screen edge (msX==640 never true: mouse clamps at 639) | default |
| Game.cpp:35724 | CommandProcessor | `if ((msX == 0) && (m_sViewDstX > 32*21)) bSendCommand(MSGID_REQUEST_PANNING, NULL, 7, NULL, NULL, NULL, NULL);` | SCREEN_W/H edge, VIEW_TILES_X/Y | observer panning at screen edge (msX==640 never true: mouse clamps at 639) | default |
| Game.cpp:35726 | CommandProcessor | `if ((msX == 640) && (m_sViewDstX < 32*m_pMapData->m_sMapSizeX - 32*21)) bSendCommand(MSGID_REQUEST_PANNING, NULL, 3, NULL, NULL, NULL, NULL);` | SCREEN_W/H edge, VIEW_TILES_X/Y | observer panning at screen edge (msX==640 never true: mouse clamps at 639) | default |
| Game.cpp:35728 | CommandProcessor | `if ((msY == 0) && (m_sViewDstY > 32*16)) bSendCommand(MSGID_REQUEST_PANNING, NULL, 1, NULL, NULL, NULL, NULL);` | SCREEN_W/H edge, VIEW_TILES_X/Y | observer panning at screen edge (msX==640 never true: mouse clamps at 639) | default |
| Game.cpp:35730 | CommandProcessor | `if ((msY == 480) && (m_sViewDstY < 32*m_pMapData->m_sMapSizeY - 32*16)) bSendCommand(MSGID_REQUEST_PANNING, NULL, 5, NULL, NULL, NULL, NULL);` | SCREEN_W/H edge, VIEW_TILES_X/Y | observer panning at screen edge (msX==640 never true: mouse clamps at 639) | default |
| Game.cpp:45532 | ShowEventList | `PutString(280 -29, 280 -52, SHOW_EVENT_LIST1, RGB(235,235,235), FALSE, 1, TRUE);` | VIEW_CENTER_X-69, VIEW_CENTER_Y+4 | 'skill in use' text near player | default |
| Game.cpp:45987 | InitDataResponseHandler | `m_sPlayerX   = sX + 14 + 5;` | SRV_WIN_X0+SRV_CENTER_TX, SRV_WIN_Y0+SRV_CENTER_TY | player = pivot+19,+17 (server-coupled) | default |
| Game.cpp:45988 | InitDataResponseHandler | `m_sPlayerY   = sY + 12 + 5;` | SRV_WIN_X0+SRV_CENTER_TX, SRV_WIN_Y0+SRV_CENTER_TY | player = pivot+19,+17 (server-coupled) | default |
| Game.cpp:46001 | InitDataResponseHandler | `m_sViewDstX = m_sViewPointX = (sX+4+5)*32;` | SRV_WIN_X0, SRV_WIN_Y0 | initial viewpoint = (pivot+9, pivot+10)*32 | default |
| Game.cpp:46002 | InitDataResponseHandler | `m_sViewDstY = m_sViewPointY = (sY+5+5)*32;` | SRV_WIN_X0, SRV_WIN_Y0 | initial viewpoint = (pivot+9, pivot+10)*32 | default |
| Game.cpp:46005 | InitDataResponseHandler | `_ReadMapData(sX + 4 + 5, sY + 5 + 5, cp);` | SRV_WIN_X0, SRV_WIN_Y0 | initial map data read at pivot+(9,10) (server-coupled) | default |
| MapData.cpp:812 | ShiftMapData | `memcpy(&m_pTmpData[4+5+ix][6+5+iy], &m_pData[4+5+ix][5+5+iy], sizeof(class CTile));` | SRV_WIN_X0/Y0, SRV_WIN_W/H | ShiftMapData copies the 21x16 server window at offset (9,10) | default |
| MapData.cpp:818 | ShiftMapData | `memcpy(&m_pTmpData[4+5+ix][6+5+iy], &m_pData[5+5+ix][5+5+iy], sizeof(class CTile));` | SRV_WIN_X0/Y0, SRV_WIN_W/H | ShiftMapData copies the 21x16 server window at offset (9,10) | default |
| MapData.cpp:825 | ShiftMapData | `memcpy(&m_pTmpData[4+5+ix][5+5+iy], &m_pData[5+5+ix][5+5+iy], sizeof(class CTile));` | SRV_WIN_X0/Y0, SRV_WIN_W/H | ShiftMapData copies the 21x16 server window at offset (9,10) | default |
| MapData.cpp:831 | ShiftMapData | `memcpy(&m_pTmpData[4+5+ix][5+5+iy], &m_pData[5+5+ix][6+5+iy], sizeof(class CTile));` | SRV_WIN_X0/Y0, SRV_WIN_W/H | ShiftMapData copies the 21x16 server window at offset (9,10) | default |
| MapData.cpp:838 | ShiftMapData | `memcpy(&m_pTmpData[4+5+ix][5+5+iy], &m_pData[4+5+ix][6+5+iy], sizeof(class CTile));` | SRV_WIN_X0/Y0, SRV_WIN_W/H | ShiftMapData copies the 21x16 server window at offset (9,10) | default |
| MapData.cpp:844 | ShiftMapData | `memcpy(&m_pTmpData[5+5+ix][5+5+iy], &m_pData[4+5+ix][6+5+iy], sizeof(class CTile));` | SRV_WIN_X0/Y0, SRV_WIN_W/H | ShiftMapData copies the 21x16 server window at offset (9,10) | default |
| MapData.cpp:851 | ShiftMapData | `memcpy(&m_pTmpData[5+5+ix][5+5+iy], &m_pData[4+5+ix][5+5+iy], sizeof(class CTile));` | SRV_WIN_X0/Y0, SRV_WIN_W/H | ShiftMapData copies the 21x16 server window at offset (9,10) | default |
| MapData.cpp:857 | ShiftMapData | `memcpy(&m_pTmpData[5+5+ix][6+5+iy], &m_pData[4+5+ix][5+5+iy], sizeof(class CTile));` | SRV_WIN_X0/Y0, SRV_WIN_W/H | ShiftMapData copies the 21x16 server window at offset (9,10) | default |
| MapData.cpp:1732 | iObjectFrameCounter | `sCenterX = (sVal / 32) + 10;` | SRV_CENTER_TX, SRV_CENTER_TY+1 | animation LOD centre tile (+10,+8) | default |
| MapData.cpp:1734 | iObjectFrameCounter | `sCenterY = (sVal / 32) + 8;` | SRV_CENTER_TX, SRV_CENTER_TY+1 | animation LOD centre tile (+10,+8) | default |
| MapData.cpp:3403 | bSetItem | `sAbsX = abs(((m_pGame->m_sViewPointX / 32) + 10) - sX);` | SRV_CENTER_TX, SRV_CENTER_TY | sound distance centre tile (+10,+7) | default |
| MapData.cpp:3404 | bSetItem | `sAbsY = abs(((m_pGame->m_sViewPointY / 32) + 7)  - sY);` | SRV_CENTER_TX, SRV_CENTER_TY | sound distance centre tile (+10,+7) | default |
| MapData.h:26 | (file scope) | `#define MAPDATASIZEX 40` | MAPDATA_W/H = VIEW_TILES + 2*margin | client map window 40x35 (server-coupled) | default |
| MapData.h:27 | (file scope) | `#define MAPDATASIZEY 35` | MAPDATA_W/H = VIEW_TILES + 2*margin | client map window 40x35 (server-coupled) | default |
| Sprite.cpp:742 | PutShadowSprite | `if( iSangX >= 0 && iSangX < 640 && iSangY >= 0 && iSangY < 427 )` | SCREEN_W, VIEW_H | PutShadowSprite per-pixel clip x<640 y<427 | default |
| Sprite.cpp:760 | PutShadowSprite | `if( iSangX >= 0 && iSangX < 640 && iSangY >= 0 && iSangY < 427 )` | SCREEN_W, VIEW_H | PutShadowSprite per-pixel clip x<640 y<427 | default |

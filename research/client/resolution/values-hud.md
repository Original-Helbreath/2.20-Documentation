# Values: HUD (icon panel, gauges, chat bar, event lists)

Class HUD rows are absolute coordinates of the bottom icon panel (art 640x53 at y 427), the
gauge panel, the chat input bar and the lower event list. They should become
`HUD_X0 + x` and `HUD_Y0 + (y - 427)` (or `VIEW_H - k` for the chat rows). Class TOPLEFT rows
are anchored to (0,0) and need no change; they are listed for completeness.
Generated from `.claude\scratch\resolution\classified.tsv` by `res_gendocs.py`; every row is a
source line found by the enumeration in `method.md` (verified by grep against
`sources\client-2.20`, line numbers as of this pass). Variable names are defined in `variables.md`.
The Build column is the preprocessor result (`ppeval.py`): `default` = compiled in the as-shipped
build (`DEF_LANGUAGE 4`, GlobalDef.h:1, no `_DEBUG`); `only lang N` / `only dbg` / `only opt` = compiled
only in other builds (other languages, `_DEBUG`, or an optional switch); `dead in all builds` = no
build compiles it; `commented` = inside a comment. See `method.md`, Build conditionals.


Rows: 81 (by build: default 73, other builds only 8, dead in all 0, commented 0)

| Where | Function | Code | Proposed | Note | Build |
|---|---|---|---|---|---|
| Game.cpp:721 | CGame | `m_stDialogBoxInfo[29].sX = 0;` | HUD_X0, HUD_Y0+7 (434) | gauge panel dialog 29 position | default |
| Game.cpp:722 | CGame | `m_stDialogBoxInfo[29].sY = 434;` | HUD_X0, HUD_Y0+7 (434) | gauge panel dialog 29 position | default |
| Game.cpp:727 | CGame | `m_stDialogBoxInfo[30].sX = 0;` | HUD_X0, HUD_Y0 (427), width 640 | icon panel dialog 30 position/size | default |
| Game.cpp:728 | CGame | `m_stDialogBoxInfo[30].sY = 427;` | HUD_X0, HUD_Y0 (427), width 640 | icon panel dialog 30 position/size | default |
| Game.cpp:729 | CGame | `m_stDialogBoxInfo[30].sSizeX = 640;` | HUD_X0, HUD_Y0 (427), width 640 | icon panel dialog 30 position/size | default |
| Game.cpp:7749 | bItemDrop_IconPannel | `if ((453 < msX) && (486 > msX) && (440 < msY) && (475 > msY)) {` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:7754 | bItemDrop_IconPannel | `if ((425 < msX) && (448 > msX) && (440 < msY) && (475 > msY)) {` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:18149 | DrawDialogBoxs | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutTransSprite(368, 439, 3, m_dwCurTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:18152 | DrawDialogBoxs | `PutString_SprFont2(380, 454, G_cTxt, 220, 200, 200);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:18158 | DrawDialogBoxs | `PutString_SprFont2(380, 454, G_cTxt, 10, 10, 10);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:18748 | DlgBoxClick_IconPannel | `if ((msX > 322) && (msX < 355) && (434 < msY) && (475 > msY)) {` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:18769 | DlgBoxClick_IconPannel | `if ((362 < msX) && (404 > msX) && (434 < msY) && (475 > msY)) {` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:18776 | DlgBoxClick_IconPannel | `if ((413 <= msX) && (446 >= msX) && (434 < msY) && (475 > msY)) {` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:18784 | DlgBoxClick_IconPannel | `if ((453 <= msX) && (486 >= msX) && (434 < msY) && (475 > msY)) {` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:18792 | DlgBoxClick_IconPannel | `if ((490 <= msX) && (522 >= msX) && (434 < msY) && (475 > msY)) {` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:18800 | DlgBoxClick_IconPannel | `if ((526 <= msX) && (552 >= msX) && (434 < msY) && (475 > msY)) {` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:18808 | DlgBoxClick_IconPannel | `if ((556 <= msX) && (587 >= msX) && (434 < msY) && (475 > msY)) {` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:18816 | DlgBoxClick_IconPannel | `if ((589 <= msX) && (621 >= msX) && (434 < msY) && (475 > msY)) {` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20036 | DrawDialogBox_IconPannel | `if ((362 < msX) && (404 > msX) && (434 < msY) && (475 > msY)) {` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20037 | DrawDialogBox_IconPannel | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFast(362, 434, 16, dwTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20042 | DrawDialogBox_IconPannel | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFast(368, 440, 4, dwTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20043 | DrawDialogBox_IconPannel | `else m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFast(368, 440, 5, dwTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20048 | DrawDialogBox_IconPannel | `if ((322 <= msX) && (355 >= msX) && (434 < msY) && (475 > msY))` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20049 | DrawDialogBox_IconPannel | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFast(322, 434, 1, dwTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20050 | DrawDialogBox_IconPannel | `else m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFast(322, 434, 2, dwTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20053 | DrawDialogBox_IconPannel | `if ((322 <= msX) && (355 >= msX) && (434 < msY) && (475 > msY))` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20054 | DrawDialogBox_IconPannel | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFast(322, 434, 0, dwTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20055 | DrawDialogBox_IconPannel | `else m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFast(322, 434, 15, dwTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20059 | DrawDialogBox_IconPannel | `if ((msY > 436) && (msY < 478))` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20061 | DrawDialogBox_IconPannel | `if ((msX > 410) && (msX < 447))` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20064 | DrawDialogBox_IconPannel | `if( m_iLevel < 80 ) PutAlignedString(360, 497, 410, "????? ????(F5)", 200,200,120);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | only lang 3 |
| Game.cpp:20066 | DrawDialogBox_IconPannel | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFast(412, 434, 6, dwTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20068 | DrawDialogBox_IconPannel | `if ((msX > 447) && (msX < 484))` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20071 | DrawDialogBox_IconPannel | `if( m_iLevel < 80 ) PutAlignedString(397, 534, 410, "???????(F6)", 200,200,120);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | only lang 3 |
| Game.cpp:20073 | DrawDialogBox_IconPannel | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFast(449, 434, 7, dwTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20075 | DrawDialogBox_IconPannel | `if ((msX > 484) && (msX < 521))` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20078 | DrawDialogBox_IconPannel | `if( m_iLevel < 80 ) PutAlignedString(434, 571, 410, "????????(F7)", 200,200,120);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | only lang 3 |
| Game.cpp:20080 | DrawDialogBox_IconPannel | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFast(486, 434, 8, dwTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20082 | DrawDialogBox_IconPannel | `if ((msX > 521) && (msX < 558))` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20085 | DrawDialogBox_IconPannel | `if( m_iLevel < 80 ) PutAlignedString(471, 608, 410, "????????(F8)", 200,200,120);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | only lang 3 |
| Game.cpp:20087 | DrawDialogBox_IconPannel | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFast(523, 434, 9, dwTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20089 | DrawDialogBox_IconPannel | `if ((msX > 558) && (msX < 595))` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20092 | DrawDialogBox_IconPannel | `if( m_iLevel < 80 ) PutAlignedString(508, 645, 410, "???????????(F9)", 200,200,120);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | only lang 3 |
| Game.cpp:20094 | DrawDialogBox_IconPannel | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFast(560, 434, 10, dwTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20096 | DrawDialogBox_IconPannel | `if ((msX > 595) && (msX < 631))` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20099 | DrawDialogBox_IconPannel | `if( m_iLevel < 80 ) PutAlignedString(530, 640, 410, "?????? ???(F12)", 200,200,120);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | only lang 3 |
| Game.cpp:20101 | DrawDialogBox_IconPannel | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFast(597, 434, 11, dwTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20105 | DrawDialogBox_IconPannel | `if ( ((msX > 144) && (msX < 317) && (msY > 434) && (msY < 477)) \|\| (m_bCtrlPressed) ) {` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20147 | DrawDialogBox_IconPannel | `PutAlignedString(140, 323, 456, G_cTxt, 200,200,120);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20158 | DrawDialogBox_GaugePannel | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFastWidth(23, 437,  12, iBarWidth, m_dwCurTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20162 | DrawDialogBox_GaugePannel | `PutString_SprNum(85, 441, G_cTxt, m_wR[5]*11, m_wG[5]*11, m_wB[5]*11);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20163 | DrawDialogBox_GaugePannel | `PutString_SprFont3(35, 440, "Poisoned", m_wR[5]*8, m_wG[5]*8, m_wB[5]*8, TRUE, 2);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20165 | DrawDialogBox_GaugePannel | `else PutString_SprNum(85, 441, G_cTxt, 200, 100, 100);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20172 | DrawDialogBox_GaugePannel | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFastWidth(23, 459,  12, iBarWidth, m_dwCurTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20175 | DrawDialogBox_GaugePannel | `PutString_SprNum(85, 463, G_cTxt, 100, 100, 200);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:20182 | DrawDialogBox_GaugePannel | `m_pSprite[DEF_SPRID_INTERFACE_ND_ICONPANNEL]->PutSpriteFastWidth(147, 435, 13, iBarWidth, m_dwCurTime);` | HUD_X0+x, HUD_Y0+(y-427) | icon/gauge panel absolute coordinate | default |
| Game.cpp:28673 | OnKeyUp | `StartInputString(10, 414, sizeof(m_cChatMsg), m_cChatMsg);` | CHAT_INPUT_X=10, CHAT_INPUT_Y=VIEW_H-13 | chat input caret (key handlers) | only dbg |
| Game.cpp:28763 | OnKeyUp | `StartInputString(10, 414, sizeof(m_cChatMsg), m_cChatMsg);` | CHAT_INPUT_X=10, CHAT_INPUT_Y=VIEW_H-13 | chat input caret (key handlers) | default |
| Game.cpp:28886 | OnKeyUp | `StartInputString(10, 414, sizeof(m_cChatMsg), m_cChatMsg);` | CHAT_INPUT_X=10, CHAT_INPUT_Y=VIEW_H-13 | chat input caret (key handlers) | default |
| Game.cpp:29033 | OnKeyUp | `StartInputString(10, 414, sizeof(m_cChatMsg), m_cChatMsg);` | CHAT_INPUT_X=10, CHAT_INPUT_Y=VIEW_H-13 | chat input caret (key handlers) | default |
| Game.cpp:29063 | OnKeyUp | `StartInputString(10, 414, sizeof(m_cChatMsg), m_cChatMsg);` | CHAT_INPUT_X=10, CHAT_INPUT_Y=VIEW_H-13 | chat input caret (key handlers) | default |
| Game.cpp:29248 | OnKeyDown | `StartInputString(10, 414, sizeof(m_cChatMsg), m_cChatMsg);` | CHAT_INPUT_X=10, CHAT_INPUT_Y=VIEW_H-13 | chat input caret (key handlers) | default |
| Game.cpp:35045 | UpdateScreen_OnGame | `StartInputString(10, 414, sizeof(m_cChatMsg), m_cChatMsg);` | CHAT_INPUT_X=10, CHAT_INPUT_Y=VIEW_H-13 | chat input caret | default |
| Game.cpp:35048 | UpdateScreen_OnGame | `StartInputString(10, 414, sizeof(m_cChatMsg), m_cChatMsg);` | CHAT_INPUT_X=10, CHAT_INPUT_Y=VIEW_H-13 | chat input caret | default |
| Game.cpp:35132 | UpdateScreen_OnGame | `else m_DDraw.DrawShadowBox(0, 413, 639, 429);` | CHAT_BAR_Y0=VIEW_H-14, CHAT_BAR_Y1=VIEW_H+2, SCREEN_W-1 | chat input shadow bar | default |
| Game.cpp:35255 | UpdateScreen_OnGame | `m_pSprite[DEF_SPRID_INTERFACE_F1HELPWINDOWS]->PutSpriteFast(23,90, 0, dwTime);` | - | F1 help window / FPS text (top-left) | only lang 2 |
| Game.cpp:35401 | UpdateScreen_OnGame | `PutString( 10, 100, G_cTxt, RGB(255,255,255) );` | - | F1 help window / FPS text (top-left) | default |
| Game.cpp:45476 | ShowEventList | `PutString(10, 10 + i*15, m_stEventHistory[i].cTxt, RGB(225,225,225), FALSE, 1, TRUE);` | - | upper event list (top-left, keep) | default |
| Game.cpp:45479 | ShowEventList | `PutString(10, 10 + i*15, m_stEventHistory[i].cTxt, RGB(130,255,130), FALSE, 1, TRUE);` | - | upper event list (top-left, keep) | default |
| Game.cpp:45482 | ShowEventList | `PutString(10, 10 + i*15, m_stEventHistory[i].cTxt, RGB(255,130,130), FALSE, 1, TRUE);` | - | upper event list (top-left, keep) | default |
| Game.cpp:45485 | ShowEventList | `PutString(10, 10 + i*15, m_stEventHistory[i].cTxt, RGB(130,130,255), FALSE, 1, TRUE);` | - | upper event list (top-left, keep) | default |
| Game.cpp:45488 | ShowEventList | `PutString(10, 10 + i*15, m_stEventHistory[i].cTxt, RGB(230, 230, 130), FALSE, 1, TRUE);` | - | upper event list (top-left, keep) | default |
| Game.cpp:45492 | ShowEventList | `PutString(10, 10 + i*15, m_stEventHistory[i].cTxt, RGB(180,255,180), FALSE, 1, TRUE);` | - | upper event list (top-left, keep) | default |
| Game.cpp:45496 | ShowEventList | `PutString(10, 10 + i*15, m_stEventHistory[i].cTxt, RGB(150,150,170), FALSE, 1, TRUE);` | - | upper event list (top-left, keep) | default |
| Game.cpp:45506 | ShowEventList | `PutString(10, 322 + i*15, m_stEventHistory2[i].cTxt, RGB(225,225,225), FALSE, 1, TRUE);` | CHAT_BASE_Y=VIEW_H-105 | lower event/chat list | default |
| Game.cpp:45509 | ShowEventList | `PutString(10, 322 + i*15, m_stEventHistory2[i].cTxt, RGB(130,255,130), FALSE, 1, TRUE);` | CHAT_BASE_Y=VIEW_H-105 | lower event/chat list | default |
| Game.cpp:45512 | ShowEventList | `PutString(10, 322 + i*15, m_stEventHistory2[i].cTxt, RGB(255,130,130), FALSE, 1, TRUE);` | CHAT_BASE_Y=VIEW_H-105 | lower event/chat list | default |
| Game.cpp:45515 | ShowEventList | `PutString(10, 322 + i*15, m_stEventHistory2[i].cTxt, RGB(130,130,255), FALSE, 1, TRUE);` | CHAT_BASE_Y=VIEW_H-105 | lower event/chat list | default |
| Game.cpp:45518 | ShowEventList | `PutString(10, 322 + i*15, m_stEventHistory2[i].cTxt, RGB(230, 230, 130), FALSE, 1, TRUE);` | CHAT_BASE_Y=VIEW_H-105 | lower event/chat list | default |
| Game.cpp:45522 | ShowEventList | `PutString(10, 322 + i*15, m_stEventHistory2[i].cTxt, RGB(180,255,180), FALSE, 1, TRUE);` | CHAT_BASE_Y=VIEW_H-105 | lower event/chat list | default |
| Game.cpp:45526 | ShowEventList | `PutString(10, 322 + i*15, m_stEventHistory2[i].cTxt, RGB(150,150,170), FALSE, 1, TRUE);` | CHAT_BASE_Y=VIEW_H-105 | lower event/chat list | default |

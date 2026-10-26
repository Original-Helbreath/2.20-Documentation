# Values: dialog anchors and clamps

Class DIALOG rows: the default-position table in the CGame constructor, runtime re-positioning
to fixed points, and the few clamps against the screen edge. Coordinates *inside* a dialog are
relative to its sX/sY and are not screen-dependent (see `coincidental.md`). Dragging a dialog
(Game.cpp:35849-35850) has no screen clamp at all; re-opening clamps the max edge (18576-18577) and
pushes a dialog back by 20 when its right/bottom edge is above 10 (18578-18579).
Dialog 5 is 310x170 here (the language-2 build, `DEF_FEEDBACKCARD`, makes it 320x320, Game.cpp:567-572).
Generated from `.claude\scratch\resolution\classified.tsv` by `res_gendocs.py`; every row is a
source line found by the enumeration in `method.md` (verified by grep against
`sources\client-2.20`, line numbers as of this pass). Variable names are defined in `variables.md`.
The Build column is the preprocessor result (`ppeval.py`): `default` = compiled in the as-shipped
build (`DEF_LANGUAGE 4`, GlobalDef.h:1, no `_DEBUG`); `only lang N` / `only dbg` / `only opt` = compiled
only in other builds (other languages, `_DEBUG`, or an optional switch); `dead in all builds` = no
build compiles it; `commented` = inside a comment. See `method.md`, Build conditionals.


Rows: 128 (by build: default 120, other builds only 6, dead in all 0, commented 2)

## Default positions and proposed anchors

Derived from the constructor table (Game.cpp:540-785). Rule used for the proposal: dialog
centre x >= 400 -> right-anchored; centre within 60 px of 320 -> centred; else left. Bottom edge
>= 380 with top >= 150 -> bottom-anchored to VIEW_H; centre within 40 px of 213 -> centred in
the play area; else top. This is a proposal, not observed behaviour (*inferred*).

| Dialog | sX, sY (line) | size | right edge, bottom | Proposed X | Proposed Y |
|---|---|---|---|---|---|
| 1 | 30, 30 (Game.cpp:543) | 270x376 | 300, 406 | 30 (left) | 30 (top) |
| 2 | 380, 210 (Game.cpp:549) | 225x185 | 605, 395 | DLG_RIGHT(380) = SCREEN_W - 260 | DLG_BOTTOM(210) = 210 + (VIEW_H-427) |
| 3 | 337, 57 (Game.cpp:555) | 258x328 | 595, 385 | DLG_RIGHT(337) = SCREEN_W - 303 | 57 (top) |
| 4 | 0, 0 (Game.cpp:561) | 270x105 | 270, 105 | 0 (set at open time) | 0 (set at open time) |
| 5 | 0, 0 (Game.cpp:575) | 310x170 (320x320 in the language-2 build, `DEF_FEEDBACKCARD`) | 310, 170 | 0 (set at open time) | 0 (set at open time) |
| 6 | 0, 0 (Game.cpp:582) | 310x170 | 310, 170 | 0 (set at open time) | 0 (set at open time) |
| 7 | 337, 57 (Game.cpp:588) | 258x339 | 595, 396 | DLG_RIGHT(337) = SCREEN_W - 303 | 57 (top) |
| 8 | 337, 57 (Game.cpp:594) | 295x346 | 632, 403 | DLG_RIGHT(337) = SCREEN_W - 303 | 57 (top) |
| 9 | 512, 0 (Game.cpp:600) | 128x128 | 640, 128 | DLG_RIGHT(512) = SCREEN_W - 128 | 0 (top) |
| 10 | 135, 273 (Game.cpp:606) | 364x162 | 499, 435 | DLG_CENTER_X(135) = 135 + (SCREEN_W-640)/2 | DLG_BOTTOM(273) = 273 + (VIEW_H-427) |
| 11 | 70, 50 (Game.cpp:612) | 258x339 | 328, 389 | 70 (left) | 50 (top) |
| 12 | 0, 0 (Game.cpp:618) | 258x339 | 258, 339 | 0 (set at open time) | 0 (set at open time) |
| 13 | 337, 57 (Game.cpp:624) | 258x339 | 595, 396 | DLG_RIGHT(337) = SCREEN_W - 303 | 57 (top) |
| 14 | 60, 50 (Game.cpp:630) | 258x339 | 318, 389 | 60 (left) | 50 (top) |
| 15 | 337, 57 (Game.cpp:637) | 258x339 | 595, 396 | DLG_RIGHT(337) = SCREEN_W - 303 | 57 (top) |
| 16 | 30, 30 (Game.cpp:643) | 304x328 | 334, 358 | 30 (left) | 30 (top) |
| 17 | 0, 0 (Game.cpp:649) | 215x87 | 215, 87 | 0 (set at open time) | 0 (set at open time) |
| 18 | 5, 65 (Game.cpp:655) | 258x339 | 263, 404 | 5 (left) | 65 (top) |
| 19 | 337, 107 (Game.cpp:661) | 258x268 | 595, 375 | DLG_RIGHT(337) = SCREEN_W - 303 | DLG_CENTER_Y(107) = 107 + (VIEW_H-427)/2 |
| 20 | 237, 57 (Game.cpp:667) | 252x87 | 489, 144 | DLG_CENTER_X(237) = 237 + (SCREEN_W-640)/2 | 57 (top) |
| 21 | 337, 57 (Game.cpp:673) | 258x339 | 595, 396 | DLG_RIGHT(337) = SCREEN_W - 303 | 57 (top) |
| 22 | 336, 88 (Game.cpp:679) | 270x346 | 606, 434 | DLG_RIGHT(336) = SCREEN_W - 304 | 88 (top) |
| 23 | 337, 57 (Game.cpp:685) | 258x339 | 595, 396 | DLG_RIGHT(337) = SCREEN_W - 303 | 57 (top) |
| 24 | 193, 241 (Game.cpp:691) | 263x100 | 456, 341 | DLG_CENTER_X(193) = 193 + (SCREEN_W-640)/2 | 241 (top) |
| 25 | 162, 40 (Game.cpp:697) | 315x171 | 477, 211 | DLG_CENTER_X(162) = 162 + (SCREEN_W-640)/2 | 40 (top) |
| 26 | 100, 60 (Game.cpp:703) | 258x339 | 358, 399 | 100 (left) | 60 (top) |
| 27 | 100, 30 (Game.cpp:709) | 520x357 | 620, 387 | DLG_CENTER_X(100) = 100 + (SCREEN_W-640)/2 | 30 (top) |
| 28 | 0, 0 (Game.cpp:715) | 258x339 | 258, 339 | 0 (set at open time) | 0 (set at open time) |
| 29 | 0, 434 (Game.cpp:721) | 157x53 | 157, 487 | HUD_X0 + 0 | HUD_Y0 + 7 |
| 30 | 0, 427 (Game.cpp:727) | 640x53 | 640, 480 | HUD_X0 + 0 | HUD_Y0 + 0 |
| 31 | 170, 70 (Game.cpp:733) | 258x339 | 428, 409 | DLG_CENTER_X(170) = 170 + (SCREEN_W-640)/2 | 70 (top) |
| 32 | 0, 0 (Game.cpp:739) | 258x339 | 258, 339 | 0 (set at open time) | 0 (set at open time) |
| 33 | 360, 65 (Game.cpp:745) | 258x339 | 618, 404 | DLG_RIGHT(360) = SCREEN_W - 280 | 65 (top) |
| 34 | 60, 50 (Game.cpp:751) | 258x339 | 318, 389 | 60 (left) | 50 (top) |
| 35 | 358, 65 (Game.cpp:757) | 258x339 | 616, 404 | DLG_RIGHT(358) = SCREEN_W - 282 | 65 (top) |
| 36 | 20, 20 (Game.cpp:763) | 310x386 | 330, 406 | 20 (left) | 20 (top) |
| 37 | 20, 20 (Game.cpp:769) | 310x386 | 330, 406 | 20 (left) | 20 (top) |
| 38 | 20, 20 (Game.cpp:775) | 310x386 | 330, 406 | 20 (left) | 20 (top) |
| 39 | 0, 0 (Game.cpp:781) | 291x413 | 291, 413 | 0 (set at open time) | 0 (set at open time) |

## Every dialog-position row found


| Where | Function | Code | Proposed | Note | Build |
|---|---|---|---|---|---|
| Game.cpp:543 | CGame | `m_stDialogBoxInfo[1].sX = 30;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:544 | CGame | `m_stDialogBoxInfo[1].sY = 30;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:549 | CGame | `m_stDialogBoxInfo[2].sX = 380;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:550 | CGame | `m_stDialogBoxInfo[2].sY = 210;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:555 | CGame | `m_stDialogBoxInfo[3].sX = 337;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:556 | CGame | `m_stDialogBoxInfo[3].sY = 57;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:561 | CGame | `m_stDialogBoxInfo[4].sX = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:562 | CGame | `m_stDialogBoxInfo[4].sY = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:569 | CGame | `m_stDialogBoxInfo[5].sX = 0;` | DLG_<n> anchor | default dialog position (constructor table) | only lang 2 |
| Game.cpp:570 | CGame | `m_stDialogBoxInfo[5].sY = 0;` | DLG_<n> anchor | default dialog position (constructor table) | only lang 2 |
| Game.cpp:575 | CGame | `m_stDialogBoxInfo[5].sX = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:576 | CGame | `m_stDialogBoxInfo[5].sY = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:582 | CGame | `m_stDialogBoxInfo[6].sX = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:583 | CGame | `m_stDialogBoxInfo[6].sY = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:588 | CGame | `m_stDialogBoxInfo[7].sX = 337;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:589 | CGame | `m_stDialogBoxInfo[7].sY = 57;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:594 | CGame | `m_stDialogBoxInfo[8].sX = 337;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:595 | CGame | `m_stDialogBoxInfo[8].sY = 57;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:600 | CGame | `m_stDialogBoxInfo[9].sX = 512;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:601 | CGame | `m_stDialogBoxInfo[9].sY = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:606 | CGame | `m_stDialogBoxInfo[10].sX = 135;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:607 | CGame | `m_stDialogBoxInfo[10].sY = 273;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:612 | CGame | `m_stDialogBoxInfo[11].sX = 70;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:613 | CGame | `m_stDialogBoxInfo[11].sY = 50;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:618 | CGame | `m_stDialogBoxInfo[12].sX = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:619 | CGame | `m_stDialogBoxInfo[12].sY = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:624 | CGame | `m_stDialogBoxInfo[13].sX = 337;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:625 | CGame | `m_stDialogBoxInfo[13].sY = 57;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:630 | CGame | `m_stDialogBoxInfo[14].sX = 60;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:631 | CGame | `m_stDialogBoxInfo[14].sY = 50;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:637 | CGame | `m_stDialogBoxInfo[15].sX = 337;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:638 | CGame | `m_stDialogBoxInfo[15].sY = 57;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:643 | CGame | `m_stDialogBoxInfo[16].sX = 30;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:644 | CGame | `m_stDialogBoxInfo[16].sY = 30;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:649 | CGame | `m_stDialogBoxInfo[17].sX = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:650 | CGame | `m_stDialogBoxInfo[17].sY = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:655 | CGame | `m_stDialogBoxInfo[18].sX = 5;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:656 | CGame | `m_stDialogBoxInfo[18].sY = 65;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:661 | CGame | `m_stDialogBoxInfo[19].sX = 337;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:662 | CGame | `m_stDialogBoxInfo[19].sY = 107;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:667 | CGame | `m_stDialogBoxInfo[20].sX = 237;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:668 | CGame | `m_stDialogBoxInfo[20].sY = 57;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:673 | CGame | `m_stDialogBoxInfo[21].sX = 337;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:674 | CGame | `m_stDialogBoxInfo[21].sY = 57;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:679 | CGame | `m_stDialogBoxInfo[22].sX = 336;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:680 | CGame | `m_stDialogBoxInfo[22].sY = 88;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:685 | CGame | `m_stDialogBoxInfo[23].sX = 337;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:686 | CGame | `m_stDialogBoxInfo[23].sY = 57;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:691 | CGame | `m_stDialogBoxInfo[24].sX = 193;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:692 | CGame | `m_stDialogBoxInfo[24].sY = 241;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:697 | CGame | `m_stDialogBoxInfo[25].sX = 162;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:698 | CGame | `m_stDialogBoxInfo[25].sY = 40;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:703 | CGame | `m_stDialogBoxInfo[26].sX = 100;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:704 | CGame | `m_stDialogBoxInfo[26].sY = 60;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:709 | CGame | `m_stDialogBoxInfo[27].sX = 100;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:710 | CGame | `m_stDialogBoxInfo[27].sY = 30;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:715 | CGame | `m_stDialogBoxInfo[28].sX = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:716 | CGame | `m_stDialogBoxInfo[28].sY = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:733 | CGame | `m_stDialogBoxInfo[31].sX = 170;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:734 | CGame | `m_stDialogBoxInfo[31].sY = 70;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:739 | CGame | `m_stDialogBoxInfo[32].sX = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:740 | CGame | `m_stDialogBoxInfo[32].sY = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:745 | CGame | `m_stDialogBoxInfo[33].sX = 360;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:746 | CGame | `m_stDialogBoxInfo[33].sY = 65;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:751 | CGame | `m_stDialogBoxInfo[34].sX = 60;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:752 | CGame | `m_stDialogBoxInfo[34].sY = 50;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:757 | CGame | `m_stDialogBoxInfo[35].sX = 358;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:758 | CGame | `m_stDialogBoxInfo[35].sY = 65;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:763 | CGame | `m_stDialogBoxInfo[36].sX = 20;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:764 | CGame | `m_stDialogBoxInfo[36].sY = 20;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:769 | CGame | `m_stDialogBoxInfo[37].sX = 20;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:770 | CGame | `m_stDialogBoxInfo[37].sY = 20;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:775 | CGame | `m_stDialogBoxInfo[38].sX = 20;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:776 | CGame | `m_stDialogBoxInfo[38].sY = 20;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:781 | CGame | `m_stDialogBoxInfo[39].sX = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:782 | CGame | `m_stDialogBoxInfo[39].sY = 0;` | DLG_<n> anchor | default dialog position (constructor table) | default |
| Game.cpp:4717 | bItemDrop_ExternalScreen | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:4719 | bItemDrop_ExternalScreen | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:4741 | bItemDrop_ExternalScreen | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:4743 | bItemDrop_ExternalScreen | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:4766 | bItemDrop_ExternalScreen | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:4768 | bItemDrop_ExternalScreen | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:4812 | bItemDrop_ExternalScreen | `m_stDialogBoxInfo[4].sY = 0;` | DLG_<n> anchor | dialog re-positioned to fixed point at runtime | default |
| Game.cpp:17881 | DrawDialogBox_GuideMap | `if( sX > 640-128-20 ) sX = 640-128;` | SCREEN_W-128-20, VIEW_H-128-20 | guide map clamp | default |
| Game.cpp:17882 | DrawDialogBox_GuideMap | `if( sY > 427-128-20 ) sY = 427-128;` | SCREEN_W-128-20, VIEW_H-128-20 | guide map clamp | default |
| Game.cpp:17950 | DrawDialogBox_GuideMap | `if( sY > 213 ) shY = sY - 17;` | VIEW_H/2 | guide map caption above/below | default |
| Game.cpp:18413 | EnableDialogBox | `// m_stDialogBoxInfo[26].sX = 0;` | DLG_<n> anchor | dialog re-positioned to fixed point at runtime (commented) | commented |
| Game.cpp:18414 | EnableDialogBox | `// m_stDialogBoxInfo[26].sY = 0;` | DLG_<n> anchor | dialog re-positioned to fixed point at runtime (commented) | commented |
| Game.cpp:18501 | EnableDialogBox | `m_stDialogBoxInfo[33].sX  = 360;` | DLG_<n> anchor | dialog re-positioned to fixed point at runtime | default |
| Game.cpp:18502 | EnableDialogBox | `m_stDialogBoxInfo[33].sY  = 65;` | DLG_<n> anchor | dialog re-positioned to fixed point at runtime | default |
| Game.cpp:18576 | EnableDialogBox | `if( m_stDialogBoxInfo[iBoxID].sY > 400 ) m_stDialogBoxInfo[iBoxID].sY = 410;` | VIEW_H-27 / SCREEN_W-20 | EnableDialogBox re-open clamp (sY>400 -> 410, sX>620) | default |
| Game.cpp:18577 | EnableDialogBox | `if( m_stDialogBoxInfo[iBoxID].sX > 620 ) m_stDialogBoxInfo[iBoxID].sX = 620;` | VIEW_H-27 / SCREEN_W-20 | EnableDialogBox re-open clamp (sY>400 -> 410, sX>620) | default |
| Game.cpp:24616 | NoticementHandler | `m_stDialogBoxInfo[5].sX  =  150;` | DLG_<n> anchor | dialog re-positioned to fixed point at runtime | only lang 3 |
| Game.cpp:24617 | NoticementHandler | `m_stDialogBoxInfo[5].sY  =  100;` | DLG_<n> anchor | dialog re-positioned to fixed point at runtime | only lang 3 |
| Game.cpp:32242 | DlbBoxDoubleClick_GuideMap | `if( sX > 640-128-20 ) sX = 640-128;` | SCREEN_W-128-20, VIEW_H-128-20 | guide map clamp | default |
| Game.cpp:32243 | DlbBoxDoubleClick_GuideMap | `if( sY > 427-128-20 ) sY = 427-128;` | SCREEN_W-128-20, VIEW_H-128-20 | guide map clamp | default |
| Game.cpp:32517 | DlbBoxDoubleClick_Inventory | `m_stDialogBoxInfo[5].sX  =  150;` | DLG_<n> anchor | dialog re-positioned to fixed point at runtime | only lang 2 |
| Game.cpp:32518 | DlbBoxDoubleClick_Inventory | `m_stDialogBoxInfo[5].sY  =  100;` | DLG_<n> anchor | dialog re-positioned to fixed point at runtime | only lang 2 |
| Game.cpp:34917 | UpdateScreen_OnGame | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:34919 | UpdateScreen_OnGame | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:34938 | UpdateScreen_OnGame | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:34940 | UpdateScreen_OnGame | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:34959 | UpdateScreen_OnGame | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:34961 | UpdateScreen_OnGame | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:35882 | CommandProcessor | `if( msX < 320 ) m_stDialogBoxInfo[9].sX = 0;` | SCREEN_W/2, VIEW_H/2, SCREEN_W, VIEW_H | guide map snaps to nearest quadrant corner | default |
| Game.cpp:35883 | CommandProcessor | `else m_stDialogBoxInfo[9].sX = 640 - m_stDialogBoxInfo[9].sSizeX;` | SCREEN_W/2, VIEW_H/2, SCREEN_W, VIEW_H | guide map snaps to nearest quadrant corner | default |
| Game.cpp:35884 | CommandProcessor | `if( msY < 213 ) m_stDialogBoxInfo[9].sY = 0;` | SCREEN_W/2, VIEW_H/2, SCREEN_W, VIEW_H | guide map snaps to nearest quadrant corner | default |
| Game.cpp:35885 | CommandProcessor | `else m_stDialogBoxInfo[9].sY = 427 - m_stDialogBoxInfo[9].sSizeY;` | SCREEN_W/2, VIEW_H/2, SCREEN_W, VIEW_H | guide map snaps to nearest quadrant corner | default |
| Game.cpp:36286 | CommandProcessor | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36288 | CommandProcessor | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36303 | CommandProcessor | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36305 | CommandProcessor | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36320 | CommandProcessor | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36322 | CommandProcessor | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36344 | CommandProcessor | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36346 | CommandProcessor | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36366 | CommandProcessor | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36368 | CommandProcessor | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36383 | CommandProcessor | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36385 | CommandProcessor | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36400 | CommandProcessor | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36402 | CommandProcessor | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36417 | CommandProcessor | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36419 | CommandProcessor | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36433 | CommandProcessor | `if ((tX + 235) > 639) tX = 639 - 235;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:36435 | CommandProcessor | `if ((tY + 100) > 479) tY = 479 - 100;` | SCREEN_W-1, SCREEN_H-1 (use QUERY_W/H = dialog 20 size 252x87, not 235x100) | item-drop query dialog clamp; 235x100 differs from dialog 20 size 252x87 (Game.cpp:669-670), so the box can reach x=656 | default |
| Game.cpp:46014 | InitDataResponseHandler | `m_stDialogBoxInfo[6].sX  =  150;` | DLG_<n> anchor | dialog re-positioned to fixed point at runtime | default |
| Game.cpp:46015 | InitDataResponseHandler | `m_stDialogBoxInfo[6].sY  =  130;` | DLG_<n> anchor | dialog re-positioned to fixed point at runtime | default |

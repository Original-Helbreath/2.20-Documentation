# Coincidental, false-positive and pitch-correct rows (every row)

Generated from `.claude\scratch\resolution\classified.tsv` by `res_gendocs.py`; every row is a
source line found by the enumeration in `method.md` (verified by grep against
`sources\client-2.20`, line numbers as of this pass). Variable names are defined in `variables.md`.
The Build column is the preprocessor result (`ppeval.py`): `default` = compiled in the as-shipped
build (`DEF_LANGUAGE 4`, GlobalDef.h:1, no `_DEBUG`); `only lang N` / `only dbg` / `only opt` = compiled
only in other builds (other languages, `_DEBUG`, or an optional switch); `dead in all builds` = no
build compiles it; `commented` = inside a comment. See `method.md`, Build conditionals.

## COINC - same number, unrelated to the screen (106)

| Where | Function | Code | Proposed | Note | Build |
|---|---|---|---|---|---|
| Game.cpp:142 | (file scope) | `char _cURLName[320];` | - | string buffer size | only lang 2 |
| Game.cpp:161 | (file scope) | `//   G_Oldpoint.x = 320;` | - | dead (commented) cursor code (commented) | commented |
| Game.cpp:162 | (file scope) | `//   G_Oldpoint.y = 240;` | - | dead (commented) cursor code (commented) | commented |
| Game.cpp:163 | (file scope) | `//   SetCursorPos(G_Oldpoint.x, G_Oldpoint.y);` | - | dead (commented) cursor code (commented) | commented |
| Game.cpp:179 | (file scope) | `memset(&_cURLName,0,320);` | - | string buffer size | only lang 2 |
| Game.cpp:284 | (file scope) | `// /   ::SetCursorPos(pPoint.x, pPoint.y);` | - | dead (commented) cursor code (commented) | commented |
| Game.cpp:285 | (file scope) | `//    ::SetCursorPos(10, 10);` | - | dead (commented) cursor code (commented) | commented |
| Game.cpp:571 | CGame | `m_stDialogBoxInfo[5].sSizeX = 320;` | - | dialog 5 size 320x320 only under DEF_FEEDBACKCARD (language 2); 310x170 in every other build (Game.cpp:575-578) | only lang 2 |
| Game.cpp:572 | CGame | `m_stDialogBoxInfo[5].sSizeY = 320;` | - | dialog 5 size 320x320 only under DEF_FEEDBACKCARD (language 2); 310x170 in every other build (Game.cpp:575-578) | only lang 2 |
| Game.cpp:784 | CGame | `m_stDialogBoxInfo[39].sSizeY = 413;` | - | dialog 39 height 413 (fits under chat bar; re-check if PLAY_H shrinks) | default |
| Game.cpp:2566 | DrawObjects | `case 224:` | - | switch label (object/sprite id) | default |
| Game.cpp:2614 | DrawObjects | `if ( ((dwTime - m_dwEnvEffectTime) > 400) && (sObjSprFrame == 6) && (G_cSpriteAlphaDegree == 2)) bAddNewEffect(65, m_sViewPointX + ix -14, m_sViewPoin` | - | per-object sprite offset in world pixels | default |
| Game.cpp:2636 | DrawObjects | `case 429:` | - | switch label (object/sprite id) | default |
| Game.cpp:2637 | DrawObjects | `if ( ((dwTime - m_dwEnvEffectTime) > 400) && (sObjSprFrame == 2)) bAddNewEffect(65, m_sViewPointX + ix -15, m_sViewPointY + iy -224, NULL, NULL, NULL,` | - | object sprite id / particle spread | default |
| Game.cpp:3723 | UpdateScreen_OnLoading | `MakeTileSpr( "maptiles4", 320, 10, TRUE);` | - | tile sprite index | default |
| Game.cpp:3738 | UpdateScreen_OnLoading | `MakeTileSpr( "Tile422-429", 422, 8, TRUE);` | - | tile sprite index | default |
| Game.cpp:6394 | bAddNewEffect | `m_pEffectList[i]->m_mY    = sY -460;` | - | effect start offset -460 px above the cast/target position (m_mY = sY-460 / dY*32-460), not view-relative | default |
| Game.cpp:6924 | bAddNewEffect | `m_pEffectList[i]->m_mY    = dY*32 -460;` | - | effect start offset -460 px above the cast/target position (m_mY = sY-460 / dY*32-460), not view-relative | default |
| Game.cpp:7704 | DrawEffects | `_DrawThunderEffect(m_pEffectList[i]->m_dX*32 - m_sViewPointX, m_pEffectList[i]->m_dY*32 - m_sViewPointY - 800,` | - | thunder bolt starts 800 px above the strike point (dY*32 - viewY - 800), not view-relative | default |
| Game.cpp:7708 | DrawEffects | `_DrawThunderEffect(m_pEffectList[i]->m_dX*32 - m_sViewPointX, m_pEffectList[i]->m_dY*32 - m_sViewPointY - 800,` | - | thunder bolt starts 800 px above the strike point (dY*32 - viewY - 800), not view-relative | default |
| Game.cpp:7712 | DrawEffects | `_DrawThunderEffect(m_pEffectList[i]->m_dX*32 - m_sViewPointX, m_pEffectList[i]->m_dY*32 - m_sViewPointY - 800,` | - | thunder bolt starts 800 px above the strike point (dY*32 - viewY - 800), not view-relative | default |
| Game.cpp:17548 | _iCheckDlgBoxFocus | `if ((m_stDialogBoxInfo[11].cMode == 0) && (msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 20) && (msY <= sY + 330)) {` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:17564 | _iCheckDlgBoxFocus | `if ((msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 40) && (msY <= sY + 320)) {` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:17580 | _iCheckDlgBoxFocus | `if ((msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 40) && (msY <= sY + 320)) {` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:17596 | _iCheckDlgBoxFocus | `if ((msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 40) && (msY <= sY + 320)) {` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:17632 | _iCheckDlgBoxFocus | `if ((msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 40) && (msY <= sY + 320)) {` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:17998 | DrawDialogBox_GuideMap | `else if( shX > 301 && shX < 330 && shY > 239 && shY < 265 ) strcpy( G_cTxt, DEF_MSG_MAPNAME_DUNGEON );` | - | guide-map region test in map cells (shX/shY are cells: m_sPlayerX-64+..., not art pixels) | default |
| Game.cpp:18546 | EnableDialogBox | `EnableDialogBox(21, 0, 480, 0);` | - | EnableDialogBox argument (sV1=480), not a coordinate | default |
| Game.cpp:19226 | DlgBoxClick_CrusadeJob | `case 1: EnableDialogBox(18, 803, NULL, NULL); break;` | - | switch label (object/sprite id) | default |
| Game.cpp:19227 | DlgBoxClick_CrusadeJob | `case 2: EnableDialogBox(18, 805, NULL, NULL); break;` | - | switch label (object/sprite id) | default |
| Game.cpp:19228 | DlgBoxClick_CrusadeJob | `case 3: EnableDialogBox(18, 808, NULL, NULL); break;` | - | switch label (object/sprite id) | default |
| Game.cpp:20235 | DrawDialogBox_Text | `if ((msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 40) && (msY <= sY + 320)) {` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:20371 | DrawDialogBox_ItemDrop | `if ((msX >= sX + 35) && (msX <= sX + 240 ) && (msY >= sY + 80) && (msY <= sY + 90))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:20386 | DrawDialogBox_ItemDrop | `if ((msX >= sX + 35) && (msX <= sX + 240 ) && (msY >= sY + 80) && (msY <= sY + 90))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:20467 | DrawDialogBox_NpcTalk | `if ((msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 40) && (msY <= sY + 320)) {` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:20744 | DlgBoxClick_LevelUpSettings | `if ((msX > sX + 228 - 50) && (msX < sX + 237 - 50) && (msY > sY + 216 - 50) && (msY < sY + 224 - 50)) {` | - | offset inside a dialog (sX/sY relative) | only dbg |
| Game.cpp:20792 | DlgBoxClick_LevelUpSettings | `if ((msX > sX + 245 - 50) && (msX < sX + 253 - 50) && (msY > sY + 216 - 50) && (msY < sY + 224 - 50)) {` | - | offset inside a dialog (sX/sY relative) | only dbg |
| Game.cpp:21421 | DrawChatMsgBox | `if( iLoc > 352 ) iLoc = 9;` | - | chat-bubble age threshold | default |
| Game.cpp:21422 | DrawChatMsgBox | `else if( iLoc > 320 ) iLoc = 10;` | - | chat-bubble age threshold | default |
| Game.cpp:23006 | DlgBoxClick_ItemDrop | `else if ((msX >= sX + 35) && (msX <= sX + 240 ) && (msY >= sY + 80) && (msY <= sY + 90))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:33458 | DrawNpcName | `else PutString2(sX, sY + 28, cTxt2, 240,240,70);` | - | RGB colour component 240 | default |
| Game.cpp:37134 | DrawDialogBox_Bank | `if( (strlen(cStr2)==0) && (strlen(cStr3)==0) ) PutAlignedString(sX + 24, sX +240, sY + 50 + i*15, cStr1, 255,255,255);` | - | offset inside a dialog (sX/sY relative) | commented |
| Game.cpp:37173 | DrawDialogBox_Bank | `if( (strlen(cStr2)==0) && (strlen(cStr3)==0) ) PutAlignedString(sX + 24, sX +240, sY + 50 + i*15, cStr1);` | - | offset inside a dialog (sX/sY relative) | commented |
| Game.cpp:37205 | DrawDialogBox_Bank | `if ((msX >= sX + 230) && (msX <= sX + 260) && (msY >= sY + 40) && (msY <= sY + 320)) {` | - | offset inside a dialog (sX/sY relative) | commented |
| Game.cpp:37355 | DrawDialogBox_Bank | `if ((msX >= sX + 230) && (msX <= sX + 260) && (msY >= sY + 40) && (msY <= sY + 320)) {` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:37479 | DrawDialogBox_Character | `PutAlignedString(sX+180, sX+250, sY + 240, G_cTxt, 45,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:38456 | DrawDialogBox_CityHallMenu | `PutAlignedString(sX, sX + szX, sY + 224, DRAW_DIALOGBOX_CITYHALL_MENU67, 55,25,25);` | - | offset inside a dialog (sY relative) | default |
| Game.cpp:38967 | DrawDialogBox_GuildMenu | `PutAlignedString(sX + 24, sX + 239, sY + 125, DRAW_DIALOGBOX_GUILDMENU18, 55,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:39432 | DrawDialogBox_Magic | `if ((msX >= sX + 30) && (msX <= sX + 240) && (msY >= sY + 70 + iYloc) && (msY <= sY + 70 + 14 + iYloc))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:39606 | DrawDialogBox_MagicShop | `PutString_SprFont(sX + 241, sY + 70 + iYloc, cMana, 5, 5, 5);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:39871 | DrawDialogBox_NpcActionQuery | `PutAlignedString(sX, sX+240, sY+20, cTxt, 45,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:39872 | DrawDialogBox_NpcActionQuery | `PutAlignedString(sX, sX+240, sY+35, cTxt2, 45,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40563 | DrawDialogBox_SellorRepairItem | `PutAlignedString(sX + 25, sX + 240, sY + 60, cTxt, 0,255,50);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40564 | DrawDialogBox_SellorRepairItem | `PutAlignedString(sX + 25 +1, sX + 240 +1, sY + 60, cTxt, 0,255,50);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40568 | DrawDialogBox_SellorRepairItem | `PutAlignedString(sX + 25, sX + 240, sY + 60, cTxt, 45,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40569 | DrawDialogBox_SellorRepairItem | `PutAlignedString(sX + 25 +1, sX + 240 +1, sY + 60, cTxt, 45,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40627 | DrawDialogBox_SellorRepairItem | `//   PutAlignedString(sX + 25, sX + 240, sY + 60, cTxt, 45,25,25);` | - | offset inside a dialog (sX/sY relative) (commented) | commented |
| Game.cpp:40628 | DrawDialogBox_SellorRepairItem | `//   PutAlignedString(sX + 25 +1, sX + 240 +1, sY + 60, cTxt, 45,25,25);` | - | offset inside a dialog (sX/sY relative) (commented) | commented |
| Game.cpp:40631 | DrawDialogBox_SellorRepairItem | `PutAlignedString(sX + 25, sX + 240, sY + 60, cTxt, 0,255,50);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40632 | DrawDialogBox_SellorRepairItem | `PutAlignedString(sX + 25 +1, sX + 240 +1, sY + 60, cTxt, 0,255,50);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40636 | DrawDialogBox_SellorRepairItem | `PutAlignedString(sX + 25, sX + 240, sY + 60, cTxt, 45,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40637 | DrawDialogBox_SellorRepairItem | `PutAlignedString(sX + 25 +1, sX + 240 +1, sY + 60, cTxt, 45,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40785 | DrawDialogBox_Shop | `PutAlignedString(sX +25, sX+240, sY+50, cTemp, 255,255,255);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40786 | DrawDialogBox_Shop | `PutAlignedString(sX +26, sX+241, sY+50, cTemp, 255,255,255);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40898 | DrawDialogBox_Shop | `PutAlignedString(sX +25, sX+240, sY + 258 +iMsgLoc*15, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40899 | DrawDialogBox_Shop | `PutAlignedString(sX +26, sX+241, sY + 258 +iMsgLoc*15, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40917 | DrawDialogBox_Shop | `PutAlignedString(sX +25, sX+240, sY + 258, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:40918 | DrawDialogBox_Shop | `PutAlignedString(sX +26, sX+241, sY + 258, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:41023 | DrawDialogBox_Shop | `PutAlignedString(sX +25, sX+240, sY + 254 -10 +14 +iMsgLoc*15, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:41024 | DrawDialogBox_Shop | `PutAlignedString(sX +25 +1, sX+240 +1, sY + 254 -10 +14 +iMsgLoc*15, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:41030 | DrawDialogBox_Shop | `PutAlignedString(sX +25, sX+240, sY + 254 -10 +14 +iMsgLoc*15, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:41031 | DrawDialogBox_Shop | `PutAlignedString(sX +25 +1, sX+240 +1, sY + 254 -10 +14 +iMsgLoc*15, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:41039 | DrawDialogBox_Shop | `PutAlignedString(sX +25, sX+240, sY + 254 -10 +14 +iMsgLoc*15, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:41040 | DrawDialogBox_Shop | `PutAlignedString(sX +25 +1, sX+240 +1, sY + 254 -10 +14 +iMsgLoc*15, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:41050 | DrawDialogBox_Shop | `PutAlignedString(sX +25, sX+240, sY + 254 -10 +14 +iMsgLoc*15, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:41051 | DrawDialogBox_Shop | `PutAlignedString(sX +25 +1, sX+240 +1, sY + 254 -10 +14 +iMsgLoc*15, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:41076 | DrawDialogBox_Shop | `PutAlignedString(sX +25, sX+240, sY + 254 -10 +14 +iMsgLoc*15, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:41077 | DrawDialogBox_Shop | `PutAlignedString(sX +25 +1, sX+240 +1, sY + 254 -10 +14 +iMsgLoc*15, cTemp, 195,25,25);` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:41209 | DrawDialogBox_Skill | `if ((msX >= sX + 240) && (msX <= sX + 260) && (msY >= sY + 30) && (msY <= sY + 320)) {` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:42471 | DlgBoxClick_Magic | `if ((msX >= sX + 30) && (msX <= sX + 240) && (msY >= sY + 70 + iYloc) && (msY <= sY + 70 + 18 + iYloc))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:42481 | DlgBoxClick_Magic | `if ((msX >= sX + 16) && (msX <= sX + 38) && (msY >= sY + 240) && (msY <= sY + 268))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:42483 | DlgBoxClick_Magic | `if ((msX >= sX + 39) && (msX <= sX + 56) && (msY >= sY + 240) && (msY <= sY + 268))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:42485 | DlgBoxClick_Magic | `if ((msX >= sX + 57) && (msX <= sX + 81) && (msY >= sY + 240) && (msY <= sY + 268))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:42487 | DlgBoxClick_Magic | `if ((msX >= sX + 82) && (msX <= sX + 101) && (msY >= sY + 240) && (msY <= sY + 268))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:42489 | DlgBoxClick_Magic | `if ((msX >= sX + 102) && (msX <= sX + 116) && (msY >= sY + 240) && (msY <= sY + 268))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:42491 | DlgBoxClick_Magic | `if ((msX >= sX + 117) && (msX <= sX + 137) && (msY >= sY + 240) && (msY <= sY + 268))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:42493 | DlgBoxClick_Magic | `if ((msX >= sX + 138) && (msX <= sX + 165) && (msY >= sY + 240) && (msY <= sY + 268))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:42495 | DlgBoxClick_Magic | `if ((msX >= sX + 166) && (msX <= sX + 197) && (msY >= sY + 240) && (msY <= sY + 268))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:42497 | DlgBoxClick_Magic | `if ((msX >= sX + 198) && (msX <= sX + 217) && (msY >= sY + 240) && (msY <= sY + 268))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:42499 | DlgBoxClick_Magic | `if ((msX >= sX + 218) && (msX <= sX + 239) && (msY >= sY + 240) && (msY <= sY + 268))` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:42893 | DlgBoxClick_Skill | `else if ((msX >= sX + 215) && (msX <= sX + 240) && (msY >= sY + 45 + i*15) && (msY <= sY + 59 + i*15)) {` | - | offset inside a dialog (sX/sY relative) | default |
| Game.cpp:45625 | RequestTeleportAndWaitData | `if( (m_sPlayerX == 239) && (m_sPlayerY == 133) \|\|` | - | map tile coordinate | only lang 3 |
| Game.cpp:45626 | RequestTeleportAndWaitData | `(m_sPlayerX == 240) && (m_sPlayerY == 133) \|\|` | - | map tile coordinate | only lang 3 |
| Game.cpp:45627 | RequestTeleportAndWaitData | `(m_sPlayerX == 240) && (m_sPlayerY == 132) \|\|` | - | map tile coordinate | only lang 3 |
| Game.cpp:45628 | RequestTeleportAndWaitData | `(m_sPlayerX == 241) && (m_sPlayerY == 132) )` | - | map tile coordinate | only lang 3 |
| Game.cpp:45664 | RequestTeleportAndWaitData | `if( (m_sPlayerX == 211) && (m_sPlayerY == 240) \|\|` | - | map tile coordinate | only lang 3 |
| Game.cpp:45665 | RequestTeleportAndWaitData | `(m_sPlayerX == 211) && (m_sPlayerY == 241) \|\|` | - | map tile coordinate | only lang 3 |
| Game.cpp:45666 | RequestTeleportAndWaitData | `(m_sPlayerX == 212) && (m_sPlayerY == 241) \|\|` | - | map tile coordinate | only lang 3 |
| Game.cpp:47741 | DrawDialogBox_ItemUpgrade | `PutAlignedString(sX + 24, sX + 248, sY + 240, DRAW_DIALOGBOX_ITEMUPGRADE29);` | - | offset inside a dialog (sX/sY relative) | default |
| MapData.cpp:47 | CMapData | `m_stFrame[10][DEF_OBJECTSTOP].m_sFrameTime  = 240;` | - | animation frame time | default |
| MapData.cpp:54 | CMapData | `m_stFrame[10][DEF_OBJECTDYING].m_sFrameTime  = 240;` | - | animation frame time | default |
| MapData.cpp:290 | CMapData | `m_stFrame[34][DEF_OBJECTSTOP].m_sFrameTime  = 240;` | - | animation frame time | default |
| MapData.cpp:297 | CMapData | `m_stFrame[34][DEF_OBJECTDYING].m_sFrameTime  = 240;` | - | animation frame time | default |
| Misc.cpp:442 | _iGetFileCheckSum | `char cRealFn[512];` | - | path buffer size | default |
| Wmain.cpp:188 | WndProc | `//    ::SetCursorPos(pPoint.x, pPoint.y);` | - | dead (commented) cursor code (commented) | commented |
| Wmain.cpp:189 | WndProc | `//    ::SetCursorPos(10, 10);` | - | dead (commented) cursor code (commented) | commented |

## FP - enumerator matched, no screen literal (114)

| Where | Function | Code | Proposed | Note | Build |
|---|---|---|---|---|---|
| Game.cpp:960 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB( 70, 70, 80),  &m_wWR[1], &m_wWG[1], &m_wWB[1]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:961 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB( 70, 70, 80),  &m_wWR[2], &m_wWG[2], &m_wWB[2]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:962 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB( 70, 70, 80),  &m_wWR[3], &m_wWG[3], &m_wWB[3]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:963 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB( 70, 100, 70), &m_wWR[4], &m_wWG[4], &m_wWB[4]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:964 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB( 130, 90, 10), &m_wWR[5], &m_wWG[5], &m_wWB[5]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:965 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB( 42, 53, 111), &m_wWR[6], &m_wWG[6], &m_wWB[6]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:966 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB( 145, 145, 145), &m_wWR[7], &m_wWG[7], &m_wWB[7]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:967 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB( 120, 100, 120), &m_wWR[8], &m_wWG[8], &m_wWB[8]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:968 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB(75, 10, 10), &m_wWR[9], &m_wWG[9], &m_wWB[9]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:969 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB(135, 104, 30),  &m_wR[10], &m_wG[10], &m_wB[10]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:974 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB(79, 79, 62),  &m_wR[2], &m_wG[2], &m_wB[2]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:975 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB(135, 104, 30),  &m_wR[3], &m_wG[3], &m_wB[3]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:977 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB(10, 60, 10),  &m_wR[5], &m_wG[5], &m_wB[5]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:981 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB(90, 60, 90),  &m_wR[9], &m_wG[9], &m_wB[9]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:983 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB(0, 35, 60),  &m_wR[10], &m_wG[10], &m_wB[10]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:986 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB(85, 85, 8), &m_wR[13], &m_wG[13], &m_wB[13]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:987 | bInit | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB(75, 10, 10), &m_wR[14], &m_wG[14], &m_wB[14]);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:4678 | bItemDrop_ExternalScreen | `m_stDialogBoxInfo[17].sX  = msX - 140;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:4679 | bItemDrop_ExternalScreen | `m_stDialogBoxInfo[17].sY  = msY - 70;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:4680 | bItemDrop_ExternalScreen | `if (m_stDialogBoxInfo[17].sY < 0) m_stDialogBoxInfo[17].sY = 0;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:4708 | bItemDrop_ExternalScreen | `EnableDialogBox(20, 1, cItemID, sType);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:4731 | bItemDrop_ExternalScreen | `EnableDialogBox(20, 3, cItemID, sType);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:4756 | bItemDrop_ExternalScreen | `EnableDialogBox(20, 2, cItemID, sType);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:4792 | bItemDrop_ExternalScreen | `m_stDialogBoxInfo[17].sX  = msX - 140;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:4793 | bItemDrop_ExternalScreen | `m_stDialogBoxInfo[17].sY  = msY - 70;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:4794 | bItemDrop_ExternalScreen | `if (m_stDialogBoxInfo[17].sY < 0) m_stDialogBoxInfo[17].sY = 0;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:4808 | bItemDrop_ExternalScreen | `m_stDialogBoxInfo[4].sX  = msX - 140;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:4809 | bItemDrop_ExternalScreen | `m_stDialogBoxInfo[4].sY  = msY - 70;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:7248 | DrawEffects | `// PutString(dX, dY, "*", RGB(255,255,255));` | - | catch-all match: colour/text id/arguments, not a screen coordinate (commented) | commented |
| Game.cpp:17952 | DrawDialogBox_GuideMap | `if( m_bZoomMap ) PutString( sX, shY, DEF_MSG_GUIDEMAP_MIN, RGB(200, 200, 120) );` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:17953 | DrawDialogBox_GuideMap | `else PutString( sX, shY, DEF_MSG_GUIDEMAP_MAX, RGB(200, 200, 120) );` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:18012 | DrawDialogBox_GuideMap | `PutString( msX-10, msY-13, G_cTxt, RGB(200, 200, 120) );` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:18213 | EnableDialogBox | `m_stDialogBoxInfo[12].sX = m_stDialogBoxInfo[1].sX + 20;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:18214 | EnableDialogBox | `m_stDialogBoxInfo[12].sY = m_stDialogBoxInfo[1].sY + 20;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:18338 | EnableDialogBox | `m_stDialogBoxInfo[23].sX = m_stDialogBoxInfo[11].sX;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:18339 | EnableDialogBox | `m_stDialogBoxInfo[23].sY = m_stDialogBoxInfo[11].sY;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:18482 | EnableDialogBox | `m_stDialogBoxInfo[28].sX = m_stDialogBoxInfo[1].sX + 20;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:18483 | EnableDialogBox | `m_stDialogBoxInfo[28].sY = m_stDialogBoxInfo[1].sY + 20;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:18490 | EnableDialogBox | `m_stDialogBoxInfo[32].sX = m_stDialogBoxInfo[1].sX + 20;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:18491 | EnableDialogBox | `m_stDialogBoxInfo[32].sY = m_stDialogBoxInfo[1].sY + 20;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:19218 | DlgBoxClick_CrusadeJob | `EnableDialogBox(18, 813, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:19450 | ShowReceivedString | `PutString(m_iInputX+1, m_iInputY+1, G_cTxt, RGB(0,0,0));` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:19451 | ShowReceivedString | `PutString(m_iInputX, m_iInputY+1, G_cTxt, RGB(0,0,0));` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:19452 | ShowReceivedString | `PutString(m_iInputX+1, m_iInputY, G_cTxt, RGB(0,0,0));` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:19453 | ShowReceivedString | `PutString(m_iInputX, m_iInputY, G_cTxt, RGB(255,255,255));` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:19584 | DlgBoxClick_Commander | `EnableDialogBox(18, 808, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:19623 | DlgBoxClick_Commander | `EnableDialogBox(18, 809, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:19645 | DlgBoxClick_Commander | `EnableDialogBox(18, 810, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:19740 | DlgBoxClick_Commander | `EnableDialogBox(18, 811, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:19779 | DlgBoxClick_Commander | `EnableDialogBox(18, 812, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:19829 | DlgBoxClick_Constructor | `EnableDialogBox(18, 805, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:19870 | DlgBoxClick_Constructor | `EnableDialogBox(18, 806, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:19893 | DlgBoxClick_Constructor | `EnableDialogBox(18, 807, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:19932 | DlgBoxClick_Soldier | `EnableDialogBox(18, 803, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:19955 | DlgBoxClick_Soldier | `EnableDialogBox(18, 804, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:21344 | DrawChatMsgBox | `rgb = RGB(255,255,255);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:21347 | DrawChatMsgBox | `rgb = RGB(255,255,255);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:21350 | DrawChatMsgBox | `rgb = RGB(255,255,20);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:21356 | DrawChatMsgBox | `rgb = RGB(255,80,80);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:21360 | DrawChatMsgBox | `rgb = RGB(255,80,80);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:23525 | _DrawThunderEffect | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB(50, 50, 100), &wR1, &wG1, &wB1);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:23526 | _DrawThunderEffect | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB(30, 30, 100), &wR2, &wG2, &wB2);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:23528 | _DrawThunderEffect | `m_Misc.ColorTransfer(m_DDraw.m_cPixelFormat, RGB(50, 50, 200), &wR4, &wG4, &wB4);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:23874 | bItemDrop_ExchangeDialog | `m_stDialogBoxInfo[17].sX  = msX - 140;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:23875 | bItemDrop_ExchangeDialog | `m_stDialogBoxInfo[17].sY  = msY - 70;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:23876 | bItemDrop_ExchangeDialog | `if (m_stDialogBoxInfo[17].sY < 0) m_stDialogBoxInfo[17].sY = 0;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:25494 | DlgBoxClick_Help | `EnableDialogBox(18, 900, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:25509 | DlgBoxClick_Help | `EnableDialogBox(18, 901, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:25514 | DlgBoxClick_Help | `EnableDialogBox(18, 902, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:25520 | DlgBoxClick_Help | `EnableDialogBox(18, 903, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:25526 | DlgBoxClick_Help | `EnableDialogBox(18, 904, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:25531 | DlgBoxClick_Help | `EnableDialogBox(18, 905, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:25536 | DlgBoxClick_Help | `EnableDialogBox(18, 906, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:25541 | DlgBoxClick_Help | `EnableDialogBox(18, 907, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:25546 | DlgBoxClick_Help | `EnableDialogBox(18, 908, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:25551 | DlgBoxClick_Help | `EnableDialogBox(18, 909, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:25556 | DlgBoxClick_Help | `EnableDialogBox(18, 910, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:25561 | DlgBoxClick_Help | `EnableDialogBox(18, 911, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:25566 | DlgBoxClick_Help | `EnableDialogBox(18, 912, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:25571 | DlgBoxClick_Help | `EnableDialogBox(18, 913, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | only lang 3 |
| Game.cpp:25576 | DlgBoxClick_Help | `EnableDialogBox(18, 914, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | only lang 3 |
| Game.cpp:30594 | NotifyMsgHandler | `if( m_bCitizen == FALSE ) EnableDialogBox(18, 800, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:30595 | NotifyMsgHandler | `else if( m_bAresden == TRUE ) EnableDialogBox(18, 801, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:30596 | NotifyMsgHandler | `else if( m_bAresden == FALSE ) EnableDialogBox(18, 802, NULL, NULL);` | - | catch-all match: colour/text id/arguments, not a screen coordinate | default |
| Game.cpp:34821 | UpdateScreen_OnGame | `_beginthreadex(0, 0, ThreadProc, 0, 0, &uAddr);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | only lang 3 |
| Game.cpp:34908 | UpdateScreen_OnGame | `EnableDialogBox(20, 1, m_stDialogBoxInfo[17].sView, m_stDialogBoxInfo[17].sV3);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:34929 | UpdateScreen_OnGame | `EnableDialogBox(20, 3, m_stDialogBoxInfo[17].sView, m_stDialogBoxInfo[17].sV3);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:34951 | UpdateScreen_OnGame | `EnableDialogBox(20, 2, m_stDialogBoxInfo[17].sView, m_stDialogBoxInfo[17].sV3);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:35177 | UpdateScreen_OnGame | `PutString(msX, msY +25, cStr1, RGB(0,255,50), FALSE, 1);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:35179 | UpdateScreen_OnGame | `PutString(msX, msY +25, cStr1, RGB(255,255,255), FALSE, 1);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:35184 | UpdateScreen_OnGame | `PutString(msX, msY +25 +iLoc, cStr2, RGB(150,150,150), FALSE, 1);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:35189 | UpdateScreen_OnGame | `PutString(msX, msY +25 +iLoc, cStr3, RGB(150,150,150), FALSE, 1);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:35195 | UpdateScreen_OnGame | `PutString(msX, msY +25 +iLoc, G_cTxt, RGB(150,150,150), FALSE, 1);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:35207 | UpdateScreen_OnGame | `PutString(msX, msY +25 +iLoc, G_cTxt, RGB(150,150,150), FALSE, 1);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:35213 | UpdateScreen_OnGame | `PutString(msX, msY +25 +iLoc, G_cTxt, RGB(150,150,150), FALSE, 1);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:35220 | UpdateScreen_OnGame | `PutString(msX, msY +25 +iLoc, G_cTxt, RGB(150,150,150), FALSE, 1);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | only lang 3 |
| Game.cpp:35229 | UpdateScreen_OnGame | `PutString(msX, msY +25 +iLoc, G_cTxt, RGB(150,150,150), FALSE, 1);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | only lang 3 |
| Game.cpp:35247 | UpdateScreen_OnGame | `PutString(msX, msY + 40, G_cTxt, RGB(150,150,150), FALSE, 1);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:35312 | UpdateScreen_OnGame | `PutString(msX, msY +30, G_cTxt, RGB(255,255,255));` | - | matched coordinate regex but no screen literal (msX/variables/ids) | dead in all builds |
| Game.cpp:36282 | CommandProcessor | `EnableDialogBox(20, 5, 11, 1);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:36299 | CommandProcessor | `EnableDialogBox(20, 0, 16, NULL);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:36316 | CommandProcessor | `EnableDialogBox(20, 0, 14, NULL);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:36340 | CommandProcessor | `EnableDialogBox(20, 5, 11, 2);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:36362 | CommandProcessor | `EnableDialogBox(20, 0, 13, NULL);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:36379 | CommandProcessor | `EnableDialogBox(20, 0, 7, NULL);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:36396 | CommandProcessor | `EnableDialogBox(20, 4, NULL, NULL);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:36413 | CommandProcessor | `EnableDialogBox(20, 4, NULL, NULL);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:36429 | CommandProcessor | `EnableDialogBox(20, 4, NULL, NULL);` | - | matched coordinate regex but no screen literal (msX/variables/ids) | default |
| Game.cpp:42103 | bItemDrop_SellList | `m_stDialogBoxInfo[17].sX  = msX - 140;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:42104 | bItemDrop_SellList | `m_stDialogBoxInfo[17].sY  = msY - 70;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:42105 | bItemDrop_SellList | `if (m_stDialogBoxInfo[17].sY < 0) m_stDialogBoxInfo[17].sY = 0;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:42191 | bItemDrop_Bank | `m_stDialogBoxInfo[17].sX  = msX - 140;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:42192 | bItemDrop_Bank | `m_stDialogBoxInfo[17].sY  = msY - 70;` | - | cursor- or dialog-relative placement | default |
| Game.cpp:42193 | bItemDrop_Bank | `if (m_stDialogBoxInfo[17].sY < 0) m_stDialogBoxInfo[17].sY = 0;` | - | cursor- or dialog-relative placement | default |

## PITCH_OK - uses the pitch variable, no literal width (129)

| Where | Function | Code | Proposed | Note | Build |
|---|---|---|---|---|---|
| DXC_ddraw.cpp:130 | bInit | `m_sBackB4Pitch       = (short)ddsd.lPitch >> 1;` | - | uses pitch variable; no literal width | default |
| DXC_ddraw.cpp:326 | ChangeDisplayMode | `m_sBackB4Pitch       = (short)ddsd.lPitch >> 1;` | - | uses pitch variable; no literal width | default |
| DXC_ddraw.cpp:533 | DrawShadowBox | `pDst = (WORD *)m_pBackB4Addr + sX + ((sY)*m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| DXC_ddraw.cpp:542 | DrawShadowBox | `pDst += m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| DXC_ddraw.cpp:551 | DrawShadowBox | `pDst += m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| DXC_ddraw.cpp:576 | DrawShadowBox | `pDst += m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| DXC_ddraw.cpp:587 | PutPixel | `pDst = (WORD *)m_pBackB4Addr + sX + ((sY)*m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| DXC_ddraw.h:71 | (file scope) | `short  m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Game.cpp:23348 | DrawLine | `pDst = (WORD *)m_DDraw.m_pBackB4Addr + iResultX + ((iResultY)*m_DDraw.m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Game.cpp:23382 | DrawLine | `pDst = (WORD *)m_DDraw.m_pBackB4Addr + iResultX + ((iResultY)*m_DDraw.m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Game.cpp:23456 | DrawLine2 | `pDst = (WORD *)m_DDraw.m_pBackB4Addr + iResultX + ((iResultY)*m_DDraw.m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Game.cpp:23490 | DrawLine2 | `pDst = (WORD *)m_DDraw.m_pBackB4Addr + iResultX + ((iResultY)*m_DDraw.m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:729 | PutShadowSprite | `pSrc = (WORD *)m_pSurfaceAddr + sx + sy*m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:744 | PutShadowSprite | `pDst[iSangY*m_pDDraw->m_sBackB4Pitch+iSangX] = ((pDst[iSangY*m_pDDraw->m_sBackB4Pitch+iSangX] & 0xE79C) >> 2);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:748 | PutShadowSprite | `pSrc += m_sPitch + m_sPitch + m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:762 | PutShadowSprite | `pDst[iSangY*m_pDDraw->m_sBackB4Pitch+iSangX] = ((pDst[iSangY*m_pDDraw->m_sBackB4Pitch+iSangX] & 0x739C) >> 2);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:766 | PutShadowSprite | `pSrc += m_sPitch + m_sPitch + m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:840 | PutShadowSpriteClip | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy+szy-1)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:841 | PutShadowSpriteClip | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY+szy-1)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:851 | PutShadowSpriteClip | `pSrc -= m_sPitch + m_sPitch + m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:852 | PutShadowSpriteClip | `pDst -= m_pDDraw->m_sBackB4Pitch + 1;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:863 | PutShadowSpriteClip | `pSrc -= m_sPitch + m_sPitch + m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:864 | PutShadowSpriteClip | `pDst -= m_pDDraw->m_sBackB4Pitch + 1;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:957 | PutTransSprite | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:958 | PutTransSprite | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:974 | PutTransSprite | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:975 | PutTransSprite | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:991 | PutTransSprite | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:992 | PutTransSprite | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1086 | PutTransSprite_NoColorKey | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1087 | PutTransSprite_NoColorKey | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1100 | PutTransSprite_NoColorKey | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1101 | PutTransSprite_NoColorKey | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1114 | PutTransSprite_NoColorKey | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1115 | PutTransSprite_NoColorKey | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1209 | PutTransSprite70 | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1210 | PutTransSprite70 | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1226 | PutTransSprite70 | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1227 | PutTransSprite70 | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1243 | PutTransSprite70 | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1244 | PutTransSprite70 | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1339 | PutTransSprite70_NoColorKey | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1340 | PutTransSprite70_NoColorKey | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1353 | PutTransSprite70_NoColorKey | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1354 | PutTransSprite70_NoColorKey | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1367 | PutTransSprite70_NoColorKey | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1368 | PutTransSprite70_NoColorKey | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1462 | PutTransSprite50 | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1463 | PutTransSprite50 | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1479 | PutTransSprite50 | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1480 | PutTransSprite50 | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1496 | PutTransSprite50 | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1497 | PutTransSprite50 | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1591 | PutTransSprite50_NoColorKey | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1592 | PutTransSprite50_NoColorKey | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1605 | PutTransSprite50_NoColorKey | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1606 | PutTransSprite50_NoColorKey | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1619 | PutTransSprite50_NoColorKey | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1620 | PutTransSprite50_NoColorKey | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1714 | PutTransSprite25 | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1715 | PutTransSprite25 | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1731 | PutTransSprite25 | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1732 | PutTransSprite25 | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1748 | PutTransSprite25 | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1749 | PutTransSprite25 | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1843 | PutTransSprite25_NoColorKey | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1844 | PutTransSprite25_NoColorKey | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1857 | PutTransSprite25_NoColorKey | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1858 | PutTransSprite25_NoColorKey | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1871 | PutTransSprite25_NoColorKey | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1872 | PutTransSprite25_NoColorKey | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1954 | PutTransSprite2 | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1955 | PutTransSprite2 | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1971 | PutTransSprite2 | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1972 | PutTransSprite2 | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1988 | PutTransSprite2 | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:1989 | PutTransSprite2 | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2073 | PutShiftTransSprite2 | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2074 | PutShiftTransSprite2 | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2090 | PutShiftTransSprite2 | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2091 | PutShiftTransSprite2 | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2107 | PutShiftTransSprite2 | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2108 | PutShiftTransSprite2 | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2200 | PutFadeSprite | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2201 | PutFadeSprite | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2211 | PutFadeSprite | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2212 | PutFadeSprite | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2223 | PutFadeSprite | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2224 | PutFadeSprite | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2232 | PutFadeSpriteDst | `void CSprite::PutFadeSpriteDst(WORD * pDstAddr, short sPitch, short sX, short sY, short sFrame, DWORD dwTime)` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2314 | PutFadeSpriteDst | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2315 | PutFadeSpriteDst | `pDst = (WORD *)pDstAddr + dX + ((dY)*sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2325 | PutFadeSpriteDst | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2326 | PutFadeSpriteDst | `pDst += sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2337 | PutFadeSpriteDst | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2338 | PutFadeSpriteDst | `pDst += sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2360 | _iOpenSprite | `m_sPitch = (short)ddsd.lPitch >> 1;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2464 | PutSpriteRGB | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2465 | PutSpriteRGB | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2484 | PutSpriteRGB | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2485 | PutSpriteRGB | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2501 | PutSpriteRGB | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2502 | PutSpriteRGB | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2599 | PutTransSpriteRGB | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2600 | PutTransSpriteRGB | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2619 | PutTransSpriteRGB | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2620 | PutTransSpriteRGB | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2636 | PutTransSpriteRGB | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2637 | PutTransSpriteRGB | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2731 | PutTransSpriteRGB_NoColorKey | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2732 | PutTransSpriteRGB_NoColorKey | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2750 | PutTransSpriteRGB_NoColorKey | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2751 | PutTransSpriteRGB_NoColorKey | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2764 | PutTransSpriteRGB_NoColorKey | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2765 | PutTransSpriteRGB_NoColorKey | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2894 | _SetAlphaDegree | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:2924 | _SetAlphaDegree | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:3009 | _bCheckCollison | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:3014 | _bCheckCollison | `//  pSrc += m_sPitch * ( msY - tY );` | - | uses pitch variable; no literal width (commented) | commented |
| Sprite.cpp:3020 | _bCheckCollison | `pSrc += m_sPitch * ( msY - tY - 3 );` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:3027 | _bCheckCollison | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:3212 | PutRevTransSprite | `pSrc = (WORD *)m_pSurfaceAddr + sx + ((sy)*m_sPitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:3213 | PutRevTransSprite | `pDst = (WORD *)m_pDDraw->m_pBackB4Addr + dX + ((dY)*m_pDDraw->m_sBackB4Pitch);` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:3232 | PutRevTransSprite | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:3233 | PutRevTransSprite | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:3252 | PutRevTransSprite | `pSrc += m_sPitch;` | - | uses pitch variable; no literal width | default |
| Sprite.cpp:3253 | PutRevTransSprite | `pDst += m_pDDraw->m_sBackB4Pitch;` | - | uses pitch variable; no literal width | default |
| Sprite.h:71 | (file scope) | `void PutFadeSpriteDst(WORD * pDstAddr, short sPitch, short sX, short sY, short sFrame, DWORD dwTime);` | - | uses pitch variable; no literal width | default |
| Sprite.h:91 | (file scope) | `short m_sPitch;` | - | uses pitch variable; no literal width | default |

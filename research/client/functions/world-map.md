# Functions: Map and client-side world

Source: `sources\client-2.20\` (librehelbreath ClientSources c01544d). 34 function definitions (including `#if` duplicate bodies). Generated from a col-0 signature scan, then reviewed by hand.

Column key: **Where** file:start-end (verified). **Purpose** from name pattern and the nearest source comment (inferred unless the row ends in [V], meaning re-checked against the function body by the 2026-10-25 review, or the overview says verified; comments are UTF-8 Korean in source, quoted as-is). **Callers/Callees** matched by bare name across all files (verified as text matches; same-named methods in different classes are merged). **Msgs** MSGID/DEF_NOTIFY/etc. names in the body (see `..\..\shared\messages\`). **Dlg** dialog ids enabled/disabled (ids in `..\data-structures.md`). **Files** path literals in the body.

## Overview

- **CMapData** (verified): holds the map cell array around the player and the per-type animation frame table `m_stFrame`; `_bDecodeMapDataContents`/`OpenMapDataFile` read `mapdata\<name>.amd` (format: `..\..\shared\formats\`). `bSetOwner`, `bSetItem`, `bSetDynamicObject`, `bSetDeadOwner` place server-sent objects into cells; `ShiftMapData` scrolls the window when the player moves; `iObjectFrameCounter` advances animations and returns the redraw flag used by the frame loop.
- **Tiles** (verified): `CTile` is one cell (owner, item, dynamic object, tile and object sprite ids). `CTileSpr` holds tile sprite metadata.
- **Client logic**: teleport/panning responses, weather (`SetWhetherStatus`), guide map and minimap, view-point easing (`CalcViewPoint`). The minimap dot uses map width for both axes (Game.cpp:17924, [RENDER-002](../../issues/rendering.md#render-002-guide-map-full-mode-divides-both-axes-by-map-width)).

## Function table

| Function | Where | Lines | Purpose | Callers | Callees | Msgs | Dlg | Files |
|---|---|---|---|---|---|---|---|---|
| `CGame::CalcViewPoint` | Game.cpp:1238-1274 | 37 | ease camera towards target [V] | UpdateScreen_OnGame | - | - | - | - |
| `CGame::_ReadMapData` | Game.cpp:14417-14608 | 192 | comment: 기준점은 데이터가 유효한 사각형의 기준 위치로 삼는다. | InitDataResponseHandler, MotionResponseHandler, ResponsePanningHandler | bSetDeadOwner, bSetDynamicObject, bSetItem, bSetOwner | DEF_OBJECTSTOP | - | - |
| `CGame::_RequestMapStatus` | Game.cpp:19240-19243 | 4 | (name only) | DlgBoxClick_Commander, DrawDialogBox_Commander, DrawDialogBox_Constructor, DrawDialogBox_Soldier, NotifyMsgHandler | bSendCommand | DEF_COMMONTYPE_REQUEST_MAPSTATUS, MSGID_COMMAND_COMMON | - | - |
| `CGame::AddMapStatusInfo` | Game.cpp:19245-19289 | 45 | comment: 이후 리스트의 내용을 모두 클라어한다. | NotifyMsgHandler | - | - | - | - |
| `CGame::DrawObjectFOE` | Game.cpp:19995-20001 | 7 | (name only) | DrawObject_OnAttack, DrawObject_OnAttackMove, DrawObject_OnDamage, DrawObject_OnDamageMove, DrawObject_OnDying, DrawObject_OnGetItem, DrawObject_OnMagic, DrawObject_OnMove, +2 | PutTransSprite, _iGetFOE | - | - | - |
| `CGame::DynamicObjectHandler` | Game.cpp:22854-22895 | 42 | comment: Dyamic Object Index | GameRecvMsgHandler | bSetDynamicObject | DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT | - | - |
| `CGame::WhetherObjectFrameCounter` | Game.cpp:23174-23256 | 83 | comment: 날씨 효과의 속도 오락가락함을 막기 위함. | UpdateScreen_OnGame | - | - | - | - |
| `CGame::DrawObjectName` | Game.cpp:33462-33618 | 157 | comment: 기본은 중립  v2.171 | DrawObject_OnAttack, DrawObject_OnAttackMove, DrawObject_OnDamage, DrawObject_OnDamageMove, DrawObject_OnDead, DrawObject_OnDying, DrawObject_OnGetItem, DrawObject_OnMagic, +3 | FindGuildName, PutString2, _iGetFOE, bSendCommand | DEF_COMMONTYPE_REQGUILDNAME, MSGID_COMMAND_COMMON | - | - |
| `CGame::GetOfficialMapName` | Game.cpp:33743-33998 | 256 | comment: 리턴값은 MapIndex를 뜻한다. | DrawDialogBox_CityHallMenu, DrawDialogBox_Constructor, DrawDialogBox_Quest, DrawDialogBox_Soldier, InitDataResponseHandler, NotifyMsgHandler, NpcTalkHandler | - | - | - | - |
| `CMapData::CMapData` | MapData.cpp:11-695 | 685 | comment: Construction/Destruction | bInit | - | DEF_OBJECTATTACK, DEF_OBJECTATTACKMOVE, DEF_OBJECTDAMAGE, DEF_OBJECTDAMAGEMOVE, DEF_OBJECTDYING, DEF_OBJECTGETITEM, +4 | - | - |
| `CMapData::Init` | MapData.cpp:697-712 | 16 | (name only) | InitDataResponseHandler | Clear | - | - | - |
| `CMapData::~CMapData` | MapData.cpp:714-716 | 3 | (name only) | - | - | - | - | - |
| `CMapData::OpenMapDataFile` | MapData.cpp:718-761 | 44 | (name only) | InitDataResponseHandler | _bDecodeMapInfo | - | - | - |
| `CMapData::_bDecodeMapInfo` | MapData.cpp:763-799 | 37 | parses MapInfo; comment: 먼저 널문자를 없앤다. | OpenMapDataFile | - | - | - | - |
| `CMapData::ShiftMapData` | MapData.cpp:801-863 | 63 | (name only) | MotionResponseHandler, ResponsePanningHandler | Clear | - | - | - |
| `CMapData::bGetIsLocateable` | MapData.cpp:865-880 | 16 | TRUE if the cell is inside the window, has no owner, is walkable and holds no mineral; the window test is inclusive (> pivot+40/+35), so exactly pivot+40 / +35 reaches m_pData[dX][dY], one past the array (MapData.cpp:867-875, [CLIENT-STATE-004](../../issues/client-state.md#client-state-004-bgetislocateable-indexes-out-of-range)) | cGetNextMoveDir | - | - | - | - |
| `CMapData::bIsTeleportLoc` | MapData.cpp:882-890 | 9 | TRUE if the tile inside the window has m_bIsTeleport set (same inclusive window test as bGetIsLocateable; indexes m_tile, not m_pData) | CommandProcessor, cGetNextMoveDir | - | - | - | - |
| `CMapData::bSetOwner` | MapData.cpp:892-1643 | 752 | comment: ++++++++++ | CommandProcessor, InitDataResponseHandler, LogEventHandler, MotionEventHandler, MotionResponseHandler, NotifyMsgHandler, _ReadMapData | RequestFullObjectData | DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT, DEF_OBJECTATTACKMOVE, DEF_OBJECTDAMAGEMOVE, DEF_OBJECTDYING, DEF_OBJECTMOVE, +2 | - | - |
| `CMapData::bGetOwner` | MapData.cpp:1646-1676 | 31 | comment: v1.4 | CommandProcessor, bItemDrop_ExternalScreen | - | - | - | - |
| `CMapData::bGetDeadOwner` | MapData.cpp:1678-1709 | 32 | comment: v1.4 | - | - | - | - | - |
| `CMapData::iObjectFrameCounter` | MapData.cpp:1711-3382 | 1672 | comment: 맵데이터 상의 오브젝트의 프레임 카운트를 증가시킨다. | UpdateScreen_OnGame | PlaySound, bAddNewEffect, cGetNextMoveDir | DEF_OBJECTATTACK, DEF_OBJECTATTACKMOVE, DEF_OBJECTDAMAGE, DEF_OBJECTDAMAGEMOVE, DEF_OBJECTDYING, DEF_OBJECTMAGIC, +3 | - | - |
| `CMapData::bSetItem` | MapData.cpp:3385-3420 | 36 | comment: 한계를 벗어난 좌표에는 아이템을 설정할 수 없다. | CommonEventHandler, _ReadMapData | PlaySound, bAddNewEffect | - | - | - |
| `CMapData::bSetDeadOwner` | MapData.cpp:3422-3497 | 76 | comment: 한계를 벗어난 좌표. 맵상에 존재한다면 무조건 지운다. | _ReadMapData | - | - | - | - |
| `CMapData::bSetChatMsgOwner` | MapData.cpp:3499-3548 | 50 | comment: 좌표한계를 벗어난 메시지. 소용없다. | ChatMsgHandler, CommandProcessor, MotionEventHandler, NotifyMsg_LevelUp, NotifyMsg_Skill | - | - | - | - |
| `CMapData::ClearChatMsg` | MapData.cpp:3550-3559 | 10 | comment: v1.411 | DrawObject_OnAttack, DrawObject_OnAttackMove, DrawObject_OnDamage, DrawObject_OnDamageMove, DrawObject_OnDying, DrawObject_OnGetItem, DrawObject_OnMagic, DrawObject_OnMove, +3 | - | - | - | - |
| `CMapData::ClearDeadChatMsg` | MapData.cpp:3561-3564 | 4 | (name only) | DrawObject_OnDead | - | - | - | - |
| `CMapData::bGetOwner` | MapData.cpp:3566-3585 | 20 | (name only) | CommandProcessor, bItemDrop_ExternalScreen | - | - | - | - |
| `CMapData::bSetDynamicObject` | MapData.cpp:3587-3644 | 58 | comment: 한계를 벗어난 좌표에는 아이템을 설정할 수 없다. | DynamicObjectHandler, _ReadMapData | bAddNewEffect | - | - | - |
| `CMapData::GetOwnerStatusByObjectID` | MapData.cpp:3646-3665 | 20 | (name only) | _SetIlusionEffect | - | - | - | - |
| `CTile::CTile` | Tile.cpp:11-34 | 24 | comment: Construction/Destruction | - | - | - | - | - |
| `CTile::~CTile` | Tile.cpp:36-39 | 4 | (name only) | - | - | - | - | - |
| `CTile::Clear` | Tile.cpp:41-88 | 48 | comment: v1.4 | Init, ShiftMapData | - | - | - | - |
| `CTileSpr::CTileSpr` | TileSpr.cpp:11-19 | 9 | comment: Construction/Destruction | - | - | - | - | - |
| `CTileSpr::~CTileSpr` | TileSpr.cpp:21-24 | 4 | (name only) | - | - | - | - | - |

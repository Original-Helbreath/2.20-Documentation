# Maps, movement, teleport, weather, dynamic objects

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
`cfg:` means data loaded from that config, not a literal file open in the body [V, review G1].
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 60

## CGame::iClientMotion_Move_Handler - SRV Game.cpp:1128-1498

Validates a move/run (coords), then a speed-hack kick: walk, 7 moves in LESS than 72*8*7-3000 = 1032 ms -> DeleteClient (1153-1160); run is separate, 43*8*7-1500 = 908 ms (1168-1175); both skipped under NO_MSGSPEEDCHECK (off). Moves the player, runs the quest-complete check (1224), applies spike-tile damage 2d4 (1239-1249; skipped for neutral players in peace mode, and admins lose no HP), charges SP -1 per run step only while m_iTimeLeft_FirmStaminar is 0 (SP < -10 kicks, 1329-1351), and sends the new map strip. There is no teleport-tile logic in 1128-1498 (no bGetIsTeleport or RequestTeleportHandler call); teleporting is a separate request.

- sig: `int CGame::iClientMotion_Move_Handler(int iClientH, short sX, short sY, char cDir, BOOL bIsRun)`
- callers: CGame::ClientMotionHandler
- callees: ClearOwner, ClearSkillUsingStatus, DeleteClient, SetOwner, _bCheckIsQuestCompleted, bGetMoveable, iComposeMoveMapData, iDice, iGetPlayerABSStatus, iSendMsg
- messages: MSGID_RESPONSE_MOTION
- types: DEF_OBJECTMOVE_CONFIRM, DEF_OBJECTMOVE_REJECT
- switches: NO_MSGSPEEDCHECK
- Korean comments (UTF-8, first 6, untranslated quote): 죄표가 일치하지 않는 명령은 무시한다. Reject메시지를 받기 이전에 전송된 메시지들이다. / Crusade 속도 조작 체크 / 7번 움직인 경우 최소 소요 시간은 72*8*7 = 4032ms. 허용 오차 3000ms

## CGame::iComposeInitMapData - SRV Game.cpp:2252-2569

Builds the initial visible objects block for a client.

- sig: `int CGame::iComposeInitMapData(short sX, short sY, int iClientH, char * pData)`
- callers: CGame::RequestInitDataHandler, CGame::RequestTeleportHandler
- callees: iGetNpcRelationship, iGetPlayerABSStatus
- Korean comments (UTF-8, first 6, untranslated quote): @@@ !!! @@@ 이곳에 조건이 입력되야만 정보가 전달된다!!! / 위치정보 입력 / 헤더 정보를 작성한다.

## CGame::iComposeMoveMapData - SRV Game.cpp:3359-3664

Builds the newly visible strip after a move.

- sig: `int CGame::iComposeMoveMapData(short sX, short sY, int iClientH, char cDir, char * pData)`
- callers: CGame::iClientMotion_Move_Handler, CGame::iRequestPanningMapDataRequest
- callees: iGetNpcRelationship, iGetPlayerABSStatus
- Korean comments (UTF-8, first 6, untranslated quote): @@@ !!! @@@ 이곳에 조건이 입력되야만 정보가 전달된다!!! / 위치정보 입력 / 헤더 정보를 작성한다.

## CGame::iClientMotion_Stop_Handler - SRV Game.cpp:16475-16545

Stop/idle motion validation.

- sig: `int CGame::iClientMotion_Stop_Handler(int iClientH, short sX, short sY, char cDir)`
- callers: CGame::ClientMotionHandler
- callees: ClearOwner, ClearSkillUsingStatus, DeleteClient, GetOwner, SetOwner, iSendMsg
- messages: MSGID_RESPONSE_MOTION
- types: DEF_OBJECTMOTION_CONFIRM
- Korean comments (UTF-8, first 6, untranslated quote): 죄표가 일치하지 않는 명령은 무시한다. Reject메시지를 받기 이전에 전송된 메시지들이다. / 죽은척하기 중 일어나는데 위에 물체가 있으면 접속을 종료시킨다. / 스킬의 사용상태를 모두 무효화 한다. //!!!!

## CGame::RequestTeleportHandler - SRV Game.cpp:23250-23857

Teleport to map (tile, recall, GM, jail, server change via MSGID_REQUEST_SAVEPLAYERDATA_REPLY): locked-map rules, enemy town recall block, force-recall timing, resend of init data. The portal path (bSearchTeleportDest) is taken whenever the player stands on a portal tile and no map name is passed, regardless of `pData[0]` ([MAP-001](../../issues/map.md#map-001-teleport-request-mode-byte-is-the-low-byte-of-the-message-id)); '0' is forced recall and falls through to '1' ([MAP-002](../../issues/map.md#map-002-forced-recall-case-0-has-no-body-and-no-break)); '1' recall to home (neutral -> "default"; level > 80 -> own city, else farm arefarm/elvfarm) (23387-23406); '2' and '3' go to a named map/x/y. Modes '1' and '3' are blocked for non-admins standing in the enemy city (23270-23281). Also clears poison (23533); the shop price modifier cPrice is computed and not used (23540-23547) ([MAP-004](../../issues/map.md#map-004-crusade-winner-shop-discount-price-computed-and-never-used)).

- sig: `void CGame::RequestTeleportHandler(int iClientH, char * pData, char * cMapName, int dX, int dY)`
- callers: CGame::AdminOrder_SummonPlayer, CGame::AdminOrder_Teleport, CGame::ApplyPKpenalty, CGame::CheckClientResponseTime, CGame::InitPlayerData, CGame::MsgProcess, CGame::PlayerMagicHandler, CGame::QuestAcceptedHandler, CGame::RequestChargedTeleportHandler, CGame::RequestGuildTeleportHandler, CGame::RequestRestartHandler, CGame::ServerStockMsgHandler, CGame::UseItemHandler
- callees: ClearOwner, DeleteClient, GetMapInitialPoint, PutLogEventFileList, PutLogList, RemoveFromTarget, SendEventToNearClient_TypeA, SendNotifyMsg, SetForceRecallTime, SetOwner, _CheckQuestEnvironment, _ClearExchangeStatus, _SendQuestContents, bGetEmptyPosition, bSearchTeleportDest, bSendMsgToLS, iComposeInitMapData, iGetMapIndex, iGetMapLocationSide, iSendMsg, iSetSide
- messages: MSGID_EVENT_LOG, MSGID_REQUEST_SAVEPLAYERDATA_REPLY, MSGID_RESPONSE_INITDATA
- notifies: DEF_NOTIFY_CONSTRUCTIONPOINT, DEF_NOTIFY_CRUSADE, DEF_NOTIFY_DOWNSKILLINDEXSET, DEF_NOTIFY_FORCERECALLTIME, DEF_NOTIFY_GIZONITEMUPGRADELEFT, DEF_NOTIFY_ITEMPOSLIST, DEF_NOTIFY_LOCKEDMAP, DEF_NOTIFY_MAGICEFFECTOFF, DEF_NOTIFY_SAFEATTACKMODE, DEF_NOTIFY_SPECIALABILITYENABLED
- types: DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT
- switches: DEF_GUILDWARMODE, DEF_V219
- Korean comments (UTF-8, first 6, untranslated quote): v2.16 2002-6-2 상대방 마을에서는 리콜이 되지 않는다. / v2.181 2002-10-24 서버 속도 향상을 위해 스트링 비교를 줄인다. / 엘바인이면

## CGame::RequestTeleportListHandler - SRV Game.cpp:23861-24002

v2.20 paid teleport list from an NPC (MSGID_REQUEST_TELEPORT_LIST).

- sig: `void CGame::RequestTeleportListHandler(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: DeleteClient, iSendMsg
- messages: MSGID_RESPONSE_TELEPORT_LIST
- types: DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT
- cfg: TeleportList
- Korean comments (UTF-8, first 6, untranslated quote): 클라이언트가 보내온 NPC 이름 / 사용자의 현재 맵이 Source Map과 다른 경우는 보내지 않는다. / 보내오는 NPC 이름이 같은지 비교한다.

## CGame::RequestChargedTeleportHandler - SRV Game.cpp:24005-24155

Executes a paid teleport with level/side/hunter/criminal checks (six error codes: level, hunter, neutral, criminal, side, gold; 24044-24114). On success the fee is also added to m_stCityStatus[side].iFunds (24110).

- sig: `void CGame::RequestChargedTeleportHandler(int iClientH, char *pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: DeleteClient, RequestTeleportHandler, SetItemCount, dwGetItemCount, iCalcTotalWeight, iSendMsg
- messages: MSGID_RESPONSE_CHARGED_TELEPORT
- types: DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT
- cfg: TeleportList
- Korean comments (UTF-8, first 6, untranslated quote): 클라이언트가 보내온 CHARGED_TELEPORT index / 사용자의 현재 맵이 Source Map과 다른 경우는 보내지 않는다. / 레벨 검사

## CGame::_bDecodeTeleportListConfigFileContents - SRV Game.cpp:24655-24892

Parses the TeleportList config.

- sig: `BOOL CGame::_bDecodeTeleportListConfigFileContents(char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: CStrTok, PutLogList, _bGetIsStringIsNumber, pGet
- cfg: TeleportList
- Korean comments (UTF-8, first 3, untranslated quote): Teleport 번호 / 이미 할당된 번호가 있다. 에러이다. / NPC 이름

## CGame::__bReadMapInfo - SRV Game.cpp:25556-27113

Parses mapdata/<map>.txt (25556-27113). Tokens: teleport-loc, waypoint, npc, random-mob-generator, maximum-object, npc-avoidrect, spot-mob-generator, map-location (sets the location name used by town/side logic), initial-point, no-attack-area, fixed-dayornight-mode, fish-point, max-fish, type, level-limit, upper-level-limit, mineral-generator, mineral-point, max-mineral, strategic-point, energy-sphere-creation-point, energy-sphere-goal-point, strike-point, item-event, energy-sphere-auto-creation, mobevent-amount (v2.17). Fightzone is a map-name prefix `fight` (25586); fixed snow is the `icebound` map or the date 2002-12-25 only (25590-25598); these are not file tokens.

- sig: `BOOL CGame::__bReadMapInfo(int iMapIndex)`
- callers: CGame::_bReadMapInfoFiles
- callees: CStrTok, PutLogList, SetNamingValueEmpty, _SetupNoAttackArea, _bGetIsStringIsNumber, bCreateNewNpc, iGetEmptyNamingValue, pGet
- Korean comments (UTF-8, first 6, untranslated quote): 사투장인지를 판단. / 2002-7-4 사투장의 갯수를 늘릴 수 있도록 / v2.20 2002-12-20 크리스마스와 iceboud맵에서는 항상 눈이 내린다.

## CGame::iAddDynamicObjectList - SRV Game.cpp:28579-28640

Registers a dynamic object (fire, ice storm, poison cloud, spike, fish, mineral) and broadcasts MSGID_DYNAMICOBJECT.

- sig: `int CGame::iAddDynamicObjectList(short sOwner, char cOwnerType, short sType, char cMapIndex, short sX, short sY, DWORD dwLastTime, int iV1)`
- callers: CGame::CheckFireBluring, CGame::MeteorStrikeHandler, CGame::PlayerMagicHandler, CGame::__bSetOccupyFlag, CGame::iCreateFish, CGame::iCreateMineral
- callees: CDynamicObject, SendEventToNearClient_TypeB, SetDynamicObject, SetTempMoveAllowedFlag, bGetDynamicObject, bGetIsMoveAllowedTile, bGetIsWater, bGetMoveable, iDice
- messages: MSGID_DYNAMICOBJECT
- types: DEF_MSGTYPE_CONFIRM
- Korean comments (UTF-8, first 6, untranslated quote): 만약 이미 위치에 객체가 존재하면 생성할 수 없다. / 다이나믹 오브젝트의 종류에 따라 놓을 수 있는 여부를 판별한다. / 비가 온다거나 하는 일이 있으면 지속 시간이 줄어든다.

## CGame::CheckDynamicObjectList - SRV Game.cpp:28642-28698

Expires dynamic objects (rain shortens fire).

- sig: `void CGame::CheckDynamicObjectList()`
- callers: CGame::OnTimer
- callees: SendEventToNearClient_TypeB, SetDynamicObject, bDeleteFish, bGetDynamicObject
- messages: MSGID_DYNAMICOBJECT
- types: DEF_MSGTYPE_REJECT
- Korean comments (UTF-8, first 6, untranslated quote): 날씨 효과에 따른 동적 객체의 지속시간 단축을 계산한다. / 불의 경우 비가 옴으로 인해 수명이 급격히 단축된다. / (남은 시간/10)*비오는 상태 만큼 시간을 뺀다.

## CGame::RequestFullObjectData - SRV Game.cpp:30349-30490

Sends full appearance data for one object id.

- sig: `void CGame::RequestFullObjectData(int iClientH, char *pData)`
- callers: CGame::MsgProcess
- callees: DeleteClient, iGetNpcRelationship, iGetPlayerABSStatus, iSendMsg
- messages: MSGID_EVENT_MOTION
- types: DEF_OBJECTSTOP
- Korean comments (UTF-8, first 6, untranslated quote): 캐릭터의 정보를 원한다. / 잘못된 인덱스값이거나 존재하지 않는 플레이어라면 무시. / m_pClientList[i]와 m_pClientList[sOwnerH]의 관계를 입력한다.

## CGame::DynamicObjectEffectProcessor - SRV Game.cpp:33283-33671

Applies field damage to occupants for three dynamic-object types only: DEF_DYNAMICOBJECT_PCLOUD_BEGIN (33293, poison cloud), ICESTORM (33406) and FIRE (33545). Spike damage is not handled here; it is applied when a player walks onto the tile, in iClientMotion_Move_Handler (1239-1249).

- sig: `void CGame::DynamicObjectEffectProcessor()`
- callers: CGame::OnTimer
- callees: CheckFireBluring, ClientKilledHandler, GetDeadOwner, GetOwner, NpcKilledHandler, SendEventToNearClient_TypeA, SendNotifyMsg, SetIceFlag, _bItemLog, bCheckResistingIceSuccess, bCheckResistingMagicSuccess, bGetDynamicObject, bRegisterDelayEvent, bRemoveFromDelayEventList, iDice
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_HP, DEF_NOTIFY_MAGICEFFECTOFF, DEF_NOTIFY_MAGICEFFECTON
- types: DEF_OBJECTDAMAGE
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 포이즌 클라우드 / Poison Damage를 입는다. / v2.17 2002-8-5 운영자는 필드류 마법에 데미지를 받지 않는다.

## CGame::_iGetPlayerNumberOnSpot - SRV Game.cpp:35815-35829

Players near a point.

- sig: `int CGame::_iGetPlayerNumberOnSpot(short dX, short dY, char cMapIndex, char cRange)`
- callers: CGame::bCreateNewNpc
- callees: GetOwner

## CGame::CheckDayOrNightMode - SRV Game.cpp:36049-36074

Day if minute < 40 else night; notifies on change.

- sig: `void CGame::CheckDayOrNightMode()`
- callers: CGame::OnTimer
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_TIMECHANGE
- Korean comments (UTF-8, first 3, untranslated quote): 현재 시간에 따라 낮 혹은 밤을 결정한다. / 주, 야간 모드 설정 / 주, 야간 모드가 변경되었다. 주, 야간 모드의 영향을 받는 맵에 존재하는 클라이언트들에게 알린다.

## CGame::_iCalcPlayerNum - SRV Game.cpp:37360-37384

Players within a radius.

- sig: `int CGame::_iCalcPlayerNum(char cMapIndex, short dX, short dY, char cRadius)`
- callers: CGame::PlayerMagicHandler, CGame::TargetSearch
- Korean comments (UTF-8, first 2, untranslated quote): 처리 속도를 높이기 위해 함수를 콜하지 않는다. / 좌표를 벗어나므로 처리하지 않는다.

## CGame::WhetherProcessor - SRV Game.cpp:37387-37437

Weather state machine per map. Maps with fixed day/night mode are skipped (37396). Chance 1/300 per map per 20 s to start, duration 3-10 min (37408-37416). The token `defined` in the switches list is only the preprocessor keyword of `#if defined(DEF_WINTER)`; DEF_WINTER is off, so only rain (1-3) is generated, except on fixed-snow maps.

- sig: `void CGame::WhetherProcessor()`
- callers: CGame::OnTimer
- callees: SendNotifyMsg, iDice
- notifies: DEF_NOTIFY_WHETHERCHANGE
- switches: DEF_WINTER (inactive; the scan's `defined` token is the preprocessor keyword)
- Korean comments (UTF-8, first 6, untranslated quote): 날씨가 변동되어야 하는 맵이라면 처리한다. / 날씨의 변동을 계산한다. / 현재 날씨가 작동중이다. 멈출때가 되었으면 멈춘다.

## CGame::iGetMapIndex - SRV Game.cpp:37953-37969

Map name -> index.

- sig: `int CGame::iGetMapIndex(char * pMapName)`
- callers: CGame::RequestTeleportHandler

## CGame::SpecialEventHandler - SRV Game.cpp:37990-38006

Spawns nothing itself. Every DEF_MOBEVENTTIME (300000 ms, Game.h:100) it sets m_bIsSpecialEventTime = TRUE and m_cSpecialEventType = 2 when 1d350 == 98, else 1 (37998-38005). MobGenerator consumes the flag (1 in 10 per map pass, 29550-29600).

- sig: `void CGame::SpecialEventHandler()`
- callers: CGame::OnTimer
- callees: iDice
- Korean comments (UTF-8, first 3, untranslated quote): 특별한 이벤트를 생성한다. / v2.17 몹 이벤트 시간이 5분으로 단축 되었으므로 데몬나올 학률도 감소 시킴 / 데몬 혹은 유니콘이 나올 가능성은 30분에 1번 1/30

## CGame::GetMapInitialPoint - SRV Game.cpp:42342-42367

Picks a random one of the map's defined initial points; the first point is chosen only when the pPlayerLocation string starts with `NONE` (42361-42363). Returns without writing if the map has no points.

- sig: `void CGame::GetMapInitialPoint(int iMapIndex, short *pX, short *pY, char * pPlayerLocation)`
- callers: CGame::InitPlayerData, CGame::RequestInitDataHandler, CGame::RequestTeleportHandler, CGame::ResurrectPlayer, CGame::_bDecodePlayerDatafileContents, CGame::bGetEmptyPosition
- callees: iDice
- Korean comments (UTF-8, first 2, untranslated quote): 리스트를 작성한다. / v1.42 소속이 중립이면 무조건 Initial Point의 처음으로 간다.

## CGame::iRequestPanningMapDataRequest - SRV Game.cpp:46156-46222

Observer panning (MSGID_REQUEST_PANNING).

- sig: `int CGame::iRequestPanningMapDataRequest(int iClientH, char * pData)`
- callers: CGame::MsgProcess
- callees: DeleteClient, iComposeMoveMapData, iSendMsg
- messages: MSGID_RESPONSE_PANNING
- types: DEF_OBJECTMOVE_CONFIRM
- Korean comments (UTF-8, first 1, untranslated quote): 메시지를 보낼때 에러가 발생했다면 제거한다.

## CGame::UpdateMapSectorInfo - SRV Game.cpp:47553-47610

Not a copy: finds the hotspot sector per category (neutral, Aresden, Elvine, monster, player) in the temp grid, clears the temp grid, and adds +1 to the permanent grid only at each hotspot cell (hotspot index 0 is ignored) (47567-47608). Called every 10 s (3811-3813); AgingMapSectorInfo runs every 5th call (3818-3822). The m_iMaxPx/Py hotspot is used for mob-event placement (29556).

- sig: `void CGame::UpdateMapSectorInfo()`
- callers: CGame::OnTimer
- callees: ClearTempSectorInfo
- Korean comments (UTF-8, first 3, untranslated quote): 그동안 저장했던 TempSectorInfo에서 정보를 얻어 SectorInfo에 저장한 다음 TempSectorInfo는 지운다. / TempSectorInfo 날린다. / Sector Info에 저장

## CGame::AgingMapSectorInfo - SRV Game.cpp:47613-47634

Decays sector activity counts.

- sig: `void CGame::AgingMapSectorInfo()`
- callers: CGame::OnTimer

## CGame::iGetMapLocationSide - SRV Game.cpp:50563-50612

Side code for a map name: `aresden`/`elvine` and `arebrk11`/`elvbrk11` return 3/4 (side + 2); buildings and farms (cityhall, cath, gshop, bsmith, wrhus, gldhall, wzdtwr, arefarm/elvfarm, arewrhus/elvwrhus) return 1/2; anything else 0 (50568-50611). RequestTeleportHandler subtracts 2 when the result is >= 3 (23330).

- sig: `int CGame::iGetMapLocationSide(char *pMapName)`
- callers: CGame::RequestInitDataHandler, CGame::RequestTeleportHandler
- Korean comments (UTF-8, first 1, untranslated quote): 건물안 혹은 농경지만 검사할때는 필요 없다.

## CMap::CMap - SRV Map.cpp:13-160

Constructor: nulls the teleport/waypoint/avoid-rect/strike-point/structure tables, clears counters and flags, default mob-event amount 15 (Map.cpp:13-160).

- sig: `CMap::CMap(class CGame * pGame)`
- callers: CGame::_bRegisterMap
- Korean comments (UTF-8, first 2, untranslated quote): v2.20 2002-12-20 크리스마스와 iceboud맵에서는 항상 눈이 내린다. / 2002-7-5 맵에서 몹이벤트 시간및 양을 조절 가능하게 디폴트 15

## CMap::~CMap - SRV Map.cpp:162-178

Destructor: frees the tile array, teleport locations, occupy flags and strategic points.

- sig: `CMap::~CMap()`

## CMap::SetOwner - SRV Map.cpp:180-189

Stores the owner handle and owner class (player/NPC) on a tile; ignores out-of-range coordinates.

- sig: `void CMap::SetOwner(short sOwner, char cOwnerClass, short sX, short sY)`
- callers: CGame::AdminOrder_SetObserverMode, CGame::DoMeteorStrikeDamageHandler, CGame::Effect_Damage_Spot, CGame::Effect_Damage_Spot_DamageMove, CGame::NpcBehavior_Attack, CGame::NpcBehavior_Flee, CGame::NpcBehavior_Move, CGame::RequestInitDataHandler, CGame::RequestTeleportHandler, CGame::bCreateNewNpc, CGame::iCalculateAttackEffect, CGame::iClientMotion_Attack_Handler, CGame::iClientMotion_GetItem_Handler, CGame::iClientMotion_Magic_Handler, CGame::iClientMotion_Move_Handler, CGame::iClientMotion_Stop_Handler

## CMap::SetDeadOwner - SRV Map.cpp:192-201

Stores the dead-owner handle and class on a tile; ignores out-of-range coordinates.

- sig: `void CMap::SetDeadOwner(short sOwner, char cOwnerClass, short sX, short sY)`
- callers: CGame::ClientKilledHandler, CGame::NpcKilledHandler, CGame::UseSkillHandler

## CMap::GetOwner - SRV Map.cpp:204-219

Reads a tile's owner handle and class (0 on out-of-range or empty tile).

- sig: `void CMap::GetOwner(short * pOwner, char * pOwnerClass, short sX, short sY)`
- callers: CGame::CalculateGuildEffect, CGame::DynamicObjectEffectProcessor, CGame::ExchangeItemHandler, CGame::GiveItemHandler, CGame::MeteorStrikeHandler, CGame::NpcMagicHandler, CGame::NpcRequestAssistance, CGame::PlayerMagicHandler, CGame::RequestSummonWarUnitHandler, CGame::TargetSearch, CGame::UseSkillHandler, CGame::_TamingHandler, CGame::__bSetAgricultureItem, CGame::__bSetConstructionKit, CGame::_bNpcBehavior_Detector, CGame::_bNpcBehavior_ManaCollector, CGame::_iGetPlayerNumberOnSpot, CGame::bAnalyzeCriminalAction, CGame::bSerchMaster, CGame::iClientMotion_Attack_Handler, CGame::iClientMotion_Stop_Handler, CGame::iGetDangerValue

## CMap::GetDeadOwner - SRV Map.cpp:222-235

Reads a tile's dead-owner handle and class (0 on out-of-range).

- sig: `void CMap::GetDeadOwner(short * pOwner, char * pOwnerClass, short sX, short sY)`
- callers: CGame::DynamicObjectEffectProcessor, CGame::NpcMagicHandler, CGame::PlayerMagicHandler, CGame::UseSkillHandler

## CMap::bGetMoveable - SRV Map.cpp:238-254

Walkability test: FALSE within 20 tiles of the map edge, if the tile has an owner, is not move-allowed, or is temporarily blocked; also returns the tile's dynamic-object type through pDOtype. The `pTopItem` parameter is assigned by value and never reaches the caller ([TILE-002](../../issues/tile.md#tile-002-bgetmoveable-assigns-to-its-by-value-ptopitem-parameter-land-mine-feature-unfinished)).

- sig: `BOOL CMap::bGetMoveable(short dX, short dY, short * pDOtype, class CItem * pTopItem) // v2.172`
- callers: CGame::Effect_Damage_Spot, CGame::bGetEmptyPosition, CGame::cGetNextMoveDir, CGame::iAddDynamicObjectList, CGame::iCalculateAttackEffect, CGame::iClientMotion_Move_Handler

## CMap::bGetIsMoveAllowedTile - SRV Map.cpp:256-268

Like bGetMoveable but ignores the owner: FALSE within 20 tiles of the edge, on not-move-allowed or temp-blocked tiles.

- sig: `BOOL CMap::bGetIsMoveAllowedTile(short dX, short dY)`
- callers: CGame::PlayerMagicHandler, CGame::iAddDynamicObjectList

## CMap::bGetIsTeleport - SRV Map.cpp:270-281

TRUE if the tile has the teleport attribute (bounds margin 14/16/12/14).

- sig: `BOOL CMap::bGetIsTeleport(short dX, short dY)`
- callers: CGame::bGetEmptyPosition

## CMap::ClearOwner - SRV Map.cpp:283-302

Clears a tile's owner and dead owner when they match the given handle and class.

- sig: `void CMap::ClearOwner(int iDebugCode, short sOwnerH, char cOwnerType, short sX, short sY)`
- callers: CGame::AdminOrder_SetObserverMode, CGame::ClientKilledHandler, CGame::DeleteClient, CGame::DeleteNpc, CGame::DoMeteorStrikeDamageHandler, CGame::Effect_Damage_Spot, CGame::Effect_Damage_Spot_DamageMove, CGame::NpcBehavior_Attack, CGame::NpcBehavior_Flee, CGame::NpcBehavior_Move, CGame::NpcKilledHandler, CGame::RequestTeleportHandler, CGame::ResurrectPlayer, CGame::TargetSearch, CGame::UseSkillHandler, CGame::iCalculateAttackEffect, CGame::iClientMotion_Attack_Handler, CGame::iClientMotion_GetItem_Handler, CGame::iClientMotion_Magic_Handler, CGame::iClientMotion_Move_Handler, CGame::iClientMotion_Stop_Handler
- Korean comments (UTF-8, first 1, untranslated quote): 현 위치에 핸들이 일치하면 모두 클리어한다.

## CMap::ClearDeadOwner - SRV Map.cpp:304-313

Clears the dead owner of a tile. Zero callers ([TILE-003](../../issues/tile.md#tile-003-cleardeadowner-and-clearsectorinfo-have-no-callers)).

- sig: `void CMap::ClearDeadOwner(short sX, short sY)`

## CMap::bSetItem - SRV Map.cpp:315-335

Pushes an item onto the tile's item stack at slot 0, deleting the bottom item if the stack is full.

- sig: `BOOL CMap::bSetItem(short sX, short sY, class CItem * pItem)`
- callers: CGame::DeleteNpc, CGame::DropItemHandler, CGame::GiveItemHandler, CGame::NpcDeadItemGenerator, CGame::NpcDeadItemGeneratorWithItemEvent, CGame::PlayerMagicHandler, CGame::ReqCreatePortionHandler, CGame::ReqGetFishThisTimeHandler, CGame::ReqSellItemConfirmHandler, CGame::_CheckMiningAction, CGame::bAddItem, CGame::bCropsItemDrop, CGame::iCalculateUseSkillItemEffect, CGame::iClientMotion_GetItem_Handler
- Korean comments (UTF-8, first 1, untranslated quote): v1.4 마지막 타일에 아이템이 있다면 삭제시키고 이동한다.

## CMap::pGetItem - SRV Map.cpp:338-368

Pops the top item off a tile's stack and reports the sprite, frame and colour of the item now on top.

- sig: `class CItem * CMap::pGetItem(short sX, short sY, short * pRemainItemSprite, short * pRemainItemSpriteFrame, char * pRemainItemColor) //v1.4 color`
- callers: CGame::CheckFireBluring, CGame::PlayerMagicHandler, CGame::iClientMotion_GetItem_Handler

## CMap::iCheckItem - SRV Map.cpp:371-383

Returns the item id of the tile's top item, or 0 if empty.

- sig: `int CMap::iCheckItem(short sX, short sY)`
- callers: CGame::CheckFireBluring

## CMap::bIsValidLoc - SRV Map.cpp:386-390

TRUE if (x, y) is inside the map bounds.

- sig: `BOOL CMap::bIsValidLoc(short sX, short sY)`
- callers: CGame::_bDecodePlayerDatafileContents

## CMap::bInit - SRV Map.cpp:392-408

Loads the map data file: stores the map name, calls _bDecodeMapDataFileContents and nulls the teleport table.

- sig: `BOOL CMap::bInit(char * pName)`
- callers: CGame::_bRegisterMap (Game.cpp:5512) only. The name-scan callers OnTimer (3799) and Initialize (Wmain.cpp:248) call the unrelated CGame::bInit().
- callees: _bDecodeMapDataFileContents
- Korean comments (UTF-8, first 1, untranslated quote): 맵파일을 로드한다.

## CMap::_bDecodeMapDataFileContents - SRV Map.cpp:410-512

Reads mapdata\<name>.amd: a 256-byte header with MAPSIZEX, MAPSIZEY and TILESIZE, then one record per tile; byte 8 bit 0x80 = blocked, 0x40 = teleport, 0x20 = farmable, and a short value 19 = water.

- sig: `BOOL CMap::_bDecodeMapDataFileContents()`
- callers: CMap::bInit
- callees: CStrTok, pGet
- Korean comments (UTF-8, first 6, untranslated quote): 헤더정보를 분석한다. / 먼저 널문자를 없앤다. / 크기에 맞게 타일을 생성한다.

## CMap::bSearchTeleportDest - SRV Map.cpp:515-530

Looks up the teleport location whose source is (x, y) and returns its destination map, coordinates and direction.

- sig: `BOOL CMap::bSearchTeleportDest(int sX, int sY, char * pMapName, int * pDx, int * pDy, char * pDir)`
- callers: CGame::RequestTeleportHandler
- Korean comments (UTF-8, first 1, untranslated quote): 같은 좌표를 갖는 텔레포트 위치를 찾았다.

## CMap::SetDynamicObject - SRV Map.cpp:532-544

Stores a dynamic-object id, type and register time on a tile.

- sig: `void CMap::SetDynamicObject(WORD wID, short sType, short sX, short sY, DWORD dwRegisterTime)`
- callers: CGame::CheckDynamicObjectList, CGame::_DeleteRandomOccupyFlag, CGame::__bSetOccupyFlag, CGame::bDeleteFish, CGame::bDeleteMineral, CGame::iAddDynamicObjectList

## CMap::bGetDynamicObject - SRV Map.cpp:546-560

Reads a tile's dynamic-object type, register time and index.

- sig: `BOOL CMap::bGetDynamicObject(short sX, short sY, short *pType, DWORD *pRegisterTime, int * pIndex)`
- callers: CGame::CheckDynamicObjectList, CGame::DynamicObjectEffectProcessor, CGame::_CheckMiningAction, CGame::iAddDynamicObjectList, CGame::iGetDangerValue

## CMap::iGetEmptyNamingValue - SRV Map.cpp:562-574

Reserves and returns the first free naming value (0-999) for a new NPC name, or -1 if none.

- sig: `int CMap::iGetEmptyNamingValue()`
- callers: CGame::AdminOrder_CallGuard, CGame::AdminOrder_Summon, CGame::AdminOrder_SummonDemon, CGame::CreateCrusadeStructures, CGame::EnergySphereProcessor, CGame::MobGenerator, CGame::PlayerMagicHandler, CGame::RequestSummonWarUnitHandler, CGame::__bReadMapInfo, CGame::__bSetAgricultureItem, CGame::__bSetConstructionKit, CGame::bAnalyzeCriminalAction

## CMap::SetNamingValueEmpty - SRV Map.cpp:576-579

Releases a naming value.

- sig: `void CMap::SetNamingValueEmpty(int iValue)`
- callers: CGame::AdminOrder_CallGuard, CGame::AdminOrder_Summon, CGame::AdminOrder_SummonDemon, CGame::CreateCrusadeStructures, CGame::DeleteNpc, CGame::EnergySphereProcessor, CGame::MobGenerator, CGame::PlayerMagicHandler, CGame::RequestSummonWarUnitHandler, CGame::__bReadMapInfo, CGame::__bSetAgricultureItem, CGame::__bSetConstructionKit, CGame::bAnalyzeCriminalAction

## CMap::bGetIsWater - SRV Map.cpp:581-592

TRUE if the tile is water (bounds margin 14/16/12/14).

- sig: `BOOL CMap::bGetIsWater(short dX, short dY)`
- callers: CGame::iAddDynamicObjectList, CGame::iCalculateUseSkillItemEffect, CGame::iCreateFish

## CMap::bGetIsFarm - SRV Map.cpp:596-607

TRUE if the tile allows farming (bounds margin 14/16/12/14).

- sig: `BOOL CMap::bGetIsFarm(short dX, short dY)`
- callers: CGame::__bSetAgricultureItem

## CMap::iAnalyze - SRV Map.cpp:610-625

Stub: a switch on query type 1 (find where fighting is) with an empty body; always returns 0. Zero callers ([MAP-009](../../issues/map.md#map-009-cmapianalyze-is-an-empty-stub)).

- sig: `int CMap::iAnalyze(char cType, int * pX, int * pY, int * pV1, int * pV2, int * pV3)`
- Korean comments (UTF-8, first 2, untranslated quote): 현재 맵의 상황을 분석하여 쿼리에 맞는 위치를 반환한다. / 현재 교전이 벌어지고 있는 곳의 위치를 찾는다.

## CMap::SetTempMoveAllowedFlag - SRV Map.cpp:627-635

Sets or clears the temporary move-allowed flag of a tile (used by structures that block movement).

- sig: `void CMap::SetTempMoveAllowedFlag(int dX, int dY, BOOL bFlag)`
- callers: CGame::bDeleteMineral, CGame::iAddDynamicObjectList

## CMap::iRegisterOccupyFlag - SRV Map.cpp:637-652

Registers an occupy flag in the first free slot (1 and up) and returns its index, or -1.

- sig: `int CMap::iRegisterOccupyFlag(int dX, int dY, int iSide, int iEKNum, int iDOI)`
- callers: CGame::__bSetOccupyFlag
- callees: COccupyFlag
- Korean comments (UTF-8, first 1, untranslated quote): 값을 할당하고 반환.

## CMap::ClearSectorInfo - SRV Map.cpp:656-668

Zeroes the permanent sector activity grid. Zero callers ([TILE-003](../../issues/tile.md#tile-003-cleardeadowner-and-clearsectorinfo-have-no-callers)).

- sig: `void CMap::ClearSectorInfo()`

## CMap::ClearTempSectorInfo - SRV Map.cpp:670-682

Zeroes the temporary sector activity grid.

- sig: `void CMap::ClearTempSectorInfo()`
- callers: CGame::UpdateMapSectorInfo

## CMap::_SetupNoAttackArea - SRV Map.cpp:684-707

Sets attribute bit 0x4 (safe zone) on every tile inside the no-attack rectangles, or on the whole map when a rect top is -10.

- sig: `void CMap::_SetupNoAttackArea()`
- callers: CGame::__bReadMapInfo
- Korean comments (UTF-8, first 4, untranslated quote): No-Magic-Area가 정의되어 있다. / 3번째 비트를 세팅한다. 안전지대 / 전 영역 공격 금지다.

## CMap::iGetAttribute - SRV Map.cpp:709-717

Returns a tile's attribute word masked with iBitMask (-1 near the map edge).

- sig: `int CMap::iGetAttribute(int dX, int dY, int iBitMask)`
- callers: CGame::Effect_Damage_Spot, CGame::Effect_Damage_Spot_DamageMove, CGame::PlayerMagicHandler, CGame::iCalculateAttackEffect

## CMap::bAddCrusadeStructureInfo - SRV Map.cpp:719-735

Records a crusade structure (type, side, position) in the first free slot and counts it.

- sig: `BOOL CMap::bAddCrusadeStructureInfo(char cType, short sX, short sY, char cSide)`
- callers: CGame::bCreateNewNpc

## CMap::bRemoveCrusadeStructureInfo - SRV Map.cpp:737-769

Removes the crusade structure at (x, y) and compacts the list.

- sig: `BOOL CMap::bRemoveCrusadeStructureInfo(short sX, short sY)`
- callers: CGame::DeleteNpc

## CMap::bAddCropsTotalSum - SRV Map.cpp:773-781

Increments the map's crop count if below DEF_MAXAGRICULTURE.

- sig: `BOOL CMap::bAddCropsTotalSum()`
- callers: CGame::bCreateNewNpc

## CMap::bRemoveCropsTotalSum - SRV Map.cpp:784-796

Decrements the map's crop count (floored at 0); only acts while the count is below DEF_MAXAGRICULTURE ([GATHERING-002](../../issues/gathering.md#gathering-002-crop-count-can-never-fall-once-a-map-reaches-200)).

- sig: `BOOL CMap::bRemoveCropsTotalSum()`
- callers: CGame::DeleteNpc, CGame::NpcKilledHandler

## CMap::RestoreStrikePoints - SRV Map.cpp:798-805

Resets every strike point's HP to its initial HP.

- sig: `void CMap::RestoreStrikePoints()`
- callers: CGame::LocalStartCrusadeMode

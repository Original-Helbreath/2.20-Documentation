# Networking, sockets and message dispatch

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
`cfg:` means data loaded from that config, not a literal file open in the body [V, review G1].
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 39

## CGame::bAccept - SRV Game.cpp:399-464

Accepts a client socket into the first free m_pClientList slot (slots start at 1), records IP, adds to the shortcut list. Rejects (accepts into a dummy socket and returns FALSE) when the gate or log socket is down, the item/npc/magic/skill/portion/quest/buildItem configs are not loaded, the game has not started, shutdown is in progress, or, under DEF_DEFENCEHACKING, before the WLserver config arrived; also when no slot is free. `XSocket::bAccept` (overload) is its callee (Game.cpp:431, 460) ([GSO-013](../../issues/game-server-other.md#gso-013-start-up-is-gated-on-the-wlserver-file-push-under-def_defencehacking)).

- sig: `BOOL CGame::bAccept(class XSocket * pXSock)`
- callers: OnAccept
- callees: CClient, PutLogList, XSocket, XSocket::bAccept, bAddClientShortCut, iGetPeerAddress
- switches: DEF_DEFENCEHACKING
- Korean comments (UTF-8, first 6, untranslated quote): 2002-12-8 World server 보안 설정을 위해 / 비어있는 배열을 찾는다. / 클라이언트 인덱스 리스트에 기록

## CGame::OnClientSocketEvent - SRV Game.cpp:467-512

WSAAsyncSelect handler for client sockets: on READCOMPLETE stamps m_dwTime and queues the message; deletes the client on close/error/oversize/bad confirm code (confirm-code mismatch uses DeleteClient(i,FALSE,TRUE); the other paths use (TRUE,TRUE)).

- sig: `void CGame::OnClientSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)`
- callers: WndProc
- callees: DeleteClient, OnClientRead, PutLogList, iOnSocketEvent
- Korean comments (UTF-8, first 6, untranslated quote): 메시지가 수신되었다. / 응답 시간을 기록한다. / 수신해야 할 메시지 크기가 버퍼보다 크다.	종료해야만 한다.

## CGame::OnClientRead - SRV Game.cpp:872-885

Copies a received client message into the message queue (bPutMsgQuene, DEF_MSGFROM_CLIENT).

- sig: `void CGame::OnClientRead(int iClientH)`
- callers: CGame::OnClientSocketEvent
- callees: PutLogList, bPutMsgQuene, pGetRcvDataPointer
- Korean comments (UTF-8, first 1, untranslated quote): 메시지 큐에 이상이 생겼다. 치명적인 에러.

## CGame::ClientMotionHandler - SRV Game.cpp:976-1126

Dispatches MSGID_COMMAND_MOTION by action (stop, move, run, attack, attack-move, damage-move, get item, magic) to iClientMotion_* handlers; there is no dir-change case in 976-1126. Responses are sent by the iClientMotion_* handlers; rejects go through SendObjectMotionRejectMsg. bCheckClientMoveFrequency guards move/run and bCheckClientAttackFrequency guards attack/attack-move; damage-move and get-item have no frequency check.

- sig: `void CGame::ClientMotionHandler(int iClientH, char * pData)`
- callers: CGame::MsgProcess
- callees: ClientKilledHandler, SendEventToNearClient_TypeA, SendObjectMotionRejectMsg, _CheckAttackType, bCheckClientAttackFrequency, bCheckClientMoveFrequency, iClientMotion_Attack_Handler, iClientMotion_GetItem_Handler, iClientMotion_Magic_Handler, iClientMotion_Move_Handler, iClientMotion_Stop_Handler
- messages: MSGID_EVENT_MOTION
- types: DEF_OBJECTATTACK, DEF_OBJECTATTACKMOVE, DEF_OBJECTDAMAGEMOVE, DEF_OBJECTGETITEM, DEF_OBJECTMAGIC, DEF_OBJECTMOVE, DEF_OBJECTRUN, DEF_OBJECTSTOP
- Korean comments (UTF-8, first 6, untranslated quote): 플레이어가 방향을 바꾸었다. / 인접한 클라이언트들에게 방향전환 이벤트를 알린다. / 달린것에 대한 효과를 처리한다. 체력감소등등

## CGame::SendEventToNearClient_TypeA - SRV Game.cpp:2890-3356

Broadcasts an object motion/log event (MSGID_EVENT_MOTION/MSGID_EVENT_LOG) for a player or NPC to clients in view, with per-receiver relationship status.

- sig: `void CGame::SendEventToNearClient_TypeA(short sOwnerH, char cOwnerType, DWORD dwMsgID, WORD wMsgType, short sV1, short sV2, short sV3)`
- callers: CGame::ActivateSpecialAbilityHandler, CGame::AdminOrder_SetObserverMode, CGame::ApplyPKpenalty, CGame::CalcTotalItemEffect, CGame::CheckClientResponseTime, CGame::ClientKilledHandler, CGame::ClientMotionHandler, CGame::DelayEventProcessor, CGame::DeleteClient, CGame::DeleteNpc, CGame::DismissGuildApproveHandler, CGame::DoMeteorStrikeDamageHandler, CGame::DynamicObjectEffectProcessor, CGame::Effect_Damage_Spot, CGame::Effect_Damage_Spot_DamageMove, CGame::GiveItemHandler, CGame::JoinGuildApproveHandler, CGame::NpcBehavior_Attack, CGame::NpcBehavior_Flee, CGame::NpcBehavior_Move, CGame::NpcBehavior_Stop, CGame::NpcKilledHandler, CGame::PlayerMagicHandler, CGame::ReleaseItemHandler, CGame::RequestCivilRightHandler ...
- callees: iGetNpcRelationship, iGetPlayerABSStatus, iSendMsg
- messages: MSGID_EVENT_LOG
- types: DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT, DEF_OBJECTATTACK, DEF_OBJECTATTACKMOVE, DEF_OBJECTDAMAGE, DEF_OBJECTDAMAGEMOVE, DEF_OBJECTDYING, DEF_OBJECTMAGIC, DEF_OBJECTMOVE, DEF_OBJECTNULLACTION, DEF_OBJECTRUN
- Korean comments (UTF-8, first 6, untranslated quote): 메시지 헤더 작성 / 메시지 전달 범위 설정 / 플레이어의 이벤트이다.

## CGame::OnMainLogSocketEvent - SRV Game.cpp:4144-4191

Main log socket handler: on connect sends MSGID_REQUEST_REGISTERGAMESERVER; on read queues DEF_MSGFROM_LOGSERVER; on loss starts shutdown mode 3.

- sig: `void CGame::OnMainLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)`
- callers: WndProc
- callees: OnMainLogRead, PutLogList, bSendMsgToLS, iOnSocketEvent
- messages: MSGID_REQUEST_REGISTERGAMESERVER
- Korean comments (UTF-8, first 6, untranslated quote): 로그서버로의 연결이 이루어 졌으므로 게임서버 등록 메시지를 전송한다. / 메시지가 수신되었다. / 확인코드가 일치하지 않는다.

## CGame::OnMainLogRead - SRV Game.cpp:4193-4204

Queues a main-log-socket message.

- sig: `void CGame::OnMainLogRead()`
- callers: CGame::OnMainLogSocketEvent
- callees: PutLogList, bPutMsgQuene, pGetRcvDataPointer
- Korean comments (UTF-8, first 1, untranslated quote): 메시지 큐에 이상이 생겼다. 치명적인 에러.

## CGame::bSendMsgToLS - SRV Game.cpp:4207-4792

Builds and sends every game->Log message (register, player data request/save/logout, guild create/disband/update, item/crusade/GM logs, shutdown, account wait status...) mostly over a round-robin sub-log socket; REGISTERGAMESERVER (4345-4382), GAMESERVERSHUTDOWNED and SENDSERVERSHUTDOWNMSG use m_pMainLogSock instead. The DEF_DBGAMESERVER branch (adds m_dwCharID, 4498-4509) is active; the `#else` file-mode branch (4512-4521) is inactive. No usable socket -> forced shutdown.

- sig: `BOOL CGame::bSendMsgToLS(DWORD dwMsg, int iClientH, BOOL bFlag,char * pData)`
- callers: CGame::AdminOrder_CallGuard, CGame::AdminOrder_CloseConn, CGame::AdminOrder_CreateItem, CGame::AdminOrder_ReserveFightzone, CGame::AdminOrder_Summon, CGame::AdminOrder_SummonDemon, CGame::AdminOrder_SummonPlayer, CGame::AdminOrder_Teleport, CGame::ChatMsgHandler, CGame::CheckClientResponseTime, CGame::DeleteClient, CGame::DismissGuildApproveHandler, CGame::GSM_RequestShutupPlayer, CGame::InitPlayerData, CGame::JoinGuildApproveHandler, CGame::MsgProcess, CGame::OnMainLogSocketEvent, CGame::RequestCreateNewGuildHandler, CGame::RequestDisbandGuildHandler, CGame::RequestInitPlayerHandler, CGame::RequestTeleportHandler, CGame::ServerStockMsgHandler, CGame::ShutUpPlayer, CGame::UserCommand_BanGuildsman, CGame::_Manager_Shutdown ...
- callees: PutLogFileList, PutLogList, XSocket, _bCheckSubLogSocketIndex, _iComposePlayerDataFileContents, bConnect, bInitBufferSize, iGetPeerAddress, iSendMsg
- messages: MSGID_ENTERGAMECONFIRM, MSGID_GAMECRUSADELOG, MSGID_GAMEITEMLOG, MSGID_GAMEMASTERLOG, MSGID_GAMESERVERSHUTDOWNED, MSGID_REQUEST_CREATENEWGUILD, MSGID_REQUEST_DISBANDGUILD, MSGID_REQUEST_NOSAVELOGOUT, MSGID_REQUEST_PLAYERDATA, MSGID_REQUEST_REGISTERGAMESERVER, MSGID_REQUEST_SAVEPLAYERDATA, MSGID_REQUEST_SAVEPLAYERDATALOGOUT, MSGID_REQUEST_SAVEPLAYERDATA_REPLY, MSGID_REQUEST_SETACCOUNTINITSTATUS, MSGID_REQUEST_SETACCOUNTWAITSTATUS, MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN, MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN, MSGID_SENDSERVERSHUTDOWNMSG
- types: DEF_MSGTYPE_CONFIRM
- cfg: GServer.cfg
- switches: DEF_DBGAMESERVER
- Korean comments (UTF-8, first 6, untranslated quote): 메시지를 보낼 Sub-log-socket 인덱스 전진 / 메시지를 보내기에 적당한 Sub-log-socket을 찾는다. / 보낼 수 있는 Sub-log-socket이 하나도 없다. 강제 셧다운 모드로 전환

## CGame::MsgProcess - SRV Game.cpp:14845-15272

Main dispatcher: drains the message queue and routes by source (client / log / gate) and MSGID to the handlers; handles shutdown requests and config-content pushes (Item/Npc/Magic/Skill/Quest/Potion/BuildItem/WLserver/DupItemID/Noticement/World/NpcItem/TeleportList). Also handles the F1+F4 shutdown hotkey, MSGID_TOTALGAMESERVERCLIENTS and MSGID_RESPONSE_REGISTERGAMESERVER; an unknown client MSGID -> DeleteClient.

- sig: `void CGame::MsgProcess()`
- callers: CGame::GameProcess
- callees: ChatMsgHandler, CheckConnectionHandler, ClientCommonHandler, ClientMotionHandler, DeleteClient, FightzoneReserveHandler, ForceDisconnectAccount, GuildNotifyHandler, LevelUpSettingsHandler, PartyOperationResultHandler, PutLogList, RequestAdminUserMode, RequestChargedTeleportHandler, RequestCheckAccountPasswordHandler, RequestCivilRightHandler, RequestCreateNewGuildHandler, RequestDisbandGuildHandler, RequestFullObjectData, RequestInitDataHandler, RequestInitPlayerHandler, RequestNoticementHandler, RequestRestartHandler, RequestRetrieveItemHandler, RequestSellItemListHandler, RequestTeleportHandler, RequestTeleportListHandler, ResponseCreateNewGuildHandler, ResponseDisbandGuildHandler, ResponsePlayerDataHandler, ResponseSavePlayerDataReplyHandler ...
- messages: MSGID_ADMINUSER, MSGID_BUILDITEMCONFIGURATIONCONTENTS, MSGID_BWM_COMMAND_SHUTUP, MSGID_BWM_INIT, MSGID_COMMAND_CHATMSG, MSGID_COMMAND_CHECKCONNECTION, MSGID_COMMAND_COMMON, MSGID_COMMAND_MOTION, MSGID_DUPITEMIDFILECONTENTS, MSGID_GAMESERVERSHUTDOWNED, MSGID_GUILDNOTIFY, MSGID_ITEMCONFIGURATIONCONTENTS, MSGID_LEVELUPSETTINGS, MSGID_MAGICCONFIGURATIONCONTENTS, MSGID_MANAGER_INIT, MSGID_MANAGER_SHUTDOWN, MSGID_NOTICEMENTFILECONTENTS, MSGID_NPCCONFIGURATIONCONTENTS, MSGID_NPCITEMCONFIGCONTENTS, MSGID_PARTYOPERATION, MSGID_PORTIONCONFIGURATIONCONTENTS, MSGID_QUESTCONFIGURATIONCONTENTS, MSGID_REQUEST_CHARGED_TELEPORT, MSGID_REQUEST_CHECKACCOUNTPASSWORD, MSGID_REQUEST_CIVILRIGHT, MSGID_REQUEST_CREATENEWGUILD, MSGID_REQUEST_DISBANDGUILD, MSGID_REQUEST_FIGHTZONE_RESERVE, MSGID_REQUEST_FORCEDISCONECTACCOUNT, MSGID_REQUEST_FULLOBJECTDATA, MSGID_REQUEST_INITDATA, MSGID_REQUEST_INITPLAYER, MSGID_REQUEST_NOTICEMENT, MSGID_REQUEST_PANNING, MSGID_REQUEST_RESTART, MSGID_REQUEST_RETRIEVEITEM, MSGID_REQUEST_SELLITEMLIST, MSGID_REQUEST_SETITEMPOS, MSGID_REQUEST_TELEPORT, MSGID_REQUEST_TELEPORT_LIST, MSGID_RESPONSE_CREATENEWGUILD, MSGID_RESPONSE_DISBANDGUILD, MSGID_RESPONSE_PLAYERDATA, MSGID_RESPONSE_REGISTERGAMESERVER, MSGID_RESPONSE_SAVEPLAYERDATA_REPLY, MSGID_SENDSERVERSHUTDOWNMSG, MSGID_SERVERSTOCKMSG, MSGID_SKILLCONFIGURATIONCONTENTS, MSGID_TELEPORTLISTCONTENTS, MSGID_TOTALGAMESERVERCLIENTS, MSGID_WLSERVERFILECONTENTS, MSGID_WORLDCONFIGCONTENTS
- notifies: DEF_NOTIFY_SERVERSHUTDOWN
- types: DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT
- switches: DEF_DEFENCEHACKING
- Korean comments (UTF-8, first 6, untranslated quote): 모든 클라이언트의 데이터를 저장한 후 프로그램을 종료해야 한다. / 게임 서버 셧다운이 시작되면 더이상의 클라이언트 접속이 거부된다. / 로그 서버에게 현재 서버가 셧다운 중임을 알린다.

## CGame::bPutMsgQuene - SRV Game.cpp:15275-15301

Enqueues (from, data, size, index, key) into the 100000-slot ring buffer m_pMsgQuene.

- sig: `BOOL CGame::bPutMsgQuene(char cFrom, char * pData, DWORD dwMsgSize, int iIndex, char cKey)`
- callers: CGame::OnClientRead, CGame::OnGateRead, CGame::OnMainLogRead, CGame::OnSubLogRead
- callees: bPut
- Korean comments (UTF-8, first 4, untranslated quote): 뮤텍스가 생성되어 있다. 어디선가 큐를 조작중이다. 그냥 리턴 / 메시지 큐가 다 찼다면 에러 / 메모리 할당이 잘못되어도 에러

## CGame::bGetMsgQuene - SRV Game.cpp:15304-15329

Dequeues the next message from the ring buffer.

- sig: `BOOL CGame::bGetMsgQuene(char * pFrom, char * pData, DWORD * pMsgSize, int * pIndex, char * pKey)`
- callers: CGame::MsgProcess
- callees: Get
- Korean comments (UTF-8, first 3, untranslated quote): 뮤텍스가 생성되어 있다. 어디선가 큐를 조작중이다. 그냥 리턴 / 가져갈 메시지가 없으면 에러 / 뮤텍스 반환

## CGame::ClientCommonHandler - SRV Game.cpp:15332-15594

Dispatches MSGID_COMMAND_COMMON by DEF_COMMONTYPE_* (items, magic, skills, guild, party, exchange, crusade, quest, NPC talk, fishing...) to the specific handlers.

- sig: `void CGame::ClientCommonHandler(int iClientH, char * pData)`
- callers: CGame::MsgProcess
- callees: ActivateSpecialAbilityHandler, BuildItemHandler, CancelExchangeItem, CancelQuestHandler, ConfirmExchangeItem, DismissGuildApproveHandler, DismissGuildRejectHandler, DropItemHandler, ExchangeItemHandler, GetFightzoneTicketHandler, GetHeroMantleHandler, GetMagicAbilityHandler, GetOccupyFlagHandler, GetRewardMoneyHandler, GiveItemHandler, JoinGuildApproveHandler, JoinGuildRejectHandler, JoinPartyHandler, MapStatusHandler, NpcTalkHandler, PlayerMagicHandler, QuestAcceptedHandler, ReleaseItemHandler, ReqCreatePortionHandler, ReqGetFishThisTimeHandler, ReqRepairItemCofirmHandler, ReqRepairItemHandler, ReqSellItemConfirmHandler, ReqSellItemHandler, RequestAcceptJoinPartyHandler ...
- Korean comments (UTF-8, first 6, untranslated quote): 2002-11-14 사냥꾼 모드 추가 / iV1의 값이 가입 신청 허가 여부이다. / v2.17 2002-7-31 케릭터 사망시 떨어트린 모든 아이템을 로그에 남기기 위해

## CGame::SendEventToNearClient_TypeB - SRV Game.cpp:16406-16473

Broadcasts a positional common event (item drop, magic, dynamic object) to clients near a map point, using the shortcut list.

- sig: `void CGame::SendEventToNearClient_TypeB(DWORD dwMsgID, WORD wMsgType, char cMapIndex, short sX, short sY, short sV1, short sV2, short sV3, short sV4)`
- callers: CGame::CheckDynamicObjectList, CGame::CheckFireBluring, CGame::DeleteNpc, CGame::DismissGuildApproveHandler, CGame::DropItemHandler, CGame::GiveItemHandler, CGame::JoinGuildApproveHandler, CGame::NpcDeadItemGenerator, CGame::NpcDeadItemGeneratorWithItemEvent, CGame::NpcMagicHandler, CGame::PlayerMagicHandler, CGame::ReqCreatePortionHandler, CGame::ReqGetFishThisTimeHandler, CGame::ReqSellItemConfirmHandler, CGame::ResponseDisbandGuildHandler, CGame::UserCommand_BanGuildsman, CGame::_CheckMiningAction, CGame::_DeleteRandomOccupyFlag, CGame::__bSetOccupyFlag, CGame::bAddItem, CGame::bCropsItemDrop, CGame::bDeleteFish, CGame::bDeleteMineral, CGame::iAddDynamicObjectList, CGame::iCalculateUseSkillItemEffect ...
- callees: iSendMsg
- Korean comments (UTF-8, first 1, untranslated quote): DEF_MAXCLIENTS 까지 루프를 돌지 않기 위한 방법. 클라이언트 숏컷 리스트에서 0이 나오면 끝이다.

## CGame::SendNotifyMsg - SRV Game.cpp:17692-19037

Builds and sends every MSGID_NOTIFY sub-type (DEF_NOTIFY_*) to one client (or a guild/broadcast for some types); 1300-line switch.

- sig: `void CGame::SendNotifyMsg(int iFromH, int iToH, WORD wMsgType, DWORD sV1, DWORD sV2, DWORD sV3, char * pString, DWORD sV4, DWORD sV5, DWORD sV6, DWORD sV7, DWORD sV8, DWORD sV9, char * pString2)`
- callers: CGame::ActivateSpecialAbilityHandler, CGame::AdminOrder_CallGuard, CGame::AdminOrder_CheckIP, CGame::AdminOrder_CloseConn, CGame::AdminOrder_CreateFish, CGame::AdminOrder_CreateItem, CGame::AdminOrder_DisconnectAll, CGame::AdminOrder_GetNpcStatus, CGame::AdminOrder_Polymorph, CGame::AdminOrder_ReserveFightzone, CGame::AdminOrder_SetAttackMode, CGame::AdminOrder_SetForceRecallTime, CGame::AdminOrder_SetInvi, CGame::AdminOrder_SetObserverMode, CGame::AdminOrder_Summon, CGame::AdminOrder_SummonDemon, CGame::AdminOrder_SummonPlayer, CGame::AdminOrder_Teleport, CGame::AdminOrder_UnsummonAll, CGame::AdminOrder_UnsummonBoss, CGame::ApplyCombatKilledPenalty, CGame::ApplyPKpenalty, CGame::ArmorLifeDecrement, CGame::BuildItemHandler, CGame::CalcExpStock ...
- callees: iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_ADMINIFO, DEF_NOTIFY_ADMINUSERLEVELLOW, DEF_NOTIFY_AGRICULTURENOAREA, DEF_NOTIFY_AGRICULTURESKILLLIMIT, DEF_NOTIFY_BUILDITEMFAIL, DEF_NOTIFY_BUILDITEMSUCCESS, DEF_NOTIFY_CANCELEXCHANGEITEM, DEF_NOTIFY_CANNOTBANGUILDMAN, DEF_NOTIFY_CANNOTCONSTRUCT, DEF_NOTIFY_CANNOTGIVEITEM, DEF_NOTIFY_CANNOTITEMTOBANK, DEF_NOTIFY_CANNOTJOINMOREGUILDSMAN, DEF_NOTIFY_CANNOTRATING, DEF_NOTIFY_CANNOTREPAIRITEM, DEF_NOTIFY_CANNOTSELLITEM, DEF_NOTIFY_CHARISMA, DEF_NOTIFY_CONSTRUCTIONPOINT, DEF_NOTIFY_CRUSADE, DEF_NOTIFY_DAMAGEMOVE, DEF_NOTIFY_DEBUGMSG, DEF_NOTIFY_DOWNSKILLINDEXSET, DEF_NOTIFY_DROPITEMFIN_COUNTCHANGED, DEF_NOTIFY_DROPITEMFIN_ERASEITEM, DEF_NOTIFY_ENEMYKILLREWARD, DEF_NOTIFY_ENEMYKILLS, DEF_NOTIFY_ENERGYSPHERECREATED, DEF_NOTIFY_ENERGYSPHEREGOALIN, DEF_NOTIFY_EVENTFISHMODE, DEF_NOTIFY_EXCHANGEITEMCOMPLETE, DEF_NOTIFY_EXP ...
- types: DEF_COMMONTYPE_DISMISSGUILDAPPROVE, DEF_COMMONTYPE_DISMISSGUILDREJECT, DEF_COMMONTYPE_JOINGUILDAPPROVE, DEF_COMMONTYPE_JOINGUILDREJECT
- cfg: GServer.cfg, WLserver cfg
- switches: DEF_DEFENCEHACKING
- Korean comments (UTF-8, first 6, untranslated quote): 여기까지 Msg 사이즈가 6이다. 다음에 오는 내용의 크기에 +6을 해서 iSendMsg에게 전달한다. / !!! sV1, sV2, sV3는 DWORD형임을 명심하라. / 2002-11-14 사냥꾼 모드 추가

## CGame::OnGateSocketEvent - SRV Game.cpp:20547-20611

Gate socket handler: on connect registers (MSGID_REQUEST_REGISTERGAMESERVER), queues reads (DEF_MSGFROM_GATESERVER), reconnects on error and counts retries.

- sig: `void CGame::OnGateSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)`
- callers: WndProc
- callees: OnGateRead, PutLogList, SendMsgToGateServer, XSocket, bConnect, bInitBufferSize, iOnSocketEvent
- messages: MSGID_REQUEST_REGISTERGAMESERVER
- cfg: GServer.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 게이트서버로의 연결이 이루어 졌으므로 게임서버 등록 메시지를 전송한다. / v1.41 gate-socket을 재연결할 때 카운팅 하는 변수. 서버 작동 도중 gate-socket이 소켓에러로 끊기면 곧바로 재접속을 / 시도하며 이 값이 일정치 이상이 되면 연결 실패로 간주되어 자동 서버 셧다운 모드(4)로 들어간다.

## CGame::OnGateRead - SRV Game.cpp:20613-20624

Queues a gate-socket message.

- sig: `void CGame::OnGateRead()`
- callers: CGame::OnGateSocketEvent
- callees: PutLogList, bPutMsgQuene, pGetRcvDataPointer
- Korean comments (UTF-8, first 1, untranslated quote): 메시지 큐에 이상이 생겼다. 치명적인 에러.

## CGame::SendMsgToGateServer - SRV Game.cpp:20626-20771

Builds game->gate messages: register, MSGID_GAMESERVERALIVE (user counts), party operations, server stock messages.

- sig: `void CGame::SendMsgToGateServer(DWORD dwMsg, int iClientH, char * pData)`
- callers: CGame::DeleteClient, CGame::GetPartyInfoHandler, CGame::InitPlayerData, CGame::JoinPartyHandler, CGame::OnGateSocketEvent, CGame::OnTimer, CGame::PartyOperationResult_Create, CGame::RequestAcceptJoinPartyHandler, CGame::RequestCreatePartyHandler, CGame::RequestDeletePartyHandler, CGame::RequestDismissPartyHandler, CGame::RequestJoinPartyHandler, CGame::SendStockMsgToGateServer
- callees: PutLogList, XSocket, bConnect, bInitBufferSize, iSendMsg
- messages: MSGID_GAMESERVERALIVE, MSGID_ITEMLOG, MSGID_PARTYOPERATION, MSGID_REQUEST_REGISTERGAMESERVER, MSGID_SERVERSTOCKMSG
- types: DEF_MSGTYPE_CONFIRM
- cfg: GServer.cfg
- Korean comments (UTF-8, first 6, untranslated quote): v2.06 12-03 게이트 서버로의 파티 관련 연산 요청 / 아이템 전달 로그다. 사용하지 않음. / 게이트 서버에게 게임서버 등록을 요청한다.

## CGame::SendObjectMotionRejectMsg - SRV Game.cpp:33236-33270

Sends MSGID_RESPONSE_MOTION reject so the client snaps back to the server position.

- sig: `void CGame::SendObjectMotionRejectMsg(int iClientH)`
- callers: CGame::ClientMotionHandler
- callees: DeleteClient, iSendMsg
- messages: MSGID_RESPONSE_MOTION
- types: DEF_OBJECTMOTION_REJECT
- Korean comments (UTF-8, first 2, untranslated quote): 이동이 불가능하다. / 메시지를 보낼때 에러가 발생했다면 제거한다.

## CGame::_iGetTotalClients - SRV Game.cpp:33272-33281

Counts non-NULL client slots.

- sig: `int CGame::_iGetTotalClients()`

## CGame::_iForcePlayerDisconect - SRV Game.cpp:37973-37988

Disconnects up to N players (used by the shutdown drain); the DeleteClient save flag is TRUE only for clients with m_bIsInitComplete, else (FALSE,FALSE) (37973-37988).

- sig: `int CGame::_iForcePlayerDisconect(int iNum)`
- callers: CGame::AdminOrder_DisconnectAll, CGame::OnTimer
- callees: DeleteClient

## CGame::ForceDisconnectAccount - SRV Game.cpp:38098-38117

Log-server request (MSGID_REQUEST_FORCEDISCONECTACCOUNT): for every client on the account, clamps wCount to 10 (38103) and sends DEF_NOTIFY_FORCEDISCONN; the DeleteClient call is commented out (38112), so it does not itself kick ([GSO-036](../../issues/game-server-other.md#gso-036-forcedisconnectaccount-does-not-disconnect)).

- sig: `void CGame::ForceDisconnectAccount(char *pAccountName, WORD wCount)`
- callers: CGame::MsgProcess
- callees: PutLogList, SendNotifyMsg
- notifies: DEF_NOTIFY_FORCEDISCONN
- Korean comments (UTF-8, first 3, untranslated quote): v2.15 가끔 강제 접속 카운트가 30000 이상 되는 경우 막기위해 의심 루틴 체크 추가 / 중복된 계정을 갖고있는 캐릭터를 삭제한다. / v1.42 카운팅 플래그를 활성화 해야 한다.

## CGame::_Manager_Init - SRV Game.cpp:40724-40794

v2.15 manager-program (remote console) init: requires a 10-byte password match, decrements m_iTotalClients, and replies with a 16-byte MSGID_RESPONSE_CHECKCONNECTION-style status (ready flag, client counts).

- sig: `void CGame::_Manager_Init(int iClientH, char * pData)`
- callers: CGame::MsgProcess
- callees: PutLogList, iSendMsg
- messages: MSGID_RESPONSE_CHECKCONNECTION
- switches: DEF_DEFENCEHACKING
- Korean comments (UTF-8, first 2, untranslated quote): 2002-12-8 World server 보안 설정을 위해 / 게임을 시작할 준비가 되었다. 메시지를 보낸다.

## CGame::_Manager_Shutdown - SRV Game.cpp:40797-40824

Manager-program shutdown command: DEAD in effect, the body starts with `return ;` (40799), so it is a no-op; the code after it (would start the exit process and tell the Log server MSGID_GAMESERVERSHUTDOWNED) never runs ([GSO-037](../../issues/game-server-other.md#gso-037-manager-program-shutdown-command-is-a-no-op)).

- sig: `void CGame::_Manager_Shutdown(int iClientH, char * pData)`
- callers: CGame::MsgProcess
- callees: PutLogList, bSendMsgToLS
- messages: MSGID_GAMESERVERSHUTDOWNED
- Korean comments (UTF-8, first 2, untranslated quote): 게임 서버 셧다운이 시작되면 더이상의 클라이언트 접속이 거부된다. / 로그 서버에게 현재 서버가 셧다운 중임을 알린다.

## CGame::SendItemNotifyMsg - SRV Game.cpp:42153-42286

Sends item-obtained / purchased / cannot-carry notifications with the full item record.

- sig: `void CGame::SendItemNotifyMsg(int iClientH, WORD wMsgType, CItem *pItem, int iV1)`
- callers: CGame::_iTalkToNpcResult_Cityhall
- callees: iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_CANNOTCARRYMOREITEM, DEF_NOTIFY_ITEMOBTAINED, DEF_NOTIFY_ITEMPURCHASED
- Korean comments (UTF-8, first 4, untranslated quote): 1개 획득했다. <- 여기서 1개란 수량 카운트를 말하는 것이 아니다 / 얻은 아이템이므로 장착되지 않았다. / Custom-Item인지의 여부

## CGame::OnSubLogSocketEvent - SRV Game.cpp:42393-42455

Handler for the 10 sub-log sockets (WM_ONLOGSOCKETEVENT+1..10): tracks active count, queues reads, marks failures.

- sig: `void CGame::OnSubLogSocketEvent(UINT message, WPARAM wParam, LPARAM lParam)`
- callers: WndProc
- callees: OnSubLogRead, PutLogFileList, PutLogList, XSocket, bConnect, bInitBufferSize, iOnSocketEvent
- cfg: GServer.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 로그서버로의 연결이 이루어 졌다. / 메시지가 수신되었다. / 확인코드가 일치하지 않는다.

## CGame::OnSubLogRead - SRV Game.cpp:42461-42472

Queues a sub-log-socket message.

- sig: `void CGame::OnSubLogRead(int iIndex)`
- callers: CGame::OnSubLogSocketEvent
- callees: PutLogList, bPutMsgQuene, pGetRcvDataPointer
- Korean comments (UTF-8, first 1, untranslated quote): 메시지 큐에 이상이 생겼다. 치명적인 에러.

## CGame::_CheckGateSockConnection - SRV Game.cpp:43099-43121

Counts missed gate connections (counter incremented once per OnTimer call, i.e. every 3 s, while m_iGateSockConnRetryTimes != 0); above 20 (about 63 s) enters auto-shutdown (code 4).

- sig: `void CGame::_CheckGateSockConnection()`
- callers: CGame::OnTimer
- callees: PutLogList
- Korean comments (UTF-8, first 4, untranslated quote): 이미 서버가 셧다운된 상태라면 카운팅 할 필요가 없다. / 60초간 Gate-socket이 연결되지 않았다면 자동 재부팅 모드로 들어간다. / 게이트 서버와의 연결이 끊겼으므로 복구할 수 없다. 자동으로 서버를 셧다운한다.

## CGame::_bCheckSubLogSocketIndex - SRV Game.cpp:43123-43154

Advances the round-robin sub-log socket index; if none usable, forces shutdown. Quirk: the probe loop does `index++` without wrapping, so when sockets are down it reads `m_pSubLogSock[10..19]` out of bounds before the 10-try cutoff (43137) [V, review R2-21] ([GSO-038](../../issues/game-server-other.md#gso-038-sub-log-socket-probe-reads-past-the-array-when-sockets-are-down)).

- sig: `BOOL CGame::_bCheckSubLogSocketIndex()`
- callers: CGame::bSendMsgToLS
- callees: PutLogList
- Korean comments (UTF-8, first 3, untranslated quote): 메시지를 보낼 Sub-log-socket 인덱스 전진 / 보낼 수 있는 Sub-log-socket이 하나도 없다. 강제 셧다운 모드로 전환 / 게임 서버 셧다운이 시작되면 더이상의 클라이언트 접속이 거부된다.

## CGame::RequestCheckAccountPasswordHandler - SRV Game.cpp:46052-46083

Log-server MSGID_REQUEST_CHECKACCOUNTPASSWORD: if a logged-in client on that account has a different password/level, drops it without saving.

- sig: `void CGame::RequestCheckAccountPasswordHandler(char *pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: DeleteClient, PutLogList
- Korean comments (UTF-8, first 2, untranslated quote): 같은 계정을 찾았다. 만약 패스워드나 레벨이 다르면 데이터 저장을 하지 않고 접속을 끊는다. / 데이터 저장을 하지 않고 끊는다.

## CGame::CheckConnectionHandler - SRV Game.cpp:48937-49039

MSGID_COMMAND_CHECKCONNECTION: compares client vs server elapsed time to catch speed hacks (disconnects only when the client elapsed time exceeds the server's by >= 10 s); also used by the manager to query readiness.

- sig: `void CGame::CheckConnectionHandler(int iClientH, char *pData)`
- callers: CGame::MsgProcess
- callees: DeleteClient, PutLogFileList, PutLogList, iSendMsg
- messages: MSGID_RESPONSE_CHECKCONNECTION
- switches: DEF_DEFENCEHACKING
- Korean comments (UTF-8, first 3, untranslated quote): 클라이언트에서 지나간 시간이 더 적다. 무시해도 된다. 속도를 사용한 경우는 서버보다 클라이언트가 시간이 더 간 경우. / 2002-12-8 World server 보안 설정을 위해 / 게임을 시작할 준비가 되었다. 메시지를 보낸다.

## CGame::bStockMsgToGateServer - SRV Game.cpp:49290-49306

Appends a GSM_* record to the gate stock buffer (flushed every second).

- sig: `BOOL CGame::bStockMsgToGateServer(char *pData, DWORD dwSize)`
- callers: CGame::AdminOrder_SetForceRecallTime, CGame::AdminOrder_SummonPlayer, CGame::CalcMeteorStrikeEffectHandler, CGame::ChatMsgHandler, CGame::CheckAndNotifyPlayerConnection, CGame::CheckCommanderConstructionPoint, CGame::ClientKilledHandler, CGame::DeleteClient, CGame::GSM_RequestFindCharacter, CGame::GSM_RequestShutupPlayer, CGame::GlobalStartCrusadeMode, CGame::ManualEndCrusadeMode, CGame::NpcKilledHandler, CGame::RequestSetGuildConstructLocHandler, CGame::RequestSetGuildTeleportLocHandler, CGame::SendCollectedMana, CGame::ShutUpPlayer, CGame::SyncMiddlelandMapInfo, CGame::ToggleWhisperPlayer, CGame::_GrandMagicLaunchMsgSend

## CGame::SendStockMsgToGateServer - SRV Game.cpp:49309-49336

Flushes the stock buffer as MSGID_SERVERSTOCKMSG to the gate and resets it.

- sig: `void CGame::SendStockMsgToGateServer()`
- callers: CGame::OnTimer
- callees: SendMsgToGateServer
- messages: MSGID_SERVERSTOCKMSG
- types: DEF_MSGTYPE_CONFIRM
- Korean comments (UTF-8, first 3, untranslated quote): 그동안 모아놓았던 메시지를 게이트 서버로 전송. / 버퍼 클리어 / 맨 앞부분에 메시지 삽입

## CGame::ServerStockMsgHandler - SRV Game.cpp:49339-49764

Parses an incoming MSGID_SERVERSTOCKMSG and executes each GSM_* record: chat, whisper, find character, shutup, summon player (GSM_REQUEST_SUMMONPLAYER, 49573), set force-recall time (GSM_REQUEST_SETFORCERECALLTIME, 49744), crusade begin/end, collected mana, grand magic launch/result, middleland status, guild teleport/construct loc, construction points, disconnect. The `_bItemLog` callee sits only in an inactive DEF_TAIWANLOG block (49598-49619).

- sig: `void CGame::ServerStockMsgHandler(char *pData)`
- callers: CGame::MsgProcess
- callees: ChatMsgHandlerGSM, CollectedManaHandler, GSM_ConstructionPoint, GSM_RequestFindCharacter, GSM_RequestShutupPlayer, GSM_SetGuildConstructLoc, GSM_SetGuildTeleportLoc, GrandMagicResultHandler, LocalEndCrusadeMode, LocalStartCrusadeMode, MeteorStrikeMsgHandler, PutLogList, RequestTeleportHandler, SendNotifyMsg, _bItemLog, bSendMsgToLS, iSendMsg
- messages: MSGID_GAMEMASTERLOG
- notifies: DEF_NOTIFY_PLAYERONGAME, DEF_NOTIFY_PLAYERSHUTUP, DEF_NOTIFY_WHISPERMODEOFF, DEF_NOTIFY_WHISPERMODEON
- cfg: World cfg
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 귓속말 상태가 해제되었음을 통보. / 구조체 클리어 / 정보 읽어들인다.

## CGame::bAddClientShortCut - SRV Game.cpp:50382-50398

Appends a client index to the dense m_iClientShortCut list used for fast iteration.

- sig: `BOOL CGame::bAddClientShortCut(int iClientH)`
- callers: CGame::bAccept
- Korean comments (UTF-8, first 1, untranslated quote): 먼저 동일한 번호가 존재하는지 검색

## CGame::RemoveClientShortCut - SRV Game.cpp:50400-50418

Removes a client index from the shortcut list and compacts it.

- sig: `void CGame::RemoveClientShortCut(int iClientH)`
- callers: CGame::DeleteClient
- Korean comments (UTF-8, first 1, untranslated quote): 빈 여백을 메꾼다.

## CGame::bCheckClientAttackFrequency - SRV Game.cpp:52615-52641

v2.171 speed-hack check: a gap under 500 ms between client times logs 'Speed hack suspect', calls DeleteClient(i,TRUE,TRUE) and returns FALSE; it always returns FALSE (callers ignore the value).

- sig: `BOOL CGame::bCheckClientAttackFrequency(int iClientH, DWORD dwClientTime)`
- callers: CGame::ClientMotionHandler
- callees: DeleteClient, PutLogList

## CGame::bCheckClientMagicFrequency - SRV Game.cpp:52645-52671

v2.171 speed-hack check for casts: gap under 1500 ms -> log + DeleteClient(i,TRUE,TRUE); always returns FALSE. DEAD: never called anywhere ([MAGIC-001](../../issues/magic.md#magic-001-bcheckclientmagicfrequency-is-defined-and-never-called)).

- sig: `BOOL CGame::bCheckClientMagicFrequency(int iClientH, DWORD dwClientTime)`
- callees: DeleteClient, PutLogList

## CGame::bCheckClientMoveFrequency - SRV Game.cpp:52674-52713

v2.171 speed-hack check for moves (tolerant of Tab-key bursts): gap under 250 ms -> log + DeleteClient(i,TRUE,TRUE); always returns FALSE. The move timer resets when m_bIsMoveBlocked or m_bIsAttackModeChange is set.

- sig: `BOOL CGame::bCheckClientMoveFrequency(int iClientH, DWORD dwClientTime)`
- callers: CGame::ClientMotionHandler
- callees: DeleteClient, PutLogList
- Korean comments (UTF-8, first 1, untranslated quote): v2.172 Tab키 연타로 인한 튕김을 막기 위함

## OnAccept - SRV Wmain.cpp:337-340

WM_USER_ACCEPT -> CGame::bAccept.

- sig: `void OnAccept()`
- callers: WndProc
- callees: bAccept

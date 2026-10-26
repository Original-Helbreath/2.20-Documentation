# Guilds and parties

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
`_bItemLog` callees here sit in inactive DEF_TAIWANLOG blocks (e.g. ResponseCreateNewGuildHandler 16590, JoinGuildApproveHandler 19072, DismissGuildApproveHandler 19131) [V, review G3].
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 29

## CClient::bCreateNewParty - SRV Client.cpp:317-333

Legacy: marks the client as party creator. DEAD: its only caller, CreateNewPartyHandler, is itself uncalled; "unused since v2.06" is [I] ([GSO-028](../../issues/game-server-other.md#gso-028-the-old-party-create-path-is-dead)).

- sig: `BOOL CClient::bCreateNewParty()`
- callers: CGame::CreateNewPartyHandler

## CGame::ResponseCreateNewGuildHandler - SRV Game.cpp:16547-16630

Log-server reply to guild creation; sets rank 0 and notifies. Re-checks level (100, or 20 without DEF_CHINESEGUILDLIMIT) and Charisma >= 20 for the named player, deducts the 1,000,000 gold (16584-16586), and silently logs "non-existing player" if the check no longer passes (16563-16568, 16628).

- sig: `void CGame::ResponseCreateNewGuildHandler(char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: DeleteClient, PutLogList, SetItemCount, _bItemLog, dwGetItemCount, iCalcTotalWeight, iSendMsg
- messages: MSGID_RESPONSE_CREATENEWGUILD
- types: DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT
- switches: DEF_CHINESEGUILDLIMIT, DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 로그 서버로부터 길드 생성 요청에 대한 응답데이터가 도착했다. / 이름이 일치하는 클라이언트를 찾는다. / 클라이언트의 길드 생성 요구가 성공하였다.

## CGame::RequestCreateNewGuildHandler - SRV Game.cpp:16632-16717

Guild creation request. Under DEF_CHINESEGUILDLIMIT it needs level 100, Charisma 20, 1,000,000 gold, a non-neutral side, not hunter mode and not the PK town state (16669-16673); the default build needs level 20 and Charisma 20 (16676). Returns silently (no reject message) when m_iGuildRank != -1 or during crusade (16642, 16661-16665). The gold is not taken here; it is deducted in ResponseCreateNewGuildHandler.

- sig: `void CGame::RequestCreateNewGuildHandler(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: DeleteClient, PutLogList, bSendMsgToLS, dwGetItemCount, iSendMsg
- messages: MSGID_REQUEST_CREATENEWGUILD, MSGID_RESPONSE_CREATENEWGUILD
- types: DEF_MSGTYPE_REJECT
- switches: DEF_CHINESEGUILDLIMIT
- Korean comments (UTF-8, first 6, untranslated quote): v2.19 2002-11-28 [중국] 길드 구성 조건을 레벨 100 과 100만 골드 / 이 캐릭터는 이미 길드에 가입하여 있으므로 길드를 만들 수 없다. / 2002-11-15 사냥꾼 모드 추가 (길드신청시 사냥꾼 모드에서는 안된다.)

## CGame::RequestDisbandGuildHandler - SRV Game.cpp:16720-16749

Guildmaster disband request to the Log server.

- sig: `void CGame::RequestDisbandGuildHandler(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: PutLogList, _bItemLog, bSendMsgToLS
- messages: MSGID_REQUEST_DISBANDGUILD
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 2, untranslated quote): 길드마스터가 아니거나 길드의 이름이 다르므로 길드해산의 권한이 없다. / 길드 해산 메시지를 로그서버로 전송한다.

## CGame::ResponseDisbandGuildHandler - SRV Game.cpp:16751-16818

Log-server reply: clears guild on all members.

- sig: `void CGame::ResponseDisbandGuildHandler(char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: DeleteClient, PutLogList, SendEventToNearClient_TypeB, SendGuildMsg, iSendMsg
- messages: MSGID_EVENT_COMMON, MSGID_RESPONSE_DISBANDGUILD
- notifies: DEF_NOTIFY_GUILDDISBANDED
- types: DEF_COMMONTYPE_CLEARGUILDNAME, DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT
- Korean comments (UTF-8, first 6, untranslated quote): 로그 서버로부터 길드 해산 요청에 대한 응답데이터가 도착했다. / 이름이 일치하는 클라이언트를 찾는다. / 클라이언트의 길드 해산 요구가 성공하였다.

## CGame::JoinGuildApproveHandler - SRV Game.cpp:19039-19090

Join approved; updates Log server (NEWGUILDSMAN). The function does not check that the caller is a guildmaster or has a guild: it only requires same side (19053), then copies the caller's guild name/GUID/location onto the named online player and sets rank DEF_GUILDSTARTRANK 12 (19056-19066). Any further restriction is client-side only ([GSO-029](../../issues/game-server-other.md#gso-029-guild-join-approval-does-not-check-the-caller-is-a-guildmaster)).

- sig: `void CGame::JoinGuildApproveHandler(int iClientH, char * pName)`
- callers: CGame::ClientCommonHandler
- callees: SendEventToNearClient_TypeA, SendEventToNearClient_TypeB, SendGuildMsg, SendNotifyMsg, _bItemLog, bSendMsgToLS
- messages: MSGID_EVENT_COMMON, MSGID_EVENT_MOTION, MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN
- notifies: DEF_NOTIFY_NEWGUILDSMAN
- types: DEF_COMMONTYPE_CLEARGUILDNAME, DEF_COMMONTYPE_JOINGUILDAPPROVE, DEF_OBJECTNULLACTION
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): pName을 갖는 클라이언트의 iClientH 길드에 대한 가입요구가 성공하였다. / pName의 이름을 갖는 클라이언트 구조체를 검색한다. / v1.4 소속 마을이 달라도 무시된다.

## CGame::JoinGuildRejectHandler - SRV Game.cpp:19092-19111

Join rejected.

- sig: `void CGame::JoinGuildRejectHandler(int iClientH, char * pName)`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg
- types: DEF_COMMONTYPE_JOINGUILDREJECT
- Korean comments (UTF-8, first 4, untranslated quote): pName을 갖는 클라이언트의 iClientH 길드에 대한 가입 요구가 실패 하였다. / pName의 이름을 갖는 클라이언트 구조체를 검색한다. / 가입 신청자에게 가입이 실패했음을 알리는 메시지를 보내준다.

## CGame::DismissGuildApproveHandler - SRV Game.cpp:19113-19154

Leave approved; updates Log server (DELGUILDSMAN). Removes the named online player from their guild with no check that they belong to the caller's guild (19124-19142); any restriction is client-side only ([GSO-030](../../issues/game-server-other.md#gso-030-guild-leave-approval-does-not-check-membership-of-the-callers-guild)).

- sig: `void CGame::DismissGuildApproveHandler(int iClientH, char * pName)`
- callers: CGame::ClientCommonHandler
- callees: SendEventToNearClient_TypeA, SendEventToNearClient_TypeB, SendGuildMsg, SendNotifyMsg, _bItemLog, bSendMsgToLS
- messages: MSGID_EVENT_COMMON, MSGID_EVENT_MOTION, MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN
- notifies: DEF_NOTIFY_DISMISSGUILDSMAN
- types: DEF_COMMONTYPE_CLEARGUILDNAME, DEF_COMMONTYPE_DISMISSGUILDAPPROVE, DEF_OBJECTNULLACTION
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): pName을 갖는 클라이언트의 iClientH 길드에 대한 탈퇴요구가 성공하였다. / pName의 이름을 갖는 클라이언트 구조체를 검색한다. / 길드 정보 화일에서 탈퇴한 길드원의 이름을 삭제한다.

## CGame::DismissGuildRejectHandler - SRV Game.cpp:19156-19175

Leave rejected.

- sig: `void CGame::DismissGuildRejectHandler(int iClientH, char * pName)`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg
- types: DEF_COMMONTYPE_DISMISSGUILDREJECT
- Korean comments (UTF-8, first 4, untranslated quote): pName을 갖는 클라이언트의 iClientH 길드에 대한 탈퇴 요구가 실패 하였다. / pName의 이름을 갖는 클라이언트 구조체를 검색한다. / 가입 신청자에게 탈퇴가 실패했음을 알리는 메시지를 보내준다.

## CGame::CalculateGuildEffect - SRV Game.cpp:20249-20295

Guild co-op exp share (disabled: early return ### ERROR POINT) ([GSO-031](../../issues/game-server-other.md#gso-031-guild-co-op-exp-share-is-disabled)).

- sig: `void CGame::CalculateGuildEffect(int iVictimH, char cVictimType, short sAttackerH)`
- callees: GetOwner, SendNotifyMsg, bCheckLevelUp, bCheckLimitedUser, iDice
- notifies: DEF_NOTIFY_EXP
- Korean comments (UTF-8, first 6, untranslated quote): ### ERROR POINT! 임시로 막아 논다. / 길드원과의 협동전투에서 경험치를 얻기 위해서는 몬스터를 죽인 길드원의 레벨보다 낮은 길드원이어야 한다. / 1D3의 값이 2일때 몬스터 경험치의 3분의 1을 얻는다.

## CGame::SendGuildMsg - SRV Game.cpp:20419-20495

Sends a guild event to every online member ([GSO-032](../../issues/game-server-other.md#gso-032-sendguildmsg-carries-a-developer-note-about-wrong-pointer-arithmetic)).

- sig: `void CGame::SendGuildMsg(int iClientH, WORD wNotifyMsgType, short sV1, short sV2, char * pString)`
- callers: CGame::DismissGuildApproveHandler, CGame::JoinGuildApproveHandler, CGame::ResponseDisbandGuildHandler, CGame::UserCommand_BanGuildsman
- callees: DeleteClient, iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_DISMISSGUILDSMAN, DEF_NOTIFY_EVENTMSGSTRING, DEF_NOTIFY_GUILDDISBANDED, DEF_NOTIFY_NEWGUILDSMAN
- Korean comments (UTF-8, first 6, untranslated quote): 같은 길드원들에게만 보내는 메시지들 / ### BUG POINT 위치가 잘못되어 포인터 연산이 잘못되었다. / 같은 길드 이름을 갖고 있는 클라이언트를 찾았다.

## CGame::GuildNotifyHandler - SRV Game.cpp:20497-20514

Inter-server guild event (not implemented) ([GSO-033](../../issues/game-server-other.md#gso-033-guild-inter-server-notify-and-the-dismiss-command-are-stubs)).

- sig: `void CGame::GuildNotifyHandler(char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- Korean comments (UTF-8, first 2, untranslated quote): 다른 게임서버로부터 길드 이벤트가 도착했다. / 아직 구현되지 않았다.

## CGame::UserCommand_BanGuildsman - SRV Game.cpp:36724-36800

/ban: guildmaster expels a member.

- sig: `void CGame::UserCommand_BanGuildsman(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendEventToNearClient_TypeA, SendEventToNearClient_TypeB, SendGuildMsg, SendNotifyMsg, _bItemLog, bSendMsgToLS, pGet
- messages: MSGID_EVENT_COMMON, MSGID_EVENT_MOTION, MSGID_REQUEST_UPDATEGUILDINFO_DELGUILDSMAN
- notifies: DEF_NOTIFY_CANNOTBANGUILDMAN, DEF_NOTIFY_DISMISSGUILDSMAN, DEF_NOTIFY_NOGUILDMASTERLEVEL, DEF_NOTIFY_PLAYERNOTONGAME, DEF_NOTIFY_SUCCESSBANGUILDMAN
- types: DEF_COMMONTYPE_BANGUILD, DEF_COMMONTYPE_CLEARGUILDNAME, DEF_OBJECTNULLACTION
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 길드 마스터가 아니어서 이 기능을 사용할수 없다. / token이 길드를 탈퇴시킬 사람 / 목표 캐릭터를 찾았다. 강제로 길드를 강퇴 시킨다.

## CGame::UserCommand_DissmissGuild - SRV Game.cpp:36804-36807

/dissmiss stub (empty) ([GSO-033](../../issues/game-server-other.md#gso-033-guild-inter-server-notify-and-the-dismiss-command-are-stubs)).

- sig: `void CGame::UserCommand_DissmissGuild(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler

## CGame::CreateNewPartyHandler - SRV Game.cpp:46728-46736

Old party create reply. DEAD: the only call site is commented out (Game.cpp:15439-15441) ([GSO-028](../../issues/game-server-other.md#gso-028-the-old-party-create-path-is-dead)).

- sig: `void CGame::CreateNewPartyHandler(int iClientH)`
- callees: SendNotifyMsg, bCreateNewParty
- notifies: DEF_NOTIFY_RESPONSE_CREATENEWPARTY

## CGame::JoinPartyHandler - SRV Game.cpp:46739-46843

Party requests by `iV1`: 0 -> RequestDeletePartyHandler (leave); 1 -> local invite: the target gets DEF_NOTIFY_QUERY_JOINPARTY on this server with no gate message (46774-46816); 2 -> member-list request via the gate (46822-46840). Reject conditions for iV1 == 1 (all send DEF_NOTIFY_PARTY 7): caller already in a party (46761), target in combat/appearance nibble set, different side, target PROCESSING (46777-46796).

- sig: `void CGame::JoinPartyHandler(int iClientH, int iV1, char *pMemberName)`
- callers: CGame::ClientCommonHandler
- callees: PutLogList, RequestDeletePartyHandler, SendMsgToGateServer, SendNotifyMsg
- messages: MSGID_PARTYOPERATION
- notifies: DEF_NOTIFY_PARTY, DEF_NOTIFY_QUERY_JOINPARTY
- Korean comments (UTF-8, first 6, untranslated quote): 파티 탈퇴 신청 / 파티 가입 신청 / 이미 파티에 가입해 있다면 파티 가입 신청을 할 수 없다.

## CGame::RequestCreatePartyHandler - SRV Game.cpp:50929-50968

Asks the gate for a new party id.

- sig: `void CGame::RequestCreatePartyHandler(int iClientH)`
- callers: CGame::ChatMsgHandler, CGame::RequestAcceptJoinPartyHandler
- callees: PutLogList, SendMsgToGateServer
- messages: MSGID_PARTYOPERATION
- Korean comments (UTF-8, first 2, untranslated quote): 파티 상태가 이미 존재하면 파티를 만들 수 없다. / Gate Server에 파티 생성후 PartyID를 알려줄 것을 요청한다.

## CGame::PartyOperationResultHandler - SRV Game.cpp:50972-51105

Gate party replies (create/join/dismiss/delete/info). Bug: the case 3 bounds check is `if ((iClientH < 0) && (iClientH > DEF_MAXCLIENTS)) return;` (51021); with `&&` it is never true, so an out-of-range client id from the gate indexes m_pClientList unchecked ([GSO-034](../../issues/game-server-other.md#gso-034-partyoperationresulthandler-bounds-check-uses--so-it-never-fires)).

- sig: `void CGame::PartyOperationResultHandler(char *pData)`
- callers: CGame::MsgProcess
- callees: PartyOperationResult_Create, PartyOperationResult_Delete, PartyOperationResult_Dismiss, PartyOperationResult_Info, PartyOperationResult_Join, PutLogList, SendNotifyMsg
- notifies: DEF_NOTIFY_PARTY
- Korean comments (UTF-8, first 6, untranslated quote): 응답 종류에 따라 처리 / 파티 생성 요청에 대한 응답 / 파티가 해산되었다.

## CGame::PartyOperationResult_Create - SRV Game.cpp:51107-51173

Registers a new party locally.

- sig: `void CGame::PartyOperationResult_Create(int iClientH, char *pName, int iResult, int iPartyID)`
- callers: CGame::PartyOperationResultHandler
- callees: PutLogList, SendMsgToGateServer, SendNotifyMsg
- messages: MSGID_PARTYOPERATION
- notifies: DEF_NOTIFY_PARTY
- Korean comments (UTF-8, first 6, untranslated quote): 파티 생성 실패 / 파티 생성 성공 / 게임 서버의 파티 리스트에 등록.

## CGame::PartyOperationResult_Join - SRV Game.cpp:51175-51224

Adds a member locally.

- sig: `void CGame::PartyOperationResult_Join(int iClientH, char *pName, int iResult, int iPartyID)`
- callers: CGame::PartyOperationResultHandler
- callees: PutLogList, SendNotifyMsg
- notifies: DEF_NOTIFY_PARTY
- Korean comments (UTF-8, first 4, untranslated quote): 파티 조인 실패 / 파티 조인 성공 / 게임 서버의 파티 리스트에 등록.

## CGame::PartyOperationResult_Dismiss - SRV Game.cpp:51226-51311

Removes a member locally.

- sig: `void CGame::PartyOperationResult_Dismiss(int iClientH, char *pName, int iResult, int iPartyID)`
- callers: CGame::PartyOperationResultHandler
- callees: PutLogList, SendNotifyMsg
- notifies: DEF_NOTIFY_PARTY
- Korean comments (UTF-8, first 6, untranslated quote): iClientH는 이미 사라지고 없는 캐릭터일수도 있다. / 파티 탈퇴 실패 ? 이런 일이? / 파티 탈퇴 성공

## CGame::PartyOperationResult_Delete - SRV Game.cpp:51313-51334

Deletes a party locally.

- sig: `void CGame::PartyOperationResult_Delete(int iPartyID)`
- callers: CGame::PartyOperationResultHandler
- callees: PutLogList, SendNotifyMsg
- notifies: DEF_NOTIFY_PARTY
- Korean comments (UTF-8, first 1, untranslated quote): 파티 해산됨

## CGame::RequestJoinPartyHandler - SRV Game.cpp:51338-51395

`/joinparty <name>`: the named player only has to be a confirmed (DEF_PARTYSTATUS_CONFIRM) member of some party; the function sends op 3 (add member) with that party id straight to the gate server (51365-51389). There is no master check and no accept prompt in this function; the accept prompt (DEF_NOTIFY_QUERY_JOINPARTY) is in JoinPartyHandler iV1 == 1 (46805-46808). If the name is not on this server the caller gets DEF_NOTIFY_PLAYERNOTONGAME ([GSO-035](../../issues/game-server-other.md#gso-035-joinparty-does-not-require-the-target-to-be-the-party-master)).

- sig: `void CGame::RequestJoinPartyHandler(int iClientH, char *pData, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendMsgToGateServer, SendNotifyMsg, pGet
- messages: MSGID_PARTYOPERATION
- notifies: DEF_NOTIFY_PLAYERNOTONGAME
- Korean comments (UTF-8, first 5, untranslated quote): 파티 가입 여부를 묻고 있다. / 게이트 서버로 파티 멤버 가입을 신청한다. / 파티에 가입하고자 한 캐릭터가 파티 마스터가 아니다.

## CGame::RequestDismissPartyHandler - SRV Game.cpp:51399-51427

Leave party.

- sig: `void CGame::RequestDismissPartyHandler(int iClientH)`
- callers: CGame::ChatMsgHandler, CGame::RequestAcceptJoinPartyHandler
- callees: SendMsgToGateServer
- messages: MSGID_PARTYOPERATION
- Korean comments (UTF-8, first 1, untranslated quote): 멤버 제거 요청

## CGame::GetPartyInfoHandler - SRV Game.cpp:51430-51456

Requests party info.

- sig: `void CGame::GetPartyInfoHandler(int iClientH)`
- callers: CGame::ChatMsgHandler
- callees: SendMsgToGateServer
- messages: MSGID_PARTYOPERATION
- Korean comments (UTF-8, first 1, untranslated quote): 파티 정보 요청

## CGame::PartyOperationResult_Info - SRV Game.cpp:51459-51466

Sends the party member list.

- sig: `void CGame::PartyOperationResult_Info(int iClientH, char * pName, int iTotal, char *pNameList)`
- callers: CGame::PartyOperationResultHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_PARTY

## CGame::RequestDeletePartyHandler - SRV Game.cpp:51468-51496

Dissolve party.

- sig: `void CGame::RequestDeletePartyHandler(int iClientH)`
- callers: CGame::ChatMsgHandler, CGame::JoinPartyHandler, CGame::RequestCivilRightHandler
- callees: SendMsgToGateServer
- messages: MSGID_PARTYOPERATION
- Korean comments (UTF-8, first 2, untranslated quote): 멤버 제거 요청 / 상태 변환

## CGame::RequestAcceptJoinPartyHandler - SRV Game.cpp:51498-51634

Master answers a join request.

- sig: `void CGame::RequestAcceptJoinPartyHandler(int iClientH, int iResult)`
- callers: CGame::ClientCommonHandler
- callees: PutLogList, RequestCreatePartyHandler, RequestDismissPartyHandler, SendMsgToGateServer, SendNotifyMsg
- messages: MSGID_PARTYOPERATION
- notifies: DEF_NOTIFY_PARTY
- Korean comments (UTF-8, first 6, untranslated quote): 파티 가입 신청 거부되었음을 알려준다. / 가입을 신청했던 플레이어가 서버상에 없다. / 가입을 신청했던 플레이어가 서버상에 없다.

## CGame::RequestGuildNameHandler - SRV Game.cpp:52597-52612

Returns an object's guild name and rank (hunter-mode format).

- sig: `void CGame::RequestGuildNameHandler(int iClientH, int iObjectID, int iIndex)`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_REQGUILDNAMEANSWER
- Korean comments (UTF-8, first 2, untranslated quote): 요청 받은 Object가 없다. / 2002-11-14 사냥꾼 모드 추가 (길드 이름 보내주는 형식이 바뀌었다.)

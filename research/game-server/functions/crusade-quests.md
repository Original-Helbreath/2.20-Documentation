# Crusade, occupy flags, fightzone, energy sphere, quests

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
`cfg:` means data loaded from that config (here often pushed by the Log server), not a literal file open in the body [V, review G1].
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 69

## CGame::bReadCrusadeStructureConfigFile - SRV Game.cpp:5354-5484

Parses Crusade.cfg into m_stCrusadeStructures[] (map name, NPC type, x, y) for war-time structure spawns.

- sig: `BOOL CGame::bReadCrusadeStructureConfigFile(char * cFn)`
- callers: CGame::OnStartGameSignal
- callees: CStrTok, PutLogList, _bGetIsStringIsNumber, pGet
- cfg: Crusade.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 게임서버의 초기화 파일을 읽을 수 없다. / 12-16 성후니 변경 / 스트럭쳐 번호

## CGame::FightzoneReserveHandler - SRV Game.cpp:27414-27528

Fightzone reservation by any player (no guild check) in 2-hour slots. Costs 1500 gold (27458-27479); rejected within 5 min of the slot end (27429), when the zone is already taken, when the player already holds a reservation, and by a parity rule `(day + side + zone) % 2` (27448). The `_bItemLog` call (27490) sits in an inactive DEF_TAIWANLOG block.

- sig: `void CGame::FightzoneReserveHandler(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: DeleteClient, PutLogFileList, PutLogList, SetItemCount, _bItemLog, dwGetItemCount, iCalcTotalWeight, iSendMsg
- messages: MSGID_RESPONSE_FIGHTZONE_RESERVE
- types: DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 예약 가능한 시간 : 두시간 간격으로 예약이 가능하며 사용완료 5분전에는 예약이 불가능하다. / 예약을 원하는 사투장 번호를 받는다. / 잘못된 fightzone 번호를 걸러낸다.

## CGame::FightzoneReserveProcessor - SRV Game.cpp:37440-37460

Every 7,200,000 ms counted from server start (3786), not on slot boundaries, clears reservations; slots GM-reserved as -1 are skipped (37450). Players holding a reservation get DEF_NOTIFY_FIGHTZONERESERVE -2.

- sig: `void CGame::FightzoneReserveProcessor()`
- callers: CGame::OnTimer
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_FIGHTZONERESERVE
- Korean comments (UTF-8, first 5, untranslated quote): v2.02 로그 삭제 / 예약된 사투장들을 초기화 시킨다.  10개의 사투장의 입장을 허락한다. / 운영자가 예약한 사투장은 예약이 불가능하다.

## CGame::NpcTalkHandler - SRV Game.cpp:39153-39239

NPC talk: only iWho 4 (city hall) and 21 (guard) call quest logic (39169, 39185); sends DEF_NOTIFY_NPCTALK. The five stub functions below (GuildHall, GShop, BSmith, WHouse, WTower) are never called ([GSO-022](../../issues/game-server-other.md#gso-022-five-npc-talk-stubs-are-never-called)).

- sig: `void CGame::NpcTalkHandler(int iClientH, int iWho)`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg, _iTalkToNpcResult_Cityhall, _iTalkToNpcResult_Guard
- notifies: DEF_NOTIFY_NPCTALK
- cfg: Item.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 길드홀 / 일반 상점 / 무기 상점

## CGame::__bSetOccupyFlag - SRV Game.cpp:39499-39890

DEAD in effect: after the comment "no flag in crusade mode" there is an unconditional `return FALSE;` (39508); the rest (39510-39890, which would plant a flag on Middleland and update tile occupy values) is unreachable. Both callers (_bDepleteDestTypeItemUseEffect 39427, _bDecodeOccupyFlagSaveFileContents 40421) always get FALSE, so no flag can be planted in this build ([GSO-020](../../issues/game-server-other.md#gso-020-occupy-flags-can-never-be-planted)).

- sig: `BOOL CGame::__bSetOccupyFlag(char cMapIndex, int dX, int dY, int iSide, int iEKNum, int iClientH, BOOL bAdminFlag)`
- callers: CGame::_bDecodeOccupyFlagSaveFileContents, CGame::_bDepleteDestTypeItemUseEffect
- callees: SendEventToNearClient_TypeB, SetDynamicObject, _CheckStrategicPointOccupyStatus, iAddDynamicObjectList, iRegisterOccupyFlag
- messages: MSGID_DYNAMICOBJECT
- types: DEF_MSGTYPE_REJECT
- Korean comments (UTF-8, first 6, untranslated quote): 크루세이드 모드에서는 깃발 설치 불가. / 미들랜드가 아닌지역을 점령할 수는 없다. / 꽂고자 하는 자리에 깃발이 없어야 한다.

## CGame::GetOccupyFlagHandler - SRV Game.cpp:39978-40140

Needs EK >= 3 and a non-neutral side; consumes up to 12 EK and stores that count in the flag item (39988, 40013-40023). Because __bSetOccupyFlag always fails (see above), the flag item cannot be planted ([GSO-020](../../issues/game-server-other.md#gso-020-occupy-flags-can-never-be-planted)).

- sig: `void CGame::GetOccupyFlagHandler(int iClientH)`
- callers: CGame::ClientCommonHandler
- callees: DeleteClient, PutLogFileList, SendNotifyMsg, _bAddClientItemList, _bInitItemAttr, iCalcTotalWeight, iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_CANNOTCARRYMOREITEM, DEF_NOTIFY_ENEMYKILLS, DEF_NOTIFY_ITEMOBTAINED
- Korean comments (UTF-8, first 6, untranslated quote): ReqPurchaseItemHandler에서 가져온 루틴을 고쳤음. / 구입하고자 하는 아이템이 아이템 리스트상에 없다. 구입이 불가능하다. / 에러 방지용 코드

## CGame::GetFightzoneTicketHandler - SRV Game.cpp:40145-40305

Gives one fightzone ticket per call to the individual player who reserved the arena (no guild logic here or in FightzoneReserveHandler): m_iFightZoneTicketNumber = 50 is set at reservation (27499), each call decrements it (40182) and stamps the item with the date from m_iReserveTime (40184-40192); at 0 it notifies -1.

- sig: `void CGame::GetFightzoneTicketHandler(int iClientH)`
- callers: CGame::AdminOrder_GetFightzoneTicket, CGame::ClientCommonHandler
- callees: DeleteClient, SendNotifyMsg, _bAddClientItemList, _bInitItemAttr, _bItemLog, iCalcTotalWeight, iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_CANNOTCARRYMOREITEM, DEF_NOTIFY_FIGHTZONERESERVE, DEF_NOTIFY_ITEMOBTAINED
- Korean comments (UTF-8, first 6, untranslated quote): 입장권을 다 사용했음을 알린다. / 사투장 번호가 음수면 예약은 했는데 입장권을 다 받은경우 .. / 에러 방지용 코드

## CGame::_iComposeFlagStatusContents - SRV Game.cpp:40307-40337

Serialises flags for saving. Reachable only from the disabled SaveOccupyFlagData (the calls at 4441/4463 are inside a /* */ block), so it never runs ([GSO-021](../../issues/game-server-other.md#gso-021-occupy-flag-persistence-and-removal-code-is-dead)).

- sig: `int CGame::_iComposeFlagStatusContents(char * pData)`
- callers: CGame::SaveOccupyFlagData

## CGame::_bDecodeOccupyFlagSaveFileContents - SRV Game.cpp:40340-40454

Loads saved flags. DEAD: no call site anywhere ([GSO-021](../../issues/game-server-other.md#gso-021-occupy-flag-persistence-and-removal-code-is-dead)).

- sig: `BOOL CGame::_bDecodeOccupyFlagSaveFileContents(char * pData, DWORD dwMsgSize)`
- callees: CStrTok, PutLogList, __bSetOccupyFlag, _bGetIsStringIsNumber, pGet
- Korean comments (UTF-8, first 3, untranslated quote): X 좌표 / Y 좌표 / 데이터를 모두 만들었으므로 깃발을 등록한다. (!!! Master Flag로 처리해야만 설치할 수 있다)

## CGame::GetHeroMantleHandler - SRV Game.cpp:40457-40624

Buys the hero mantle for EKs.

- sig: `void CGame::GetHeroMantleHandler(int iClientH)`
- callers: CGame::ClientCommonHandler
- callees: DeleteClient, PutLogFileList, SendNotifyMsg, _bAddClientItemList, _bInitItemAttr, _bItemLog, iCalcTotalWeight, iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_CANNOTCARRYMOREITEM, DEF_NOTIFY_ENEMYKILLS, DEF_NOTIFY_ITEMOBTAINED
- Korean comments (UTF-8, first 6, untranslated quote): 아레스덴 영웅 망토 / 엘바인 영웅 망토 / ReqPurchaseItemHandler에서 가져온 루틴을 고쳤음.

## CGame::_iTalkToNpcResult_Cityhall - SRV Game.cpp:41351-41475

City hall quest/reward logic.

- sig: `int CGame::_iTalkToNpcResult_Cityhall(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCoun`
- callers: CGame::NpcTalkHandler
- callees: GetExp, SendItemNotifyMsg, SendNotifyMsg, _ClearQuestStatus, __iSearchForQuest, _bAddClientItemList, _bCheckItemReceiveCondition, _bInitItemAttr, iDice
- notifies: DEF_NOTIFY_CANNOTCARRYMOREITEM, DEF_NOTIFY_ITEMOBTAINED, DEF_NOTIFY_QUESTREWARD
- cfg: Item.cfg, Quest.cfg
- Korean comments (UTF-8, first 6, untranslated quote): -1  : 현재 임무 수행중 / -2  : 마을이 다르거나 중립은 임무를 얻을 수 없다. / -3  : 범죄자는 임무를 얻을 수 없다.

## CGame::_iTalkToNpcResult_Guard - SRV Game.cpp:41478-41534

Guard talk responses.

- sig: `int CGame::_iTalkToNpcResult_Guard(int iClientH, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCount, `
- callers: CGame::NpcTalkHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_NPCTALK
- Korean comments (UTF-8, first 6, untranslated quote): 경비병과의 대화. 플레이어의 위치에 적절한 대화 메시지를 디스플레이한다. / v2.181 2002-10-24 서버 속도 향상을 위해 스트링 비교를 줄인다. / 말 건 사람이 아레스덴 소속

## CGame::_bDecodeQuestConfigFileContents - SRV Game.cpp:41537-41910

Parses Quest.cfg pushed by the Log server (MSGID_QUESTCONFIGURATIONCONTENTS, Game.cpp:15226); the function names no file.

- sig: `BOOL CGame::_bDecodeQuestConfigFileContents(char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: CStrTok, PutLogList, _bGetIsStringIsNumber, pGet
- cfg: Quest.cfg
- Korean comments (UTF-8, first 4, untranslated quote): 퀘스트 번호 / 이미 할당된 번호가 있다. 에러이다. / 퀘스트 사이드

## CGame::__iSearchForQuest - SRV Game.cpp:41913-41974

Picks one quest at random (1dN) from those matching: giver NPC (iWho), the player's side, level range, minimum contribution, maximum contribution (ContributionLimit), required skill level, and crusade gating (during a crusade only assign-type 1 quests; outside it assign-type 1 is never chosen). Returns -1 if none; the reward slot is 1d3 [V, review R2-14] (filters 41927-41944, pick 41955, reward 41958).

- sig: `int CGame::__iSearchForQuest(int iClientH, int iWho, int * pQuestType, int * pMode, int * pRewardType, int * pRewardAmount, int * pContribution, char * pTargetName, int * pTargetType, int * pTargetCou`
- callers: CGame::_iTalkToNpcResult_Cityhall
- callees: iDice
- cfg: Quest.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 적합한 미션 배열 초기화. / 고려해야 할 스킬이 있다. / 크루세이드 모드일때는 할당 타입이 1인 퀘스트만 부여된다.

## CGame::QuestAcceptedHandler - SRV Game.cpp:41978-42004

Player accepts the quest. Assign type 1 with quest type 10 is a one-shot teleport: the quest status is cleared and the player is teleported to the quest map and x/y (41984-41989). Otherwise it records the quest and ID, resets the count, then calls _CheckQuestEnvironment and _SendQuestContents [V, review R2-14].

- sig: `void CGame::QuestAcceptedHandler(int iClientH)`
- callers: CGame::ClientCommonHandler
- callees: RequestTeleportHandler, _CheckQuestEnvironment, _ClearQuestStatus, _SendQuestContents
- cfg: Quest.cfg
- Korean comments (UTF-8, first 3, untranslated quote): 전면전용 퀘스트이다. / 텔레포트용 1회성 퀘스트. 텔레포트하고 나면 끝이다. 더이상의 퀘스트 여부가 없음. / 플레이어가 퀘스트를 수락했다. 퀘스트 번호, ID값 할당.

## CGame::_SendQuestContents - SRV Game.cpp:42007-42039

Sends the current quest.

- sig: `void CGame::_SendQuestContents(int iClientH)`
- callers: CGame::QuestAcceptedHandler, CGame::RequestInitDataHandler, CGame::RequestTeleportHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_QUESTCONTENTS
- cfg: Quest.cfg
- Korean comments (UTF-8, first 2, untranslated quote): 할당된 Quest가 없다. / 할당된 Quest 내용을 보내준다.

## CGame::_CheckQuestEnvironment - SRV Game.cpp:42041-42086

Validates the quest still exists and sets location flags. Quest indexes 35-40 (deleted quests) and a quest whose ID no longer matches are aborted (QUESTABORTED, quest cleared); only types MONSTERHUNT and GOPLACE set the map-match location flag (42052-42058, 42076-42077) [V, review R2-14].

- sig: `void CGame::_CheckQuestEnvironment(int iClientH)`
- callers: CGame::QuestAcceptedHandler, CGame::RequestInitDataHandler, CGame::RequestTeleportHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_QUESTABORTED
- cfg: Quest.cfg
- Korean comments (UTF-8, first 3, untranslated quote): v2.181 2002-11-4  Quest 관련 버그 수정 (삭제된 퀘스트를 가지고 있는 유저의 퀘스트를 초기화 한다.) / 퀘스트 ID 값이 다르다. 할당받았던 그 퀘스트가 아니다. 퀘스트는 취소된다. / 몬스터 사냥의 경우 매번 몬스터 사냥시 맵 이름을 비교하는 것은 비효율적이므로 미리 맵 이름을 비교, 플래그를 설정해 놓는다.

## CGame::_bCheckIsQuestCompleted - SRV Game.cpp:42088-42126

Checks completion and notifies (QUESTCOMPLETED). Only MONSTERHUNT (location flag set and count >= max) and GOPLACE (location flag set and within range of x/y) can complete here; every other quest type never completes in this function (42100, 42110) [V, review R2-14] ([GSO-027](../../issues/game-server-other.md#gso-027-only-monsterhunt-and-goplace-quests-can-complete)).

- sig: `BOOL CGame::_bCheckIsQuestCompleted(int iClientH)`
- callers: CGame::NpcKilledHandler, CGame::iClientMotion_Move_Handler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_QUESTCOMPLETED
- cfg: Quest.cfg
- Korean comments (UTF-8, first 5, untranslated quote): 퀘스트가 종료되었는지 판단한 후 메시지 처리. / 몬스터 사냥 퀘스트가 완료되었다. 완료되었으면 의뢰인에게 돌아가 대화를 하면 포상받는다. / 맵은 맞고

## CGame::_iTalkToNpcResult_GuildHall - SRV Game.cpp:42128-42131

Stub. DEAD in effect: returns -4 and has no call site.

- sig: `int CGame::_iTalkToNpcResult_GuildHall(int iClientH, int *pQuestType, int *pMode, int *pRewardType, int *pRewardAmount, int *pContribution, char *pTargetName, int *pTargetType, int *pTargetCount, int `

## CGame::_iTalkToNpcResult_GShop - SRV Game.cpp:42133-42136

Stub. DEAD in effect: returns -4 and has no call site.

- sig: `int CGame::_iTalkToNpcResult_GShop(int iClientH, int *pQuestType, int *pMode, int *pRewardType, int *pRewardAmount, int *pContribution, char *pTargetName, int *pTargetType, int *pTargetCount, int *pX,`

## CGame::_iTalkToNpcResult_BSmith - SRV Game.cpp:42138-42141

Stub. DEAD in effect: returns -4 and has no call site.

- sig: `int CGame::_iTalkToNpcResult_BSmith(int iClientH, int *pQuestType, int *pMode, int *pRewardType, int *pRewardAmount, int *pContribution, char *pTargetName, int *pTargetType, int *pTargetCount, int *pX`

## CGame::_iTalkToNpcResult_WHouse - SRV Game.cpp:42143-42146

Stub. DEAD in effect: returns -4 and has no call site.

- sig: `int CGame::_iTalkToNpcResult_WHouse(int iClientH, int *pQuestType, int *pMode, int *pRewardType, int *pRewardAmount, int *pContribution, char *pTargetName, int *pTargetType, int *pTargetCount, int *pX`

## CGame::_iTalkToNpcResult_WTower - SRV Game.cpp:42148-42151

Stub. DEAD in effect: returns -4 and has no call site.

- sig: `int CGame::_iTalkToNpcResult_WTower(int iClientH, int *pQuestType, int *pMode, int *pRewardType, int *pRewardAmount, int *pContribution, char *pTargetName, int *pTargetType, int *pTargetCount, int *pX`

## CGame::_ClearQuestStatus - SRV Game.cpp:42317-42326

Clears quest fields.

- sig: `void CGame::_ClearQuestStatus(int iClientH)`
- callers: CGame::CancelQuestHandler, CGame::QuestAcceptedHandler, CGame::_iTalkToNpcResult_Cityhall

## CGame::_CheckStrategicPointOccupyStatus - SRV Game.cpp:42371-42391

Computes the strategic status from strategic points.

- sig: `void CGame::_CheckStrategicPointOccupyStatus(char cMapIndex)`
- callers: CGame::__bSetOccupyFlag
- Korean comments (UTF-8, first 1, untranslated quote): 전략적 지점의 가중치를 곱해서 더한다.

## CGame::_DeleteRandomOccupyFlag - SRV Game.cpp:46637-46725

Removes a random flag (on loss). DEAD: no call site anywhere ([GSO-021](../../issues/game-server-other.md#gso-021-occupy-flag-persistence-and-removal-code-is-dead)).

- sig: `void CGame::_DeleteRandomOccupyFlag(int iMapIndex)`
- callees: PutLogList, SendEventToNearClient_TypeB, SetDynamicObject, iDice
- messages: MSGID_DYNAMICOBJECT
- types: DEF_MSGTYPE_REJECT
- Korean comments (UTF-8, first 6, untranslated quote): 총 깃발 갯수를 구한다. / 랜덤하게 하나 정한다. / m_pMapList[iMapIndex]->m_pOccupyFlag[i] : 이 깃발을 없앤다.

## CGame::bCheckEnergySphereDestination - SRV Game.cpp:46846-46991

Sphere pushed to a goal: awards the side and notifies.

- sig: `BOOL CGame::bCheckEnergySphereDestination(int iNpcH, short sAttackerH, char cAttackerType)`
- callers: CGame::Effect_Damage_Spot, CGame::iCalculateAttackEffect
- callees: EnergySphereProcessor, PutLogFileList, SendNotifyMsg
- notifies: DEF_NOTIFY_ENERGYSPHEREGOALIN
- Korean comments (UTF-8, first 6, untranslated quote): 사투장의 에너지 스피어 / 아레스덴 목표지점에 도착했다. / 현재 에너지 스피어 골 인덱스 저장

## CGame::EnergySphereProcessor - SRV Game.cpp:46993-47110

Spawns an energy sphere. The automatic path acts only on Middleland (m_iMiddlelandMapIndex), needs 1d2000 == 123 per 3 s, a world-wide m_iTotalGameServerClients >= 500 (pushed by the gate, 14942) and no sphere goal already pending (47000-47008). The map-by-id path is only the admin `/energysphere` command (level >= 2).

- sig: `void CGame::EnergySphereProcessor(BOOL bIsAdminCreate, int iClientH)`
- callers: CGame::ChatMsgHandler, CGame::OnTimer, CGame::bCheckEnergySphereDestination
- callees: PutLogFileList, PutLogList, SendNotifyMsg, SetNamingValueEmpty, bCreateNewNpc, iDice, iGetEmptyNamingValue
- notifies: DEF_NOTIFY_ENERGYSPHERECREATED
- Korean comments (UTF-8, first 6, untranslated quote): 에너지 스피어가 만들어질 확률은 3초마다 2000분의 1 / 에너지 스피어는 총 사용자 500명 이상일때만 만들어 진다. / 만약 이미 에너지 스피어의 골이 할당되어 있으면 만들면 안된다.

## CGame::CancelQuestHandler - SRV Game.cpp:47163-47171

Cancels the quest.

- sig: `void CGame::CancelQuestHandler(int iClientH)`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg, _ClearQuestStatus
- notifies: DEF_NOTIFY_QUESTABORTED
- Korean comments (UTF-8, first 2, untranslated quote): 퀘스트 상태를 클리어한다. / 퀘스트가 취소되었음을 알려준다.

## CGame::__bSetConstructionKit - SRV Game.cpp:47637-47708

Engineers place a crusade structure (limit per map).

- sig: `BOOL CGame::__bSetConstructionKit(int iMapIndex, int dX, int dY, int iType, int iTimeCost, int iClientH)`
- callers: CGame::_bDepleteDestTypeItemUseEffect
- callees: GetOwner, PutLogList, SendNotifyMsg, SetNamingValueEmpty, bCreateNewNpc, iGetEmptyNamingValue
- notifies: DEF_NOTIFY_NOMORECRUSADESTRUCTURE
- Korean comments (UTF-8, first 6, untranslated quote): 크루세이드 모드가 아니거나 건설한 사람이 공병이 아니면 무시. / 해당 맵에 크루세이드 건물 개수 제한에 걸리지 않는다면 / 해당 위치에 건축물 NPC를 생성.

## CGame::_bNpcBehavior_ManaCollector - SRV Game.cpp:47711-47781

Mana collector structure: regenerates MP of friendly players nearby, and also pulls up to 3 per call from adjacent Mana Stones (type 42) into m_iCollectedMana[side] (47753-47770).

- sig: `BOOL CGame::_bNpcBehavior_ManaCollector(int iNpcH)`
- callers: CGame::NpcBehavior_Stop
- callees: GetOwner, SendNotifyMsg, iDice
- notifies: DEF_NOTIFY_MP
- Korean comments (UTF-8, first 6, untranslated quote): NPC 주변에 있는 캐릭터들의 마나를 올려준다. / 같은 편인 경우 마나를 올려준다. / Mana Point는 1D(Magic)씩 올라간다.

## CGame::_bNpcBehavior_Detector - SRV Game.cpp:47784-47836

Detector structure: reveals invisible enemies nearby.

- sig: `BOOL CGame::_bNpcBehavior_Detector(int iNpcH)`
- callers: CGame::NpcBehavior_Stop
- callees: GetOwner, SetInvisibilityFlag
- Korean comments (UTF-8, first 2, untranslated quote): NPC 주변에 있는 적 캐릭터를 탐지해 경보를 울린다. / 적이다. 투명상태라면 투명을 푼다.

## CGame::CheckCrusadeResultCalculation - SRV Game.cpp:47839-47909

After crusade: contribution is capped at 300000 (not DEF_MAXWARCONTRIBUTION 500000) (47846). Draw: exp stock += contribution/6. Win and loss both first add a level bonus (level x200 up to 80, x100 up to 100, else x30), then win: exp stock += contribution x1.2, loss: += contribution/5 (47862-47889). The loss-case log line writes /10 (47893), which does not match the /5 award ([GSO-023](../../issues/game-server-other.md#gso-023-crusade-result-loss-log-writes-10-but-the-award-is-5)).

- sig: `void CGame::CheckCrusadeResultCalculation(int iClientH)`
- callers: CGame::CheckClientResponseTime
- callees: SendNotifyMsg, _bCrusadeLog
- notifies: DEF_NOTIFY_CRUSADE
- Korean comments (UTF-8, first 6, untranslated quote): 전면전이 종료되었고 전쟁 공헌도가 0이 아니라면 해당하는 처리를 해 주어야 한다. / v2.15 최대 전쟁 공헌도 경험치 감소 / 전쟁에서 비겼다. 양쪽 모두에게 원래 경험치 제공

## CGame::LocalStartCrusadeMode - SRV Game.cpp:47912-47946

Starts crusade on this server: GUID file, duties reset, strike points.

- sig: `void CGame::LocalStartCrusadeMode(DWORD dwCrusadeGUID)`
- callers: CGame::GlobalStartCrusadeMode, CGame::OnKeyUp, CGame::ServerStockMsgHandler
- callees: CreateCrusadeStructures, PutLogList, RestoreStrikePoints, SendNotifyMsg, _CreateCrusadeGUID, _bCrusadeLog
- notifies: DEF_NOTIFY_CRUSADE
- Korean comments (UTF-8, first 5, untranslated quote): 크루세이드 GUID 파일을 만든다. / 모든 클라이언트에게 전면전 모드가 시작되었음을 알려준다. 맡은 임무 클리어 후 통보함. / 스트라이크 포인트 HP 초기화.

## CGame::LocalEndCrusadeMode - SRV Game.cpp:47948-47988

Ends crusade: removes structures and units, sets winner, recalls civilians.

- sig: `void CGame::LocalEndCrusadeMode(int iWinnerSide)`
- callers: CGame::CalcMeteorStrikeEffectHandler, CGame::ManualEndCrusadeMode, CGame::ServerStockMsgHandler
- callees: KillCrusadeObjects, PutLogList, RecallHunterPlayer, RemoveCrusadeStructures, SendNotifyMsg, _CreateCrusadeGUID, _bCrusadeLog
- notifies: DEF_NOTIFY_CRUSADE
- Korean comments (UTF-8, first 6, untranslated quote): 설치되었던 크루세이드 건축물 제거. / 전쟁용 유닛 등 제거 / 이긴쪽 사이드를 입력.

## CGame::SaveOccupyFlagData - SRV Game.cpp:47991-48016

DEAD in effect: the first statement after the declarations is an unconditional `return;` (Game.cpp:47997), so nothing after it runs (no log, no write to GameData\OccupyFlag.txt). OnTimer still calls it every 3 min (3760-3763), so it is a no-op. `_iComposeFlagStatusContents` is reachable only from here, so it never executes either ([GSO-021](../../issues/game-server-other.md#gso-021-occupy-flag-persistence-and-removal-code-is-dead)).

- sig: `void CGame::SaveOccupyFlagData()`
- callers: CGame::ChatMsgHandler, CGame::MsgProcess, CGame::OnTimer
- callees: PutLogList, _iComposeFlagStatusContents

## CGame::CreateCrusadeStructures - SRV Game.cpp:48019-48134

Spawns configured structures and picks 3 mana stones. If fewer than 3 type-42 entries exist it still indexes iMSIndex[0..2] unconditionally (48040-48045) (minor) ([GSO-024](../../issues/game-server-other.md#gso-024-createcrusadestructures-indexes-three-mana-stones-unconditionally)).

- sig: `void CGame::CreateCrusadeStructures()`
- callers: CGame::LocalStartCrusadeMode
- callees: PutLogList, SetNamingValueEmpty, bCreateNewNpc, iDice, iGetEmptyNamingValue
- cfg: Crusade.cfg, Npc.cfg
- Korean comments (UTF-8, first 6, untranslated quote): v2.05 마나 스톤의 인덱스를 따로 구한다. / 정의된 마나 스톤의 갯수가 3개 이하라면 모두 다 선택 / 임의의 3개를 선택한다.

## CGame::SendCollectedMana - SRV Game.cpp:48137-48170

Sends collected mana to the gate (GSM_COLLECTEDMANA); returns early if the Aresden and Elvine amounts are both 0, and afterwards zeroes m_iCollectedMana[0..2] [V, review R2-13].

- sig: `void CGame::SendCollectedMana()`
- callers: CGame::OnTimer
- callees: bStockMsgToGateServer
- Korean comments (UTF-8, first 2, untranslated quote): 모인 마나가 없다면 메시지 전송하지 않는다. 마나 스톤이 없는 맵에서 메시지를 전송하면 안된다. / 그동안 모은 마나량 보내고 클리어.

## CGame::CollectedManaHandler - SRV Game.cpp:48173-48195

Adds the other servers' mana to this server's m_iAresdenMana / m_iElvineMana pools (each only when that side's map index is not -1); the grand magic generators later drain the pools. Quirk: the Elvine log line is guarded by `wAresdenMana > 0` (copy-paste bug) [V, review R2-13] ([GSO-025](../../issues/game-server-other.md#gso-025-collectedmanahandler-guards-the-elvine-log-line-with-the-aresden-amount)).

- sig: `void CGame::CollectedManaHandler(WORD wAresdenMana, WORD wElvineMana)`
- callers: CGame::ServerStockMsgHandler
- callees: PutLogList

## CGame::_NpcBehavior_GrandMagicGenerator - SRV Game.cpp:48198-48243

Grand magic generator: when the side's pooled mana exceeds DEF_GMGMANACONSUMEUNIT (15), it zeroes the pool (surplus lost) and adds exactly 1 to m_iManaStock; it launches the meteor strike when the stock exceeds m_iMaxMana (48202-48212).

- sig: `void CGame::_NpcBehavior_GrandMagicGenerator(int iNpcH)`
- callers: CGame::NpcBehavior_Stop
- callees: MeteorStrikeMsgHandler, PutLogList, _GrandMagicLaunchMsgSend
- Korean comments (UTF-8, first 6, untranslated quote): 마나를 흡수한다. / 만약 마법 공격을 하기에 충분한 마나가 찼다면 적국으로 마법 공격 / 메테오 스트라이트 시그널

## CGame::MeteorStrikeMsgHandler - SRV Game.cpp:48246-48308

Incoming meteor strike: warns and schedules damage.

- sig: `void CGame::MeteorStrikeMsgHandler(char cAttackerSide)`
- callers: CGame::OnKeyUp, CGame::ServerStockMsgHandler, CGame::_NpcBehavior_GrandMagicGenerator
- callees: SendNotifyMsg, bRegisterDelayEvent
- notifies: DEF_NOTIFY_METEORSTRIKECOMING
- Korean comments (UTF-8, first 6, untranslated quote): 아레스덴 측의 폭격이다. 딜레이 이벤트에 등록 / 현재 서버에 폭격 지점이 있다. / v2.19 2002-11-12 강콜 지역을 확대하기 위해 LocationName 을 m_cName 으로 변경함  Location Name을 강콜 지역으로 설정함 .

## CGame::_LinkStrikePointMapIndex - SRV Game.cpp:48310-48333

Links strike points to their building maps.

- sig: `void CGame::_LinkStrikePointMapIndex()`
- callers: CGame::OnStartGameSignal
- callees: PutLogList
- Korean comments (UTF-8, first 3, untranslated quote): 마을과 건물 내부를 다른 서버에 띄우면 링크가 불가능하므로 다운된다! / 스트라이크 포인트가 정의된 맵이다. 각각의 스트라이크 포인트와 연계된 맵의 인덱스를 구해 저장한다. / 인덱스를 구했다. 저장한다.

## CGame::MeteorStrikeHandler - SRV Game.cpp:48336-48450

Applies a strike to a random intact strike point.

- sig: `void CGame::MeteorStrikeHandler(int iMapIndex)`
- callers: CGame::DelayEventProcessor
- callees: GetOwner, PutLogList, SendNotifyMsg, bRegisterDelayEvent, iAddDynamicObjectList, iDice
- notifies: DEF_NOTIFY_METEORSTRIKEHIT
- Korean comments (UTF-8, first 6, untranslated quote): 스트라이크 포인트의 인덱스를 넣는다. / 결과 보고용 스트럭춰 클리어 / 폭격할 건물이 없다. 모두 파괴되었다.

## CGame::RequestSetGuildTeleportLocHandler - SRV Game.cpp:48453-48558

Commander sets the guild teleport point (crusade).

- sig: `void CGame::RequestSetGuildTeleportLocHandler(int iClientH, int dX, int dY, int iGuildGUID, char * pMapName)`
- callers: CGame::ClientCommonHandler
- callees: PutLogList, bStockMsgToGateServer
- Korean comments (UTF-8, first 6, untranslated quote): v2.04 좌표 보정 / 게이트 서버 메시지 작성 / 먼저 같은 GUID를 가진 길드 텔레포트 좌표가 설정되어 있는지 찾는다.

## CGame::RequestSetGuildConstructLocHandler - SRV Game.cpp:48560-48662

Commander sets the construction point.

- sig: `void CGame::RequestSetGuildConstructLocHandler(int iClientH, int dX, int dY, int iGuildGUID, char * pMapName)`
- callers: CGame::ClientCommonHandler
- callees: PutLogList, bStockMsgToGateServer
- Korean comments (UTF-8, first 6, untranslated quote): 게이트 서버 메시지 작성 / 먼저 같은 GUID를 가진 길드 컨스트럭트 좌표가 설정되어 있는지 찾는다. / 텔레포트 위치의 2번 변수들을 사용한다.

## CGame::RequestGuildTeleportHandler - SRV Game.cpp:48664-48698

Teleports a guild member to the guild teleport point.

- sig: `void CGame::RequestGuildTeleportHandler(int iClientH)`
- callers: CGame::ClientCommonHandler
- callees: PutLogList, RequestTeleportHandler, SendNotifyMsg
- notifies: DEF_NOTIFY_LOCKEDMAP
- Korean comments (UTF-8, first 6, untranslated quote): 맵에 갖혔다면 그 시간동안 텔레포트 불가능. / 길드를 찾았다. / !!! RequestTeleportHandler내에서 m_cMapName을 쓰기 때문에 그대로 파라미터로 넘겨주면 오동작

## CGame::RequestSummonWarUnitHandler - SRV Game.cpp:48700-48935

Commander summons war units for construction points.

- sig: `void CGame::RequestSummonWarUnitHandler(int iClientH, int dX, int dY, char cType, char cNum, char cMode)`
- callers: CGame::ClientCommonHandler
- callees: GetOwner, PutLogList, SendNotifyMsg, SetNamingValueEmpty, _bItemLog, bCreateNewNpc, bSetNpcFollowMode, iGetEmptyNamingValue
- notifies: DEF_NOTIFY_CANNOTCONSTRUCT, DEF_NOTIFY_CONSTRUCTIONPOINT
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 에러 체크용 / 건설하기에 포인트가 모자란다. / 건물 내부면 전쟁 유니트 소환 불가.

## CGame::SelectCrusadeDutyHandler - SRV Game.cpp:49042-49072

Player picks soldier/engineer/commander. Commander (3) requires guild rank 0 (49046). Under DEF_V219, a first-time commander on the losing side (`3 - m_iWinnerSide == side`) is given 3000 construction points (49051-49060).

- sig: `void CGame::SelectCrusadeDutyHandler(int iClientH, int iDuty)`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg, _bCrusadeLog
- notifies: DEF_NOTIFY_CRUSADE
- switches: DEF_V219
- Korean comments (UTF-8, first 2, untranslated quote): 길드 마스터가 아닌데 지휘관 역할을 맡겠다는 메시지가 수신되면 무시. / v2.19 2002-11-15  전면전시 전에 전면전에 진쪽은 길드마스터들은 소환 포인트를 3000을 넣어 준다..

## CGame::MapStatusHandler - SRV Game.cpp:49074-49164

Commander map status request.

- sig: `void CGame::MapStatusHandler(int iClientH, int iMode, char * pMapName)`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg, _SendMapStatus
- notifies: DEF_NOTIFY_TCLOC
- Korean comments (UTF-8, first 6, untranslated quote): 건설 위치를 저장해 놓는다. / 길드 소속이 아닌 군인의 요청이다. / 데이터 캐시 구조체 초기화

## CGame::_SendMapStatus - SRV Game.cpp:49167-49227

Sends structure positions in chunks of 100.

- sig: `void CGame::_SendMapStatus(int iClientH)`
- callers: CGame::CheckClientResponseTime, CGame::MapStatusHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_MAPSTATUSLAST, DEF_NOTIFY_MAPSTATUSNEXT
- Korean comments (UTF-8, first 5, untranslated quote): 총 몇개의 데이터가 있는지 알리는 곳 띄움. / 100개씩 정보를 보낸다. / v2.15 보통 캐릭터는 마나 스톤만 보여준다.

## CGame::RemoveCrusadeStructures - SRV Game.cpp:49230-49248

Deletes all crusade structure NPCs.

- sig: `void CGame::RemoveCrusadeStructures()`
- callers: CGame::LocalEndCrusadeMode
- callees: DeleteNpc

## CGame::RequestHelpHandler - SRV Game.cpp:49268-49287

Sends position to the guild commander.

- sig: `void CGame::RequestHelpHandler(int iClientH)`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_HELP, DEF_NOTIFY_HELPFAILED
- Korean comments (UTF-8, first 3, untranslated quote): 헬프 요청이다. 길드 마스터에게 좌표를 전달해 준다. / 같은 길드 마스터이고 지휘관 역활을 맡은 상태이다. / 현재 맵에 지휘관이 없음을 통보해 준다.

## CGame::DoMeteorStrikeDamageHandler - SRV Game.cpp:49866-49917

Meteor damage to players with side != 0 in the struck map: the first two damage formulas are overwritten by `iDice(1,level) + level` (49873-49876), so damage is level+1 to 2*level, capped at 255; protect status 2 halves it; admins take 0 (49872-49884). Kills go through ClientKilledHandler and count as casualties ([GSO-026](../../issues/game-server-other.md#gso-026-meteor-strike-damage-formulas-are-overwritten)).

- sig: `void CGame::DoMeteorStrikeDamageHandler(int iMapIndex)`
- callers: CGame::DelayEventProcessor
- callees: ClearOwner, ClientKilledHandler, SendEventToNearClient_TypeA, SendNotifyMsg, SetOwner, bRemoveFromDelayEventList, iDice
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_HP, DEF_NOTIFY_MAGICEFFECTOFF
- types: DEF_OBJECTDAMAGE
- Korean comments (UTF-8, first 6, untranslated quote): 해당 맵의 플레이어들의 HP를 일괄 삭감 / 최대 255이상의 대미지는 받지 않는다. / v2.04 만약 마방이 걸려 있다면 대미지는 절반

## CGame::CalcMeteorStrikeEffectHandler - SRV Game.cpp:49922-50044

Tallies strike results; ends crusade when all buildings fall.

- sig: `void CGame::CalcMeteorStrikeEffectHandler(int iMapIndex)`
- callers: CGame::DelayEventProcessor
- callees: GrandMagicResultHandler, LocalEndCrusadeMode, PutLogList, bStockMsgToGateServer
- Korean comments (UTF-8, first 6, untranslated quote): 사용자에게 남은 건물의 HP를 보여주기 위한 변수 초기화 / 폭격의 효과를 계산한다. 모든 건물이 파괴되었다면 전면전 모드 종료. 그렇지 않으면 결과를 모든 서버에 리포트. / 작동중인 건물이 없다. 크루세이드 모드 종료: 다른 서버에 알려준다.

## CGame::GrandMagicResultHandler - SRV Game.cpp:50048-50056

Shows strike results to clients.

- sig: `void CGame::GrandMagicResultHandler(char *cMapName, int iCrashedStructureNum, int iStructureDamageAmount, int iCasualities, int iActiveStructure, int iSTcount,char * pData)`
- callers: CGame::CalcMeteorStrikeEffectHandler, CGame::ServerStockMsgHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_GRANDMAGICRESULT

## CGame::_GrandMagicLaunchMsgSend - SRV Game.cpp:50058-50077

Broadcasts a grand magic launch via the gate.

- sig: `void CGame::_GrandMagicLaunchMsgSend(int iType, char cAttackerSide)`
- callers: CGame::OnKeyUp, CGame::_NpcBehavior_GrandMagicGenerator
- callees: bStockMsgToGateServer

## CGame::SyncMiddlelandMapInfo - SRV Game.cpp:50079-50131

Shares Middleland structure positions with other servers.

- sig: `void CGame::SyncMiddlelandMapInfo()`
- callers: CGame::OnTimer
- callees: bStockMsgToGateServer
- Korean comments (UTF-8, first 4, untranslated quote): 미들랜드의 정보를 모든 서버에게 싱크시킨다. 미들랜드를 담당한 서버라면 바로 구조체를 업데이트 하고 나머지 서버들에게 정보 전송. / 구조체 클리어 / 구조체 복사 및 전송 메시지 작성

## CGame::GSM_SetGuildTeleportLoc - SRV Game.cpp:50134-50202

Remote server set the guild teleport point.

- sig: `void CGame::GSM_SetGuildTeleportLoc(int iGuildGUID, int dX, int dY, char * pMapName)`
- callers: CGame::ServerStockMsgHandler
- callees: PutLogList
- Korean comments (UTF-8, first 6, untranslated quote): 클라이언트로부터의 요청이 아니라 다른 서버로부터의 텔레포트 좌표 설정 요청임. 응답할 필요 없음. / 먼저 같은 GUID를 가진 길드 텔레포트 좌표가 설정되어 있는지 찾는다. / 이미 길드가 있다.

## CGame::GSM_SetGuildConstructLoc - SRV Game.cpp:50204-50272

Remote server set the construction point.

- sig: `void CGame::GSM_SetGuildConstructLoc(int iGuildGUID, int dX, int dY, char * pMapName)`
- callers: CGame::ServerStockMsgHandler
- callees: PutLogList
- Korean comments (UTF-8, first 6, untranslated quote): 클라이언트로부터의 요청이 아니라 다른 서버로부터의 텔레포트 좌표 설정 요청임. 응답할 필요 없음. / 먼저 같은 GUID를 가진 길드 텔레포트 좌표가 설정되어 있는지 찾는다. / 이미 길드가 있다.

## CGame::GlobalStartCrusadeMode - SRV Game.cpp:50274-50300

Starts crusade on all servers (once a day guard).

- sig: `void CGame::GlobalStartCrusadeMode()`
- callers: CGame::ChatMsgHandler, CGame::CrusadeWarStarter, CGame::OnKeyUp
- callees: LocalStartCrusadeMode, bStockMsgToGateServer
- Korean comments (UTF-8, first 1, untranslated quote): 오류로 인해 하루에 두번 전면전이 발생하는 것을 막는 코드

## CGame::CheckCommanderConstructionPoint - SRV Game.cpp:50302-50353

Moves construction points to the commander.

- sig: `void CGame::CheckCommanderConstructionPoint(int iClientH)`
- callers: CGame::CheckClientResponseTime
- callees: SendNotifyMsg, bStockMsgToGateServer
- notifies: DEF_NOTIFY_CONSTRUCTIONPOINT
- Korean comments (UTF-8, first 6, untranslated quote): 파이터 / 건설자: 모아 놓은 포인트를 길드마스터 지휘관에게 전달한다. / 현재 서버 내에 길드마스터 지휘관이 있다. 지휘관의 포인트를 높인 후

## CGame::GSM_ConstructionPoint - SRV Game.cpp:50355-50380

Remote construction points for a commander (+exp).

- sig: `void CGame::GSM_ConstructionPoint(int iGuildGUID, int iPoint)`
- callers: CGame::ServerStockMsgHandler
- callees: PutLogList, SendNotifyMsg
- notifies: DEF_NOTIFY_CONSTRUCTIONPOINT
- Korean comments (UTF-8, first 2, untranslated quote): 해당 길드마스터 지휘관을 찾았다. / v2.19 2002-12-4 길드 마스터의 경험치를 높힌다.

## CGame::_CreateCrusadeGUID - SRV Game.cpp:50420-50457

Writes GameData/CrusadeGUID.Txt with the current crusade GUID and winner side.

- sig: `void CGame::_CreateCrusadeGUID(DWORD dwCrusadeGUID, int iWinnerSide)`
- callers: CGame::LocalEndCrusadeMode, CGame::LocalStartCrusadeMode
- callees: PutLogList
- Korean comments (UTF-8, first 1, untranslated quote): 파일을 만들 수 없거나 사이즈가 지나치게 작은 경우는 .

## CGame::bReadCrusadeGUIDFile - SRV Game.cpp:50460-50523

Reads GameData/CrusadeGUID.Txt (crusade GUID and winner side) to resume crusade state.

- sig: `BOOL CGame::bReadCrusadeGUIDFile(char * cFn)`
- callers: CGame::OnStartGameSignal
- callees: CStrTok, PutLogList, pGet
- Korean comments (UTF-8, first 1, untranslated quote): 게임서버의 초기화 파일을 읽을 수 없다.

## CGame::ManualEndCrusadeMode - SRV Game.cpp:50526-50560

GM ends crusade as a draw.

- sig: `void CGame::ManualEndCrusadeMode(int iWinnerSide)`
- callers: CGame::ChatMsgHandler, CGame::OnKeyUp
- callees: LocalEndCrusadeMode, bStockMsgToGateServer
- Korean comments (UTF-8, first 2, untranslated quote): 매뉴얼로 크루세이드 모드를 종료시킨다. 비긴 상태로 종료시킨다. / 다른 서버에 크루세이드 종료를 알림.

## CGame::CrusadeWarStarter - SRV Game.cpp:50669-50689

Starts crusade at scheduled times, only on the server whose m_cServerName equals the file's `game-server-name` value (m_bIsCrusadeWarStarter, set at 50729-50731, 50761); it is not hard-wired to server 1 despite the source comment.

- sig: `void CGame::CrusadeWarStarter()`
- callers: CGame::OnTimer
- callees: GlobalStartCrusadeMode
- cfg: WarSchedule.cfg (read by bReadCrusadeScheduleConfigFile)
- Korean comments (UTF-8, first 2, untranslated quote): 게임 서버 1번에서만 / 리스트에서 찾았다. 전면전 시작

## CGame::bReadCrusadeScheduleConfigFile - SRV Game.cpp:50691-50779

Reads WarSchedule.cfg (opened by OnStartGameSignal, 20239): rows `schedule = day hour minute`, up to DEF_MAXSCHEDULE=10 (Game.h:166), into m_stCrusadeWarSchedule, after matching `game-server-name = <name>` against m_cServerName. GServer.cfg is not read here.

- sig: `BOOL CGame::bReadCrusadeScheduleConfigFile(char *pFn)`
- callers: CGame::OnStartGameSignal
- callees: CStrTok, PutLogList, pGet
- cfg: WarSchedule.cfg
- Korean comments (UTF-8, first 1, untranslated quote): 게임서버의 초기화 파일을 읽을 수 없다.

## CGame::KillCrusadeObjects - SRV Game.cpp:50781-50799

Kills all summoned war units.

- sig: `void CGame::KillCrusadeObjects()`
- callers: CGame::LocalEndCrusadeMode
- callees: NpcKilledHandler
- Korean comments (UTF-8, first 1, untranslated quote): 소환된 모든 몬스터를 죽인다.

## CGame::SetForceRecallTime - SRV Game.cpp:52716-52779

Sets the enemy-town stay time: the server value m_sForceRecallTime (minutes) wins if > 0 (from the `force-recall` token, 25099); else by weekday: Mon-Thu 3 min (1 under DEF_CONSULTATION, off), Fri 15, Sat 45, Sun 60 (52729-52750). The already-running branch uses 1 min for Mon-Thu regardless of DEF_CONSULTATION (52763-52766) and, if the remaining time exceeds that value, sets it to 1 (it never extends) (52773) ([PK-LAW-003](../../issues/pk-law.md#pk-law-003-force-recall-residual-time-branch-is-dead-code)).

- sig: `void CGame::SetForceRecallTime(int iClientH)`
- callers: CGame::RequestInitDataHandler, CGame::RequestTeleportHandler
- cfg: World cfg
- switches: DEF_CONSULTATION
- Korean comments (UTF-8, first 6, untranslated quote): 남아있을 수 있는 잔존 시간을 설정해 준다. / 시간을 구한다음 iWarPeriod를 구한다. / 서버에 강콜 시간이 설정되어 있으면 서버 강콜 시간을 강콜 시간으로 한다.

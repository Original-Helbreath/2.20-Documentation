# Characters: init, stats, levels, skills, regen

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
`cfg:` means data loaded from that config (often via the Log server), not a literal file open in the body [V, review G1].
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 46

## CGame::RequestInitPlayerHandler - SRV Game.cpp:1501-1609

MSGID_REQUEST_INITPLAYER: stores char/account/password, handles duplicates on the account and by character name, asks the Log server for player data (MSGID_REQUEST_PLAYERDATA). Duplicate rule: password match -> the OLD session is deleted (DeleteClient(i,TRUE,TRUE,FALSE)); password mismatch -> the NEW client is dropped (DeleteClient(iClientH,FALSE,FALSE)). bDecode is a no-op ([GSO-002](../../issues/game-server-other.md#gso-002-cmiscbencode-and-bdecode-are-no-ops)).

- sig: `void CGame::RequestInitPlayerHandler(int iClientH, char * pData, char cKey)`
- callers: CGame::MsgProcess
- callees: DeleteClient, PutLogList, bDecode, bSendMsgToLS
- messages: MSGID_REQUEST_PLAYERDATA
- Korean comments (UTF-8, first 6, untranslated quote): 플레이어의 이름, 계정의 이름, 패스워드를 기록하고 로그 서버로 플레이어 데이터 전송을 요청한다. / 중복된 계정을 갖고있는 캐릭터를 삭제한다. / 패스워드가 일치하면 뚫고 들어갈 수 있다. 그러나 다르면 접속 불가.

## CGame::RequestInitDataHandler - SRV Game.cpp:1611-2250

MSGID_REQUEST_INITDATA: after data loaded, places the player on the map and sends MSGID_RESPONSE_INITDATA, character/item contents, map data and the initial notify set (crusade, special ability, force-recall, etc.).

- sig: `void CGame::RequestInitDataHandler(int iClientH, char * pData, char cKey, BOOL bIsNoNameCheck)`
- callers: CGame::MsgProcess, CGame::ResurrectPlayer
- callees: CalcTotalItemEffect, DeleteClient, GetMapInitialPoint, PutLogEventFileList, PutLogFileList, PutLogList, SendEventToNearClient_TypeA, SendNotifyMsg, SetForceRecallTime, SetNoHunterMode, SetOwner, _CheckQuestEnvironment, _SendQuestContents, _iGetArrowItemIndex, bDecode, bGetEmptyPosition, iComposeInitMapData, iGetMapLocationSide, iSendMsg, iSetSide
- messages: MSGID_EVENT_LOG, MSGID_PLAYERCHARACTERCONTENTS, MSGID_PLAYERITEMLISTCONTENTS, MSGID_RESPONSE_INITDATA
- notifies: DEF_NOTIFY_CONSTRUCTIONPOINT, DEF_NOTIFY_CRUSADE, DEF_NOTIFY_DOWNSKILLINDEXSET, DEF_NOTIFY_FORCERECALLTIME, DEF_NOTIFY_GIZONITEMUPGRADELEFT, DEF_NOTIFY_ITEMPOSLIST, DEF_NOTIFY_SAFEATTACKMODE, DEF_NOTIFY_SPECIALABILITYENABLED
- types: DEF_MSGTYPE_CONFIRM
- switches: DEF_GUILDWARMODE, DEF_V219
- Korean comments (UTF-8, first 6, untranslated quote): 최초 접속 데이터를 전송해 준다. 여기서 이름이 일치하는지 확인할 수도 있다. / 이름이 일치하는지 검사한다. 일치하지 않으면 삭제하고 귀환한다. / 캐릭터의 신상정보를 전송한다.

## CGame::DeleteClient - SRV Game.cpp:2571-2887

Removes a client: notifies others, clears map ownership, cancels exchange/whisper/party, saves (MSGID_REQUEST_SAVEPLAYERDATALOGOUT) or not, updates account wait status, frees the slot. Also rewrites the respawn map for killed players (resurr1/resurr2/jail with locked-map times, mirroring RequestRestartHandler) and stocks GSM_DISCONNECT to the gate.

- sig: `void CGame::DeleteClient(int iClientH, BOOL bSave, BOOL bNotify, BOOL bCountLogout, BOOL bForceCloseConn)`
- callers: CGame::AdminOrder_CloseConn, CGame::CheckClientResponseTime, CGame::CheckConnectionHandler, CGame::CheckSpecialEventThirdYear, CGame::FightzoneReserveHandler, CGame::GetFightzoneTicketHandler, CGame::GetHeroMantleHandler, CGame::GetOccupyFlagHandler, CGame::GetRewardMoneyHandler, CGame::GiveItemHandler, CGame::InitPlayerData, CGame::MsgProcess, CGame::OnClientSocketEvent, CGame::PlayerMagicHandler, CGame::ReqCreatePortionHandler, CGame::ReqRepairItemCofirmHandler, CGame::ReqSellItemConfirmHandler, CGame::RequestChargedTeleportHandler, CGame::RequestCheckAccountPasswordHandler, CGame::RequestCivilRightHandler, CGame::RequestCreateNewGuildHandler, CGame::RequestFullObjectData, CGame::RequestInitDataHandler, CGame::RequestInitPlayerHandler, CGame::RequestPurchaseItemHandler ...
- callees: ClearOwner, LocalSavePlayerData, PutLogEventFileList, PutLogList, RemoveClientShortCut, RemoveFromTarget, SendEventToNearClient_TypeA, SendMsgToGateServer, SendNotifyMsg, _ClearExchangeStatus, bRemoveFromDelayEventList, bSendMsgToLS, bStockMsgToGateServer, iDice
- messages: MSGID_EVENT_LOG, MSGID_PARTYOPERATION, MSGID_REQUEST_NOSAVELOGOUT, MSGID_REQUEST_SAVEPLAYERDATALOGOUT, MSGID_REQUEST_SETACCOUNTWAITSTATUS
- notifies: DEF_NOTIFY_WHISPERMODEOFF
- types: DEF_MSGTYPE_REJECT
- Korean comments (UTF-8, first 6, untranslated quote): 다른 클라이언트들에게 플레이어 접속종료를 알린다. //!!!!!!! Recursion Error가 뜬다!!!! / 재귀호출 에러땜에 비활성화 됨. / 게임상에 있어야만 처리가 가능한 부분.

## CGame::ResponsePlayerDataHandler - SRV Game.cpp:4796-4853

MSGID_RESPONSE_PLAYERDATA from the Log server: finds the client by name and calls InitPlayerData or rejects: MSGID_RESPONSE_INITPLAYER REJECT is sent only if `*cp==1`, otherwise DeleteClient(i,FALSE,FALSE).

- sig: `void CGame::ResponsePlayerDataHandler(char * pData, DWORD dwSize)`
- callers: CGame::MsgProcess
- callees: DeleteClient, InitPlayerData, PutLogList, iSendMsg
- messages: MSGID_RESPONSE_INITPLAYER
- types: DEF_MSGTYPE_REJECT
- Korean comments (UTF-8, first 5, untranslated quote): 로그 서버로부터 플레이어 데이터가 도착했다. / 이름이 일치하는 클라이언트 배열을 검색한다. / 클라이언트 데이터가 정상적으로 수신되었다.

## CGame::InitPlayerData - SRV Game.cpp:4855-5043

Decodes the player record (_bDecodePlayerDatafileContents), validates it, computes derived state (next-level exp, items, guild check, party), sends MSGID_RESPONSE_INITPLAYER (confirm) to the client and MSGID_ENTERGAMECONFIRM to the Log server (bSendMsgToLS).

- sig: `void CGame::InitPlayerData(int iClientH, char * pData, DWORD dwSize)`
- callers: CGame::ResponsePlayerDataHandler
- callees: CalcTotalItemEffect, CheckSpecialEventThirdYear, DeleteClient, GetMapInitialPoint, PutLogList, RequestTeleportHandler, SendMsgToGateServer, SendNotifyMsg, SetInvisibilityFlag, SetNoHunterMode, ___RestorePlayerCharacteristics, ___RestorePlayerRating, _bDecodePlayerDatafileContents, bSendMsgToLS, iCalcTotalWeight, iDice, iSendMsg, iSetSide
- messages: MSGID_ENTERGAMECONFIRM, MSGID_PARTYOPERATION, MSGID_RESPONSE_INITPLAYER
- notifies: DEF_NOTIFY_GUILDDISBANDED
- types: DEF_MSGTYPE_CONFIRM
- Korean comments (UTF-8, first 6, untranslated quote): 이미 초기화 된 캐릭터의 데이터이다. 이런일이 있을 수 있을까? / Log Server로부터의 메시지를 분석하여 플레이어의 데이터를 초기화한다. / 만약 로그서버로부터의 메시지가 파일손상등의 이유로 내용이 정확하게 맞지 않으면 초기화 하지 않고 클라이언트를 삭제한다.

## CGame::TimeHitPointsUp - SRV Game.cpp:20297-20341

HP regen tick: 1d(Vit) (min Vit/2), reduced by iTemp/MaxHPdown when the MaxHPdown side effect is set (20318-20319), + HP stock + static stock, +AddHP%, capped at max HP. Skipped when hunger <= 0, dead, or skill 19 (play dead) is in use.

- sig: `void CGame::TimeHitPointsUp(int iClientH)`
- callers: CGame::CheckClientResponseTime
- callees: SendNotifyMsg, iDice, iGetMaxHP
- notifies: DEF_NOTIFY_HP
- Korean comments (UTF-8, first 6, untranslated quote): v1.4 죽은척하기 기술 사용중에는 체력 오르지 않음. / HP 차는 최소값을 보장. / v1.4 HP차는 양을 재조정한다.

## CGame::TimeManaPointsUp - SRV Game.cpp:20343-20372

MP regen tick: 1d(Mag) +AddMP%, max MP = 2*Mag + 2*Level + Int/2.

- sig: `void CGame::TimeManaPointsUp(int iClientH)`
- callers: CGame::CheckClientResponseTime
- callees: SendNotifyMsg, iDice
- notifies: DEF_NOTIFY_MP
- Korean comments (UTF-8, first 2, untranslated quote): v1.4 죽은척하기 기술 사용중에는 마나 오르지 않음. / Mana Point는 20초마다 1D(Magic)씩 올라간다.

## CGame::TimeStaminarPointsUp - SRV Game.cpp:20375-20412

SP regen tick: 1d(Vit/3) +AddSP%, +15/+10/+5 for level <=20/40/60; max SP = 2*Str + 2*Level.

- sig: `void CGame::TimeStaminarPointsUp(int iClientH)`
- callers: CGame::CheckClientResponseTime
- callees: SendNotifyMsg, iDice
- notifies: DEF_NOTIFY_SP
- Korean comments (UTF-8, first 3, untranslated quote): v1.4 죽은척하기 기술 사용중에는 스테미너 오르지 않음. / Staminar Point는 10초마다 1D(Vit/3)씩 올라간다. / v2.03 레벨 60 이하는 정기적으로 스테미나가 많이 찬다.

## CGame::ToggleCombatModeHandler - SRV Game.cpp:20518-20545

Toggles peace/combat stance bit in appearance and broadcasts.

- sig: `void CGame::ToggleCombatModeHandler(int iClientH)`
- callers: CGame::ClientCommonHandler
- callees: SendEventToNearClient_TypeA
- messages: MSGID_EVENT_MOTION
- types: DEF_OBJECTNULLACTION
- Korean comments (UTF-8, first 4, untranslated quote): 죽은척하기나 죽은 상태에서는 변환할 수 없다. / 비전투 모드였다. 전투모드로 바꾼다. / 전투 모드였다. 비전투모드로 바꾼다.

## CGame::_bDecodeSkillConfigFileContents - SRV Game.cpp:24487-24651

Parses Skill.cfg pushed by the Log server (MSGID_SKILLCONFIGURATIONCONTENTS) into m_pSkillConfigList[60].

- sig: `BOOL CGame::_bDecodeSkillConfigFileContents(char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: CStrTok, PutLogList, _bGetIsStringIsNumber, pGet
- cfg: Skill.cfg
- Korean comments (UTF-8, first 4, untranslated quote): 스킬 번호 / 이미 할당된 번호가 있다. 에러이다. / 스킬 이름

## CGame::TrainSkillResponse - SRV Game.cpp:25385-25526

Teaches a skill at level iSkillLevel if not known (shop/book) and notifies.

- sig: `void CGame::TrainSkillResponse(BOOL bSuccess, int iClientH, int iSkillNum, int iSkillLevel)`
- callers: CGame::UseItemHandler
- callees: DeleteClient, _bItemLog, bCheckTotalSkillMasteryPoints, iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_SKILLTRAINSUCCESS
- cfg: Skill.cfg
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 이미 기술을 익힌 상태라면 소용이 없다. / 기술 총 합을 재 계산한다. / 기술을 배웠다는 메시지를 전송한다.

## CGame::_iGetSkillNumber - SRV Game.cpp:25529-25546

Skill name -> index.

- sig: `int CGame::_iGetSkillNumber(char * pSkillName)`
- cfg: Skill.cfg
- Korean comments (UTF-8, first 1, untranslated quote): 같은 이름을 가진 기술 설정을 찾았다. 기술 번호를 반환한다.

## CGame::iGetLevelExp - SRV Game.cpp:27183-27193

Recursive exp-to-reach-level formula: E(L) = E(L-1) + L*(50 + L*(L/17)^2).

- sig: `int CGame::iGetLevelExp(int iLevel)`
- callers: CGame::bInit

## CGame::_iCalcSkillSSNpoint - SRV Game.cpp:27195-27208

Skill-SSN threshold per skill level: L for L<=50, L*L/10 above.

- sig: `int CGame::_iCalcSkillSSNpoint(int iLevel)`
- callers: CGame::bInit

## CGame::bCheckLevelUp - SRV Game.cpp:27210-27306

Applies level-ups while exp >= next-level exp: +1 level, adds the LU_* allocations (<=3 total) to stats capped at 200, DEF_NOTIFY_LEVELUP, recomputes; at max level converts overflow exp into Gizon upgrade points. Calls SetNoHunterMode when level >= 100 (DEF_LIMITHUNTERLEVEL - 1 test) and the player is in hunter mode.

- sig: `BOOL CGame::bCheckLevelUp(int iClientH)`
- callers: CGame::CalcExpStock, CGame::CalculateGuildEffect, CGame::EnemyKillRewardHandler
- callees: CalcTotalItemEffect, PutLogFileList, SendNotifyMsg, SetNoHunterMode
- notifies: DEF_NOTIFY_EXP, DEF_NOTIFY_GIZONITEMUPGRADELEFT, DEF_NOTIFY_LEVELUP
- cfg: World cfg
- Korean comments (UTF-8, first 6, untranslated quote): 최대 레벨 제한 / 여기서 지존 관련 보너스 특성치 체크를 한다. / 경험치 환원

## CGame::LevelUpSettingsHandler - SRV Game.cpp:27310-27411

MSGID_LEVELUPSETTINGS: validates and stores the player's per-level stat allocation (m_cLU_*).

- sig: `void CGame::LevelUpSettingsHandler(int iClientH, char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- Korean comments (UTF-8, first 3, untranslated quote): Level-Up Setting값에 오류가 있는지 검사한다. / 오류가 없으면 값을 할당한다. / Level-Up Setting값에 오류가 있다.

## CGame::bCheckLimitedUser - SRV Game.cpp:27531-27545

Traveller cap: applies to side == DEF_NETURAL only; when exp >= m_iLevelExp20 it is clamped to m_iLevelExp20 - 1 (level 19 maximum) and DEF_NOTIFY_TRAVELERLIMITEDLEVEL is sent. DEF_LEVELLIMIT is used only in the hunger rule (3883); m_iLimitedUserExp is unused ([CHARACTER-006](../../issues/character.md#character-006-m_ilimiteduserexp-is-unused)).

- sig: `BOOL CGame::bCheckLimitedUser(int iClientH)`
- callers: CGame::CalcExpStock, CGame::CalculateGuildEffect, CGame::EnemyKillRewardHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_TRAVELERLIMITEDLEVEL
- Korean comments (UTF-8, first 1, untranslated quote): 여행자가 레벨 20 경험치를 얻었다면 19수준으로 환원.

## CGame::RequestCivilRightHandler - SRV Game.cpp:27547-27616

MSGID_REQUEST_CIVILRIGHT: a neutral of level >= 5 (`< 5` rejects, so level 5 qualifies) becomes a citizen of the town named by the current map's m_cLocationName (no cityhall check here). Afterwards it calls RequestDeletePartyHandler (if in a party), CheckSpecialEventThirdYear and RequestHuntmode. The `_bItemLog` callee is in an inactive DEF_TAIWANLOG block ([PK-LAW-005](../../issues/pk-law.md#pk-law-005-citizenship-can-be-granted-on-any-map)).

- sig: `void CGame::RequestCivilRightHandler(int iClientH, char *pData)`
- callers: CGame::MsgProcess
- callees: CheckSpecialEventThirdYear, DeleteClient, RequestDeletePartyHandler, RequestHuntmode, SendEventToNearClient_TypeA, _bItemLog, iSendMsg
- messages: MSGID_EVENT_MOTION, MSGID_RESPONSE_CIVILRIGHT
- types: DEF_OBJECTNULLACTION
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 이미 한 마을의 소속이 있다면 시민권을 가질 수 없다. / 레벨이 5 이하라도 시민권을 얻을 수 없다. / 현재 맵의 이름을 할당한다.

## CGame::_iCalcMaxLoad - SRV Game.cpp:28572-28577

Max carry weight = (Str + Level) * 500.

- sig: `int CGame::_iCalcMaxLoad(int iClientH)`
- callers: CGame::ConfirmExchangeItem, CGame::GetRewardMoneyHandler, CGame::ReqSellItemHandler, CGame::RequestRetrieveItemHandler, CGame::_bAddClientItemList, CGame::_bCheckItemReceiveCondition, CGame::_bDecodePlayerDatafileContents

## CGame::CalculateSSN_ItemIndex - SRV Game.cpp:28700-28835

Adds skill-use count for the weapon's related skill; when SSN exceeds the threshold the skill rises (capped by stat*2 or level*2), hit ratio +1 if wielding it, notifies DEF_NOTIFY_SKILL.

- sig: `void CGame::CalculateSSN_ItemIndex(int iClientH, short sWeaponIndex, int iValue)`
- callers: CGame::iCalculateAttackEffect
- callees: SendNotifyMsg, bCheckTotalSkillMasteryPoints
- notifies: DEF_NOTIFY_SKILL
- switches: DEF_TESTSERVER
- Korean comments (UTF-8, first 6, untranslated quote): 스킬 수준이 0이라면 카운트가 오르지 않는다. / 만약 SkillSSN이 제한치를 넘어섰다면 Skill값이 증가한다. / 스킬이 올랐다.

## CGame::CalculateSSN_SkillIndex - SRV Game.cpp:28838-28986

Same as above for a skill index; combat skills (4-10,14,21) gain double since v2.19.

- sig: `void CGame::CalculateSSN_SkillIndex(int iClientH, short sSkillIndex, int iValue)`
- callers: CGame::BuildItemHandler, CGame::PlayerMagicHandler, CGame::ReqCreatePortionHandler, CGame::ReqGetFishThisTimeHandler, CGame::UseSkillHandler, CGame::_CheckMiningAction, CGame::bCheckResistingMagicSuccess, CGame::bCheckResistingPoisonSuccess, CGame::iCalculateAttackEffect, CGame::iCalculateUseSkillItemEffect
- callees: SendNotifyMsg, bCheckTotalSkillMasteryPoints
- notifies: DEF_NOTIFY_SKILL
- switches: DEF_TESTSERVER
- Korean comments (UTF-8, first 6, untranslated quote): 기술 수준이 0이라면 스킬은 오르지 않는다. / v2.19 2002-11-17 공격 스킬은 빨리 오른다. / 만약 SkillSSN이 제한치를 넘어섰다면 Skill값이 증가한다.

## CGame::bCheckTotalSkillMasteryPoints - SRV Game.cpp:32531-32641

Enforces the 700 skill-point cap (32531-32641): if the down-skill (m_iDownSkillIndex) is set and its mastery > 0 it is lowered; if it is set but already 0, the skill with the lowest SSN among those with mastery >= 21 (excluding the skill just raised) is used; with no down-skill set nothing is found and the function returns FALSE. Each pass removes 1 point, or the whole skill if mastery <= 20, resets that skill's SSN, and lowers hit ratio when a related weapon is equipped.

- sig: `BOOL CGame::bCheckTotalSkillMasteryPoints(int iClientH, int iSkill)`
- callers: CGame::CalculateSSN_ItemIndex, CGame::CalculateSSN_SkillIndex, CGame::GetMagicAbilityHandler, CGame::TrainSkillResponse
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_SKILL
- Korean comments (UTF-8, first 6, untranslated quote): 스킬의 포인트가 초과되었다. 초과된 포인트 만큼 루프를 돌며 SSN이 가장 낮은 스킬을 내린다. / 지정된 스킬이 있다. / 마법 저항

## CGame::iCalcTotalWeight - SRV Game.cpp:34605-34636

Sums carried item weight (gold /20) and refreshes carry-anywhere effects (alter-item-drop index).

- sig: `int CGame::iCalcTotalWeight(int iClientH)`
- callers: CGame::ConfirmExchangeItem, CGame::DropItemHandler, CGame::FightzoneReserveHandler, CGame::GetFightzoneTicketHandler, CGame::GetHeroMantleHandler, CGame::GetOccupyFlagHandler, CGame::GetRewardMoneyHandler, CGame::GiveItemHandler, CGame::InitPlayerData, CGame::ItemDepleteHandler, CGame::ReqRepairItemCofirmHandler, CGame::ReqSellItemConfirmHandler, CGame::ReqSellItemHandler, CGame::RequestChargedTeleportHandler, CGame::RequestPurchaseItemHandler, CGame::RequestRetrieveItemHandler, CGame::RequestStudyMagicHandler, CGame::ResponseCreateNewGuildHandler, CGame::_bAddClientItemList, CGame::_bDecodePlayerDatafileContents, CGame::bSetItemToBankItem, CGame::iCalculateAttackEffect
- callees: iGetItemWeight
- Korean comments (UTF-8, first 2, untranslated quote): 착용하지 않아도 효과가 있는 아이템 효과. 소지하고 있어도 효과가 있기 때문에 여기서 검사. / 수명이 있어야 효과가 있다.

## CGame::SetPlayerProfile - SRV Game.cpp:34811-34832

/setpf: stores the player's profile text (spaces to underscores).

- sig: `void CGame::SetPlayerProfile(int iClientH, char * pMsg, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- Korean comments (UTF-8, first 2, untranslated quote): 공백란을 언더바로 바꾼다. / 에러 방지 코드

## CGame::GetPlayerProfile - SRV Game.cpp:34834-34875

/pf: returns another player's profile (DEF_NOTIFY_PLAYERPROFILE) or not-online.

- sig: `void CGame::GetPlayerProfile(int iClientH, char * pMsg, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, pGet
- notifies: DEF_NOTIFY_PLAYERNOTONGAME, DEF_NOTIFY_PLAYERPROFILE
- Korean comments (UTF-8, first 3, untranslated quote): token이 곧 프로필을 얻고자 하는 사용자 이름 / v2.19 2002-12-18 profile이 않나오는 버그 수정 / 현재 접속중이 아니다.

## CGame::___RestorePlayerCharacteristics - SRV Game.cpp:34878-35247

Repairs illegal stat totals on load against the budget (level-1)*3+70. Over budget: trimmed per stat. Under budget: ADDS the missing points (stats below 10 first, then skill-prerequisite targets, then random) (34878-35247).

- sig: `void CGame::___RestorePlayerCharacteristics(int iClientH)`
- callers: CGame::InitPlayerData
- callees: PutLogList, iDice
- Korean comments (UTF-8, first 6, untranslated quote): 먼저 값을 백업한다. / 복구할 필요가 없다면 귀환. / 이제 iToBeRestoredPoint 만큼의 포인트를 복구한다.

## CGame::CalcTotalItemEffect - SRV Game.cpp:35250-35813

Recomputes every derived stat from equipped items: attack dice, hit ratio (skill of weapon), defense ratio (Dex*2 + armour), absorption by slot, rare-item main/sub effects (AddAR/DR/HP/SP/MP/MR/PR/CD/Exp/Gold/abs), special ability, mana save, appearance.

- sig: `void CGame::CalcTotalItemEffect(int iClientH, int iEquipItemID, BOOL bNotify)`
- callers: CGame::InitPlayerData, CGame::ReleaseItemHandler, CGame::RequestInitDataHandler, CGame::bCheckLevelUp, CGame::bEquipItemHandler
- callees: SendEventToNearClient_TypeA, SendNotifyMsg, _iGetArrowItemIndex
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_SPECIALABILITYSTATUS
- types: DEF_OBJECTNULLACTION
- Korean comments (UTF-8, first 6, untranslated quote): 잘못된 무기 장착 조합이다. 둘 중 하나를 내려 놓는다. / 한손 검의 장착 상태를 해제한다. / 속성별 대미지 흡수

## CGame::PoisonEffect - SRV Game.cpp:35975-36003

Poison tick: 1d(poison level) damage (never below 1 HP); chance to cure = PoisonResist skill - 10 + AddPR (min 10%).

- sig: `void CGame::PoisonEffect(int iClientH, int iV1)`
- callers: CGame::CheckClientResponseTime
- callees: SendNotifyMsg, iDice
- notifies: DEF_NOTIFY_HP, DEF_NOTIFY_MAGICEFFECTOFF
- Korean comments (UTF-8, first 3, untranslated quote): 중독으로 죽지는 않는다. 다만 체력이 계속 깍이고 최소 1만 남는다. / 독성 저항 확률로 중독이 풀릴 수 있다. / 중독이 풀렸음을 알린다.

## CGame::bCheckResistingPoisonSuccess - SRV Game.cpp:36007-36033

Poison resist roll: 1d100 < PoisonResist skill + AddPR; success trains the skill.

- sig: `BOOL CGame::bCheckResistingPoisonSuccess(short sOwnerH, char cOwnerType)`
- callers: CGame::PlayerMagicHandler, CGame::iCalculateAttackEffect
- callees: CalculateSSN_SkillIndex, iDice
- Korean comments (UTF-8, first 3, untranslated quote): 독성 저항이 성공했는지를 계산한다. / 독성 저항 실패. 중독된다. / 독성 저항 성공. 플레이어라면 스킬을 올린다.

## CGame::SetPlayerReputation - SRV Game.cpp:36192-36256

/rep+ /rep-: rates another citizen once per cooldown (20*60 ticks of the 3 s counter decremented at 3876 in CheckClientResponseTime, called from the `m_dwGameTime2 > 3000` block at 3682, about 1 hour [V, review R2-21; period traced]); needs level >= 15 (36192); criminals and travellers cannot; value clamped to +-10000.

- sig: `void CGame::SetPlayerReputation(int iClientH, char * pMsg, char cValue, DWORD dwMsgSize)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, SendNotifyMsg, pGet
- notifies: DEF_NOTIFY_CANNOTRATING, DEF_NOTIFY_PLAYERNOTONGAME, DEF_NOTIFY_RATINGPLAYER
- Korean comments (UTF-8, first 6, untranslated quote): 평가를 내릴 시간이 되지 않았거나 범죄자는 평가를 내릴 수 없다. 기능을 사용할 수 없다. / 소속 마을이 없는 사람은 평판을 내릴 수 없다. / token이 곧 평판을 평가할 사용자 이름

## CGame::CalcExpStock - SRV Game.cpp:36374-36428

Every 10 s moves m_iExpStock into exp (not on no-reward maps) and adds it to m_iAutoExpAmount, notifies DEF_NOTIFY_EXP only if bCheckLimitedUser returns FALSE, checks level-up, gives 100 gold (levels 1-5) or 300 gold (6-20) once per call when it levelled up (final level).

- sig: `void CGame::CalcExpStock(int iClientH)`
- callers: CGame::CheckClientResponseTime
- callees: SendNotifyMsg, _bInitItemAttr, bAddItem, bCheckLevelUp, bCheckLimitedUser
- notifies: DEF_NOTIFY_EXP
- Korean comments (UTF-8, first 6, untranslated quote): v2.12 2002-2-6 지존도 현재 경험치가 최대 레벨 경험치보다 적으면 경험치를 얻을수 있게 변경 .. v2.15 삭제. 지존도 경험치 얻는다. / 포상과 페널티가 없는 맵 위에 서 있다면 경험치는 올라가지 않는다. / 그동안 축적 되었던 경험치를 계산한다.

## CGame::___RestorePlayerRating - SRV Game.cpp:36430-36436

Resets out-of-range reputation (\|rating\| > 10000) to 0.

- sig: `void CGame::___RestorePlayerRating(int iClientH)`
- callers: CGame::InitPlayerData

## CGame::iGetExpLevel - SRV Game.cpp:36438-36447

Inverse lookup: level whose table band contains the given exp.

- sig: `int CGame::iGetExpLevel(int iExp)`
- callers: CGame::EnemyKillRewardHandler, CGame::PK_KillRewardHandler
- Korean comments (UTF-8, first 1, untranslated quote): 경험치가 어느 레벨 수준에 속하는지 판단한다.

## CGame::ToggleSafeAttackModeHandler - SRV Game.cpp:38054-38066

Toggles safe-attack mode (no friendly damage, +40% mana cost).

- sig: `void CGame::ToggleSafeAttackModeHandler(int iClientH) //v1.1`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_SAFEATTACKMODE
- Korean comments (UTF-8, first 1, untranslated quote): 안전 공격 모드를 토글한다.

## CGame::SetDownSkillIndexHandler - SRV Game.cpp:39397-39407

Stores which skill to lower when the 700-point skill cap is exceeded.

- sig: `void CGame::SetDownSkillIndexHandler(int iClientH, int iSkillIndex)`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_DOWNSKILLINDEXSET
- Korean comments (UTF-8, first 1, untranslated quote): 다운 스킬을 설정했으니 응답을 보낸다.

## CGame::iGetMaxHP - SRV Game.cpp:42328-42340

Max HP = Vit*3 + Level*2 + Str/2, reduced by MaxHPdown side effect.

- sig: `int CGame::iGetMaxHP(int iClientH)`
- callers: CGame::ResurrectPlayer, CGame::TimeHitPointsUp, CGame::UseItemHandler, CGame::_iComposePlayerDataFileContents, CGame::iCalculateAttackEffect

## CGame::CheckSpecialEvent - SRV Game.cpp:44416-44466

v1.42 one-off event gift check (1st-anniversary ring, disabled since 2002-8-6).

- sig: `void CGame::CheckSpecialEvent(int iClientH)`
- callees: PutLogFileList, _bAddClientItemList, _bInitItemAttr
- Korean comments (UTF-8, first 6, untranslated quote): 2002-8-6 일주년 반지 더 이상 주지 않는다. / v1.42 이벤트 아이템을 준다. 2000.8.1일 1주년 반지 수여 / 저렙은 주지 않는다.

## CGame::CheckSpecialEventThirdYear - SRV Game.cpp:44470-44615

Third-anniversary gift (Nov 2002, citizens level >= 15).

- sig: `void CGame::CheckSpecialEventThirdYear(int iClientH)`
- callers: CGame::InitPlayerData, CGame::RequestCivilRightHandler
- callees: DeleteClient, PutLogFileList, _bAddClientItemList, _bInitItemAttr, _bItemLog, iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_ITEMOBTAINED
- switches: DEF_TESTSERVER
- Korean comments (UTF-8, first 6, untranslated quote): 시민권이 있는 사용자만 준다. / 2002-10-31 레벨 15이상만 / 기간 : 2002-11-01 ~ 2002-11-30 (11월 한달간)

## CGame::GetMagicAbilityHandler - SRV Game.cpp:46144-46154

Sets Magic skill to 20 for a player who asks (one-off) and rechecks skill totals.

- sig: `void CGame::GetMagicAbilityHandler(int iClientH)`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg, bCheckTotalSkillMasteryPoints
- notifies: DEF_NOTIFY_SKILL
- Korean comments (UTF-8, first 2, untranslated quote): 마법 능력 스킬을 20으로 변경후 알려줌 / v1.4311 스킬의 총 합을 체크

## CGame::RequestRestartHandler - SRV Game.cpp:46246-46321

Respawn after death: traveller -> "default" map; Aresden citizen killed in elvine -> elvjail with a 3 min lock, else level > 80 -> resurr1 else arefarm (Elvine mirror: arejail / resurr2 / elvfarm); in crusade it adds the dead-penalty logic (locked town map 5 min, DeadPenaltyTime 10 min). Sets HP = 3*Vit + 2*Lvl + Str/2 (ignores MaxHPdown) and hunger 100; MP and SP are NOT restored (46246-46321) ([CHARACTER-005](../../issues/character.md#character-005-restart-hp-ignores-the-maxhpdown-divisor)).

- sig: `void CGame::RequestRestartHandler(int iClientH)`
- callers: CGame::MsgProcess
- callees: RequestTeleportHandler
- Korean comments (UTF-8, first 6, untranslated quote): v2.181 2002-10-24 서버 속도 향상을 위해 스트링 비교를 줄인다. / 여행자라면  default맵으로 간다. / 소속이 있다면 소속 마을의 부활존으로 간다.

## CGame::RecallHunterPlayer - SRV Game.cpp:49251-49265

After crusade end sets m_iTimeLeft_ForceRecall = 0 for hunter-mode players with m_bIsWarLocation (49251); called from LocalEndCrusadeMode (47979).

- sig: `void CGame::RecallHunterPlayer()`
- callers: CGame::LocalEndCrusadeMode

## CGame::ResurrectPlayer - SRV Game.cpp:50853-50876

Not in-place: clears targeting and map ownership, moves the player to GetMapInitialPoint, sets full HP/MP/SP, then calls RequestInitDataHandler. DEAD in effect: its only caller (Game.cpp:4087) is inside a /* */ block ([CHARACTER-007](../../issues/character.md#character-007-resurrectplayer-has-no-live-caller)).

- sig: `void CGame::ResurrectPlayer(int iClientH)`
- callees: ClearOwner, GetMapInitialPoint, PutLogList, RemoveFromTarget, RequestInitDataHandler, SendEventToNearClient_TypeA, iGetMaxHP
- messages: MSGID_EVENT_LOG
- types: DEF_MSGTYPE_REJECT
- Korean comments (UTF-8, first 5, untranslated quote): 캐릭터를 부활시킨다. / 현재 이 플레이어를 목표로 삼고 있는 몹들의 공격행동을 멈추게 한다. / 현재 맵 위치에서 지운다.

## CGame::GetExp - SRV Game.cpp:51636-51775

Adds exp to the stock: low-level bonus (L<=80: x(1.025+0.025*(80-L))), farm-map reductions above 80, party split with bonus (DEF_V219 table: 1 member 0%, then 2, 5, 7, 10, 14, 17, 20% for 2-8 members) among living members (HP > 0). The 5-minute activity test applies to the exp RECEIVER (iClientH m_dwLastActionTime), not to each member; an own-kill attacker in a party also gets unit/10 extra (51688-51765).

- sig: `void CGame::GetExp(int iClientH, int iExp, BOOL bIsAttackerOwn)`
- callers: CGame::BuildItemHandler, CGame::CheckClientResponseTime, CGame::Effect_Damage_Spot, CGame::Effect_Damage_Spot_DamageMove, CGame::NpcKilledHandler, CGame::ReqCreatePortionHandler, CGame::ReqGetFishThisTimeHandler, CGame::_CheckMiningAction, CGame::_iTalkToNpcResult_Cityhall, CGame::bCropsItemDrop, CGame::iCalculateAttackEffect, CGame::iCalculateUseSkillItemEffect, CGame::iClientMotion_Attack_Handler
- callees: PutLogFileList
- switches: DEF_BEGINNEREVENTMODE, DEF_TESTSERVER, DEF_V219
- Korean comments (UTF-8, first 6, untranslated quote): v2.1 파티제에 맞추어 경험치를 나누고, 레벨이 80 미만인 경우 경험치를 더 얻는다. / 레벨 80 이하는 보너스 경험치가 적용된다. / v2.181 2002-10-25 초보용 미들이면 경치가 두배

## CGame::RequestHuntmode - SRV Game.cpp:52782-52823

v2.19 toggles civilian ('hunter') mode; updates appearance. Needs PKCount == 0, init complete and a map name starting "cityhall" (52791); the level test is `level > 100 && !hunter -> return`, so a level-100 player can still enable it. The mode is stored as m_cLocation "arehunter"/"elvhunter" ([PK-LAW-007](../../issues/pk-law.md#pk-law-007-hunter-mode-level-thresholds-disagree)).

- sig: `void CGame::RequestHuntmode(int iClientH)`
- callers: CGame::ClientCommonHandler, CGame::RequestCivilRightHandler
- callees: SendEventToNearClient_TypeA, SendNotifyMsg
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_RESPONSE_HUNTMODE
- types: DEF_OBJECTNULLACTION
- switches: DEF_V219
- Korean comments (UTF-8, first 2, untranslated quote): v2.20 2002-12-31 민간인 모드 레벨 제한 과 이동지역 제한 추가 / 특성이 바뀌므로 외양을 새로 보낸다.

## CGame::SetNoHunterMode - SRV Game.cpp:52825-52854

Forces civilian mode off (level reached 100 or crusade end).

- sig: `void CGame::SetNoHunterMode(int iClientH,BOOL bSendMsg) // v2.20 2002-12-31 민간인 모드 레벨 제한 과 이동지역 제한 추가`
- callers: CGame::InitPlayerData, CGame::RequestInitDataHandler, CGame::bCheckLevelUp
- callees: SendEventToNearClient_TypeA, SendNotifyMsg
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_RESPONSE_HUNTMODE
- types: DEF_OBJECTNULLACTION
- switches: DEF_V219
- Korean comments (UTF-8, first 2, untranslated quote): v2.20 2002-12-31 민간인 모드 레벨 제한 과 이동지역 제한 추가 / 특성이 바뀌므로 외양을 새로 보낸다.

## CGame::iSetSide - SRV Game.cpp:52857-52941

Derives m_cSide (0 traveller, 1 Aresden, 2 Elvine), town/shop/tower/warehouse/building flags and hunter mode from the location and current map name.

- sig: `int CGame::iSetSide(int iClientH)`
- callers: CGame::InitPlayerData, CGame::RequestInitDataHandler, CGame::RequestTeleportHandler
- switches: DEF_ALLPLAYERPROTECT, DEF_V219
- Korean comments (UTF-8, first 6, untranslated quote): 플레이어의 소속에 따라 Side를 결정한다. 여행자라면 0을 유지하고 Aresden이면 1, Elvine이면 2를 할당한다. / v2.181 2002-10-24 중립 공격 받는 버그 수정 / v2.17 2002-7-21 마을과 농경지에서만 같은 편에게 공격 받지 않게 설정함

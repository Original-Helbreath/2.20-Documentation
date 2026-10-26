# NPCs and AI

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
`cfg:` means data loaded from that config (here pushed by the Log server), not a literal file open in the body [V, review G1].
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 27

## CGame::bSetNpcFollowMode - SRV Game.cpp:9985-10040

Makes an NPC follow a named player/NPC on the same map. The follower is found with memcmp(m_cName, name, 5) (first 5 bytes only) and iIndex is uninitialised, so the `iIndex == -1` test (10031) is undefined on a miss. It also copies the leader's side onto the follower (10036) ([ENTITY-003](../../issues/entity.md#entity-003-bsetnpcfollowmode-and-bsetnpcattackmode-use-an-uninitialised-index)).

- sig: `BOOL CGame::bSetNpcFollowMode(char * pName, char * pFollowName, char cFollowOwnerType)`
- callers: CGame::AdminOrder_Summon, CGame::MobGenerator, CGame::PlayerMagicHandler, CGame::RequestSummonWarUnitHandler
- Korean comments (UTF-8, first 4, untranslated quote): 따라다닐 대상을 발견했다. / 다른 맵에 존재하는 대상은 따라다닐 수 없다. / 따라다닐 대상을 발견했다.

## CGame::bCreateNewNpc - SRV Game.cpp:10043-10365

Spawns an NPC from Npc.cfg with move type, waypoints, area, special ability, summon flags, guild GUID; day-of-week limit. Full signature has 17 parameters, ending `char cChangeSide, BOOL bHideGenMode, BOOL bIsSummoned, BOOL bFirmBerserk, BOOL bIsMaster, int iGuildGUID` (Game.cpp:10043). Returns the NPC slot index `i` on success (10361; crop NPCs return it early at 10353), not 1; callers compare against FALSE. bFirmBerserk doubles ExpDice (10323). bHideGenMode rejects the spawn if a player is within 7 tiles (10168) ([ENTITY-005](../../issues/entity.md#entity-005-bcreatenewnpc-declared-bool-but-returns-the-slot-index)).

- sig: `BOOL CGame::bCreateNewNpc(char * pNpcName, char * pName, char * pMapName, short sClass, char cSA, char cMoveType, int * poX, int * poY, char * pWaypointList, RECT * pArea, int iSpotMobIndex, char cChangeSide, BOOL bHideGenMode, BOOL bIsSummoned, BOOL bFirmBerserk, BOOL bIsMaster, int iGuildGUID)`
- note: the older bCreateNewNpc at Game.cpp:10368 is inside a /* */ block (DEAD).
- callers: CGame::AdminOrder_CallGuard, CGame::AdminOrder_Summon, CGame::AdminOrder_SummonDemon, CGame::CreateCrusadeStructures, CGame::EnergySphereProcessor, CGame::MobGenerator, CGame::PlayerMagicHandler, CGame::RequestSummonWarUnitHandler, CGame::__bReadMapInfo, CGame::__bSetAgricultureItem, CGame::__bSetConstructionKit, CGame::bAnalyzeCriminalAction
- callees: CNpc, PutLogList, SendEventToNearClient_TypeA, SetOwner, _bInitNpcAttr, _iGetPlayerNumberOnSpot, bAddCropsTotalSum, bAddCrusadeStructureInfo, bGetEmptyPosition, iDice
- messages: MSGID_EVENT_LOG
- types: DEF_MSGTYPE_CONFIRM
- Korean comments (UTF-8, first 6, untranslated quote): 이런 이름의 NPC는 없다. / 생성시 요일제한에 걸리는지 검사해야 한다. 10보다 크면 모든 요일에 다 생성가능. 0이면 일요일, 1이면 월요일... / 지금 생성이 불가능한 NPC이다.

## CGame::NpcProcess - SRV Game.cpp:10434-10536

Ticks every NPC by action time (attack-state tick shortened by 0-700 ms, floor 600 ms, 10446-10455; ice adds 50 %, 10460), HP/MP regen, summon expiry, behaviour dispatch. HP regen only when HP < iMaxHP = HitDice d8 + HitDice (re-rolled each tick) and only for non-summoned NPCs (10486-10493); MP regen only if m_cMagicLevel != 0 (10468-10477). Expiry (10516-10532): type 29 after 90 s -> NpcKilledHandler, type 64 (crop) after DEF_PLANTTIME 5 min -> DeleteNpc, other summons after DEF_SUMMONTIME 5 min -> NpcKilledHandler.

- sig: `void CGame::NpcProcess()`
- callers: CGame::GameProcess
- callees: DeleteNpc, NpcBehavior_Attack, NpcBehavior_Dead, NpcBehavior_Flee, NpcBehavior_Move, NpcBehavior_Stop, NpcKilledHandler, iDice
- Korean comments (UTF-8, first 6, untranslated quote): 속도를 조정한다. 공격시에는 몬스터들의 속도가 빨라진다. / v1.42 냉동 상태라면 움직임이 50% 느려진다. / 시간 저장.

## CGame::cGetNextMoveDir - SRV Game.cpp:11437-11475

Greedy step direction towards a destination avoiding blocked tiles (7 args; `cTurn` 0/1 picks the rotation direction of the blocked-tile search). This is the CGame overload only; `CMisc::cGetNextMoveDir(sX,sY,dX,dY)` (Misc.cpp:31) is a different function (see support.md).

- sig: `char CGame::cGetNextMoveDir(short sX, short sY, short dstX, short dstY, char cMapIndex, char cTurn, int * pError)`
- callers (CGame overload only [V]): CGame::NpcBehavior_Move (11621, 11642), CGame::NpcBehavior_Attack (12243), CGame::NpcBehavior_Flee (14826)
- callees: GetPoint, bGetMoveable, m_Misc.cGetNextMoveDir (11454; omitted by the name scan as a same-name call)

## CGame::bGetEmptyPosition - SRV Game.cpp:11481-11504

Finds a free tile around a point; if none, writes the map initial point into the out-parameters and still returns FALSE (11499-11503). GetMapInitialPoint returns without writing if the map has no initial point ([MAP-007](../../issues/map.md#map-007-map-without-initial-points-leaves-uninitialised-coordinates)).

- sig: `BOOL CGame::bGetEmptyPosition(short * pX, short * pY, char cMapIndex)`
- callers: CGame::RequestInitDataHandler, CGame::RequestTeleportHandler, CGame::bCreateNewNpc
- callees: GetMapInitialPoint, bGetIsTeleport, bGetMoveable
- Korean comments (UTF-8, first 2, untranslated quote): 플레이어나 NPC가 위치할 수 있는 영역을 찾는다. / 위치시킬만한 영역이 없으므로 해당 맵의 InitPoint에 할당한다.

## CGame::NpcBehavior_Move - SRV Game.cpp:11506-11663

Move behaviour: waypoint/random/follow movement, target search, sector stats. Summoned NPCs on Hold return without moving (11516); ActionLimit 2/3/5 switch to STOP (11521-11527); slow NPCs (m_dwActionTime < 1000) engage a found target 1 turn in 3 (11561-11570).

- sig: `void CGame::NpcBehavior_Move(int iNpcH)`
- callers: CGame::NpcProcess
- callees: CalcNextWayPointDestination, ClearOwner, SendEventToNearClient_TypeA, SetOwner, TargetSearch, cGetNextMoveDir, iDice
- messages: MSGID_EVENT_MOTION
- types: DEF_OBJECTMOVE
- Korean comments (UTF-8, first 6, untranslated quote): 소환몹의 제어 상태가 Hold라면 이동하지 않는다. / 이동할 수 없는 NPC는 리턴 / v1.432-2 맵 섹터 정보를 갱신한다.

## CGame::TargetSearch - SRV Game.cpp:11665-11802

Finds the nearest attackable target within search range (side rules, invisibility, structures). GMs (AdminUserLevel > 0) are never targeted (11707); type 51 searches only structures 36-41 (11685-11742); invisible targets are ignored unless the attacker has special ability 1 (11784); the self-skip `break` (11695) abandons the rest of that column ([ENTITY-010](../../issues/entity.md#entity-010-targetsearch-self-skip-break-abandons-the-rest-of-the-column)).

- sig: `void CGame::TargetSearch(int iNpcH, short * pTarget, char * pTargetType)`
- callers: CGame::NpcBehavior_Flee, CGame::NpcBehavior_Move, CGame::NpcBehavior_Stop
- callees: ClearOwner, GetOwner, _iCalcPlayerNum
- Korean comments (UTF-8, first 6, untranslated quote): 인덱스 iNpcH에 해당하는 NPC가 공격대상을 찾는다. / 공격 제한 없음 / 건축물만 공격함

## CGame::NpcBehavior_Attack - SRV Game.cpp:11805-12259

Attack behaviour: melee/range/magic choice, attack strategies, chase, with separate tower / catapult / dark-elf / beholder / frost branches (11911-12207).

- sig: `void CGame::NpcBehavior_Attack(int iNpcH)`
- callers: CGame::NpcProcess
- callees: ClearOwner, NpcMagicHandler, SendEventToNearClient_TypeA, SendNotifyMsg, SetIceFlag, SetOwner, bCheckResistingIceSuccess, bRegisterDelayEvent, cGetNextMoveDir (CGame overload at 12243 only; the CMisc overload is called at 11904, 11961, 12131), iCalculateAttackEffect, iDice, iGetDangerValue
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_MAGICEFFECTON
- types: DEF_OBJECTATTACK, DEF_OBJECTMOVE
- cfg: Magic.cfg
- Korean comments (UTF-8, first 6, untranslated quote): v1.432-2 맵 섹터 정보를 갱신한다. / 연속 공격 카운트 초기화 / 턴 카운트 증가.

## CGame::RemoveFromTarget - SRV Game.cpp:12261-12303

Clears every NPC targeting the given object. Side effect (12270-12277): for a player target whose guild GUID equals a war unit's GUID, every such NPC with m_cActionLimit == 0 gets m_bIsSummoned = TRUE and a fresh m_dwSummonedTime. There is no commander or death test: callers include teleport (23299), invisibility cast (22218), resurrect (50860) and disconnect (2608), so any such call converts the guild's war units to 5-minute summons. The source comment states the intent (commander death); the code does not implement that condition. `iCode` = invisibility keeps the target for special-ability-1 NPCs (12284-12287) ([ENTITY-002](../../issues/entity.md#entity-002-removefromtarget-turns-every-guild-war-unit-into-a-5-minute-summon)).

- sig: `void CGame::RemoveFromTarget(short sTargetH, char cTargetType, int iCode)`
- callers: CGame::ClientKilledHandler, CGame::DeleteClient, CGame::DeleteNpc, CGame::NpcKilledHandler, CGame::NpcMagicHandler, CGame::PlayerMagicHandler, CGame::RequestTeleportHandler, CGame::ResurrectPlayer
- Korean comments (UTF-8, first 4, untranslated quote): Crusade 만약 지휘관이 사망한 경우 지휘관 산하 NPC들은 소환몹 모드로 전환되어 일정시간 / 이후에 자동 폭파된다. 몹이 넘쳐나는 것을 막기 위함. / 이동 가능한 NPC만 소환 모드로 전환

## CGame::NpcBehavior_Dead - SRV Game.cpp:14704-14720

Corpse timer; deletes the NPC after the regen time.

- sig: `void CGame::NpcBehavior_Dead(int iNpcH)`
- callers: CGame::NpcProcess
- callees: DeleteNpc
- Korean comments (UTF-8, first 2, untranslated quote): 턴 카운트 증가. / 먼저 턴 카운트 클리어.

## CGame::iGetDangerValue - SRV Game.cpp:14722-14759

Danger score of a tile for fleeing.

- sig: `int CGame::iGetDangerValue(int iNpcH, short dX, short dY)`
- callers: CGame::NpcBehavior_Attack
- callees: GetOwner, bGetDynamicObject

## CGame::NpcBehavior_Flee - SRV Game.cpp:14761-14842

Flee behaviour. It is also entered on danger > bravery and when HP <= 2 (11881-11898). For the exchange/two-by-one attack strategies it returns to ATTACK after 2 turns; for others it requests assistance on 50 % of ticks (14787), flees up to 10 turns, then regains 1d(HitDice) HP if HP <= 3 (14791-14799).

- sig: `void CGame::NpcBehavior_Flee(int iNpcH)`
- callers: CGame::NpcProcess
- callees: ClearOwner, NpcRequestAssistance, SendEventToNearClient_TypeA, SetOwner, TargetSearch, cGetNextMoveDir, iDice
- messages: MSGID_EVENT_MOTION
- types: DEF_OBJECTMOVE
- Korean comments (UTF-8, first 6, untranslated quote): 턴 카운트 증가. / 공격 인공지능에 대한 효과계산. / 공격 -> 후퇴 -> ...

## CGame::_bDecodeNpcConfigFileContents - SRV Game.cpp:19677-20035

Parses Npc.cfg pushed by the Log server (MSGID_NPCCONFIGURATIONCONTENTS, Game.cpp:15210-15213) into m_pNpcConfigList[100]. The function names no file.

- sig: `BOOL CGame::_bDecodeNpcConfigFileContents(char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: CNpc, CStrTok, PutLogList, _bGetIsStringIsNumber, pGet
- cfg: Npc.cfg (via the Log server)
- messages: MSGID_NPCCONFIGURATIONCONTENTS
- Korean comments (UTF-8, first 4, untranslated quote): NPC 이름 / cGenDayWeekLimit  // 특정 요일에만 생성되는 몬스터여부 / 연결된 채팅 메시지가 존재한다. 내용을 읽어 저장한다.

## CGame::_bInitNpcAttr - SRV Game.cpp:20037-20206

Initialises an NPC instance: HP from HitDice, exp from ExpDice, stats, special-ability exp bonuses (SA 1 +25 %, 2 +30 %, 5 +15 %, 6/7 +20 %, 8 +25 %, 3/4 by |AbsDamage|). Also sets the attack strategy (random 1-10, except types 43-47 and 51 which are NORMAL), m_iAILevel = 1d3, forces SA 1 for type 53 (beholder), and resets SA 3/4 when the config's AbsDamage sign conflicts (20085-20133).

- sig: `BOOL CGame::_bInitNpcAttr(class CNpc * pNpc, char * pNpcName, short sClass, char cSA)`
- callers: CGame::bCreateNewNpc
- callees: iDice
- cfg: Npc.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 같은 이름을 가진 NPC 설정을 찾았다. 설정값으로 초기화한다. / HitDice에 따라 공식이 다르다. 몹간의 차별을 두기 위함. / v1.4 확인코드

## CGame::ReleaseFollowMode - SRV Game.cpp:24158-24171

Releases NPCs following a dead owner.

- sig: `void CGame::ReleaseFollowMode(short sOwnerH, char cOwnerType)`
- callers: CGame::NpcKilledHandler

## CGame::_bDecodeNpcItemConfigFileContents - SRV Game.cpp:25144-25360

Parses NpcItem config (DEF_NPCITEM) pushed by the Log server (MSGID_NPCITEMCONFIGCONTENTS, Game.cpp:15260-15262) into each NPC type's m_vNpcItem list. The block header token is `NpcItem` (25341). It does not read Item.cfg; it matches names against m_pNpcConfigList.

- sig: `BOOL CGame::_bDecodeNpcItemConfigFileContents(char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: CNpcItem, CStrTok, PutLogList, _bGetIsStringIsNumber, pGet
- messages: MSGID_NPCITEMCONFIGCONTENTS
- Korean comments (UTF-8, first 6, untranslated quote): NPC 이름 / npcitem type 설정 / 아이템 이름

## CGame::MobGenerator - SRV Game.cpp:28988-29957

Every 600 ms tops up random and spot mob generators per map (level-based monster tables, mob events). The `#ifdef DEF_CHECKUSERNPCNUM` user-count scaling is inactive, and even if it were on, `iResultNum = m_iMaximumObject - 30` is assigned for every map unconditionally (29030-29032), so the cap is never user-scaled. The `break` at 29038 (Middleland during crusade) leaves the whole map loop, so maps after Middleland stop generating (random and spot) while that holds. Spot generators: 1/3 chance per spot per 600 ms tick, need m_iMaximumObject > m_iTotalActiveObject (29813-29816). The mob-event flag set by SpecialEventHandler is consumed here (1 in 10 per map pass, 29550-29600) ([ENTITY-006](../../issues/entity.md#entity-006-mobgenerator-user-count-scaling-inactive-crusade-break-stops-generation-on-later-maps)).

- sig: `void CGame::MobGenerator()`
- callers: CGame::OnTimer
- callees: PutLogList, SendNotifyMsg, SetNamingValueEmpty, _cGetSpecialAbility, bCreateNewNpc, bSetNpcFollowMode, iDice, iGetEmptyNamingValue
- notifies: DEF_NOTIFY_MONSTEREVENT_POSITION
- switches: DEF_CHECKUSERNPCNUM
- Korean comments (UTF-8, first 6, untranslated quote): v1.432 월드 서버의 총 사용자 수에 비례하여 최대 오브젝트 수를 결정한다. 1000명 이상이면 100%  최소 50% / 사용자 수에 비례하여 랜덤 몹 갯수 보정. / v1.432 숫자 보정

## CGame::CalcNextWayPointDestination - SRV Game.cpp:29960-30031

Picks the next waypoint/random destination by move type. Quirks: RANDOMWAYPOINT uses `rand() % (total-1)` (division by zero with a single waypoint) and RANDOMAREA uses `rand() % range` (zero-width area divides by zero) (29982, 29990-29992) [V, review R2-21] ([ENTITY-007](../../issues/entity.md#entity-007-calcnextwaypointdestination-divides-by-zero)).

- sig: `void CGame::CalcNextWayPointDestination(int iNpcH)`
- callers: CGame::NpcBehavior_Move
- Korean comments (UTF-8, first 5, untranslated quote): 지정된 위치를 벗어나지 않으며 추격이 끝난 후에 돌아온다. / 완전 랜덤 / 갈 수 있는 좌표가 나올때 까지 30번 반복

## CGame::DeleteNpc - SRV Game.cpp:30033-30347

Removes an NPC at corpse removal (after m_dwRegenTime), not at death; non-summoned monsters may drop at most one item: a hard-coded per-type alchemy table (30117-30290) or an NpcItem drop via bGetItemNameWhenDeleteNpc; types 30/31/32 with no item give Gold (item 90) 1d(ExpDice*4)+ExpDice minus a third (30295-30302). Also adjusts guild teleport-loc counters for structures/war units (30073-30094), spot-mob counters (30058) and the crop sum (30098). The `_bInitItemAttr` callee here is the ID overload (30306) and `_bItemLog` the by-name form (30341).

- sig: `void CGame::DeleteNpc(int iNpcH)`
- callers: CGame::Effect_Damage_Spot, CGame::NpcBehavior_Dead, CGame::NpcProcess, CGame::RemoveCrusadeStructures, CGame::iCalculateAttackEffect
- callees: ClearOwner, RemoveFromTarget, SendEventToNearClient_TypeA, SendEventToNearClient_TypeB, SetNamingValueEmpty, _bInitItemAttr, _bItemLog, bGetItemNameWhenDeleteNpc, bRemoveCropsTotalSum, bRemoveCrusadeStructureInfo, bRemoveFromDelayEventList, bSetItem, iDice
- messages: MSGID_EVENT_COMMON, MSGID_EVENT_LOG
- types: DEF_COMMONTYPE_ITEMDROP, DEF_MSGTYPE_REJECT
- switches: DEF_LOGTIME
- Korean comments (UTF-8, first 6, untranslated quote): 죽어있던 위치에서 삭제 / NPC의 NamigValue를 얻어와 사용중인 표시를 해지한다. / NamingValue를 비우고 동작중인 개체 수를 감소시킨다.

## CGame::NpcBehavior_Stop - SRV Game.cpp:30550-30624

Stationary NPCs: only m_cActionLimit == 5 is handled: type 38 mana collector, 39 detector, 41 grand magic generator (each every 3rd turn), 42 mana stone (m_iV1 += 5, cap 5), 40 nothing, default (towers 36/37) runs TargetSearch and switches to ATTACK (30563-30620); the attack itself happens in NpcBehavior_Attack. Limits 2/3 (shopkeepers) fall through and only increment m_sBehaviorTurnCount; there is no idle animation.

- sig: `void CGame::NpcBehavior_Stop(int iNpcH)`
- callers: CGame::NpcProcess
- callees: SendEventToNearClient_TypeA, TargetSearch, _NpcBehavior_GrandMagicGenerator, _bNpcBehavior_Detector, _bNpcBehavior_ManaCollector
- messages: MSGID_EVENT_MOTION
- types: DEF_OBJECTATTACK
- Korean comments (UTF-8, first 6, untranslated quote): STOP한 NPC는 대장장이같이 전혀 움직이지 않으며 가끔 제자리에서 다른 동작을 하는 것들이다. / m_cActionLimit 가 2, 3, 5인 NPC들이 이런 속성을 갖는다. / 턴 카운트 증가.

## CGame::iGetFollowerNumber - SRV Game.cpp:32975-32989

Number of NPCs following an owner.

- sig: `int CGame::iGetFollowerNumber(short sOwnerH, char cOwnerType)`
- callers: CGame::PlayerMagicHandler

## CGame::bSetNpcAttackMode - SRV Game.cpp:35934-35972

Sets a named NPC's target (optionally permanent). Declared `void` despite the `b` prefix. The NPC is found with memcmp(m_cName, name, 5) (first 5 bytes only) and iIndex is uninitialised ([ENTITY-003](../../issues/entity.md#entity-003-bsetnpcfollowmode-and-bsetnpcattackmode-use-an-uninitialised-index)).

- sig: `void CGame::bSetNpcAttackMode(char * cName, int iTargetH, char cTargetType, BOOL bIsPermAttack)`
- callers: CGame::AdminOrder_CallGuard, CGame::bAnalyzeCriminalAction
- Korean comments (UTF-8, first 3, untranslated quote): 같은 이름을 가진 NPC가 없다. / 목표물을 할당한다. / 영구 공격 모드 여부 설정

## CGame::NpcRequestAssistance - SRV Game.cpp:38068-38096

Calls ONE idle same-side NPC nearby to attack: scans 17x17 (sX/sY +-8), matches m_cSide equal (not type, 38084), only NPCs in DEF_BEHAVIOR_MOVE that are not perm-attack, and `return`s after the first one found (38093).

- sig: `void CGame::NpcRequestAssistance(int iNpcH)`
- callers: CGame::NpcBehavior_Flee
- callees: GetOwner
- Korean comments (UTF-8, first 2, untranslated quote): iNpc 근방에 있는 전투중이 아닌 NPC에게 도움을 요청한다. / 조건에 부합하는 NPC를 찾았다.

## CGame::SetSummonMobAction - SRV Game.cpp:39893-39976

Summoned-mob control: free / hold / target.

- sig: `void CGame::SetSummonMobAction(int iClientH, int iMode, DWORD dwMsgSize, char *pData)`
- callers: CGame::ChatMsgHandler
- callees: CStrTok, pGet
- Korean comments (UTF-8, first 6, untranslated quote): v2.1 중립은 몹공격 명령을 할 수 없다. / Free 모드 / Hold 모드

## CGame::_cGetSpecialAbility - SRV Game.cpp:44129-44207

Random special ability for a mob. Leaves `cSA` uninitialised for kinds outside 1-9 (44129-44207) ([ENTITY-011](../../issues/entity.md#entity-011-_cgetspecialability-leaves-the-result-uninitialised-for-some-kinds)).

- sig: `char CGame::_cGetSpecialAbility(int iKindSA)`
- callers: CGame::MobGenerator
- callees: iDice

## CGame::_TamingHandler - SRV Game.cpp:46085-46142

Taming skill: a stub. It computes range = skill/12 (46095), scans the square, maps each NPC type to a taming level (46112-46131) and compares skill/10 to it, but both outcomes of the test end in `break` (46136-46138); no state is changed, so nothing is tamed ([ENTITY-001](../../issues/entity.md#entity-001-taming-is-a-stub-and-unreachable-on-land)).

- sig: `void CGame::_TamingHandler(int iClientH, int iSkillNum, char cMapIndex, int dX, int dY)`
- callers: CGame::iCalculateUseSkillItemEffect
- callees: GetOwner
- Korean comments (UTF-8, first 6, untranslated quote): 최대 범위는 8 / 몬스터의 종류에 따라 길들여질 것인지를 결정한다. / 슬라임

## CGame::bSerchMaster - SRV Game.cpp:51778-51809

Finds a master mob of the same type for followers. DEAD in effect: its only call site is commented out (Game.cpp:11606 `//bSerchMaster(iNpcH);`) ([ENTITY-008](../../issues/entity.md#entity-008-bserchmaster-masterfollower-packs-is-dead)).

- sig: `BOOL CGame::bSerchMaster(int iNpcH)`
- callees: GetOwner
- Korean comments (UTF-8, first 2, untranslated quote): 인덱스 iNpcH에 해당하는 NPC가 Master를 찾는다. / 타입이 같고 랜덤하게 움직이고 있는 몹이 있었다면

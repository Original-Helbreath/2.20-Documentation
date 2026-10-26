# Combat, damage, death/PK and magic

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
`_bItemLog` callees in this doc sit inside inactive DEF_TAIWANLOG blocks (e.g. iCalculateAttackEffect 13733, RequestStudyMagicHandler 24961) and so are not compiled [V, review G3].
`cfg:` means data loaded from that config (often via the Log server), not a literal file open in the body [V, review G1].
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 38

## CGame::iClientMotion_Attack_Handler - SRV Game.cpp:11285-11432

Validates an attack (position, speed check: 7 msgs in < 80*8*7-3000 = 1480 ms), resolves target by object id, applies dash, runs iCalculateAttackEffect, and responds. Calls _CheckMiningAction when the target tile is empty (11402) and updates the sector activity counters.

- sig: `int CGame::iClientMotion_Attack_Handler(int iClientH, short sX, short sY, short dX, short dY, short wType, char cDir, WORD wTargetObjectID, BOOL bResponse, BOOL bIsDash)`
- callers: CGame::ClientMotionHandler
- callees: ClearOwner, ClearSkillUsingStatus, DeleteClient, GetExp, GetOwner, SetOwner, _CheckMiningAction, iCalculateAttackEffect, iSendMsg
- messages: MSGID_RESPONSE_MOTION
- types: DEF_OBJECTMOTION_ATTACK_CONFIRM
- switches: NO_MSGSPEEDCHECK
- Korean comments (UTF-8, first 6, untranslated quote): 잘못된 데이터에 대해서는 응답하지 않는다. / 공격 메시지 7번 80*8*7 = 4480ms. 오차 한계 3000ms. / v1.4 tdX, tdY 좌표를 wObjectID로부터 추출한다. 단 일반 활 공격은 적용되지 않는다. 활 필살기는 적용된다.

## CGame::iCalculateAttackEffect - SRV Game.cpp:12306-14275

Core melee/missile resolution: attacker AP and hit ratio, PvP protections, target defense, hit chance clamp 15-99%, damage modifiers, armour/shield absorption by hit location, special abilities, armour wear and hammer strip, death handling, NPC counter-attack, weapon wear and skill gain; returns exp.

- sig: `int CGame::iCalculateAttackEffect(short sTargetH, char cTargetType, short sAttackerH, char cAttackerType, int tdX, int tdY, int iAttackMode, BOOL bNearAttack, BOOL bIsDash)`
- callers: CGame::NpcBehavior_Attack, CGame::iClientMotion_Attack_Handler
- callees: CalculateSSN_ItemIndex, CalculateSSN_SkillIndex, ClearOwner, ClearSkillUsingStatus, ClientKilledHandler, DeleteClient, DeleteNpc, GetExp, ItemDepleteHandler, NpcKilledHandler, PutLogList, ReleaseItemHandler, SendEventToNearClient_TypeA, SendNotifyMsg, SetIceFlag, SetInvisibilityFlag, SetOwner, _bItemLog, _iGetArrowItemIndex, _iGetWeaponSkillType, bAnalyzeCriminalAction, bCheckEnergySphereDestination, bCheckResistingPoisonSuccess, bCropsItemDrop, bGetMoveable, bProbabilityTable, bRegisterDelayEvent, bRemoveFromDelayEventList, iCalcTotalWeight, iDice ...
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_CONSTRUCTIONPOINT, DEF_NOTIFY_DAMAGEMOVE, DEF_NOTIFY_HP, DEF_NOTIFY_ITEMLIFESPANEND, DEF_NOTIFY_ITEMRELEASED, DEF_NOTIFY_MAGICEFFECTOFF, DEF_NOTIFY_MAGICEFFECTON, DEF_NOTIFY_SETITEMCOUNT, DEF_NOTIFY_SUPERATTACKLEFT
- types: DEF_OBJECTDAMAGE, DEF_OBJECTMOVE, DEF_OBJECTNULLACTION
- switches: DEF_ALLPLAYERPROTECT, DEF_BEGINNERTOWNPROTECT, DEF_SAMESIDETOWNPROTECT, DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 공격한 자의 Attack Point와 HitRatio를 얻는다. / 공격위치가 공격 불가능 맵이라면 공격 불가능 / 2002-11-14 사냥꾼 모드 추가 (사냥꾼은 다른 Player를 공격하지 않는다. 전면전시 제외)

## CGame::NpcKilledHandler - SRV Game.cpp:14449-14701

NPC death: dying broadcast, drops (NpcDeadItemGenerator), exp ExpTotal/3 + remaining, quest count, unicorn rating -5, crusade construction/war-contribution credit, explosive death magic.

- sig: `void CGame::NpcKilledHandler(short sAttackerH, char cAttackerType, int iNpcH, short sDamage)`
- callers: CGame::AdminOrder_UnsummonAll, CGame::AdminOrder_UnsummonBoss, CGame::DynamicObjectEffectProcessor, CGame::Effect_Damage_Spot, CGame::Effect_Damage_Spot_DamageMove, CGame::KillCrusadeObjects, CGame::NpcProcess, CGame::iCalculateAttackEffect
- callees: ClearOwner, GetExp, NpcDeadItemGenerator, NpcMagicHandler, PutLogEventFileList, PutLogList, ReleaseFollowMode, RemoveFromTarget, SendEventToNearClient_TypeA, SendNotifyMsg, SetDeadOwner, _bCheckIsQuestCompleted, bRemoveCropsTotalSum, bStockMsgToGateServer
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_CONSTRUCTIONPOINT
- types: DEF_OBJECTDYING
- cfg: Quest.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 맵의 살아있는 객체수를 줄인다. / 현재 이 NPC를 공격대상으로 삼고있는 객체들을 해제한다. / 현재 이 NPC를 따르고 있던 NPC들의 이동방식을 변경한다.

## CGame::ClientKilledHandler - SRV Game.cpp:19253-19521

Player death: marks dead, cancels exchange, broadcasts dying, decides PK / criminal kill / enemy kill and applies penalties by the victim's PK tier; war units credit construction points. Matrix: a neutral (traveller) victim -> innocent: ApplyPKpenalty on the killer, criminal: PK_KillRewardHandler. A citizen/guildsman victim killed by a traveller: innocent victim -> ApplyPKpenalty, criminal victim (PKCount > 0) -> nothing (empty branch 19359-19362); killed by same-side player: innocent -> ApplyPKpenalty, criminal -> PK_KillRewardHandler; other side: EnemyKillRewardHandler. A PLAYER_INDIRECT kill only logs (no penalty or reward).

- sig: `void CGame::ClientKilledHandler(int iClientH, int iAttackerH, char cAttackerType, short sDamage)`
- callers: CGame::CheckUniqueItemEquipment, CGame::ClientMotionHandler, CGame::DoMeteorStrikeDamageHandler, CGame::DynamicObjectEffectProcessor, CGame::Effect_Damage_Spot, CGame::Effect_Damage_Spot_DamageMove, CGame::iCalculateAttackEffect
- callees: ApplyCombatKilledPenalty, ApplyPKpenalty, ClearOwner, EnemyKillRewardHandler, PK_KillRewardHandler, PutLogList, RemoveFromTarget, SendEventToNearClient_TypeA, SendNotifyMsg, SetDeadOwner, _ClearExchangeStatus, _bPKLog, bStockMsgToGateServer
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_CONSTRUCTIONPOINT, DEF_NOTIFY_KILLED
- types: DEF_OBJECTDYING
- switches: DEF_LOCALNPCNAME
- Korean comments (UTF-8, first 6, untranslated quote): 사투장인 경우 시간을 입력한다. / 2002-7-4 사투장의 갯수를 늘릴 수 있도록 / HP는 0이다.

## CGame::iClientMotion_Magic_Handler - SRV Game.cpp:20773-20837

Validates the cast motion (position only; there is no frequency check here), clears skill use and invisibility, sends MSGID_RESPONSE_MOTION; the effect comes later via DEF_COMMONTYPE_MAGIC.

- sig: `int CGame::iClientMotion_Magic_Handler(int iClientH, short sX, short sY, char cDir)`
- callers: CGame::ClientMotionHandler
- callees: ClearOwner, ClearSkillUsingStatus, DeleteClient, SetInvisibilityFlag, SetOwner, bRemoveFromDelayEventList, iSendMsg
- messages: MSGID_RESPONSE_MOTION
- types: DEF_OBJECTMOTION_CONFIRM
- Korean comments (UTF-8, first 6, untranslated quote): 죄표가 일치하지 않는 명령은 무시한다. Reject메시지를 받기 이전에 전송된 메시지들이다. / v1.432-2 맵 섹터 정보를 갱신한다. / 스킬의 사용상태를 모두 무효화 한다.

## CGame::PlayerMagicHandler - SRV Game.cpp:20842-22814

Resolves a player spell: learned check, cast probability (Magery x circle table + Int bonus, level/circle penalty, weather, staff bonus), mana cost (safe mode +40%, mana-save), hit ratio for resistance, then the per-type effect (damage spot/area/linear, HP/SP up/down, teleport, summon, create item, protect, hold, invisibility, dynamic fields, possession, confuse, poison, berserk, polymorph, ice, tremor...).

- sig: `void CGame::PlayerMagicHandler(int iClientH, int dX, int dY, short sType, BOOL bItemEffect, int iV1)`
- callers: CGame::ClientCommonHandler, CGame::UseItemHandler
- callees: ArmorLifeDecrement, CalculateSSN_SkillIndex, DeleteClient, Effect_Damage_Spot, Effect_Damage_Spot_DamageMove, Effect_HpUp_Spot, Effect_SpDown_Spot, Effect_SpUp_Spot, GetDeadOwner, GetOwner, GetPoint2, RemoveFromTarget, RequestTeleportHandler, SendEventToNearClient_TypeA, SendEventToNearClient_TypeB, SendNotifyMsg, SetBerserkFlag, SetIceFlag, SetInvisibilityFlag, SetNamingValueEmpty, _bAddClientItemList, _bInitItemAttr, _bItemLog, _iCalcPlayerNum, bAnalyzeCriminalAction, bCheckResistingIceSuccess, bCheckResistingMagicSuccess, bCheckResistingPoisonSuccess, bCreateNewNpc, bGetIsMoveAllowedTile ...
- messages: MSGID_EVENT_COMMON, MSGID_EVENT_MOTION, MSGID_NOTIFY
- notifies: DEF_NOTIFY_CANNOTCARRYMOREITEM, DEF_NOTIFY_ITEMOBTAINED, DEF_NOTIFY_MAGICEFFECTOFF, DEF_NOTIFY_MAGICEFFECTON, DEF_NOTIFY_MP
- types: DEF_COMMONTYPE_ITEMDROP, DEF_COMMONTYPE_MAGIC, DEF_COMMONTYPE_SETITEM, DEF_OBJECTDAMAGE, DEF_OBJECTNULLACTION
- cfg: Magic.cfg
- switches: DEF_GUILDWARMODE, DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 길드전 모드일때는 특정 마법은 사용할 수 없다. / v2.12 배우지 않은 마법이라면 사용할 수 없다. / 마법 사용위치가 공격 불가능 맵이라면 캐스팅 불가능

## CGame::NpcMagicHandler - SRV Game.cpp:22817-23247

NPC spell resolution with the NPC's magic hit ratio and weather bonus.

- sig: `void CGame::NpcMagicHandler(int iNpcH, short dX, short dY, short sType)`
- callers: CGame::NpcBehavior_Attack, CGame::NpcKilledHandler
- callees: Effect_Damage_Spot, Effect_Damage_Spot_DamageMove, Effect_HpUp_Spot, Effect_SpDown_Spot, Effect_SpUp_Spot, GetDeadOwner, GetOwner, GetPoint2, RemoveFromTarget, SendEventToNearClient_TypeB, SendNotifyMsg, SetIceFlag, SetInvisibilityFlag, bCheckResistingIceSuccess, bCheckResistingMagicSuccess, bRegisterDelayEvent, bRemoveFromDelayEventList, iGetWhetherMagicBonusEffect
- messages: MSGID_EVENT_COMMON
- notifies: DEF_NOTIFY_MAGICEFFECTON
- types: DEF_COMMONTYPE_MAGIC
- cfg: Magic.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 공격위치가 공격 불가능 맵이라면 공격 불가능 / 몬스터 별 마법 명중률 입력 / 날씨에 의한 마법 공격력 조정

## CGame::_bDecodeMagicConfigFileContents - SRV Game.cpp:24173-24483

Parses Magic.cfg pushed by the Log server (MSGID_MAGICCONFIGURATIONCONTENTS) into m_pMagicConfigList[100].

- sig: `BOOL CGame::_bDecodeMagicConfigFileContents(char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: CStrTok, PutLogList, _bGetIsStringIsNumber, pGet
- cfg: Magic.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 마법 번호 / 이미 할당된 번호가 있다. 에러이다. / 마법 이름

## CGame::RequestStudyMagicHandler - SRV Game.cpp:24898-25015

Learns a spell (scroll or wizard tower purchase): checks Int requirement, gold, tower location (v2.20).

- sig: `void CGame::RequestStudyMagicHandler(int iClientH, char * pName, BOOL bIsPurchase)`
- callers: CGame::ClientCommonHandler, CGame::UseItemHandler
- callees: DeleteClient, SetItemCount, _bItemLog, _iGetMagicNumber, dwGetItemCount, iCalcTotalWeight, iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_MAGICSTUDYFAIL, DEF_NOTIFY_MAGICSTUDYSUCCESS
- cfg: Magic.cfg
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 마법을 배운다. / 이런 이름의 마법은 존재하지 않는다. 무시한다. / 일반적으로 배울수 없는 마법이라면(가격이 음수) 배울 수 없다.

## CGame::_iGetMagicNumber - SRV Game.cpp:25363-25383

Spell name -> index, returning Int requirement and gold cost.

- sig: `int CGame::_iGetMagicNumber(char * pMagicName, int * pReqInt, int * pCost)`
- callers: CGame::RequestStudyMagicHandler
- cfg: Magic.cfg
- Korean comments (UTF-8, first 1, untranslated quote): 같은 이름을 가진 마법 설정을 찾았다. 마법 번호를 반환한다.

## CGame::ApplyPKpenalty - SRV Game.cpp:27986-28055

PK: returns early if the attacker is in safe-attack mode with PKCount == 0 (27994); else PKCount+1, exp loss = iDice(victimLvl/2+1, 50) + iDice(attackerLvl/2+1, 50) (two dice sums, not 1d50 x factor), rating -10, city crimes+1; jailed in arejail/elvjail (locked-map time 60*3 units, minus 3 per 3 s tick = 180 s) only when the PK happened in the attacker's own city (the `#ifndef DEF_PKPENALTY` path is the active one; with DEF_PKPENALTY it is always).

- sig: `void CGame::ApplyPKpenalty(short sAttackerH, short sVictumH)`
- callers: CGame::ClientKilledHandler
- callees: RequestTeleportHandler, SendEventToNearClient_TypeA, SendNotifyMsg, _bPKLog, iDice
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_PKPENALTY
- types: DEF_OBJECTNULLACTION
- switches: DEF_PKPENALTY
- Korean comments (UTF-8, first 6, untranslated quote): 안전 공격 모드가 켜진 상태에서 공격으로 인해 같은 편이 죽을수는 없지만 만약 있을경우 무시 / 단 공격자가 범죄자가 아니어야만 해당된다. / PK Count 증가

## CGame::PK_KillRewardHandler - SRV Game.cpp:28058-28081

Killing a criminal: only if the attacker's PKCount == 0, reward gold += iGetExpLevel(victim exp) x 3 (not m_iLevel), capped at DEF_MAXREWARDGOLD (no exp).

- sig: `void CGame::PK_KillRewardHandler(short sAttackerH, short sVictumH)`
- callers: CGame::ClientKilledHandler
- callees: SendNotifyMsg, _bPKLog, iGetExpLevel
- notifies: DEF_NOTIFY_PKCAPTURED
- Korean comments (UTF-8, first 2, untranslated quote): PK를 잡은 사람이 PK면 아무런 득이 없다. / 포상금만 누적. 경험치는 오르지 않는다.

## CGame::EnemyKillRewardHandler - SRV Game.cpp:28083-28261

Enemy kill: reward only if the attacker's PKCount == 0; exp (3d(3*L)+L)/3 with L = iGetExpLevel(victim exp), EK+1 if that level >= 30 (80 if attacker >= 80) and the victim's location differs from the map (not own town), reward gold 1dL; crusade adds war contribution x5 and construction points = victim m_iLevel/2.

- sig: `void CGame::EnemyKillRewardHandler(int iAttackerH, int iClientH)`
- callers: CGame::ClientKilledHandler
- callees: PutLogList, SendNotifyMsg, _bPKLog, bCheckLevelUp, bCheckLimitedUser, iDice, iGetExpLevel
- notifies: DEF_NOTIFY_CONSTRUCTIONPOINT, DEF_NOTIFY_ENEMYKILLREWARD, DEF_NOTIFY_EXP
- Korean comments (UTF-8, first 6, untranslated quote): 최고렙인 경우 무조건 레벨 80 이상을 잡아야 한다. / 희생자의 레벨이 80이상이고 / 희생자가 죽은 곳이 자신의 마을이 아니라면 EK로 인정

## CGame::ApplyCombatKilledPenalty - SRV Game.cpp:28263-28335

Death penalty: PKCount-1; exp loss 1d(5 x tier x level) (neutral /3); item drops by tier (level < 80 drops one fewer) only for m_bIsNeutral != TRUE; in crusade PKCount is still decremented and the function returns (no exp loss, no drops). The bItemDrop argument is unused while DEF_ITEMDROP is off.

- sig: `void CGame::ApplyCombatKilledPenalty(int iClientH, char cPenaltyLevel, BOOL bIsSAattacked, BOOL bItemDrop)`
- callers: CGame::ClientKilledHandler
- callees: SendNotifyMsg, _PenaltyItemDrop, _bPKLog, iDice
- notifies: DEF_NOTIFY_EXP, DEF_NOTIFY_PKPENALTY
- switches: DEF_ITEMDROP
- Korean comments (UTF-8, first 6, untranslated quote): 크루세이드 모드인 경우 전쟁중에 죽어도 아무런 페널티가 없다. / PKcount만 감소 / PKcount 감소

## CGame::_PenaltyItemDrop - SRV Game.cpp:28338-28436

Drops N random inventory items (Stone of Sacrifice replaces all drops; owner-bound, special-ability (unless SA-killed) and lucky 10% are skipped).

- sig: `void CGame::_PenaltyItemDrop(int iClientH, int iTotal, BOOL bIsSAattacked , BOOL bItemDrop)`
- callers: CGame::ApplyCombatKilledPenalty
- callees: DropItemHandler, PutLogFileList, iDice
- switches: DEF_ITEMDROP
- Korean comments (UTF-8, first 6, untranslated quote): v2.04 스톤 오브 새크리파이스가 맞는지 확인 / 대체적으로 떨어지는 아이템이 있다면 다른 아이템이 떨어지지 않고 이 아이템만 떨어진다. / 다시 검색

## CGame::Effect_Damage_Spot - SRV Game.cpp:31094-31644

Spell damage to one target: dice + Mag/3.3 % bonus + AddMagicalDamage, fightzone (+1/3) and crusade multipliers (the crusade multiplier applies only to a crusade SOLDIER, m_iCrusadeDuty == 1, hitting a player: level <= 80 x1.7, <= 100 x1.5, else x1.33), PvP protections, elemental absorption, ring/amulet damage save, AddAbsMD, protection, death handling.

- sig: `void CGame::Effect_Damage_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3, BOOL bExp, int iAttr)`
- callers: CGame::NpcMagicHandler, CGame::PlayerMagicHandler
- callees: ClearOwner, ClearSkillUsingStatus, ClientKilledHandler, DeleteNpc, GetExp, ItemDepleteHandler, NpcKilledHandler, SendEventToNearClient_TypeA, SendNotifyMsg, SetOwner, bCheckEnergySphereDestination, bGetMoveable, bRemoveFromDelayEventList, iDice, iGetAttribute, iGetPlayerRelationship
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_HP, DEF_NOTIFY_MAGICEFFECTOFF, DEF_NOTIFY_SUPERATTACKLEFT
- types: DEF_OBJECTDAMAGE, DEF_OBJECTMOVE
- switches: DEF_ALLPLAYERPROTECT, DEF_BEGINNERTOWNPROTECT, DEF_SAMESIDETOWNPROTECT
- Korean comments (UTF-8, first 6, untranslated quote): 해당 타켓에게 대미지를 먹인다. / 공격자가 플레이어라면 Mag에 따른 보너스 대미지를 가산 / 2002-11-14 사냥꾼 모드 추가 (사냥꾼은 다른 Player를 공격하지 않는다. 전면전시 제외)

## CGame::Effect_Damage_Spot_DamageMove - SRV Game.cpp:31646-32159

As Effect_Damage_Spot but also knocks the target back.

- sig: `void CGame::Effect_Damage_Spot_DamageMove(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sAtkX, short sAtkY, short sV1, short sV2, short sV3, BOOL bExp, int iAttr)`
- callers: CGame::NpcMagicHandler, CGame::PlayerMagicHandler
- callees: ClearOwner, ClearSkillUsingStatus, ClientKilledHandler, GetExp, ItemDepleteHandler, NpcKilledHandler, SendEventToNearClient_TypeA, SendNotifyMsg, SetOwner, bRemoveFromDelayEventList, iDice, iGetAttribute, iGetPlayerRelationship
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_DAMAGEMOVE, DEF_NOTIFY_HP, DEF_NOTIFY_MAGICEFFECTOFF, DEF_NOTIFY_SUPERATTACKLEFT
- types: DEF_OBJECTDAMAGE
- switches: DEF_ALLPLAYERPROTECT, DEF_BEGINNERTOWNPROTECT, DEF_SAMESIDETOWNPROTECT
- Korean comments (UTF-8, first 6, untranslated quote): 해당 타켓에게 대미지를 먹인다. / 공격자가 플레이어라면 Mag에 따른 보너스 대미지를 가산 / 2002-11-14 사냥꾼 모드 추가 (사냥꾼은 다른 Player를 공격하지 않는다. 전면전시 제외)

## CGame::Effect_HpUp_Spot - SRV Game.cpp:32161-32207

Heal spell (respects MaxHPdown cap).

- sig: `void CGame::Effect_HpUp_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3)`
- callers: CGame::NpcMagicHandler, CGame::PlayerMagicHandler
- callees: SendNotifyMsg, iDice
- notifies: DEF_NOTIFY_HP
- Korean comments (UTF-8, first 2, untranslated quote): 해당 타켓의 HP를 올린다. / v1.432 Heal로도 블러드 소드류 HP 최대치 제한은 적용됨

## CGame::Effect_SpDown_Spot - SRV Game.cpp:32209-32264

Stamina drain spell.

- sig: `void CGame::Effect_SpDown_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3)`
- callers: CGame::NpcMagicHandler, CGame::PlayerMagicHandler
- callees: SendNotifyMsg, iDice, iGetPlayerRelationship
- notifies: DEF_NOTIFY_SP
- Korean comments (UTF-8, first 6, untranslated quote): 해당 타켓의 Sp를 내린다. / v2.13 만약 공격자가 플레이어이고 공격자가 안전 공격 모드라면 공격의 의미가 없다. / 적

## CGame::Effect_SpUp_Spot - SRV Game.cpp:32267-32298

Stamina restore spell.

- sig: `void CGame::Effect_SpUp_Spot(short sAttackerH, char cAttackerType, short sTargetH, char cTargetType, short sV1, short sV2, short sV3)`
- callers: CGame::NpcMagicHandler, CGame::PlayerMagicHandler
- callees: SendNotifyMsg, iDice
- notifies: DEF_NOTIFY_SP
- Korean comments (UTF-8, first 2, untranslated quote): 해당 타켓의 Sp를 올린다. / NPC는 스태미너 개념이 없다.

## CGame::bCheckResistingMagicSuccess - SRV Game.cpp:32301-32367

Magic resist roll: hit/resist*50 clamped 15-99; resist = MagicResist skill + Mag-50 + item bonuses; protection type 2/5 blocks; 9th+ circle bypasses type 2.

- sig: `BOOL CGame::bCheckResistingMagicSuccess(char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio)`
- callers: CGame::DynamicObjectEffectProcessor, CGame::NpcMagicHandler, CGame::PlayerMagicHandler
- callees: CalculateSSN_SkillIndex, iDice
- Korean comments (UTF-8, first 6, untranslated quote): v1.4 운영자에 대한 마법 공격은 무의미 / v2.13 운영자 레벨1은 공격이 가능하다. / v2.19 2002-12-7 산타복 관련

## CGame::bCheckResistingIceSuccess - SRV Game.cpp:32370-32406

Freeze resist: 1d100 <= 2*AddAbsWater (players), water NPCs immune, 30 s immunity after a warm potion.

- sig: `BOOL CGame::bCheckResistingIceSuccess(char cAttackerDir, short sTargetH, char cTargetType, int iHitRatio)`
- callers: CGame::DynamicObjectEffectProcessor, CGame::NpcBehavior_Attack, CGame::NpcMagicHandler, CGame::PlayerMagicHandler
- callees: iDice
- Korean comments (UTF-8, first 5, untranslated quote): 냉동될 것인가의 확률 계산. / v1.4 운영자에 대한 냉동공격은 무의미 / v2.172 해동 포션을 먹고 30초간은 얼지 않는다.

## CGame::SetInvisibilityFlag - SRV Game.cpp:33165-33186

Sets/clears the invisibility status bit and broadcasts.

- sig: `void CGame::SetInvisibilityFlag(short sOwnerH, char cOwnerType, BOOL bStatus)`
- callers: CGame::AdminOrder_SetInvi, CGame::DelayEventProcessor, CGame::InitPlayerData, CGame::NpcMagicHandler, CGame::PlayerMagicHandler, CGame::UseItemHandler, CGame::UseSkillHandler, CGame::_CheckMiningAction, CGame::_bNpcBehavior_Detector, CGame::iCalculateAttackEffect, CGame::iClientMotion_Magic_Handler
- callees: SendEventToNearClient_TypeA
- messages: MSGID_EVENT_MOTION
- types: DEF_OBJECTNULLACTION
- Korean comments (UTF-8, first 2, untranslated quote): 외형이 바뀐것을 통보 / 외형이 바뀐것을 통보

## CGame::SetBerserkFlag - SRV Game.cpp:33188-33209

Sets/clears the berserk status bit and broadcasts.

- sig: `void CGame::SetBerserkFlag(short sOwnerH, char cOwnerType, BOOL bStatus)`
- callers: CGame::DelayEventProcessor, CGame::PlayerMagicHandler
- callees: SendEventToNearClient_TypeA
- messages: MSGID_EVENT_MOTION
- types: DEF_OBJECTNULLACTION
- Korean comments (UTF-8, first 2, untranslated quote): 외형이 바뀐것을 통보 / 외형이 바뀐것을 통보

## CGame::SetIceFlag - SRV Game.cpp:33212-33233

Sets/clears the frozen status bit and broadcasts.

- sig: `void CGame::SetIceFlag(short sOwnerH, char cOwnerType, BOOL bStatus)`
- callers: CGame::DelayEventProcessor, CGame::DynamicObjectEffectProcessor, CGame::NpcBehavior_Attack, CGame::NpcMagicHandler, CGame::PlayerMagicHandler, CGame::iCalculateAttackEffect
- callees: SendEventToNearClient_TypeA
- messages: MSGID_EVENT_MOTION
- types: DEF_OBJECTNULLACTION
- Korean comments (UTF-8, first 2, untranslated quote): 외형이 바뀐것을 통보 / 외형이 바뀐것을 통보

## CGame::bAnalyzeCriminalAction - SRV Game.cpp:35838-35896

Attacking a non-hostile player spawns a guard that attacks the attacker; guard names exist only for maps aresden / elvine / default (else 'XXX', no spawn); returns FALSE in crusade. Callers: melee only when the hit KILLS (13744-13745); magic from PlayerMagicHandler (22331 etc.).

- sig: `BOOL CGame::bAnalyzeCriminalAction(int iClientH, short dX, short dY, BOOL bIsCheck)`
- callers: CGame::PlayerMagicHandler, CGame::iCalculateAttackEffect
- callees: GetOwner, SetNamingValueEmpty, _bGetIsPlayerHostile, bCreateNewNpc, bSetNpcAttackMode, iGetEmptyNamingValue
- Korean comments (UTF-8, first 6, untranslated quote): dX, dY에 대한 지속적 위해 행위가 범죄에 해당하는지 판단한다. / 범죄행위다. 마을에 있었다면 소환 가드로 공격시킨다. / <- 이것은 마을이 아니므로 NPC를 생성하지 못하게 하기 위함

## CGame::_bGetIsPlayerHostile - SRV Game.cpp:35898-35932

Whether attacking a player is legal (enemy side, or target is criminal).

- sig: `BOOL CGame::_bGetIsPlayerHostile(int iClientH, int sOwnerH)`
- callers: CGame::bAnalyzeCriminalAction
- Korean comments (UTF-8, first 5, untranslated quote): 자기 자신에 대한 공격은 범죄라 볼 수 없다. / 공격자는 여행자이다. / 상대방이 범죄자이면 적성이며 그 이외에는 아니다.

## CGame::iGetWhetherMagicBonusEffect - SRV Game.cpp:37465-37495

Weather modifier per spell in weather 1-3: spells 10, 37, 43, 51 get +1; spells 20, 30 get -1. The code comment says electric up / fire down, but the spell-to-element mapping is not confirmed from this function alone [I] (later electric and fire spells are absent from the table, [MAGIC-002](../../issues/magic.md#magic-002-weather-bonus-table-is-incomplete)).

- sig: `int CGame::iGetWhetherMagicBonusEffect(short sType, char cWheatherStatus)`
- callers: CGame::NpcMagicHandler, CGame::PlayerMagicHandler
- Korean comments (UTF-8, first 1, untranslated quote): 전기계열 마법의 경우 비가 오면 보너스 타격치가 붙는다. 반면 파이어 계열 마법의 타격치는 낮아진다.

## CGame::iGetPlayerRelationship - SRV Game.cpp:37498-37556

Relationship code between two players: 0 neutral,1 friend,2 enemy,3 same guild,4 other guild same side,5 own guildmaster,6 PK of same town,7 I am criminal/same town.

- sig: `int CGame::iGetPlayerRelationship(int iClientH, int iOpponentH)`
- callers: CGame::Effect_Damage_Spot, CGame::Effect_Damage_Spot_DamageMove, CGame::Effect_SpDown_Spot, CGame::iCalculateAttackEffect
- Korean comments (UTF-8, first 6, untranslated quote): 본인이 범죄자이하면 상대방이 마을이 같은 경우 위협. 다르면 적 / 위협 / 적

## CGame::iGetPlayerABSStatus - SRV Game.cpp:37686-37732

Packs the 4-bit absolute status (side, criminal, hunter) sent with player events (2002-11-14 hunter mode). In a fightzone, a different guild (both ranked) sets bit 3 (criminal/enemy bit, 37705-37712).

- sig: `int CGame::iGetPlayerABSStatus(int iWhatH, int iRecvH)`
- callers: CGame::RequestFullObjectData, CGame::SendEventToNearClient_TypeA, CGame::iClientMotion_Move_Handler, CGame::iComposeInitMapData, CGame::iComposeMoveMapData
- Korean comments (UTF-8, first 6, untranslated quote): 2002-11-14 사냥꾼 모드 추가 / 2002-11-14 변형됨 / 4개의 bit에 따라 의미를 부여한다.

## CGame::iGetNpcRelationship - SRV Game.cpp:37772-37798

NPC side vs player: neutral, attack-all, Aresden, Elvine.

- sig: `int CGame::iGetNpcRelationship(int iWhatH, int iRecvH)`
- callers: CGame::RequestFullObjectData, CGame::SendEventToNearClient_TypeA, CGame::iComposeInitMapData, CGame::iComposeMoveMapData
- Korean comments (UTF-8, first 4, untranslated quote): 중립 / 모두를 공격하는 NPC / aresden 소속 (경비병, 상점들의 주인들)

## CGame::iGetPlayerRelationship_SendEvent - SRV Game.cpp:37837-37911

Relationship from the receiver's viewpoint, for event packets.

- sig: `int CGame::iGetPlayerRelationship_SendEvent(int iClientH, int iOpponentH)`
- Korean comments (UTF-8, first 6, untranslated quote): 여기서 사이드 계산하는 방법은 관점이 다르므로 주의한다. / 내 데이터를 작성하는 것이 아니라 다른 이에게 보내는 데이터를 작성하는 것임을 주의. / 내가 범죄자이하면 상대방에게는 마을이 다를 경우 적, 아니면 PK로 인식된다.

## CGame::iGetNpcRelationship_SendEvent - SRV Game.cpp:37914-37950

NPC relationship for event packets.

- sig: `int CGame::iGetNpcRelationship_SendEvent(int iNpcH, int iOpponentH)`
- Korean comments (UTF-8, first 6, untranslated quote): 여기서 사이드 계산하는 방법은 관점이 다르므로 주의한다. / 내(NPC) 데이터를 작성하는 것이 아니라 다른 이에게 보내는 데이터를 작성하는 것임을 주의. / 상대방이 범죄자이하면 편이 같으면 위협으로 아니면 NPC 적으로 인식된다.

## CGame::_CheckAttackType - SRV Game.cpp:38193-38258

Downgrades attack mode: no arrows / no bow / no critical left -> plain attack. A critical (super) attack mode also needs the related weapon skill mastery at 100 (mastery < 100 -> plain attack).

- sig: `void CGame::_CheckAttackType(int iClientH, short *spType)`
- callers: CGame::ClientMotionHandler
- Korean comments (UTF-8, first 6, untranslated quote): 화살이 없다면 화살 Effect는 넣지 않는다. / 활 공격인데 장착한 무기가 활이 아니다. 일반 공격으로 전환. / 맨손 필살기

## CGame::CheckFireBluring - SRV Game.cpp:39241-39265

Turns flammable floor items (coal, id 355) into a fire dynamic object.

- sig: `void CGame::CheckFireBluring(char cMapIndex, int sX, int sY)`
- callers: CGame::DynamicObjectEffectProcessor
- callees: SendEventToNearClient_TypeB, iAddDynamicObjectList, iCheckItem, pGetItem
- messages: MSGID_EVENT_COMMON
- types: DEF_COMMONTYPE_SETITEM
- Korean comments (UTF-8, first 2, untranslated quote): 만약 이 위치에 발화성 아이템이 있다면 지우고 불 오브젝트를 놓는다. / 석탄이다. 아이템을 지우고 불을 만든다.

## CGame::_iGetWeaponSkillType - SRV Game.cpp:39309-39350

Returns the skill index of the weapon class in hand (5 hand, 7 dagger, 8 long sword, 9 fencing, 10 axe, 14 hammer, 21 staff, 6 bow).

- sig: `int CGame::_iGetWeaponSkillType(int iClientH)`
- callers: CGame::iCalculateAttackEffect
- Korean comments (UTF-8, first 6, untranslated quote): 현재 무장한 무기의 스킬 번호를 반환 / 맨손공격 / 단검류

## CGame::iGetComboAttackBonus - SRV Game.cpp:39362-39394

Combo bonus damage per weapon skill and combo count 2..4 from the ___iCAB tables; count <= 1 returns 0. Skills 5-10 use their own tables; skill 14 (hammer) uses ___iCAB6 (zeros) and skill 21 (staff) uses ___iCAB10 (1,2,3) (39362-39394) ([COMBAT-007](../../issues/combat.md#combat-007-hammer-and-staff-combo-bonuses-borrow-other-weapons-tables)).

- sig: `int CGame::iGetComboAttackBonus(int iSkill, int iComboCount)`
- callers: CGame::iCalculateAttackEffect
- Korean comments (UTF-8, first 6, untranslated quote): 맨손 공격 / 활공격 / 단검 공격

## CGame::ActivateSpecialAbilityHandler - SRV Game.cpp:47113-47161

Activates the equipped weapon/armour special ability for its duration; schedules the cooldown (DEF_SPECABLTYTIMESEC 1200).

- sig: `void CGame::ActivateSpecialAbilityHandler(int iClientH)`
- callers: CGame::ClientCommonHandler
- callees: SendEventToNearClient_TypeA, SendNotifyMsg
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_SPECIALABILITYSTATUS
- types: DEF_OBJECTNULLACTION
- switches: DEF_GUILDWARMODE
- Korean comments (UTF-8, first 6, untranslated quote): 특수 능력을 사용했다. / 길드전 모드일때는 특수 능력 사용 불가 / 다음 사용 가능 시간을 입력한다.

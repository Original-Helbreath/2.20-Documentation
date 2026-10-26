# Items, shops, warehouse, exchange, crafting, potions, fishing, mining, farming, drops

Source: v2.20 HGserver export (commit 1697e41 [V: matches librehelbreath/HGserver 1697e41 "Imported HGServer 2.20 sources"; Game.cpp and Wmain.cpp byte-equal to git show]), files under `.claude/scratch/srv220/`.
Lines are `SRV <file>:<line>`. Purposes are [V] (read in code) unless tagged [I]/[U].
`_bItemLog` callees are compiled only where noted; many call sites sit inside inactive DEF_TAIWANLOG blocks (PurchaseItem 17089, both bSetItemToBankItem, ReqRepairItemCofirmHandler 34585 (typo'd `#ifdef TAIWANLOG`), BuildItemHandler 43755, TrainSkillResponse, ...) [V, review G3] ([GSO-011](../../issues/game-server-other.md#gso-011-misspelt-build-switches-leave-blocks-dead)).
`cfg:` means data loaded from that config, not a literal file open in the body [V, review G1].
Callers/callees come from a token scan (scratch `extract.py`), so overloaded names merge [I].
`inactive` = compiled out with the current GlobalDef.h; `DEAD` = inside a /* */ block.

Functions: 69

## CGame::_bDecodeItemConfigFileContents - SRV Game.cpp:9480-9854

Parses Item.cfg pushed by the Log server (MSGID_ITEMCONFIGURATIONCONTENTS) into m_pItemConfigList[5000].

- sig: `BOOL CGame::_bDecodeItemConfigFileContents(char * pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: CStrTok, PutLogList, _bGetIsStringIsNumber, pGet
- cfg: Item.cfg
- Korean comments (UTF-8, first 2, untranslated quote): 만약 이미 이런 아이 값을 가진 아이템이 정의되어 있다면 에러이다. / 이미 같은 고유번호를 가진 아이템이 정의되어 있다. 중복 정의 에러!

## CGame::_bInitItemAttr - SRV Game.cpp:9857-9908

Initialises an item from the config by name. Used by every caller except the ID-overload callers listed under the next entry (the list below is the merged name-scan list).

- sig: `BOOL CGame::_bInitItemAttr(class CItem * pItem, char * pItemName)`
- callers (merged with the ID overload [I]; name-overload callers = all except those named in the next entry): CGame::AdminOrder_CreateFish, CGame::AdminOrder_CreateItem, CGame::BuildItemHandler, CGame::CalcExpStock, CGame::CheckSpecialEvent, CGame::CheckSpecialEventThirdYear, CGame::ConfirmExchangeItem, CGame::DeleteNpc, CGame::DropItemHandler, CGame::FishGenerator, CGame::GetFightzoneTicketHandler, CGame::GetHeroMantleHandler, CGame::GetOccupyFlagHandler, CGame::GetRewardMoneyHandler, CGame::GiveItemHandler, CGame::NpcDeadItemGenerator, CGame::NpcDeadItemGeneratorWithItemEvent, CGame::OnStartGameSignal, CGame::PlayerMagicHandler, CGame::ReqCreatePortionHandler, CGame::ReqSellItemConfirmHandler, CGame::RequestItemUpgradeHandler, CGame::RequestPurchaseItemHandler, CGame::_CheckMiningAction, CGame::_bDecodeBuildItemConfigFileContents ...
- cfg: Item.cfg
- Korean comments (UTF-8, first 3, untranslated quote): 같은 이름을 가진 아이템 설정을 찾았다. 설정값을 복사한다. / 최초로 생성된 아이템의 수명은 최대치이다. / 아이템리스트를 모두 검색했음에도 발견되지 않았다.

## CGame::_bInitItemAttr - SRV Game.cpp:9911-9962

Initialises an item from the config by item id (v2.17).

- sig: `BOOL CGame::_bInitItemAttr(class CItem * pItem, int iItemID)`
- callers (ID overload only [V], Game.cpp call sites): PlayerMagicHandler 22103, DeleteNpc 30306, iCalculateUseSkillItemEffect 33776, AdminOrder_CreateFish 37041, FishGenerator 37348, _CheckMiningAction 39089, GetHeroMantleHandler 40480, NpcDeadItemGenerator 44921/45222, RequestItemUpgradeHandler (ids 736-738: 51904, 51950, 52336), bCropsItemDrop 53101. Merged name-scan list for reference: CGame::AdminOrder_CreateFish, CGame::AdminOrder_CreateItem, CGame::BuildItemHandler, CGame::CalcExpStock, CGame::CheckSpecialEvent, CGame::CheckSpecialEventThirdYear, CGame::ConfirmExchangeItem, CGame::DeleteNpc, CGame::DropItemHandler, CGame::FishGenerator, CGame::GetFightzoneTicketHandler, CGame::GetHeroMantleHandler, CGame::GetOccupyFlagHandler, CGame::GetRewardMoneyHandler, CGame::GiveItemHandler, CGame::NpcDeadItemGenerator, CGame::NpcDeadItemGeneratorWithItemEvent, CGame::OnStartGameSignal, CGame::PlayerMagicHandler, CGame::ReqCreatePortionHandler, CGame::ReqSellItemConfirmHandler, CGame::RequestItemUpgradeHandler, CGame::RequestPurchaseItemHandler, CGame::_CheckMiningAction, CGame::_bDecodeBuildItemConfigFileContents ...
- cfg: Item.cfg
- Korean comments (UTF-8, first 3, untranslated quote): 같은 이름을 가진 아이템 설정을 찾았다. 설정값을 복사한다. / 최초로 생성된 아이템의 수명은 최대치이다. / 아이템리스트를 모두 검색했음에도 발견되지 않았다.

## CGame::DropItemHandler - SRV Game.cpp:15597-15723

Drops an inventory item (or part of a stack) on the floor; logs DEF_ITEMLOG_DROP.

- sig: `void CGame::DropItemHandler(int iClientH, short sItemIndex, int iAmount, char * pItemName, BOOL bByPlayer)`
- callers: CGame::ClientCommonHandler, CGame::_PenaltyItemDrop
- callees: ReleaseItemHandler, SendEventToNearClient_TypeB, SendNotifyMsg, SetItemCount, _bInitItemAttr, _bItemLog, _iGetArrowItemIndex, bSetItem, iCalcTotalWeight
- messages: MSGID_EVENT_COMMON
- notifies: DEF_NOTIFY_DROPITEMFIN_COUNTCHANGED, DEF_NOTIFY_DROPITEMFIN_ERASEITEM, DEF_NOTIFY_ITEMRELEASED
- types: DEF_COMMONTYPE_ITEMDROP
- Korean comments (UTF-8, first 6, untranslated quote): // Amount가 -1이면 소모품일경우 전부를, 일반 아이템은 그냥 떨어뜨린다는 의미 / Amount가 -1이고 소모품이면 수량의 전부를 떨어뜨린다. / 아이템 이름이 일치하지 않아도 무시

## CGame::iClientMotion_GetItem_Handler - SRV Game.cpp:15725-15902

Picks up the top item of the tile if weight allows; MSGID_NOTIFY item obtained.

- sig: `int CGame::iClientMotion_GetItem_Handler(int iClientH, short sX, short sY, char cDir)`
- callers: CGame::ClientMotionHandler
- callees: ClearOwner, ClearSkillUsingStatus, DeleteClient, SendEventToNearClient_TypeB, SetOwner, _bAddClientItemList, _bItemLog, bSetItem, iSendMsg, pGetItem
- messages: MSGID_EVENT_COMMON, MSGID_NOTIFY, MSGID_RESPONSE_MOTION
- notifies: DEF_NOTIFY_CANNOTCARRYMOREITEM, DEF_NOTIFY_ITEMOBTAINED
- types: DEF_COMMONTYPE_SETITEM, DEF_OBJECTMOTION_CONFIRM
- Korean comments (UTF-8, first 6, untranslated quote): 죄표가 일치하지 않는 명령은 무시한다. Reject메시지를 받기 이전에 전송된 메시지들이다. / v1.432-2 맵 섹터 정보를 갱신한다. / 스킬의 사용상태를 모두 무효화 한다.

## CGame::_bAddClientItemList - SRV Game.cpp:15904-15962

Adds an item to inventory, merging stackables; checks weight.

- sig: `BOOL CGame::_bAddClientItemList(int iClientH, class CItem * pItem, int * pDelReq)`
- callers: CGame::AdminOrder_CreateItem, CGame::CheckSpecialEvent, CGame::CheckSpecialEventThirdYear, CGame::GetFightzoneTicketHandler, CGame::GetHeroMantleHandler, CGame::GetOccupyFlagHandler, CGame::GetRewardMoneyHandler, CGame::GiveItemHandler, CGame::PlayerMagicHandler, CGame::ReqCreatePortionHandler, CGame::ReqSellItemConfirmHandler, CGame::RequestPurchaseItemHandler, CGame::_iTalkToNpcResult_Cityhall, CGame::bAddItem, CGame::iClientMotion_GetItem_Handler
- callees: _iCalcMaxLoad, _iGetArrowItemIndex, iCalcTotalWeight, iGetItemWeight
- Korean comments (UTF-8, first 6, untranslated quote): 집을 아이템 중량계산 / 수량개념이 있는 아이템 / 수량 개념이 없는 아이템

## CGame::bEquipItemHandler - SRV Game.cpp:15965-16403

Equips an item: lifespan, level, gender, stat limits, slot conflicts (two-hand vs hands, full-body), appearance bits, weapon speed (DEF_WEAPONSPEEDLIMIT off).

- sig: `BOOL CGame::bEquipItemHandler(int iClientH, short sItemIndex, BOOL bNotify)`
- callers: CGame::ClientCommonHandler, CGame::_bDecodePlayerDatafileContents
- callees: CalcTotalItemEffect, ReleaseItemHandler, SendEventToNearClient_TypeA, SendNotifyMsg, iGetItemWeight
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_ITEMRELEASED
- types: DEF_OBJECTNULLACTION
- switches: DEF_KOREA, DEF_WEAPONSPEEDLIMIT
- Korean comments (UTF-8, first 6, untranslated quote): 만약 장착하고자 하는 아이템의 수명이 0인 경우는 장착할 수 없다. / v1.4334 제작 아이템이 아닌 경우  레벨 제한에 걸려도 장착할 수 없다. / 성별 제한에 걸려도 장착할 수 없다.

## CGame::RequestPurchaseItemHandler - SRV Game.cpp:16820-17092

Shop purchase: own-town/shop check, price (crusade winner -10%), Charisma discount (Cha-10)/4 %, floor at half price, gold deduction.

- sig: `void CGame::RequestPurchaseItemHandler(int iClientH, char * pItemName, int iNum)`
- callers: CGame::ClientCommonHandler
- callees: DeleteClient, SetItemCount, _bAddClientItemList, _bInitItemAttr, _bItemLog, dwGetItemCount, iCalcTotalWeight, iDice, iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_CANNOTCARRYMOREITEM, DEF_NOTIFY_ITEMPURCHASED, DEF_NOTIFY_NOTENOUGHGOLD
- switches: DEF_LOGTIME, DEF_TAIWANLOG, DEF_V219
- Korean comments (UTF-8, first 6, untranslated quote): 만약 아이템을 구입하고자 하는 곳이 자신의 마을이 아니라면 구입할 수 없다. / v2.181 2002-10-24 서버 속도 향상을 위해 스트링 비교를 줄인다. / 2002-11-15 사고 팔수 있는 곳인지 체크하는 변수 추가

## CGame::GiveItemHandler - SRV Game.cpp:17094-17690

Gives an item to a player or NPC (guild join/dismiss tokens, quest items, sell-to-NPC), splitting stacks.

- sig: `void CGame::GiveItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, WORD wObjectID, char * pItemName)`
- callers: CGame::ClientCommonHandler
- callees: DeleteClient, GetOwner, PutLogList, ReleaseItemHandler, SendEventToNearClient_TypeA, SendEventToNearClient_TypeB, SendNotifyMsg, SetItemCount, _bAddClientItemList, _bInitItemAttr, _bItemLog, _iGetArrowItemIndex, bSetItem, bSetItemToBankItem, iCalcTotalWeight, iSendMsg
- messages: MSGID_EVENT_COMMON, MSGID_EVENT_MOTION, MSGID_NOTIFY
- notifies: DEF_NOTIFY_CANNOTCARRYMOREITEM, DEF_NOTIFY_CANNOTGIVEITEM, DEF_NOTIFY_CANNOTITEMTOBANK, DEF_NOTIFY_DROPITEMFIN_ERASEITEM, DEF_NOTIFY_EXP, DEF_NOTIFY_GIVEITEMFIN_COUNTCHANGED, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM, DEF_NOTIFY_ITEMOBTAINED, DEF_NOTIFY_QUERY_DISMISSGUILDREQPERMISSION, DEF_NOTIFY_QUERY_JOINGUILDREQPERMISSION
- types: DEF_COMMONTYPE_DISMISSGUILDAPPROVE, DEF_COMMONTYPE_ITEMDROP, DEF_OBJECTNULLACTION
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 아이템 이름이 일치하지 않아도 무시된다. / 소비성 아이템이었고 수량만큼 감소시키고 남은 게 있다면 / 분할하고자 하는 아이템이 리스트에 없는 거다. 이런 일은 일어날 수가 없지만

## CGame::dwGetItemCount - SRV Game.cpp:19178-19195

Count of the first inventory stack with that name (0 if none); not a sum over stacks (callers rely on a single Gold stack) [V, review R2-09].

- sig: `DWORD CGame::dwGetItemCount(int iClientH, char * pName)`
- callers: CGame::FightzoneReserveHandler, CGame::ReqRepairItemCofirmHandler, CGame::RequestChargedTeleportHandler, CGame::RequestCreateNewGuildHandler, CGame::RequestPurchaseItemHandler, CGame::RequestStudyMagicHandler, CGame::ResponseCreateNewGuildHandler

## CGame::SetItemCount - SRV Game.cpp:19197-19227

Sets the count of an item by name; deletes it at 0 and notifies.

- sig: `int CGame::SetItemCount(int iClientH, char * pItemName, DWORD dwCount)`
- callers (name overload only [V]): ResponseCreateNewGuildHandler 16585, RequestPurchaseItemHandler 17042, RequestChargedTeleportHandler 24104, RequestStudyMagicHandler 24935, FightzoneReserveHandler 27479, ReqRepairItemCofirmHandler 34590. Merged name-scan list for reference: CGame::BuildItemHandler, CGame::ConfirmExchangeItem, CGame::DropItemHandler, CGame::FightzoneReserveHandler, CGame::GiveItemHandler, CGame::ReqCreatePortionHandler, CGame::ReqRepairItemCofirmHandler, CGame::ReqSellItemConfirmHandler, CGame::RequestChargedTeleportHandler, CGame::RequestPurchaseItemHandler, CGame::RequestRetrieveItemHandler, CGame::RequestStudyMagicHandler, CGame::ResponseCreateNewGuildHandler
- callees: ItemDepleteHandler, SendNotifyMsg, iGetItemWeight
- notifies: DEF_NOTIFY_SETITEMCOUNT
- Korean comments (UTF-8, first 2, untranslated quote): 카운트가 0이면 모두 소모된 것이므로 리스트에서 삭제한다. / 아이템의 수량이 변경되었음을 알린다.

## CGame::SetItemCount - SRV Game.cpp:19230-19250

Sets the count of an item by inventory index.

- sig: `int CGame::SetItemCount(int iClientH, int iItemIndex, DWORD dwCount)`
- callers (index overload only [V]): DropItemHandler 15651, GiveItemHandler 17138, RequestRetrieveItemHandler 27695, ReqSellItemConfirmHandler 34265/34293, ReqCreatePortionHandler 38473, ConfirmExchangeItem 41128/41151, BuildItemHandler 43772/43862. Merged name-scan list for reference: CGame::BuildItemHandler, CGame::ConfirmExchangeItem, CGame::DropItemHandler, CGame::FightzoneReserveHandler, CGame::GiveItemHandler, CGame::ReqCreatePortionHandler, CGame::ReqRepairItemCofirmHandler, CGame::ReqSellItemConfirmHandler, CGame::RequestChargedTeleportHandler, CGame::RequestPurchaseItemHandler, CGame::RequestRetrieveItemHandler, CGame::RequestStudyMagicHandler, CGame::ResponseCreateNewGuildHandler
- callees: ItemDepleteHandler, SendNotifyMsg, iGetItemWeight
- notifies: DEF_NOTIFY_SETITEMCOUNT
- Korean comments (UTF-8, first 2, untranslated quote): 카운트가 0이면 모두 소모된 것이므로 리스트에서 삭제한다. / 아이템의 수량이 변경되었음을 알린다.

## CGame::ReleaseItemHandler - SRV Game.cpp:19523-19674

Unequips an item and clears its appearance bits.

- sig: `void CGame::ReleaseItemHandler(int iClientH, short sItemIndex, BOOL bNotice)`
- callers: CGame::ArmorLifeDecrement, CGame::CheckUniqueItemEquipment, CGame::ClientCommonHandler, CGame::ConfirmExchangeItem, CGame::DropItemHandler, CGame::GiveItemHandler, CGame::ItemDepleteHandler, CGame::bEquipItemHandler, CGame::iCalculateAttackEffect
- callees: CalcTotalItemEffect, SendEventToNearClient_TypeA
- messages: MSGID_EVENT_MOTION
- types: DEF_OBJECTNULLACTION
- Korean comments (UTF-8, first 6, untranslated quote): 장착되지 않은 아이템이라면 무시한다. / 아이템의 해제위치에 맞게 Appr변수를 조정한다. / 오른손에 장착하는 아이템. 무기류이다.

## CGame::RequestRetrieveItemHandler - SRV Game.cpp:27619-27856

Warehouse withdrawal (MSGID_REQUEST_RETRIEVEITEM), warehouse-location check (v2.20), weight check.

- sig: `void CGame::RequestRetrieveItemHandler(int iClientH, char *pData)`
- callers: CGame::MsgProcess
- callees: DeleteClient, SetItemCount, _bItemLog, _iCalcMaxLoad, _iGetArrowItemIndex, iCalcTotalWeight, iGetItemWeight, iSendMsg
- messages: MSGID_NOTIFY, MSGID_RESPONSE_RETRIEVEITEM
- notifies: DEF_NOTIFY_CANNOTCARRYMOREITEM
- types: DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT
- Korean comments (UTF-8, first 6, untranslated quote): v2.20 2002-12-23 창고인지 체크 한다. / 오류다. / 중량계산

## CGame::bSetItemToBankItem - SRV Game.cpp:27858-27983

Deposits an inventory item into the warehouse (by inventory index), warehouse-location check.

- sig: `BOOL CGame::bSetItemToBankItem(int iClientH, short sItemIndex)`
- callers: CGame::GiveItemHandler (17578 only; 17316 uses the pointer overload)
- callees: _bItemLog, iCalcTotalWeight, iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_ITEMTOBANK
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 소지하고 있는 아이템을 보관한다. / v2.20 2002-12-23 창고인지 체크 한다. / 비어있는 위치를 찾았다.

## CGame::GetRewardMoneyHandler - SRV Game.cpp:28438-28569

Pays accumulated reward gold up to half the free weight.

- sig: `void CGame::GetRewardMoneyHandler(int iClientH)`
- callers: CGame::ClientCommonHandler
- callees: DeleteClient, SendNotifyMsg, _bAddClientItemList, _bInitItemAttr, _iCalcMaxLoad, iCalcTotalWeight, iGetItemWeight, iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_ITEMOBTAINED, DEF_NOTIFY_REWARDGOLD
- Korean comments (UTF-8, first 6, untranslated quote): 남은 중량을 계산한다. / 중량을 반으로 나눈다. <- 찾은 돈으로 아이템을 살 공간은 마련해 둬야 하므로. / (iWeightLeft / pItem->m_wWeight)가 최대 받을 수 있는 Gold갯수. 갖고있는 포상금과 비교한다.

## CGame::_iGetArrowItemIndex - SRV Game.cpp:30492-30508

Finds an arrow stack in inventory.

- sig: `int CGame::_iGetArrowItemIndex(int iClientH)`
- callers: CGame::CalcTotalItemEffect, CGame::DropItemHandler, CGame::GiveItemHandler, CGame::ItemDepleteHandler, CGame::RequestInitDataHandler, CGame::RequestRetrieveItemHandler, CGame::UseItemHandler, CGame::_bAddClientItemList, CGame::iCalculateAttackEffect
- Korean comments (UTF-8, first 1, untranslated quote): Arrow 아이템이 1개 이상 있으면 인덱스 값을 반환한다.

## CGame::ItemDepleteHandler - SRV Game.cpp:30510-30548

Removes a used-up item (release, ITEMDEPLETED_ERASEITEM notify, delete, arrow index and weight refresh); logs DEF_ITEMLOG_DEPLETE only when bIsLog is set and the item type is not consume/eat/use-deplete/use-deplete-dest/material, and never for ids 380-382, despite the Korean comment saying scrolls must be logged [V, review R2-10] ([GSO-042](../../issues/game-server-other.md#gso-042-scroll-depletion-is-never-logged)).

- sig: `void CGame::ItemDepleteHandler(int iClientH, short sItemIndex, BOOL bIsUseItemResult, BOOL bIsLog)`
- callers: CGame::Effect_Damage_Spot, CGame::Effect_Damage_Spot_DamageMove, CGame::ReqCreatePortionHandler, CGame::ReqSellItemConfirmHandler, CGame::RequestItemUpgradeHandler, CGame::SetItemCount, CGame::UseItemHandler, CGame::iCalculateAttackEffect
- callees: ReleaseItemHandler, SendNotifyMsg, _bItemLog, _iGetArrowItemIndex, iCalcTotalWeight
- notifies: DEF_NOTIFY_ITEMDEPLETED_ERASEITEM
- Korean comments (UTF-8, first 6, untranslated quote): 소모성 아이템의 카운트가 0이면 삭제한다. 혹은 중고 아이템을 팔아버린 겅우 / v2.15 불필요한 Deplete 로그를 남기지 않는다. / 마법 스크롤지는 로그에 남아야 한다.

## CGame::UseItemHandler - SRV Game.cpp:30626-31091

Uses consumables: HP/MP/SP potions, HP stock food, magic scrolls, attribute changers, lottery, firm stamina, warm potion, show map, skill items (delayed), destination-type items.

- sig: `void CGame::UseItemHandler(int iClientH, short sItemIndex, short dX, short dY, short sDestItemID)`
- callers: CGame::ClientCommonHandler
- callees: ItemDepleteHandler, PlayerMagicHandler, RequestStudyMagicHandler, RequestTeleportHandler, SendEventToNearClient_TypeA, SendNotifyMsg, SetInvisibilityFlag, TrainSkillResponse, _bDepleteDestTypeItemUseEffect, _iGetArrowItemIndex, bRegisterDelayEvent, bRemoveFromDelayEventList, iDice, iGetMaxHP
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_HP, DEF_NOTIFY_ITEMLIFESPANEND, DEF_NOTIFY_MAGICEFFECTOFF, DEF_NOTIFY_MP, DEF_NOTIFY_SHOWMAP, DEF_NOTIFY_SP
- types: DEF_OBJECTNULLACTION
- cfg: Magic.cfg, Skill.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 아이템의 효과에 맞는 처리를 한다. / v2.172 2002-7-5 해동 시약. / 냉동 상태인 경우 해동 되었다는 메세지를 보내준다.

## CGame::bSetItemToBankItem - SRV Game.cpp:32409-32529

Deposits an item into the warehouse (by CItem pointer).

- sig: `BOOL CGame::bSetItemToBankItem(int iClientH, class CItem * pItem)`
- callers: CGame::GiveItemHandler (17316 only; 17578 uses the index overload)
- callees: _bItemLog, iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_ITEMTOBANK
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 소지하고 있는 아이템을 보관한다. / v2.20 2002-12-23 창고인지 체크 한다. / 비어있는 위치를 찾았다.

## CGame::ClearSkillUsingStatus - SRV Game.cpp:33674-33698

Cancels active skill use (and event fishing).

- sig: `void CGame::ClearSkillUsingStatus(int iClientH)`
- callers: CGame::Effect_Damage_Spot, CGame::Effect_Damage_Spot_DamageMove, CGame::bDeleteFish, CGame::iCalculateAttackEffect, CGame::iClientMotion_Attack_Handler, CGame::iClientMotion_GetItem_Handler, CGame::iClientMotion_Magic_Handler, CGame::iClientMotion_Move_Handler, CGame::iClientMotion_Stop_Handler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_FISHCANCELED
- Korean comments (UTF-8, first 3, untranslated quote): 만약 이벤트 낚시와 같은 것을 하고 있었다면 여기서 해제되었음을 통보해 준다. / 이벤트 낚시 모드였다. 해제시킨다. / 카운트 감소

## CGame::iCalculateUseSkillItemEffect - SRV Game.cpp:33700-33798

Skill-item results: fishing/meat gathering (skill vs 1d105), taming.

- sig: `int CGame::iCalculateUseSkillItemEffect(int iOwnerH, char cOwnerType, char cOwnerSkill, int iSkillNum, char cMapIndex, int dX, int dY)`
- callers: CGame::DelayEventProcessor
- callees: CalculateSSN_SkillIndex, GetExp, SendEventToNearClient_TypeB, SendNotifyMsg, _TamingHandler, _bInitItemAttr, bGetIsWater, bSetItem, iCheckFish, iDice
- messages: MSGID_EVENT_COMMON
- notifies: DEF_NOTIFY_FISHSUCCESS
- types: DEF_COMMONTYPE_ITEMDROP
- cfg: Skill.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 스킬 사용 여부 주사위를 굴린다. / 스킬이 100이라고 해도 가끔 낚시를 실패하게 하기 위해서 1D105 / 실패다.

## CGame::UseSkillHandler - SRV Game.cpp:33802-33915

Starts a timed skill (pretend corpse etc.) and schedules the result.

- sig: `void CGame::UseSkillHandler(int iClientH, int iV1, int iV2, int iV3)`
- callers: CGame::ClientCommonHandler
- callees: CalculateSSN_SkillIndex, ClearOwner, GetDeadOwner, GetOwner, SendEventToNearClient_TypeA, SendNotifyMsg, SetDeadOwner, SetInvisibilityFlag, bRemoveFromDelayEventList, iDice
- messages: MSGID_EVENT_MOTION
- notifies: DEF_NOTIFY_SKILLUSINGEND
- types: DEF_OBJECTDYING
- cfg: Skill.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 이미 기술을 사용중이라도 리턴. / v1.3 !!1해킹 걸러내기용! / 플레이어의 기술수준에 따라 성공여부를 계산한다.

## CGame::ReqSellItemHandler - SRV Game.cpp:33917-34101

Quotes a sell price (category-based, remaining life) - DEF_NOTIFY_SELLITEMPRICE.

- sig: `void CGame::ReqSellItemHandler(int iClientH, char cItemID, char cSellToWhom, int iNum, char * pItemName)`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg, _iCalcMaxLoad, iCalcTotalWeight, iGetItemWeight
- notifies: DEF_NOTIFY_CANNOTSELLITEM, DEF_NOTIFY_SELLITEMPRICE
- Korean comments (UTF-8, first 6, untranslated quote): 사용자의 아이템 팔기 요구. / v2.13 성후니 수정 물건을 어디서나 팔게 수정하여서 팔때는 NPC 구분이 필요없다. / 단 카테고리를 기준으로 아이템 가격을 결정한다.

## CGame::ReqSellItemConfirmHandler - SRV Game.cpp:34103-34426

Executes the sale and pays gold (shop location check v2.182).

- sig: `void CGame::ReqSellItemConfirmHandler(int iClientH, char cItemID, int iNum, char * pString)`
- callers: CGame::ClientCommonHandler, CGame::RequestSellItemListHandler
- callees: DeleteClient, ItemDepleteHandler, SendEventToNearClient_TypeB, SendNotifyMsg, SetItemCount, _bAddClientItemList, _bInitItemAttr, _bItemLog, bSetItem, iCalcTotalWeight, iSendMsg
- messages: MSGID_EVENT_COMMON, MSGID_NOTIFY
- notifies: DEF_NOTIFY_CANNOTCARRYMOREITEM, DEF_NOTIFY_ITEMOBTAINED, DEF_NOTIFY_ITEMSOLD
- types: DEF_COMMONTYPE_ITEMDROP
- Korean comments (UTF-8, first 6, untranslated quote): 아이템을 팔겠다는 것이 결정되었다. / v2.182 2002-11-15 사고 팔수 있는 곳인지 체크하는 변수 추가 / 아이템의 종류에 따라 가격 계산.

## CGame::ReqRepairItemHandler - SRV Game.cpp:34428-34505

Quote: price/2, minus (short)((cur/max)*0.5*price) when cur > 0 (price/2 at 0 life; max 0 is treated as 1). Weapons (category 1-10) need repairer type 24, categories 11-13 and 43-50 need 15 (CANNOTREPAIRITEM code 2 otherwise); other categories reply code 1 [V, review R2-01] (34428-34505).

- sig: `void CGame::ReqRepairItemHandler(int iClientH, char cItemID, char cRepairWhom, char * pString)`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_CANNOTREPAIRITEM, DEF_NOTIFY_REPAIRITEMPRICE
- Korean comments (UTF-8, first 6, untranslated quote): 아이템을 고치겠다는 요구. / 아이템의 종류에 따라 가격 계산. / 무기류다

## CGame::ReqRepairItemCofirmHandler - SRV Game.cpp:34507-34603

Executes the repair at a shop/blacksmith (v2.19). Requires `m_bIsOnShop` (34524); has no repairer-type test (that is only in the quote handler). Recomputes the same price (with `abs()`), replies NOTENOUGHGOLD if short (34556), sets life to max (34582), takes the gold (34590) and credits the fee to the player's side city funds `m_stCityStatus[side].iFunds` (34596) [V, review R2-02] ([GSO-007](../../issues/game-server-other.md#gso-007-repair-confirmation-has-no-repairer-type-check)).

- sig: `void CGame::ReqRepairItemCofirmHandler(int iClientH, char cItemID, char * pString)`
- callers: CGame::ClientCommonHandler
- callees: DeleteClient, SendNotifyMsg, SetItemCount, _bItemLog, dwGetItemCount, iCalcTotalWeight, iSendMsg
- messages: MSGID_NOTIFY
- notifies: DEF_NOTIFY_ITEMREPAIRED, DEF_NOTIFY_NOTENOUGHGOLD
- switches: TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 아이템을 수리하겠다는 것이 결정되었다. / v2.19 2002-11-12 상점이나 대장간에서만 아이템 수리가 가능하게 수정 (해킹방지를 위해) / 아이템의 종류에 따라 가격 계산.

## CGame::iCreateFish - SRV Game.cpp:36810-36858

Places a fish dynamic object on water.

- sig: `int CGame::iCreateFish(char cMapIndex, short sX, short sY, short sType, class CItem * pItem, int iDifficulty, DWORD dwLastTime)`
- callers: CGame::AdminOrder_CreateFish, CGame::FishGenerator
- callees: CFish, bGetIsWater, iAddDynamicObjectList
- Korean comments (UTF-8, first 6, untranslated quote): 맵의 위치가 물인지 확인한다. / 빈 공간에 물고기를 만든다. / Dynamic Object를 발생시킨다. Owner에 Fish 인덱스를 넣는다.

## CGame::bDeleteFish - SRV Game.cpp:36861-36898

Removes a fish and cancels anglers.

- sig: `BOOL CGame::bDeleteFish(int iHandle, int iDelMode)`
- callers: CGame::CheckDynamicObjectList, CGame::ReqGetFishThisTimeHandler
- callees: ClearSkillUsingStatus, SendEventToNearClient_TypeB, SendNotifyMsg, SetDynamicObject
- messages: MSGID_DYNAMICOBJECT
- notifies: DEF_NOTIFY_FISHCANCELED
- types: DEF_MSGTYPE_REJECT
- Korean comments (UTF-8, first 6, untranslated quote): 연관되어 있는 DynamicObject를 삭제한다. / 맵에서 삭제한다. / 이 물고기와 연결되어 있는 플레이어들에게 물고기가 사라져 낚시가 취소되었음을 알려준다.

## CGame::iCheckFish - SRV Game.cpp:36901-36945

Finds a fish object near the cast point and enters event-fishing mode.

- sig: `int CGame::iCheckFish(int iClientH, char cMapIndex, short dX, short dY)`
- callers: CGame::iCalculateUseSkillItemEffect
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_EVENTFISHMODE
- Korean comments (UTF-8, first 6, untranslated quote): 맵의 특정 위치 내에 물고기 다이나믹 오브젝트가 있는지 판단한다. / 물고기 다이나믹 오브젝트를 찾았다. 이제 이 다이나믹 오브젝트 인덱스를 소유한 Fish의 인덱스를 반환한다. / 이미 낚시모드에 들어가 있는 캐릭터는 중복 동작이 불가능.

## CGame::FishProcessor - SRV Game.cpp:36947-36986

Every 4 s adjusts each angler's catch chance by the Fishing skill vs fish difficulty.

- sig: `void CGame::FishProcessor()`
- callers: CGame::OnTimer
- callees: SendNotifyMsg, iDice
- notifies: DEF_NOTIFY_FISHCHANCE
- Korean comments (UTF-8, first 6, untranslated quote): 이벤트 낚시 모드가 할당된 플레이어들을 처리한다. / 이 플레이어에게 할당된 물고기가 있다. 낚을 확률을 재계산하여 통보해 준다. / 낚시 기술의 스킬

## CGame::ReqGetFishThisTimeHandler - SRV Game.cpp:37201-37252

Player pulls: success if 1d100 <= m_iFishChance. On success the fish item is dropped on the angler's tile (bSetItem + ITEMDROP broadcast, not placed in the inventory), exp is GetExp(iDice(difficulty,6)), fishing SSN +difficulty, FISHSUCCESS sent, fish deleted; on failure the engaging count is decremented and FISHFAIL sent [V, review R2-11].

- sig: `void CGame::ReqGetFishThisTimeHandler(int iClientH)`
- callers: CGame::ClientCommonHandler
- callees: CalculateSSN_SkillIndex, GetExp, SendEventToNearClient_TypeB, SendNotifyMsg, bDeleteFish, bSetItem, iDice
- messages: MSGID_EVENT_COMMON
- notifies: DEF_NOTIFY_FISHFAIL, DEF_NOTIFY_FISHSUCCESS
- types: DEF_COMMONTYPE_ITEMDROP
- Korean comments (UTF-8, first 6, untranslated quote): 낚시 스킬 사용중임을 해제시킨다. / 이 물고기를 낚는데 성공하였다! / 경험치 증가

## CGame::FishGenerator - SRV Game.cpp:37255-37357

Spawns fish at map fish points up to the map maximum.

- sig: `void CGame::FishGenerator()`
- callers: CGame::OnTimer
- callees: _bInitItemAttr, iCreateFish, iDice
- Korean comments (UTF-8, first 6, untranslated quote): 만들 물고기 종류와 난이도, 지속 시간을 결정한다. / 적색잉어 / 녹색잉어

## CGame::ReqCreatePortionHandler - SRV Game.cpp:38261-38604

Alchemy: matches ingredient set against Potion.cfg, skill-vs-difficulty roll, creates potion.

- sig: `void CGame::ReqCreatePortionHandler(int iClientH, char *pData)`
- callers: CGame::ClientCommonHandler
- callees: CalculateSSN_SkillIndex, DeleteClient, GetExp, ItemDepleteHandler, SendEventToNearClient_TypeB, SendNotifyMsg, SetItemCount, _bAddClientItemList, _bInitItemAttr, bSetItem, iDice, iSendMsg
- messages: MSGID_EVENT_COMMON, MSGID_NOTIFY
- notifies: DEF_NOTIFY_CANNOTCARRYMOREITEM, DEF_NOTIFY_ITEMOBTAINED, DEF_NOTIFY_LOWPORTIONSKILL, DEF_NOTIFY_NOMATCHINGPORTION, DEF_NOTIFY_PORTIONFAIL, DEF_NOTIFY_PORTIONSUCCESS
- types: DEF_COMMONTYPE_ITEMDROP
- cfg: Potion.cfg
- switches: defined
- Korean comments (UTF-8, first 6, untranslated quote): 포션의 재료 인덱스를 받았다. 이 재료가 포션을 만들 수 있는 조합인지 확인한다. / 데이터가 유효한 아이템 인덱스인지 체크한다. / 먼저 이미 있는 리스트인지 검색

## CGame::_bDecodePortionConfigFileContents - SRV Game.cpp:38608-38734

Parses Potion.cfg (portion.cfg) recipes.

- sig: `BOOL CGame::_bDecodePortionConfigFileContents(char *pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: CStrTok, PutLogList, _bGetIsStringIsNumber, pGet
- cfg: Potion.cfg
- Korean comments (UTF-8, first 6, untranslated quote): 포션 번호 / 이미 할당된 번호가 있다. 에러이다. / 포션 이름

## CGame::MineralGenerator - SRV Game.cpp:38796-38815

Spawns mineral deposits at mineral points.

- sig: `void CGame::MineralGenerator()`
- callers: CGame::OnTimer
- callees: iCreateMineral, iDice
- Korean comments (UTF-8, first 1, untranslated quote): v2.12 2002-2-6 미네럴 생성 루틴 변경 ..

## CGame::iCreateMineral - SRV Game.cpp:38817-38877

Creates a mineral object (ore or gem types by level).

- sig: `int CGame::iCreateMineral(char cMapIndex, int tX, int tY, char cLevel)`
- callers: CGame::MineralGenerator
- callees: CMineral, iAddDynamicObjectList, iDice
- Korean comments (UTF-8, first 6, untranslated quote): 빈 공간에 광물덩이를 만든다. / 광물류 / 보석류

## CGame::_CheckMiningAction - SRV Game.cpp:38880-39121

Pickaxe hit on a mineral: Mining skill - difficulty vs 1d100, yields ore/gems and small exp.

- sig: `void CGame::_CheckMiningAction(int iClientH, int dX, int dY)`
- callers: CGame::iClientMotion_Attack_Handler
- callees: CalculateSSN_SkillIndex, GetExp, SendEventToNearClient_TypeB, SetInvisibilityFlag, _bInitItemAttr, bDeleteMineral, bGetDynamicObject, bRemoveFromDelayEventList, bSetItem, iDice
- messages: MSGID_EVENT_COMMON
- types: DEF_COMMONTYPE_ITEMDROP
- Korean comments (UTF-8, first 6, untranslated quote): v2.17 2002-6-29 광물 캘때 투명 모드였다면 마법 Casting시에 해제된다. / 광물채취를 시도한 캐릭터의 조건을 살핀다. 곡괭이를 들고 있는가? 광물 채취 스킬은? / 곡괭이에 해당하는 외형이면 통과

## CGame::bDeleteMineral - SRV Game.cpp:39124-39151

Removes a depleted mineral.

- sig: `BOOL CGame::bDeleteMineral(int iIndex)`
- callers: CGame::_CheckMiningAction
- callees: SendEventToNearClient_TypeB, SetDynamicObject, SetTempMoveAllowedFlag
- messages: MSGID_DYNAMICOBJECT
- types: DEF_MSGTYPE_REJECT
- Korean comments (UTF-8, first 4, untranslated quote): 맵에서 광물 동적 객체를 삭제한다. / 광물이 사라졌으므로 이동이 가능하게 한다. / 광물 개체 수 감소

## CGame::_bDepleteDestTypeItemUseEffect - SRV Game.cpp:39410-39496

Destination items: occupy flag, construction kit, dye, farming seed.

- sig: `BOOL CGame::_bDepleteDestTypeItemUseEffect(int iClientH, int dX, int dY, short sItemIndex, short sDestItemID)`
- callers: CGame::UseItemHandler
- callees: SendNotifyMsg, __bSetAgricultureItem, __bSetConstructionKit, __bSetOccupyFlag
- notifies: DEF_NOTIFY_ITEMCOLORCHANGE, DEF_NOTIFY_NOTFLAGSPOT
- Korean comments (UTF-8, first 6, untranslated quote): 위치를 지정하고 사용 후 사라지는 아이템 효과 처리 부분 / 영토 점령용 깃발이다. m_sItemEffectValue1 가 사이드, m_sItemSpecEffectValue1이 EK 수를 나타낸다. m_sItemEffectValue2번이 1이면 Admin Flag / 마스터 깃발이면 아무데나 꽂을 수 있고 EKNum은 무조건 100이다.

## CGame::_SetItemPos - SRV Game.cpp:40626-40653

Stores client inventory icon positions (MSGID_REQUEST_SETITEMPOS).

- sig: `void CGame::_SetItemPos(int iClientH, char *pData)`
- callers: CGame::MsgProcess
- Korean comments (UTF-8, first 1, untranslated quote): 잘못된 좌표값 보정

## CGame::CheckUniqueItemEquipment - SRV Game.cpp:40655-40686

Unequips owner-bound items worn by someone else, and deals iDice(10,10) damage to the wearer, which can kill (ClientKilledHandler) [V, review R2-12].

- sig: `void CGame::CheckUniqueItemEquipment(int iClientH)`
- callers: CGame::CheckClientResponseTime
- callees: ClientKilledHandler, ReleaseItemHandler, SendNotifyMsg, iDice
- notifies: DEF_NOTIFY_ITEMRELEASED
- Korean comments (UTF-8, first 5, untranslated quote): v1.3 유니크 아이템의 장착 상태를 판단한다. 특정인만이 착용 가능한 아이템을 장착한 경우 벗겨진다. / Touch Effect Type이 DEF_ITET_OWNER이면 Touch Effect Value 1, 2, 3이 주인 캐릭터의 고유값을 갖는다. / ID 값이 맞다.

## CGame::ExchangeItemHandler - SRV Game.cpp:40826-40915

Opens a trade window with another player.

- sig: `void CGame::ExchangeItemHandler(int iClientH, short sItemIndex, int iAmount, short dX, short dY, WORD wObjectID, char *pItemName)`
- callers: CGame::ClientCommonHandler
- callees: GetOwner, SendNotifyMsg, _ClearExchangeStatus
- notifies: DEF_NOTIFY_OPENEXCHANGEWINDOW
- switches: DEF_GUILDWARMODE
- Korean comments (UTF-8, first 6, untranslated quote): 길드전 모드에서는 교환 불가 / v2.172 2002-7-5 교환상태인데 또 이런 메시지가 날라오면 해킹이다. / 아이템을 누구와 교환하겠다는 메시지가 도착했다. 상대방에게 알리고 양쪽 모두 교환창을 열도록 한다.

## CGame::SetExchangeItem - SRV Game.cpp:40919-40963

Puts an item into the trade.

- sig: `void CGame::SetExchangeItem(int iClientH, int iItemIndex, int iAmount)`
- callers: CGame::ClientCommonHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_SETEXCHANGEITEM
- Korean comments (UTF-8, first 5, untranslated quote): 교환할 상대방이 없거나 교환하고자 했던 그 캐릭터가 아니다. / 교환하고자 하는 상대방에게 아이템을 알려준다. / 해당 아이템이 존재하는지, 수량이 맞는지 체크한다.

## CGame::ConfirmExchangeItem - SRV Game.cpp:40966-41194

Both confirmed: checks space and weight, swaps the items, logs.

- sig: `void CGame::ConfirmExchangeItem(int iClientH)`
- callers: CGame::ClientCommonHandler
- callees: ReleaseItemHandler, SendNotifyMsg, SetItemCount, _ClearExchangeStatus, _bInitItemAttr, _bItemLog, _iCalcMaxLoad, _iGetItemSpaceLeft, bAddItem, bCopyItemContents, iCalcTotalWeight, iGetItemWeight
- notifies: DEF_NOTIFY_EXCHANGEITEMCOMPLETE, DEF_NOTIFY_GIVEITEMFIN_ERASEITEM
- Korean comments (UTF-8, first 6, untranslated quote): v1.42 설마 이런 경우가? / 교환하고자 했던 그 캐릭터가 아니다. 교환 상태는 취소. / 상대방도 교환의사를 밝혔다. 교환이 성립할 수 있는지 계산한다. 아이템 갯수, 무게 제한을 판단.

## CGame::_iGetItemSpaceLeft - SRV Game.cpp:41198-41207

Free inventory slots.

- sig: `int CGame::_iGetItemSpaceLeft(int iClientH)`
- callers: CGame::ConfirmExchangeItem

## CGame::bAddItem - SRV Game.cpp:41209-41322

Gives a server-created item to a player or drops it if full.

- sig: `BOOL CGame::bAddItem(int iClientH, CItem * pItem, char cMode)`
- callers: CGame::BuildItemHandler, CGame::CalcExpStock, CGame::ConfirmExchangeItem
- callees: SendEventToNearClient_TypeB, _bAddClientItemList, bSetItem, iSendMsg
- messages: MSGID_EVENT_COMMON, MSGID_NOTIFY
- notifies: DEF_NOTIFY_CANNOTCARRYMOREITEM, DEF_NOTIFY_ITEMOBTAINED
- types: DEF_COMMONTYPE_ITEMDROP
- Korean comments (UTF-8, first 6, untranslated quote): 아이템을 획득했다. / 1개 획득했다. Amount가 아니다! / 수량을 입력

## CGame::CancelExchangeItem - SRV Game.cpp:41325-41333

Cancels the trade for both.

- sig: `void CGame::CancelExchangeItem(int iClientH)`
- callers: CGame::ClientCommonHandler
- callees: _ClearExchangeStatus
- Korean comments (UTF-8, first 1, untranslated quote): 현재 교환을 취소한다. 상대방도 자동으로 취소된다.

## CGame::_ClearExchangeStatus - SRV Game.cpp:41336-41349

Resets one side's trade state and notifies.

- sig: `void CGame::_ClearExchangeStatus(int iClientH)`
- callers: CGame::CancelExchangeItem, CGame::ClientKilledHandler, CGame::ConfirmExchangeItem, CGame::DeleteClient, CGame::ExchangeItemHandler, CGame::RequestTeleportHandler
- callees: SendNotifyMsg
- notifies: DEF_NOTIFY_CANCELEXCHANGEITEM

## CGame::_bCheckItemReceiveCondition - SRV Game.cpp:42288-42315

Weight/space check before giving an item.

- sig: `BOOL CGame::_bCheckItemReceiveCondition(int iClientH, CItem *pItem)`
- callers: CGame::_iTalkToNpcResult_Cityhall
- callees: _iCalcMaxLoad, iGetItemWeight
- Korean comments (UTF-8, first 4, untranslated quote): 먼저 중량 초과 여부를 가린다. / 수량개념이 있는 아이템 / 수량 개념이 없는 아이템

## CGame::_bDecodeBuildItemConfigFileContents - SRV Game.cpp:43156-43523

Parses BuildItem.cfg recipes.

- sig: `BOOL CGame::_bDecodeBuildItemConfigFileContents(char *pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: CStrTok, PutLogList, _bGetIsStringIsNumber, _bInitItemAttr, pGet
- cfg: BuildItem.cfg
- Korean comments (UTF-8, first 5, untranslated quote): 아이템 이름 / 스킬 제한치 / 아이템의 존재가 확인되었다.

## CGame::BuildItemHandler - SRV Game.cpp:43525-43943

Manufacturing (DEF_COMMONTYPE_BUILDITEM): matches recipe, skill roll, quality from material purity, exp.

- sig: `void CGame::BuildItemHandler(int iClientH, char *pData)`
- callers: CGame::ClientCommonHandler
- callees: CalculateSSN_SkillIndex, GetExp, PutLogFileList, PutLogList, SendNotifyMsg, SetItemCount, _bInitItemAttr, _bItemLog, bAddItem, iDice
- notifies: DEF_NOTIFY_BUILDITEMFAIL, DEF_NOTIFY_BUILDITEMSUCCESS
- cfg: BuildItem.cfg
- switches: DEF_TAIWANLOG
- Korean comments (UTF-8, first 6, untranslated quote): 사용자가 아이템 제작을 신청했다. / 앞부분의 빈공간을 없앤다. / 실패다.

## CGame::NpcDeadItemGenerator - SRV Game.cpp:44638-45622

Monster drop: chance 1 - prob/10000 (8500, -900 when not in a forming party); 65% gold, else potions/stones or a rare weapon/armour by monster gen level with random prefix attributes ([GSO-001](../../issues/game-server-other.md#gso-001-monster-drop-party-test-lowers-the-threshold-for-nearly-every-kill)).

- sig: `void CGame::NpcDeadItemGenerator(int iNpcH, short sAttackerH, char cAttackerType)`
- callers: CGame::NpcKilledHandler
- callees: NpcDeadItemGeneratorWithItemEvent, SendEventToNearClient_TypeB, _AdjustRareItemValue, _bInitItemAttr, _bItemLog, bCheckInItemEventList, bSetItem, iDice
- messages: MSGID_EVENT_COMMON
- types: DEF_COMMONTYPE_ITEMDROP
- switches: DEF_BALLEVENT, DEF_CHINESECANDYEVENT, DEF_CONSULTATION, DEF_ITEMDROPRATINGUP, DEF_LOGTIME, DEF_TESTSERVER
- Korean comments (UTF-8, first 6, untranslated quote): adamas(2002-10-28) : 아이템 이벤트 형식 변경 / v2.19 2002-11-17 파티하면 아이템 떨어질 확률이 10% 높아진다. / v2.20 2002-2-22 아이템 확률 증가

## CGame::NpcDeadItemGeneratorWithItemEvent - SRV Game.cpp:45634-45755

Item-event drops configured per map/date/mob (v2.04/2002-10-23).

- sig: `BOOL CGame::NpcDeadItemGeneratorWithItemEvent(int iNpcH, short sAttackerH, char cAttackerType)`
- callers: CGame::NpcDeadItemGenerator
- callees: PutDebugMsg, SendEventToNearClient_TypeB, _bInitItemAttr, _bItemLog, bSetItem, iDice
- messages: MSGID_EVENT_COMMON
- types: DEF_COMMONTYPE_ITEMDROP
- switches: DEF_LOGTIME
- Korean comments (UTF-8, first 6, untranslated quote): v2.04 아이템 이벤트용 특수 코드 / 날짜 같고, / 아이템 이벤트 개수가 남았으면.

## CGame::bCheckInItemEventList - SRV Game.cpp:45759-45791

Excludes type-0 event items from normal drops.

- sig: `BOOL CGame::bCheckInItemEventList(int iItemID, int iNpcH)`
- callers: CGame::NpcDeadItemGenerator
- cfg: Item.cfg
- Korean comments (UTF-8, first 3, untranslated quote): item id로 item name을 가져 온다. / 해당 item이 item event에 있는지 검사한다. / type이 0이 아니면 건너 뛴다.

## CGame::_bDecodeDupItemIDFileContents - SRV Game.cpp:45794-45930

Parses DupItemID.cfg (duplicated-item fingerprints).

- sig: `BOOL CGame::_bDecodeDupItemIDFileContents(char *pData, DWORD dwMsgSize)`
- callers: CGame::MsgProcess
- callees: CStrTok, PutLogList, _bGetIsStringIsNumber, pGet
- cfg: DupItemID.cfg
- Korean comments (UTF-8, first 2, untranslated quote): 스킬 번호 / 이미 할당된 번호가 있다. 에러이다.

## CGame::_bCheckDupItemID - SRV Game.cpp:45932-45949

Matches an item against the DupItemID list and refreshes its price.

- sig: `BOOL CGame::_bCheckDupItemID(CItem *pItem)`
- callers: CGame::_bDecodePlayerDatafileContents
- cfg: DupItemID.cfg
- Korean comments (UTF-8, first 1, untranslated quote): 가격 정보를 갱신하고 리턴.

## CGame::_AdjustRareItemValue - SRV Game.cpp:45951-45988

Applies rare-prefix effects to item stats (light: weight, strong: lifespan...).

- sig: `void CGame::_AdjustRareItemValue(CItem *pItem)`
- callers: CGame::NpcDeadItemGenerator, CGame::_bDecodePlayerDatafileContents
- Korean comments (UTF-8, first 6, untranslated quote): 희귀 아이템 효과 종류: / 0-None 1-필살기대미지추가 2-중독효과 3-정의의 / 5-민첩의 6-가벼운 7-예리한 8-강화된 9-고대문명의

## CGame::RequestSellItemListHandler - SRV Game.cpp:46601-46635

Sells a list of items in one request.

- sig: `void CGame::RequestSellItemListHandler(int iClientH, char * pData)`
- callers: CGame::MsgProcess
- callees: ReqSellItemConfirmHandler
- Korean comments (UTF-8, first 3, untranslated quote): 내용을 다 읽었다. 순서대로 팔아치운다. / cIndex에 해당하는 아이템을 판다. / 이 루틴을 수행한 다음 클라이언트가 삭제되었을 수 있으니 주의!

## CGame::iGetItemWeight - SRV Game.cpp:47173-47184

Weight of a stack; gold counts 1/20.

- sig: `int CGame::iGetItemWeight(CItem *pItem, int iCount)`
- callers: CGame::ConfirmExchangeItem, CGame::GetRewardMoneyHandler, CGame::ReqSellItemHandler, CGame::RequestRetrieveItemHandler, CGame::SetItemCount, CGame::_bAddClientItemList, CGame::_bCheckItemReceiveCondition, CGame::_bDecodePlayerDatafileContents, CGame::bEquipItemHandler, CGame::iCalcTotalWeight
- Korean comments (UTF-8, first 1, untranslated quote): 아이템의 수량에 따른 무게를 계산한다. Gold인 경우 무게를 20분의 1로 변경

## CGame::bGetItemNameWhenDeleteNpc - SRV Game.cpp:47187-47550

NpcItem table drop pick (type 1: two dice must match; type 2: range table). Only 47193-47258 is compiled (DEF_NPCITEM active); lines 47260-47547 are the `#else` branch and are inactive, and it is that inactive branch that uses iItemprobability (1-in-N dice test) ([GSO-008](../../issues/game-server-other.md#gso-008-the-drop-probability-deletenpc-passes-is-used-only-by-an-inactive-branch)).

- sig: `BOOL CGame::bGetItemNameWhenDeleteNpc(int & iItemID, short sNpcType, int iItemprobability)`
- callers: CGame::DeleteNpc
- callees: iDice
- cfg: Npc.cfg
- switches: DEF_TESTSERVER, defined
- Korean comments (UTF-8, first 6, untranslated quote): 2002-09-17 #1 NPCITEM 개선 / 설정 파일의 여러 개의 아이템 중 하나를 고른다. / NPC를 삭제할때 특수 아이템을 발생시킬 것인지의 여부를 계산한다.

## CGame::bCopyItemContents - SRV Game.cpp:50614-50667

Copies every field of one CItem to another.

- sig: `BOOL CGame::bCopyItemContents(CItem * pCopy, CItem *pOriginal)`
- callers: CGame::ConfirmExchangeItem
- Korean comments (UTF-8, first 2, untranslated quote): 아이템의 고유 번호 / v1.432 명중률 가감 사용 안한다. 대신 특수 능력 수치가 들어간다.

## CGame::bCheckAndConvertPlusWeaponItem - SRV Game.cpp:50878-50927

Converts legacy '+1/+2' named weapons into the base item with the bonus in m_dwAttribute.

- sig: `BOOL CGame::bCheckAndConvertPlusWeaponItem(int iClientH, int iItemIndex)`
- callers: CGame::_bDecodePlayerDatafileContents
- Korean comments (UTF-8, first 6, untranslated quote): 이 아이템이 +1, +2 이름이 붙은 아이템이라면 Attribute 플래그로 특성치를 이동시키고 일반 아이템으로 변형시킨다. / 단검 +1 / 쇼트 소드 +1

## CGame::RequestItemUpgradeHandler - SRV Game.cpp:51812-52438

Weapon/armour upgrade with majestic points (Gizon) or upgrade stones; special Merien/Xelima handling.

- sig: `void CGame::RequestItemUpgradeHandler(int iClientH, int iItemIndex)`
- callers: CGame::ClientCommonHandler
- callees: ItemDepleteHandler, SendNotifyMsg, _bInitItemAttr, _bItemLog, bCheckIsItemUpgradeSuccess
- notifies: DEF_NOTIFY_GIZONITEMCANGE, DEF_NOTIFY_GIZONITEMUPGRADELEFT, DEF_NOTIFY_ITEMATTRIBUTECHANGE, DEF_NOTIFY_ITEMUPGRADEFAIL
- Korean comments (UTF-8, first 6, untranslated quote): 최대치이다. 더이상 업그레이드 불가능 / 더이상 아이템 업그레이드가 불가능 합니다. / v2.19 2002-12-7 지존 아이템 관련

## CGame::bCheckIsItemUpgradeSuccess - SRV Game.cpp:52441-52485

Upgrade roll by current +level (20,15,9,8,7,7,5,5,3,2 %), crafted bonus, doubled with bonus stone.

- sig: `BOOL CGame::bCheckIsItemUpgradeSuccess(int iClientH, int iItemIndex, int iSomH, BOOL bBonus)`
- callers: CGame::RequestItemUpgradeHandler
- callees: _bItemLog, iDice
- Korean comments (UTF-8, first 2, untranslated quote): +1 :90%     +1~+2(제작 아이템 완성도 200%일때 보너스 성공률 +10%) / 제작된 무기라면 성공 확률 ItemSpecialEffectValue2가 100이상일때 증가된다. 최대 10%

## CGame::ArmorLifeDecrement - SRV Game.cpp:52488-52593

Wears the body armour of a hit target (not same side).

- sig: `void CGame::ArmorLifeDecrement(int iAttackerH, int iTargetH, char cOwnerType, int iValue)`
- callers: CGame::PlayerMagicHandler
- callees: ReleaseItemHandler, SendNotifyMsg
- notifies: DEF_NOTIFY_ITEMLIFESPANEND
- Korean comments (UTF-8, first 6, untranslated quote): v1.3 피격당했으므로 장착하고 있는 몸통 방어구의 수명을 줄인다. / v2.16 같은 편이면 수명 달지 않게 / 몸통 갑옷

## CGame::__bSetAgricultureItem - SRV Game.cpp:52945-53058

v2.19 farming: plants a crop NPC (type 64) on farm tiles.

- sig: `BOOL CGame::__bSetAgricultureItem(int iMapIndex, int dX, int dY, int iType,int iSsn, int iClientH)`
- callers: CGame::_bDepleteDestTypeItemUseEffect
- callees: GetOwner, PutLogList, SendEventToNearClient_TypeA, SendNotifyMsg, SetNamingValueEmpty, bCreateNewNpc, bGetIsFarm, iGetEmptyNamingValue
- messages: MSGID_EVENT_LOG
- notifies: DEF_NOTIFY_AGRICULTURENOAREA, DEF_NOTIFY_AGRICULTURESKILLLIMIT, DEF_NOTIFY_NOMOREAGRICULTURE
- types: DEF_MSGTYPE_CONFIRM
- Korean comments (UTF-8, first 6, untranslated quote): // 크루세이드 모드가 아니거나 건설한 사람이 공병이 아니면 무시. / 해당 맵에 농작물 개수 제한에 걸렸다... / 쓸수가 없다.. 스킬 제한에 걸린다..

## CGame::bCropsItemDrop - SRV Game.cpp:53065-53138

Harvest drop (watermelon, pumpkin...) and exp.

- sig: `BOOL CGame::bCropsItemDrop(int iClientH, short iTargetH,BOOL bMobDropPos)`
- callers: CGame::iCalculateAttackEffect
- callees: GetExp, SendEventToNearClient_TypeB, _bInitItemAttr, bSetItem, iDice
- messages: MSGID_EVENT_COMMON
- types: DEF_COMMONTYPE_ITEMDROP
- Korean comments (UTF-8, first 6, untranslated quote): 수박 / 경험치를 준다. / 호박

## CGame::bProbabilityTable - SRV Game.cpp:53189-53215

Farming success tables by skill vs crop level.

- sig: `int CGame::bProbabilityTable(int x,int y,int iTable)`
- callers: CGame::iCalculateAttackEffect
- callees: iDice

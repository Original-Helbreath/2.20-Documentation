# Functions: Items, inventory, shops, bank, exchange

Source: `sources\client-2.20\` (librehelbreath ClientSources c01544d). 34 function definitions (including `#if` duplicate bodies). Generated from a col-0 signature scan, then reviewed by hand.

Column key: **Where** file:start-end (verified). **Purpose** from name pattern and the nearest source comment (inferred unless the row ends in [V], meaning re-checked against the function body by the 2026-10-25 review, or the overview says verified; comments are UTF-8 Korean in source, quoted as-is). **Callers/Callees** matched by bare name across all files (verified as text matches; same-named methods in different classes are merged). **Msgs** MSGID/DEF_NOTIFY/etc. names in the body (see `..\..\shared\messages\`). **Dlg** dialog ids enabled/disabled (ids in `..\data-structures.md`). **Files** path literals in the body.

## Overview

- Item model (verified): `CItem` per slot in `m_pItemList[DEF_MAXITEMS]` and `m_pBankList`; names via `CItemName` (ItemName.cfg, English names when `DEF_ENGLISHITEM`); `CBuildItem` recipes from the build-item contents (`_bDecodeBuildItemContents`).
- Server events arrive as `NotifyMsg_Item*` (in `networking.md`); user actions go out through `bSendCommand` with `DEF_COMMONTYPE_*` subcommands (equip, drop, give, sell, repair, upgrade, exchange).

## Function table

| Function | Where | Lines | Purpose | Callers | Callees | Msgs | Dlg | Files |
|---|---|---|---|---|---|---|---|---|
| `CBuildItem::CBuildItem` | BuildItem.cpp:11-35 | 25 | comment: Construction/Destruction | - | - | - | - | - |
| `CBuildItem::~CBuildItem` | BuildItem.cpp:37-40 | 4 | (name only) | - | - | - | - | - |
| `CGame::_SetItemOrder` | Game.cpp:4570-4593 | 24 | (name only) | DlbBoxDoubleClick_Inventory, bDlgBoxPress_Inventory | - | - | - | - |
| `CGame::_bCheckDraggingItemRelease` | Game.cpp:4595-4650 | 56 | comment: 해당 다이얼로그 위에 아이템이 드롭되었다. | CommandProcessor | EnableDialogBox, bItemDrop_Bank, bItemDrop_Character, bItemDrop_ExchangeDialog, bItemDrop_ExternalScreen, bItemDrop_IconPannel, bItemDrop_Inventory, bItemDrop_ItemUpgrade, bItemDrop_SellList, bItemDrop_SkillDialog | - | - | - |
| `CGame::bItemDrop_ExternalScreen` | Game.cpp:4654-4830 | 177 | drop of the dragged item onto ExternalScreen; comment: 아이템을 버릴 것인지 캐릭터에게 줄 것인지를 판단한다. | _bCheckDraggingItemRelease | EnableDialogBox, GetNpcName, _ItemDropHistory, bCheckItemOperationEnabled, bGetOwner, bSendCommand | DEF_COMMONTYPE_GIVEITEMTOCHAR, DEF_COMMONTYPE_ITEMDROP, MSGID_COMMAND_COMMON | 4, 17, 20 | - |
| `CGame::SetItemCount` | Game.cpp:5783-5797 | 15 | (name only) | - | - | - | - | - |
| `CGame::bItemDrop_IconPannel` | Game.cpp:7734-7758 | 25 | drop of the dragged item onto IconPannel; comment: 인벤토리에 놓았다면 | _bCheckDraggingItemRelease | bItemDrop_Character, bItemDrop_Inventory | - | - | - |
| `CGame::__bDecodeContentsAndBuildItemForSaleList` | Game.cpp:7973-8118 | 146 | comment: 아이템 이름 | _LoadShopMenuContents | CStrTok, pGet | - | - | - |
| `CGame::InitItemList` | Game.cpp:17689-17869 | 181 | comment: 아이템 순서 배열을 초기화한다. | GameRecvMsgHandler | - | - | - | - |
| `CGame::DisplayGold` | Game.cpp:20970-21003 | 34 | (name only) | DrawDialogBox_Exchange, DrawDialogBox_Inventory | - | - | - | - |
| `CGame::_bCheckItemByType` | Game.cpp:22843-22851 | 9 | TRUE if any inventory item has item type cType [V] | CommandProcessor | - | - | - | - |
| `CGame::_bIsItemOnHand` | Game.cpp:22898-22919 | 22 | TRUE if an equipped right-hand weapon is not a staff (appr2 weapon id 35-39) or id 27, or any left-hand or two-hand item is equipped (blocks casting) [V] | DlbBoxDoubleClick_Inventory, DlgBoxClick_Inventory, UseMagic | - | - | - | - |
| `CGame::_iCalcTotalWeight` | Game.cpp:22921-22944 | 24 | sum item weights; consumables and arrows weigh weight x count, Gold divided by 20 [V] | DrawDialogBox_Character | - | - | - | - |
| `CGame::_iGetTotalItemNum` | Game.cpp:23063-23072 | 10 | (name only) | DlgBoxClick_Bank, DlgBoxClick_Shop, DrawDialogBox_Shop | - | - | - | - |
| `CGame::bItemDrop_ExchangeDialog` | Game.cpp:23858-23903 | 46 | drop of the dragged item onto ExchangeDialog; comment: 이미 아이템이 설정되어 있어도 바꿀 수 없다. 안전 관계상 교환창을 다시 띄워야 한다. | _bCheckDraggingItemRelease | EnableDialogBox, bSendCommand | DEF_COMMONTYPE_SETEXCHANGEITEM, MSGID_COMMAND_COMMON | 17 | - |
| `CGame::_iGetBankItemCount` | Game.cpp:23967-23976 | 10 | (name only) | DlgBoxClick_NpcActionQuery, UpdateScreen_OnGame, bItemDrop_Bank | - | - | - | - |
| `CGame::_ItemDropHistory` | Game.cpp:24223-24268 | 46 | comment: 히스토리에 해당 아이템이 있는지 검색. | bItemDrop_ExternalScreen | - | - | - | - |
| `CGame::__bDecodeBuildItemContents` | Game.cpp:24271-24407 | 137 | comment: 아이템 이름 | _bDecodeBuildItemContents | CStrTok, pGet | - | - | - |
| `CGame::_bCheckCurrentBuildItemStatus` | Game.cpp:24410-24582 | 173 | comment: Element1 검사 | DlbBoxDoubleClick_Inventory, bDlgBoxPress_SkillDlg, bItemDrop_SkillDialog | - | - | - | - |
| `CGame::RetrieveItemHandler` | Game.cpp:31887-31986 | 100 | comment: 아이템을 성공적으로 되찾았다. | GameRecvMsgHandler | AddEventList, GetItemName, bSendCommand | DEF_MSGTYPE_REJECT, MSGID_REQUEST_SETITEMPOS | - | - |
| `CGame::EraseItem` | Game.cpp:31988-32027 | 40 | remove item and clear shortcuts [V] | NotifyMsg_DropItemFin_EraseItem, NotifyMsg_GiveItemFin_EraseItem, NotifyMsg_ItemDepleted_EraseItem | AddEventList, GetItemName | - | - | - |
| `CGame::GetItemName` | Game.cpp:34512-34638 | 127 | comment: Custom Made Item이다. | DlbBoxDoubleClick_Character, DlbBoxDoubleClick_Inventory, DrawDialogBox_Bank, DrawDialogBox_Exchange, DrawDialogBox_Fishing, DrawDialogBox_ItemDrop, DrawDialogBox_ItemUpgrade, DrawDialogBox_NpcActionQuery, +28 | - | - | - | - |
| `CGame::GetItemName` | Game.cpp:34640-34747 | 108 | comment: 희귀 아이템이다. | DlbBoxDoubleClick_Character, DlbBoxDoubleClick_Inventory, DrawDialogBox_Bank, DrawDialogBox_Exchange, DrawDialogBox_Fishing, DrawDialogBox_ItemDrop, DrawDialogBox_ItemUpgrade, DrawDialogBox_NpcActionQuery, +28 | - | - | - | - |
| `CGame::bItemDrop_Character` | Game.cpp:41965-41968 | 4 | drop of the dragged item onto Character; comment: \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/ | DlbBoxDoubleClick_Inventory, _bCheckDraggingItemRelease, bItemDrop_IconPannel | ItemEquipHandler | - | - | - |
| `CGame::bItemDrop_Inventory` | Game.cpp:41970-42065 | 96 | drop of the dragged item onto Inventory; comment: "기술 사용중에는 장착된 아이템을 해제할 수 없습니다." | _bCheckDraggingItemRelease, bItemDrop_IconPannel | AddEventList, GetItemName, PlaySound, bSendCommand | DEF_COMMONTYPE_RELEASEITEM, MSGID_COMMAND_COMMON, MSGID_REQUEST_SETITEMPOS | - | - |
| `CGame::bItemDrop_SellList` | Game.cpp:42067-42132 | 66 | drop of the dragged item onto SellList; comment: 이미 팔고자 하는 아이템 리스트에 있음 무시 | _bCheckDraggingItemRelease | AddEventList, EnableDialogBox, GetItemName | - | 17 | - |
| `CGame::bItemDrop_ItemUpgrade` | Game.cpp:42134-42158 | 25 | drop of the dragged item onto ItemUpgrade | _bCheckDraggingItemRelease | PlaySound | - | - | - |
| `CGame::bItemDrop_Bank` | Game.cpp:42160-42208 | 49 | drop of the dragged item onto Bank; comment: 존재하지 않는 아이템이라면 리턴. | _bCheckDraggingItemRelease | AddEventList, EnableDialogBox, _iGetBankItemCount, bSendCommand | DEF_COMMONTYPE_GIVEITEMTOCHAR, MSGID_COMMAND_COMMON | 17 | - |
| `CGame::ReleaseEquipHandler` | Game.cpp:48396-48405 | 10 | comment: "아이템 %s: 장착 해제 되었습니다." | ItemEquipHandler | AddEventList, GetItemName | - | - | - |
| `CGame::ItemEquipHandler` | Game.cpp:48407-48504 | 98 | comment: 아이템 관련 연산이 가능한 상태인지 판별 | UseShortCut, bItemDrop_Character | AddEventList, GetItemName, PlaySound, ReleaseEquipHandler, bCheckItemOperationEnabled, bSendCommand | DEF_COMMONTYPE_EQUIPITEM, MSGID_COMMAND_COMMON | - | - |
| `CItem::CItem` | Item.cpp:11-17 | 7 | comment: Construction/Destruction | - | - | - | - | - |
| `CItem::~CItem` | Item.cpp:19-22 | 4 | (name only) | - | - | - | - | - |
| `CItemName::CItemName` | ItemName.cpp:11-15 | 5 | comment: Construction/Destruction | - | - | - | - | - |
| `CItemName::~CItemName` | ItemName.cpp:17-20 | 4 | (name only) | - | - | - | - | - |

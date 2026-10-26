# Functions: Chat, commands, guild/party, filters

Source: `sources\client-2.20\` (librehelbreath ClientSources c01544d). 25 function definitions (including `#if` duplicate bodies). Generated from a col-0 signature scan, then reviewed by hand.

Column key: **Where** file:start-end (verified). **Purpose** from name pattern and the nearest source comment (inferred unless the row ends in [V], meaning re-checked against the function body by the 2026-10-25 review, or the overview says verified; comments are UTF-8 Korean in source, quoted as-is). **Callers/Callees** matched by bare name across all files (verified as text matches; same-named methods in different classes are merged). **Msgs** MSGID/DEF_NOTIFY/etc. names in the body (see `..\..\shared\messages\`). **Dlg** dialog ids enabled/disabled (ids in `..\data-structures.md`). **Files** path literals in the body.

## Overview

- `ChatMsgHandler` receives chat; `bCheckLocalChatCommand` handles client-side slash commands (e.g. `/showframe` per GlobalDef.h:50) before sending the rest as `MSGID_COMMAND_CHATMSG`. Filters: `CGameMonitor` badword list (badword.txt) and `CCurse` (Curse.txt, DEF_LANGUAGE > 2: Korea, English, Japan; Game.cpp:1002, 35063). `CMsg` is a chat/event line with timestamp.

## Function table

| Function | Where | Lines | Purpose | Callers | Callees | Msgs | Dlg | Files |
|---|---|---|---|---|---|---|---|---|
| `CCurse::CCurse` | Curse.cpp:25-28 | 4 | comment: Construction/Destruction | - | - | - | - | - |
| `CCurse::~CCurse` | Curse.cpp:31-34 | 4 | (name only) | - | - | - | - | - |
| `CCurse::LoadCurse` | Curse.cpp:36-57 | 22 | (name only) | bInit | - | - | - | - |
| `CCurse::filterChar` | Curse.cpp:59-84 | 26 | comment: hirakana | ConvertString, IsCurse | - | - | - | - |
| `CCurse::getField` | Curse.cpp:86-94 | 9 | (name only) | ConvertString, IsCurse | - | - | - | - |
| `CCurse::IsCurse` | Curse.cpp:97-131 | 35 | comment: filter characters to keep users from | ConvertString | filterChar, getField | - | - | - |
| `CCurse::ConvertString` | Curse.cpp:133-191 | 59 | comment: skip converting, if no curse is found. | UpdateScreen_OnGame | IsCurse, filterChar, getField | - | - | - |
| `CGame::ClearGuildNameList` | Game.cpp:4891-4899 | 9 | (name only) | CommonEventHandler, NotifyMsg_DismissGuildsMan, NotifyMsg_NewGuildsMan | - | - | - | - |
| `CGame::CreateNewGuildResponseHandler` | Game.cpp:5429-5448 | 20 | comment: 길드 생성요구에 대한 응답이 왔다. | GameRecvMsgHandler | - | DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT | - | - |
| `CGame::DisbandGuildResponseHandler` | Game.cpp:5594-5613 | 20 | comment: 길드 해산요구에 대한 응답이 왔다. | GameRecvMsgHandler | - | DEF_MSGTYPE_CONFIRM, DEF_MSGTYPE_REJECT | - | - |
| `CGame::_PutGuildOperationList` | Game.cpp:5679-5690 | 12 | 100-entry guild-operation queue push/shift [V] | NotifyMsg_BanGuildMan, NotifyMsg_DismissGuildApprove, NotifyMsg_DismissGuildReject, NotifyMsg_GuildDisbanded, NotifyMsg_JoinGuildApprove, NotifyMsg_JoinGuildReject, NotifyMsg_QueryDismissGuildPermission, NotifyMsg_QueryJoinGuildPermission | - | - | - | - |
| `CGame::_ShiftGuildOperationList` | Game.cpp:5692-5708 | 17 | 100-entry guild-operation queue push/shift [V] | DlgBoxClick_GuildOp | - | - | - | - |
| `CGame::AddEventList` | Game.cpp:5800-5889 | 90 | comment: v2.12 | ChatMsgHandler, ClearSkillUsingStatus, CommandProcessor, CreateScreenShot, DlbBoxDoubleClick_Character, DlbBoxDoubleClick_Inventory, DlgBoxClick_Bank, DlgBoxClick_Fish, +82 | - | - | - | - |
| `CGame::PutChatScrollList` | Game.cpp:15662-15676 | 15 | comment: m_dwTime이 사실 메시지의 종류를 의미한다. | ChatMsgHandler | CMsg | - | - | - |
| `CGame::ChatMsgHandler` | Game.cpp:15678-15802 | 125 | comment: 수신 거부 당한 캐릭터의 대화 내용은 적지 않는다. | GameRecvMsgHandler | AddEventList, CMsg, PutChatScrollList, _GetBackBufferDC, _ReleaseBackBufferDC, _RemoveChatMsgListByObjectID, bCheckExID, bCheckIMEString, bSetChatMsgOwner, iGetTextLengthLoc | - | - | - |
| `CGame::SetTopMsg` | Game.cpp:20003-20010 | 8 | timed blinking top banner (set / draw) [V] | CannotConstruct, DlgBoxClick_Commander, DlgBoxClick_Constructor, DlgBoxClick_Soldier, InitDataResponseHandler, MeteorStrikeComing, NotifyMsgHandler, NotifyMsg_EnemyKillReward, +1 | - | - | - | - |
| `CGame::DrawTopMsg` | Game.cpp:20012-20023 | 12 | timed blinking top banner (set / draw) [V] | UpdateScreen_OnGame | DrawShadowBox, PutAlignedString | - | - | - |
| `CGame::_RemoveChatMsgListByObjectID` | Game.cpp:22772-22781 | 10 | (name only) | ChatMsgHandler, CommandProcessor, LogEventHandler, MotionEventHandler, NotifyMsg_LevelUp, NotifyMsg_Skill | - | - | - | - |
| `CGame::FindGuildName` | Game.cpp:33620-33648 | 29 | guild-name cache lookup with LRU replacement [V] | DrawObjectName | - | - | - | - |
| `CGame::bCheckLocalChatCommand` | Game.cpp:34000-34121 | 122 | comment: 2002.11.1 안상하 추가. 프레임 보여주기. | bSendCommand | AddEventList, CMsg, CStrTok, pGet | - | - | - |
| `CGame::PointCommandHandler` | Game.cpp:34768-34800 | 33 | comment: 상하천재 | CommandProcessor, bDlgBoxPress_Inventory | AddEventList, PlaySound, bSendCommand | DEF_COMMONTYPE_MAGIC, DEF_COMMONTYPE_REQUEST_JOINPARTY, DEF_COMMONTYPE_REQ_USEITEM, MSGID_COMMAND_COMMON | - | - |
| `CGame::CommandProcessor` | Game.cpp:35701-37100 | 1400 | comment: 마우스로부터 입력된 명령을 분석하여 적절한 동작을 취한다. | UpdateScreen_OnGame | AddEventList, CMsg, ChangeGameMode, ClearSkillUsingStatus, EnableDialogBox, EndInputString, GetPlayerTurn, PlaySound, PointCommandHandler, RequestTeleportAndWaitData, +17 | DEF_OBJECTATTACK, DEF_OBJECTATTACKMOVE, DEF_OBJECTDAMAGEMOVE, DEF_OBJECTGETITEM, DEF_OBJECTMAGIC, DEF_OBJECTMOVE, +4 | 20 | - |
| `CGame::ShowEventList` | Game.cpp:45466-45537 | 72 | comment: 공지사항 메시지 | UpdateScreen_OnGame | PutString, _GetBackBufferDC, _ReleaseBackBufferDC | - | - | - |
| `CMsg::CMsg` | Msg.cpp:11-21 | 11 | comment: Construction/Destruction | ChatMsgHandler, CommandProcessor, CrusadeContributionResult, CrusadeWarResult, GrandMagicResult, MotionEventHandler, NotifyMsg_LevelUp, NotifyMsg_QuestReward, +10 | - | - | - | - |
| `CMsg::~CMsg` | Msg.cpp:23-26 | 4 | (name only) | - | - | - | - | - |

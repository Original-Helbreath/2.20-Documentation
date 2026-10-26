# Overloaded, straddling, colliding and dead commands

Judgement layer over the generated family files. Every claim cites the line it rests on; S = `sources/hgserver-librehelbreath/src/`, C = `sources/client-2.20/`. [V] read in code, [I] inferred.

## Contents
- Overloaded envelopes (one message, unrelated jobs)
- Overloaded sub-commands
- Commands that straddle systems
- Value collisions and aliases
- Dead, unreachable and silently dropped
- Build-switch gated

## Overloaded envelopes

| Message | Jobs it carries | Evidence |
|---|---|---|
| `MSGID_COMMAND_CHATMSG` | normal chat, 5 prefix channels, whisper, ~45 slash commands: GM admin, party (gate), guild ban, summon control, crusade start/end, energy sphere, emergency shutdown | [V] `ChatMsgHandler` S Game.cpp:10588-11047. Party create, getinfo and dismiss exist only as typed text (the client never builds `/createparty`, no string in C Game.cpp); invite, leave (iV1=0) and accept come from the party dialog via COMMONTYPE_REQUEST_JOINPARTY / REQUEST_ACCEPTJOINPARTY (C Game.cpp:19070, 19089, 19127; S 46749-46755), so `/deleteparty` (S 11006) and dialog leave reach the same handler |
| `MSGID_COMMAND_COMMON` | ~50 sub-commands across item, shop, trade, guild, party, quest, crusade, crafting, skills, magic | [V] switch S Game.cpp:15379-15593; one fixed frame (15343-15376) whose fields mean different things per command |
| `MSGID_COMMAND_MOTION` | walking, running, melee, dash attack, spell-cast animation, item pick-up, knock-back report | [V] S Game.cpp:1027-1125 |
| `MSGID_SERVERSTOCKMSG` (GSM) | cross-server chat and whisper, find-player, GM summon/mute/recall timer, every crusade sync | [V] `gate-relay.md`; senders in ChatMsgHandler, AdminOrder_*, crusade code |
| `MSGID_EVENT_COMMON` | ground items, spell visuals, guild-name wipe | [V] 4 COMMONTYPE values reused downstream (client cases C Game.cpp:4869-4885) |

## Overloaded sub-commands

| Command | Unrelated jobs | Evidence |
|---|---|---|
| `DEF_COMMONTYPE_GIVEITEMTOCHAR` | six outcomes: (a) item 88 to a guild-master player -> join request; (b) item 89 to the master of your own guild (not crusade) -> leave request; (c) NPC Howard -> warehouse deposit; (d) NPC Kennedy + item 89 -> immediate leave, -300 exp; (e) any other NPC or empty tile -> item dropped on the floor (no hand-in, no Cityhall); (f) plain gift to a player (self-gift ignored) | [V] `GiveItemHandler` S Game.cpp:17094 (88: 17410-17420, 89: 17431-17440, Howard 17314/17576, Kennedy 17598-17624, other-NPC drop 17332-17342, 17625+) sends NOTIFY_QUERY_JOINGUILDREQPERMISSION / QUERY_DISMISSGUILDREQPERMISSION to the master player and ITEMTOBANK via bSetItemToBankItem; warehouse withdraw is a separate MSGID (`MSGID_REQUEST_RETRIEVEITEM`) |
| `DEF_COMMONTYPE_REQ_USEITEM` | potions, scrolls (teleport, recall), map items (NOTIFY_SHOWMAP), skill manuals (TrainSkillResponse S Game.cpp:30793), dye, flags, construction kits, seeds | [V] `UseItemHandler` S Game.cpp:30626 switches on item effect type |
| `DEF_COMMONTYPE_ITEMDROP` / `MAGIC` / `SETITEM` / `CLEARGUILDNAME` | same value used as a client command and as a server event (ITEMDROP, MAGIC) or event only (SETITEM, CLEARGUILDNAME) | [V] server cases 15519, 15580; event senders `SendEventToNearClient_TypeB(MSGID_EVENT_COMMON, DEF_COMMONTYPE_...)` (23 ITEMDROP, 4 MAGIC, 5 SETITEM, 4 CLEARGUILDNAME sites) |
| `DEF_COMMONTYPE_JOINGUILDAPPROVE/REJECT`, `DISMISSGUILDAPPROVE/REJECT` | client command and NOTIFY sub-type | [V] cases in ClientCommonHandler 15554-15568 and in SendNotifyMsg 18956-18971; client NotifyMsgHandler C Game.cpp:31351-31355 |
| Spell casting | split over two messages: MOTION `DEF_OBJECTMAGIC` (animation, spell in dX) then COMMON `DEF_COMMONTYPE_MAGIC` (target, `iV3-100` = spell) | [V] S Game.cpp:1112-1121 and 15580-15582 |
| `DEF_OBJECTNULLACTION` | generic "appearance changed" broadcast used by ~25 handlers (equip, invisibility, berserk, guild change, hunter mode) | [V] uses listed in `motion.md` |
| `DEF_NOTIFY_PLAYERNOTONGAME` | shared "name not found" reply for /pf, /rep, /attack, /closeconn, /ban, /joinparty (S Game.cpp:34870, 36251, 36528, 36714, 36795, 51394). `/fi` never reports not-found: its send is commented (S 34695) and a miss gets silence | [V] `notify-types.md` row ([COMMANDS-018](../../issues/commands.md#commands-018-fi-gives-no-reply-when-the-player-is-not-found)) |
| `MSGID_COMMAND_CHECKCONNECTION` | keep-alive and client clock drift (speed-hack) check | [V] `CheckConnectionHandler` S Game.cpp:48937 |

## Commands that straddle systems

| Command | Systems | Why |
|---|---|---|
| `^` chat prefix | chat, guild, gm, gate | guild chat on this server, cross-server guild chat via GSM_CHATMSG, and admin global notice (GSM type 10) in one branch; also checks SP >= 5 but charges 3 (S Game.cpp:10654-10721; [CHAT-004](../../issues/chat.md#chat-004-guild-relay--is-sent-before-the-level-and-sp-gate)) |
| `~` chat prefix | chat (was listed as chat + crusade) | nationality channel: every player with the same `m_cSide`, neutral included, no crusade condition (S Game.cpp:10724-10741, 11146-11149); reassigned to chat, no longer a straddler |
| `/ban` | guild via chat | guild management as a chat command; success notify dropped |
| `/hold`, `/tgt`, `/free` | magic/entity via chat | summon AI control is only reachable by typing |
| `/begincrusadetotalwar`, `/endcrusadetotalwar`, `/energysphere` | crusade, gm, gate | GM chat starts world events that sync via GSM |
| `REQUEST_SELECTCRUSADEDUTY`, `SETGUILDTELEPORTLOC`, `SETGUILDCONSTRUCTLOC`, `GUILDTELEPORT`, `REQUEST_HELP` | crusade, guild, map | crusade roles are tied to guild rank and guild GUID |
| `REQ_GETHEROMANTLE`, `REQ_GETOCCUPYFLAG`, `REQ_GETREWARDMONEY` | crusade/pk-law, item, shop | enemy-kill counters converted to items or gold at the Cityhall |
| `REQ_PURCHASEITEM` | shop, guild | also used by the guild menu (C Game.cpp:5292, 5313) |
| `MSGID_REQUEST_CREATENEWGUILD`/`DISBANDGUILD` | guild, session | same ID on client->game and game->Log links |
| `DEF_OBJECTGETITEM` | item, movement | pick-up is a motion |
| `DEF_OBJECTDAMAGEMOVE` / `NOTIFY_DAMAGEMOVE` | combat, movement | the victim's client is told to move itself and reports back |
| `MSGID_REQUEST_PANNING` | movement, gm | camera pan, only in GM observer mode (S Game.cpp:46165) |
| `DEF_NOTIFY_SP` | character, chat | shout/guild/party chat costs stamina |

Full matrix of secondary owners: `by-system.md` ("also serves" lines).

## Value collisions and aliases

| Value | Names | Effect |
|---|---|---|
| 0x0A25 | `DEF_COMMONTYPE_CLEARGUILDNAME` (S->C event) / `DEF_COMMONTYPE_REQ_GETOCCUPYFIGHTZONETICKET` (C->S) | no wire conflict: different parent messages |
| 0x0A30 | `DEF_COMMONTYPE_REQUEST_ACCEPTJOINPARTY` / `REQUEST_CREATENEWPARTY` (dead) | old command value recycled |
| 0x0A19 | `REQ_CREATEPOTION` (server) / `REQ_CREATEPORTION` (client) | spelling alias, live |
| 0x0BA5 | `DEF_NOTIFY_GIZONITEMCANGE` (server) / `GIZONEITEMCHANGE` (client) | spelling alias, live |
| 0x0FA314DB, 0x0FA314DC, 0x2900AD30 | EVENT_COMMON/MAGICCONFIG, COMMAND_COMMON/SKILLCONFIG, REQUEST_SELLITEMLIST/REQUEST_IPTIME | on different links; see `shared/messages/README.md` |
| 0x0A00-0x0A07 | DEF_COMMONTYPE_* vs DEF_LOGRESMSGTYPE_* | different parents |
| 0x0A03 | `DEF_COMMONTYPE_REQ_LISTCONTENTS` (dead) vs `DEF_LOGRESMSGTYPE_INPUTKEYCODE` (client header) vs `INPUTKEYCODEFAILED` (server header) | server-side alias of INPUTKEYCODE; other-subtypes row reads "unused" (S NetMessages.h:309) ([COMMANDS-008](../../issues/commands.md#commands-008-req_listcontents-and-response_joinparty-are-defined-only)) |
| 0x0A04-0x0A07 | client header: REQ_PURCHASEITEM / GIVEITEMTOCHAR / JOINGUILD* vs LOGRES REALACCOUNT..LESSTHENFIFTEEN | different parents (C NetMessages.h:293-302) |
| 0x0F14, 0x0F15 | `DEF_MSGTYPE_CONFIRM/REJECT` vs `DEF_LOGRESMSGTYPE_CONFIRM/REJECT` | identical values on different parents |
| 0x0F1C-0x0F1F | DEF_LOGRESMSGTYPE_* vs DEF_ENTERGAMEMSGTYPE_* | different parents |

## Dead, unreachable and silently dropped

What the code does now for each command that is unused, stubbed or dropped. Each row is catalogued as an issue in [issues/commands.md](../../issues/commands.md).

| Command | Kind | Evidence |
|---|---|---|
| `DEF_COMMONTYPE_REQ_TRAINSKILL` | handler body commented, client never sends | S Game.cpp:15541-15543 ([COMMANDS-001](../../issues/commands.md#commands-001-req_trainskill-handler-is-commented-out-and-no-client-sender-exists)) |
| `DEF_COMMONTYPE_REQUEST_CREATENEWPARTY` | case commented | S Game.cpp:15439-15441 ([COMMANDS-002](../../issues/commands.md#commands-002-request_createnewparty-case-commented-its-reply-notify-has-no-sender)) |
| `DEF_COMMONTYPE_REQ_GETOCCUPYFLAG` | live handler, no client sender | S Game.cpp:15471; no C use ([COMMANDS-003](../../issues/commands.md#commands-003-req_getoccupyflag-has-a-live-handler-and-no-client-sender)) |
| `DEF_COMMONTYPE_REQUEST_HELP` | live handler, no client sender, replies NOTIFY_HELP/HELPFAILED have no client case | S Game.cpp:15415, 49268-49288 ([COMMANDS-004](../../issues/commands.md#commands-004-request_help-and-its-helphelpfailed-replies-are-unreachable)) |
| `DEF_COMMONTYPE_BANGUILD`, `DEF_NOTIFY_SUCCESSBANGUILDMAN` | passed to SendNotifyMsg, which has no case for them and no default: nothing sent | S Game.cpp:36782, 36785; switch 17715-19025 (no default in the main switch; the `default:` at 17800 belongs to an inner switch) ([COMMANDS-005](../../issues/commands.md#commands-005-banguild-and-successbanguildman-are-passed-to-sendnotifymsg-which-has-no-case)) |
| `DEF_COMMONTYPE_REQUEST_CREATENEWPARTY` consequence: `DEF_NOTIFY_RESPONSE_CREATENEWPARTY` | its only sender `CreateNewPartyHandler` has no caller (call at 15440 is commented); client handler exists | S Game.cpp:46728, 46735, 15440, 18236; C Game.cpp:30829 ([COMMANDS-002](../../issues/commands.md#commands-002-request_createnewparty-case-commented-its-reply-notify-has-no-sender)) |
| `MSGID_ITEMLOG` | packing case (comment: "not used") and gate handler, no `SendMsgToGateServer` caller | S Game.cpp:20654-20657; G GateCore.cpp:410 ([COMMANDS-006](../../issues/commands.md#commands-006-msgid_itemlog-has-a-packing-case-and-gate-handler-but-no-sender)) |
| `MSGID_REQUEST_SETACCOUNTINITSTATUS` | only a case label shared with SETACCOUNTWAITSTATUS, no caller | S Game.cpp:4281-4282 ([COMMANDS-007](../../issues/commands.md#commands-007-msgid_request_setaccountinitstatus-is-only-a-shared-case-label)) |
| `DEF_NOTIFY_HELP`, `HELPFAILED` | unreachable: only replies to REQUEST_HELP, which the client never sends | S Game.cpp:15415 ([COMMANDS-004](../../issues/commands.md#commands-004-request_help-and-its-helphelpfailed-replies-are-unreachable)) |
| `DEF_COMMONTYPE_REQ_LISTCONTENTS`, `RESPONSE_JOINPARTY` | defined only | headers ([COMMANDS-008](../../issues/commands.md#commands-008-req_listcontents-and-response_joinparty-are-defined-only)) |
| `DEF_NOTIFY_CANNOTJOINMOREGUILDSMAN`, `LIMITEDLEVEL`, `CHARISMA` | packing case and client case exist, no server call | `notify-types.md` ([COMMANDS-009](../../issues/commands.md#commands-009-cannotjoinmoreguildsman-limitedlevel-and-charisma-notifies-are-never-sent)) |
| `DEF_NOTIFY_SKILLTRAINFAIL` | no SendNotifyMsg case; only a hand-built send inside a commented `/* */` block, plus a client case | S Game.cpp:25443-25525 (send at 25498) ([COMMANDS-010](../../issues/commands.md#commands-010-skilltrainfail-is-sent-only-inside-a-commented-block)) |
| `DEF_NOTIFY_WAITFORGUILDOPERATION`, `EVENTMSGSTRING`, `NEWDYNAMICOBJECT`, `DELDYNAMICOBJECT`, `DEBUGMSG`, `DUTYSELECTED` | never sent, never handled | `notify-types.md` ([COMMANDS-011](../../issues/commands.md#commands-011-six-notifies-are-never-sent-and-never-handled)) |
| `MSGID_ADMINUSER` | empty handler, no sender | S Game.cpp:35831-35836 ([COMMANDS-012](../../issues/commands.md#commands-012-msgid_adminuser-and-msgid_manager_shutdown-handlers-do-nothing)) |
| `MSGID_MANAGER_SHUTDOWN` | handler starts with `return ;` | S Game.cpp:40799 ([COMMANDS-012](../../issues/commands.md#commands-012-msgid_adminuser-and-msgid_manager_shutdown-handlers-do-nothing)) |
| `MSGID_REQUEST_SAVEARESDENOCCUPYFLAGDATA` / `...ELVINE...`, `MSGID_COLLECTEDMANA` | sends only in comments | S Game.cpp:14865-14866, 48142 ([COMMANDS-013](../../issues/commands.md#commands-013-occupy-flag-save-requests-and-msgid_collectedmana-are-sent-only-in-comments)) |
| `/dissmiss` | empty handler | S Game.cpp:36804-36807 ([COMMANDS-014](../../issues/commands.md#commands-014-dissmiss-and-polymorph-are-stubs)) |
| `/polymorph` | stub, ignores its argument | S Game.cpp:38143-38145 ([COMMANDS-014](../../issues/commands.md#commands-014-dissmiss-and-polymorph-are-stubs)) |
| Korean slash aliases | cannot match in this UTF-8 export (byte length vs memcmp length) | `chat-commands.md` ([COMMANDS-015](../../issues/commands.md#commands-015-korean-slash-aliases-cannot-match-in-the-utf-8-export)) |
| 30 MSGIDs | defined only (mostly Main/World Log internals) | `msgids.md` status column ([COMMANDS-016](../../issues/commands.md#commands-016-about-30-msgids-are-defined-but-unused)) |

## Build-switch gated (shipped server: DEF_CHINESE on, DEF_TAIWAN/DEF_TESTSERVER off)

| Command | Switch | Effect |
|---|---|---|
| `/who` | DEF_CHINESE | admin >= 3 instead of >= 1 (S Game.cpp:10753-10757) |
| `/shutup`, `/setattackmode`, `/createitem`, `/enableadmincreateitem`, `/setobservermode` | DEF_TAIWAN (off) | different admin levels; see `chat-commands.md` |
| every `+enable` GM command | DEF_TESTSERVER (off) | needs `/enableadmincommand 0909114` first |
| `/closeconn` | `DEF_TIAWAN` typo | the Taiwan branch can never compile (S Game.cpp:36665) ([COMMANDS-017](../../issues/commands.md#commands-017-closeconn-taiwan-branch-uses-the-undefined-name-def_tiawan)) |
| `MSGID_WLSERVERFILECONTENTS` | DEF_DEFENCEHACKING (on via DEF_CHINESE, GlobalDef.h) | live in shipped build |
| `DEF_NOTIFY_FIGHTZONERESERVE` decode use | DEF_DBGAMESERVER | both #ifdef branches use it |
| client `MSGID_REQUEST_CREATENEWACCOUNT`, `REQUEST_INPUTKEYCODE`, `REQUEST_CHANGEPASSWORD` variants | DEF_MAKE_ACCOUNT / DEF_LANGUAGE==2 | Korean-build account features; English client DEF_LANGUAGE 4 |
| client `/enableadmincommand` pre-type on Ctrl+Q | _DEBUG | C Game.cpp:28669-28676 |

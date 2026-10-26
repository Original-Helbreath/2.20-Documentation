# System: guild

*Verdict (size review): cohesive (creation, membership and the guild file form one lifecycle; guild war is a short section that points to arena); single file with a Contents block.*

Helbreath 2.20. Paths: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`. World log server has no
source: `research/servers/official-worldserver.md` (decompiled behaviour). Tags: [V] read in source, [I] inferred,
[U] unverified. Function bodies: `research/game-server/functions/guild-party.md`.

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flow - 7. Rules - 8. Couplings -
9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose

A guild is a permanent, nation-bound player organisation with one master. It is the unit of the crusade command chain
(only a guild master can be a crusade commander; commanders lead their own guild) [V S Game.cpp:49046, 49278-49279],
the unit of the arena (guild-vs-guild in fight zones) [V S Game.cpp:37604-37616], and a social channel (`@`, `^`). The
player experience: buy a creation at the City Hall, recruit by having applicants hand over a "guild admission ticket"
(item 88) which pops a yes/no box on the master's screen, leave by handing the master a "secession ticket" (item 89) or
giving it to the guild clerk NPC Kennedy, and see guild names over heads [V S Game.cpp:17410-17447, 17598-17623]. The
guild's existence and member list live in a text file on the world log server; membership on each character is
re-validated at every login against that file's GUID [V doc official-worldserver.md 3.4, PLAYERDATA guild-status].

## 2. Subsystems

| # | Subsystem | Intent |
|---|---|---|
| 2.1 | Creation and disband | expensive, gated founding through the world log (single source of truth for names) |
| 2.2 | Admission | ticket item 88 given to the master -> master approves or rejects |
| 2.3 | Departure | ticket 89 to the master (approve/reject) or to NPC Kennedy (self-service, exp penalty); `/ban` by the master |
| 2.4 | Guild file and login validation | world log `Guild\AscII<c>\<guild>.txt`; guild-status byte clears members of a deleted guild |
| 2.5 | Guild events to members | `SendGuildMsg` notifies online members on this server |
| 2.6 | Guild identity display | rank/name in object appearance; `REQGUILDNAME` lookups; client name cache |
| 2.7 | Guild combat relations | same-guild never enemy; other guilds are enemies inside fight zones (guild war) |
| 2.8 | Guild crusade roles | commander duty, guild teleport point, construction point, construction points (see crusade doc) |

## 3. Functions

| Sub | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | `CGame::DlgBoxClick_GuildMenu` | client | C Game.cpp:5188-5427 | City Hall guild menu: create (rank -1 only), disband (rank 0), buy tickets, fight-zone menu (rank 0) [V] |
| 2.1 | `CGame::RequestCreateNewGuildHandler` | game server | S Game.cpp:16632-16717 | gates, GUID, store name/location, `MSGID_REQUEST_CREATENEWGUILD` to Log [V] |
| 2.1 | world log `HandleCreateNewGuild` | world log | 0x409170 | create the guild file or reject if it exists [V doc] |
| 2.1 | `CGame::ResponseCreateNewGuildHandler` | game server | S Game.cpp:16547-16630 | rank 0, take 1,000,000 gold (Chinese limit), reply [V] |
| 2.1 | `CGame::RequestDisbandGuildHandler`, `ResponseDisbandGuildHandler` | game server | S Game.cpp:16720-16818 | master only -> Log deletes file -> clear local members [V] |
| 2.1 | `CreateNewGuildResponseHandler`, `DisbandGuildResponseHandler` | client | C Game.cpp:5429, 5594 | dialog result [V doc] |
| 2.2 | `CGame::GiveItemHandler` item 88 branch | game server | S Game.cpp:17410-17428 | applicant gives ticket to master -> `QUERY_JOINGUILDREQPERMISSION` [V] |
| 2.2 | `CGame::DlgBoxClick_GuildOp`, `_PutGuildOperationList` | client | C Game.cpp:5712-5781, 5679-5708 | master's queue of up to 100 pending join/leave requests [V doc] |
| 2.2 | `CGame::JoinGuildApproveHandler` / `JoinGuildRejectHandler` | game server | S Game.cpp:19039-19111 | apply join, `NEWGUILDSMAN` to Log, notify guild [V] |
| 2.3 | `GiveItemHandler` item 89 to master | game server | S Game.cpp:17431-17447 | `QUERY_DISMISSGUILDREQPERMISSION` [V] |
| 2.3 | `GiveItemHandler` item 89 to Kennedy | game server | S Game.cpp:17598-17623 | self-leave, -300 exp [V] |
| 2.3 | `CGame::DismissGuildApproveHandler` / `DismissGuildRejectHandler` | game server | S Game.cpp:19113-19175 | apply leave, `DELGUILDSMAN` to Log [V doc] |
| 2.3 | `CGame::UserCommand_BanGuildsman` | game server | S Game.cpp:36724-36800 | `/ban name` [V] |
| 2.3 | `CGame::UserCommand_DissmissGuild` | game server | S Game.cpp:36804-36807 | `/dissmiss`, empty [V doc] |
| 2.4 | world log `HandleGuildAddGuildsman` / `HandleGuildDelGuildsman` / `CheckGuildStatusForPlayerData` | world log | 0x409d30, 0x409fa0, 0x403b10 | member list edits; guild-status byte at login [V doc] |
| 2.4 | `RequestInitDataHandler` guild check | game server | S Game.cpp:4937-4946 | guild-status 0 -> clear guild, notify `GUILDDISBANDED` [V] |
| 2.5 | `CGame::SendGuildMsg` | game server | S Game.cpp:20419-20495 | notify online same-guild members; for `GUILDDISBANDED` also clears them [V S 20446-20459] |
| 2.5 | `CGame::GuildNotifyHandler` | game server | S Game.cpp:20497-20514 | `MSGID_GUILDNOTIFY` from other servers, not implemented [V doc] |
| 2.6 | `CGame::RequestGuildNameHandler` | game server | S Game.cpp:52597-52612 | answer `REQGUILDNAME` [V doc] |
| 2.6 | `CGame::FindGuildName`, `ClearGuildNameList` | client | C Game.cpp:33620-33648, 4891 | LRU name cache for drawing [V doc] |
| 2.6 | `NotifyMsg_NewGuildsMan`, `NotifyMsg_JoinGuildApprove/Reject`, `NotifyMsg_GuildDisbanded` | client | C Game.cpp:44757, 25447, 25471, 25097 | UI updates [V] |
| 2.7 | `iGetPlayerRelationship`, `iGetPlayerABSStatus`, `iGetPlayerRelationship_SendEvent` | game server | S Game.cpp:37498, 37686, 37837 | relationship codes 3 member / 5 master / 2 enemy in fight zones / 6 other master [V] |
| 2.8 | `CalculateGuildEffect` | game server | S Game.cpp:20249-20295 | guild co-op exp share, disabled by early return [V doc] |

## 4. Commands and messages

- Client -> game: `MSGID_REQUEST_CREATENEWGUILD`, `MSGID_REQUEST_DISBANDGUILD` (own MsgIDs, dispatched S Game.cpp:15054-15060);
  `MSGID_COMMAND_COMMON` with `JOINGUILDAPPROVE/REJECT`, `DISMISSGUILDAPPROVE/REJECT` (pString = applicant name)
  [V S 15554-15568], `REQGUILDNAME` [V S 15386]; `COMMONTYPE_GIVEITEMTOCHAR` carries the tickets; `REQ_PURCHASEITEM`
  buys tickets (C Game.cpp:5292, 5313). Slash: `/ban name`, `/dissmiss` (dead). Chat `@`, `^`.
- Game -> world log: `MSGID_REQUEST_CREATENEWGUILD` (char, guild 20, location 10, GUID), `REQUEST_DISBANDGUILD`,
  `REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN` / `_DELGUILDSMAN` (char, guild) [V doc official-worldserver.md 3.4].
- World log -> game: `RESPONSE_CREATENEWGUILD`, `RESPONSE_DISBANDGUILD` (confirm/reject + char) [V S 15116-15124].
- Game -> client: `RESPONSE_CREATENEWGUILD/DISBANDGUILD`; notifies `QUERY_JOINGUILDREQPERMISSION`,
  `QUERY_DISMISSGUILDREQPERMISSION`, `NEWGUILDSMAN`, `DISMISSGUILDSMAN`, `GUILDDISBANDED`, `NOGUILDMASTERLEVEL`,
  `CANNOTBANGUILDMAN`, `REQGUILDNAMEANSWER`; the common types `JOINGUILDAPPROVE/REJECT`, `DISMISSGUILDAPPROVE/REJECT`,
  `BANGUILD` are also sent as notifies [V S 18946-18978, 19069, 36785]; `EVENT_COMMON CLEARGUILDNAME` to nearby clients.
- Never sent: `CANNOTJOINMOREGUILDSMAN`, `WAITFORGUILDOPERATION`, `SUCCESSBANGUILDMAN` (no SendNotifyMsg case),
  `EVENTMSGSTRING` (no caller passes it) [V doc notify-types.md] ([GUILD-007](../issues/guild.md#guild-007-dead-or-unwired-guild-features)).
- `MSGID_GUILDNOTIFY` (inter-server guild event): received, ignored [V doc].

## 5. Data

| Field | Where | Meaning |
|---|---|---|
| `m_cGuildName[22]` | S Client.h:64 | name, "NONE" when none; name is stored before Log confirms creation [V S 16704-16705] |
| `m_iGuildRank` | S Client.h:66 | -1 none, 0 master, `DEF_GUILDSTARTRANK` 12 member; no other ranks are ever set [V grep: only -1/0/12 assignments] |
| `m_iGuildGUID` | S Client.h:67 | `wYear+wMonth+wDay+wHour+wMinute+timeGetTime()` at creation [V S 16710-16711] |
| `m_cLocation` | S Client.h | guild nation copied from the creating map's location name, and from master to joiners [V S 16708, 19064] |
| char file `character-guild-name/-rank/-GUID` | world log | persisted [V doc official-worldserver.md] |
| `Guild\AscII<c>\<guild>.txt` | world log | `[GUILD-INFO] guildmaster-name, guild-GUID, guild-location; [GUILDSMAN] guildsman-name = ...` [V doc] |
| `CGuildsMan {m_cName, m_iRank}` | S GuildsMan.h | class included by Client.h, no use found [V] |
| `DEF_MAXGUILDSMAN` 128 | S Client.h:27 | defined, never referenced in .cpp -> no member cap [V grep] |
| `DEF_MAXGUILDS` 1000 | S Game.h:154 | size of `m_pGuildTeleportLoc` (crusade) [V] |
| tickets: item ID 88 (admission), 89 (secession) | Item.cfg | [V S 17410, 17431] |

## 6. Flow

**F1 Create.** City Hall dialog 7, mode 0 -> "create" only if client rank == -1 [V C Game.cpp:5204] -> name entry ->
`MSGID_REQUEST_CREATENEWGUILD` -> server: not in crusade [V S 16642]; rank must be -1 (else silent) [V 16661]; Chinese limit
(active: DEF_CHINESE -> DEF_CHINESEGUILDLIMIT, S GlobalDef.h:67): level >= 100, CHR >= 20, not neutral, not hunter, gold
>= 1,000,000, not in an enemy/PK town state [V 16669-16673]; else reject reply -> store name, location, GUID; send to Log
[V 16704-16714] -> world log creates the file or rejects on duplicate name -> `ResponseCreateNewGuildHandler`: rank 0,
gold deducted now, reply confirm [V 16575-16586]. The handler acts only for a client that still has level >= 100
(>= 20 without the Chinese limit) and Charisma >= 20 [V 16563-16570]; the deduction is `dwGoldCount - 1000000` [V 16584-16586]
(silent branch and unchecked deduction: [GUILD-005](../issues/guild.md#guild-005-create-response-is-silent-when-the-gate-fails-and-the-gold-is-deducted-without-a-re-check)).

**F2 Join.** Applicant (rank -1, non-neutral, same side) gives ticket 88 to a master (rank 0) [V S 17415-17418] -> ticket
consumed; master gets `QUERY_JOINGUILDREQPERMISSION` with applicant name -> client queues it (dialog 8) -> master clicks
approve -> `JOINGUILDAPPROVE name` -> `JoinGuildApproveHandler`: target must be online here and same side [V 19053]; copy
guild name, GUID, location, rank 12 [V 19057-19066]; `NEWGUILDSMAN` to Log; applicant gets the approve notify with guild
name and rank [V 18956-18967]; guild notified; appearance refresh.

**F3 Leave.** (a) ticket 89 to the master (not in crusade, same guild, applicant is a member) -> master's approve ->
`DismissGuildApproveHandler` -> `DELGUILDSMAN` to Log [V S 17431-17447; doc 19113]. (b) ticket 89 to NPC Kennedy (not in
crusade, member not master) -> guild cleared, appearance refresh, -300 exp [V 17598-17623]. (c) `/ban name` by the master:
same guild name required -> `DELGUILDSMAN`, guild notified, target cleared, `BANGUILD` to target, nearby clients clear the
cached name [V 36734-36789].

**F4 Disband.** Master (rank 0, name matches) -> `REQUEST_DISBANDGUILD` -> Log deletes the file -> `ResponseDisbandGuildHandler`:
`SendGuildMsg(GUILDDISBANDED)` clears every online member on this server, master cleared, nearby `CLEARGUILDNAME`
[V S 16736-16785, 20446-20459]. Members on other servers and offline members are cleared at their next login, because the
world log's guild-status byte is 0 when the file is gone [V S 4937-4946; doc CheckGuildStatusForPlayerData].

## 7. Rules and invariants

| Rule | Value | Where |
|---|---|---|
| create (shipped, Chinese limit) | level 100, CHR 20, 1,000,000 gold, side, not hunter, in town | S 16669-16673 [V] |
| create (no Chinese limit) | level 20, CHR 20, side, not hunter, in town | S 16676-16679 [V] |
| create / disband in crusade | refused silently | S 16642, 16724 [V] |
| leave by ticket 89 in crusade | refused (falls through to normal give/drop) | S 17431, 17602 [V] |
| join in crusade | allowed (comment v2.03) | S 17407 [V] |
| join requires | applicant rank -1, non-neutral, same side as master (client also checks) | S 17415-17418, 19053 [V] |
| Kennedy leave penalty | -300 exp (floor 0) | S 17620-17621 [V] |
| ranks | 0 master, 12 member | S 16578, 19066 [V] |
| member cap | none enforced (`DEF_MAXGUILDSMAN` unused) | [V grep] |
| login validation | game server clears the guild when `cGuildStatus == 0` (S 4937-4946 [V]); the missing-file and GUID-difference tests are on the world log side [V doc] |

## 8. Couplings

- **crusade** - commander duty needs rank 0 (`SelectCrusadeDutyHandler` S 49046); `RequestHelpHandler` finds the master
  commander by `m_iGuildGUID` (S 49278); guild teleport/construct points are keyed by `m_iGuildGUID`
  (`m_pGuildTeleportLoc[DEF_MAXGUILDS]`); construction points from war-unit kills go to the guild's commander (S 14650-14668,
  19477-19496); GSM_SETGUILDTELEPORTLOC / CONSTRUCTIONPOINT carry the GUID across servers.
- **arena** - fight-zone menu sits in the guild dialog and is master-only in the client (C 5240); in fight zones other
  guilds are enemies (S 37613-37668).
- **combat / pk-law (other cluster)** - relationship codes in `iGetPlayerRelationship*` (same guild = never enemy).
- **chat** - `@`/`^` by guild name / GUID.
- **items / trade (other cluster)** - tickets are items handled in `GiveItemHandler`; ticket purchase via `REQ_PURCHASEITEM`.
- **persistence (infrastructure)** - world log guild files; guild-status byte in `RESPONSE_PLAYERDATA`.
- **entity / presentation** - appearance refresh (`SendEventToNearClient_TypeA` NULLACTION), `CLEARGUILDNAME`, `REQGUILDNAME`.

## 9. Current behaviour vs intent

Defects are in [issues/guild.md](../issues/guild.md); the neutral lines are:

1. `JoinGuildApproveHandler` and `DismissGuildApproveHandler` check only that the target is online on the same side; the ticket handshake exists only in the client ([GUILD-001](../issues/guild.md#guild-001-join-and-dismiss-approval-have-no-server-side-authority-check)).
2. Kennedy leave sends no `DELGUILDSMAN`, so the guild file keeps a stale member line ([GUILD-002](../issues/guild.md#guild-002-kennedy-leave-does-not-update-the-guild-file)).
3. `SendGuildMsg` is local and `GuildNotifyHandler` is unimplemented, so disband reaches only members on this server ([GUILD-003](../issues/guild.md#guild-003-disband-and-guild-events-reach-only-this-server)).
4. The master can `/ban` himself, as the same-guild-name test passes ([GUILD-004](../issues/guild.md#guild-004-master-can-ban-himself)).
5. The requested name is stored in `m_cGuildName` with rank -1 before the Log answers (S 16704); a reject resets it. The create response is silent when the client no longer meets the gate, and the gold deduction is not re-checked ([GUILD-005](../issues/guild.md#guild-005-create-response-is-silent-when-the-gate-fails-and-the-gold-is-deducted-without-a-re-check)).
6. The GUID is the sum of date fields plus `timeGetTime()` (S 16711) ([GUILD-006](../issues/guild.md#guild-006-weak-guild-guid)).
7. [V] Guild location comes from the map, not the player's side (S 16708); with the "in town" gate this is the own town in practice [I].
8. `/dissmiss` is empty, `CalculateGuildEffect` is disabled by an early return, several notifies are never sent and `CGuildsMan` is unused ([GUILD-007](../issues/guild.md#guild-007-dead-or-unwired-guild-features)); there is no member cap ([GUILD-008](../issues/guild.md#guild-008-no-guild-member-cap)).
9. Already-in-guild create and non-master disband only log on the server; the client waits ([GUILD-009](../issues/guild.md#guild-009-silent-failures-on-create-and-disband)).

## 10. Open questions

- Ticket names: item 88 is the guild join application and 89 the guild withdrawal application (2.03 Item.cfg, cp949
  `길드가입신청서` / `길드탈퇴신청서`, category 6 type 9 rows; 3.51 Item.cfg calls them `GuildAdmissionTicket` /
  `GuildSecessionTicket`). The price column was not mapped to a field, so prices stay [U]. No official 2.20 Item.cfg exists;
  2.03 is the chosen copy (`research/shared/formats/config-files.md`).
- Whether the world log rejects `NEWGUILDSMAN` for a non-existent guild file (decomp detail) [U].
- Guild war beyond fight-zone relationships: `DEF_GUILDWARMODE` (off, S GlobalDef.h:8) disables force-recall in fight
  zones, blocks some spells and also gates exchange and other paths [V S 2150, 20853, 933, 23756, 40839, 47124]; whether official servers ever ran with it is unknown [U].

## 11. Doc discrepancies

None against the code. Addition to `guild-party.md`: `ResponseDisbandGuildHandler`'s member clearing happens inside
`SendGuildMsg` (S 20446-20459), not in the handler itself.

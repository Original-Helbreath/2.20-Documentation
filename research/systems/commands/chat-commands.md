# Slash and prefix chat commands

All server chat commands travel inside MSGID_COMMAND_CHATMSG and are parsed in `ChatMsgHandler` S Game.cpp:10539-11225. Common gates on every chat line (10547-10558): init complete, size <= 113, not muted (m_iTimeLeft_ShutUp), packet name matches sender, observers may not talk unless admin. Every chat line of an admin (level > 0) is logged to the Log server as `GM Chat(...)` (10577-10584). Slash commands are matched by `memcmp` prefix in source order; the first match returns, so order is behaviour.

Shipped build: DEF_CHINESE on, DEF_TAIWAN off, DEF_TESTSERVER off (S GlobalDef.h:13-16). With DEF_TESTSERVER off, commands marked `+enable` also need `m_bIsAdminCommandEnabled`, set by `/enableadmincommand 0909114` (level >= 1). Admin levels are 0-4 (`m_iAdminUserLevel`, from the account/character record).

Korean aliases: the source is a UTF-8 export of cp949 code, so each Korean literal is now longer than the memcmp length beside it and cannot match what a cp949 client sends; in the original cp949 build they matched ([COMMANDS-015](../../issues/commands.md#commands-015-korean-slash-aliases-cannot-match-in-the-utf-8-export)). Flagged per row as `alias (cp949)`.

## Prefix modes (first character of the line)

| Prefix | Mode byte | Line | Who receives | Cost / gate | System | Notes |
|---|---|---|---|---|---|---|
| (none) | 0 | S Game.cpp:11094 | players within 10x7 tiles on the same map; in crusade, not enemy side | - | chat | if a whisper target is set, becomes mode 20 unless the text starts with '#' (11071-11080) |
| `$` | 4 | S Game.cpp:10589 | same party (m_iPartyID) | SP 3 (deducted only when m_iTimeLeft_FirmStaminar == 0) | party |  |
| `@` | 1 | S Game.cpp:10607 | same guild name on this server; in crusade, not enemy side (S 11113-11128) | level > 1, SP 3 (deducted only when m_iTimeLeft_FirmStaminar == 0) | guild |  |
| `!` | 2 (admin: 10) | S Game.cpp:10627 | everyone on this server (crusade: not enemy side) | level > 10, SP 5 (deducted only when m_iTimeLeft_FirmStaminar == 0), HP > 0 | chat | admin shout uses mode 10 (GM style) |
| `^` | 1 (admin: 10) | S Game.cpp:10654 | guild on this server and, via GSM_CHATMSG, on all servers; admins also send a global notice (GSM type 10) | guild member, level > 10, SP 3 (checks SP >= 5; deducted only when m_iTimeLeft_FirmStaminar == 0), text < 90 | guild | STRADDLES chat/guild/gm/gate; SP check and charge disagree (5 vs 3); mode-1 delivery to the guild skips enemy-side members in crusade (S 11113-11128) |
| `~` | 3 | S Game.cpp:10724 | same side (town) on this server | level > 1, SP 3 (deducted only when m_iTimeLeft_FirmStaminar == 0), HP > 0 | chat | nationality channel: every player with the same m_cSide (neutral included), no crusade condition in the code (S 10724-10741, 11146-11149) |
| `#` | - | S Game.cpp:11076 | escape: send to local area while in whisper mode | - | chat |  |
| whisper | 20 | S Game.cpp:11171 | sender + target; remote target via GSM_WHISFERMSG | not muted | chat | whisper to an admin is logged as 'PC Chat ... to GM' (11203-11210) |

## Server slash commands

| Command | Aliases | Match line | Handler (def line) | Params | Effect / intent | System | Secondary | Gate (shipped build) | Gate variants by switch | Cross-server | Status |
|---|---|---|---|---|---|---|---|---|---|---|---|
| /who | - | S Game.cpp:10751 | inline SendNotifyMsg TOTALUSERS (S Game.cpp:10758) | - | Show total users on all servers. | gm | chat | admin >= 3 (DEF_CHINESE) | non-Chinese build: admin >= 1 (10756) | uses gate total (MSGID_TOTALGAMESERVERCLIENTS) | live; prefix has no trailing space, so '/whoever' also matches |
| /fi <name> | alias (cp949) '/접속확인' | S Game.cpp:10762 | CheckAndNotifyPlayerConnection (S Game.cpp:34638) | name | Is a player online; admins also get map/location (34674). | chat | gm | none | - | GSM_REQUEST_FINDCHARACTER if not local | live |
| /to [name] | alias (cp949) '/귓속말' | S Game.cpp:10768 | ToggleWhisperPlayer (S Game.cpp:34726) | name or empty | Set/clear the whisper target; later plain lines go only to that player (mode 20). | chat |  | none | - | GSM_REQUEST_FINDCHARACTER, then GSM_WHISFERMSG per line | live; matches any '/to...' prefix |
| /setpf <text> | alias (cp949) '/프로필설정' | S Game.cpp:10774 | SetPlayerProfile (S Game.cpp:34811) | profile text | Set own profile text. | character | chat | none | - | - | live |
| /pf <name> | alias (cp949) '/프로필보기' | S Game.cpp:10780 | GetPlayerProfile (S Game.cpp:34834) | name | Read a player's profile (NOTIFY_PLAYERPROFILE). | character | chat | none | - | - | live (local server only) |
| /shutup <name> <time> | - | S Game.cpp:10786 | ShutUpPlayer (S Game.cpp:36077) | name, time | Mute a player. | gm | chat | admin >= 2 +enable | DEF_TAIWAN: admin >= 1 | GSM_REQUEST_SHUTUPPLAYER | live |
| /rep+ <name> | alias (cp949) '/좋은평가' | S Game.cpp:10792 | SetPlayerReputation(1) (S Game.cpp:36192) | name | Give +1 reputation (cooldown, NOTIFY_CANNOTRATING). | character |  | none (rating cooldown) | - | - | live |
| /rep- <name> | alias (cp949) '/나쁜평가' | S Game.cpp:10798 | SetPlayerReputation(0) (S Game.cpp:36192) | name | Give -1 reputation. | character |  | none (rating cooldown) | - | - | live |
| /hold | alias (cp949) '/멈춤' | S Game.cpp:10804 | SetSummonMobAction(1) (S Game.cpp:39893) | - | Own summoned monsters hold position. | magic | entity | player must have a side (39902) | - | - | live |
| /tgt <name> | alias (cp949) '/공격' | S Game.cpp:10810 | SetSummonMobAction(2) | target name | Own summons attack a target. | magic | entity;combat | player must have a side | a commented '#if defined(DEF_CHINESE)' (10809) once removed it for China | - | live |
| /free | alias (cp949) '/자유' | S Game.cpp:10816 | SetSummonMobAction(0) | - | Own summons follow freely. | magic | entity | player must have a side | - | - | live |
| /closeconn <name> | - | S Game.cpp:10821 | AdminOrder_CloseConn (S Game.cpp:36650) | name | Kick a player (not one with a higher admin level). | gm | session | admin >= 2 +enable | typo 'DEF_TIAWAN' (never defined) means the Taiwan level 3 branch can never compile | - | live ([COMMANDS-017](../../issues/commands.md#commands-017-closeconn-taiwan-branch-uses-the-undefined-name-def_tiawan)) |
| /ban <name> | alias (cp949) '/강퇴' | S Game.cpp:10828 | UserCommand_BanGuildsman (S Game.cpp:36724) | name | Guild master expels a member (Log update, guild notify, name cleared). | guild | chat | guild rank 0 (master), target online on this server | - | - | live; success notify is silently dropped (no SendNotifyMsg case) |
| /reservefightzone <n> | - | S Game.cpp:10834 | AdminOrder_ReserveFightzone (S Game.cpp:36587) | fightzone number | GM reserves a fightzone. | arena | gm | admin >= 2 | - | - | live |
| /dissmiss <name> | - | S Game.cpp:10839 | UserCommand_DissmissGuild (S Game.cpp:36804) | name | Intended: leave/dismiss guild by command. | guild |  | - | - | - | DEAD: handler body is empty ([COMMANDS-014](../../issues/commands.md#commands-014-dissmiss-and-polymorph-are-stubs)) |
| /attack <name> | - | S Game.cpp:10844 | AdminOrder_CallGuard (S Game.cpp:36449) | name | Spawn a guard that attacks the player. | gm | entity;combat | admin >= 2 +enable | - | - | live |
| /createfish <x> <y> <item> | - | S Game.cpp:10850 | AdminOrder_CreateFish (S Game.cpp:36990) | x, y, item | Spawn a fish object. | gm | gathering;entity | admin >= 2 | - | - | live |
| /teleport <map> <x> <y> | - | S Game.cpp:10855 | AdminOrder_Teleport (S Game.cpp:37057) | map, x, y | GM teleport. | gm | map | admin >= 1, not in observer mode | - | - | live |
| /tp <map> <x> <y> | - | S Game.cpp:10860 | AdminOrder_Teleport | map, x, y | Short form of /teleport. | gm | map | admin >= 1 | - | - | live |
| /summondemon  | - | S Game.cpp:10865 | AdminOrder_SummonDemon (S Game.cpp:36535) | - (needs trailing space) | Spawn a demon next to the GM. | gm | entity | admin >= 3 +enable | - | - | live |
| /summonplayer <name> | - | S Game.cpp:10871 | AdminOrder_SummonPlayer (S Game.cpp:44306) | name | Pull a player to the GM. | gm | map | admin >= 2 +enable | - | GSM_REQUEST_SUMMONPLAYER | live |
| /unsummonall  | - | S Game.cpp:10876 | AdminOrder_UnsummonAll (S Game.cpp:44069) | - | Remove all summoned NPCs on the map. | gm | entity | admin >= 3 +enable | - | - | live |
| /unsummonboss  | - | S Game.cpp:10882 | AdminOrder_UnsummonBoss (S Game.cpp:44099) | - | Remove boss NPCs. | gm | entity | admin >= 3 +enable | - | - | live |
| /checkip <ip> | - | S Game.cpp:10887 | AdminOrder_CheckIP (S Game.cpp:38008) | ip | List characters on an IP (NOTIFY_IPACCOUNTINFO). | gm | session | admin >= 2 | - | - | live |
| /polymorph <name> | - | S Game.cpp:10892 | AdminOrder_Polymorph (S Game.cpp:38119) | name | Intended: transform a player. | gm | entity | admin >= 3 | - | - | DEAD: stub, token parsed and ignored (38144) ([COMMANDS-014](../../issues/commands.md#commands-014-dissmiss-and-polymorph-are-stubs)) |
| /setinvi <0/1> | - | S Game.cpp:10897 | AdminOrder_SetInvi (S Game.cpp:38150) | flag | GM invisibility. | gm | magic | admin >= 1 | - | - | live |
| /gns <npc> | - | S Game.cpp:10902 | AdminOrder_GetNpcStatus (S Game.cpp:39269) | npc name | NPC status (NOTIFY_ADMINIFO). | gm | entity | admin >= 2 | - | - | live; memcmp length 4 means '/gns' without space also matches |
| /setattackmode <0/1> | - | S Game.cpp:10907 | AdminOrder_SetAttackMode (S Game.cpp:43946) | flag | Toggle map-wide attack mode (NOTIFY_GLOBALATTACKMODE). | gm | combat;pk-law | admin >= 3 | DEF_TAIWAN: admin >= 2 | - | live |
| /setforcerecalltime <min> | - | S Game.cpp:10912 | AdminOrder_SetForceRecallTime (S Game.cpp:44006) | minutes | Set the enemy-town force-recall timer. | gm | pk-law;map | +enable, admin >= 3 | - | GSM_REQUEST_SETFORCERECALLTIME | live |
| /summon <npc> <n> | - | S Game.cpp:10917 | AdminOrder_Summon (S Game.cpp:44209) | npc name, count | Spawn NPCs. | gm | entity | admin >= 3 +enable | - | - | live |
| /disconnectall  | - | S Game.cpp:10922 | AdminOrder_DisconnectAll (S Game.cpp:44618) | - | Kick everyone. | gm | session | +enable, admin >= 4 | - | - | live |
| /createitem <name> ... | - | S Game.cpp:10927 | AdminOrder_CreateItem (S Game.cpp:46354) | item name, attrs | Create an item. | gm | item | admin >= 4 and m_bIsAdminCreateItemEnabled | DEF_TAIWAN: admin >= 3; old IP whitelist commented (46382) | - | live |
| /enableadmincreateitem 147258  | - | S Game.cpp:10932 | AdminOrder_EnableAdminCreateItem (S Game.cpp:46323) | password literal in the match string | Unlock /createitem for this session. | gm | item | admin >= 3 | DEF_TAIWAN: admin >= 4 | - | live; no return after the call, parsing falls through the remaining matches |
| /enableadmincommand 0909114  | - | S Game.cpp:10937 | AdminOrder_EnableAdminCommand (S Game.cpp:46341) | password literal | Unlock '+enable' GM commands for this session (v2.18). Client _DEBUG build pre-types it on Ctrl+Q (C Game.cpp:28669-28676). | gm |  | admin >= 1 | DEF_TESTSERVER builds skip the need | - | live |
| /energysphere  | - | S Game.cpp:10942 | EnergySphereProcessor(TRUE) | - | Spawn the energy-sphere event. | crusade | gm;entity | admin >= 2 +enable | - | - | live |
| /begincrusadetotalwar  | - | S Game.cpp:10956 | GlobalStartCrusadeMode | - | Start crusade on all servers. | crusade | gm | admin >= 3 +enable | - | GSM_BEGINCRUSADE | live |
| /endcrusadetotalwar  | - | S Game.cpp:10972 | ManualEndCrusadeMode(0) | - | End crusade on all servers. | crusade | gm | admin >= 3 +enable | - | GSM_ENDCRUSADE | live |
| /createparty | - | S Game.cpp:10986 | RequestCreatePartyHandler (S Game.cpp:50929) | - | Create a party (gate owns parties). | party | chat | none | - | MSGID_PARTYOPERATION to gate | live; client party dialog uses COMMONTYPE_REQUEST_JOINPARTY instead, so this is typed by hand |
| /joinparty <name> | - | S Game.cpp:10991 | RequestJoinPartyHandler (S Game.cpp:51338) | name | Join a player's party. | party | chat | none | - | MSGID_PARTYOPERATION | live |
| /dismissparty | - | S Game.cpp:10996 | RequestDismissPartyHandler (S Game.cpp:51399) | - | Leave the party. | party | chat | none | - | MSGID_PARTYOPERATION | live |
| /getpartyinfo | - | S Game.cpp:11001 | GetPartyInfoHandler (S Game.cpp:51430) | - | List party members. | party | chat | none | - | MSGID_PARTYOPERATION | live |
| /deleteparty | - | S Game.cpp:11006 | RequestDeletePartyHandler (S Game.cpp:51468) | - | Disband the party. | party | chat | none | - | MSGID_PARTYOPERATION | live |
| /shutdownthisserverrightnow  | - | S Game.cpp:11011 | inline (sets m_bOnExitProcess, saves flags) | - | Emergency shutdown of this game server. | gm | session | admin == 4 exactly (no +enable check) | - | MSGID_GAMESERVERSHUTDOWNED to Log | live |
| /setobservermode  | - | S Game.cpp:11033 | AdminOrder_SetObserverMode (S Game.cpp:46224) | - | Toggle observer (invisible spectator, panning). | gm | movement;map | admin >= 3 | DEF_TAIWAN: admin >= 1 (11031) | - | live |
| /getticket  | - | S Game.cpp:11040 | AdminOrder_GetFightzoneTicket (S Game.cpp:50801) | - | GM takes fightzone tickets (only inside a fightzone map). | arena | gm | admin >= 2 | - | - | live |

## Client-local slash commands (never sent)

Intercepted in `bCheckLocalChatCommand` C Game.cpp:34000-34121, called from `bSendCommand` at C Game.cpp:1967 before a chat packet is built; return TRUE swallows the line.

| Command | Line | Effect / intent | System | Notes |
|---|---|---|---|---|
| /showframe | C Game.cpp:34011 | Toggle FPS display. | ui-only | 2002-11-01 |
| /enabletogglescreen | C Game.cpp:34019 | Allow window/fullscreen toggle. | ui-only | 2002-12-27 |
| /whon | C Game.cpp:34026 | Show incoming whispers. | chat | client filter only |
| /whoff | C Game.cpp:34033 | Hide incoming whispers. | chat | client filter only |
| /shon | C Game.cpp:34040 | Show shouts. | chat | client filter only |
| /shoff | C Game.cpp:34047 | Hide shouts. | chat | client filter only |
| /tooff <name> | C Game.cpp:34056 | Ignore whispers from one name (single slot m_pExID). | chat | v2.11; name is a client-side block, not a server command |
| /toon <name> | C Game.cpp:34086 | Remove that ignore. | chat |  |

Rows: 8 prefix modes, 45 server slash commands (10 more as cp949 aliases inside rows), 8 client-local commands.

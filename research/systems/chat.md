# System: chat

*Verdict (size review): one cohesive system read as a whole (modes, relay and filters share one packet and one dispatcher); kept as a single file with a Contents block.*

Helbreath 2.20. Paths: `S` = `sources/hgserver-librehelbreath/src/`, `C` = `sources/client-2.20/`, `G` = `sources/gateserver-librehelbreath/`.
Tags: [V] read in source at the cited line, [I] inferred, [U] unverified. Slash-command rows are not repeated here: see
`research/systems/commands/chat-commands.md`; function bodies: `research/game-server/functions/chat-gm.md`, `research/client/functions/chat.md`.

## Contents
1. Purpose - 2. Subsystems - 3. Functions - 4. Commands and messages - 5. Data - 6. Flow - 7. Rules and invariants -
8. Couplings - 9. Current behaviour vs intent - 10. Open questions - 11. Doc discrepancies

## 1. Purpose

Chat is the social backbone of a two-nation PvP world. The player types one line; the first character picks the
audience: people standing nearby (spoken bubble), the whole server (shout), the own nation (town shout), the guild, the
party, or one named person (whisper). Louder channels cost stamina (SP) and need a minimum level; the evident intent is that shouting is a
resource decision and new characters cannot spam [I; gates at V S Game.cpp:10589-10741]. During a crusade, enemies cannot hear you
(war secrecy) [V S Game.cpp:11104-11109]. Guild chat (`^`) and whispers reach players on other game servers through the
gate, so a guild spread over several maps still talks as one [V S Game.cpp:10654-10700, 11171-11197]. Operators get
logging of every GM line and of every whisper to a GM, a mute (`/shutup`), rotating notices and a bad-word monitor hook
[V S Game.cpp:10577-10584, 11203-11210, 36077, 36323, 40688-40720]. The client adds comfort filters (hide whispers or
shouts, ignore one name) and a language filter (Curse.txt) that rewrites outgoing text [V C Game.cpp:34026-34121, 35063].

## 2. Subsystems

| # | Subsystem | Intent | Nests in |
|---|---|---|---|
| 2.1 | Input and client pre-filter | throttle, curse rewrite, local commands, English-build IME guard, then one `MSGID_COMMAND_CHATMSG` | client |
| 2.2 | Server gatekeeping | reject muted, observers, spoofed names, oversize lines; log GM lines; count sector activity | server dispatcher |
| 2.3 | Mode selection by prefix | map first char to send mode 0/1/2/3/4/10/20 with SP and level costs | server dispatcher |
| 2.4 | Local fan-out | deliver to recipients on this server by mode (area, guild, server, side, party) with the crusade side filter | server dispatcher |
| 2.5 | Whisper | `/to` target selection, local or remote lookup, per-line delivery, GM whisper logging | server + gate |
| 2.6 | Cross-server relay | GSM records (CHATMSG, WHISFERMSG, FINDCHARACTER, DISCONNECT) fanned out by the gate | gate bus |
| 2.7 | Moderation | mute (local and remote), bad-word monitor mute, confuse-language garble, chat-to-Log logging | server |
| 2.8 | Notices | `notice.txt` rotation every 80 s; `Noticement.txt` sent at login if the client copy differs | server |
| 2.9 | Client display and filters | chat bubble over the speaker, scroll list, event list, /whon /whoff /shon /shoff /tooff /toon | client |

Slash commands (`/...`) ride the same packet and dispatcher, so chat is also the GM console and the party/guild command
line (see section 8 and `overloaded.md`).

## 3. Functions

| Subsystem | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | `CGame::UpdateScreen_OnGame` (chat input branch) | client | C Game.cpp:35050-35076 | 700 ms throttle, `m_curse.ConvertString`, IME guard for `!`/`~` (DEF_LANGUAGE 4), `bSendCommand(MSGID_COMMAND_CHATMSG)` [V] |
| 2.1 | `CGame::bCheckLocalChatCommand` | client | C Game.cpp:34000-34121 | swallows client-only commands before send [V] |
| 2.1 | `CCurse::LoadCurse / IsCurse / ConvertString / filterChar` | client | C Curse.cpp:36-191 | Curse.txt word list, match and replace [V] |
| 2.1 | Curse.txt load | client | C Game.cpp:1003-1011 | XOR-decode (key 35) to `Curse.tmp`, load, delete; only `#if DEF_LANGUAGE > 2` [V] |
| 2.2-2.4 | `CGame::ChatMsgHandler` | game server | S Game.cpp:10539-11225 | dispatcher: gates, prefix modes, slash commands, confuse, whisper, fan-out [V] |
| 2.5 | `CGame::ToggleWhisperPlayer` | game server | S Game.cpp:34726-34808 | `/to`: set or clear target; remote lookup via GSM_REQUEST_FINDCHARACTER [V] |
| 2.5 | `CGame::CheckAndNotifyPlayerConnection` | game server | S Game.cpp:34638-34722 | `/fi` online check (shares the FINDCHARACTER path) [V doc] |
| 2.6 | `CGame::ServerStockMsgHandler` cases GSM_DISCONNECT / WHISFERMSG / CHATMSG / FINDCHARACTER | game server | S Game.cpp:49353-49412, 49682-49742 | receive side of the relay [V] |
| 2.6 | `CGame::ChatMsgHandlerGSM` | game server | S Game.cpp:11228-11282 | rebuilds a chat packet from a remote record and fans it out [V] |
| 2.6 | `CGame::GSM_RequestFindCharacter` | game server | S Game.cpp:49821-49864 | answers a remote lookup [V doc] |
| 2.6 | `CGateCore::SendMsgToAllGameServers` | gate | G GateCore.cpp:1015-1031 | blind fan-out to every other game server [V doc gate/subsystems.md 4] |
| 2.7 | `CGame::ShutUpPlayer`, `GSM_RequestShutupPlayer` | game server | S Game.cpp:36077-36189, 49767-49819 | GM mute local/remote [V doc] |
| 2.7 | `CGame::_BWM_Init`, `_BWM_Command_Shutup` | game server | S Game.cpp:40688-40720 | external bad-word monitor registers and mutes [V doc] |
| 2.7 | `CGame::bCheckBadWord` | game server | S Game.cpp:36035-36047 | server bad-word check, stub [V doc] |
| 2.8 | `CGame::NoticeHandler` | game server | S Game.cpp:36323-36351 | random notice line every 80 s; does nothing when only one notice is loaded (`m_iTotalNoticeMsg <= 1`, S 36329-36330) [V doc] |
| 2.8 | `CGame::RequestNoticementHandler` | game server | S Game.cpp:45990-46029 | login noticement by size compare [V doc] |
| 2.9 | `CGame::ChatMsgHandler` | client | C Game.cpp:15678-15802 | receive: ignore list, filters, wrap, bubble, event list [V] |
| 2.9 | `CGame::PutChatScrollList`, `AddEventList`, `_RemoveChatMsgListByObjectID` | client | C Game.cpp:15662, 5800, 22772 | display [V doc] |
| 2.9 | `CGame::_bCheckBadWords` + `CGameMonitor` (badword.txt) | client | C Game.cpp:23841-23856, 990-1001 | name filter for new characters, not chat [V] |

## 4. Commands and messages

- Packet: `MSGID_COMMAND_CHATMSG` both directions; layout `+6 objectID(u16, server overwrites), +8 x, +10 y` then name 10,
  `+20` mode byte, `+21` text [V S Game.cpp:11084-11088; C Game.cpp:15695-15712].
- Prefix modes: none=0, `@`=1, `!`=2 (admin 10), `~`=3, `$`=4, `^`=1 (admin 10) plus relay, whisper=20, `#` escapes
  whisper mode (table: `chat-commands.md`).
- Slash: `/to`, `/fi` (chat); `/shutup` (gm) ride the same dispatcher. Client-local: `/whon /whoff /shon /shoff /tooff /toon`.
- Notifies: `DEF_NOTIFY_WHISPERMODEON/OFF`, `PLAYERONGAME`, `PLAYERNOTONGAME`, `PLAYERSHUTUP`, `NOTICEMSG`, `SP` (cost).
- Gate GSM (inside `MSGID_SERVERSTOCKMSG`): `GSM_CHATMSG` 0x0C, `GSM_WHISFERMSG` 0x0D, `GSM_REQUEST/RESPONSE_FINDCHARACTER`
  0x01/0x02, `GSM_DISCONNECT` 0x0E, `GSM_REQUEST/RESPONSE_SHUTUPPLAYER` 0x10/0x11 (`gate-relay.md`).
- Log: `MSGID_GAMEMASTERLOG` (GM chat lines, whispers to GMs, mutes) to the world log `GMLogData\GMEvents*.log`
  [V doc servers/official-worldserver.md 3.4].
- Login text: `MSGID_REQUEST_NOTICEMENT` / `RESPONSE_NOTICEMENT`; content pushed by Log as `NOTICEMENTFILECONTENTS`.

## 5. Data

| Item | Where | Use |
|---|---|---|
| `m_iTimeLeft_ShutUp` | S Client.h:160 | mute ticks (1 tick = one `CheckClientResponseTime` pass, about 3 s) [V doc] |
| `m_iWhisperPlayerIndex`, `m_cWhisperPlayerName[12]`, `m_bIsCheckingWhisperPlayer` | S Client.h:58, 137-138 | whisper target; index 10000 = remote [V S Game.cpp:49728] |
| `m_iSP`, `m_iTimeLeft_FirmStaminar` | S Client.h | SP cost, waived while the stamina-lock potion runs [V S Game.cpp:10594] |
| `m_cMagicEffectStatus[DEF_MAGICTYPE_CONFUSE]` | S Client.h | confuse-language garble [V S Game.cpp:11052] |
| `m_stTempSectorInfo[x][y].i*Activity` | S Map.h | chat counts as sector activity (map heat data) [V S Game.cpp:10560-10571] |
| client `m_bWhisper`, `m_bShout`, `m_pExID` | C Game.h | display filters, one ignore slot [V C Game.cpp:15715-15733] |
| `contents\Curse.txt` | client, XOR 35 | lines `curse<TAB>replacement`; lines starting `$`, `;` or newline are comments; empty replacement = drop the whole line [V C Curse.cpp:38-51, 168-172] |
| `contents\badword.txt` | client, XOR 35 | character-name filter only [V C Game.cpp:995-1001, 25865, 26403] |
| `notice.txt` | server | rotating notices [V doc chat-gm.md] |
| `Noticement.txt` | world log, pushed | login notice [V doc official-worldserver.md] |

Nothing chat-related is persisted in the character file except the mute: `m_iTimeLeft_ShutUp` is written in the player record
and restored at login [V S 6731 compose, 5866 decode, 8084, 9149].

## 6. Flow

**F1 Local spoken line.** Player types text -> client throttle 700 ms, curse rewrite, send [V C Game.cpp:35060-35074] ->
`ChatMsgHandler` gates: init, size <= 113, not muted, packet name equals own name, observer only if admin
[V S Game.cpp:10547-10552] -> GM line logged to Log [V 10577-10584] -> no prefix, so mode 0; if a whisper target is set,
mode 20 unless the line starts `#` [V 11071-11080] -> confuse garble 2/3 of the time [V 11052-11067] -> server writes its
object id and the mode into the packet [V 11084-11088] -> loop over all clients on the same map within x+-9, y+-6; in
crusade, skip enemy-side listeners [V 11097-11111] -> client `ChatMsgHandler` drops ignored names, hidden whispers or
shouts, wraps at 305 px, attaches a bubble to the speaker tile, and for non-zero modes adds to the event list
[V C Game.cpp:15715-15799].

**F2 Guild-wide cross-server line (`^`).** Prefix replaced by space -> if admin and text < 90: GSM_CHATMSG type 10 (notice)
to the gate -> if a guild member and text < 90: GSM_CHATMSG type 1 with the guild GUID [V S Game.cpp:10654-10700] -> then
the local SP/level gate decides local mode 1 or, on failure, mode 0 (public speech in the area, prefix blanked) [V 10701-10717, 11090-11115] -> gate forwards the stock buffer to every other
server [V doc gate/subsystems.md 4] -> receiver `ServerStockMsgHandler` case GSM_CHATMSG -> `ChatMsgHandlerGSM`: type 1 to
clients with the same GUID; type 10 (and 2) to every client [V S Game.cpp:49398-49412, 11262-11280].

**F3 Whisper.** `/to Name` -> `ToggleWhisperPlayer`: search this server by name; self is refused; found -> index set,
`WHISPERMODEON` [V S Game.cpp:34760-34802]; not found -> GSM_REQUEST_FINDCHARACTER with (serverID, clientH, target, self),
`m_bIsCheckingWhisperPlayer` = TRUE [V 34775-34800] -> the server that hosts the target answers GSM_RESPONSE_FINDCHARACTER
[V doc 49832] -> requester sets index 10000 and gets `WHISPERMODEON` [V 49723-49731]. Each later plain line: mode 20,
echoed to the sender, then either GSM_WHISFERMSG (name + whole packet) to the gate or a direct send if the local slot still
holds the same name [V 11171-11211]; the receiving server matches by name and logs it if the target is a GM
[V 49369-49394]. When a player logs out, DeleteClient sends GSM_DISCONNECT and other servers clear whisper targets that
point to that name [V doc gate-relay.md; S Game.cpp:49353-49366].

**F4 Mute.** `/shutup name minutes` (admin >= 2, enabled) -> local `m_iTimeLeft_ShutUp = minutes*20` or remote via
GSM_REQUEST_SHUTUPPLAYER; muted players are dropped at the dispatcher's first gate [V doc chat-gm.md; S Game.cpp:10550].

## 7. Rules and invariants

| Rule | Value | Where |
|---|---|---|
| max packet | 113 bytes (text about 90) | S Game.cpp:10549 [V] |
| spoken range | |dx| < 10, |dy| < 7, same map | S Game.cpp:11097-11101 [V] |
| gate failure | on `@ $ ~ ! ^` a line that fails its SP / level / HP gate is sent as mode 0 (normal speech to everyone in range, prefix blanked), not dropped | S 10593-10606, 10611-10626, 10632-10650, 10701-10717, 10727-10741; 11090-11115 [V] |
| `$` party | SP >= 3, cost 3 | S 10589-10600 [V] |
| `@` guild (this server) | level > 1, SP >= 3, cost 3 | S 10602-10615 [V] |
| `!` shout (this server only) | level > 10, SP >= 5, cost 5, HP > 0; admin -> mode 10 | S 10617-10632 [V] |
| `~` nation (this server) | level > 1, SP >= 3, cost 3, HP > 0; every same `m_cSide` incl. neutral | S 10724-10741, 11146-11149 [V] |
| `^` guild (all servers) | relay: guild member, text < 90 only; local: level > 10, SP >= 5, cost 3 | S 10654-10713 [V] |
| SP waived | while `m_iTimeLeft_FirmStaminar` > 0 | S 10594 etc. [V] |
| crusade filter | modes 0, 1, 2/10 skip listeners whose side differs (both non-neutral); `~`, `$`, whisper unaffected | S 11104-11143 [V] |
| client throttle | one line per 700 ms (excess silently dropped) | C Game.cpp:35059 [V] |
| English build | `!`/`~` lines with IME (non-ASCII) text not sent; incoming modes 0/2/3 with IME text dropped | C 35068-35072, 15720-15724 [V] |
| `/shoff` | hides modes 2 and 3 only; GM mode 10 still shows | C 15730-15733 [V] |
| mute unit | `/shutup` minutes x 20 ticks; BWM fixed `20*3*10` = 600 ticks, about 30 min at the file's 3 s tick, while its comment says 10 min | doc chat-gm.md [V doc]; S 40710, 36131 [V] |

## 8. Couplings

Out of chat (chat reaches into):
- **party** - mode 4 delivers by `m_iPartyID` [V S 11152-11155]; `/createparty` etc. dispatch from ChatMsgHandler (party doc).
- **guild** - mode 1 delivers by `m_cGuildName` (local) or `m_iGuildGUID` (remote) [V S 11118, 11267]; `/ban` dispatch.
- **crusade** - `m_bIsCrusadeMode` + `m_cSide` gate delivery [V S 11104]; `/begincrusadetotalwar`, `/energysphere` dispatch.
- **gm / infrastructure** - admin level, `m_bIsAdminCommandEnabled`, MSGID_GAMEMASTERLOG to the world log, observer mode.
- **character** - SP (`DEF_NOTIFY_SP`), level, HP gates; `/setpf`, `/pf`, `/rep+` dispatch.
- **magic** - confuse-language status [V S 11052].
- **map/world** - sector activity counters (`m_stTempSectorInfo`) updated per chat line [V S 10560-10571].
- **gate (infrastructure)** - GSM bus; the gate never parses the records.
- **client presentation** - bubble placement `bSetChatMsgOwner`, event list, dialog 10 (chat window) [V C 15790-15798].

Into chat (others reach in): session teardown (`DeleteClient` sends GSM_DISCONNECT) [V doc gate-relay.md]; GM tools
(`ShutUpPlayer`, BWM monitor) write `m_iTimeLeft_ShutUp`; character creation reuses badword.txt.

## 9. Current behaviour vs intent

Defects are in [issues/chat.md](../issues/chat.md); the neutral lines are:

1. Curse.txt rewrites outgoing text in the sender's client (C Game.cpp:35063); the server's `bCheckBadWord` is a stub ([CHAT-001](../issues/chat.md#chat-001-language-filter-is-client-side-only)).
2. A curse match rebuilds the line through `filterChar(str, "\t _-.:^")` (C Curse.cpp:147-158), stripping all spaces and punctuation, including a `^` prefix ([CHAT-002](../issues/chat.md#chat-002-a-curse-match-squashes-the-whole-line)).
3. `LoadCurse` passes file text as a printf format (C Curse.cpp:47) ([CHAT-003](../issues/chat.md#chat-003-loadcurse-passes-file-text-as-a-printf-format)).
4. `^` sends the GSM records before the local level/SP gate; a failed gate then downgrades the line to mode 0, and the SP check is 5 while the charge is 3 ([CHAT-004](../issues/chat.md#chat-004-guild-relay--is-sent-before-the-level-and-sp-gate)). The same downgrade applies to `@`, `$`, `~` and `!` (section 7, gate failure row).
5. `!` never relays; `ChatMsgHandlerGSM` handles type 2 but no sender builds a type-2 record ([CHAT-005](../issues/chat.md#chat-005-world-shout-gsm-type-2-has-a-receiver-but-no-sender)).
6. In `ChatMsgHandlerGSM` type 1, `break` on the first uninitialised client ends the loop (S 11266); type 2/10 sends even to uninitialised sockets ([CHAT-006](../issues/chat.md#chat-006-remote-guild-delivery-stops-at-the-first-uninitialised-client)).
7. If no server answers a whisper lookup, `m_bIsCheckingWhisperPlayer` stays TRUE and later lines go out as mode 0 ([CHAT-007](../issues/chat.md#chat-007-whisper-target-lookup-has-no-failure-reply)).
8. GSM_DISCONNECT clears only the first matching whisperer (S 49360-49364) ([CHAT-008](../issues/chat.md#chat-008-gsm_disconnect-clears-only-the-first-whisperer)).
9. `/shutup` is minutes x 20 ticks; BWM is 600 ticks (30 minutes) while its comment says 10 ([CHAT-009](../issues/chat.md#chat-009-mute-units-disagree-between-shutup-and-the-bwm-monitor)).
10. `~`, `$` and whispers ignore the crusade side filter, and remote `^` lines are not filtered on the receiving server ([CHAT-010](../issues/chat.md#chat-010-crusade-chat-secrecy-is-partial)).
11. The confuse garble writes Korean double bytes over English text (S 11059-11061) ([CHAT-011](../issues/chat.md#chat-011-confuse-garble-writes-double-byte-text-over-english-text)).

## 10. Open questions

- Whether the official BWM (bad-word monitor) program existed as a separate tool and how it connected (only the
  `MSGID_BWM_*` handlers are in source) [U].
- Client display rules per mode colour (in `PutChatScrollList` / event list) were not traced; client cluster owns them.

## 11. Doc discrepancies

None against the code. Additions to `chat-commands.md`: the `^` relay is sent before the level/SP gate (section 9.4, [CHAT-004](../issues/chat.md#chat-004-guild-relay--is-sent-before-the-level-and-sp-gate));
GSM type 2 has a receiver but no sender (9.5, [CHAT-005](../issues/chat.md#chat-005-world-shout-gsm-type-2-has-a-receiver-but-no-sender)).

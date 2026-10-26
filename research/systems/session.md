# System: session (login, character select/create, enter game, server change, logout, timeouts)

*Size verdict (2026-10-25): one cohesive doc. The subsystems are stages of one session lifecycle and
each flow crosses all of them, so it is not split; use the Contents block.*

Cluster: infrastructure. Citation tags `SRV`, `CLI`, `GATE`, `MLS`, `WLS` and [V]/[I]/[U] exactly as in
`network.md` (header). Line numbers as of 2026-10-25. Wire layouts are not repeated: client <-> Log in
`research/servers/client-log-protocol.md`, game <-> Log in `research/servers/game-log-protocol.md`,
the official handlers in `research/servers/official-mainserver.md` / `official-worldserver.md`.

## Contents
1. Purpose
2. Subsystems
3. Functions
4. Commands and messages
5. Data
6. Flow
7. Rules and invariants
8. Couplings
9. Intent vs implementation
10. Open questions
11. Doc discrepancies

## 1. Purpose

Carry one player from "typed account and password" to "standing on a map", keep exactly one live
copy of that character in the world while they play, move them between game servers without a
second login, and get them out again with their state saved. What the player experiences: login
screen, the four-slot character screen, a "connecting" screen, play, a "changing server" pause
on some map exits, an 11-tick (about 11 s) logout countdown, and the "account in use - force login?" prompt.
What the operator experiences: account state (NULL / INIT / WAIT / PLAYING) held in the World Log
server and refreshed by the game servers.

Design intent [I]: **the authority for who may enter is the Log side.** The game server is not
account-stateless (it keeps `m_cAccountName/Password` and resolves duplicate logins by comparing passwords
locally, F4; `Game.cpp:1556-1598`, `RequestCheckAccountPasswordHandler` `46052-46083`), but it admits a
claimed name/account/password only because the Log server must return a matching character file. No session
ticket exists (`research/servers/client-log-protocol.md` 8 step 4; see section 9) [V doc, I intent].

## 2. Subsystems

```
session
 +- 2.1 Account login            client <-> MLS (one TCP connection per request)
 +- 2.2 Character roster          list, create, delete: client <-> MLS, MLS <-> WLS relay
 +- 2.3 Enter-game routing        client -> MLS (first entry) or WLS (after server change) -> game server address
 +- 2.4 Game-server admission     INITPLAYER -> Log REQUEST_PLAYERDATA -> InitPlayerData -> INITDATA
 |    +- 2.4.1 Duplicate-login resolution (same account / same name on this server)
 |    +- 2.4.2 Account-state tracking (ENTERGAMECONFIRM, WAIT, CHECKACCOUNTPASSWORD, FORCEDISCONN)
 +- 2.5 Server change            save-reply -> NOTIFY_SERVERCHANGE -> reconnect to WLS -> new server
 +- 2.6 Logout and disconnect    client countdown, DeleteClient save policy, respawn rewrite
 +- 2.7 Timeouts and liveness    10 s server timeout, client lag cut-off, WLS 20 s INIT/WAIT, gate 20 s party stamp
 +- 2.8 Server-wide session gates accept gate, shutdown drain, overflow redirect
```

Intent per subsystem:
- **2.1** authenticate against the account file and check the world is up; stateless per request so the
  MLS can sit behind three public ports [V `MLS 0x413344-0x41346c` via doc].
- **2.2** keep character files with the world (WLS) and only the name list with the account (MLS)
  [V doc `official-mainserver.md` 3.3-3.4].
- **2.3** the client never knows the world's server layout; the Log side maps "map name" to "game server
  address" for first entry and every server change [V doc topology verdict].
- **2.4** turn a TCP connection into an in-world player only after the Log server returns a valid record.
- **2.4.1** a reconnecting player must be able to evict their own stale session, but nobody else may.
- **2.5** let a world be split across game-server processes by map while keeping the player's state and
  party.
- **2.6** always persist on exit, and put dead, jailed or observer characters somewhere sane for next login.
- **2.7** free slots and account locks held by vanished clients.
- **2.8** refuse players until the server is fully configured, and drain them before exit.

## 3. Functions

| Sub | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | `UpdateScreen_OnLogin` | CLI | `Game.cpp:27703-27999` | login screen, sends `MSGID_REQUEST_LOGIN` [V doc] |
| 2.1 | `ConnectionEstablishHandler` | CLI | `Game.cpp:3030-3081` | on connect, sends the request chosen by `m_dwConnectMode` [V doc] |
| 2.1 | `LogResponseHandler` | CLI | `Game.cpp:14773-15449` | parses every Log reply by MsgType; deletes the socket after it [V doc] |
| 2.1 | LOGIN handler | MLS | `0x4036f0` | account file, world-active check, block date, password; asks WLS for list [V doc] |
| 2.2 | `UpdateScreen_OnSelectCharacter` | CLI | `Game.cpp:21806-22084` (English build body) | slot screen; ENTERGAME at `21614-21640` area [V doc] |
| 2.2 | `UpdateScreen_OnCreateNewCharacter` | CLI | `Game.cpp:25923-26453` | 70-point stat roll, sends CREATENEWCHARACTER [V doc] |
| 2.2 | `HandleRequestCharInfoList` / `HandleCreateNewCharacter` / `HandleDeleteCharacter` | WLS | `0x404370`, `0x405850`, `0x407cc0` | build slot list, write new file, delete file [V doc] |
| 2.3 | ENTERGAME handler | MLS | `0x406840` | PLAYING / FORCEDISCONN / route to game server, then INITSTATUS to WLS [V doc] |
| 2.3 | `HandleRequestEnterGame` | WLS | `0x405440` | ENTERGAME after server change; needs an account entry; ignores the kind [V doc] |
| 2.4 | `bAccept` | SRV | `Game.cpp:399-464` | admission gate (2.8) [V] |
| 2.4 | `RequestInitPlayerHandler` | SRV | `Game.cpp:1501-1609` | store identity, resolve duplicates, ask Log for data [V] |
| 2.4 | `ResponsePlayerDataHandler` | SRV | `Game.cpp:4796-4853` | Log reply: load, or reject ("world full" byte) [V doc] |
| 2.4 | `InitPlayerData` | SRV | `Game.cpp:4855-5043` | decode+validate, INITPLAYER confirm, `m_bIsInitComplete`, ENTERGAMECONFIRM, party check, overflow redirect [V] |
| 2.4 | `RequestInitDataHandler` | SRV | `Game.cpp:1611-2250` | name re-check, character/item contents, map placement, initial notifies [V] |
| 2.4 | `InitPlayerResponseHandler` / `InitDataResponseHandler` | CLI | `Game.cpp:3085-3109`, `45800-46075` | send INITDATA; load the map and enter play [V doc] |
| 2.4.2 | `RequestCheckAccountPasswordHandler` | SRV | `Game.cpp:46052-46083` | WLS-pushed password/level check, drop without save [V] |
| 2.4.2 | `ForceDisconnectAccount` | SRV | `Game.cpp:38098-38117` | WLS/MLS force-login: notify FORCEDISCONN (count <= 10) [V doc] |
| 2.4.2 | `HandleEnterGameConfirm`, `HandleGSWaitStatusMsg`, `SetAccountStatus` | WLS | `0x4033f0`, `0x40adf0`, `0x40a210` | account state machine [V doc] |
| 2.5 | `RequestTeleportHandler` (cross-server branch) | SRV | `Game.cpp:23344-23374` (also 23437, 23470, 23512) | save-with-reply, set `m_bIsOnServerChange` [V] |
| 2.5 | `ResponseSavePlayerDataReplyHandler` | SRV | `Game.cpp:36354-36372` | on save ack, send `DEF_NOTIFY_SERVERCHANGE` [V] |
| 2.5 | `SendNotifyMsg` case SERVERCHANGE | SRV | `Game.cpp:18644-18667` | map 10 + Log address 15 + port i32 [V] |
| 2.5 | `NotifyMsg_ServerChange` | CLI | `Game.cpp:45039-45090` | drop game socket, connect to that address, ENTERGAME kind `NEW_TOWLSBUTMLS` [V] |
| 2.6 | System-menu logout / `OnTimer` countdown | CLI | `Game.cpp:33378-33392`, `35318-35345` | 11 one-second ticks (`m_cLogOutCount = 11`; 1 s in debug), then close socket [V] |
| 2.6 | `NotifyMsg_ForceDisconn` | CLI | `Game.cpp:43517-43532` | forced countdown of at most 5 s, cannot be cancelled [V] |
| 2.6 | `DeleteClient` | SRV | `Game.cpp:2571-2887` | notify, clear map, respawn rewrite, save / no-save / wait-status, party release, free slot [V] |
| 2.6 | `OnClientSocketEvent` | SRV | `Game.cpp:467-512` | socket close -> `DeleteClient(i, TRUE, TRUE)` [V] |
| 2.7 | `CheckClientResponseTime` (timeout) | SRV | `Game.cpp:3850-3866` | > 10 s silence -> delete (save if initialised) [V] |
| 2.7 | `CGame::OnTimer` (lag) | CLI | `Game.cpp:4288-4301` | 7 checks with >= 6 unanswered commands -> connection lost; the 2 s gate runs against the last-receive stamp, so about 7 s under server silence (1 s ticks), ~14 s only with steady traffic [V code, I effect] |
| 2.7 | `CheckAccountStatusTimeout` | WLS | `0x40aa30` | INIT/WAIT older than 20 s -> NULL [V doc] |
| 2.8 | `_iForcePlayerDisconect` | SRV | `Game.cpp:37973-37988` | shutdown drain, 15 per 2 s [V doc] |

## 4. Commands and messages

From `research/systems/commands/by-system.md` (session and account rows):
- Client -> MLS: `MSGID_REQUEST_LOGIN`, `_CREATENEWACCOUNT`, `_CREATENEWCHARACTER`, `_DELETECHARACTER`,
  `_ENTERGAME` (kinds `NEW 0x0F1C`, `NOENTER_FORCEDISCONN 0x0F1D`, `NEW_TOWLSBUTMLS 0x0F1F`),
  `_CHANGEPASSWORD`; replies `MSGID_RESPONSE_LOG` / `_RESPONSE_ENTERGAME` with `DEF_LOGRESMSGTYPE_*` /
  `DEF_ENTERGAMERESTYPE_*` (client reads MsgType only).
- Client -> game: `MSGID_REQUEST_INITPLAYER`, `MSGID_REQUEST_INITDATA`, `MSGID_COMMAND_CHECKCONNECTION`;
  game -> client: `MSGID_RESPONSE_INITPLAYER`, `MSGID_RESPONSE_INITDATA`, `MSGID_PLAYERCHARACTERCONTENTS`,
  `MSGID_PLAYERITEMLISTCONTENTS`, notifies `DEF_NOTIFY_SERVERCHANGE`, `DEF_NOTIFY_FORCEDISCONN`,
  `DEF_NOTIFY_SERVERSHUTDOWN`.
- Game -> WLS: `MSGID_REQUEST_PLAYERDATA`, `MSGID_ENTERGAMECONFIRM`, `MSGID_REQUEST_SAVEPLAYERDATA[_REPLY|LOGOUT]`,
  `MSGID_REQUEST_NOSAVELOGOUT`, `MSGID_REQUEST_SETACCOUNTWAITSTATUS`; WLS -> game:
  `MSGID_RESPONSE_PLAYERDATA`, `MSGID_RESPONSE_SAVEPLAYERDATA_REPLY`, `MSGID_REQUEST_CHECKACCOUNTPASSWORD`,
  `MSGID_REQUEST_FORCEDISCONECTACCOUNT`.
- MLS <-> WLS: `REQUEST/RESPONSE_CHARINFOLIST`, `REQUEST_SETACCOUNTINITSTATUS`, `REQUEST_CLEARACCOUNTSTATUS`,
  `REQUEST_FORCEDISCONECTACCOUNT`, `RESPONSE_LOG` relay (`official-mainserver.md` 5).
- Game -> gate: party op 4 (remove on logout), op 5 (check on entry), op 7 (server-change stamp)
  (`SRV Game.cpp:2786-2796, 5001-5011, 2839-2848`).
- GSM: `GSM_DISCONNECT` stocked from `DeleteClient` (by-system session row) [V doc].

## 5. Data

| Item | Where | Notes |
|---|---|---|
| `m_cCharName`, `m_cAccountName`, `m_cAccountPassword` | `CClient` | set in `RequestInitPlayerHandler` `SRV Game.cpp:1602-1604`; plaintext password kept in memory for the session and sent in every save [V] |
| `m_bIsInitComplete` | `CClient` | FALSE in constructor (`SRV Client.cpp:34`), TRUE in `InitPlayerData` `Game.cpp:4988` [V] |
| `m_bIsOnServerChange`, `m_bIsOnWaitingProcess` | `CClient` | set at `Game.cpp:23371-23372`; switches `DeleteClient` to the WAIT branch [V] |
| `m_bIsObserverMode` | `CClient` | from the INITPLAYER byte (`Game.cpp:1552, 1605`) [V] |
| `m_dwTime` | `CClient` | last-receive stamp (`Game.cpp:485`) for the timeout [V] |
| `m_dwCharID` | `CClient` | DB-mode record id echoed in saves (`persistence.md`) [V doc] |
| `m_cLockedMapName`, `m_iLockedMapTime` | `CClient` | jail target written on forced disconnect (`Game.cpp:2706-2716`) [V] |
| Client roster | `CCharInfo m_pCharList[4]` | 65-byte slot records (`client-log-protocol.md` 5) [V doc] |
| WLS account table | 5000 x 0x30 at this+0xa06c | state, account, password, server, level, time (`official-worldserver.md` 6) [V doc] |
| MLS connection table | 10000 x 0x60 at this+0x9c9c | "in game in world X" (`official-mainserver.md` 6) [V doc] |
| Constants | `DEF_CLIENTTIMEOUT` 10 s (`SRV Game.h:59`), `DEF_MAXONESERVERUSERS` 800 (`Game.h:155`), 4 character slots, WLS INIT/WAIT timeout 20 s | [V] |

## 6. Flow

**F1. First entry** (client -> MLS -> WLS -> MLS -> client -> game -> WLS -> game -> client).
Scope: the English build skips the client's gateway pre-step (`m_bIsCheckingGateway`, `MSGID_GETMINIMUMLOADGATEWAY`,
`CLI Game.cpp:3041-3045`), which exists only in `DEF_USING_GATEWAY` builds (Taiwan; `CLI GlobalDef.h:160-166`) [V].
1. CLI login screen -> new `m_pLSock`, `m_dwConnectMode = MSGID_REQUEST_LOGIN`; on FD_CONNECT
   `ConnectionEstablishHandler` sends it (`CLI Game.cpp:3030-3081`) [V doc].
2. MLS LOGIN `0x4036f0`: account file, world registered **and activated**, block date, password; on
   success sends `REQUEST_CHARINFOLIST` to the WLS; WLS reads each character file and returns a ready-made
   `RESPONSE_LOG` CONFIRM which the MLS forwards (`official-mainserver.md` 3.1, 5) [V doc].
3. CLI `LogResponseHandler` CONFIRM: version words must be 2 / 20, fill `m_pCharList` (`CLI Game.cpp:14932-15098`) [V doc].
4. Player picks a slot: ENTERGAME kind NEW with name, slot map, level (`CLI Game.cpp:21614-21640`) [V doc].
5. MLS ENTERGAME `0x406840`: account in game -> `PLAYING` (client offers force login, which re-sends kind
   `0x0F1D` -> MLS sends `REQUEST_FORCEDISCONECTACCOUNT` to the WLS and replies `FORCEDISCONN`); else CONFIRM
   with game-server address/port and `SETACCOUNTINITSTATUS` to the WLS (state INIT) [V doc].
6. CLI connects to the game server; `ConnectionEstablishHandler` sends `MSGID_REQUEST_INITPLAYER` (name,
   account, password, observer flag) (`CLI Game.cpp:1870-1903`) [V doc].
7. SRV `bAccept` admits only if all start conditions hold (`Game.cpp:403-416`); `RequestInitPlayerHandler`
   resolves duplicates (F4) and sends `MSGID_REQUEST_PLAYERDATA` on a sub-log socket (`Game.cpp:1608`) [V].
8. WLS `HandleRequestPlayerData` `0x403640`: file must exist and its `account-name` must match; returns the
   text plus guild-status byte [V doc].
9. SRV `ResponsePlayerDataHandler` -> `InitPlayerData` (`Game.cpp:4796-4853, 4855`): decode, validate (stat sum,
   name, penalty date, items; `game-log-protocol.md` 6.2), send `RESPONSE_INITPLAYER` CONFIRM
   (`4972`), set `m_bIsInitComplete` (`4988`), send `MSGID_ENTERGAMECONFIRM` (`4991`) -> WLS state PLAYING;
   if in a party, gate op 5 (`5001-5011`); if more than 800 players, teleport to bisle/resurr (`5027-5040`) [V].
10. CLI `InitPlayerResponseHandler` sends `MSGID_REQUEST_INITDATA` (`CLI Game.cpp:3085-3109`) [V doc];
    SRV `RequestInitDataHandler` re-checks the name (`Game.cpp:1626-1641`), sends character and item contents,
    places the player and sends `MSGID_RESPONSE_INITDATA` + initial notifies; CLI `InitDataResponseHandler`
    loads the map (`CLI Game.cpp:45800-46075`) [V].

**F2. Server change** (game A -> WLS -> game A -> client -> WLS -> game B):
1. SRV A `RequestTeleportHandler`: destination map not hosted -> store destination map/x/y in the client,
   `bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA_REPLY, i, FALSE)` (count flag FALSE), set
   `m_bIsOnServerChange` (`Game.cpp:23344-23374`) [V].
2. WLS writes the file and answers `RESPONSE_SAVEPLAYERDATA_REPLY` (`WLS 0x404e80`) [V doc].
3. SRV A `ResponseSavePlayerDataReplyHandler` -> `DEF_NOTIFY_SERVERCHANGE` with the destination map, the game
   server's own `log-server-address` and, in `DEF_DEFENCEHACKING` builds, the pushed `world-server-port`
   (`Game.cpp:36354-36372, 18644-18667`) [V].
4. CLI `NotifyMsg_ServerChange` deletes the game socket, connects to that address and queues ENTERGAME kind
   `NEW_TOWLSBUTMLS` (`CLI Game.cpp:45039-45090`) [V].
5. SRV A sees the socket close -> `DeleteClient(i, TRUE, TRUE)`; because `m_bIsOnServerChange` is set it
   skips the save, sends gate party op 7 and `MSGID_REQUEST_SETACCOUNTWAITSTATUS` (state WAIT)
   (`Game.cpp:2639, 2830-2852`) [V].
6. WLS `HandleRequestEnterGame` needs any account entry (WAIT here), finds the server for the map, replies
   CONFIRM; then F1 steps 6-10 run against game B; B's party op 5 clears A's stamp at the gate
   (`research/gate/subsystems.md` 5) [V doc].

**F3. Logout**: CLI system menu starts an 11-tick countdown (`CLI Game.cpp:33382-33386`); at 0 it deletes
the game socket (`35330-35334`) - no logout message exists [V]. SRV `OnClientSocketEvent` close ->
`DeleteClient(i, TRUE, TRUE)` -> dead/jail/observer/fightzone respawn rewrite (`Game.cpp:2642-2766`) ->
gate party op 4 -> `MSGID_REQUEST_SAVEPLAYERDATALOGOUT` with count flag TRUE, or `LocalSavePlayerData`
if no Log socket (`2800`; not-initialised clients get NOSAVELOGOUT, `2802`) -> WLS writes the file, clears the account (NULL) and tells the MLS
`CLEARACCOUNTSTATUS` [V; WLS V doc].

**F4. Duplicate login on one game server** (`SRV Game.cpp:1556-1598`) [V]: a live client with the same
account (first loop) or the same character name (second loop): password equal -> the **old** session is
deleted with save and count flag FALSE; password different -> the **new** connection is deleted without save.
The first loop passes count FALSE, the second loop uses the default TRUE for this password-mismatch drop of the
new client (`1573`, `1595`). A dropped new client goes through the `DeleteClient` else-branch and sends
`MSGID_REQUEST_NOSAVELOGOUT` carrying `bCountLogout` and the claimed account/character (`2828`); see section 10.

**F5. Force login across servers**: MLS -> WLS `REQUEST_FORCEDISCONECTACCOUNT` -> owning game server
`ForceDisconnectAccount` sends `DEF_NOTIFY_FORCEDISCONN` (`SRV Game.cpp:38098-38117`) -> CLI forced countdown
of at most 5 s (`CLI Game.cpp:43517-43532`) -> socket close -> F3 save path. The game server does not kick by
itself (the `DeleteClient` call is commented out) [V doc].

## 7. Rules and invariants

- Server timeout: a client silent for more than 10 s is deleted, with save only if initialised
  (`SRV Game.cpp:3850-3866`) [V]. The keep-alive is every 8 s (`CLI Game.cpp:4272-4277`) [V].
- Admission requires the gate, Log and seven config flags, `m_bIsGameStarted`, no shutdown in progress and,
  under `DEF_DEFENCEHACKING`, the WLserver config (`SRV Game.cpp:403-416`) [V].
- One live session per account per game server and one per character name (F4) [V]. Across servers the
  WLS/MLS enforce it with PLAYING (`official-mainserver.md` 3.5) [V doc].
- A character must pass load validation or the client is dropped as "data error": stat sum
  `(level-1)*3+70`, each stat 10..200, valid names, penalty date in the past, all items known
  (`research/servers/game-log-protocol.md` 6.2) [V doc].
- Over 800 players on one server (`DEF_MAXONESERVERUSERS`), a newly entered player is teleported to
  `bisle` or the side's resurrection map (`SRV Game.cpp:5027-5040`) [V].
- Respawn rewrite on save (`SRV Game.cpp:2642-2766`) [V]: killed players go to their side's
  resurrection/farm map by level; `/closeconn` victims (`bForceCloseConn`) go to `bisle` locked for 10 min
  (`2706-2716`; only caller `36707`); observers and anyone in a `fight*` map go to their town.
- Server change keeps the player reserved for 20 s at the WLS (WAIT timeout, `WLS 0x40aa30`) and at the gate
  for party membership (20 s stamp, `research/gate/subsystems.md` 5) [V doc].
- Client logout countdown 11 one-second ticks (`m_cLogOutCount = 11`, `CLI Game.cpp:33386`, `35322-35330`), forced countdown <= 5 s
  (`43527`) [V].

## 8. Couplings

Out of session:
- **persistence** (`persistence.md`): every exit path chooses a save message; coupling point
  `bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA*)` inside `DeleteClient`, `RequestTeleportHandler`,
  `CheckClientResponseTime` (autosave) [V].
- **party** (social cluster): gate party ops 4/5/7 from `DeleteClient` and `InitPlayerData`; coupling point
  `SendMsgToGateServer(MSGID_PARTYOPERATION, ...)` and `m_stPartyInfo` [V].
- **map/movement** (gameplay cluster): the cross-server branch of `RequestTeleportHandler` is the only
  server-change trigger; `GetMapInitialPoint`, `bGetEmptyPosition` place the player in `RequestInitDataHandler` [V doc].
- **pk-law / combat**: the respawn rewrite in `DeleteClient` reads `m_bIsKilled`, `m_cSide`, level and the
  locked-map fields [V].
- **crusade / quest / guild**: `RequestInitDataHandler` sends crusade, construction-point, quest and guild
  state on entry; `InitPlayerData` clears a guild that vanished while offline (guild-status byte) [V doc].
- **admin**: GMs (admin level 1..4) become invisible on entry (`SRV Game.cpp:4934-4935`); `/closeconn` and
  `/disconnectall` reuse `DeleteClient` / `_iForcePlayerDisconect` [V].

Into session:
- **server-topology**: `bAccept` start gates; shutdown drain calls `_iForcePlayerDisconect`; Log link loss
  forces shutdown (code 3) [V].
- **network**: socket errors and unknown messages call `DeleteClient` [V].
- **admin anti-hack**: speed checks and the hourly exp check call `DeleteClient(i, TRUE, TRUE)` [V].
- **client presentation** (other cluster): `UpdateScreen_On*` modes and dialogs 12/25 render the session states.

## 9. Current behaviour vs intent

- The ENTERGAME confirm carries no ticket; the game server re-asks the WLS, which checks only the account name and applies its password check to INIT/WAIT accounts ([SESS-001](../issues/session.md#sess-001-no-session-ticket-credential-checks-are-weak)).
- `RequestInitDataHandler` has no init-complete test ([SESS-002](../issues/session.md#sess-002-requestinitdatahandler-has-no-init-complete-guard)).
- The duplicate check precedes the Log reply and the WLS overwrites PLAYING on a second confirm ([SESS-003](../issues/session.md#sess-003-duplicate-login-race)).
- The game server's `SETACCOUNTINITSTATUS` case is never called ([SESS-004](../issues/session.md#sess-004-setaccountinitstatus-from-the-game-server-is-dead)).
- The server-change notify carries the internal Log port unless `DEF_DEFENCEHACKING` is defined ([SESS-005](../issues/session.md#sess-005-server-change-port-depends-on-a-build-switch)).
- The `else if` in `CheckClientResponseTime` repeats the outer test ([SESS-006](../issues/session.md#sess-006-redundant-timeout-branch-in-checkclientresponsetime)).
- `world-server-max-user` is parsed and displayed but not enforced ([SESS-007](../issues/session.md#sess-007-world-user-cap-parsed-never-enforced)).
- `ForceDisconnectAccount` only notifies the client ([SESS-008](../issues/session.md#sess-008-force-disconnect-relies-on-client-co-operation)).

## 10. Open questions

- How the official MLS behaves when a client sends ENTERGAME kind `NEW_TOWLSBUTMLS` to it instead of the WLS
  (only matters for merged-server setups) [U].
- Whether a "changing server" UI state existed on the client beyond `DEF_GAMEMODE_ONCONNECTING` with message
  "55" (`CLI Game.cpp:45084-45089`) [U: string table not checked].
- Whether the WLS ever times out a PLAYING account whose game server is alive but lost the player silently
  (only INIT/WAIT time out) [V doc for INIT/WAIT; U for PLAYING recovery besides game-server loss].

- What the WLS `NOSAVELOGOUT` handler does with the count flag on a count=TRUE message (account state
  cleared or not) (`Game.cpp:1573, 1595, 2828`; `persistence.md` section 6). The WLS handler is not decompiled [U].

## 11. Doc discrepancies

- `research/servers/client-log-protocol.md` 8 step 5 says the game server reads INITPLAYER "up to the
  observer flag" at `SRV Game.cpp:1520-1553`; confirmed (observer byte at 1552) [V]. No conflict.
- `research/game-server/functions/character.md` describes the duplicate rule correctly but says "by
  character name" uses `DeleteClient(iClientH,FALSE,FALSE)`; the second loop passes no count flag
  (default TRUE) while the first passes FALSE (`SRV Game.cpp:1573` vs `1595`): a logout-count difference
  between the two loops [V].

# System: admin (GM commands, admin levels, monitoring, logging, anti-hack)

*Size verdict (2026-10-25): one doc with a Contents block. The five subsystems share the admin-level model
and the GM-log path, and each is too small for its own sidecar; the full per-command table already lives in
`research/game-server/functions/chat-gm.md` and is not repeated.*

Cluster: infrastructure. Citation tags `SRV`, `CLI`, `GATE`, `MLS`, `WLS` and [V]/[I]/[U] as in
`network.md` (header). Line numbers as of 2026-10-25. Admin levels quoted are for the shipped build
(`DEF_CHINESE` on, `DEF_TAIWAN` and `DEF_TESTSERVER` off).

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

Give the operator staff a way to police and steer a live world from inside the game client (GM slash
commands), to watch servers from outside (monitor tools, window status lines, log files), to leave an
audit trail of valuable item movements, PK and GM actions, and to throw out cheating clients
automatically. Players see GMs as invisible, untouchable observers who can mute, summon, teleport or
disconnect them; they see the anti-hack only as sudden disconnects (and, for exp abuse, a one-month
character block). Operators see `GMEvents`, `ItemEvents`, `EKEvents` daily logs on the WLS and several
local log files on each game server and the gate.

Design intent [I]: **trust is per character and per session.** A GM is an ordinary character whose saved
record carries `admin-user-level`; dangerous commands additionally need a per-session "enable" password
typed in chat (added v2.18, 2002-10-15, per the comments) [V `SRV Game.cpp:5731, 10937`].

## 2. Subsystems

```
admin
 +- 2.1 Admin levels             0 player; 1..4 GM tiers; >= 10 disguised appearance; GM immunities
 +- 2.2 GM command dispatch      '/' commands in ChatMsgHandler, level gates, enable passwords
 |    +- 2.2.1 Session enables   /enableadmincommand <pw>, /enableadmincreateitem <pw>
 |    +- 2.2.2 Cross-server GM   summon / shut-up / force-recall time via gate GSM records
 +- 2.3 Monitoring               window status line, gate monitors, manager program, bad-word monitor
 +- 2.4 Logging                  GM chat and orders, item log (good items), PK log, crusade log, local files
 +- 2.5 Anti-hack                speed checks (batch + gap), clock skew, exp-per-hour block, flood, data validation, DupItemID
 +- 2.6 Network admission        gate IP allow-list, WLS / MLS allow-lists (operator-side security)
```

Intent per subsystem [I]:
- **2.1** let staff play inside the world without being part of its economy or combat.
- **2.2** cheap in-band administration with no separate admin protocol; the enable step limits damage from a
  stolen GM character [I from the v2.18 comment].
- **2.3** let a second process (bad-word filter, manager console, gate monitors) observe or act without a GM
  logged in.
- **2.4** reconstruct disputed trades, item duplications and GM abuse after the fact.
- **2.5** cheap server-side heuristics against the common 2002 cheats (speed hacks, macro levelling, packet
  replay).
- **2.6** keep strangers off the internal ports.

`DEF_DEFENCEHACKING` is **not** part of anti-hack despite its name: every site concerns the World Log
server config and port (`server-topology.md` 9) [V].

## 3. Functions

Per-command detail: `research/game-server/functions/chat-gm.md` (35 functions with levels).

| Sub | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.1 | `_bDecodePlayerDatafileContents` (admin level) | SRV | `Game.cpp:5731`, clamp `6341`, disguise `6369-6370` | level from the record; negative -> 0; >= 10 sets the appearance type [V] |
| 2.1 | `InitPlayerData` | SRV | `Game.cpp:4934-4935` | levels 1..4 enter invisible [V] |
| 2.2 | `ChatMsgHandler` | SRV | `Game.cpp:10539-11225`; `/` block `10745-11043` | dispatch of all slash commands [V] |
| 2.2.1 | `AdminOrder_EnableAdminCommand` / `_EnableAdminCreateItem` | SRV | `Game.cpp:46341-46352`, `46323-46338` | set per-session flags; password is the literal in the dispatcher (`10937`, `10932`) [V] |
| 2.2 | `AdminOrder_*` (24 commands) | SRV | `Game.cpp:36449-50849` | see chat-gm.md [V doc] |
| 2.2.2 | `ShutUpPlayer`, `AdminOrder_SummonPlayer`, `AdminOrder_SetForceRecallTime` | SRV | `Game.cpp:36077-36189`, `44306-44414`, `44006-44067` | local action or GSM record to other servers [V doc] |
| 2.2.2 | `ServerStockMsgHandler` (GSM shut-up / summon / force-recall), `GSM_RequestShutupPlayer` | SRV | `Game.cpp:49339-49764`, `49767-49819` | execute remote GM orders [V doc] |
| 2.3 | `DisplayInfo` | SRV | `Game.cpp:888-973` | status line: max level, world cap, players, sub-log sockets, crusade, gate retries, reboots [V] |
| 2.3 | `_Manager_Init`, `CheckConnectionHandler` (manager branch) | SRV | `Game.cpp:40724-40794`, `48978-49038` | manager login by password, 16-byte status reply [V] |
| 2.3 | `_BWM_Init`, `_BWM_Command_Shutup` | SRV | `Game.cpp:40688-40695`, `40697-40720` | bad-word monitor registration and mute order [V] |
| 2.3 | `SendMsgToMonitor`, `CheckGameServerActivity`, `OnKeyUp` F2/F3 | GATE | `GateCore.cpp:710-785`, `688-708`, `579-668` | monitor feed and operator lists [V doc] |
| 2.4 | `bSendMsgToLS` cases GAMEMASTERLOG / GAMEITEMLOG / GAMECRUSADELOG | SRV | `Game.cpp:4688-4753` | text logs to the WLS [V doc] |
| 2.4 | `_bItemLog` (two active overloads), `_bCheckGoodItem` | SRV | `Game.cpp:42704-42816`, `42819-42870`, `42874-43010` | item log for valuable items only [V doc] |
| 2.4 | `_bPKLog`, `_bCrusadeLog` | SRV | `Game.cpp:43050-43097`, `43012-43047` | PK and crusade logs [V doc] |
| 2.4 | `PutLogFileList`, `PutAdminLogFileList`, `PutLogEventFileList`, `PutDebugMsg` | SRV | `Wmain.cpp:378-396`, `398-416`, `460-478`, `480-498` | `Events.log`, `AdminEvents.log`, `LogEvents.log`, `DebugMsg.log` [V doc] |
| 2.4 | `WriteGameMasterLog`, `WriteGameItemLog`, `WriteCrusadeLog` | WLS | `0x40b470`, `0x40b6a0`, `0x40b8d0` | daily `GMLogData\`, `ItemLogData\`, `EKEventsLog\` files [V doc] |
| 2.4 | `ItemLog`, `PutLogFileList` | GATE | `GateCore.cpp:972-1011`, `Wmain.cpp:405-423` | `ItemEvents.log` (legacy `MSGID_ITEMLOG`), `HackAttacker.log` [V doc] |
| 2.5 | `iClientMotion_Move_Handler` batch check | SRV | `Game.cpp:1149-1180` | 7 walks < 1032 ms or 7 runs < 908 ms -> delete [V] |
| 2.5 | `iClientMotion_Attack_Handler` batch check | SRV | `Game.cpp:11304-11317` | 7 attacks < 1480 ms -> delete [V] |
| 2.5 | `bCheckClientMoveFrequency` / `bCheckClientAttackFrequency` / `bCheckClientMagicFrequency` | SRV | `Game.cpp:52674-52713`, `52615-52641`, `52645-52671` | per-message gap 250 / 500 / 1500 ms; magic one never called [V] |
| 2.5 | `CheckConnectionHandler` | SRV | `Game.cpp:48937-48976` | client elapsed time exceeds server elapsed time by >= 10 s -> delete [V] |
| 2.5 | `CheckClientResponseTime` (exp/hour, skill flood) | SRV | `Game.cpp:4027-4057`, `4016-4021` | > 300000 exp/hour -> one-month block; >= 2 skill messages per tick -> delete [V] |
| 2.5 | `_bDecodePlayerDatafileContents` validation, `_bCheckDupItemID` | SRV | `Game.cpp:6263-6323`, `6097-6099`, `6237-6239` | stat-sum, names, penalty date; NOTUSED and duplicated item ids deleted at load, logged, and refunded as reward gold (`persistence.md` 7) [V] |
| 2.6 | `CGateCore::bAccept` | GATE | `GateCore.cpp:231-283` | IP allow-list; miss -> `HackAttacker.log` [V doc] |
| 2.6 | `HandleAcceptSocket`, accept handler | WLS / MLS | `WLS 0x4019e0`, `MLS 0x402770` | internal-port allow-lists [V doc] |

## 4. Commands and messages

From `research/systems/commands/by-system.md` (gm, monitor, logging rows) and `chat-commands.md`:
- Slash commands (server): `/who`, `/shutup`, `/closeconn`, `/attack`, `/createfish`, `/teleport`, `/tp`,
  `/summondemon`, `/summonplayer`, `/unsummonall`, `/unsummonboss`, `/checkip`, `/polymorph` (dead), `/setinvi`,
  `/gns`, `/setattackmode`, `/setforcerecalltime`, `/summon`, `/disconnectall`, `/createitem`,
  `/enableadmincreateitem 147258`, `/enableadmincommand 0909114`, `/shutdownthisserverrightnow`,
  `/setobservermode`; plus GM-only crusade/arena commands `/begincrusadetotalwar`, `/endcrusadetotalwar`,
  `/energysphere`, `/reservefightzone`, `/getticket`.
- Notifies: `DEF_NOTIFY_ADMINUSERLEVELLOW`, `_TOTALUSERS`, `_IPACCOUNTINFO`, `_ADMINIFO`, `_OBSERVERMODE`,
  `_GLOBALATTACKMODE`, `_PLAYERSHUTUP`, `_PLAYERONGAME`.
- GSM: `GSM_REQUEST_SUMMONPLAYER`, `GSM_REQUEST_SHUTUPPLAYER`, `GSM_RESPONSE_SHUTUPPLAYER`,
  `GSM_REQUEST_SETFORCERECALLTIME`.
- Logs to WLS: `MSGID_GAMEMASTERLOG`, `MSGID_GAMEITEMLOG` (items and PK), `MSGID_GAMECRUSADELOG`; dead:
  `MSGID_ITEMLOG` (to gate).
- Monitor tools (client socket on the game server): `MSGID_MANAGER_INIT`, `MSGID_MANAGER_SHUTDOWN` (dead),
  `MSGID_BWM_INIT`, `MSGID_BWM_COMMAND_SHUTUP`, `MSGID_RESPONSE_CHECKCONNECTION`; gate monitor set in
  `server-topology.md` 4. Dead client message: `MSGID_ADMINUSER` (`RequestAdminUserMode` is empty).
- MLS admin tool: `REGISTERDBSERVER`, `ACCOUNTINFOCHANGE`, `ACCOUNTCONNSTATUS`, `CLEARACCOUNTCONNSTATUS`,
  `BLOCKACCOUNT`, `ACCOUNTTIMECHANGE` (`research/shared/messages/admin-log.md`).

## 5. Data

| Item | Where | Notes |
|---|---|---|
| `m_iAdminUserLevel` | `CClient`, saved as `admin-user-level` / DB offset 181 | set only by editing the record [V `SRV Game.cpp:5731`; doc offset] |
| `m_bIsAdminCommandEnabled`, `m_bIsAdminCreateItemEnabled` | `CClient` | session-only, cleared on reconnect [V `SRV Game.cpp:46337, 46351`] |
| `m_bIsManager`, `m_bIsBWMonitor` | `CClient` | manager excluded from user count (`Game.cpp:2878, 40742`); BWM flag never read [V] |
| `m_iTimeLeft_ShutUp` | `CClient`, saved | ticks of 3 s; `/shutup` uses minutes x 20, BWM uses 600 (`research/game-server/functions/chat-gm.md`) [V doc] |
| `m_iPenaltyBlockYear/Month/Day` | `CClient`, saved | blocks loading until the date passes (`SRV Game.cpp:6319-6323`) [V] |
| `m_dwSpeedHackCheckTime`, `m_iSpeedHackCheckExp` | `CClient` | hourly exp window (`Game.cpp:4027-4057`) [V] |
| `m_iMoveMsgRecvCount`, `m_dwMoveLAT`, `m_iRunMsgRecvCount`, `m_dwRunLAT`, `m_iAttackMsgRecvCount`, `m_dwAttackLAT` | `CClient` | batch windows [V] |
| `m_dwMoveFreqTime`, `m_dwAttackFreqTime`, `m_dwMagicFreqTime` | `CClient` | per-message gap, **client-supplied time** [V `Game.cpp:52615-52713`] |
| `m_dwInitCCTime`, `m_dwInitCCTimeRcv` | `CClient` | clock-skew base (`Game.cpp:48948-48951`) [V] |
| `DupItemID.cfg` | WLS push | list of known duplicated item ids (`research/servers/game-log-protocol.md` 5); matching items are deleted at load and refunded as reward gold (`Game.cpp:6097-6101, 6237-6241, 6419`) [V] |
| Log files | SRV: `Events.log`, `AdminEvents.log`, `LogEvents.log`, `DebugMsg.log`; WLS: `GMLogData\GMEvents<date>.log`, `ItemLogData\ItemEvents<date>.log`, `EKEventsLog\EKEvents<date>.log`; GATE: `ItemEvents.log`, `HackAttacker.log`, `GameServerLog\GameServerLog<date>.log` | [V doc] |

## 6. Flow

**F1. GM command, local** (example `/closeconn <name>`):
1. CLI chat input sends `MSGID_COMMAND_CHATMSG` with the text (chat system) [V doc].
2. SRV `ChatMsgHandler`: must be initialised, not muted, sender name must match (`Game.cpp:10547-10555`); `/`
   branch matches `"/closeconn "` (`10821`) -> `AdminOrder_CloseConn` [V].
3. `AdminOrder_CloseConn`: level >= 2 and (non-test build) `m_bIsAdminCommandEnabled`, else
   `DEF_NOTIFY_ADMINUSERLEVELLOW` (`research/game-server/functions/chat-gm.md`) [V doc]; finds the target,
   `DeleteClient(i, TRUE, TRUE, TRUE, TRUE)` -> target saved and jailed in `bisle` for 10 min
   (`SRV Game.cpp:36707, 2706-2716`) [V]; logs `MSGID_GAMEMASTERLOG` -> WLS `GMEvents` file [V doc].

**F2. GM command, cross-server** (example `/summonplayer <name>` for a player on another server): the GM's
server does not find the name locally, stocks `GSM_REQUEST_SUMMONPLAYER`; the gate fans it out; the owning
server's `ServerStockMsgHandler` teleports the player to the GM's map via `RequestTeleportHandler`, which may
itself trigger a server change (`SRV Game.cpp:44306-44414, 49573`; `session.md` F2) [V doc].

**F3. Speed hack (move)**: each `MSGID_COMMAND_MOTION` move -> `iClientMotion_Move_Handler` counts walks;
on the 7th, if less than 72*8*7-3000 ms passed since the window start, `DeleteClient(i, TRUE, TRUE)`
(`SRV Game.cpp:1149-1158`) [V]. After the move is applied, `bCheckClientMoveFrequency` compares the
client-stamped time with the previous one; under 250 ms -> delete (`1058`, `52674-52713`) [V].

**F4. Exp abuse**: every 3 s tick, per client, once an hour has passed since `m_dwSpeedHackCheckTime`: if exp
rose by more than 300000, set the penalty-block date one month ahead and `DeleteClient(i, TRUE, TRUE)`; the
save carries the block date, so the next load fails validation until the date passes
(`SRV Game.cpp:4027-4057, 6319-6323`) [V]. The window is a fixed (tumbling) one that restarts only after a
passing check (`4057-4062`), and the penalty fields live in the character record (`m_iPenaltyBlock*`), not the account.

**F5. Bad-word monitor mute**: an external process connects to the game-server client port, sends
`MSGID_BWM_INIT` (no password) then `MSGID_BWM_COMMAND_SHUTUP` with a name at offset 16; the server mutes that
player for 600 ticks (~30 min) and writes `AdminEvents.log` (`SRV Game.cpp:14986-14991, 40688-40720`) [V].

## 7. Rules and invariants

- Admin level gates (shipped build; from `chat-gm.md` and verified spot checks): `/teleport` `/tp` `/setinvi`
  `/enableadmincommand` >= 1; `/shutup` `/closeconn` `/attack` `/createfish` `/checkip` `/gns` `/summonplayer`
  `/reservefightzone` `/getticket` `/energysphere` >= 2; `/summondemon` `/unsummonall` `/unsummonboss`
  `/polymorph` `/setattackmode` `/setforcerecalltime` `/summon` `/setobservermode` `/enableadmincreateitem`
  `/who` (Chinese build) crusade start/end >= 3; `/disconnectall` `/createitem` >= 4;
  `/shutdownthisserverrightnow` == 4 (`SRV Game.cpp:10753, 10958-11043, 46329, 46370`) [V].
- Commands that also need `/enableadmincommand` this session (non-test build): shutup, closeconn, attack,
  summondemon, summonplayer, unsummonall, unsummonboss, setforcerecalltime, summon, disconnectall, energysphere,
  crusade start/end (`research/game-server/build-config.md` TESTSERVER row) [V doc]. `/createitem` needs
  `/enableadmincreateitem` instead (`SRV Game.cpp:46385`) [V]. `/shutdownthisserverrightnow` needs neither
  (`11011-11027`) [V].
- GM immunities (level > 0): not attackable (`SRV Game.cpp:12600`), always resists magic and ice
  (`32312, 32380`), unaffected by dynamic-object damage (`33306, 33419, 33567`), skipped by NPC target search
  (`11707`), no hunger (`3883`), exempt from map level limits / force recall (`4061, 4071, 4106`), may teleport
  where players may not (`23273, 23281, 23702, 23713`), no spike damage (`1246`); levels 1..4 enter invisible
  (`4934-4935`) [V].
- All GM chat is copied to `MSGID_GAMEMASTERLOG`, and players' whispers to a GM are logged too
  (`SRV Game.cpp:10577-10582, 11203-11208`) [V].
- Speed thresholds: walk batch 7 in < 1032 ms, run batch 7 in < 908 ms, attack batch 7 in < 1480 ms
  (`SRV Game.cpp:1156, 1171, 11309`); gaps 250 ms move, 500 ms attack, 1500 ms magic (uncalled); clock skew
  >= 10 s (`48960`); exp > 300000 per hour (`4030`) [V]. All except the clock check can be disabled with
  `NO_MSGSPEEDCHECK` (batch) or are always on (gap) (`GlobalDef.h:6`) [V].
- Item log only for "good" items: gold over 10000, a fixed id list, or attribute mask `0xF0F0F001` with id > 30
  (`research/game-server/functions/ui-logging.md` `_bCheckGoodItem`) [V doc].
- Gate allow-list: exact 16-byte IP compare, at most 10 entries, empty list rejects everyone
  (`research/gate/subsystems.md` 3) [V doc].

## 8. Couplings

Out of admin:
- **session**: `/closeconn` and every anti-hack check call `DeleteClient`; the penalty block is enforced in the
  load validation (`_bDecodePlayerDatafileContents`) [V].
- **persistence**: admin level, shut-up time and penalty date are record fields; logs ride the Log link [V].
- **combat / magic / movement** (gameplay cluster): GM immunity checks inside `iCalculateAttackEffect`,
  `bCheckResistingMagicSuccess`, `bCheckResistingIceSuccess`, `DynamicObjectEffectProcessor`, `TargetSearch`;
  speed checks inside the motion handlers [V].
- **NPC** (gameplay): `/summon`, `/summondemon`, `/attack` use `bCreateNewNpc`, `bSetNpcAttackMode`,
  `bSetNpcFollowMode`; `/unsummon*` use `NpcKilledHandler` [V doc].
- **crusade / arena / gathering / items**: GM triggers `GlobalStartCrusadeMode`, `ManualEndCrusadeMode`,
  `EnergySphereProcessor`, `AdminOrder_ReserveFightzone`, `AdminOrder_CreateFish`, `AdminOrder_CreateItem`
  (`_bInitItemAttr`, `_bAddClientItemList`) [V doc].
- **chat** (social cluster): commands live inside `ChatMsgHandler`; mutes block chat (`m_iTimeLeft_ShutUp`) [V].
- **server-topology**: `/shutdownthisserverrightnow` (code 2), `/disconnectall` (`_iForcePlayerDisconect`) [V].

Into admin:
- **item, trade, shop, guild, PK, crusade systems** call `_bItemLog`, `_bPKLog`, `_bCrusadeLog` at their
  transaction points (caller lists in `research/game-server/functions/ui-logging.md`) [V doc].
- **network**: `ClientMotionHandler` hosts the per-message frequency checks [V].

## 9. Current behaviour vs intent

- `MSGID_BWM_COMMAND_SHUTUP` is accepted from any client socket that passed `bAccept` ([ADMIN-001](../issues/admin.md#admin-001-monitor-messages-carry-no-login-check)).
- The manager password and the GM enable passwords are compiled-in literals, and `_Manager_Init` can be repeated ([ADMIN-002](../issues/admin.md#admin-002-manager-password-is-a-shared-literal-_manager_init-is-repeatable), [ADMIN-003](../issues/admin.md#admin-003-gm-enable-passwords-are-compiled-in-literals)). `_Manager_Shutdown` returns immediately ([TOPO-007](../issues/server-topology.md#topo-007-manager-shutdown-is-dead)).
- Motion frequency checks use the client clock and run after the action ([ADMIN-004](../issues/admin.md#admin-004-frequency-checks-use-the-client-clock-and-run-after-the-action)); the magic check has no caller ([ADMIN-005](../issues/admin.md#admin-005-magic-speed-check-never-wired)).
- The clock-skew check fires only on a fast client clock, and its ban is commented out ([ADMIN-006](../issues/admin.md#admin-006-clock-skew-check-only-catches-a-fast-clock)).
- The `/createitem` subnet check is commented out ([ADMIN-007](../issues/admin.md#admin-007-ip-restriction-on-createitem-removed)).
- BWM shut-up uses 600 ticks, `/shutup` minutes x 20 ([ADMIN-008](../issues/admin.md#admin-008-shut-up-units-disagree)).
- Item logging is mostly compiled out and the gate's item-log path is unused ([ADMIN-009](../issues/admin.md#admin-009-item-log-coverage-is-narrow-in-this-build)).
- `RequestAdminUserMode` is empty ([ADMIN-010](../issues/admin.md#admin-010-requestadminusermode-is-empty)); `/polymorph` does nothing ([ADMIN-011](../issues/admin.md#admin-011-polymorph-parses-and-does-nothing)); `/shutdownthisserverrightnow` writes no GM log ([ADMIN-012](../issues/admin.md#admin-012-shutdownthisserverrightnow-writes-no-gm-log)).

## 10. Open questions

- What the bad-word monitor and manager programs were (no binaries in the tree) and whether they ran on the
  same LAN only [U].
- Whether GM levels above 4 (other than the >= 10 disguise) had meaning in other builds [U: only the level
  tests above exist in 2.20].
- Whether the WLS or an outside tool ever acted on the item logs automatically (for example generating
  `DupItemID.cfg`) [U].

## 11. Doc discrepancies

- `research/game-server/functions/network.md` says `bCheckClientAttackFrequency` "always returns FALSE (callers
  ignore the value)"; confirmed, and the callers run it **after** the attack is processed (`SRV Game.cpp:1082,
  1101`) [V]. Not a conflict, an addition.
- `research/game-server/functions/chat-gm.md` lists `/who` at level >= 3 "under DEF_CHINESE (10753)"; confirmed
  (`SRV Game.cpp:10752-10756`) [V].

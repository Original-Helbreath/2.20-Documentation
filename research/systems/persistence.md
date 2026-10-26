# System: persistence (accounts, characters, guilds, save cadence, World Log server, DB mode)

*Size verdict (2026-10-25): one cohesive doc. Ownership (who stores what) frames every other section,
so it is not split; use the Contents block. Field lists stay in the format docs and are not repeated.*

Cluster: infrastructure. Citation tags `SRV`, `CLI`, `GATE`, `MLS`, `WLS` and [V]/[I]/[U] as in
`network.md` (header). Line numbers as of 2026-10-25. Formats: `research/shared/formats/persistence-files.md`,
`research/servers/persistence.md`; wire layouts: `research/servers/game-log-protocol.md` 6.

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

Keep everything a player owns - account, characters, items, bank, skills, guild membership - outside
the game-server processes, so game servers can crash, reboot or be added without losing anyone, and
a player can move between game servers carrying one record. The player experiences it as "my
character is where I logged out" and, on a crash, "I lost up to half an hour". The operator
experiences a folder tree of plain text files on the Log server host
(`Account\`, `Character\`, `Guild\`, `AscII<n>` buckets) that can be edited by hand.

Design intent [I]: **the game server owns the meaning of the character record; the Log server owns its
storage.** The WLS stores and returns the text verbatim and parses only the few keys it needs for
ownership and guild checks (`research/shared/formats/persistence-files.md` "Character file") [V doc].

## 2. Subsystems

```
persistence
 +- 2.1 Ownership split          MLS: accounts; WLS: characters + guilds; game server: nothing durable
 +- 2.2 Record codec             game server encode/decode; two build variants (FILE text / DB binary)
 +- 2.3 Load path                REQUEST_PLAYERDATA -> RESPONSE_PLAYERDATA -> decode + validate
 +- 2.4 Save cadence             autosave 30 min, logout, server change, duplicate kick, forced paths
 +- 2.5 Fallback local save      game server writes a dated text file when no Log socket is usable
 +- 2.6 Guild store              create / disband / member add / member remove; guild-status check on load
 +- 2.7 Account store            MLS account file: password, validity, block date, character names
 +- 2.8 Server-side state files  occupy flags (WLS), CrusadeGUID / OccupyFlag (game server, local)
 +- 2.9 Account-state table      WLS NULL/INIT/WAIT/PLAYING (in memory only; see session.md)
```

Intent per subsystem [I]:
- **2.1** game servers are disposable; durable data lives behind one process per world (WLS) and one per
  service (MLS) [V doc `research/servers/topology.md`].
- **2.2** one human-readable format the game server writes and reads back (FILE), plus a later binary
  "v2.14 DB save speed-up" variant (`SRV Game.cpp:4499` comment; `GlobalDef.h:268` note "faster WORLD server save by
  sending CharID", which reads as a speed-up of the world-server store) [I].
- **2.3** refuse to put a corrupted or tampered record into the world.
- **2.4** bound loss on a crash (30 min) and always save on exit; save before a hand-off so the next server
  loads the current state.
- **2.5** never lose a logout when the Log server link is down.
- **2.6** guilds outlive their members' sessions; a guild deleted while a member was offline is noticed on
  the member's next login.
- **2.7** accounts are an MLS concern; game servers never see account files.
- **2.8** crusade state survives restarts.

## 3. Functions

| Sub | Function | Program | file:line | Role |
|---|---|---|---|---|
| 2.2 | `_iComposePlayerDataFileContents` (DB, active) | SRV | `Game.cpp:6428-6941` | binary block C encoder [V doc] |
| 2.2 | `_bDecodePlayerDatafileContents` (DB, active) | SRV | `Game.cpp:5551-6426` | binary decoder + validation [V doc] |
| 2.2 | `_iComposePlayerDataFileContents` (FILE, inactive) | SRV | `Game.cpp:8917-9475` | `key = value` text encoder [V doc] |
| 2.2 | `_bDecodePlayerDatafileContents` (FILE, inactive) | SRV | `Game.cpp:6945-8914` | text decoder, 77 keys [V doc] |
| 2.3 | `RequestInitPlayerHandler` -> `bSendMsgToLS(MSGID_REQUEST_PLAYERDATA)` | SRV | `Game.cpp:1608`, `4384-4422` | load request [V] |
| 2.3 | `ResponsePlayerDataHandler`, `InitPlayerData` | SRV | `Game.cpp:4796-4853`, `4855-5043` | load reply, decode, guild-status, validation drop [V doc] |
| 2.3 | `HandleRequestPlayerData`, `CheckGuildStatusForPlayerData` | WLS | `0x403640`, `0x403b10` | read file, check owner, guild byte [V doc] |
| 2.4 | `CheckClientResponseTime` (autosave) | SRV | `Game.cpp:3927-3933` | 30 min, not in fight zones [V] |
| 2.4 | `DeleteClient` (save branch) | SRV | `Game.cpp:2639-2852` | logout save / no-save / wait status [V] |
| 2.4 | `RequestTeleportHandler` (cross-server) | SRV | `Game.cpp:23369, 23437, 23470, 23512` | save-with-reply [V] |
| 2.4 | `bSendMsgToLS` (save cases) | SRV | `Game.cpp:4469-4522` | header + char ID (DB) + count flag + record [V] |
| 2.4 | `HandleSavePlayerData` | WLS | `0x404e80` | write `Character\AscII<c>\<name>.txt`; logout clears account; reply variant acks [V doc] |
| 2.5 | `LocalSavePlayerData` | SRV | `Game.cpp:38736-38794` | dated local file when the logout save cannot be sent [V] |
| 2.6 | `RequestCreateNewGuildHandler` / `ResponseCreateNewGuildHandler` | SRV | via `bSendMsgToLS` `Game.cpp:4558-4598`; reply `16547-16610` | guild create round trip (guild system owns rules) [V doc] |
| 2.6 | `HandleCreateNewGuild`, `HandleDisbandGuild`, `HandleGuildAddGuildsman`, `HandleGuildDelGuildsman` | WLS | `0x409170`, `0x409940`, `0x409d30`, `0x409fa0` | guild file writes [V doc] |
| 2.7 | account reader `sub_404400`, create `0x40b890`, password `0x40caa0` | MLS | as named | account file (`official-mainserver.md` 4) [V doc] |
| 2.8 | `SaveOccupyFlagData` | SRV | `Game.cpp:47991-48010` | returns immediately (dead) [V] |
| 2.8 | `_CreateCrusadeGUID` | SRV | `Game.cpp:50420-50433` | writes `GameData\CrusadeGUID.txt` locally (crusade cluster) [V] |
| 2.8 | `HandleSaveOccupyFlagData` | WLS | `0x408f80` | writes occupy-flag files; never reached from 2.20 [V doc] |

## 4. Commands and messages

- Game -> WLS: `MSGID_REQUEST_PLAYERDATA`, `MSGID_REQUEST_SAVEPLAYERDATA` (autosave),
  `MSGID_REQUEST_SAVEPLAYERDATA_REPLY` (server change), `MSGID_REQUEST_SAVEPLAYERDATALOGOUT` (logout),
  `MSGID_REQUEST_NOSAVELOGOUT`, `MSGID_REQUEST_CREATENEWGUILD`, `MSGID_REQUEST_DISBANDGUILD`,
  `MSGID_REQUEST_UPDATEGUILDINFO_NEWGUILDSMAN` / `_DELGUILDSMAN`; dead: `MSGID_REQUEST_SAVEARESDEN/ELVINEOCCUPYFLAGDATA`.
- WLS -> game: `MSGID_RESPONSE_PLAYERDATA`, `MSGID_RESPONSE_SAVEPLAYERDATA_REPLY`,
  `MSGID_RESPONSE_CREATENEWGUILD`, `MSGID_RESPONSE_DISBANDGUILD`, `MSGID_GUILDNOTIFY` (ignored by 2.20),
  occupy-flag pushes `0x17081034/35` (`official-worldserver.md` 3.1).
- Client -> MLS -> WLS: `MSGID_REQUEST_CREATENEWCHARACTER`, `MSGID_REQUEST_DELETECHARACTER`,
  `MSGID_REQUEST_CHANGEPASSWORD`, `REQUEST/RESPONSE_CHARINFOLIST` (session system drives them).
- Counting flag byte in every save/no-save message: TRUE = real logout, FALSE = duplicate kick or server
  change (`research/servers/game-log-protocol.md` 3) [V doc].

## 5. Data

| Store | Owner | Path / location | Notes |
|---|---|---|---|
| Character file | WLS | `Character\AscII<c>\<name>.txt`, `<c>` = decimal first byte | text, game-server format; new-file template written by WLS `0x405850` (`official-worldserver.md` 7) [V doc] |
| Guild file | WLS | `Guild\AscII<c>\<guild>.txt` | master, GUID, location, `[GUILDSMAN]` list [V doc] |
| Account file | MLS | `Account\AscII<c>\<account>.txt` | plaintext password, valid/block dates, `[CHARACTERS]` [V doc] |
| Occupy flags | WLS | `GameData\AresdenOccupyFlag.txt`, `ElvineOccupyFlag.txt` | pushed at registration; never saved by 2.20 [V doc] |
| Crusade GUID | game server | `GameData\CrusadeGUID.txt` | read at start (`SRV Game.cpp:20237`), written by `_CreateCrusadeGUID` [V] |
| Local fallback | game server | `Character_<M>_<D>_<H>_<Min>\AscII<c>\<name>.txt` | `SRV Game.cpp:38754-38781` [V] |
| Binary block C (DB mode) | wire only | 443 fixed bytes + skills + items (53 each) + bank flag + bank items (48 each) | `research/servers/game-log-protocol.md` 6.5 [V doc] |
| `m_dwCharID` | `CClient` | DB-mode record id, echoed at offset 36 of saves | `SRV Game.cpp:4498-4503` [V] |
| `m_bIsBankModified` | `CClient` | DB-mode hint so the store can skip the bank rewrite | written at `SRV Game.cpp:6870`; set TRUE at `27745, 27822, 27966, 32512` [V] |
| Limits | | 50 inventory, 120 bank, 60 skills, 100 spells, names 10 bytes | `research/game-server/data-structures.md` CClient [V doc] |

## 6. Flow

**F1. Load** (game -> WLS -> game): `RequestInitPlayerHandler` sends `MSGID_REQUEST_PLAYERDATA` (name,
account, password, client IP) on a sub-log socket (`SRV Game.cpp:1608, 4384-4422`) -> WLS opens the file,
checks `account-name`, computes the guild-status byte by opening the guild file and comparing
`guild-GUID`, replies with the text (`WLS 0x403640, 0x403b10`) -> SRV `ResponsePlayerDataHandler` finds the
client by name -> `InitPlayerData` decodes; a guild-status 0 clears the guild and notifies
`DEF_NOTIFY_GUILDDISBANDED`; any validation failure deletes the client as "data error"
(`SRV Game.cpp:4855-4960`; `game-log-protocol.md` 6.2) [V doc].

**F2. Autosave**: every 3 s tick, for each initialised client not in a fight zone whose last save is older
than 30 min: `bSendMsgToLS(MSGID_REQUEST_SAVEPLAYERDATA, i)` (`SRV Game.cpp:3927-3933`) -> WLS
`HandleSavePlayerData(0,0)` overwrites the file, no reply [V; WLS V doc].

**F3. Logout save**: `DeleteClient(i, TRUE, ...)` for an initialised client not changing server:
respawn rewrite, then `MSGID_REQUEST_SAVEPLAYERDATALOGOUT` with the count flag; if `bSendMsgToLS` returns
FALSE (no sub-socket), `LocalSavePlayerData` (`SRV Game.cpp:2639, 2775-2800`) -> WLS writes the file, clears
the account state and sends `CLEARACCOUNTSTATUS` to the MLS (`WLS 0x404e80`) [V; WLS V doc].

**F4. Hand-off save**: `RequestTeleportHandler` sends `MSGID_REQUEST_SAVEPLAYERDATA_REPLY` with count flag
FALSE and waits; only the WLS ack triggers `DEF_NOTIFY_SERVERCHANGE` (`SRV Game.cpp:23369, 36354-36372`).
This ordering is what guarantees the next server loads the post-teleport record (`session.md` F2) [V].

**F5. Guild create** (persistence part): game server validates (guild system), sends
`MSGID_REQUEST_CREATENEWGUILD` with a clock-made GUID (`SRV Game.cpp:4558-4598`) -> WLS creates the guild file
or rejects if it exists (`WLS 0x409170`) -> SRV `ResponseCreateNewGuildHandler` sets rank 0 and deducts the
fee on confirm (`SRV Game.cpp:16547-16610`) [V doc].

**F6. Character create/delete** (MLS + WLS): MLS checks password, stat ranges and name, forwards to the WLS
which writes the starter file; on success the MLS appends `account-character = <name>`. Delete removes the
name from the account file **before** the WLS deletes the character file (`official-mainserver.md` 3.3-3.4) [V doc].

## 7. Rules and invariants

- Save cadence: autosave every `DEF_AUTOSAVETIME` = 30 min (`SRV Game.h:60`), skipped while in a fight zone
  (`Game.cpp:3928`); always on logout, timeout, kick and server change; never on trade, drop or
  purchase [V; "never" by grep: the only `MSGID_REQUEST_SAVEPLAYERDATA` caller is `3931`].
- The timer starts at accept (`m_dwAutoSaveTime` set in `bAccept`, `SRV Game.cpp:427`), so the first autosave
  is 30 min after connecting, not after entering [V].
- Count flag: TRUE only for genuine logouts; FALSE for duplicate kicks (`SRV Game.cpp:1564, 1586`) and server
  changes (`23369`) [V].
- A save is sent only for initialised clients (`DeleteClient` checks `m_bIsInitComplete`, `SRV Game.cpp:2775`);
  others get `NOSAVELOGOUT` (`2802`) [V].
- Loaded records are refused unless stats sum to `(level-1)*3+70` with each in 10..200, names pass
  `bCheckValidName`, the penalty-block date has passed and every item name exists (`game-log-protocol.md` 6.2) [V doc].
- Load-time item cleanup: the decoders delete items of type `DEF_ITEMTYPE_NOTUSED` and any item whose id is in
  the DupItemID list (`_bCheckDupItemID`, logged via `_bItemLog(DUPITEMID)`), and credit the sum of their prices to
  `m_iRewardGold` (`+= iNotUsedItemPrice`; sent in `1729`). They also reset a 0-life Gem of Sacrifice to 1 and
  set `m_iSpeedHackCheckExp = m_iExp`. Sites: inventory `Game.cpp:6078-6101`, bank `6226-6241`, `6410-6422`;
  FILE decoder `7300-7312`, `7856`, `8907` [V].
- Body size cap 30000 bytes bounds the record: 50 items x 53 + 120 bank x 48 + fixed part fits (derived) [I].
- Passwords are stored and transmitted in plaintext everywhere (account file, every save message, the
  WLS INIT entry) [V doc].

## 8. Couplings

Out of persistence:
- **every gameplay system** owns fields in the record: the codec (`_iComposePlayerDataFileContents` /
  `_bDecodePlayerDatafileContents`) is the coupling point; adding a field means touching both and the
  WLS template [V doc].
- **guild** (social cluster): `RequestCreateNewGuildHandler`, `ResponseCreateNewGuildHandler`,
  `ResponseDisbandGuildHandler`, guildsman updates (`SRV Game.cpp:19085, 19128, 36766` callers) [V doc].
- **crusade** (gameplay cluster): `m_dwCrusadeGUID` in the record and in `GameData\CrusadeGUID.txt`;
  occupy-flag saving (dead) [V].
- **item anti-dup** (admin): `_bCheckDupItemID` and `_bItemLog(DEF_ITEMLOG_DUPITEMID)` run inside the DB
  decoder (calls at `SRV Game.cpp:6097`, `6237`; log writes `6099`, `6239`); hits are deleted and refunded as
  reward gold, not only logged [V].

Into persistence:
- **session**: chooses which save message to send on every exit path (`DeleteClient`) [V].
- **server-topology**: the sub-log sockets and their failure; loss of all sub-sockets forces shutdown, which
  drains players through the save path [V].
- **admin / logging**: GM, item and crusade logs ride the same Log link and end up as WLS files
  (`admin.md`) [V doc].

## 9. Current behaviour vs intent

- `DEF_CHINESE` selects the binary block C save/load layout, which the only existing WLS (text-mode files) does not speak ([PERSIST-001](../issues/persistence.md#persist-001-the-shipped-game-server-build-cannot-talk-to-the-shipped-wls)).
- The DB-build fallback save writes block C with `fopen(..., "wt")` ([PERSIST-002](../issues/persistence.md#persist-002-fallback-save-writes-binary-in-text-mode)).
- `SaveOccupyFlagData` returns immediately, so occupy flags are not persisted ([PERSIST-003](../issues/persistence.md#persist-003-occupy-flag-persistence-is-dead)).
- The bank-modified flag is never cleared after a save ([PERSIST-004](../issues/persistence.md#persist-004-bank-modified-flag-never-clears)).
- Items are not saved on trade or drop, so a crash rolls back to the last autosave ([PERSIST-005](../issues/persistence.md#persist-005-item-state-between-autosaves-is-volatile-duplication-window)).
- The MLS account-creation code writes `Hack = <password>` instead of `account-password = ` ([PERSIST-006](../issues/persistence.md#persist-006-account-creation-is-broken-in-the-mls-binary)).
- The WLS checks file ownership but not the password on load; the password is checked later at ENTERGAMECONFIRM for INIT/WAIT accounts only ([SESS-001](../issues/session.md#sess-001-no-session-ticket-credential-checks-are-weak)).

## 10. Open questions

- What stored DB-mode records in the Chinese/USA deployments (the `DBGAMESERVER` partner) and its schema [U:
  no binary or source].
- Whether `DupItemID.cfg` was maintained by hand or generated from item logs [U].
- Whether the WLS ever backs up character files (`backup-drive-letter` is read but no use was found,
  `official-worldserver.md` 1) [U].

## 11. Doc discrepancies

- `research/servers/game-log-protocol.md` 3 lists the autosave site as `SRV Game.cpp:3928-3933`; the send is
  at `3931` inside that range [V]. No conflict.
- `research/game-server/functions/persistence.md` cites `LocalSavePlayerData` path building at 38755-38768;
  confirmed (`SRV Game.cpp:38754-38781`) [V].

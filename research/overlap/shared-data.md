# shared data formats and config

Conventions: see `../overlap.md`. Field-level layouts stay in `../shared/formats/` and
`../servers/`; this file says who writes and who reads each artefact.

| Artefact | Writer(s) | Reader(s) | Format | Reference |
|---|---|---|---|---|
| Account file | MLS (create, password, connection adds) | MLS | text, key = value | `../servers/official-mainserver.md` 4 [V] |
| Character file `Character\AscII<c>\<name>.txt` | WLS (new-character template); GS content via save messages, written by WLS | WLS reads -> GS decodes; WLS reads names/appearance for the char list | text sections `[NAME-ACCOUNT]`..`[EOF]`, 77 keys | `../shared/formats/persistence-files.md`; official-worldserver.md 7 [V] |
| Guild file `Guild\AscII<c>\<guild>.txt` | WLS on GS guild create/update/disband | WLS (guild-status byte in player data) | text `[GUILD-INFO]`, `[GUILDSMAN]` | official-worldserver.md 7 [V] |
| Content cfgs (Item, Item2/3, Npc, Magic, Skill, Potion, Quest, BuildItem, DupItemID, Noticement) | operator | WLS reads, pushes verbatim; GS decodes (`_bDecode*ConfigFileContents`, SRV Game.cpp:15198-15268) | text, one push message per file | `../shared/formats/server-content-cfg.md` [V] |
| Client display cfgs (ItemName, magic/skill/BItem, GameMsgList) | operator | client only | text, some XOR-obfuscated | `../shared/formats/client-files.md` [V] |
| Occupy flags `GameData\*OccupyFlag.txt` | WLS (on GS save) | WLS pushes to GS | text | `../issues/persistence.md#persist-003-occupy-flag-persistence-is-dead` |
| Crusade files (`Crusade.cfg`, `WarSchedule.cfg`, `CrusadeGUID.txt`) | operator; GS writes GUID | GS only (each server its own copy) | text | `../shared/formats/server-program-cfg.md` [V] |
| `.amd` maps | map editor | **client and GS, separate loaders** | binary, 256-byte header, 10-byte tiles | `../shared/formats/amd-map.md` [V both loaders, amd-map.md:19] |
| Map info `.txt` | operator | GS only | text keys | `../shared/formats/mapdata-txt.md` |
| `GServer.cfg` | operator | GS | text | server-program-cfg.md |
| `GateServer.cfg` | operator | gate | text; `build-date` must match GS `DEF_BUILDDATE` | `../issues/setup-2.20.md#setup-001-gate-build-date-612-does-not-match-the-game-server-429` |
| `WLserver.cfg` | operator | WLS; GS receives it as `MSGID_WLSERVERFILECONTENTS` and takes `world-server-port` | text | topology.md:37-40 [V] |
| `HMLserver.cfg`, `LOGIN.CFG` | operator | MLS; client | text; client address must equal MLS public address | `../issues/setup-2.20.md#setup-011-the-clients-log-server-address-is-hard-coded` |
| Logs (GM, item, crusade, EK) | GS emits over log link; gate writes `ItemEvents.log` | WLS writes daily files | text | official-worldserver.md 7; game-gate-protocol.md 2 [V] |

## Patterns
- **One writer per persistent file.** Every account, character and guild file is written only by a
  log server; the GS never touches disk for players except its fallback save
  (`../issues/persistence.md#persist-002-fallback-save-writes-binary-in-text-mode`) [V].
- **The record format is shared across three programs** that each hold part of the schema: WLS
  writes the initial template, GS is the full codec, WLS (and MLS via CONFIRM bytes) read a subset
  for the character list. Binary vs text save mode must agree:
  `../issues/setup-2.20.md#setup-003-binary-vs-text-saves-the-game-server-must-be-built-in-file-mode`.
- **Content is server-pushed, display text is client-local.** Item/magic/skill numbers exist twice:
  authoritative in the WLS cfgs, display names in the client cfgs, joined only by ID or name [V
  client-files.md:58].
- **Maps are the only binary asset both sides parse.** Client and server each have their own `.amd`
  loader over one file.

## Doc discrepancies
None found in this pass.

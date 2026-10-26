# Persistence: what the Log server stores and in which format

Tags and [V]/[I]/[U] as in `topology.md`. The Log server source is lost, so only the **character
file** format is verified (the game server writes and reads it). The account and guild files below
are proposals that match the style of the character file and the fields the protocol needs; every
field there is [I].

## Contents
1. Who owns what
2. Folder layout
3. Character file (verified)
4. Account file (inferred)
5. Guild file (inferred)
6. New character defaults
7. Text mode vs binary mode
8. What the gate and game server store

## 1. Who owns what

| Data | Owner | Evidence |
|---|---|---|
| Accounts, passwords | Log server | client login goes only to the Log server; passwords travel in every game -> Log player message (`game-log-protocol.md` 3) [V] |
| Characters (all stats, items, bank, skills) | Log server | `REQUEST_PLAYERDATA` / `SAVEPLAYERDATA*` [V] |
| Guilds (name, master, location, GUID, existence) | Log server | `CREATENEWGUILD`, `DISBANDGUILD`, guild-status byte in `RESPONSE_PLAYERDATA` (`SRV Game.cpp:4937-4946`) [V] |
| Guild membership | the character file (`character-guild-name`, `-GUID`, `-rank`); the Log server also gets add/remove events (`UPDATEGUILDINFO_*`) [V] | |
| Game data configs (items, NPCs, ...) | Log server's folder; pushed at registration | `game-log-protocol.md` 5 [V] |
| Parties | gate, memory only | `gate-server.md` [V] |
| Crusade GUID | game server, `GameData\CrusadeGUID.txt` | `SRV Game.cpp:20233-20239` [V] |

## 2. Folder layout

Verified fallback written by the game server when no Log socket is available
(`LocalSavePlayerData`, `SRV Game.cpp:38736-38790`; same in 2.03 at `SRV203 Game.cpp:33109`):

```
Character_<month>_<day>_<hour>_<minute>\AscII<decimal code of the first name byte>\<name>.txt
```

The Log server most likely used the same bucket scheme under a fixed `Character\` root, e.g.
`Character\AscII65\Abc.txt` for "Abc" [I: the game server copies a layout the Log-server team knew;
`SRV ascII.txt` is a year-2000 listing of codes 0-255 in the same log format, consistent with a tool
that walked these buckets]. Names are used as filenames, which is consistent with the name rules that
reject filename-unsafe characters (`client-log-protocol.md` 6) [I].

Proposed full layout [I] - confirmed by the official binaries: `Account\AscII<n>\<account>.txt` (MLS), `Character\AscII<n>\<name>.txt` and `Guild\AscII<n>\<guild>.txt` (WLS) (corrected from official binary, MainServer.exe 0x40394c, WorldServer.exe 0x403640, 0x409170):

```
Account\AscII<n>\<account>.txt
Character\AscII<n>\<character>.txt
Guild\AscII<n>\<guild>.txt
```

Case: Windows filenames are case-insensitive, so "abc" and "ABC" collide. Treat names as
case-insensitive for uniqueness [I].

## 3. Character file (verified)

Writer: `SRV Game.cpp:8917-9474` (FILE variant of `_iComposePlayerDataFileContents`). Reader: FILE
variant of `_bDecodePlayerDatafileContents`, `SRV Game.cpp:6945-8916`, keys at `8591-8712`. The reader
tokenises on `=`, spaces, tabs and newlines and matches keys by prefix, so spacing is free and order
inside a section does not matter, but **sections must appear in this order**: `[ITEM-EQUIP-STATUS]`
must be last because equip flags and positions apply to the items already read (comment at
`SRV Game.cpp:9435`). [V]

Plaintext passwords do **not** appear in the character file the game server writes. [V] (The official WLS writes `account-password = <password>` into a brand-new character file, which the first game-server save overwrites: corrected from official binary, WorldServer.exe 0x4183b8.) They travel in clear in the protocol
(client -> Log after the XSocket cipher; game -> Log in plain text) and so must be stored by the Log
server in its account file (4). [V transport / I storage]

```
[FILE-DATE]

file-saved-date: 2026 10 25 13 5          year month day hour minute

[NAME-ACCOUNT]

character-name     = Abc
account-name       = abcacct              written, not read back

[STATUS]

character-profile 	=__________           "__________" when empty; one token, no spaces
character-location   = NONE               NONE | aresden | elvine (other values the game server checks: see its location handling)
character-guild-name = NONE
character-guild-GUID = -1
character-guild-rank = -1                 -1 none, 0 master, 1.. members
character-loc-map = default
character-loc-x   = -1                    -1/-1 = map's initial point
character-loc-y   = -1

character-HP       = 0                    <= 0 is replaced by max HP when saved
character-MP       = 0
character-SP       = 0
character-LEVEL    = 1
character-RATING   = 0
character-STR      = 10
character-INT      = 10
character-VIT      = 10
character-DEX      = 10
character-MAG      = 10
character-CHARISMA = 10
character-LUCK     = 10
character-EXP      = 0
character-LU_Str   = 0                    pending level-up points, 0..3 each, total <= 3
character-LU_Vit   = 0
character-LU_Dex   = 0
character-LU_Int   = 0
character-LU_Mag   = 0
character-LU_Char  = 0
character-EK-Count = 0
character-PK-Count = 0
character-reward-gold = 0
character-downskillindex = -1
character-IDnum1 = 0                      0 = game server assigns an ID
character-IDnum2 = 0
character-IDnum3 = 0
party-rank = 0
party-IDnum1 = 0
party-IDnum2 = 0
party-IDnum3 = 0

sex-status       = 1                      1 male, 2 female
skin-status      = 1
hairstyle-status = 0
haircolor-status = 0
underwear-status = 0
hunger-status    = 100
timeleft-shutup  = 0
timeleft-rating  = 0
timeleft-force-recall  = 0
timeleft-firm-staminar = 0
admin-user-level = 0
penalty-block-date = -1 -1 -1             year month day
character-quest-number = 0
character-quest-ID     = 0
current-quest-count    = 0
quest-reward-type      = 0
quest-reward-amount    = 0
character-contribution = 0
character-war-contribution = 0
character-quest-completed = 0
special-event-id = 0
super-attack-left = 0
reserved-fightzone-id = 0 0 0             number, reserve time, ticket
special-ability-time = 0
locked-map-name = NONE
locked-map-time = 0
crusade-job = 0
crusade-GUID = 0
construct-point = 0
dead-penalty-time = 0
party-id = 0
gizon-item-upgade-left = 0                (sic: "upgade")
appr1 = 0                                 written, not read; recomputed by the game server
appr2 = 0
appr3 = 0
appr4 = 0
appr-color = 0

[ITEMLIST]

character-item = Dagger               1 0 0 0 0 0 0 0 0 300 0
character-bank-item = Gold                 1000 0 0 0 0 0 0 0 0 0 0

[MAGIC-SKILL-MASTERY]

//------------------012345678901234567890123456789012345678901234567890
magic-mastery     = 0000...                100 digits, '1' = spell learned
skill-mastery     = 0 0 0 ...              60 numbers
skill-SSN     = 0 0 0 ...                  60 numbers

[ITEM-EQUIP-STATUS]

item-equip-status = 00000000000000000000000000000000000000000000000000   50 flags, one per character-item line in order
item-position-x = 40 40 ...                50 numbers, inventory x per item line (unused slots 40)
item-position-y = 30 30 ...                50 numbers (unused slots 30)
[EOF]
```

The values shown are illustrative except where the comment states a rule. Item line fields
(`SRV Game.cpp:9307-9346`): name space-padded to 20 characters, then count, touch-effect type, touch
values 1-3, colour, special-effect values 1-3, current lifespan, attribute (all decimal). Bank lines
have the same 11 numbers. Up to 50 items and 120 bank items (`SRV Client.h:25-26`). [V]

Other accepted keys: the reader also takes `character-account-status`, `character-DefenseRatio`,
`character-HitRatio` (`SRV Game.cpp:8591-8712`), which the writer no longer emits. [V]

Load validation (fail = the player is dropped as "data error"): see `game-log-protocol.md` 6.2. In
particular: stat sum must equal (level-1)*3+70 with each stat 10..200, and every item name must exist
in the pushed item config. [V]

**Recommended Log server handling:** treat the file as an opaque blob. On save, write the payload
text to `Character\AscII<n>\<name>.txt` (after checking that `character-name` matches the message's
name field). On load, send the file back. Parse only the few fields the Log server needs for the
character list (2.20 slot record, `client-log-protocol.md` 5): level, the five appearance values,
stats, sex/skin/hair/underwear, map name, location, guild info, logout date. [I]

## 4. Account file (inferred)

The official account format differs from this proposal; see `official-mainserver.md` section 4 (corrected from official binary, MainServer.exe 0x40c043).

Fields come from what the protocol carries or needs [I]; format follows the character file's style.

```
[ACCOUNT]

account-name        = abcacct
account-password    = secret12             plaintext, max 10 characters (the wire field is 10 bytes)
account-created     = 2026 10 25 13 5
account-status      = 0                    0 normal, 1 blocked (reject reason in client-log-protocol.md 10)
account-block-date  = -1 -1 -1
account-email       = ...                  CREATENEWACCOUNT fields (client-log-protocol.md 3):
account-gender      = ...                  e-mail 50, gender 10 (random on the client), age 10,
account-age         = ...                  country 17, SSN 28, quiz 45, answer 20
account-country     = ...
account-ssn         = ...                  store or drop
account-quiz        = ...
account-answer      = ...

[CHARACTERS]

account-character-1 = Abc
account-character-2 = NONE
account-character-3 = NONE
account-character-4 = NONE

[EOF]
```

- Plaintext password: the user recalls plaintext storage; the protocol is consistent with it (the
  Log server receives and resends the password in clear, and `CHECKACCOUNTPASSWORD` compares strings,
  `SRV Game.cpp:46052-46083`). [V protocol / I storage]
- Character slots: the client shows at most 4 (`CLI Game.cpp:29457`, `client-log-protocol.md` 5). [V count / I key names]
- Account state (online, which server, changing server) is runtime data; keep it in memory, not in the
  file. [I]

## 5. Guild file (inferred)

The official guild format differs from this proposal; see `official-worldserver.md` section 7 (corrected from official binary, WorldServer.exe 0x409170).

```
[GUILD]

guild-name      = Knights
guild-GUID      = 1729900000               from CREATENEWGUILD; generated by the game server
guild-master    = Abc
guild-location  = aresden
guild-created   = 2026 10 25 13 5

[MEMBERS]

guild-member    = Abc 0                    name rank
guild-member    = Def 1

[EOF]
```

Needed because: the Log server must reject a duplicate guild name on create (`RESPONSE_CREATENEWGUILD`
REJECT), check master and name on disband, and report "guild gone" (guild-status byte 0) when a
member of a disbanded guild logs in. Membership is updated from `UPDATEGUILDINFO_NEWGUILDSMAN` /
`DELGUILDSMAN`. The guild's members' own files keep their rank. [V needs / I format]

## 6. New character defaults

The Log server creates the character file on `CREATENEWCHARACTER` (`client-log-protocol.md` 6).
Values from the request: name, gender, skin, hair style, hair colour, underwear, six stats (10-14 each,
sum 70). Defaults [I unless cited]:

| Field | Value | Reason |
|---|---|---|
| level | 1 | stat-sum rule (level-1)*3+70 = 70 [V rule] |
| map, x, y | `default`, -1, -1 | traveller start map; `default.amd` exists in `SRV mapdata`; dead travellers return to it (`SRV Game.cpp:2652-2655`) |
| location | `NONE` | traveller; set when the player joins a town [V meaning] |
| guild | `NONE`, GUID -1, rank -1 | writer defaults (`SRV Game.cpp:8957-8980`) [V] |
| HP, MP, SP | 0 | replaced by max values by the game server [V for HP at `SRV Game.cpp:8996-8997`; MP/SP I] |
| luck | 10 | |
| hunger | 100 | |
| penalty date | -1 -1 -1 | |
| IDnum1-3 | 0 | the game server assigns an ID when IDnum1 is 0 [V, decoder] |
| items | starting kit (weapon, clothes, potions...) | unknown which; the game server accepts any valid item names [U] |
| magic | 100 zeros; skills 60 zeros | |

The original kit is unknown [U]. Any item from `Item*.cfg` loads. Now known: Dagger, RecallScroll, Red/Blue/GreenPotion, KneeTrousers(M) and Shirt(M) or Chemise(W) (corrected from official binary, WorldServer.exe 0x417e58-0x418018; full template in `official-worldserver.md` 7).

## 7. Text mode vs binary mode

The exported HGserver tree defines `DEF_CHINESE`, which turns on `DEF_DBGAMESERVER` (binary player
block) (`SRV GlobalDef.h:16, 72-73`). Recommendation [I]: **build the game server without
`DEF_DBGAMESERVER`** (Korean or other-country flags, or just remove the define). Then the save and load
payloads are exactly the character file, the Log server stays a file store, and the text format above
is the only one to support. If the binary build must be kept, the Log server converts between block C
(`game-log-protocol.md` 6.5) and the text file.

## 8. What the gate and game server store

- Gate: three append-only logs; no party, guild or account files (`gate-server.md` 6). [V]
- Game server: logs (`ui.md`), `GameData\CrusadeGUID.txt`, and the emergency character dumps of section 2. [V]

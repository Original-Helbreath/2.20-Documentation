# Account, character and guild files

Owned by the Log server (MLS: accounts; WLS: characters and guilds). The full field lists are
already in `research/servers/`; this page says who reads and writes each file and where the
game-server side of the character text lives. Tags: [V] read in code or binary, [I] inferred.

## Contents
- Summary
- Character file
- Account file
- Guild file
- Gaps

## Summary

| File | Path on the Log server | Written by | Read by | Field list |
|---|---|---|---|---|
| Account | `Account\AscII<c>\<account>.txt`, `<c>` = decimal of the first byte | MLS on CREATENEWACCOUNT / password change | MLS on LOGIN | `research/servers/official-mainserver.md` section 4 (`:164`) [V binary]; proposed format `persistence.md` section 4 [I] |
| Character | `Character\AscII<c>\<char>.txt` | WLS on SAVEPLAYERDATA* (text arrives from the game server) and on CREATENEWCHARACTER | WLS on REQUEST_PLAYERDATA (text goes to the game server) | `research/servers/persistence.md` section 3 (`:56`) [V]; WLS handling `official-worldserver.md:128-130` [V binary] |
| Guild | `Guild\AscII<c>\<guild>.txt` | WLS on CREATENEWGUILD / guildsman add/del | WLS guild-status check | `official-worldserver.md:129,146` [V binary]; `persistence.md` section 5 [I] |

`srv220/ascII.txt` is a log listing `ASCII(0..n)` folder creation, not a data file [V sample].

## Character file

Text of `key = value` lines; the Log server stores it verbatim and the game server is the only
program that interprets it [V `official-worldserver.md:130`].

- Game server encoder: `CGame::_iComposePlayerDataFileContents` `.claude/scratch/srv220/Game.cpp:8917`
  (FILE mode; DB-mode twin `:6428` under `#ifdef DEF_DBGAMESERVER`, `:5549`) [V].
- Game server decoder: `CGame::_bDecodePlayerDatafileContents` `srv220/Game.cpp:6945` (FILE mode;
  twin `:5551`), separators `"= \t\n"` (`:6948`) [V].
- Keys the 2.20 decoder recognises (77, in code order, `srv220/Game.cpp:6945-8900`) [V]:
  `character-loc-map`, `character-loc-x`, `character-loc-y`, `character-account-status`,
  `character-item`, `character-bank-item`, `sex-status`, `skin-status`, `hairstyle-status`,
  `haircolor-status`, `underwear-status`, `item-equip-status`, `character-guild-name`,
  `character-guild-rank`, `character-HP`, `character-DefenseRatio`, `character-HitRatio`,
  `character-LEVEL`, `character-STR`, `character-INT`, `character-VIT`, `character-DEX`,
  `character-MAG`, `character-CHARISMA`, `character-LUCK`, `character-EXP`, `magic-mastery`,
  `skill-mastery`, `character-location`, `character-MP`, `character-SP`, `character-LU_Str`,
  `character-LU_Vit`, `character-LU_Dex`, `character-LU_Int`, `character-LU_Mag`,
  `character-LU_Char`, `character-EK-Count`, `character-PK-Count`, `character-reward-gold`,
  `skill-SSN`, `character-profile`, `hunger-status`, `admin-user-level`, `timeleft-shutup`,
  `timeleft-rating`, `character-RATING`, `character-guild-GUID`, `character-downskillindex`,
  `item-position-x`, `item-position-y`, `character-IDnum1..3`, `penalty-block-date`,
  `character-quest-number`, `current-quest-count`, `quest-reward-type`, `quest-reward-amount`,
  `character-contribution`, `character-quest-ID`, `character-quest-completed`,
  `timeleft-force-recall`, `timeleft-firm-staminar`, `special-event-id`, `super-attack-left`,
  `reserved-fightzone-id`, `special-ability-time`, `character-war-contribution`,
  `locked-map-name`, `locked-map-time`, `crusade-job`, `construct-point`, `crusade-GUID`,
  `dead-penalty-time`, `party-id`, `gizon-item-upgade-left`, `[EOF]`.
  Value layouts per key: `persistence.md` section 3.
- The Log server also reads `account-name`, `character-guild-name/-rank/-GUID` itself
  (`official-worldserver.md:128-129`) [V binary].
- Binary player data (DB mode) exists only on the game-server side; the official WLS has no
  binary parser (`official-worldserver.md:130`) [V binary].

## Account file

Only the official MLS reads or writes it; no source exists [V by search]. Format and path
builder: `official-mainserver.md:164-200`.

## Guild file

Only the official WLS touches it. The game server keeps guild state in the character file and
sends create/disband/member messages (`../messages/game-log.md`, `MSGID_REQUEST_CREATENEWGUILD`
and neighbours) [V header]. Format: `official-worldserver.md` (guild file section) and
`persistence.md` section 5.

## Gaps

- 2.03 single-Log-server account and guild formats: no source, inferred only (`persistence.md`).

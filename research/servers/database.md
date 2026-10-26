# Database-backed Log servers (DB mode)

What is known about the SQL databases behind the official Log servers when the game server is built in DB mode.
Tags and source abbreviations as in `topology.md`; [V] verified, [I] inferred, [U] unknown. Nothing was run:
the executables were read as files only.

## Contents
1. Summary
2. Evidence
3. DB mode in the 2.20 game server
4. Connection
5. Account database `mainlogin`
6. Character database `ws1`
7. Crusade and Heldenian results
8. Queries found in the executables
9. What is missing
10. Relevance to the 2.20 recreation

## 1. Summary

- The official database-backed Log servers used **Microsoft SQL Server**, reached through ADO over ODBC
  (`Provider=MSDASQL.1`), logging in as `sa` with a password compiled into the executable [V, 3.51-era files].
- Two databases: **`mainlogin`** (accounts, Main Log server) and **`ws1`** (characters, World Log server; one per
  world) [V].
- The account schema is complete. The character tables are known by name and key only [V].
- All of this comes from 3.51-era files (2003-2004). No 2.20-era database file is known; that 2.20 used the same
  setup is [I]: its DB mode dates from v2.14 (2002-02-15) and its record matches these tables (section 6).
- DB mode is a build option, not the only mode: 2.20 also has the text-file mode (`persistence.md`), which the
  official `WorldServer.exe` V2.15 we have uses [V].

## 2. Evidence

| Item | Where | Date | Used for |
|---|---|---|---|
| `WorldLServer-3.51.exe` | `downloads/outpost/351SqlFiles.zip` | PE 2003-04-16 | connection string, character queries |
| `MainLServer.exe` | same zip | PE 2004-04-28 | connection string, account queries |
| `mainlogin.txt` | same zip; kept in 2.20-Reference `as-found/sql-3.51/` | - | account database schema |
| `RebuildIdx.sql` | `downloads/outpost/351configsandsqldump.rar`; 2.20-Reference `as-found/sql-3.51/` | - | character database name and tables |
| `CrusadeScript.SQL`, `HeldenianScript.SQL` | same rar; 2.20-Reference `as-found/sql-3.51/` | - | war result tables and procedures |

The pack comes from a Spanish-language operator (`mainlogin.txt` puts the data files in
`C:\Archivos de Programa\Microsoft SQL Server\MSSQL\data\`), so the account script may carry local edits; the
schema comments are in English and one column is marked "for japan" [V]. The two war scripts carry the original
Korean comments (cp949) [V]. The executables are not in any repo; they stay in the Downloads folder.

## 3. DB mode in the 2.20 game server

- `DEF_DBGAMESERVER` ("DB 게임서버 모드") is switched on by `DEF_CHINESE`, `DEF_JAPAN`, `DEF_USA`,
  `DEF_INTERSERVER`, `DEF_TAIWAN` and `DEF_TESTSERVER`; Korea and no-flag builds use text files
  (`SRV GlobalDef.h:35, 42, 59, 73`) [V].
- Introduced in v2.14, 2002-02-15, "to speed up World server saves (send and receive the CharID)"
  (`SRV GlobalDef.h:268`) [V].
- The game server itself has no database code; the only trace is an unused `char sql[256]` in
  `_iComposePlayerDataFileContents` (`SRV Game.cpp:6431`) [V]. DB mode only changes what goes over the wire:
  a 4-byte `m_dwCharID` and the binary block C instead of the character text (`game-log-protocol.md` 6.2-6.5) [V].

## 4. Connection

| Server | Connection string (password left out here) | Meaning |
|---|---|---|
| World Log server | `Data Source=character;User ID=sa;pwd=...;Provider=MSDASQL.1;` | ODBC data source named `character` [V] |
| Main Log server | `DSN=account;User ID=sa;pwd=...;Provider=MSDASQL.1;` | ODBC DSN named `account` [V] |

Both are UTF-16 strings in the executables, and both import `ole32` / `OLEAUT32` (`CoCreateInstance`, `OleRun`):
ADO used from C++ through COM [V imports; ADO I from the provider string]. The DSNs point at the databases
`mainlogin` and `ws1` [I from the scripts].

## 5. Account database `mainlogin`

From `mainlogin.txt` [V]. Data and index on separate files (filegroup `Second`).

`ACCOUNT_T`, primary key `cAccountID`, unique `iAccountID`:

| Column | Type | Note |
|---|---|---|
| `cAccountID` | NCHAR(10) NOT NULL | account name |
| `cPasswd` | NCHAR(10) | password, plaintext |
| `iAccountID` | INT NOT NULL | numeric account ID |
| `cLoginIpAddress` | NCHAR(20) | last login IP |
| `IsGMAccount` | BIT | GM flag |
| `CreateDate`, `LoginDate`, `LogoutDate`, `BlockDate` | SMALLDATETIME | `BlockDate` drives "Account Blocked ... Date(%d %d %d)" |
| `cRealName` | NCHAR(30) | widened from 10 |
| `cSSN` | NCHAR(20) | national ID number |
| `cEmail` | NCHAR(60) | widened from 50, "for japan" |
| `cCharRecord` | NCHAR(100) | "Character Document" |
| `cQuiz`, `cAnswer` | NCHAR(45), NCHAR(20) | recovery question |
| `ValidDate`, `iValidTime` | SMALLDATETIME, INT | paid time (billing) |

`WS1_T`, `WS2_T` (one per world server): `iAccountID` INT NOT NULL, `cCharName` NCHAR(10); foreign key to
`ACCOUNT_T.iAccountID` with cascading delete and update. Index `WS1_IDX` on `WS1_T(iAccountID)`, fill factor 80.
This is the character list shown at login. The Main Log server reads it as `select * from %s_T where ...` with
the world name filled in [V], so the table name follows the world name (`WS1`, matching the client's forced
world name, `client-log-protocol.md`).

Also referenced by the Main Log server, with no script: `CODE_T` (`where cKEYCODE = '%s'`, a key or coupon code
lookup) [V query, I purpose] and an "account-time-info" insert (billing time) [V string].

## 6. Character database `ws1`

`RebuildIdx.sql` (`use ws1`) rebuilds the indexes of six tables: `character_t`, `skill_t`, `item_t`,
`bankitem_t`, `guild_t`, `guildmember_t` [V]. No `CREATE TABLE` for them is known. Columns visible in the
World Log server's queries [V]: `CHARACTER_T.CharID`, `CHARACTER_T.cCharName`; `SKILL_T`, `ITEM_T`, `BANKITEM_T`
keyed by `CharID`; `GUILD_T.cGuildName`; `GUILDMEMBER_T.cGuildName`, `cMemberName`.

The rest can be derived from the game server's DB-mode record (`game-log-protocol.md` 6.5), which maps onto the
tables one to one [I]:

| Table | Holds (from block C) |
|---|---|
| `CHARACTER_T` | `CharID` plus the fixed part: ID numbers, level, stats, EXP, level-up points, look, location and map, profile, admin level, guild name / GUID / rank, quests, crusade and war fields, HP/MP/SP, EK/PK, timers, magic mastery string |
| `SKILL_T` | one row per skill with mastery > 0: `CharID`, skill ID, mastery, SSN |
| `ITEM_T` | one row per carried item (max 50): `CharID` and the 53-byte item record fields |
| `BANKITEM_T` | one row per bank item (max 120): `CharID` and the 48-byte bank record fields; the record's "bank modified" byte lets the server skip rewriting it |
| `GUILD_T`, `GUILDMEMBER_T` | guilds and their members by name |

## 7. Crusade and Heldenian results

From `CrusadeScript.SQL` and `HeldenianScript.SQL` [V]. Same design for both:
- `CrusadeReportT` / `HeldenianReportT`: one row per finished war (`startDate`, `endDate`, `winner` in
  aresden / elvine / draw; Heldenian adds `type`). Not indexed, grows forever.
- `CrusadeResultT` / `HeldenianResultT`: exactly three rows (aresden, elvine, draw) with `totalWin`, `lastWar`
  (win / lose / draw) and `lastResultCount` (streak length).
- Procedures: `sp_SetCrusadeWinner(@Winner, @StartDate, @EndDate)` updates the winner and loser rows and appends
  the report; `sp_GetCrusadeLastWinner` / `LastLoser` read them back. The Heldenian set mirrors it with `@Type`.

Which program called these is [U]: no `sp_` name appears in the two Log server executables [V]. 2.20 has no
Heldenian; its crusade results go through the Log server's occupy-flag messages instead (`official-compat-220.md` 4).

## 8. Queries found in the executables

World Log server 3.51 [V]:
```
select * from CHARACTER_T where CharID = (select MAX(CharID) from CHARACTER_T)
select * from CHARACTER_T where cCharName = ('%s')
select * from CHARACTER_T where upper(cCharName) = upper('%s')
select * from CHARACTER_T where upper(cCharName) = upper('%s') and left(cCharName,1) = ('%c')
select * from CHARACTER_T where charID = %u
select * from SKILL_T where CharID = %ld
select * from ITEM_T where CharID = %ld
select * from BANKITEM_T where CharID = %ld
delete SKILL_T where CharID = %ld          (and ITEM_T, BANKITEM_T)
select * from GUILD_T where cGuildName = '%s'
select * from GUILDMEMBER_T where cMemberName = '%s'
delete GUILD_T where cGuildName = '%s'
delete GUILDMEMBER_T where cGuildName = '%s'
delete GUILDMEMBER_T where cMemberName = '%s'
delete GUILDMEMBER_T where (cGuildName = '%s' and cMemberName = '%s')
```
Main Log server 3.51 [V]:
```
select * from ACCOUNT_T where cAccountID = '%s'
select * from ACCOUNT_T where iAccountID = '%d'
select * from ACCOUNT_T where iAccountID = (select MAX(iAccountID) from ACCOUNT_T)
select * from %s_T where iAccountID = %ld
select * from %s_T where cCharName = ('%s')
select * from CODE_T where cKEYCODE = '%s'
```
Observations:
- There is no `INSERT` or `UPDATE` text: rows are written through ADO recordsets (open with `select`, then add or
  change fields) [I]. New IDs come from `MAX(...)`, not an identity column [V query].
- A save deletes a character's skill, item and bank rows and writes them again [I from the deletes].
- Queries are built with `wsprintf` around quoted names, so a name containing `'` breaks them [I].
- Column names do not appear as strings, so fields were read by position [I].

## 9. What is missing

- The `CREATE TABLE` statements for the six `ws1` tables, their column names and types [U].
- Any data, and any 2.20-era version of these files [U].
- The 2.20-era DB-mode World Log server itself; only the 3.51 one exists [V].

## 10. Relevance to the 2.20 recreation

The recreation targets text-file storage (`recreation-plan.md` 1), so none of this is required. It matters if the
Log server should also serve a game server built as shipped (`DEF_CHINESE`, DB mode): then the Log server must
speak block C and keep the `CharID`, and the tables in sections 5-6 are the model to follow.

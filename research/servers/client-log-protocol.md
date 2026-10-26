# Client <-> Log server protocol (v2.20)

Tags and [V]/[I]/[U] as in `topology.md`. Framing and cipher as in `wire-format.md`. Offsets below
are **body offsets** (0 = MsgID, 4 = MsgType, 6 = first payload byte). Client build analysed:
`DEF_LANGUAGE 4` (English, `CLI GlobalDef.h:1`).

## Contents
1. Connection model and addresses
2. Message ID table
3. Requests (layouts)
4. Responses: how the client parses them
5. Character-selection screen (character list)
6. Character-creation screen
7. Delete character
8. Selecting a character and entering the game
9. Server change (map hosted by another game server)
10. Result codes and on-screen messages
11. Version check, world name, hard-coded address

## 1. Connection model and addresses

- One TCP connection per operation. The client connects, sends the request selected by
  `m_dwConnectMode` once `FD_CONNECT` fires (`CLI Game.cpp:3041-3079`), handles the first reply in
  `LogResponseHandler`, then deletes the socket (`CLI Game.cpp:15446-15448`). The Log server can reply
  and keep or drop the connection; the client closes it. [V]
- The client ignores the reply's MsgID; it switches only on the MsgType word (`CLI Game.cpp:14781-14813`).
  So the Log server may use `MSGID_RESPONSE_LOG` (0x0FC94203) for account/character replies and
  `MSGID_RESPONSE_ENTERGAME` (0x0FC94206) for enter-game replies (confirmed, and the official ENTERGAME CONFIRM is only 24 bytes, without the game-server name: corrected from official binary, MainServer.exe 0x407019, WorldServer.exe 0x405634), as the names in
  `SRV NetMessages.h:269, 272` suggest. [V client side / I which ID the original used]
- Address: `login.cfg` keys `log-server-address`, `log-server-port` (`CLI Game.cpp:15606-15609`), **but
  only when `DEF_LANGUAGE <= 3`**. The analysed English build ignores the file and hard-codes
  `61.100.0.103:2848` (`CLI Game.cpp:15543-15556`); `DEF_TESTSERVER` hard-codes `203.234.215.200:2848`
  (`CLI Game.cpp:15528-15534`). A recreated server therefore needs a rebuilt or patched client ([SETUP-011](../issues/setup-2.20.md#setup-011-the-clients-log-server-address-is-hard-coded)). [V]
- Port arithmetic `m_iLogServerPort + (rand() % 1)` is always the configured port (`CLI Game.cpp:27820`). [V]

## 2. Message IDs (client -> Log)

| MsgID | Value | Sent from | Size (body) | Reply MsgTypes |
|---|---|---|---|---|
| `MSGID_REQUEST_LOGIN` | 0x0FC94201 | `CLI Game.cpp:1665-1703` | 56 | CONFIRM, REJECT, PASSWORDMISMATCH, NOTEXISTINGACCOUNT, NOTEXISTINGWORLDSERVER, billing codes |
| `MSGID_REQUEST_CREATENEWACCOUNT` | 0x0FC94202 | `CLI Game.cpp:1605-1663` | 264 | NEWACCOUNTCREATED, NEWACCOUNTFAILED, ALREADYEXISTINGACCOUNT, INVALIDKOREANSSN, LESSTHENFIFTEEN |
| `MSGID_REQUEST_CREATENEWCHARACTER` | 0x0FC94204 | `CLI Game.cpp:1705-1761` | 77 | NEWCHARACTERCREATED, NEWCHARACTERFAILED, ALREADYEXISTINGCHARACTER |
| `MSGID_REQUEST_ENTERGAME` | 0x0FC94205 | `CLI Game.cpp:1763-1803` | 200 | ENTERGAMERESTYPE_CONFIRM / REJECT / PLAYING / FORCEDISCONN |
| `MSGID_REQUEST_DELETECHARACTER` | 0x0FC94207 | `CLI Game.cpp:1805-1829` | 66 | CHARACTERDELETED, NOTEXISTINGCHARACTER, REJECT |
| `MSGID_REQUEST_CHANGEPASSWORD` | 0x0FC94210 | `CLI Game.cpp:1574-1603` (China variant 1538-1572) | 46 (China 64) | PASSWORDCHANGESUCCESS, PASSWORDCHANGEFAIL |
| `MSGID_REQUEST_INPUTKEYCODE` | 0x0FC94212 | `CLI Game.cpp:1511-1536`, China build only | 43 | INPUTKEYCODE + reason byte |
| `MSGID_GETMINIMUMLOADGATEWAY` | 0x3B1890EA | `CLI Game.cpp:1665`, only with `DEF_USING_GATEWAY` | 56 | address+port, see section 11 |

IDs: `SRV NetMessages.h:267-290, 450`, same values in `CLI NetMessages.h:253-276, 429`. [V]

## 3. Requests

All strings fixed-width, zero-padded (see `wire-format.md`). MsgType is 0 unless stated.

### MSGID_REQUEST_LOGIN (56)
| Off | Size | Field |
|---|---|---|
| 6 | 10 | account name |
| 16 | 10 | password (plaintext) |
| 26 | 30 | world server name, "WS1" (section 11) |
`CLI Game.cpp:1682-1701`. With `DEF_ACCOUNTLONG` (Japan/Terra) the account field is `DEF_ACCOUNTLEN` long and the server may answer `DEF_LOGRESMSGTYPE_REALACCOUNT` (0x0A04) with the real 10-char account (`CLI Game.cpp:15411-15418`). Not in this build. [V]

### MSGID_REQUEST_CREATENEWACCOUNT (264)
| Off | Size | Field |
|---|---|---|
| 6 | 10 | account name |
| 16 | 10 | password |
| 26 | 50 | e-mail |
| 76 | 10 | gender: the literal "Male" or "Female", **chosen at random** by the client (`CLI Game.cpp:1627-1631`) |
| 86 | 10 | age |
| 96 | 4 | "xxxx" (birth year placeholder) |
| 100 | 2 | "xx" |
| 102 | 2 | "xx" |
| 104 | 17 | country |
| 121 | 28 | SSN / ID number |
| 149 | 45 | quiz (security question) |
| 194 | 20 | answer |
| 214 | 50 | first command-line token, lower-cased (`G_cCmdLineTokenA_Lowercase`) |
`CLI Game.cpp:1612-1662`. [V]

### MSGID_REQUEST_CREATENEWCHARACTER (77)
| Off | Size | Field |
|---|---|---|
| 6 | 10 | character name |
| 16 | 10 | account name |
| 26 | 10 | password |
| 36 | 30 | world name |
| 66 | 1 | gender (1 male, 2 female) |
| 67 | 1 | skin colour (1..3) |
| 68 | 1 | hair style (0..7) |
| 69 | 1 | hair colour (0..15) |
| 70 | 1 | underwear colour (0..7) |
| 71 | 1 | STR |
| 72 | 1 | VIT |
| 73 | 1 | DEX |
| 74 | 1 | INT |
| 75 | 1 | MAG |
| 76 | 1 | CHR |
`CLI Game.cpp:1712-1760`. Ranges: section 6. [V]

### MSGID_REQUEST_ENTERGAME (200)
MsgType = enter kind (`m_wEnterGameType`, `CLI Game.cpp:1768`):
`DEF_ENTERGAMEMSGTYPE_NEW 0x0F1C` (normal), `..._NOENTER_FORCEDISCONN 0x0F1D` (user chose "force
login" after PLAYING), `..._CHANGINGSERVER 0x0F1E` (defined, unused by client 2.20),
`..._NEW_TOWLSBUTMLS 0x0F1F` (after a server change) (`SRV NetMessages.h:312-315`; set at
`CLI Game.cpp:21631, 29418, 45087`). [V]
| Off | Size | Field |
|---|---|---|
| 6 | 10 | character name |
| 16 | 10 | map name: from the character list on first entry (`CLI Game.cpp:21636-21639`), the destination map after a server change (`CLI Game.cpp:45050`) |
| 26 | 10 | account name |
| 36 | 10 | password |
| 46 | 4 | level (i32), from the character list (`CLI Game.cpp:21620`) |
| 50 | 30 | world name |
| 80 | 120 | `G_cCmdLineTokenA`, the launcher's command-line token ("billing string" per comment `CLI Game.cpp:1799`) |
`CLI Game.cpp:1770-1802`. [V]

### MSGID_REQUEST_DELETECHARACTER (66)
MsgType = 1-based slot index of the character (`CLI Game.cpp:1810, 21785`).
| Off | Size | Field |
|---|---|---|
| 6 | 10 | character name (`m_pCharList[slot-1]->m_cName`) |
| 16 | 10 | account |
| 26 | 10 | password |
| 36 | 30 | world name |
`CLI Game.cpp:1812-1828`. [V]

### MSGID_REQUEST_CHANGEPASSWORD (46; China 64)
| Off | Size | Field |
|---|---|---|
| 6 | 10 | account |
| 16 | 10 | old password |
| 26 | 10 | new password |
| 36 | 10 | new password again |
China (`DEF_LANGUAGE == 2`) inserts an 18-byte SSN after the old password (`CLI Game.cpp:1556-1559`). `CLI Game.cpp:1580-1602`. [V]

### MSGID_REQUEST_INPUTKEYCODE (43, China only)
account 10, password 10, key code 17 (`CLI Game.cpp:1518-1535`). [V]

## 4. Responses (as parsed by `CLI Game.cpp:14773-15449`)

| MsgType | Value | Payload after offset 6 | Client action | Cite |
|---|---|---|---|---|
| `DEF_LOGRESMSGTYPE_CONFIRM` | 0x0F14 | login result + character list (section 5) | go to character selection | `CLI Game.cpp:14932-15098` |
| `DEF_LOGRESMSGTYPE_REJECT` | 0x0F15 | blockYear i32, blockMonth i32, blockDay i32 | message "7H": account blocked until date | `CLI Game.cpp:15100-15118` |
| `..._PASSWORDMISMATCH` | 0x0F16 | none | "11" | `CLI Game.cpp:15155-15160` |
| `..._NOTEXISTINGACCOUNT` | 0x0F17 | none | "12" | `CLI Game.cpp:15162-15167` |
| `..._NEWACCOUNTCREATED` | 0x0F18 | none | "54" | `CLI Game.cpp:15169-15174` |
| `..._NEWACCOUNTFAILED` | 0x0F19 | none | "05" | `CLI Game.cpp:15176-15181` |
| `..._ALREADYEXISTINGACCOUNT` | 0x0F1A | none | "06" | `CLI Game.cpp:15183-15188` |
| `..._NOTEXISTINGCHARACTER` | 0x0F1B | none | "Not existing character!" | `CLI Game.cpp:15190-15195` |
| `..._NEWCHARACTERCREATED` | 0x0F1C | name 10, then list (section 6) | "47" | `CLI Game.cpp:15197-15310` |
| `..._NEWCHARACTERFAILED` | 0x0F1D | none | "28" | `CLI Game.cpp:15312-15317` |
| `..._ALREADYEXISTINGCHARACTER` | 0x0F1E | none | "29" | `CLI Game.cpp:15319-15324` |
| `..._CHARACTERDELETED` | 0x0F1F | list (section 7) | "3A" | `CLI Game.cpp:14814-14930` |
| `DEF_ENTERGAMERESTYPE_PLAYING` | 0x0F20 | none | "account in use, force login?" dialog | `CLI Game.cpp:15326-15329` |
| `DEF_ENTERGAMERESTYPE_REJECT` | 0x0F21 | reason u8 (1..7) | see section 10 | `CLI Game.cpp:15362-15379` |
| `DEF_ENTERGAMERESTYPE_CONFIRM` | 0x0F22 | address 16, port u16, game server name 20 | connect to game server | `CLI Game.cpp:15331-15360` |
| `DEF_ENTERGAMERESTYPE_FORCEDISCONN` | 0x0F23 | none | "3X": logging the other session out | `CLI Game.cpp:15381-15386` |
| `..._NOTENOUGHPOINT` | 0x0F30 | none | "7I" (billing) | `CLI Game.cpp:15121-15125` |
| `..._ACCOUNTLOCKED` | 0x0F31 | none | "7K" | `CLI Game.cpp:15128-15132` |
| `..._SERVICENOTAVAILABLE` | 0x0F32 | none | "7L" | `CLI Game.cpp:15135-15139` |
| `..._PASSWORDCHANGESUCCESS` | 0x0A00 | none | "6B" | `CLI Game.cpp:15141-15146` |
| `..._PASSWORDCHANGEFAIL` | 0x0A01 | none | "6C" | `CLI Game.cpp:15148-15153` |
| `..._NOTEXISTINGWORLDSERVER` | 0x0A02 | none | "1Y" | `CLI Game.cpp:15388-15393` |
| `..._INPUTKEYCODE` | 0x0A03 | reason u8 1..5 | "8U/82/81/8V/8W" | `CLI Game.cpp:15395-15408` |
| `..._REALACCOUNT` | 0x0A04 | account | stores it, no close | `CLI Game.cpp:15411-15418` |
| `..._FORCECHANGEPASSWORD` | 0x0A05 | none | "6M" | `CLI Game.cpp:15421-15430` |
| `..._INVALIDKOREANSSN` | 0x0A06 | none | "1a" | `CLI Game.cpp:15432-15436` |
| `..._LESSTHENFIFTEEN` | 0x0A07 | none | "1b" (under 15) | `CLI Game.cpp:15438-15442` |

Values: `CLI NetMessages.h:278-315`. The server header lacks 0x0F30-0x0F32 and 0x0A04-0x0A07 and calls
0x0A03 `INPUTKEYCODEFAILED` (`SRV NetMessages.h:293-309`). Note the value overlap: CONFIRM/REJECT of
log replies equal `DEF_MSGTYPE_CONFIRM/REJECT`, and 0x0F1C..0x0F1F are both result codes and
enter-kinds; the client only ever reads them as results. [V]

## 5. Character-selection screen (character list)

### Login CONFIRM payload
| Off | Size | Field | Use in client |
|---|---|---|---|
| 6 | 2 | server upper version (u16) | must equal `DEF_UPPERVERSION` 2 (section 11) |
| 8 | 2 | server lower version (u16) | must equal `DEF_LOWERVERSION` 20 |
| 10 | 1 | account status | ignored since v1.43 (`CLI Game.cpp:14947`) |
| 11 | 2 | account valid-until year | "Account validated till (y/m/d)" if year != 0 |
| 13 | 2 | month |  |
| 15 | 2 | day |  |
| 17 | 2 | IP valid-until year | "IP validity ..." if year != 0 |
| 19 | 2 | month |  |
| 21 | 2 | day |  |
| 23 | 1 | character count N (0..4) |  |
| 24 | ... | N slot records (below) |  |
| ... | 4 | account time left, seconds (i32) | "Account use time left" when account year is 0 and > 0, else "It is not a registered account." |
| ... | 4 | IP time left, seconds (i32) | IP line |
`CLI Game.cpp:14935-15089`; display `CLI Game.cpp:29609-29650`, strings `CLI lan_eng.h:308-315`. For a
free server send account year 0 and a large time-left, IP year 0 and time-left 0 (shows a blank IP
line, `CLI lan_eng.h:314`). [V layout / I recommended values]

### Slot record (65 bytes; 50 bytes when damaged)
| Off | Size | Field | Displayed? | Character-file source (see `persistence.md`) |
|---|---|---|---|---|
| 0 | 10 | name | drawn under the figure (`CLI Game.cpp:29491`) | file name / `character-name` |
| 10 | 1 | valid flag: non-zero = OK; **0 = damaged file**, then the client skips 40 more bytes from this flag (`CLI Game.cpp:14988-14992`), so a damaged slot is 10 + 40 = 50 bytes and has no figure | - | Log server sets 0 when the file will not parse [I] |
| 11 | 2 | appr1 | figure (`CLI Game.cpp:29473`) | `appr1`; for a new character (hairstyle<<8)\|(haircolor<<4)\|underwear, the formula the game server uses (`SRV Game.cpp:6372`) |
| 13 | 2 | appr2 | figure | `appr2` (0 for new) |
| 15 | 2 | appr3 | figure | `appr3` (0) |
| 17 | 2 | appr4 | figure | `appr4` (0) |
| 19 | 2 | sex (1 male, 2 female) | figure body type; sex 0 = not drawn (`CLI Game.cpp:29467-29484`) | `sex-status` |
| 21 | 2 | skin (1..3) | body type = (sex==1 ? 1 : 4) + skin - 1 (`CLI Game.cpp:29467-29471`) | `skin-status` |
| 23 | 2 | level | drawn (`CLI Game.cpp:29492-29494`); also sent back in ENTERGAME; delete allowed only if < 50 | `character-LEVEL` |
| 25 | 4 | experience (u32) | drawn (`CLI Game.cpp:29496-29497`) | `character-EXP` |
| 29 | 2 | STR | stored, not drawn | `character-STR` |
| 31 | 2 | VIT | not drawn | `character-VIT` |
| 33 | 2 | DEX | not drawn | `character-DEX` |
| 35 | 2 | INT | not drawn | `character-INT` |
| 37 | 2 | MAG | not drawn | `character-MAG` |
| 39 | 2 | CHR | not drawn | `character-CHARISMA` |
| 41 | 4 | appearance colour (i32) | figure colours (`CLI Game.cpp:29477`) | `appr-color` |
| 45 | 2 | year | newest of all slots shown as "Last Logout : y/m/d h : m" (`CLI Game.cpp:29503-29512, 29647-29649`) | `file-saved-date` |
| 47 | 2 | month |  | `file-saved-date` |
| 49 | 2 | day |  | `file-saved-date` |
| 51 | 2 | hour |  | `file-saved-date` |
| 53 | 2 | minute |  | `file-saved-date` |
| 55 | 10 | map name | not drawn; becomes the map field of ENTERGAME (`CLI Game.cpp:21636-21639`) | `character-loc-map` |
Parsing: `CLI Game.cpp:14984-15079` (identical copies at 14830-14925 and 15210-15305). Storage struct:
`CLI CharInfo.h` (`m_cName[12], m_cMapName[12], m_sSkinCol, m_sSex, m_sAppr1..4, m_sStr..m_sChr,
m_sLevel, m_iExp, m_iApprColor, m_iYear..m_iMinute`). [V]

Not in the record: guild, hair fields as separate values, equipment list, location side. Equipment
shows only through appr1-4 and the colour word, which the game server computes and saves in the
character file (`SRV Game.cpp:9267-9289` writes `appr1..appr4`, `appr-color`). The Log server copies
them; it does not compute them except for a brand-new character. [V layout / I that the Log server
copied them from the file]

Slots: at most 4 (`CLI Game.cpp:29457`, `m_pCharList[4]`; "New Character" only if fewer than 4,
`CLI Game.cpp:21770-21777`). Empty slots are simply absent: N counts present characters and the
client fills slots 0..N-1. A name with a space is not drawn (`CLI Game.cpp:29486`). [V]

## 6. Character-creation screen

Client-side rules (`CLI Game.cpp:25819-25920, 25923-26260`):
- Name: 1..10 characters (input box length 11), `bCheckValidName` true, not in the bad-word list
  (`CLI Game.cpp:25862-25865`). `bCheckValidName` (Korean build `CLI Misc.cpp:278-305`) rejects
  `, = space \n \t . \ / : * ? < > | " `` ` and checks DBCS ranges; other builds use
  `CLI Misc.cpp:308-...`. These are exactly the characters a Windows file name cannot hold, which
  supports per-character files named after the character [I].
- Stats: each 10..14, total exactly 70 (create button disabled while points remain,
  `CLI Game.cpp:25863, 25987-25988, 26109-26190`). Presets set STR14 VIT12 DEX14 / MAG14 INT14 VIT12
  (`CLI Game.cpp:26235-26258`). Defaults 10 each with random appearance (`CLI Game.cpp:15484-15497`).
- Appearance ranges: gender 1..2, skin 1..3, hair style 0..7, hair colour 0..15, underwear 0..7
  (`CLI Game.cpp:26069-26106`).

Server rules the Log server must enforce [I unless cited]:
- The same name rules (the game server re-checks the character and account names with
  `bCheckValidName` on every load and refuses the character otherwise, `SRV Game.cpp:6315-6316`) [V].
- Stats 10..14 each, sum 70. The game server refuses any character whose six stats do not sum to
  `(level - 1) * 3 + 70` or are outside 10..200 (`SRV Game.cpp:6286-6307`, `SRV Game.h:74, 87`) [V];
  a level-1 character must therefore have exactly 70.
- Name unique across the world (all accounts) -> else `ALREADYEXISTINGCHARACTER`; account already has
  4 -> `NEWCHARACTERFAILED`; bad account/password -> `NEWCHARACTERFAILED` [I].
- On success reply `DEF_LOGRESMSGTYPE_NEWCHARACTERCREATED` with payload `name 10, count u8, N slot
  records` (`CLI Game.cpp:15200-15306`). The client shows "47" and returns to the menu (it re-logs in to
  see the list) [V].
- Initial character-file values: see `persistence.md` section "New character defaults".

## 7. Delete character

Request: section 3. Client allows it only for level < 50 (`CLI Game.cpp:21782`); the on-screen text
says 30 (`CLI Game.cpp:29551-29556`). Server should re-check level, ownership and password [I].

Success reply `DEF_LOGRESMSGTYPE_CHARACTERDELETED` (0x0F1F): `[6] account status u8 (ignored)
[7] count u8 [8] N slot records` (`CLI Game.cpp:14816-14926`). The client replaces its list and shows
"3A". Failure: `NOTEXISTINGCHARACTER` or `REJECT` (REJECT shows the block-date screen, so prefer
`NOTEXISTINGCHARACTER`) [I].

## 8. Selecting a character and entering the game

1. Client sends `MSGID_REQUEST_ENTERGAME` kind NEW with the slot's name, map and level
   (`CLI Game.cpp:21614-21640`).
2. Log server checks account/password and that the character belongs to the account [I]; if the
   account already has a character in game it replies `PLAYING` (0x0F20) [I from `CLI Game.cpp:15326`].
   The user may then choose force login, which re-sends ENTERGAME with kind `NOENTER_FORCEDISCONN`
   (`CLI Game.cpp:29410-29422`); the Log server then sends `MSGID_REQUEST_FORCEDISCONECTACCOUNT` to the
   game server holding that account (`SRV Game.cpp:15100-15104`) and replies `FORCEDISCONN` (0x0F23,
   "3X, try again shortly") [I for the pairing].
3. Log server finds the game server that registered the requested map (map list from
   `MSGID_REQUEST_REGISTERGAMESERVER`, `SRV Game.cpp:4372-4378`). None -> `REJECT` reason 3 ("game server
   not running") [I].
4. Reply `DEF_ENTERGAMERESTYPE_CONFIRM` (0x0F22):

| Off | Size | Field |
|---|---|---|
| 6 | 16 | game server IP as text (zero-padded) |
| 22 | 2 | game server port (u16) |
| 24 | 20 | game server name (stored as `m_cGameServerName`, echoed to the game server) |
`CLI Game.cpp:15336-15357`. No session key or ticket is issued; the game server later asks the Log
server for the character by name/account/password (`game-log-protocol.md`). [V]

5. Client connects to that address and sends `MSGID_REQUEST_INITPLAYER` (0x05040205) to the game server:
   `[6] name 10 [16] account 10 [26] password 10 [36] observer flag u8 [37] game server name 20`,
   57 bytes (`CLI Game.cpp:1870-1903`). The game server reads up to the observer flag
   (`SRV Game.cpp:1520-1553`). [V]
6. Game server -> Log `MSGID_REQUEST_PLAYERDATA`; on the reply it sends the client
   `MSGID_RESPONSE_INITPLAYER` CONFIRM, or REJECT if the Log server rejected with byte 1 (client shows
   "3J", world full: `SRV Game.cpp:4821-4832`, `CLI Game.cpp:3085-3109`). [V]

The map name the client sends is a routing hint; the authoritative position is in the character file
the game server receives. [I]

## 9. Server change

When a teleport targets a map the current game server does not host (`SRV Game.cpp:23344-23374` and
the recall/teleport cases to 23516):
1. Game server saves the player: `MSGID_REQUEST_SAVEPLAYERDATA_REPLY` (count flag FALSE). [V]
2. Log replies `MSGID_RESPONSE_SAVEPLAYERDATA_REPLY` with the name; the game server sends the client
   `MSGID_NOTIFY` / `DEF_NOTIFY_SERVERCHANGE` (0x0B24): `[6] map 10 [16] Log/world server IP 15
   [31] port i32` = 35 bytes (`SRV Game.cpp:18646-18667`). The IP is the game server's
   `log-server-address`; the port is `world-server-port` from the pushed WLserver config in
   `DEF_DEFENCEHACKING` builds, else the game server's own internal log port (`SRV Game.cpp:18654-18664`). [V]
3. Client drops the game socket, connects to that address:port, and sends ENTERGAME with kind
   `NEW_TOWLSBUTMLS` and the new map (`CLI Game.cpp:45044-45089`). [V]
4. Log server answers as in section 8 with the game server for the new map. [I]

So the client-facing Log listener must also accept ENTERGAME on the world port (2501) or, for
non-`DEFENCEHACKING` builds, on the internal port (3840) alongside game servers [I; the first
message's MsgID tells the two kinds of peer apart].

## 10. Result codes and on-screen messages

ENTERGAME REJECT reason byte (`CLI Game.cpp:15369-15378`) and text (Korean comments
`CLI Game.cpp:31655-31786`, English `CLI lan_eng.h:418-454`):

| Reason | Code | Meaning |
|---|---|---|
| 1 | 3E | trial account: character above the trial level cannot play without a registered account/IP |
| 2 | 3F | registered IP group exceeded its user limit |
| 3 | 33 | game server not running, try later |
| 4 | 3D | account and character data mismatch, contact admin |
| 5 | 3G | game server shutting down |
| 6 | 3Z | server user limit reached |
| 7 | 3J | world full, try another world |

## 11. Version check, world name, hard-coded address

- Version: the CONFIRM words must equal the client's `DEF_UPPERVERSION`/`DEF_LOWERVERSION`, 2 and 20 for
  `DEF_LANGUAGE 4` (`CLI GlobalDef.h:18-20`), checked in release builds only (`CLI Game.cpp:15094-15097`).
  Mismatch -> "version not match" screen. The game server's `Version.h` holds "2"/"20" as strings and
  is not sent by it. [V]
- World name: without `DEF_SELECTSERVER` the client forces "WS1" (`CLI Game.cpp:15516-15523`) and labels
  it "ABADDON Server" in the English build (`CLI Game.cpp:29693-29694`, `CLI lan_eng.h:18`). [V]
- Gateway (compiled out): with `DEF_USING_GATEWAY` the client first sends `MSGID_GETMINIMUMLOADGATEWAY`
  and reads `[6] IP 15 [21] port i32`, then uses that as the Log address (`CLI Game.cpp:14786-14811,
  27747-27752`). [V]

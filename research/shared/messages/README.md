# Message catalogue: conventions

Every `MSGID_*`, `DEF_*` and `GSM_*` constant in the four NetMessages.h files, plus every message
name that appears only in the official Log server tables. Generated, not hand-written: rerun
`python .claude/scratch/shared-ref/gen_messages.py` (writes only this folder) after a source changes.

## Contents
- Sources and tags
- Columns
- Counts
- How the link was assigned (all inferred)
- Caveats and known gaps

## Sources and tags

| Tag | Source | Header |
|---|---|---|
| 2.03 | game server 2.03, `sources/hgserver-librehelbreath` at commit 1c98b2f (extracted to `.claude/scratch/shared-ref/srv203/`) | `NetMessages.h` |
| 2.19g | gate 2.19, `sources/gateserver-librehelbreath` at commit 0b994cd (extracted to `.claude/scratch/shared-ref/gate219/`) | `NetMessages.h` |
| 2.20c | client 2.20, `sources/client-2.20/` | `NetMessages.h` |
| 2.20s | game server 2.20, `.claude/scratch/srv220/` | `NetMessages.h` |
| off | official MainServer.exe / WorldServer.exe, per the dispatcher tables in `research/servers/official-mainserver.md` section 2 and `official-worldserver.md` section 2 | (binary) |

## Columns

- **Hex**: the value. All headers and the official tables were compared; **0 value conflicts** were
  found among the 390 names [verified by the generator], meaning no *name* has two values. The
  reverse does not hold: **hex is not a unique key for `MSGID_*`**. All four headers define three
  pairs of different names with one value: `0x0FA314DB` (`MSGID_EVENT_COMMON` and
  `MSGID_MAGICCONFIGURATIONCONTENTS`), `0x0FA314DC` (`MSGID_COMMAND_COMMON` and
  `MSGID_SKILLCONFIGURATIONCONTENTS`), `0x2900AD30` (`MSGID_REQUEST_IPTIME` and
  `MSGID_REQUEST_SELLITEMLIST`). A dispatcher cannot tell the two names of a pair apart on the wire;
  the pairs sit in different link files (client-game vs game-log; `MSGID_REQUEST_IPTIME` in
  `unused.md`). `DEF_*`/`GSM_*` sub-types are 16-bit or 8-bit and are only unique inside their
  parent message.
- **Parent**: for a `DEF_*`/`GSM_*`, the last `MSGID_*` defined above it in the header [inferred from
  header order, not from code], with four prefix overrides because the header order puts them under
  an unrelated message: `DEF_LOGRESMSGTYPE_*` -> `MSGID_RESPONSE_LOG`, `DEF_ENTERGAMEMSGTYPE_*` ->
  `MSGID_REQUEST_ENTERGAME`, `DEF_ENTERGAMERESTYPE_*` -> `MSGID_RESPONSE_ENTERGAME`, `GSM_*` ->
  `MSGID_SERVERSTOCKMSG`. Example: `DEF_NOTIFY_*` are the sub-type word of `MSGID_NOTIFY`,
  `DEF_COMMONTYPE_*` of `MSGID_COMMAND_COMMON`.
- **Ver**: which headers define the name, plus `off` if an official table names it.
- **Client / Game 2.20 / Game 2.03 / Gate 2.19**: `file:line` occurrences in `.cpp` files,
  both `//` and `/* ... */` comments excluded (block comments are blanked line-preserving by
  `strip_comments` in the generator) [verified by text scan]. `case` = a `case NAME:` label (usually the receive
  dispatcher, but also a switch over a sub-type or over the request a reply answers - open the line
  to tell); `ref` = any other use (usually a send or a compare). At most three of each are listed,
  with `(+n)` for the rest.
- **Official**: sender column and table row of the official docs.
- **Docs**: which `research/servers/*.md` mention the name (only that folder: names documented in
  `research/gate/*.md` or `research/shared/formats/*.md` can show `-`, for example
  `MSGID_REQUEST_EXEC_1DOTBAT` and `MSGID_REQUEST_EXEC_2DOTBAT`, see `gate/functions.md` 1.2).
  Byte layouts live there and are not repeated here.

## Counts (measured by the generator, 2026-10-25)

390 names: 139 `MSGID_*`, 251 sub-types (`DEF_*`, `GSM_*`). Per link file:

| File | Rows |
|---|---|
| `client-game.md` | 237 |
| `client-log.md` | 39 |
| `game-log.md` | 41 |
| `game-gate.md` | 17 |
| `game-game.md` (GSM records via gate) | 18 |
| `main-world.md` | 11 |
| `admin-log.md` (official MLS admin/DB, no source user) | 6 |
| `unused.md` | 22 |

Per header: 2.20s 377, 2.20c 374, 2.19g 340, 2.03 334 defines matched.

## How the link was assigned (all inferred)

Rules, applied in order, from where a name is used in source:
1. `GSM_*` -> game-game (records inside `MSGID_SERVERSTOCKMSG`, relayed by the gate).
2. Used in client and game server -> client-game.
3. Used in client only -> client-log.
4. Used in gate source -> game-gate.
5. Only in the official tables, sender WLS/MLS -> main-world; sender admin -> admin-log.
6. Used in game server and named in `game-log-protocol.md` or an official table -> game-log.
7. Used in game server and named in `game-gate-protocol.md` -> game-gate.
8. Used in game server only -> game-log (guess; marked "guess" in the row).
9. Sub-type with no use of its own -> parent's link; anything else -> unused.

So a row's link is a heuristic. Known mis-sorts to expect: a few messages both the client and the
log server use (for example `MSGID_REQUEST_ENTERGAME`, shared by client-log and game-log in 2.20)
appear in one file only; `DEF_MSGTYPE_CONFIRM`/`REJECT` are used on every link but sit in
client-game. The `Docs` and `Official` columns show the other links.

## Caveats and known gaps

- Main <-> World messages exist only in the official binary (no source); their rows have no
  file:line and rely on `official-mainserver.md` section 5.
- The 2.20 Log server source is missing, so the Log side of client-log and game-log has no
  file:line; the official binaries are the only receiver evidence.
- The game server has `#ifdef DEF_DBGAMESERVER` / `#else` duplicates of some functions
  (`srv220/Game.cpp:5549-9477`), so one message can show two `case` hits; only one is compiled.
- `unused.md` means unused in the scanned source trees. `MSGID_RESPONSE_ENTERGAME` 0x0FC94206 and
  `MSGID_RESPONSE_CHANGEPASSWORD` 0x0FC94211 are live on the wire (the official MLS sends them; the client reads only the
  MsgType, `official-mainserver.md` 3.5 and 3.6, `official-compat-220.md`). The `Official` column is filled only from the
  incoming-message tables of the official docs, so outgoing official messages show `-`. Read those docs before assigning an ID.
- `DEF_LOGRESMSGTYPE_INPUTKEYCODEFAILED` (2.20s only) is a client-side result code with no use of its own; it stays in
  `unused.md` rather than inheriting the `MSGID_RESPONSE_LOG` link (its sibling `..._INPUTKEYCODE` is a client-log row).
- Rule 8 exceptions, fixed in the generator: client-link commands and notifies that the 2.20 client never sends or receives
  (`DEF_COMMONTYPE_REQ_TRAINSKILL`, `REQ_GETOCCUPYFLAG`, `REQUEST_HELP`, `BANGUILD`, `DEF_NOTIFY_HELP`, `HELPFAILED`,
  `MSGID_ADMINUSER`, `MSGID_MANAGER_SHUTDOWN`; `srv220/Game.cpp:14966-15088`) are filed under client-game, not game-log.
  `DEF_NOTIFY_SUCCESSBANGUILDMAN`, `CHARISMA`, `CANNOTJOINMOREGUILDSMAN` and `LIMITEDLEVEL` have a client case but the
  server never sends them (row basis says so). `DEF_COMMONTYPE_CLEARGUILDNAME` is sent as `MSGID_EVENT_COMMON` and its
  value 0x0A25 equals `DEF_COMMONTYPE_REQ_GETOCCUPYFIGHTZONETICKET` in the 2.20 server header.
- `MSGID_REQUEST_SAVEARESDENOCCUPYFLAGDATA` / `...ELVINE...` show no live use in 2.20 and 2.03: the
  send `case` and every caller sit in `/* */` blocks (`srv220/Game.cpp:4424-4468`), so those game
  servers never send them. Formerly listed as live because block comments were not stripped.
- Direction per row is not computed. Read it from the name (REQUEST/COMMAND toward the server
  side, RESPONSE/NOTIFY/EVENT back) and confirm at the cited line.

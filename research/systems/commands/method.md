# Method, counts and status rules

How the command catalogue was built, so completeness can be checked and the files regenerated.

## Contents
- Sources
- Enumeration
- Counts
- Columns
- Status rules
- Build switches assumed
- Regenerating
- Unverified

## Sources

| Tag | Path | Note |
|---|---|---|
| S | `sources/hgserver-librehelbreath/src/` | 2.20 game server (reports v2.19). Same line numbers as `.claude/scratch/srv220/` used by `research/game-server/`; the only difference is one `static_cast` at Game.cpp:48960 |
| C | `sources/client-2.20/` | 2.20 client, DEF_LANGUAGE 4 (English). Canonical tree. `sources/client-2.20-helbreathorg/` is a second copy with cp949 comments; line numbers were checked identical by the round-2 reviewer (not re-run here) |
| G | `sources/gateserver-librehelbreath/` | 2.19 gate |
| - | Log server | no 2.20 source; its side of every Log message is unseen |

## Enumeration

`.claude/scratch/commands/enumerate_cmds.py`:
1. Every `#define` in S NetMessages.h + ActionID.h, C NetMessages.h + ActionID.h, G NetMessages.h (union by name).
2. Every whole-word occurrence of each name in every `.cpp`/`.h` of the three trees (headers excluded from rows), with enclosing function, `case` vs other use, `#if` stack, and whether the line is code or inside `//` / `/* */`.
3. `gen.py` splits uses into handler (`case` outside a packer), sender (any other code use) and packing (`case` inside SendNotifyMsg, SendGuildMsg, SendItemNotifyMsg, bSendMsgToLS, SendMsgToGateServer, bSendCommand, SendEventToNearClient_TypeA), evaluates the `#if` stack against the shipped switches, and joins the hand annotations (`annot.py`, `annot_notify.py`, `slash.py`).

Slash commands: every `memcmp(cp, "/...")` in `ChatMsgHandler` S Game.cpp:10745-11046 and every `memcmp(cBuff, "/...")` in client `bCheckLocalChatCommand` C Game.cpp:34000-34121 (`bCheckLocalChatCommand` ends at 34121); the slash block of `ChatMsgHandler` is 10745-11046 inside the whole function 10539-11225; a grep for `"/[a-z]` over C Game.cpp found no other client slash strings except the _DEBUG pre-type (28672), `/to` built for whisper replies (28750-28762, 29032, 29062) and the command-line switch `/egparam` (854, not chat).

## Counts (measured 2026-10-25)

| Family | Rows | File |
|---|---|---|
| MSGID_* | 139 | `msgids.md` |
| DEF_COMMONTYPE_* | 56 (54 in the S header; plus REQ_CREATEPORTION, the client/gate spelling of REQ_CREATEPOTION, and REQUEST_CREATENEWPARTY, defined only in the gate header and named in a commented S case) | `common-types.md` |
| DEF_OBJECT* motion | 17 | `motion.md` |
| DEF_NOTIFY_* | 141 | `notify-types.md` |
| GSM_* | 18 | `gate-relay.md` |
| Other sub-types (MSGTYPE, LOGRES, ENTERGAME, GUILDNOTIFY, DEF_MSGID) | 36 | `other-subtypes.md` |
| Server slash commands | 45 entry strings (+10 cp949 aliases noted in the rows) | `chat-commands.md` |
| Chat prefix modes | 8 | `chat-commands.md` |
| Client-local slash commands | 8 | `chat-commands.md` |

409 defines found; 2 (`DEF_TOTALACTION`, `DEF_TOTALCHARACTERS`) are array sizes, not commands, and are excluded. 407 define rows + 61 chat rows (45 slash + 8 prefix modes + 8 client-local) = 468. `by-system.md` places 432 of them: 371 family rows (the 36 other-subtypes are not placed) + 45 slash + 8 prefix modes + 8 client-local. The system list has 27 systems (round 2 added `envelope` and `gathering`). Each family file ends with its own status tally.

Cross-check with `research/shared/messages/README.md`: 139 MSGID_* there too; its 251 sub-types = my 56 + 141 + 18 + 36 = 251.

## Columns

Name, Value (S header, else first header), Dir (derived; C client, S game, L Log, G gate, X monitor), Sender sites, Handler sites, Packing case, Effect/intent (hand), System and Secondary (hand; list in `by-system.md`), Build gates seen (from `#if` stacks of the sites), Status, Params/overloading/notes (hand), Headers (which headers define it). Sites are capped at 4 with `(+n)`; the full list is in `.claude/scratch/commands/uses.json`.

## Status rules

- `live`: a compiled sender and a compiled receiver exist (for NOTIFY: server send + client `NotifyMsgHandler` case).
- `live (Log end has no 2.20 source)` / `live (other end has no source)` / `live (Log side, no source)`: one end is the Log server, an external monitor tool, or the gate-only link, which have no source here.
- `live (implicit ...)` / `live (alias ...)`: hand override, reason in the cell.
- `DEAD`: hand override after reading the code (reason in notes). `DEAD (only in comments)`: every use is commented.
- `handler only`, `sent, no handler`, `sent, client ignores`, `client handler only (never sent)`, `unused (defined only)`, `not compiled`: as named.

## Build switches assumed

Server (S GlobalDef.h): DEF_CHINESE on, which turns on DEF_CHINESECANDYEVENT, DEF_CHINESEGUILDLIMIT, DEF_NPCITEM, DEF_DBGAMESERVER, DEF_LOCALNPCNAME, DEF_ENGLISHITEMNAME, DEF_LOGTIME, DEF_DEFENCEHACKING; also DEF_V219, DEF_BEGINNERTOWNPROTECT. Off: DEF_TESTSERVER, DEF_TAIWAN (so DEF_TAIWANLOG), DEF_KOREA, DEF_USA, DEF_JAPAN, DEF_INTERSERVER. Client: DEF_LANGUAGE 4, no other switch, release (no _DEBUG). Client gates are shown but only DEF_LANGUAGE is evaluated.

## Regenerating

`python enumerate_cmds.py && python gen.py && python gen_bysys.py` in `.claude/scratch/commands/`. `overloaded.md`, `doc-discrepancies.md`, this file and the index are hand-written.

## Unverified

- Intent/system columns are judgement [I] grounded in the handler names and bodies read; byte layouts were not re-derived.
- Log-server and monitor ends of their messages are unseen (no source).
- Admin levels come from each handler's first check; a handler may have later checks not listed.
- Notify sender lists come from the token scan; a notify built by hand under another name would be missed (4 hand-built ones were found: NOTENOUGHGOLD, MAGICSTUDYSUCCESS/FAIL, SKILLTRAINSUCCESS, ITEMTOBANK).

# shared definitions (constants in more than one program)

Conventions: see `../overlap.md`. Measured by `.claude/scratch/overlap/defs.py` over every `.h`/`.cpp`
in C, H (game server) and G (gate @ 0b994cd), names matching `DEF_*`, `MSGID_*`, `*MSGTYPE*`,
`NOTIFY_*`. MLS/WLS IDs are known only from binaries (`../shared/messages/main-world.md`).

## Counts [V measured]
- Defines: C 737, H 761, G 430. Present in two or more programs: 516; in all three: 363.
- Of the 516, **509 have one value everywhere; 7 differ** (table below).
- NetMessages.h alone: 374 / 377 / 340. No message ID or sub-type has two values; differences are
  only which names exist (shared-code.md) and three renames at the same value.

## Values that differ

| Constant | Client | Game server | Gate | Meaning |
|---|---|---|---|---|
| `DEF_MAXBANKITEMS` | 121 (CLI Game.h:81, "120+1") | 120 (SRV Client.h:26) | - | client keeps one spare display slot [I from comment] |
| `DEF_MAXGUILDSMAN` | 32 (CLI Game.h:82) | 128 (SRV Client.h:27) | - | client list holds fewer members than the server tracks [V values] |
| `DEF_MAXBUILDITEMS` | 100 (CLI Game.h:90) | 300 (SRV Game.h:58) | - | client recipe display list vs server recipe table [V] |
| `DEF_MAXCLIENTS` | - | 2000 (SRV Game.h:53) | 500 (GATE GateCore.h:26) | different objects: players vs gate peers [V] |
| `DEF_TOTALACTION` | 15 (CLI ActionID.h:2) | 10 (SRV ActionID.h:2) | 10 (GATE ActionID.h:2) | client has extra local animation states [I] |
| `DEF_LOWERVERSION` | 19/19/20 (CLI GlobalDef.h:11,14,17, per build) | "20" (SRV Version.h:6) | - | client builds sent version words; server compares [I]; see `../issues/setup-2.20.md#setup-006-mainserver-login-confirm-version-words-byte-patch` |
| `DEF_UPPERVERSION` | 2 (CLI GlobalDef.h:10) | "2" (SRV Version.h:2) | - | same value, int vs string literal [V] |

## Same value, different name [V netmsg.py]
- `0x0A30`: C/H `DEF_COMMONTYPE_REQUEST_ACCEPTJOINPARTY` (CLI NetMessages.h:62, SRV :62) vs G
  `DEF_COMMONTYPE_REQUEST_CREATENEWPARTY` (GATE NetMessages.h:61). The gate never receives COMMON
  messages, so this is a stale name only [I].
- `0x0A19`: `REQ_CREATEPORTION` (C, G) vs `REQ_CREATEPOTION` (H, SRV NetMessages.h:44).
- `0x0BA5`: `DEF_NOTIFY_GIZONEITEMCHANGE` (CLI :219) vs `DEF_NOTIFY_GIZONITEMCANGE` (SRV :221).

## Shared value spaces that overlap by design
MsgType and sub-type values are reused across namespaces (0x0F14 is both `DEF_MSGTYPE_CONFIRM` and
`DEF_LOGRESMSGTYPE_CONFIRM`; 0x0A01.. are both COMMON sub-types and log result codes). One MsgID
pair is shared across families: `MSGID_EVENT_COMMON` = `MSGID_MAGICCONFIGURATIONCONTENTS` =
0x0FA314DB in all three (CLI NetMessages.h:17, 235) - see
`../issues/network.md#net-003-envelope-id-collision-with-config-pushes`.

## Limits that agree [V]
`DEF_MAXITEMS` 50, `DEF_MAXMAGICTYPE` 100, `DEF_MAXSKILLTYPE` 60, `DEF_MAXPARTYMEMBERS` 8 (C, H);
`DEF_MSGBUFFERSIZE` 30000 (H, G); `DEF_XSOCKBLOCKLIMIT` 300 (all three, XSocket.h:51/50).
Server-only limits: `DEF_MAXNPCS` 5000, `DEF_MAXMAPS` 100, `DEF_MAXITEMTYPES` 5000,
`DEF_MAXNPCTYPES` 100, `DEF_MAXGUILDS` 1000 (SRV Game.h:54-57, 154); gate `DEF_MAXGAMESERVERS` 20.
View window 21x16 tiles is a protocol fact of the init snapshot [V `../systems/entity.md`:28], not a
shared named constant [I: no common define found by this scan].
Log-server limits (binaries): MLS 4 worlds, 10 000 clients/accounts; WLS 9 999 sockets, 5 000 account
states, 10 game-server allow entries [V official-mainserver.md, official-worldserver.md].

## Doc discrepancies
None found in this pass.

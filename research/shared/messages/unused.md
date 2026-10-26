# Messages: Defined but unused

Generated from the NetMessages.h files by `.claude/scratch/shared-ref/gen_messages.py`; conventions in `README.md`. Sender -> receiver: - -> - (and reverse for responses; per-row direction is in the name: REQUEST/COMMAND = toward the server side, RESPONSE/NOTIFY/EVENT = back). 
Byte layouts: not in research/servers; read the handlers cited below.

Note: 'unused' means no use in the source trees scanned. Some rows are still live on the wire: the official MLS/WLS binaries send `MSGID_RESPONSE_ENTERGAME` and `MSGID_RESPONSE_CHANGEPASSWORD` (the client reads only the MsgType; see `../../servers/official-mainserver.md` sections 3.5, 3.6 and `../../servers/official-compat-220.md`). The Official column covers only rows of the official docs' incoming-message tables, so outgoing official messages show `-` here. Check those docs before assigning a new ID.

| Hex | Name | Parent | Ver | Link basis (inferred) | Client 2.20 | Game 2.20 | Game 2.03 | Gate 2.19 | Official | Docs |
|---|---|---|---|---|---|---|---|---|---|---|
| 0x0512A3F7 | `MSGID_RESPONSE_REGISTERDBSERVER` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | - |
| 0x0FC94206 | `MSGID_RESPONSE_ENTERGAME` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | client-log-protocol.md |
| 0x0FC94211 | `MSGID_RESPONSE_CHANGEPASSWORD` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | official-mainserver.md |
| 0x0FC94213 | `MSGID_RESPONSE_INPUTKEYCODE` | - | 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | - |
| 0x13000002 | `MSGID_IPINFOCHANGE` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | game-log-protocol.md, world-log-server.md |
| 0x14000010 | `MSGID_ANNOUNCEACCOUNTNEWPASSWORD` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | game-log-protocol.md, world-log-server.md |
| 0x14E91200 | `MSGID_REQUEST_IPIDSTATUS` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | - |
| 0x14E91201 | `MSGID_RESPONSE_IPIDSTATUS` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | - |
| 0x14E91203 | `MSGID_RESPONSE_ACCOUNTCONNSTATUS` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | - |
| 0x14E91205 | `MSGID_RESPONSE_CLEARACCOUNTCONNSTATUS` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | - |
| 0x15000001 | `MSGID_REQUEST_NOCOUNTINGSAVELOGOUT` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | game-log-protocol.md, world-log-server.md |
| 0x167C0A30 | `MSGID_OCCUPYFLAGDATA` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | - |
| 0x167C0A31 | `MSGID_REQUEST_SAVEARESDENOCCUPYFLAGDATA` | - | 2.03 2.19g 2.20c 2.20s off | defined but no use found in any source | - | - | - | - | game server (not the 2.20 one: it never sends these, `SRV Game.cpp:3762-3764, 4424-4468, 47997`) (`official-worldserver.md:85`) | - |
| 0x167C0A32 | `MSGID_REQUEST_SAVEELVINEOCCUPYFLAGDATA` | - | 2.03 2.19g 2.20c 2.20s off | defined but no use found in any source | - | - | - | - | game server (not the 2.20 one: it never sends these, `SRV Game.cpp:3762-3764, 4424-4468, 47997`) (`official-worldserver.md:85`) | - |
| 0x17081034 | `MSGID_ARESDENOCCUPYFLAGSAVEFILECONTENTS` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | - |
| 0x17081035 | `MSGID_ELVINEOCCUPYFLAGSAVEFILECONTENTS` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | - |
| 0x2900AD20 | `MSGID_IPTIMECHANGE` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | game-log-protocol.md |
| 0x2900AD30 | `MSGID_REQUEST_IPTIME` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | - |
| 0x2900AD31 | `MSGID_RESPONSE_IPTIME` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | - |
| 0x3AE90000 | `MSGID_COLLECTEDMANA` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | - |
| 0x3AE90001 | `MSGID_METEORSTRIKE` | - | 2.03 2.19g 2.20c 2.20s | defined but no use found in any source | - | - | - | - | - | - |
| 0x0A03 | `DEF_LOGRESMSGTYPE_INPUTKEYCODEFAILED` | `MSGID_RESPONSE_LOG` | 2.20s | defined but no use found; client-side result code, not a parent-link message | - | - | - | - | - | - |
